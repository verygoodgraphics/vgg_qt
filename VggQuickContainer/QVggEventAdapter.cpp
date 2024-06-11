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

#include "QVggEventAdapter.hpp"

#include "VGG/Keycode.hpp"

#include <QGuiApplication>

#include <unordered_map>

namespace
{

#define VGG_RELEASED 0
#define VGG_PRESSED 1

auto& getKeyMap()
{
  static std::unordered_map<int, EVGGScancode> s_keyMap;
  return s_keyMap;
}

auto& getKeyState()
{
  static uint8_t s_keyState[VGG_NUM_SCANCODES];
  return s_keyState;
}

int vggSendKeyboardKey(uint8_t state, EVGGScancode scancode)
{
  if (scancode == VGG_SCANCODE_UNKNOWN || scancode >= VGG_NUM_SCANCODES)
  {
    return 0;
  }

  switch (state)
  {
    case VGG_PRESSED:
    case VGG_RELEASED:
      break;
    default:
      return 0;
  }

  getKeyState()[scancode] = state;

  return 0;
}

} // namespace

EVGGKeymod QVggEventAdapter::toVggModState(Qt::KeyboardModifiers keyboardModifiers)
{
  int keyMod = VGG_KMOD_NONE;

  if (keyboardModifiers.testFlag(Qt::ShiftModifier))
  {
    keyMod |= VGG_KMOD_SHIFT;
  }
  if (keyboardModifiers.testFlag(Qt::ControlModifier))
  {
    keyMod |= VGG_KMOD_CTRL;
  }
  if (keyboardModifiers.testFlag(Qt::AltModifier))
  {
    keyMod |= VGG_KMOD_ALT;
  }
  if (keyboardModifiers.testFlag(Qt::MetaModifier))
  {
    keyMod |= VGG_KMOD_GUI;
  }

  return static_cast<EVGGKeymod>(keyMod);
}

EVGGKeymod QVggEventAdapter::getModState()
{
  return toVggModState(QGuiApplication::keyboardModifiers());
}

uint8_t* QVggEventAdapter::getKeyboardState(int* nums)
{
  if (nums)
  {
    *nums = VGG_NUM_SCANCODES;
  }

  return getKeyState();
}

UEvent QVggEventAdapter::keyPressEvent(QKeyEvent* event)
{
  auto code = getKeyMap()[event->key()];
  vggSendKeyboardKey(VGG_PRESSED, code);

  UEvent vggEvent;
  vggEvent.key.type = VGG_KEYDOWN;
  vggEvent.key.keysym.mod = toVggModState(event->modifiers());
  vggEvent.key.keysym.sym = toVggKeyCode(event->key());

  return vggEvent;
}

UEvent QVggEventAdapter::keyReleaseEvent(QKeyEvent* event)
{
  auto code = getKeyMap()[event->key()];
  vggSendKeyboardKey(VGG_RELEASED, code);

  UEvent vggEvent;
  vggEvent.key.type = VGG_KEYUP;
  vggEvent.key.keysym.mod = toVggModState(event->modifiers());
  vggEvent.key.keysym.sym = toVggKeyCode(event->key());

  return vggEvent;
}

