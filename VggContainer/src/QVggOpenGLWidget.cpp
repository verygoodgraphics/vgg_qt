/*
 * Copyright 2023 VeryGoodGraphics LTD <bd@verygoodgraphics.com>
 *
 * Licensed under the VGG License, Version 1.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://www.verygoodgraphics.com/licenses/LICENSE-1.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "VggContainer/QVggOpenGLWidget.hpp"
#include "VggContainer/QVggEventAdapter.hpp"

#include "VGG/QtContainer.hpp"

#include <QMouseEvent>
#include <QOpenGLFunctions>
#include <QTimer>
#include <QWheelEvent>
#include <QWindow>

// ======================================================================
// QVggOpenGLWidgetImpl
// ======================================================================
class QVggOpenGLWidgetImpl
{
  friend QVggOpenGLWidget;

  QVggOpenGLWidget*                 m_api;
  std::unique_ptr<VGG::QtContainer> m_container;

  QOpenGLFunctions m_funcs;

  QTimer  m_animator;
  QPointF m_lastMouseMovePosition;

public:
  QVggOpenGLWidgetImpl(QVggOpenGLWidget* api)
    : m_api{ api }
    , m_container{ new VGG::QtContainer }
  {
    m_animator.setInterval(16);
  }

  // === GL ============================================================
  void initializeGL()
  {
    m_funcs.initializeOpenGLFunctions();
  }

  void resizeGL(int w, int h)
  {
    auto scale = m_api->windowHandle()->devicePixelRatio();

    UEvent evt;
    evt.window.type = VGG_WINDOWEVENT;
    evt.window.event = VGG_WINDOWEVENT_SIZE_CHANGED;
    evt.window.data1 = w;
    evt.window.data2 = h;
    evt.window.drawableWidth = w * scale;
    evt.window.drawableHeight = h * scale;

    m_container->onEvent(evt);
  }

  void paintGL()
  {
    m_container->paint(true);
  }

  // =================================================================
  void init(int w, int h)
  {
    m_funcs.glViewport(0, 0, w, h);
    m_container->init(w, h, m_api->windowHandle()->devicePixelRatio());
  }

  // === api =========================================================
  bool load(
    const std::string& filePath,
    const char*        designDocSchemaFilePath = nullptr,
    const char*        layoutDocSchemaFilePath = nullptr)
  {
    return m_container->load(filePath, designDocSchemaFilePath, layoutDocSchemaFilePath);
  }

  void setEventListener(QVggOpenGLWidget::EventListener listener)
  {
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

  // === events =====================================================
  void mousePressEvent(QMouseEvent* event)
  {
    UEvent evt;
    evt.button.type = VGG_MOUSEBUTTONDOWN;
    fillVggEvent(evt, event);

    m_container->onEvent(evt);
  }

  void mouseMoveEvent(QMouseEvent* event)
  {
    UEvent evt;
    evt.motion.type = VGG_MOUSEMOTION;
    evt.motion.windowX = event->position().x();
    evt.motion.windowY = event->position().y();

    auto delta = event->position() - m_lastMouseMovePosition;
    evt.motion.xrel = delta.x();
    evt.motion.yrel = delta.y();

    m_container->onEvent(evt);

    m_lastMouseMovePosition = event->position();
  }

  void mouseReleaseEvent(QMouseEvent* event)
  {
    UEvent evt;
    evt.button.type = VGG_MOUSEBUTTONUP;
    fillVggEvent(evt, event);

    m_container->onEvent(evt);
  }

  void wheelEvent(QWheelEvent* event)
  {
    UEvent evt;
    evt.wheel.type = VGG_MOUSEWHEEL;

    auto p = event->position();
    evt.wheel.mouseX = p.x();
    evt.wheel.mouseY = p.y();

    auto delta = event->pixelDelta();
    evt.wheel.x = delta.x();
    evt.wheel.y = delta.y();
    evt.wheel.preciseX = delta.x();
    evt.wheel.preciseY = delta.y();

    m_container->onEvent(evt);
  }

  void keyPressEvent(QKeyEvent* event)
  {
    auto vggEvent = QVggEventAdapter::keyPressEvent(event);
    m_container->onEvent(vggEvent);
  }

  void keyReleaseEvent(QKeyEvent* event)
  {
    auto vggEvent = QVggEventAdapter::keyReleaseEvent(event);
    m_container->onEvent(vggEvent);
  }

  // === Private ===========================================================
private:
  void fillVggEvent(UEvent& vggEvent, QMouseEvent* mouseEvent)
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

    auto p = mouseEvent->position();
    vggEvent.button.windowX = p.x();
    vggEvent.button.windowY = p.y();
  }
};

// ======================================================================
// QVggOpenGLWidget
// ======================================================================
QVggOpenGLWidget::QVggOpenGLWidget(QWidget* parent)
  : QOpenGLWidget(parent)
  , m_impl(new QVggOpenGLWidgetImpl(this))
{
  QObject::connect(
    &m_impl->m_animator,
    &QTimer::timeout,
    this,
    [this]()
    {
      if (this->m_impl->m_container->needsPaint())
      {
        this->update();
      }
      this->m_impl->m_container->dispatch(); // todo, improve dispatch
    });
  m_impl->m_animator.start();

  setMouseTracking(true);
}

QVggOpenGLWidget::~QVggOpenGLWidget()
{
  m_impl->m_animator.stop();
  delete m_impl;
}

// === api ===============================================================
bool QVggOpenGLWidget::load(
  const std::string& filePath,
  const char*        designDocSchemaFilePath,
  const char*        layoutDocSchemaFilePath)
{
  return m_impl->load(filePath, designDocSchemaFilePath, layoutDocSchemaFilePath);
}

void QVggOpenGLWidget::setEventListener(EventListener listener)
{
  m_impl->setEventListener(listener);
}

// === GL ===============================================================
void QVggOpenGLWidget::initializeGL()
{
  m_impl->initializeGL();
  init(this->width(), this->height());

  context()->setShareContext(QOpenGLContext::globalShareContext());
}

void QVggOpenGLWidget::resizeGL(int w, int h)
{
  init(w, h);

  m_impl->resizeGL(w, h);
}

void QVggOpenGLWidget::paintGL()
{
  if (!this->isVisible())
  {
    return;
  }

  m_impl->paintGL();
}

// === events ===============================================================
void QVggOpenGLWidget::mousePressEvent(QMouseEvent* event)
{
  m_impl->mousePressEvent(event);
}
void QVggOpenGLWidget::mouseReleaseEvent(QMouseEvent* event)
{
  m_impl->mouseReleaseEvent(event);
}

void QVggOpenGLWidget::mouseMoveEvent(QMouseEvent* event)
{
  m_impl->mouseMoveEvent(event);
}

void QVggOpenGLWidget::wheelEvent(QWheelEvent* event)
{
  m_impl->wheelEvent(event);
}

void QVggOpenGLWidget::keyPressEvent(QKeyEvent* event)
{
  m_impl->keyPressEvent(event);
}
void QVggOpenGLWidget::keyReleaseEvent(QKeyEvent* event)
{
  m_impl->keyReleaseEvent(event);
}

// === private ===============================================================
void QVggOpenGLWidget::init(int w, int h)
{
  m_impl->init(w, h);
}
