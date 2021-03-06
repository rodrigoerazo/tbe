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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA.
 */

#include "tbe_global.h"
#include "SaveLevelInfo.h"
#include "Popup.h"
#include <QFileDialog>

SaveLevelInfo::SaveLevelInfo(Level *aLevelPtr, QWidget *parent)
    : QDialog(parent), theLevelPtr(aLevelPtr)
{
    isUserOKOverwritingFile = false;

    ui.setupUi(this);
    ui.theFileNameField        ->setText(theLevelPtr->getLevelFileName());
    ui.theAuthorNameField      ->setText(theLevelPtr->theLevelAuthor);
    ui.theLevelDescriptionField->setText(theLevelPtr->theLevelDescription);
    ui.theLicenseField         ->setText(theLevelPtr->theLevelLicense);
    ui.theTitleField           ->setText(theLevelPtr->theLevelName);

    QDate myDate = QDate::fromString(theLevelPtr->theLevelDate);
    if (myDate.isValid())
        ui.theDateEdit->setDate(myDate);
    else {
        DEBUG5("SaveLevelInfo::SaveLevelInfo - Date '%s' is not understood, setting to current date",
               ASCII(theLevelPtr->theLevelDate));
        ui.theDateEdit->setDate(QDate::currentDate());
    }

    connect(ui.theFileNameField, SIGNAL(textEdited(const QString &)), this, SLOT(fileNameChanged()));
}


SaveLevelInfo::~SaveLevelInfo()
{
}



void SaveLevelInfo::accept()
{
    if (performFileExists(ui.theFileNameField->text())) {
        commitToLevel();
        QDialog::accept();
    }
}


bool SaveLevelInfo::commitToLevel(void)
{
    theLevelPtr->theLevelAuthor      = ui.theAuthorNameField->text().trimmed();
    theLevelPtr->theLevelDate        = ui.theDateEdit->text().trimmed();
    theLevelPtr->theLevelLicense     = ui.theLicenseField->text().trimmed();
    theLevelPtr->theLevelDescription = ui.theLevelDescriptionField->toPlainText().trimmed();
    theLevelPtr->theLevelName        = ui.theTitleField->text().trimmed();

    if (ui.theFileNameField->text().isEmpty())
        return false;
    else
        theLevelPtr->setLevelFileName(ui.theFileNameField->text());

    if (ui.theTitleField->text().isEmpty())
        return false;
    if (ui.theAuthorNameField->text().isEmpty())
        return false;

    return true;
}


void SaveLevelInfo::FileDialogButton_clicked()
{
    // use QFileInfo to brush up the file name
    QFileInfo myFileInfo(ui.theFileNameField->text());

    isUserOKOverwritingFile = false;

    QString myFileName = QFileDialog::getSaveFileName(this,
                                                      tr("Save Level"), myFileInfo.absoluteFilePath(), tr("TBE levels (*.tbe *.xml)"));

    if (performFileExists(myFileName) == false)
        return;

    ui.theFileNameField->setText(myFileName);
}


bool SaveLevelInfo::performFileExists(const QString &aFileName)
{
    if (isUserOKOverwritingFile == true)
        return true;

    if (QFile::exists(aFileName)) {
        DEBUG5("File '%s' already exists!", ASCII(aFileName) );
        if (Popup::YesNoQuestion(tr("A File with name '%1' file already exists. Overwrite?\n").arg(
                                     aFileName),
                                 this) == false)
            return false;
        isUserOKOverwritingFile = true;
    }
    return true;
}
