#include "VGG/Environment.hpp"
#include "QVggEventAdapter.hpp"
#include "QVggQuickItem.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>

int main(int argc, char* argv[])
{
  VGG::Environment::setUp();
  QVggEventAdapter::setup();

  QGuiApplication app(argc, argv);
  qmlRegisterType<QVggQuickItem>("QVggQuickItem", 1, 0, "QVggQuickItem");

#ifdef VGG_USE_QT_6
  QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);
#endif // VGG_USE_QT_6

  QObject::connect(&app, &QGuiApplication::aboutToQuit, [&]() { VGG::Environment::tearDown(); });

  QQmlApplicationEngine engine;
  const QUrl            url("qrc:/main.qml");
  QObject::connect(
    &engine,
    &QQmlApplicationEngine::objectCreated,
    &app,
    [url](QObject* obj, const QUrl& objUrl)
    {
      if (!obj && url == objUrl)
        QCoreApplication::exit(-1);
    },
    Qt::QueuedConnection);
  engine.load(url);

  return app.exec();
}
