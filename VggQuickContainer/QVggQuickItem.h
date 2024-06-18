#pragma once
#include <mutex>
#include <memory>
#include <vector>
#include <QTimer>
#include <QThread>
#include <QQuickItem>
#include <QQuickWindow>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOffscreenSurface>
#include <QSGSimpleTextureNode>
#include <QOpenGLFramebufferObject>
#include "VGG/QtQuickContainer.hpp"

typedef std::unique_ptr<VGG::QtQuickContainer> TVggQuickContainer;

class QVggRenderThread : public QThread
{
  Q_OBJECT

public:
  QVggRenderThread(TVggQuickContainer& container, std::mutex& lock, QObject* creator);

public:
  void InitOffScreenSurface();
  void InitOpenGLContext(QOpenGLContext* sharedContext);
  auto getOpenGLContext();

  void                      setFbo(QOpenGLFramebufferObject* fbo);
  QOpenGLFramebufferObject* getFbo();

public slots:
  void setFileSource(QString str);
  void sizeChanged(QSize size);
  void renderNext();
  void shutDown();

signals:
  void textureReady(QImage image);

private:
  QOffscreenSurface*        m_surface;
  QOpenGLContext*           m_context;
  QOpenGLFramebufferObject* m_renderFbo;
  QString                   m_fileSource;
  QSize                     m_size;
  double                    m_dpi;
  TVggQuickContainer&       m_container;
  std::mutex&               m_lock;
  bool                      m_needResetContainer;
  bool                      m_sizeChanged;
  bool                      m_needStopped;
  QObject*                  m_creator;
};

class QVggTextureNode
  : public QObject
  , public QSGSimpleTextureNode
{
  Q_OBJECT

public:
  QVggTextureNode(QQuickWindow* window);
  ~QVggTextureNode() override;

signals:
  void textureInUse();
  void pendingNewTexture();

public slots:
  // This function gets called on the FBO rendering thread and will store the
  // texture id and size and schedule an update on the window.
  void newTexture(QImage);

  // Before the scene graph starts to render, we update to the pending texture
  void prepareNode();

private:
  QImage        m_image;
  std::mutex    m_lock;
  QSGTexture*   m_texture;
  QQuickWindow* m_window;
};

class QVggQuickItem : public QQuickItem
{
  Q_OBJECT
  Q_PROPERTY(QString fileSource READ fileSource WRITE setFileSource NOTIFY fileSourceChanged)

public:
  explicit QVggQuickItem(QQuickItem* parent = nullptr);
  ~QVggQuickItem() override;

public:
  using EventListener = std::function<void(
    std::shared_ptr<VGG::ISdk> vggSdk,
    std::string                type,
    std::string                targetId,
    std::string                targetPath)>;

public:
  QString fileSource() const;
  void    setFileSource(const QString& src);
  void    setEventListener(EventListener listener);
  void    fillVggEvent(UEvent& vggEvent, QMouseEvent* mouseEvent);

signals:
  void fileSourceChanged(QString newFileSource);
  void sizeChanged(QSize size);

public Q_SLOTS:
  void ready();

protected:
  virtual QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*) override;
  virtual void     mouseMoveEvent(QMouseEvent* event) override;
  void             hoverMoveEvent(QHoverEvent* event) override;
  virtual void     mouseReleaseEvent(QMouseEvent* event) override;
  virtual void     keyPressEvent(QKeyEvent* event) override;
  virtual void     keyReleaseEvent(QKeyEvent* event) override;
  virtual void     mousePressEvent(QMouseEvent* event) override;
  virtual void     wheelEvent(QWheelEvent* event) override;

private:
  QString            m_fileSource;
  TVggQuickContainer m_container;
  std::mutex         m_lock;
  QTimer             m_dispatchTimer;
  QPointF            m_lastMouseMovePosition;
  QVggRenderThread*  m_renderThread;
};
