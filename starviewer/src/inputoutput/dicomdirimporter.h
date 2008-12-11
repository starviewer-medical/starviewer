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
#include <QProgressDialog>

class QString;

namespace udg {

class DICOMImage;
class DICOMTagReader;
class PatientFiller;
class LocalDatabaseManagerThreaded;
class QThreadRunWithExec;

/** Aquesta classe permet importar un dicomdir a la nostra base de dades.
    Només suporta importar dades d'un sol pacient a cada crida, per tant,
    cal assegurar-se que se li passa un studyUID correcte.
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class DICOMDIRImporter : QObject {
    Q_OBJECT

public:

    enum DICOMDIRImporterError {Ok, DatabaseError, NoEnoughSpace, ErrorFreeingSpace, ErrorCopyingFiles, PatientInconsistent, ErrorOpeningDicomdir, DicomdirInconsistent};

    ///Importa les dades del dicomdir que es trova a dicomdirPath que pertanyen a l'study amb UID studyUID
    void import( QString dicomdirPath, QString studyUID, QString seriesUID, QString imageUID );

    ///Retorna l'últim error produït al importar el dicomdir
    DICOMDIRImporterError getLastError();

signals:
    ///Senyal que ens indica que s'ha importat una imatge a disc. Quan s'emet aquest senyal encara no s'ha guardat a la bd.
    void imageImportedToDisk(DICOMTagReader *dicomTagReader);

    void importFinished();
    void importAborted();

private:
    DICOMDIRReader m_readDicomdir;
    DICOMDIRImporterError m_lastError;
    QProgressDialog *m_qprogressDialog;

    ///crea les connexions necessàries per importar dicomdir
    void createConnections(PatientFiller *patientFiller, LocalDatabaseManagerThreaded *localDatabaseManagerThreaded, QThreadRunWithExec *fillersThread); 

    void importStudy(QString studyUID, QString seriesUID, QString sopInstanceUID);

    void importSeries(QString studyUID, QString seriesUID, QString sopInstanceUID);

    void importImage(DICOMImage image);

    ///Copia al disc dur una imatge del dicomdir
    bool copyDicomdirImageToLocal(QString dicomdirImagePath, QString localImagePath);
};

}
#endif
