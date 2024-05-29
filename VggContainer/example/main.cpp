// #include "mainwindow.h"
#include "Counter.h"

#include "VggContainer/QVggEventAdapter.hpp"
#include "VGG/Environment.hpp"

#include <QApplication>

#include <filesystem>
#include <iostream>

int main(int argc, char* argv[])
{
  namespace fs = std::filesystem;
  std::cout << "Current path is " << fs::current_path() << std::endl;

  QApplication a(argc, argv);

  VGG::Environment::setUp();
  QVggEventAdapter::setup();

  // MainWindow w;
  // w.resize(1920, 1080);
  // w.show();

#ifdef _MSC_VER
  Counter cppCounter1{ "../../assets/counter_with_js.daruma", true };
#else
  // cwd = path/to/repo
  Counter cppCounter1{ "./VggContainer/example/assets/counter_without_js.daruma",
                       /*isJsCounter=*/false };
  // Counter cppCounter1{ "./VggContainer/example/assets/counter_with_js.daruma",
  // /*isJsCounter=*/true };
#endif

  if (auto widget = cppCounter1.widget())
  {
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

  QObject::connect(&a, &QApplication::aboutToQuit, [&]() { VGG::Environment::tearDown(); });
  return a.exec();
}
