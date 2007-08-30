/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGMHDFILECLASSIFIERSTEP_H
#define UDGMHDFILECLASSIFIERSTEP_H

#include "patientfillerstep.h"

namespace udg {

/**
Classificador d'arxius mhd. Crea tota l'estructura de pacient i en un estudi coloca tantes series com volums tinguem

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class MHDFileClassifierStep : public PatientFillerStep
{
public:
    MHDFileClassifierStep();

    ~MHDFileClassifierStep();

    bool fill();

    QString name() {  return "MHDFileClassifierStep";  }

private:
    /// Donat un arxiu el classifica dins de pacient.
    /// Retorna fals si no el pot classificar, és a dir, no és un arxiu que es pugui parsejar com a MHD, cert altrament.
    bool classifyFile( QString file );

};

}

#endif
