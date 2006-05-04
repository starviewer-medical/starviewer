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
#include <list>
#include "patientstudy.h"

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
    void setName( const char *name );
    void setName( QString name );
    QString getName();

    /// Assigna/Obté cognom del pacient
    void setSurname( const char *surname );
    void setSurname( QString surname );
    QString getSurname();
    
    /// Assigna/Obté data de naixement
    void setDateOfBirth( int day , int month , int year );
    QString getDateOfBirth();
    int getDayOfBirth();
    int getMonthOfBirth();
    int getYearOfBirth();

    /// Obté edat del pacient
    int getAge();

    /// Assigna/Obté sexe del pacient
    inline char getSex() const { return m_sex; };
    
    /// Assignar/Obtenir el pes del pacient
    void setWeight( double weight );
    double getWeight() const { return m_weight; };

    /// Assignar/Obtenir l'aclçada del pacient
    void setHeight( double height );
    double getHeight() const { return m_height; };
    
private:
    /// Identificador de pacient \TODO equival al tag DICOM ???? 
    QString m_patientID;

    /// Nom del pacient
    QString m_name;

    /// Cognom del pacient
    QString m_surname;

    /// Data de naixement
    QDate m_dateOfBirth;

    /// Sexe
    char m_sex;

    /// Pes ( en Kg. ) del pacient
    double m_weight;

    /// Alçada ( en metres ) del pacient
    double m_height;

    /// Llista d'estudis \TODO això podria ser un map o un hash, per determinar
    std::list< PatientStudy > m_studiesList;
};

}

#endif
