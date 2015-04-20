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

#ifndef UDGMENUGRIDWIDGET_H
#define UDGMENUGRIDWIDGET_H

#include <QWidget>
#include "ui_qhangingprotocolswidgetbase.h"

namespace udg {

class HangingProtocol;

/**
 * @brief The QHangingProtocolsWidget class is a widget that shows three groups of hanging protocols consisting of a grid of icons and a caption for each group.
 *        The three groups are the hanging protocols for the current study, the ones for the prior study and the combined ones.
 */
class QHangingProtocolsWidget : public QWidget, private ::Ui::QHangingProtocolsWidgetBase {
Q_OBJECT
public:
    explicit QHangingProtocolsWidget(QWidget *parent = 0);
    virtual ~QHangingProtocolsWidget();

public slots:
    /// Sets the hanging protocol items to be displayed in each group. Previous items of each group are deleted.
    void setItems(const QList<HangingProtocol*> &combined, const QList<HangingProtocol*> &current, const QList<HangingProtocol*> &prior);
    /// Posa els hanging protocols que ha de representar el menú
    void setHangingItems(const QList<HangingProtocol*> &listOfCandidates);

    /// Marks the item corresponding to the given combined hanging protocol as active and the other combined items as inactive.
    /// If the parameter is null all combined items are marked as inactive.
    void setActiveCombinedHangingProtocol(HangingProtocol *HangingProtocol);
    /// Marks the item corresponding to the given current hanging protocol as active and the other current items as inactive.
    /// If the parameter is null all current items are marked as inactive.
    void setActiveCurrentHangingProtocol(HangingProtocol *hangingProtocol);
    /// Marks the item corresponding to the given prior hanging protocol as active and the other prior items as inactive.
    /// If the parameter is null all prior items are marked as inactive.
    void setActivePriorHangingProtocol(HangingProtocol *hangingProtocol);

signals:
    /// Emitted when a combined hanging protocol has been selected.
    void selectedCombined(int);
    /// Emitted when a hanging protocol for the current study has been selected.
    void selectedCurrent(int);
    /// Emitted when a hanging protocol for the prior study has been selected.
    void selectedPrior(int);
    /// Emet que s'ha escollit un grid
    void selectedGrid(int);

};

}

#endif
