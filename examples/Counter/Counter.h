#pragma once

#include "VggContainer/QVggOpenGLWidget.hpp"

class Counter {
  QVggOpenGLWidget m_vggContainer;

public:
  Counter(const std::string &vggFilePath, bool isJsCounter, QWidget* parent = nullptr);

  QWidget *widget() { return &m_vggContainer; }
};
