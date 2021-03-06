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

#ifndef RESIZEAWAREQUICKWIDGET_H
#define RESIZEAWAREQUICKWIDGET_H

#include "Position.h"
#include "RequestDialog.h"
#include <QQuickWidget>

class ViewWorldItem;

class ResizeAwareQuickWidget : public QQuickWidget, public RequestDialog
{
    Q_OBJECT

public:
    ResizeAwareQuickWidget(QWidget *parent = Q_NULLPTR);

    /// This property describes the width of a handle in pixels
    Q_PROPERTY(int handleWidth MEMBER theHandleWidth NOTIFY handleWidthChanged)
    /// This property describes the height of a handle in pixels
    Q_PROPERTY(int handleHeight MEMBER theHandleHeight NOTIFY handleHeightChanged)

    /// Property describes the number of pixels per meter. This number changes
    /// if the user resizes the window.
    Q_PROPERTY(int pixPerMeter MEMBER thePixPerMeter NOTIFY pixPerMeterChanged)

    Q_PROPERTY(qreal aspectRatio READ aspectRatio NOTIFY aspectRatioChanged)
    qreal aspectRatio() const
    { return theWorldWidthInMeters / theWorldHeightInMeters; }

    Q_PROPERTY(int buttonHeight MEMBER theButtonHeight NOTIFY buttonHeightChanged)
    Q_PROPERTY(int buttonIconSize MEMBER theButtonIconSize NOTIFY buttonIconSizeChanged)


    ///
    bool setupQmlSource(const QUrl &url);

    // --------------------------------------- RequestDialog implementation
    // (only available through the RequestDialog interface!)
private:
    QQuickItem *showChooseLevel() override;
    QQuickItem *showChoosePhoneNumber(const QStringList& aSeriesOfNumbers) override;
    QQuickItem *showLevelInfo() override;
    QQuickItem *showWinFail(bool isWin) override;
    void setContextProperty(const QString &aName, const QVariant &aValue) override;

signals:
    void aspectRatioChanged();
    void buttonHeightChanged();
    void buttonIconSizeChanged();
    void handleWidthChanged();
    void handleHeightChanged();
    void pixPerMeterChanged();

public slots:
    /// Re-calculate all dimensions based on the new world sizes.
    /// Will emit various signals for changed dimensions.
    void updateWorldSize(qreal aWidthInMeter, qreal aHeightInMeter);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    int theHandleHeight;
    int theHandleWidth;
    int theButtonHeight;
    int theButtonIconSize;

    qreal theWorldWidthInMeters;
    qreal theWorldHeightInMeters;

    int thePixPerMeter;

    ViewWorldItem* theGameViewPtr;
};

#endif // RESIZEAWAREQUICKWIDGET_H
