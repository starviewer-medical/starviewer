/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGLOCALDATABASESTUDY_H
#define UDGLOCALDATABASESTUDY_H

#include <QList>

#include "localdatabasebasedal.h"
#include "study.h"

namespace udg {

class DicomMask;

/** Aquesta classe s'encarrega de dur a terme les operacions relacionades amb l'objecte estudi de la cache de l'aplicació.
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class LocalDatabaseStudyDAL: public LocalDatabaseBaseDAL
{
public:

    LocalDatabaseStudyDAL(DatabaseConnection *dbConnection);

    ///insereix el nou estudi, i insereix com LastAccessDate la data actual
    void insert(Study *newStudy, const QDate &lastAccessData);

    ///updata l'estudi 
    void update(Study *studyToUpdate, const QDate &LastAcessDate);

    ///Esborra els estudis que compleixen amb els criteris de la màscara de cerca, només té en compte l'StudyUID
    void del(const DicomMask &studyMaskToDelete);

    ///Cerca les estudis que compleixen amb els criteris de la màscara de cerca, només té en compte l'StudyUID, retorna els estudis ordenats per LastAccessDate de manera creixent
    QList<Study*> queryOrderByLastAccessDate(const DicomMask &studyMaskToQuery, QDate lastAccessDateMinor = QDate(), QDate lastAccessDateEqualOrMajor = QDate() );

    ///Cerca les estudis que compleixen amb els criteris de la màscara de cerca, només té en compte l'StudyUID
    QList<Study*> query(const DicomMask &studyMaskToQuery, QDate lastAccessDateMinor = QDate(), QDate lastAccessDateEqualOrMajor = QDate() );

    ///Ens retorna els pacients que tenen estudis que compleixen amb els criteris de la màscara. Té en compte el patientID, patient name, data de l'estudi i l'study instance UID
    QList<Patient*> queryPatientStudy(const DicomMask &patientStudyMaskToQuery, QDate lastAccessDateMinor = QDate(), QDate lastAccessDateEqualOrMajor = QDate());

    ///Ens retorna quan estudis té un determinat pacient
    int countHowManyStudiesHaveAPatient(const QString &patientID);

private :

    ///Construeix la sentència sql per inserir el nou estudi
    QString buildSqlInsert(Study *newStudy, const QDate &lastAcessDate);

    ///Construeix la sentència updata l'estudi
    QString buildSqlUpdate(Study *studyToUpdate, const QDate &lastAccessDate);

    ///Construeix la setència per fer select d'estudis a partir de la màscara, només té en compte el StudyUID i els estudis que tinguin un LastAccessDate menor que el de la màscara
    QString buildSqlSelect(const DicomMask &studyMaskToSelect, const QDate &lastAccessDateMinor, const QDate &lastAccessDateEqualOrMajor);

    ///Construeix la setència per esborrar l'estudi a partir de la màscara, només té en compte el StudyUID
    QString buildSqlDelete(const DicomMask &studyMaskToDelete);

    ///Construeix la sentència per fer select d'estudi i pacients a partir de la màscara. Té en compte studyUID, Patient Id, Patient Name, i data de l'estudi
    QString buildSqlSelectStudyPatient(const DicomMask &studyMaskToSelect, const QDate &lastAccessDateMinor, const QDate &lastAccessDateEqualOrMajor);

    ///Construeix la sentència per comptar quants estudis té un pacient
    QString buildSqlCountHowManyStudiesHaveAPatient(const QString &patientID);

    ///Emplena un l'objecte Study de la fila passada per paràmetre
    Study* fillStudy(char **reply, int row, int columns);

    ///Emplena un objecte Patient a partir de la fila passada per paràmetre
    Patient* fillPatient(char **reply, int row, int columns);
};
}

#endif
