import QtQuick 2.12
import QtQuick.Window 2.12
import QNV 1.0 as QNV

Window {
    id: main
    visible: true
    title: 'qnv'
    color: '#282C34'

    property int gridWidth: Math.floor(width / gSession.cellWidth)
    property int gridHeight: Math.floor(height / gSession.cellHeight)
    property int xOffset: (width - gridWidth * gSession.cellWidth) / 2
    property int yOffset: (height - gridHeight * gSession.cellHeight) / 2

    Repeater {
        model: gGridList
        delegate: QNV.GridRenderer {
            grid: model.grid
            session: gSession
            width: grid.gridWidth * gSession.cellWidth
            height: grid.gridHeight * gSession.cellHeight
            x: grid.winX * gSession.cellWidth + xOffset
            y: grid.winY * gSession.cellHeight + yOffset
        }
    }

    onGridWidthChanged: gNeovim.tryResize(gridWidth, gridHeight);
    onGridHeightChanged: gNeovim.tryResize(gridWidth, gridHeight);
}
