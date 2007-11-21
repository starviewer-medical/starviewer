/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGSYNCHRONIZETOOLDATA_H
#define UDGSYNCHRONIZETOOLDATA_H

#include <tooldata.h>

namespace udg {

class Q2DViewer;

/**
Classe per guardar les dades de la tool de sincronització.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class SynchronizeToolData : public ToolData
{
Q_OBJECT
public:
    SynchronizeToolData();

    ~SynchronizeToolData();

    int getIncrement();

public slots:

    /// Per canviar les dades de sincronitzacio
    void setIncrement( int value );

signals:

    /// Signat que s'emet quan les dades canvien
    void dataChanged();

private:

    /// Dades per la sincronització de l'slicing
    int m_increment;
};

}

#endif
