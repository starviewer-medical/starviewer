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

#ifndef UDGHANGINGPROTOCOLLAYOUT_H
#define UDGHANGINGPROTOCOLLAYOUT_H

#include <QStringList>

namespace udg {

class HangingProtocolLayout {

public:
    HangingProtocolLayout();

    ~HangingProtocolLayout();

    /// Posar el nombre de pantalles
    void setNumberOfScreens(int numberOfScreens);
    /// Posar la llista de píxels en vertical
    void setVerticalPixelsList(const QList<int> &verticalPixelsList);
    /// Posar la llista de píxels en horitzontal
    void setHorizontalPixelsList(const QList<int> &horizontalPixelsList);
    /// Posar la llista de posicions dels visualitzadors
    void setDisplayEnvironmentSpatialPositionList(const QStringList &displayEnvironmentSpatialPosition);
    /// Obtenir el nombre de pantalles
    int getNumberOfScreens() const;
    /// Obtenir la llista de píxels en vertical
    QList<int> getVerticalPixelsList() const;
    /// Obtenir la llista de píxels en horitzontal
    QList<int> getHorizontalPixelsList() const;
    /// Obtenir la llista de posicions dels visualitzadors
    QStringList getDisplayEnvironmentSpatialPositionList() const;

private:
    /// Hanging Protocol Environment Module Attributes
    /// Nombre de pantalles
    int m_numberOfScreens;
    /// Llista de píxels en vertical
    QList<int> m_verticalPixelsList;
    /// Llista de píxels en horitzontal
    QList<int> m_horizontalPixelsList;
    /// Llista de posicions dels visualitzadors
    QStringList m_displayEnvironmentSpatialPosition;
};

}

#endif
