// #include "mainwindow.h"
#include "Counter.h"

#include "VggContainer/QVggEventAdapter.hpp"

#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  QVggEventAdapter::setup();

  // MainWindow w;
  // w.resize(1920, 1080);
  // w.show();

  Counter cppCounter1{"../../../../Counter/assets/counter_without_js.daruma",
                      false };
  if (auto widget = cppCounter1.widget()) {
    widget->setWindowTitle("cpp counter1");
    widget->move(0, 0);
  }

  // Counter cppCounter2{"../../../../Counter/assets/counter_without_js.daruma",
  //                     false};
  // if (auto widget = cppCounter2.widget()) {
  //   widget->setWindowTitle("cpp counter2");
  //   widget->move(800, 0);
  // }

  // Counter jsCounter1{"../../../../Counter/assets/counter_with_js.daruma", true};
  // if (auto widget = jsCounter1.widget()) {
  //   widget->setWindowTitle("js counter1");
  //   widget->move(0, 800);
  // }

  // Counter jsCounter2{"../../../../Counter/assets/counter_with_js.daruma", true};
  // if (auto widget = jsCounter2.widget()) {
  //   widget->setWindowTitle("js counter2");
  //   widget->move(800, 800);
  // }

  return a.exec();
}
