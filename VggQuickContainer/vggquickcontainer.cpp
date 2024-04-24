#include "vggquickcontainer.h"

#include <QSGSimpleTextureNode>
#include <QSGDynamicTexture>

class VggTexture : public QSGDynamicTexture
{
public:
  virtual bool updateTexture() override
  {
    return false;
  }
  virtual qint64 comparisonKey() const override
  {
    return 0;
  }
  virtual QSize textureSize() const override
  {
    return QSize(1, 1);
  }
  virtual bool hasMipmaps() const override
  {
    return false;
  }
  virtual bool hasAlphaChannel() const override
  {
    return true;
  }
};

VggQuickContainer::VggQuickContainer(QQuickItem* parent)
  : QQuickItem(parent)
{
  // By default, QQuickItem does not draw anything. If you subclass
  // QQuickItem to create a visual item, you will need to uncomment the
  // following line and re-implement updatePaintNode()
  setFlag(ItemHasContents, true);

  printf("fileSource: %s\n", m_fileSource.toStdString().c_str());
}

VggQuickContainer::~VggQuickContainer()
{
}

QSGNode* VggQuickContainer::updatePaintNode(
  QSGNode*             oldNode,
  UpdatePaintNodeData* updatePaintNodeData)
{
  QSGSimpleTextureNode* n = static_cast<QSGSimpleTextureNode*>(oldNode);
  if (!n)
  {
    n = new QSGSimpleTextureNode();
    auto t = new VggTexture();
    n->setTexture(t);
  }
  n->setRect(boundingRect());
  return n;
}
