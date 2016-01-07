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

#ifndef UDGVOLUMEBUILDER_H
#define UDGVOLUMEBUILDER_H

#include <QString>

namespace udg {

class Volume;

/**
    Classe base per la generació de nous objectes Volume que contindrà un conjunt d'imatges assignades a una nova sèrie
    i aquesta a un estudi (nou o no depenén del tipus de generador).

    Es dóna la possiblitat d'assignar una descripció a la nova sèrie que es generarà (\sa setSeriesDescription()).
  */
class VolumeBuilder {
public:
    VolumeBuilder();

    virtual ~VolumeBuilder();

    /// Mètode encarregat de generar el nou Volume.
    virtual Volume* build() = 0;

    /// Assignar/Obtenir la descripció que es vol assignar a la nova sèrie que es generarà.
    void setSeriesDescription(QString description);
    QString getSeriesDescription() const;

protected:
    QString m_seriesDescription;
};

};  // End namespace udg

#endif // VOLUMEBUILDER_H
