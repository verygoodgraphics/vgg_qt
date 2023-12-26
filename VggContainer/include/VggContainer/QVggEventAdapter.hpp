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

#include "VGG/Event.hpp"
#include "VGG/EventAPI.hpp"

#include <QKeyEvent>

class QVggEventAdapter : public EventAPI {
public:
  static void setup();

  static UEvent keyPressEvent(QKeyEvent *event);
  static UEvent keyReleaseEvent(QKeyEvent *event);

  EVGGKeymod getModState() override;
  uint8_t *getKeyboardState(int *nums) override;

private:
  static EVGGKeymod toVggModState(Qt::KeyboardModifiers keyboardModifiers);
  static EVGGKeyCode toVggKeyCode(int key);
};
