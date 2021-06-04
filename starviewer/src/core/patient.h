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

#ifndef UDGPATIENT_H
#define UDGPATIENT_H

#include <QObject>
#include <QString>
#include <QDate>
#include "study.h"

namespace udg {

/**
    Classe que representa la unitat central de treball de l'aplicació: el Pacient.
    Guardarà tota la informació relacionada amb aquests (Estudis, Sèries, etc.)
  */
class Patient : public QObject {
Q_OBJECT
public:
    /// Enumeració dels graus de similitud
    enum PatientsSimilarity { SamePatients, DifferentPatients, SamePatientIDsDifferentPatientNames, SamePatientNamesDifferentPatientIDs };

    Patient(QObject *parent = 0);

    /// Constructor per còpia, necessari pels operadors +, +=, -, -=
    Patient(const Patient &patient, QObject *parent = 0);
    ~Patient();

    /// Assigna/Obté el nom complet del pacient
    void setFullName(const QString &name);
    QString getFullName() const;

    /// Assigna/Obté l'ID del pacient
    void setID(const QString &id);
    QString getID() const;

    /// Assigna/Obté el ID de pacient a la BD d'Starviewer
    void setDatabaseID(qlonglong databaseID);
    qlonglong getDatabaseID() const;

    /// Assigna/Obté data de naixement
    void setBirthDate(int day, int month, int year);
    void setBirthDate(const QString &date);
    QString getBirthDateAsString();
    QDate getBirthDate() const;
    int getDayOfBirth();
    int getMonthOfBirth();
    int getYearOfBirth();

    /// Assigna/Obté sexe del pacient
    void setSex(const QString &sex);
    QString getSex() const;

    /// Afegeix un nou estudi. A l'estudi se li assigna com a "parentPatient" aquest Patient.
    /// Retorna fals si existeix un estudi ja amb el mateix uid o l'uid és buit.
    bool addStudy(Study *study);

    /// Li treu al pacient l'estudi amb l'UID donat
    void removeStudy(const QString &uid);

    /// Obté l'estudi amb l'UID donat. Si no n'hi ha cap amb aquest uid retorna nul
    Study* getStudy(const QString &uid);

    /// Retorna cert si aquest pacient té assignat l'estudi amb l'UID donat, fals altrament
    bool studyExists(const QString &uid);

    /// Mètode per obtenir el nombre d'estudis del pacient
    int getNumberOfStudies();

    /// Mètode per obtenir la llista d'estudis del pacient. Per defecte ens la retorna ordenada amb els estudis més recents primer.
    QList<Study*> getStudies(Study::StudySortType sortCriteria = Study::RecentStudiesFirst) const;

    /// Ens retorna una llista amb els estudis del pacient que continguin la modalitat donada. La llista d'estudis no està en cap ordre concret.
    QList<Study*> getStudiesByModality(const QString &modality);
    
    /// Returns a string list with its studies modalities
    QStringList getModalities() const;
    
    /// Mètode ràpid per trobar si hi ha una series amb el uid donat. Retorna nul si aquesta no existeix
    Series* getSeries(const QString &uid);

    /// Ens retorna una llista amb les sèries que estiguin seleccionades
    QList<Series*> getSelectedSeries();
    
    /// Returns a list of volumes that compose the patient.
    QList<Volume*> getVolumesList();

    /// Returns the number of volumes that compose the patient.
    int getNumberOfVolumes();

    /// Ens diu si aquest arxiu pertany a alguna series del pacient
    bool hasFile(const QString &filename);

    /// Operador d'assignació
    Patient& operator =(const Patient &patient);

    /// Operador de fusió de pacient. Amb aquest operador podrem unificar en un sol objecte Patient la informació de dos Patient
    /// sempre que siguin identificats com a el mateix pacient. Això seria equivalent a un operador de Unió de conjunts
    /// TODO aquí caldria rumiar-se si es pot forçar la fusió encara que siguin pacients diferents aparentment.
    Patient operator+(const Patient &patient);
    Patient operator+=(const Patient &patient);

    /// Retorna si es considera que es pot identificar com al mateix pacient (a partir de l'ID i el nom).
    /// No compara ni els estudis ni les sèries que conté, únicament la identificació de pacient.
    Patient::PatientsSimilarity compareTo(const Patient *patient) const;

    /// Volca informació sobre el pacient en un string útil per tasques de debug
    /// TODO Incorporar paràmetre "verbose" per poder escollir si fer un volcat complet o no
    QString toString() const;

    /// Marca com a seleccionada la sèrie amb l'UID indicat
    /// En cas que no existeixi tal UID, se selecciona la primera
    /// sèrie del primer estudi
    void setSelectedSeries(const QString &selectedSeriesUID);

signals:
    /// Emitted when a study has been added
    void studyAdded(Study*);

    /// Emitted when a study has been removed
    void studyRemoved(Study*);

private:
    /// Donat un pacient, copia únicament la informació del pacient. No fa res respecte els estudis,series o imatges.
    /// La informació es matxaca i no es fa cap mena de comprovació.
    void copyPatientInformation(const Patient *patient);

    /// Inserta un estudi a la llista d'estudis ordenat per data. Els estudis més recents primer.
    /// Pre: se presuposa que s'ha comprovat anteriorment que l'estudi no existeix a la llista
    void insertStudy(Study *study);

    /// Troba l'índex de l'estudi amb l'uid donat a la llista d'estudis
    /// @param uid L'uid d'estudi que volem trobar
    /// @return L'índex d'aquell estudi dins de la llista, -1 si no existeix l'estudi amb aquell uid.
    int findStudyIndex(const QString &uid);

    /// Crea un missatge de log per saber quins pacients estem fusionant
    /// @param patient Les dades del pacient que es fusiona amb aquest
    void patientFusionLogMessage(const Patient &patient);

    // Returns the patient name without non-alphanumeric characters, with spaces at the beginning and at the end trimmed, and converted to uppercase.
    QString getCleanName() const;

private:
    /// Informació comuna de pacient per a totes les imatges que fan referència a aquest pacient. Apartat C.7.1.1 PS 3.3 DICOM.
    /// Aquests són atributs del pacient necessaris per a interpretació diagnòstica de les imatges del pacient i són comunes per
    /// a tots els estudis realitzats en el pacient. \TODO de moment no incloem cap atribut opcional

    /// Nom complet del pacient. (0010,0010) Tipus 2.
    QString m_fullName;

    /// Identificador primari donada al pacient per l'hospital. (0010,0020) Tipus 2.
    QString m_patientID;

    /// ID de pacient a la bd d'Starviewer
    qlonglong m_databaseID;

    /// Data de naixement. (0010,0030) Tipus 2.
    QDate m_birthDate;

    /// Sexe. (0010,0040) Tipus 2. M = male, F = female, O = other
    QString m_sex;

    /// Indica si la identitat real del pacient s'ha tret dels atributs i de les dades. (0012,0062) Tipus 3. Considerem
    /// aquest paràmetre per si és necessari a l'hora d'anonimatitzar. Això ens obligaria a fer servir els tags (0012,0063) i (0012,0064)
    // TODO Aquest atribut encara no es farà servir, però és per tenir-ho en compte per si calgués.
    bool m_identityIsRemoved;

    /// Llista que conté els estudis del pacient ordenats per data
    QList<Study*> m_studiesList;
};

}

#endif
