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

  // qt6 need this
  // QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);

  QObject::connect(&app, &QGuiApplication::aboutToQuit, [&]() { VGG::Environment::tearDown(); });

  int returnValue = 0;

  {

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

    returnValue = app.exec();
  }

  QVggQuickItem::tearDown();
  return returnValue;
}
