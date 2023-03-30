import QtQuick 2.12
import QtQuick.Window 2.12
// clean this up lol
import QNV 1.0 as QNV
import Qnv 1.0

Window {
    id: main
    visible: true
    title: 'qnv'
    color: '#282C34'

    property int gridWidth: Math.floor(width / gSession.cellWidth)
    property int gridHeight: Math.floor(height / gSession.cellHeight)
    property int xOffset: (width - gridWidth * gSession.cellWidth) / 2
    property int yOffset: (height - gridHeight * gSession.cellHeight) / 2

    function xToPx(v) { return v * gSession.cellWidth + xOffset; }
    function yToPx(v) { return v * gSession.cellHeight + yOffset; }

    function xAnchor(id, x, dir, w) {
        const parent = gGridList.get(id);
        if (dir == Qnv.NE || dir == Qnv.SE)
            return xToPx(parent.winX + x - w);
        return xToPx(parent.winX + x);
    }

    function yAnchor(id, y, dir, h) {
        const parent = gGridList.get(id);
        if (dir == Qnv.SE || dir == Qnv.SW)
            return yToPx(parent.winY + y - h);
        return yToPx(parent.winY + y);
    }

    Repeater {
        model: gGridList
        delegate: QNV.GridRenderer {
            grid: model.grid
            session: gSession
            width: grid.gridWidth * gSession.cellWidth
            height: grid.gridHeight * gSession.cellHeight
            x: grid.anchorGrid < 1 ? xToPx(grid.winX)
                : xAnchor(grid.anchorGrid, grid.anchorCol, grid.anchor, grid.gridWidth)
            y: grid.anchorGrid < 1 ? yToPx(grid.winY)
                : yAnchor(grid.anchorGrid, grid.anchorRow, grid.anchor, grid.gridHeight)
            z: grid.zIndex
            visible: !grid.hidden
        }
    }

    onGridWidthChanged: gNeovim.tryResize(gridWidth, gridHeight);
    onGridHeightChanged: gNeovim.tryResize(gridWidth, gridHeight);
}
