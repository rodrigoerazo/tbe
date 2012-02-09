/* The Butterfly Effect
 * This file copyright (C) 2009, 2010  Klaas van Gend
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

#ifndef ANCHORS_H
#define ANCHORS_H

#include <QGraphicsItem>
#include <QGraphicsSvgItem>

#include "DrawObject.h"
#include "BaseObject.h"
#include "EditObjectDialog.h"

// forward declarations
class Anchor;
class QGraphicsScene;
class UndoObjectChange;
class DetonatorBox;

/// the Anchors class manages the resize/rotate/delete anchors around a selected DrawObject
class Anchors : public QObject
{
	Q_OBJECT

public:
	Anchors(DrawObject* anObjectPtr);
	virtual ~Anchors();

	QGraphicsScene* getScenePtr();

	enum AnchorType
	{
		NONE,
		RESIZEHORI,
		RESIZEVERTI,
		ROTATE,
		BUTTON
	};

	BaseObject*   getBOPtr(void) const;

	/// @returns true if anItem is one of the Anchors
	bool isAnchor(QGraphicsItem* anItem);

	bool pushUndo(QUndoCommand* anUndo)
		{ return theDrawObjectPtr->pushUndo(anUndo); }

	/// iterates through all Anchor's to make them update their position
	void updatePosition();

	static EditObjectDialog* getEditObjectDialogPtr(void)
	{ return theObjectDialogPtr; }
	static void clearEditObjectDialogPtr(void)
	{ delete theObjectDialogPtr; theObjectDialogPtr = NULL; }

protected slots:
	void delete_clicked();
	void editObject_clicked();
	void setPhoneNr_clicked();

private:
	DetonatorBox* theDetBoxPtr;
	DrawObject* theDrawObjectPtr;

	typedef QList<Anchor*> AnchorList;
	AnchorList theAnchorList;

	static EditObjectDialog* theObjectDialogPtr;

	// kill copy constructor & assignment operator
	Anchors(const Anchors&);
	const Anchors& operator=(const Anchors&);
};



/// Anchor is a single item used by the Anchors class
class Anchor : public QGraphicsSvgItem
{
	Q_OBJECT

public:
	enum AnchorPosition
	{
		RIGHT       = 0,
		TOPRIGHT    = 1,
		TOP         = 2,
		TOPLEFT     = 3,
		LEFT        = 4,
		BOTTOMLEFT  = 5,
		BOTTOM      = 6,
		BOTTOMRIGHT = 7,
		BUTTONBAR   = 8
	};

	/// Constructor
	Anchor(Anchors::AnchorType aDirection, AnchorPosition anIndex, Anchors* aParent);

public slots:
	/// slot that tells the position may have changed - called by the Anchors class.
	virtual void updatePosition(Position myC, qreal myW, qreal myH);

protected:
	void scaleIcon(void);

	Anchors* theParentPtr;
	Anchors::AnchorType theDirection;
	AnchorPosition theIndex;
	static const int theIconSize;
	int theButtonIndex;

	qreal theOffset;
	qreal theOldAngle;

	int getDX();
	int getDY();

	// kill copy constructor & assignment operator
	Anchor(const Anchor&);
	const Anchor& operator=(const Anchor&);
};



/// ResizeAnchor is a single item used by the Anchors class
class ResizeAnchor : public Anchor
{
	Q_OBJECT

public:
	/// Constructor
	ResizeAnchor(Anchors::AnchorType aDirection, AnchorPosition anIndex, Anchors* aParent);

public slots:
	/** overridden from QGraphicsItem
	 *  if the user drags the object around, this even will be called for each pixel.
	 *  let's actually adjust the coordinates!!!
	 *
	 *  @param event the even to handle
	 */
	virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );

	/** overridden from QGraphicsItem
	 *  we want to know when the user clicks on this Anchor
	 *
	 *  @param event the even to handle
	 */
	virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );

	/** overridden from QGraphicsItem
	 *  we want to know when the user clicks on this Anchor
	 *
	 *  @param event the even to handle
	 */
	virtual void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event );

private:
	UndoObjectChange* theUndoResizePtr;

	// kill copy constructor & assignment operator
	ResizeAnchor(const ResizeAnchor&);
	const ResizeAnchor& operator=(const ResizeAnchor&);
};


/// RotateAnchor is a single item used by the Anchors class
class RotateAnchor : public Anchor
{
	Q_OBJECT

public:
	/// Constructor
	RotateAnchor(Anchors::AnchorType aDirection, AnchorPosition anIndex, Anchors* aParent);

public slots:
	/** overridden from QGraphicsItem
	 *  if the user drags the object around, this even will be called for each pixel.
	 *  let's actually adjust the coordinates!!!
	 *
	 *  @param event the even to handle
	 */
	virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );

	/** overridden from QGraphicsItem
	 *  we want to know when the user clicks on this Anchor
	 *
	 *  @param event the even to handle
	 */
	virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );

	/** overridden from QGraphicsItem
	 *  we want to know when the user clicks on this Anchor
	 *
	 *  @param event the even to handle
	 */
	virtual void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event );

private:
	UndoObjectChange* theUndoRotatePtr;

	float theHotspotAngle;

	float getCurrentAngle(Vector aHotspot) const;

	// kill copy constructor & assignment operator
	RotateAnchor(const RotateAnchor&);
	const RotateAnchor& operator=(const RotateAnchor&);
};


/// ButtonAnchor is a single item used by the Anchors class
class ButtonAnchor : public Anchor
{
	Q_OBJECT

public:
	/// Constructor
	ButtonAnchor(Anchors* aParent, const QString& anIconName, int aButtonIndex);

signals:
	void clicked(void);

public slots:
	/** overridden from QGraphicsItem
	 *  we want to know when the user clicks on this ButtonAnchor
	 *
	 *  @param event the even to handle
	 */
	virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );

private:
	// kill copy constructor & assignment operator
	ButtonAnchor(const ButtonAnchor&);
	const ButtonAnchor& operator=(const ButtonAnchor&);
};


#endif // ANCHORS_H