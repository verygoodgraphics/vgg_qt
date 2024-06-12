#include "QVggEventAdapter.hpp"
#include "QVggQuickItem.h"
#include <QGuiApplication>

#ifdef VGG_USE_QT_6
#define EVENT_POS position
#else
#define EVENT_POS pos
#endif // VGG_USE_QT_6

std::vector<QThread*> QVggQuickItem::m_threads;

QVggRenderThread::QVggRenderThread(
  const QSize&        size,
  TVggQuickContainer& container,
  std::mutex&         lock)
  : m_surface(nullptr)
  , m_context(nullptr)
  , m_renderFbo(nullptr)
  , m_size(size)
  , m_dpi{ 1.0 }
  , m_container(container)
  , m_lock(lock)
  , m_needResetContainer{ false }
  , m_needStopped{ false }
{
}

void QVggRenderThread::InitOffScreenSurface()
{
  assert(!m_surface && m_context);
  m_surface = new QOffscreenSurface();
  m_surface->setFormat(m_context->format());
  m_surface->create();
}

void QVggRenderThread::InitOpenGLContext(QOpenGLContext* sharedContext)
{
  assert(!m_context);
  m_context = new QOpenGLContext();
  m_context->setFormat(sharedContext->format());
  m_context->setShareContext(sharedContext);
  m_context->create();
}

auto QVggRenderThread::getOpenGLContext()
{
  return m_context;
}

void QVggRenderThread::setFileSource(QString str)
{
  std::lock_guard<std::mutex> lock(m_lock);
  m_fileSource = str;
  m_needResetContainer = true;
}

void QVggRenderThread::sizeChanged(QSize size)
{
  if (size == m_size || !size.width() || !size.height())
  {
    return;
  }

  std::lock_guard<std::mutex> lock(m_lock);
  m_size = size;
  m_needResetContainer = true;
}

void QVggRenderThread::renderNext()
{
  if (!m_needStopped)
  {
    std::lock_guard<std::mutex> lock(m_lock);
    m_context->makeCurrent(m_surface);

    if (!m_renderFbo || m_needResetContainer)
    {
      QOpenGLFramebufferObjectFormat format;
      format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
      delete m_renderFbo;
      m_renderFbo = new QOpenGLFramebufferObject(m_size, format);

      m_container.reset(
        new VGG::QtQuickContainer(m_size.width(), m_size.height(), m_dpi, m_renderFbo->handle()));
      m_container->load(m_fileSource.toStdString());

      m_needResetContainer = false;
    }

    m_renderFbo->bind();

    // for transparence
    // m_context->functions()->glViewport(0, 0, m_size.width(), m_size.height());
    // m_context->functions()->glEnable(GL_BLEND);
    // m_context->functions()->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // m_context->functions()->glClearColor(0, 0, 0, 0);
    // m_context->functions()->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_container->paint();

    // We need to flush the contents to the FBO before posting
    // the texture to the other thread, otherwise, we might
    // get unexpected results.
    m_context->functions()->glFlush();

    m_renderFbo->bindDefault();
    emit textureReady(m_renderFbo->toImage(false));
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(16));
}

void QVggRenderThread::shutDown()
{
  std::lock_guard<std::mutex> lock(m_lock);
  m_needStopped = true;
  m_container.reset(nullptr);

  m_context->makeCurrent(m_surface);
  delete m_renderFbo;
  m_context->doneCurrent();
  delete m_context;

  // schedule this to be deleted only after we're done cleaning up
  m_surface->deleteLater();

  // Stop event processing, move the thread to GUI and make sure it is deleted.
  exit();
  moveToThread(QGuiApplication::instance()->thread());
}

QVggTextureNode::QVggTextureNode(QQuickWindow* window)
  : m_texture(nullptr)
  , m_window(window)
{
  // Our texture node must have a texture
  QImage img(1, 1, QImage::Format_ARGB32);
  m_texture = m_window->createTextureFromImage(img);
  setTexture(m_texture);
  setFiltering(QSGTexture::Linear);
}

QVggTextureNode ::~QVggTextureNode()
{
  delete m_texture;
}

void QVggTextureNode::newTexture(QImage image)
{
  std::lock_guard<std::mutex> lock(m_lock);
  m_image = image;

  // We cannot call QQuickWindow::update directly here, as this is only allowed
  // from the rendering thread or GUI thread.
  emit pendingNewTexture();
}

