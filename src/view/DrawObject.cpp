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

#include "DrawObject.h"
#include "BaseObject.h"
#include "PieMenu.h"
#include "Anchors.h"
#include "ImageStore.h"
#include "DrawWorld.h"

#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOption>
#include <QGraphicsSceneMouseEvent>
#include <QFocusEvent>
#include <QUndoStack>

#include "UndoMoveCommand.h"
#include <cmath>

// set/get using static setter/getter in DrawObject
// as such it is available to DrawObject and derived classes
static QUndoStack* theUndoStackPtr = NULL;

Anchors* DrawObject::theAnchorsPtr = NULL;

QSvgRenderer* DrawObject::theCrossRendererPtr = NULL;

// Constructors/Destructors
//  

DrawObject::DrawObject (BaseObject* aBaseObjectPtr)
	: theBaseObjectPtr(aBaseObjectPtr), theRenderer (NULL),
	thePixmapPtr(NULL),	theUndeleteDrawWorldPtr(NULL), theUndoMovePtr(NULL)
{
	if (theBaseObjectPtr!=NULL)
		initAttributes();
}

DrawObject::DrawObject (BaseObject* aBaseObjectPtr,
						const QString& anImageName,
						UNUSED_ARG DrawObject::ImageType anImageType)
	: theBaseObjectPtr(aBaseObjectPtr), theRenderer (NULL),
	thePixmapPtr(NULL), theUndeleteDrawWorldPtr(NULL), theUndoMovePtr(NULL)
{
	initAttributes();
	if (anImageType==IMAGE_PNG || anImageType==IMAGE_ANY)
		thePixmapPtr= ImageStore::getPNGPixmap(anImageName);
	if (thePixmapPtr == NULL)
		theRenderer = ImageStore::getRenderer(anImageName);
}


DrawObject::~DrawObject ( )
{
	if (theAnchorsPtr)
	{
		delete theAnchorsPtr;
		theAnchorsPtr = NULL;
	}
}

//  
// Methods
//  


// Accessor methods
//  

void DrawObject::setUndoStackPtr(QUndoStack* aPtr)
{
	assert(aPtr != NULL);
	theUndoStackPtr = aPtr;
}
    
QUndoStack* DrawObject::getUndoStackPtr(void)
{
	assert(theUndoStackPtr != NULL);
	return theUndoStackPtr;
}

// Other methods
//  

void DrawObject::advance(int)
{
	applyPosition();
}

void DrawObject::applyPosition(void)
{
	Position myPosition = theBaseObjectPtr->getTempCenter();
    // Qt has Y positive downwards, whereas all of the model has Y upwards.
    // that's what the minus is for :-)
    setPos(myPosition.x, -myPosition.y);
	rotate((-myPosition.angle-theOldAngle)*180/3.14);
	theOldAngle=-myPosition.angle;
}

QRectF DrawObject::boundingRect() const
{
	qreal myWidth = theBaseObjectPtr->getTheWidth()*theScale;
	qreal myHeight= theBaseObjectPtr->getTheHeight()*theScale;
	qreal adjust = 0.03;
	
    return QRectF(-myWidth/2-adjust, -myHeight/2-adjust, myWidth+2*adjust, myHeight+2*adjust);
}

bool DrawObject::checkForCollision(void)
{
	isCollidingDuringDrag = (scene()->collidingItems(this).isEmpty() == false);
	return isCollidingDuringDrag;
}

void DrawObject::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
	if (theBaseObjectPtr->isMovable()==true &&
		theIsLevelEditor == false)
	{
		PieMenu myMenu(this);
		myMenu.exec(event->screenPos());
	}
}

bool DrawObject::deregister()
{
	theUndeleteDrawWorldPtr = reinterpret_cast<DrawWorld*>(scene());
	focusRemove();
	scene()->removeItem(this);
	return true;
}

void DrawObject::drawCollisionCross(QPainter* aPainter, const QRectF& myRect)
{
	// draw a dotted outline if colliding during move of object
	if (isCollidingDuringDrag)
	{
		if (theCrossRendererPtr)
			theCrossRendererPtr->render(aPainter, myRect);
		else
		{
			aPainter->drawLine(myRect.bottomLeft(), myRect.topRight());
			aPainter->drawLine(myRect.bottomRight(), myRect.topLeft());
		}
	}
}

void DrawObject::focusInEvent ( QFocusEvent * event )
{
	DEBUG5("focusInEvent for %p with %d\n", this, event->reason());
	if (theAnchorsPtr!=NULL)
	{
		// the Anchors destructor will notify the other DrawObject
		delete theAnchorsPtr;
	}
	theAnchorsPtr=new Anchors(this);
	update();
}

void DrawObject::focusOutEvent ( QFocusEvent * event )
{
	DEBUG5("focusInEvent for %p with %d\n", this, event->reason());
	delete theAnchorsPtr;
	theAnchorsPtr = NULL;
}

void DrawObject::focusRemove(bool alsoDeleteAnchors)
{
	if (theAnchorsPtr!=NULL && alsoDeleteAnchors==true)
	{
		delete theAnchorsPtr;
		theAnchorsPtr=NULL;
	}
	update();
}

