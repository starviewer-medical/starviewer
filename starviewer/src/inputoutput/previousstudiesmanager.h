/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGPREVIOUSSTUDIESMANAGER_H
#define UDGPREVIOUSSTUDIESMANAGER_H

#include <QObject>
#include <QHash>
#include "pacsdevice.h"

namespace udg {

class Study;

class PreviousStudiesManager : public QObject {
Q_OBJECT
public:

    PreviousStudiesManager();
    ~PreviousStudiesManager();

    /// Fa una consulta d'estudis previs assíncrona als PACS passats per paràmetre
    /// Si ja s'estigués executant una consulta la cancel·laria i faria la nova consulta
    void queryPreviousStudies(Study *study);

    /// Cancel·la les consultes actuals que s'estan executant, i cancel·la les consultes encuades per executar
    void cancelCurrentQuery();

    /// Indica si s'executen queries en aquest moment
    bool isExecutingQueries();

signals:

    /// Signal que s'emet quan ha finalitzat la consulta d'estudis previs
    void queryPreviousStudiesFinished( QList<Study*>, QHash<QString, QString> hashPacsIDOfStudyInstanceUID );

    /// Signal que s'emet per indicar que la query ha finalitzat
    void queryFinished();

    /// Signal que s'emet per indicar que s'ha produït un error a la consulta d'estudis d'un PACS
    void errorQueryingPreviousStudies(PacsDevice pacs);

};

}

#endif // UDGPREVIOUSSTUDIESMANAGER_H