void QVggTextureNode::prepareNode()
{
  std::lock_guard<std::mutex> lock(m_lock);

  if (!m_image.isNull())
  {
    delete m_texture;
    m_texture = m_window->createTextureFromImage(m_image, QQuickWindow::TextureHasAlphaChannel);
    markDirty(DirtyMaterial);
    this->setTexture(m_texture);

    // This will notify the rendering thread that the texture is now being rendered
    // and it can start rendering to the other one.
    emit textureInUse();
  }
}

QVggQuickItem::QVggQuickItem(QQuickItem* parent)
  : QQuickItem(parent)
{
  // By default, QQuickItem does not draw anything. If you subclass
  // QQuickItem to create a visual item, you will need to uncomment the
  // following line and re-implement updatePaintNode()
  setFlag(ItemHasContents, true);

  m_renderThread = new QVggRenderThread(QSize(1, 1), m_container, m_lock);
  QVggQuickItem::m_threads.emplace_back(m_renderThread);

  // TODO should m_renderThread call exec()?
  QObject::connect(
    this,
    &QVggQuickItem::fileSourceChanged,
    m_renderThread,
    &QVggRenderThread::setFileSource,
    Qt::QueuedConnection);

  auto emitSizeChange = [this]() { emit sizeChanged(QSize(width(), height())); };

  QObject::connect(this, &QVggQuickItem::widthChanged, emitSizeChange);
  QObject::connect(this, &QVggQuickItem::heightChanged, emitSizeChange);
  QObject::connect(
    this,
    &QVggQuickItem::sizeChanged,
    m_renderThread,
    &QVggRenderThread::sizeChanged,
    Qt::QueuedConnection);

  this->setAcceptedMouseButtons(Qt::MouseButton::AllButtons);

  m_dispatchTimer.setInterval(16);
  QObject::connect(
    &m_dispatchTimer,
    &QTimer::timeout,
    this,
    [this]()
    {
      std::lock_guard<std::mutex> lock(m_lock);

      if (!m_container)
      {
        return;
      }
      m_container->dispatch(); // todo, improve dispatch
    });
  m_dispatchTimer.start();
}

QVggQuickItem::~QVggQuickItem()
{
  assert(!m_container);
}

void QVggQuickItem::ready()
{
  m_renderThread->InitOffScreenSurface();
  m_renderThread->moveToThread(m_renderThread);

  // connect(
  //   window(),
  //   &QQuickWindow::sceneGraphInvalidated,
  //   m_renderThread,
  //   &QVggRenderThread::shutDown,
  //   Qt::QueuedConnection);
  connect(
    window(),
    &QQuickWindow::closing,
    m_renderThread,
    &QVggRenderThread::shutDown,
    Qt::QueuedConnection);

  m_renderThread->start();
  update();
}

QString QVggQuickItem::fileSource() const
{
  return m_fileSource;
}

void QVggQuickItem::setFileSource(const QString& src)
{
  if (src == m_fileSource)
  {
    return;
  }

  m_fileSource = src;
  emit fileSourceChanged(m_fileSource);
}

void QVggQuickItem::setEventListener(QVggQuickItem::EventListener listener)
{
  std::lock_guard<std::mutex> lock(m_lock);

  if (listener)
  {
    auto sdk = m_container->sdk();
    m_container->setEventListener(
      [listener, sdk](std::string type, std::string targetId, std::string targetPath)
      { listener(sdk, type, targetId, targetPath); });
  }
  else
  {
    m_container->setEventListener(nullptr);
  }
}

void QVggQuickItem::fillVggEvent(UEvent& vggEvent, QMouseEvent* mouseEvent)
{
  switch (mouseEvent->button())
  {
    case Qt::LeftButton:
      // jsButtonIndex + 1
      // https://developer.mozilla.org/en-US/docs/Web/API/MouseEvent/button#value
      vggEvent.button.button = 1;
      break;
    case Qt::MiddleButton:
      vggEvent.button.button = 2;
      break;
    case Qt::RightButton:
      vggEvent.button.button = 3;
      break;
    default:
      break;
  }

  auto p = mouseEvent->EVENT_POS();
  vggEvent.button.windowX = p.x();
  vggEvent.button.windowY = p.y();
}

void QVggQuickItem::keyPressEvent(QKeyEvent* event)
{
  std::lock_guard<std::mutex> lock(m_lock);
  auto                        vggEvent = QVggEventAdapter::keyPressEvent(event);
  m_container->onEvent(vggEvent);
}

void QVggQuickItem::keyReleaseEvent(QKeyEvent* event)
{
  std::lock_guard<std::mutex> lock(m_lock);
  auto                        vggEvent = QVggEventAdapter::keyReleaseEvent(event);
  m_container->onEvent(vggEvent);
}

