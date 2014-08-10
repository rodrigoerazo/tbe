/* The Butterfly Effect
 * This file copyright (C) 2011,2014 Klaas van Gend
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

#include "AbstractObject.h"
#include "InsertUndoCommand.h"
#include "ListViewItemTooltip.h"
#include "Position.h"
#include "ViewObject.h"
#include "ui_ListViewItemTooltip.h"

ListViewItemTooltip::ListViewItemTooltip(ToolboxGroup *aTBGPtr,
                                         ResizingGraphicsView* aParent) :
    AnimatedDialog(aParent, AnimatedDialog::TOOLTIP),
    ui(new Ui::ListViewItemTooltip),
    theTBGPtr(aTBGPtr)
{
    ui->setupUi(this);
    Q_ASSERT(aTBGPtr);
    Q_ASSERT(aParent);

    // set the description and help
    ui->labelName->setText(theTBGPtr->theGroupName.result());
    ui->labelCount->setText(tr("%1x").arg(theTBGPtr->count()));
    AbstractObjectPtr myAOPtr = theTBGPtr->first();
    ui->labelDescription->setText(myAOPtr->getToolTip());

    // set the image
    ViewObject* myVOPtr = myAOPtr->createViewObject();

    // scale & rotate the image, map dimensions from scene to view
    qreal myBitmapConvertedWidth  = myAOPtr->getTheWidth() * THESCALE * aParent->transform().m11();
    qreal myBitmapConvertedHeight = myAOPtr->getTheHeight() * THESCALE * aParent->transform().m22();
    QSize myPixmapRegularSize(myBitmapConvertedWidth, myBitmapConvertedHeight);
    QPixmap myPixmap = myVOPtr->pixmap().scaled(myPixmapRegularSize, Qt::IgnoreAspectRatio);
    QTransform myTransform;
    myTransform.rotateRadians(-myAOPtr->getTempCenter().angle);
    QPixmap myFinalPixmap = myPixmap.transformed(myTransform);

    ui->buttonObjectImage->setMinimumSize(myFinalPixmap.size());
    ui->buttonObjectImage->setMaximumSize(myFinalPixmap.size());
    ui->buttonObjectImage->setIcon(myFinalPixmap);
    ui->buttonObjectImage->setIconSize(myFinalPixmap.size());

    // make it appear at the right height (next to the object that is clicked)
    // TODO: hardcoded Y coordinate for now...
    theYCoord = 50;
}


ListViewItemTooltip::~ListViewItemTooltip()
{

}


void ListViewItemTooltip::on_buttonRemove_clicked()
{
    setDeleteOnDisappear(true);
    disappearAnimated();
}



void ListViewItemTooltip::on_buttonObjectImage_clicked()
{
    // TODO: make the newly inserted object appear underneath where one
    // clicked the button...
    InsertUndoCommand::createInsertUndoCommand(theTBGPtr);
    emit disappearAnimated();
}