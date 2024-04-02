#ifndef VGGQUICKCONTAINER_H
#define VGGQUICKCONTAINER_H

#include <QtQuick/QQuickPaintedItem>

class VggQuickContainer : public QQuickPaintedItem
{
    Q_OBJECT
    QML_ELEMENT
    Q_DISABLE_COPY(VggQuickContainer)
public:
    explicit VggQuickContainer(QQuickItem *parent = nullptr);
    void paint(QPainter *painter) override;
    ~VggQuickContainer() override;
};

#endif // VGGQUICKCONTAINER_H