void QVggQuickItem::mousePressEvent(QMouseEvent* event)
{
  std::lock_guard<std::mutex> lock(m_lock);
  UEvent                      evt;
  evt.button.type = VGG_MOUSEBUTTONDOWN;
  fillVggEvent(evt, event);

  m_container->onEvent(evt);
}

void QVggQuickItem::mouseMoveEvent(QMouseEvent* event)
{
  std::lock_guard<std::mutex> lock(m_lock);
  UEvent                      evt;
  evt.motion.type = VGG_MOUSEMOTION;
  evt.motion.windowX = event->EVENT_POS().x();
  evt.motion.windowY = event->EVENT_POS().y();

  auto delta = event->EVENT_POS() - m_lastMouseMovePosition;
  evt.motion.xrel = delta.x();
  evt.motion.yrel = delta.y();

  m_container->onEvent(evt);

  m_lastMouseMovePosition = event->EVENT_POS();
}

void QVggQuickItem::mouseReleaseEvent(QMouseEvent* event)
{
  std::lock_guard<std::mutex> lock(m_lock);
  UEvent                      evt;
  evt.button.type = VGG_MOUSEBUTTONUP;
  fillVggEvent(evt, event);

  m_container->onEvent(evt);
}

void QVggQuickItem::wheelEvent(QWheelEvent* event)
{
  std::lock_guard<std::mutex> lock(m_lock);
  UEvent                      evt;
  evt.wheel.type = VGG_MOUSEWHEEL;

  auto p = event->EVENT_POS();
  evt.wheel.mouseX = p.x();
  evt.wheel.mouseY = p.y();

  auto delta = event->pixelDelta();
  evt.wheel.x = delta.x();
  evt.wheel.y = delta.y();
  evt.wheel.preciseX = delta.x();
  evt.wheel.preciseY = delta.y();

  m_container->onEvent(evt);
}

QSGNode* QVggQuickItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* updatePaintNodeData)
{
  std::lock_guard<std::mutex> lock(m_lock);

  QVggTextureNode* node = static_cast<QVggTextureNode*>(oldNode);

  if (!m_renderThread->getOpenGLContext())
  {
    // QOpenGLContext* current = window()->openglContext();
    QOpenGLContext* current = QOpenGLContext::currentContext();

    // Some GL implementations requres that the currently bound m_context is
    // made non-current before we set up sharing, so we doneCurrent here
    // and makeCurrent down below while setting up our own m_context.
    current->doneCurrent();

    m_renderThread->InitOpenGLContext(current);
    m_renderThread->getOpenGLContext()->moveToThread(m_renderThread);

    current->makeCurrent(window());

    QMetaObject::invokeMethod(this, "ready");
    return nullptr;
  }

  if (!node)
  {
    node = new QVggTextureNode(window());

    /* Set up connections to get the production of FBO textures in sync with vsync on the
     * rendering thread.
     *
     * When a new texture is ready on the rendering thread, we use a direct connection to
     * the texture node to let it know a new texture can be used. The node will then
     * emit pendingNewTexture which we bind to QQuickWindow::update to schedule a redraw.
     *
     * When the scene graph starts rendering the next frame, the prepareNode() function
     * is used to update the node with the new texture. Once it completes, it emits
     * textureInUse() which we connect to the FBO rendering thread's renderNext() to have
     * it start producing content into its current "back buffer".
     *
     * This FBO rendering pipeline is throttled by vsync on the scene graph rendering thread.
     */
    connect(
      m_renderThread,
      &QVggRenderThread::textureReady,
      node,
      &QVggTextureNode::newTexture,
      Qt::DirectConnection);
    connect(
      node,
      &QVggTextureNode::pendingNewTexture,
      window(),
      &QQuickWindow::update,
      Qt::QueuedConnection);
    connect(
      window(),
      &QQuickWindow::beforeRendering,
      node,
      &QVggTextureNode::prepareNode,
      Qt::DirectConnection);
    connect(
      node,
      &QVggTextureNode::textureInUse,
      m_renderThread,
      &QVggRenderThread::renderNext,
      Qt::QueuedConnection);

    // Get the production of FBO textures started..
    QMetaObject::invokeMethod(m_renderThread, "renderNext", Qt::QueuedConnection);
  }

  node->setRect(boundingRect());
  return node;
}

// TODO maybe delete at ~QVggQuickItem ?
void QVggQuickItem::tearDown()
{
  // As the render threads make use of our QGuiApplication object
  // to clean up gracefully, wait for them to finish before
  // QGuiApp is taken off the heap.
  for (auto item : m_threads)
  {
    item->wait();
    delete item;
  }
}
