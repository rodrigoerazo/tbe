/* The Butterfly Effect
 * This file copyright (C) 2010  Klaas van Gend
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

#ifndef STUBDRAWWORLD_H
#define STUBDRAWWORLD_H

#include "BaseObject.h"
#include "World.h"

#include <QGraphicsScene>
#include <QUndoStack>

#include "Box2D.h"

/**
  * STUB VERSION OF DrawWorld
  */

class DrawWorld : public QGraphicsScene
{
public:

    Q_OBJECT

public:
    // Constructors/Destructors
    //

    /**
     * Empty Constructor
     */
    DrawWorld (void);
//  DrawWorld (MainWindow* aMainWindowPtr, World* aWorldPtr);

    /**
     * Empty Destructor
     */
    virtual ~DrawWorld ( );

    // Public accessor methods
    //

    qreal getWidth();
    qreal getHeight();

    QUndoStack *getTheUndoStackPtr(void);

    /** iterate through all registered objects and make them create new
      * bitmap caches.
      */
    void invalidateCaching(void);

public:
    const static int theMaxNumberOfGraphicsListElements = 125;
    typedef QList<QGraphicsItem *> GraphicsList;
    GraphicsList theGraphicsList;
    void addDebugDrawToList(QGraphicsItem *anItem);
    void clearGraphicsList(int aCount);

public:
    World *theWorldPtr;
    void initAttributes ( ) ;

};

#endif // STUBDRAWWORLD_H
