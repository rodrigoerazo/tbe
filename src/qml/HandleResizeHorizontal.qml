/* The Butterfly Effect
 * This file copyright (C) 2016 Klaas van Gend
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation
 * applicable version is GPL version 2 only.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA.
 */

import QtQuick 2.0

Image {
    property real sign: 1; // 1 means right, -1 means left

    anchors.horizontalCenter: (sign>0) ? parent.right : parent.left
    anchors.horizontalCenterOffset: sign * width/3
    anchors.verticalCenter: parent.verticalCenter
    source: img("ActionResizeHorizontally")

    visible: theDecorated.isHResize === true && theDecorator.isBackInToolbox !== true;

    width: ResizeInfo.handleWidth
    height: ResizeInfo.handleHeight
    sourceSize.width: width
    sourceSize.height: height

    MouseArea {
        anchors.fill: parent
        drag{ target: parent; axis: Drag.XAxis}
        onMouseXChanged: {
            if (theDecorator.theActiveHandle != parent)
                theDecorator.startNewUndo("HandleResize", parent);
            if(drag.active){
                var myMouseX = mouseX
                if (theDecorated.width + sign*mouseX < minSize)
                    myMouseX = - sign*theDecorated.width + sign*minSize
                theDecorated.width += sign*myMouseX
                theDecorated.x += -sign*myMouseX * (0.5 - sign*0.5*Math.cos(theDecorated.rotation/180*Math.PI))
                theDecorated.y += myMouseX * 0.5 * Math.sin(theDecorated.rotation/180*Math.PI)
                theDecorator.width = theDecorated.width
                theDecorated.updateVars();
            }
        }
        onReleased: {
            theDecorated.updateVars();
            theDecorated.restoreBindings();
        }
    }
}
