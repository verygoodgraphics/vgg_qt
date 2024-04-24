#ifndef VGGQUICKCONTAINER_H
#define VGGQUICKCONTAINER_H

#include <QtQuick/QQuickItem>
#include <QtQuick/QSGNode>

class VggQuickContainer : public QQuickItem
{
  Q_OBJECT
  Q_PROPERTY(QString fileSource READ fileSource WRITE setFileSource NOTIFY fileSourceChanged)
  QML_ELEMENT
  Q_DISABLE_COPY(VggQuickContainer)
public:
  explicit VggQuickContainer(QQuickItem* parent = nullptr);
  ~VggQuickContainer() override;

  inline QString fileSource() const
  {
    return m_fileSource;
  }

  inline void setFileSource(const QString& src)
  {
    if (src == m_fileSource)
    {
      return;
    }
    m_fileSource = src;
    emit fileSourceChanged();
  }

signals:
  void fileSourceChanged();

protected:
  virtual QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* updatePaintNodeData)
    override;

  virtual void mouseMoveEvent(QMouseEvent* event) override {}
  virtual void mousePressEvent(QMouseEvent* event) override {}
  virtual void mouseReleaseEvent(QMouseEvent* event) override {}
  virtual void mouseDoubleClickEvent(QMouseEvent* event) override {}
#if 0
  virtual void hoverEnterEvent(QHoverEvent* event) override;
  virtual void hoverLeaveEvent(QHoverEvent* event) override;
  virtual void hoverMoveEvent(QHoverEvent* event) override;
  virtual void keyPressEvent(QKeyEvent* event) override;
  virtual void keyReleaseEvent(QKeyEvent* event) override;
  virtual void wheelEvent(QWheelEvent* event) override;
  virtual void touchEvent(QTouchEvent* event) override;
  virtual void inputMethodEvent(QInputMethodEvent* event) override;
  virtual void focusInEvent(QFocusEvent* event) override;
  virtual void focusOutEvent(QFocusEvent* event) override;
  virtual void dragEnterEvent(QDragEnterEvent* event) override;
  virtual void dragLeaveEvent(QDragLeaveEvent* event) override;
  virtual void dragMoveEvent(QDragMoveEvent* event) override;
#endif

private:
  QString m_fileSource;
};

#endif // VGGQUICKCONTAINER_H