void QVggEventAdapter::setup()
{
  auto eventApi = std::make_unique<QVggEventAdapter>();
  EventManager::registerEventAPI(std::move(eventApi));

  auto& keyMap = getKeyMap();

  for (int k = Qt::Key_A, v = VGG_SCANCODE_A; k <= Qt::Key_Z; ++k, ++v)
  {
    keyMap[k] = static_cast<EVGGScancode>(v);
  }

  for (int k = Qt::Key_1, v = VGG_SCANCODE_1; k <= Qt::Key_9; ++k, ++v)
  {
    keyMap[k] = static_cast<EVGGScancode>(v);
  }
  keyMap[Qt::Key_0] = VGG_SCANCODE_0;

  keyMap[Qt::Key_Return] = VGG_SCANCODE_RETURN;
  keyMap[Qt::Key_Escape] = VGG_SCANCODE_ESCAPE;
  keyMap[Qt::Key_Backspace] = VGG_SCANCODE_BACKSPACE;
  keyMap[Qt::Key_Tab] = VGG_SCANCODE_TAB;
  keyMap[Qt::Key_Space] = VGG_SCANCODE_SPACE;

  keyMap[Qt::Key_Minus] = VGG_SCANCODE_MINUS;
  keyMap[Qt::Key_Equal] = VGG_SCANCODE_EQUALS;
  keyMap[Qt::Key_BracketLeft] = VGG_SCANCODE_LEFTBRACKET;
  keyMap[Qt::Key_BracketRight] = VGG_SCANCODE_RIGHTBRACKET;
  keyMap[Qt::Key_Backslash] = VGG_SCANCODE_BACKSLASH;

  // keyMap[Qt::Key_] = VGG_SCANCODE_NONUSHASH;

  keyMap[Qt::Key_Semicolon] = VGG_SCANCODE_SEMICOLON;
  keyMap[Qt::Key_Apostrophe] = VGG_SCANCODE_APOSTROPHE;
  // keyMap[Qt::Key_] = VGG_SCANCODE_GRAVE;
  keyMap[Qt::Key_Comma] = VGG_SCANCODE_COMMA;
  keyMap[Qt::Key_Period] = VGG_SCANCODE_PERIOD;
  keyMap[Qt::Key_Slash] = VGG_SCANCODE_SLASH;

  keyMap[Qt::Key_CapsLock] = VGG_SCANCODE_CAPSLOCK;
  for (int k = Qt::Key_F1, v = VGG_SCANCODE_F1; k <= Qt::Key_F12; ++k, ++v)
  {
    keyMap[k] = static_cast<EVGGScancode>(v);
  }

  keyMap[Qt::Key_ScreenSaver] = VGG_SCANCODE_PRINTSCREEN;
  keyMap[Qt::Key_ScrollLock] = VGG_SCANCODE_SCROLLLOCK;
  keyMap[Qt::Key_Pause] = VGG_SCANCODE_PAUSE;
  keyMap[Qt::Key_Insert] = VGG_SCANCODE_INSERT;
  keyMap[Qt::Key_Home] = VGG_SCANCODE_HOME;
  keyMap[Qt::Key_PageUp] = VGG_SCANCODE_PAGEUP;
  keyMap[Qt::Key_Delete] = VGG_SCANCODE_DELETE;
  keyMap[Qt::Key_End] = VGG_SCANCODE_END;
  keyMap[Qt::Key_PageDown] = VGG_SCANCODE_PAGEDOWN;
  keyMap[Qt::Key_Right] = VGG_SCANCODE_RIGHT;
  keyMap[Qt::Key_Left] = VGG_SCANCODE_LEFT;
  keyMap[Qt::Key_Down] = VGG_SCANCODE_DOWN;
  keyMap[Qt::Key_Up] = VGG_SCANCODE_UP;
  keyMap[Qt::Key_NumLock] = VGG_SCANCODE_NUMLOCKCLEAR;

  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_DIVIDE;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_MULTIPLY;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_MINUS;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_PLUS;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_ENTER;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_1;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_2;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_3;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_4;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_5;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_6;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_7;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_8;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_9;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_0;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_PERIOD;
  // keyMap[Qt::Key_] = VGG_SCANCODE_NONUSBACKSLASH;
  // keyMap[Qt::Key_] = VGG_SCANCODE_APPLICATION;
  // keyMap[Qt::Key_] = VGG_SCANCODE_POWER;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_EQUALS;

  for (int k = Qt::Key_F13, v = VGG_SCANCODE_F13; k <= Qt::Key_F24; ++k, ++v)
  {
    keyMap[k] = static_cast<EVGGScancode>(v);
  }

  keyMap[Qt::Key_Execute] = VGG_SCANCODE_EXECUTE;
  keyMap[Qt::Key_Help] = VGG_SCANCODE_HELP;
  keyMap[Qt::Key_Menu] = VGG_SCANCODE_MENU;
  keyMap[Qt::Key_Select] = VGG_SCANCODE_SELECT;
  keyMap[Qt::Key_Stop] = VGG_SCANCODE_STOP;
  // keyMap[Qt::Key_] = VGG_SCANCODE_AGAIN;
  keyMap[Qt::Key_Undo] = VGG_SCANCODE_UNDO;
  keyMap[Qt::Key_Cut] = VGG_SCANCODE_CUT;
  keyMap[Qt::Key_Copy] = VGG_SCANCODE_COPY;
  keyMap[Qt::Key_Paste] = VGG_SCANCODE_PASTE;
  keyMap[Qt::Key_Find] = VGG_SCANCODE_FIND;
  keyMap[Qt::Key_VolumeMute] = VGG_SCANCODE_MUTE;
  keyMap[Qt::Key_VolumeUp] = VGG_SCANCODE_VOLUMEUP;
  keyMap[Qt::Key_VolumeDown] = VGG_SCANCODE_VOLUMEDOWN;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_COMMA;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_EQUALSAS400;
  // keyMap[Qt::Key_] = VGG_SCANCODE_INTERNATIONAL1;
  // keyMap[Qt::Key_] = VGG_SCANCODE_INTERNATIONAL2;
  // keyMap[Qt::Key_] = VGG_SCANCODE_INTERNATIONAL3;
  // keyMap[Qt::Key_] = VGG_SCANCODE_INTERNATIONAL4;
  // keyMap[Qt::Key_] = VGG_SCANCODE_INTERNATIONAL5;
  // keyMap[Qt::Key_] = VGG_SCANCODE_INTERNATIONAL6;
  // keyMap[Qt::Key_] = VGG_SCANCODE_INTERNATIONAL7;
  // keyMap[Qt::Key_] = VGG_SCANCODE_INTERNATIONAL8;
  // keyMap[Qt::Key_] = VGG_SCANCODE_INTERNATIONAL9;
  // keyMap[Qt::Key_] = VGG_SCANCODE_LANG1;
  // keyMap[Qt::Key_] = VGG_SCANCODE_LANG2;
  // keyMap[Qt::Key_] = VGG_SCANCODE_LANG3;
  // keyMap[Qt::Key_] = VGG_SCANCODE_LANG4;
  // keyMap[Qt::Key_] = VGG_SCANCODE_LANG5;
  // keyMap[Qt::Key_] = VGG_SCANCODE_LANG6;
  // keyMap[Qt::Key_] = VGG_SCANCODE_LANG7;
  // keyMap[Qt::Key_] = VGG_SCANCODE_LANG8;
  // keyMap[Qt::Key_] = VGG_SCANCODE_LANG9;
  // keyMap[Qt::Key_] = VGG_SCANCODE_ALTERASE;
  keyMap[Qt::Key_SysReq] = VGG_SCANCODE_SYSREQ;
  keyMap[Qt::Key_Cancel] = VGG_SCANCODE_CANCEL;
  keyMap[Qt::Key_Clear] = VGG_SCANCODE_CLEAR;
  // keyMap[Qt::Key_] = VGG_SCANCODE_PRIOR;
  // keyMap[Qt::Key_] = VGG_SCANCODE_RETURN2;
  // keyMap[Qt::Key_] = VGG_SCANCODE_SEPARATOR;
  // keyMap[Qt::Key_] = VGG_SCANCODE_OUT;
  // keyMap[Qt::Key_] = VGG_SCANCODE_OPER;
  // keyMap[Qt::Key_] = VGG_SCANCODE_CLEARAGAIN;
  // keyMap[Qt::Key_] = VGG_SCANCODE_CRSEL;
  // keyMap[Qt::Key_] = VGG_SCANCODE_EXSEL;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_00;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_000;
  // keyMap[Qt::Key_] = VGG_SCANCODE_THOUSANDSSEPARATOR;
  // keyMap[Qt::Key_] = VGG_SCANCODE_DECIMALSEPARATOR;
  keyMap[Qt::Key_currency] = VGG_SCANCODE_CURRENCYUNIT;
  // keyMap[Qt::Key_] = VGG_SCANCODE_CURRENCYSUBUNIT;
  keyMap[Qt::Key_ParenLeft] = VGG_SCANCODE_KP_LEFTPAREN;
  keyMap[Qt::Key_ParenRight] = VGG_SCANCODE_KP_RIGHTPAREN;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_LEFTBRACE;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_RIGHTBRACE;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_TAB;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_BACKSPACE;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_A;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_B;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_C;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_D;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_E;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_F;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_XOR;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_POWER;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_PERCENT;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_LESS;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_GREATER;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_AMPERSAND;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_DBLAMPERSAND;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_VERTICALBAR;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_DBLVERTICALBAR;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_COLON;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_HASH;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_SPACE;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_AT;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_EXCLAM;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_MEMSTORE;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_MEMRECALL;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_MEMCLEAR;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_MEMADD;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_MEMSUBTRACT;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_MEMMULTIPLY;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_MEMDIVIDE;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_PLUSMINUS;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_CLEAR;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_CLEARENTRY;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_BINARY;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_OCTAL;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_DECIMAL;
  // keyMap[Qt::Key_] = VGG_SCANCODE_KP_HEXADECIMAL;
  // keyMap[Qt::Key_] = VGG_SCANCODE_LCTRL;
  // keyMap[Qt::Key_] = VGG_SCANCODE_LSHIFT;
  // keyMap[Qt::Key_] = VGG_SCANCODE_LALT;
  // keyMap[Qt::Key_] = VGG_SCANCODE_LGUI;
  // keyMap[Qt::Key_] = VGG_SCANCODE_RCTRL;
  // keyMap[Qt::Key_] = VGG_SCANCODE_RSHIFT;
  // keyMap[Qt::Key_] = VGG_SCANCODE_RALT;
  // keyMap[Qt::Key_] = VGG_SCANCODE_RGUI;
  // keyMap[Qt::Key_] = VGG_SCANCODE_MODE;
  keyMap[Qt::Key_MediaNext] = VGG_SCANCODE_AUDIONEXT;
  keyMap[Qt::Key_MediaPrevious] = VGG_SCANCODE_AUDIOPREV;
  keyMap[Qt::Key_MediaStop] = VGG_SCANCODE_AUDIOSTOP;
  keyMap[Qt::Key_MediaPlay] = VGG_SCANCODE_AUDIOPLAY;
  // keyMap[Qt::Key_] = VGG_SCANCODE_AUDIOMUTE;
  // keyMap[Qt::Key_] = VGG_SCANCODE_MEDIASELECT;
  keyMap[Qt::Key_WWW] = VGG_SCANCODE_WWW;
  keyMap[Qt::Key_LaunchMail] = VGG_SCANCODE_MAIL;
  keyMap[Qt::Key_Calendar] = VGG_SCANCODE_CALCULATOR;
  // keyMap[Qt::Key_] = VGG_SCANCODE_COMPUTER;
  keyMap[Qt::Key_Search] = VGG_SCANCODE_AC_SEARCH;
  // keyMap[Qt::Key_] = VGG_SCANCODE_AC_HOME;
  // keyMap[Qt::Key_] = VGG_SCANCODE_AC_BACK;
  // keyMap[Qt::Key_] = VGG_SCANCODE_AC_FORWARD;
  // keyMap[Qt::Key_] = VGG_SCANCODE_AC_STOP;
  // keyMap[Qt::Key_] = VGG_SCANCODE_AC_REFRESH;
  // keyMap[Qt::Key_] = VGG_SCANCODE_AC_BOOKMARKS;
  keyMap[Qt::Key_MonBrightnessDown] = VGG_SCANCODE_BRIGHTNESSDOWN;
  keyMap[Qt::Key_MonBrightnessUp] = VGG_SCANCODE_BRIGHTNESSUP;
  keyMap[Qt::Key_Display] = VGG_SCANCODE_DISPLAYSWITCH;
  keyMap[Qt::Key_KeyboardLightOnOff] = VGG_SCANCODE_KBDILLUMTOGGLE;
  keyMap[Qt::Key_KeyboardBrightnessDown] = VGG_SCANCODE_KBDILLUMDOWN;
  keyMap[Qt::Key_KeyboardBrightnessUp] = VGG_SCANCODE_KBDILLUMUP;
  keyMap[Qt::Key_Eject] = VGG_SCANCODE_EJECT;
  keyMap[Qt::Key_Sleep] = VGG_SCANCODE_SLEEP;
  // keyMap[Qt::Key_] = VGG_SCANCODE_APP1;
  // keyMap[Qt::Key_] = VGG_SCANCODE_APP2;
  keyMap[Qt::Key_AudioRewind] = VGG_SCANCODE_AUDIOREWIND;
  keyMap[Qt::Key_AudioForward] = VGG_SCANCODE_AUDIOFASTFORWARD;
  keyMap[Qt::Key_ApplicationLeft] = VGG_SCANCODE_SOFTLEFT;
  keyMap[Qt::Key_ApplicationRight] = VGG_SCANCODE_SOFTRIGHT;
  keyMap[Qt::Key_Call] = VGG_SCANCODE_CALL;
  keyMap[Qt::Key_Hangup] = VGG_SCANCODE_ENDCALL;
}

EVGGKeyCode QVggEventAdapter::toVggKeyCode(int key)
{
  auto scanCode = getKeyMap()[key];
  return EventManager::getKeyFromScancode(scanCode);
}
