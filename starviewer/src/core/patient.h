/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGPATIENT_H
#define UDGPATIENT_H

#include <QObject>
#include <QString>
#include <QDate>
#include <QHash>
#include "study.h"

namespace udg {

/**
Classe que representa la unitat central de treball de l'aplicació: el Pacient. Guardarà tota la informació relacionada amb aquests ( Estudis, Sèries , etc )

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class Patient : public QObject
{
Q_OBJECT
public:
    Patient( QObject *parent = 0 );

    ~Patient();

    /// Assigna/Obté nom del pacient
    void setName( QString name );
    QString getName();

    /// Assigna/Obté cognom del pacient
    void setSurname( QString surname );
    QString getSurname();

    /// Assigna/Obté data de naixement
    void setBirthDate( int day , int month , int year );
    QString getBirthDate();
    int getDayOfBirth();
    int getMonthOfBirth();
    int getYearOfBirth();

    /// Assigna/Obté sexe del pacient
    inline char getSex() const { return m_sex; };

    /// Afegeix un nou estudi
    void addStudy( Study *study );

    /// Li treu al pacient l'estudi amb l'UID donat
    void removeStudy( QString uid );

    /// Obté l'estudi amb l'UID donat
    Study* getStudy( QString uid );

    /// Retorna cert si aquest pacient té assignat l'estudi amb l'UID donat, fals altrament
    bool studyExists( QString uid );

private:
    /// Informació comuna de pacient per a totes les imatges que fan referència a aquest pacient. Apartat C.7.1.1 PS 3.3 DICOM. Aquests són atributs del pacient necessaris per a interpretació diagnòstica de les imatges del pacient i són comunes per a tots els estudis realitzats en el pacient. \TODO de moment no incloem cap atribut opcional

    /// Nom complet del pacient. (0010,0010) Tipus 2.
    QString m_fullName;
    /// Nom del pacient
    QString m_name;
    /// Cognom del pacient
    QString m_surname;

    /// Identificador primari donada al pacient per l'hospital. (0010,0020) Tipus 2.
    QString m_patientID;

    /// Data de naixement. (0010,0030) Tipus 2.
    QDate m_birthDate;

    /// Sexe. (0010,0040) Tipus 2. M = male, F = female, O = other
    char m_sex;

    /// Indica si la identitat real del pacient s'ha tret dels atributs i de les dades. (0012,0062) Tipus 3. Considerem aquest paràmetre per si és necessari a l'hora d'anonimatitzar. Això ens obligaria a fer servir els tags (0012,0063) i (0012,0064) \TODO aquest atribut encara no es farà servir, però és per tenir-ho en compte per si calgués.
    bool m_identityIsRemoved;

    /// Taula de hash que conté els estudis del pacient
    typedef QHash< QString , Study* > StudiesHashType;
    StudiesHashType m_studiesHash;

};

}

#endif
