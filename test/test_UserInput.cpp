#include <catch2/catch_test_macros.hpp>
#include <vector>
#include "tostring.hpp"
#include "qnv/UserInput.hpp"

CATCH_QT_TOSTRING(Qt::KeyboardModifiers)
CATCH_QT_TOSTRING(Qt::Key)

struct TestCase
{
  int key;
  int mods;
  QString text;
  QString expected;
};

enum : int {
  No = Qt::NoModifier,
  Shift = Qt::ShiftModifier,
  Ctrl = Qt::ControlModifier,
  Alt = Qt::AltModifier,
  Meta = Qt::MetaModifier,
  Keypad = Qt::KeypadModifier,
  GroupSwitch = Qt::GroupSwitchModifier,
};

static const std::vector<TestCase> kTestCases = {
  { Qt::Key_Alt, Alt, "", {} },
  { Qt::Key_Control, Ctrl, "", {} },
  { Qt::Key_Shift, Shift, "", {} },
  { Qt::Key_Meta, Meta, "", {} },
  {
      Qt::Key_AltGr,
      GroupSwitch,
      "",
      {},
  },

  { Qt::Key_A, No, "a", "a" },
  { Qt::Key_A, Alt, "a", "<A-a>" },
  { Qt::Key_A, Ctrl, "\u0001", "<C-a>" },
  { Qt::Key_A, Ctrl | Alt, "\u0001", "<C-A-a>" },
  { Qt::Key_A, Shift, "A", "A" },
  { Qt::Key_A, Shift | Alt, "A", "<A-S-A>" },
  { Qt::Key_A, Shift | Ctrl, "\u0001", "<C-S-A>" },
  { Qt::Key_A, Shift | Ctrl | Alt, "\u0001", "<C-A-S-A>" },

  { Qt::Key_Equal, No, "=", "=" },
  { Qt::Key_Equal, Ctrl, "=", "<C-=>" },
  { Qt::Key_Equal, Alt, "=", "<A-=>" },
  { Qt::Key_Equal, Ctrl | Alt, "=", "<C-A-=>" },
  { Qt::Key_Plus, Shift, "+", "+" },
  // should "S-" be here? because it is
  { Qt::Key_Plus, Shift | Ctrl, "+", "<C-S-+>" },
  { Qt::Key_Plus, Shift | Alt, "+", "<A-S-+>" },
  { Qt::Key_Plus, Shift | Ctrl | Alt, "+", "<C-A-S-+>" },

  { Qt::Key_F1, No, "", "<F1>" },
  { Qt::Key_F1, Alt, "", "<A-F1>" },
  { Qt::Key_F1, Ctrl, "", "<C-F1>" },
  { Qt::Key_F1, Ctrl | Alt, "", "<C-A-F1>" },
  { Qt::Key_F1, Shift, "", "<S-F1>" },
  { Qt::Key_F1, Shift | Alt, "", "<A-S-F1>" },
  { Qt::Key_F1, Shift | Ctrl, "", "<C-S-F1>" },
  { Qt::Key_F1, Shift | Ctrl | Alt, "", "<C-A-S-F1>" },

  { Qt::Key_Return, No, "\r", "<CR>" },
  { Qt::Key_Return, Alt, "\r", "<A-CR>" },
  { Qt::Key_Return, Ctrl, "\r", "<C-CR>" },
  { Qt::Key_Return, Ctrl | Alt, "\r", "<C-A-CR>" },
  { Qt::Key_Return, Shift, "\r", "<S-CR>" },
  { Qt::Key_Return, Shift | Alt, "\r", "<A-S-CR>" },
  { Qt::Key_Return, Shift | Ctrl, "\r", "<C-S-CR>" },
  { Qt::Key_Return, Shift | Ctrl | Alt, "\r", "<C-A-S-CR>" },

  { Qt::Key_Less, No, "<", "<lt>" },
  { Qt::Key_Less, Alt, "<", "<A-lt>" },
  { Qt::Key_Less, Ctrl, "<", "<C-lt>" },
  { Qt::Key_Less, Ctrl | Alt, "<", "<C-A-lt>" },
  { Qt::Key_Less, Shift, "<", "<S-lt>" },
  { Qt::Key_Less, Shift | Alt, "<", "<A-S-lt>" },
  { Qt::Key_Less, Shift | Ctrl, "<", "<C-S-lt>" },
  { Qt::Key_Less, Shift | Ctrl | Alt, "<", "<C-A-S-lt>" },

  { Qt::Key_Space, No, " ", "<Space>" },
  { Qt::Key_Space, Shift, " ", "<S-Space>" },
  { Qt::Key_Space, Ctrl, "\u0000", "<C-Space>" },
  { Qt::Key_Space, Alt, " ", "<A-Space>" },
  { Qt::Key_Space, Ctrl | Alt, "\u0000", "<C-A-Space>" },
  { Qt::Key_Space, Shift | Ctrl, "\u0000", "<C-S-Space>" },
  { Qt::Key_Space, Shift | Alt, " ", "<A-S-Space>" },
  { Qt::Key_Space, Shift | Ctrl | Alt, "\u0000", "<C-A-S-Space>" },

  { Qt::Key(321), GroupSwitch, "ł", "ł" },
  { Qt::Key(321), Shift | GroupSwitch, "Ł", "Ł" },
  { Qt::Key(321), Alt | GroupSwitch, "ł", "<A-ł>" },
  { Qt::Key(321), Ctrl | GroupSwitch, "ł", "<C-ł>" },
  { Qt::Key(321), Ctrl | Alt | GroupSwitch, "ł", "<C-A-ł>" },
  // same, should "S-" be here?
  // { Qt::Key(321), Shift|Alt|GroupSwitch, "Ł", "<A-Ł>" },
  // { Qt::Key(321), Shift|Ctrl|GroupSwitch, "Ł", "<C-Ł>" },
  // { Qt::Key(321), Shift|Ctrl|Alt|GroupSwitch, "Ł", "<C-A-Ł>" },

  { Qt::Key_Pause, No, "", {} },
  { Qt::Key(0), Ctrl, "", {} }, // ctrl+pause sends this
  { Qt::Key_Help, No, "", "<Help>" },

  // TODO: test numpad keys. no idea how to enable numpad keys on
  // a thinkpad, check what events are produced with a full keyboard
};

TEST_CASE("input")
{
  for (const auto& test : kTestCases) {
    auto key = Qt::Key(test.key);
    auto mods = Qt::KeyboardModifiers(test.mods);
    auto text = test.text;
    CAPTURE(key);
    CAPTURE(mods);
    CAPTURE(text);
    QKeyEvent ev { QEvent::KeyPress, test.key, mods, text };
    auto result = UserInput::translateKeyCode(ev);
    CHECK(result == test.expected);
  }
}

// vim: tw=100 sw=2 sts=2 et
