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

#pragma once

#include <QOpenGLWidget>

#include "VGG/ISdk.hpp"

class QVggOpenGLWidgetImpl;
class QVggOpenGLWidget : public QOpenGLWidget {
  Q_OBJECT
  QVggOpenGLWidgetImpl *m_impl;

public:
  using EventListener =
      std::function<void(std::shared_ptr<VGG::ISdk> vggSdk, std::string type,
                         std::string targetId, std::string targetPath)>;

public:
  QVggOpenGLWidget(QWidget *parent = nullptr);
  ~QVggOpenGLWidget();

  bool load(const std::string &filePath,
            const char *designDocSchemaFilePath = nullptr,
            const char *layoutDocSchemaFilePath = nullptr);
  void setEventListener(EventListener listener);

protected:
  virtual void initializeGL() override;
  virtual void resizeGL(int w, int h) override;
  virtual void paintGL() override;

  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;

  void keyPressEvent(QKeyEvent *event) override;
  void keyReleaseEvent(QKeyEvent *event) override;

private:
  void init(int w, int h);
};
