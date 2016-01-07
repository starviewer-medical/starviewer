/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#ifndef QFUSIONBALANCEWIDGET_H
#define QFUSIONBALANCEWIDGET_H

#include <QWidget>

class QBoxLayout;
class QLabel;
class QSlider;

namespace udg {

/**
    This widget provides controls to modify the fusion balance of between two volumes.
    The fusion balance is represented as an integer in the range [0, 100].
    It has labels for each volume and a slider in between. It may be oriented horizontally (the default) or vertically.
 */
class QFusionBalanceWidget : public QWidget {

    Q_OBJECT

public:
    /// Creates the widget with empty labels, the slider with a value of 50 (the middle) and an horizontal orientation.
    QFusionBalanceWidget(QWidget *parent = 0);
    virtual ~QFusionBalanceWidget();

    /// Sets the modality label of the first volume.
    void setFirstVolumeModality(const QString &modality);
    /// Sets the modality label of the second volume.
    void setSecondVolumeModality(const QString &modality);

    /// Returns the current balance value.
    int getBalance() const;

public slots:
    /// Orients the widget according to the given orientation.
    void setOrientation(Qt::Orientation orientation);
    /// Orients the widget horizontally.
    void setHorizontalOrientation();
    /// Orients the widget vertically.
    void setVerticalOrientation();
    /// Toggles the orientation of the widget.
    void toggleOrientation();

    /// Sets the balance to the given value.
    void setBalance(int balance);

signals:
    /// This signal is emitted when the balance is changed.
    void balanceChanged(int balance);

private slots:
    /// Updates the percent labels of both volumes according to the given balance value.
    void updatePercentLabels(int balance);

private:
    /// Main layout.
    QBoxLayout *m_layout;

    /// The slider used to change the balance value.
    QSlider *m_slider;

    /// Modality label of the first volume.
    QLabel *m_firstVolumeLabel;
    /// Percent label of the first volume.
    QLabel *m_firstVolumePercentLabel;

    /// Modality label of the second volume.
    QLabel *m_secondVolumeLabel;
    /// Percent label of the second volume.
    QLabel *m_secondVolumePercentLabel;

};

}

#endif
