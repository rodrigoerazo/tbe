#include "DrawObject.h"
#include "BaseObject.h"

#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOption>

// Constructors/Destructors
//  

DrawObject::DrawObject (BaseObject* aBaseObjectPtr)
	: theBaseObjectPtr(aBaseObjectPtr)
{
	theBodyID = aBaseObjectPtr->getTheBodyID();
	initAttributes();

	// the objects sizes usually are less than a meter
	// however, that does not sit well with QPainter, which is still a 
	// bitmap-oriented class - we're discussing images of less than one-by-one pixel.
	// that's what we need scaling for.
	
	// TODO: Let's assume milimeters here!
    scale(1.0/theScale, 1.0/theScale);
    
    // in radians!
    theOldAngle=0;//aBaseObjectPtr->getTheCenter().angle;
}

DrawObject::~DrawObject ( ) { }

//  
// Methods
//  


// Accessor methods
//  


// Other methods
//  

void DrawObject::advance(int step)
{
	applyPosition();
}

void DrawObject::applyPosition(void)
{
	// TODO FIXME: Add rotation here
    const dReal *pos1 = dGeomGetPosition (theBaseObjectPtr->getTheGeomID());
    const dReal *ang  = dGeomGetRotation (theBaseObjectPtr->getTheGeomID());
//DEBUG5("%p: %f, %f\n", this, pos1[0], pos1[1]);
    
    qreal myAngle = atan2(ang[1], ang[0]);
    
    // Qt has Y positive downwards, whereas all of the model has Y upwards.
    // that's what the minus is for :-)
    setPos(pos1[0], -pos1[1]);
    rotate((myAngle-theOldAngle)*180/3.14);
    theOldAngle=myAngle;
}

QRectF DrawObject::boundingRect() const
{
	qreal myWidth = theBaseObjectPtr->getTheWidth()*theScale;
	qreal myHeight= theBaseObjectPtr->getTheHeight()*theScale;
	qreal adjust = 0.1;
	
    return QRectF(-myWidth/2-adjust, -myHeight/2-adjust, myWidth+2*adjust, myHeight+2*adjust);
}

void DrawObject::initAttributes ( ) 
{
	
}

void DrawObject::paint(QPainter* myPainter, const QStyleOptionGraphicsItem *, QWidget *)
{
	qreal myWidth = theBaseObjectPtr->getTheWidth()*theScale;
	qreal myHeight= theBaseObjectPtr->getTheHeight()*theScale;
printf("%p: %f %f\n", this, myWidth, myHeight);
	QColor color(qrand() % 256, qrand() % 256, qrand() % 256);
    // Body
    myPainter->drawRect(-myWidth/2, -myHeight/2, myWidth, myHeight);

    myPainter->setBrush(color);

    myPainter->drawEllipse(-myWidth/2, -myHeight/2, myWidth, myHeight);

    applyPosition();
}
