#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <QtGlobal>
#include <QObject>
#include <QChar>

namespace Qnv {
Q_NAMESPACE
enum class Anchor : uint8_t { NW, NE, SW, SE };
Q_ENUM_NS(Anchor);
} // namespace Qnv

Q_DECLARE_METATYPE(Qnv::Anchor);

// TODO(ms): fix this, it's just to get the enum visible in QML and not break the code
using namespace Qnv;

struct GridLine
{
    struct Cell
    {
        uint32_t mChar { 0 };
        int32_t mHlId { 0 };
        int32_t mRepeat { 1 };
    };

    int64_t mGrid;
    int64_t mRow;
    int64_t mColStart;
    std::vector<Cell> mCells;
};

struct GridCell
{
    GridCell() { }
    QChar mCh[2] { 0, 0 }; ///< Character
    uint32_t mHl { 0 }; ///< Highlight group
};

struct HlGroup
{
    enum Attr {
        Inverse = 0x01,
        Bold = 0x02,
        Italic = 0x04,
        // The next three bits are all underline styles
        UnderlineMask = 0x38,
        Underline = 0x08,
        Undercurl = 0x10,
        Underdouble = 0x18,
        Underdotted = 0x20,
        Underdashed = 0x28,
        // 0x30 and 0x38 spare for underline styles
        Standout = 0x0040,
        Strikethrough = 0x0080,
        AltFont = 0x0100,
        // 0x0200 spare
        Nocombine = 0x0400,
        BgIndexed = 0x0800,
        FgIndexed = 0x1000,
        Default = 0x2000,
        Global = 0x4000,
    };

    int32_t mBgColor { -1 };
    int32_t mFgColor { -1 };
    int32_t mSpColor { -1 };
    uint16_t mAttrs { 0 };
    int16_t mHlBlend { -1 };

    friend bool operator==(const HlGroup& a, const HlGroup& b)
    {
        return a.mBgColor == b.mBgColor
            && a.mFgColor == b.mFgColor
            && a.mSpColor == b.mSpColor
            && a.mAttrs == b.mAttrs
            && a.mHlBlend == b.mHlBlend;
    }

    friend bool operator!=(const HlGroup& a, const HlGroup& b)
    {
        return !(a == b);
    }
};
