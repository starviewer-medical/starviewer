/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDICOMDIRIMPORTER_H
#define UDGDICOMDIRIMPORTER_H

#include <QObject>

#include "dicomdirreader.h"

class QString;

namespace udg {

class Status;
class DICOMImage;
class DICOMTagReader;

/** Aquesta classe permet importar un dicomdir a la nostra base de dades.
    Només suporta importar dades d'un sol pacient a cada crida, per tant,
    cal assegurar-se que se li passa un studyUID correcte.
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class DICOMDIRImporter : QObject {
    Q_OBJECT

public:
    ///Importa les dades del dicomdir que es trova a dicomdirPath que pertanyen a l'study amb UID studyUID
    Status import( QString dicomdirPath, QString studyUID, QString seriesUID, QString imageUID );

signals:
    ///Senyal que ens indica que s'ha importat una imatge a disc. Quan s'emet aquest senyal encara no s'ha guardat a la bd.
    void imageImportedToDisk(DICOMTagReader *dicomTagReader);

private:
    DICOMDIRReader m_readDicomdir;

    Status importStudy(QString studyUID, QString seriesUID, QString sopInstanceUID);

    Status importSeries(QString studyUID, QString seriesUID, QString sopInstanceUID);

    Status importImage(DICOMImage image);
};

}
#endif