void DrawObject::hoverMoveEvent ( QGraphicsSceneHoverEvent *)
{
	// TODO: hover should report to EditState - so it can create a HoverPointer for delete
}

void DrawObject::hoverLeaveEvent ( QGraphicsSceneHoverEvent *)
{
	// TODO: hover should report to EditState - so it can create a HoverPointer for delete
}

void DrawObject::initAttributes ( )
{
	// the objects sizes usually are less than a meter
	// however, that does not sit well with QPainter, which is still a
	// bitmap-oriented class - we're discussing images of less than one-by-one pixel.
	// that's what we need scaling for.
	//
	// theScale is set to 100.0 - that implies centimeters.
	scale(1.0/theScale, 1.0/theScale);

	// in radians!
	theOldAngle=0;//aBaseObjectPtr->getOrigCenter().angle;

	// make sure that this item is selectable & draggable
	// (if the object allows it - of course)
	if (theBaseObjectPtr->isMovable())
	{
		setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);
		setAcceptHoverEvents(true);
	}

	if (theCrossRendererPtr==NULL)
	{
		theCrossRendererPtr = ImageStore::getRenderer("BigCross");
	}

//    setCacheMode(QGraphicsItem::ItemCoordinateCache, QSize(128,128));
	setToolTip(theBaseObjectPtr->getToolTip());

	isCollidingDuringDrag=false;

	applyPosition();

}


void DrawObject::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
	DEBUG5("DrawObject::mouseMoveEvent(%d)\n", event->type());

	// do not allow movement of predefined objects in game mode
	if (theBaseObjectPtr->isMovable() == false)
		return;

	QPointF myPos=event->scenePos ();


	// if this is the first call to mouseMove, we need to create and initialise the undomove
	if (theUndoMovePtr ==NULL)
	{
		theUndoMovePtr = new UndoMoveCommand(this, theBaseObjectPtr);
		if (theAnchorsPtr)
		{
			delete theAnchorsPtr;
			theAnchorsPtr = NULL;
		}
	}

	// TODO: problem: if you click an object near the side, it will still register as if you
	// clicked in the exact center - with an unvoluntary movement as a result

	checkForCollision();

	// do not allow object to be moved through X and Y axes to negative coords
	if ( (myPos.x()-theBaseObjectPtr->getTheWidth()/2.0) >= 0.0
		 && (myPos.y()+theBaseObjectPtr->getTheHeight()/2.0) <= 0.0)
	{
		theUndoMovePtr->setNewPosition(myPos, (isCollidingDuringDrag==false) );
		theUndoMovePtr->redo();
	}
}

void DrawObject::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
{
	DEBUG5("DrawObject::mouseReleaseEvent(%d) for button %d\n", event->type(), event->button());
	// TODO: FIXME: for now, we're not discriminative for left or right mousebutton...

	if (theUndoMovePtr == NULL)
		return;

	// are we currently in a collision?
	// in that case, go back to last known good
	checkForCollision();
	if (isCollidingDuringDrag)
	{
		DEBUG4("Reverting to last known non-colliding position\n");
		theUndoMovePtr->revertToLastGood();
		isCollidingDuringDrag = false;
	}

	// is the position any different?
	if (theUndoMovePtr->hasMoved())
		pushUndo(theUndoMovePtr);
	else
		delete theUndoMovePtr;
	theUndoMovePtr = NULL;

	QGraphicsItem::mouseReleaseEvent(event);
	if (theAnchorsPtr==NULL)
		theAnchorsPtr= new Anchors(this);
}

void DrawObject::paint(QPainter* myPainter, const QStyleOptionGraphicsItem *, QWidget *)
{
	qreal myWidth = theBaseObjectPtr->getTheWidth()*theScale;
	qreal myHeight= theBaseObjectPtr->getTheHeight()*theScale;
	QRectF myRect(-myWidth/2.0,-myHeight/2.0,myWidth,myHeight);

	DEBUG6("DrawObject::paint for %p: @(%f,%f)\n", this, myWidth, myHeight);
	if (thePixmapPtr != NULL)
	{
		myPainter->drawPixmap(myRect, *thePixmapPtr, thePixmapPtr->rect());
		goto checkForCollision;
	}

	if (theRenderer != NULL)
	{
		theRenderer->render(myPainter, myRect);
		goto checkForCollision;
	}

	// Backup for Body drawing
	{
		QColor color(qrand() % 256, qrand() % 256, qrand() % 256);
		myPainter->setBrush(color);
		myPainter->drawEllipse(myRect);
	}

checkForCollision:
	drawCollisionCross(myPainter, myRect);
}

bool DrawObject::pushUndo(QUndoCommand* anUndo)
{
	getUndoStackPtr()->push(anUndo);
	return true;
}

bool DrawObject::reregister()
{
	assert(theUndeleteDrawWorldPtr);
	theUndeleteDrawWorldPtr->addItem(this);
	return true;
}
