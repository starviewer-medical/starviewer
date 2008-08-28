/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGLOCALDATABASEMANAGER_H
#define UDGLOCALDATABASEMANAGER_H

#include <QList>
#include <QObject>

#include "status.h"
#include "image.h"
#include "series.h"
#include "study.h"
#include "patient.h"

namespace udg {

class DicomMask;
class DatabaseConnection;

/** Manager de la base de dades local, permet interactuar amb tots els objectes de la base de dades
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

class LocalDatabaseManager : public QObject
{
Q_OBJECT
public:
    ///Es defineix els tipus d'error que podem tenir, el DatabaseError indica quan és error de Sqlite
    enum LastError{Ok, DatabaseLocked, DatabaseCorrupted, SyntaxErrorSQL, DeletingFilesError, DatabaseError};

    ///Constructor de la classe
    LocalDatabaseManager();

    ///Ens retorna els pacients que compleixen amb els criteris de la màscara, només es té en compte el patientID
    QList<Patient*> queryPatient(DicomMask patientMaskToQuery);

    ///Ens retorna els estudis que compleixen amb els criteris de la màscara, només es té en compte l'StudyUID
    QList<Study*> queryStudy(DicomMask studyMaskToQuery);

    ///Ens retorna les series que compleixen amb els criteris de la màscara, només es té en compte l'StudyUID i SeriesUID 
    QList<Series*> querySeries(DicomMask seriesMaskToQuery);

    ///Ens retorna les imatges que compleixen amb els criteris de la màscara, només es té en compte l'StudyUID, SeriesUID i SOPInstanceUID
    QList<Image*> queryImage(DicomMask imageMaskToQuery);

    ///Ens retorna els pacients que tenen estudis que compleixen amb els criteris de la màscara. Té en compte el patientID, patient name, data de l'estudi i l'study instance UID
    QList<Patient*> queryPatientStudy(DicomMask patientStudyMaskToQuery);

    ///Retorna tota l'estructura Patient,Study,Series,Image, de l'estudi que compleix amb el criteri de cerca, té en compte el Study InstanceUID, el SeriesInstanceUID i el SOP Instance UID
    Patient* retrieve(DicomMask maskToRetrieve);

    ///Neteja totes les taules de la base de dades i esborra tots els estudis descarregats
    void clear();

    ///comptacta la base de dades
    void compact();

    LastError getLastError();

public slots:

    ///Inseriex un nou pacient a la base de dades
    void insert(Patient *newPatient);

private :

    LastError m_lastError;

    ///Guarda a la base de dades la llista d'estudis passada per paràmetre, si algun dels estudis ja existeix actualitza la info
    int saveStudies(DatabaseConnection *dbConnect, QList<Study*> listStudyToSave);
    ///Guarda a la base de dades la llista de series passada per paràmetre, si alguna de les series ja existeix actualitza la info
    int saveSeries(DatabaseConnection *dbConnect, QList<Series*> listSeriesToSave);
    ///Guarda a la base de dades la llista d'imatges passada per paràmetre, si alguna de les imatges ja existeix actualitza la info
    int saveImages(DatabaseConnection *dbConnect, QList<Image*> listImageToSave);

    ///Guarda el pacient a la base de dades, si ja existeix li actualitza la informació
    int savePatient(DatabaseConnection *dbConnect, Patient *patientToSave);
    ///Guarda el pacient a la base de dades, si ja existeix li actualitza la informació
    int saveStudy(DatabaseConnection *dbConnect, Study *studyToSave);
    ///Guarda el pacient a la base de dades, si ja existeix li actualitza la informació
    int saveSeries(DatabaseConnection *dbConnect, Series *seriesToSave);
    ///Guarda la imatge a la base de dades, si ja existeix li actualitza la informació
    int saveImage(DatabaseConnection *dbConnect, Image *imageToSave, int imageOrderInSeries);

    ///Aquesta classe s'encarrega d'esborrar les objectes descarregats si es produeix un error mentre s'insereixen els nous objectes a la base de dades
    void deleteRetrievedObjects(Patient *failedPatient);

    ///Passant un status de sqlite ens el converteix al nostra status
    void setLastError(int sqliteLastError);

};

}

#endif
