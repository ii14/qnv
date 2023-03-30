#include "qnv/UserInput.hpp"

#include <QHash>
#include <QString>
#include <QDebug>

static const QHash<int, QString> kSpecialKeys = {
  // MISC KEYS
  { Qt::Key_Escape, "Esc" },
  { Qt::Key_Tab, "Tab" },
  { Qt::Key_Backspace, "BS" },
  { Qt::Key_Return, "CR" },
  { Qt::Key_Enter, "CR" },
  { Qt::Key_Insert, "Insert" },
  { Qt::Key_Delete, "Del" },
  // CURSOR MOVEMENT
  { Qt::Key_Home, "Home" },
  { Qt::Key_End, "End" },
  { Qt::Key_Left, "Left" },
  { Qt::Key_Up, "Up" },
  { Qt::Key_Right, "Right" },
  { Qt::Key_Down, "Down" },
  { Qt::Key_PageUp, "PageUp" },
  { Qt::Key_PageDown, "PageDown" },
  // FUNCTION KEYS
  { Qt::Key_F1, "F1" },
  { Qt::Key_F2, "F2" },
  { Qt::Key_F3, "F3" },
  { Qt::Key_F4, "F4" },
  { Qt::Key_F5, "F5" },
  { Qt::Key_F6, "F6" },
  { Qt::Key_F7, "F7" },
  { Qt::Key_F8, "F8" },
  { Qt::Key_F9, "F9" },
  { Qt::Key_F10, "F10" },
  { Qt::Key_F11, "F11" },
  { Qt::Key_F12, "F12" },
  // EXTRA KEYS
  { Qt::Key_Help, "Help" },
  // ASCII
  { Qt::Key_Space, "Space" },
  { Qt::Key_Backslash, "Bslash" },
  { Qt::Key_Less, "lt" },
};

// wtf is "<kOrigin> keypad origin (middle)"?
static const QHash<int, QString> kNumpadKeys = {
  { Qt::Key_Home, "kHome" },
  { Qt::Key_End, "kEnd" },
  { Qt::Key_Left, "kLeft" },
  { Qt::Key_Up, "kUp" },
  { Qt::Key_Right, "kRight" },
  { Qt::Key_Down, "kDown" },
  { Qt::Key_PageUp, "kPageUp" },
  { Qt::Key_PageDown, "kPageDown" },
  { Qt::Key_Delete, "kDel" },
  { '+', "kPlus" },
  { '-', "kMinus" },
  { '*', "kMultiply" },
  { '/', "kDivide" },
  { '.', "kPoint" },
  { ',', "kComma" },
  { '=', "kEqual" },
  { Qt::Key_Enter, "kEnter" },
  { '0', "k0" },
  { '1', "k1" },
  { '2', "k2" },
  { '3', "k3" },
  { '4', "k4" },
  { '5', "k5" },
  { '6', "k6" },
  { '7', "k7" },
  { '8', "k8" },
  { '9', "k9" },
};

QString UserInput::translateKeyCode(const QKeyEvent& ev)
{
  const auto mods = ev.modifiers();
  bool keycode = false;
  QString text;

  if (mods & Qt::KeypadModifier) {
    auto it = kNumpadKeys.find(ev.key());
    if (it == kNumpadKeys.end())
      return {};
    text = it.value();
    keycode = true;
  } else {
    auto it = kSpecialKeys.find(ev.key());
    if (it != kSpecialKeys.end()) {
      text = it.value();
      keycode = true;
    } else if (mods & Qt::ShiftModifier) {
      text = ev.text();
    } else {
      text = ev.text().toLower();
    }
  }

  if (text.isEmpty())
    return {};

  QString mod;
  if (mods & Qt::ControlModifier)
    mod += "C-";
  if (mods & Qt::AltModifier)
    mod += "A-";
  if (mods & Qt::MetaModifier)
    mod += "D-";
  if (mods & Qt::ShiftModifier && (keycode || (not keycode && not mod.isNull())))
    mod += "S-";
  if (not mod.isNull()) {
    if (not keycode) {
      if (mods & Qt::ShiftModifier) {
        text = QChar(ev.key());
      } else {
        text = QChar(ev.key()).toLower();
      }
    }
    keycode = true;
  }
  if (text.isEmpty())
    return {};
  if (keycode)
    return '<' + mod + text + '>';
  return text;
}

bool UserInput::eventFilter(QObject* watched, QEvent* event)
{
  if (event->type() != QEvent::KeyPress)
    return QObject::eventFilter(watched, event);
  const QString key = translateKeyCode(*static_cast<QKeyEvent*>(event));
  if (not key.isEmpty())
    emit keyEvent(key);
  return true;
}

// vim: tw=100 sw=2 sts=2 et
