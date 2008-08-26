/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGTESTDATABASE_H
#define UDGTESTDATABASE_H

#include <QList>

#include "status.h"
#include "image.h"
#include "series.h"
#include "study.h"
#include "patient.h"

namespace udg {


/** Manager de la base de dades local, permet interactuar amb tots els objectes de la base de dades
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

class TestDatabase
{
public:

    static Patient* getPatientComplete();

    static Image* getImage();
    static Image* getImage2();

    static Series *getSeries();
    static Series *getSeries2();

    static Study *getStudy();
    static Study *getStudy2();

    static Patient* getPatient();
    static Patient* getPatient2();

private :


};

}

#endif
