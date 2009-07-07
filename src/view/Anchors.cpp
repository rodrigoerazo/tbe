/* The Butterfly Effect
 * This file copyright (C) 2009  Klaas van Gend
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "Anchors.h"
#include "ImageStore.h"
#include "DrawObject.h"
#include "BaseObject.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>


//////////////////////////////////////////////////////////////////////////////
// first the implementation for class Anchors, below for class Anchor
// (note the missing 's')


Anchors::Anchors(DrawObject* anObjectPtr)
		: theDrawObjectPtr(anObjectPtr)
{
	// the below code creates the 8 anchors around the object

	PieMenu::EditMode myMode = PieMenu::NONE;
	if (theDrawObjectPtr->getBaseObjectPtr()->isResizable()&BaseObject::HORIZONTALRESIZE)
		myMode = PieMenu::RESIZE_HORI;
	theAnchorList.push_back(new Anchor(myMode, LEFT,  VMIDDLE, this));
	theAnchorList.push_back(new Anchor(myMode, RIGHT, VMIDDLE, this));

	if (theDrawObjectPtr->getBaseObjectPtr()->isResizable()&BaseObject::VERTICALRESIZE)
		myMode = PieMenu::RESIZE_VERTI;
	else
		myMode = PieMenu::NONE;
	theAnchorList.push_back(new Anchor(myMode, HMIDDLE, TOP,    this));
	theAnchorList.push_back(new Anchor(myMode, HMIDDLE, BOTTOM, this));

	if (theDrawObjectPtr->getBaseObjectPtr()->isRotatable())
		myMode = PieMenu::ROTATE;
	else
		myMode = PieMenu::NONE;
	theAnchorList.push_back(new Anchor(myMode, LEFT,  TOP,    this));
	theAnchorList.push_back(new Anchor(myMode, LEFT,  BOTTOM, this));
	theAnchorList.push_back(new Anchor(myMode, RIGHT, BOTTOM, this));
	theAnchorList.push_back(new Anchor(myMode, RIGHT, TOP,    this));
}

Anchors::~Anchors()
{
	while(!theAnchorList.isEmpty())
	{
		Anchor* myPtr = theAnchorList.first();
		delete myPtr;
		theAnchorList.pop_front();
	}
	theDrawObjectPtr->focusRemove(false);
}


QGraphicsScene* Anchors::getScenePtr()
{
	return theDrawObjectPtr->scene();
}

void Anchors::resize(PieMenu::EditMode aDirection, qreal aDelta)
{
}

void Anchors::rotate(qreal aDelta)
{
}

void Anchors::updatePosition()
{
	for (int i=0; i<theAnchorList.count();i++)
	{
		theAnchorList[i]->updatePosition();
	}
}

//////////////////////////////////////////////////////////////////////////////

Anchor::Anchor(PieMenu::EditMode aDirection, Anchors::HPosition anHPos, Anchors::VPosition aVPos, Anchors* aParent)
		: theParentPtr(aParent),
		  theDirection(aDirection),
		  theHPos(anHPos), theVPos(aVPos),
		  theUndoPtr(NULL)
{
	// get the QSvgRenderer for my icon
	setSharedRenderer(ImageStore::getRenderer(PieMenu::getEditModeIconName(aDirection)));

	if (aDirection!=PieMenu::NONE)
		setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsFocusable);

	// calculate how to scale my icon to look like it should
	QRectF mySquare=theParentPtr->getScenePtr()->views()[0]->mapToScene(QRect(0,0,theIconSize,theIconSize)).boundingRect();
	printf("32 pix = %f hori / %f verti\n", mySquare.width(), mySquare.height());
	// so we now know that our image should be reduced to width&height
	scale(mySquare.width()/boundingRect().width(),
		  mySquare.height()/boundingRect().height());
	theParentPtr->getScenePtr()->addItem(this);
	setZValue(1.0);

	theOffset=mySquare.width()/2.0;
	updatePosition();

	if (aDirection != PieMenu::NONE)
		setEnabled(true);
}


void Anchor::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
	DEBUG5("Anchor::mouseMoveEvent(%d)\n", event->type());

	// calculate which direction we're moving
	QPointF myDelta = event->scenePos() - thePrevMousePos;
	if (theDirection==PieMenu::RESIZE_HORI)
		myDelta.setY(0);
	if (theDirection==PieMenu::RESIZE_VERTI)
		myDelta.setX(0);
	theUndoPtr->setDelta(theHPos, myDelta);

	theParentPtr->updatePosition();
}

void Anchor::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
	DEBUG5("Anchor::mousePressEvent\n");

	// record cursor position here
	thePrevMousePos=event->scenePos ();

	// reset cursor to hori/verti/rotate
	if (theDirection==PieMenu::RESIZE_HORI)
		setCursor(Qt::SizeHorCursor);
	if (theDirection==PieMenu::RESIZE_VERTI)
		setCursor(Qt::SizeVerCursor);
	// TODO: add cursor ROTATE shape

	assert(theUndoPtr==NULL);
	theUndoPtr = theParentPtr->createUndoResize();
}

void Anchor::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
{
	DEBUG5("Anchor::mouseReleaseEvent\n");

	QPointF myDelta = event->scenePos() - thePrevMousePos;
	if (theDirection==PieMenu::RESIZE_HORI)
		myDelta.setY(0);
	if (theDirection==PieMenu::RESIZE_VERTI)
		myDelta.setX(0);

	// there was actual movement?
	if (myDelta.x()+myDelta.y() > 0.0)
	{
		DEBUG5("PUSHED UNDO RESIZE: dx=%f, dy=%f\n", myDelta.x(), myDelta.y());
		theParentPtr->pushUndo(theUndoPtr);
		// and make sure a next resize starts all over again
		theUndoPtr = NULL;
		thePrevMousePos=event->scenePos ();
	}
}

void Anchor::updatePosition()
{
	// TODO: the below code is executed 8 times for each resize/rotate
	// it may be smarter to just supply the Position and width/height as arguments
	Position myC = theParentPtr->getCenter();
	qreal myDX = 0.5*theParentPtr->getWidth()  * static_cast<qreal>(theHPos);
	qreal myDY = 0.5*theParentPtr->getHeight() * static_cast<qreal>(theVPos);

	myDX += (static_cast<qreal>(theHPos)-1.0)*theOffset;
	myDY += (static_cast<qreal>(theVPos)+1.0)*theOffset;

	setPos(myC.x+myDX, -myC.y-myDY);
}
