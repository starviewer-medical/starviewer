/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "patient.h"
#include "logging.h"

#include <QStringList>
#include <QChar>

namespace udg {

Patient::Patient(QObject *parent)
 : QObject( parent )
{
}

Patient::Patient( const Patient &patient, QObject *parent )
 : QObject( parent )
{
    m_fullName = patient.m_fullName;
    m_patientID = patient.m_patientID;
    m_birthDate = patient.m_birthDate;
    m_sex = patient.m_sex;
    m_identityIsRemoved = patient.m_identityIsRemoved;
    m_studiesSet = patient.m_studiesSet;
}


Patient::~Patient()
{
    m_studiesSet.clear();
}

void Patient::setFullName( QString name )
{
    m_fullName = name;
}

void Patient::setID( QString id )
{
    m_patientID = id;
}

void Patient::setBirthDate( int day , int month , int year )
{
    m_birthDate.setYMD( year , month , day );
}

void Patient::setBirthDate( QString date )
{
    m_birthDate = QDate::fromString( date.remove("."), "yyyyMMdd" );
}

QString Patient::getBirthDate()
{
    return m_birthDate.toString( Qt::LocaleDate );
}

int Patient::getDayOfBirth()
{
    return m_birthDate.day();
}

int Patient::getMonthOfBirth()
{
    return m_birthDate.month();
}

int Patient::getYearOfBirth()
{
    return m_birthDate.year();
}

void Patient::setSex( QString sex )
{
    m_sex = sex;
}

bool Patient::addStudy( Study *study )
{
    bool ok = true;
    QString uid = study->getInstanceUID();
    if( uid.isEmpty() )
    {
        ok = false;
        DEBUG_LOG("L'uid de l'estudi està buit! No el podem insertar per inconsistent");
    }
    else if( this->studyExists(uid) )
    {
        ok = false;
        DEBUG_LOG("Ja existeix un estudi amb aquest mateix UID:: " + uid );
    }
    else
    {
        study->setParentPatient( this );
        this->insertStudy( study );
    }

    return ok;
}

void Patient::removeStudy( QString uid )
{
    int index = this->findStudyIndex(uid);
    if( index != -1 )
        m_studiesSet.removeAt( index );
}

Study *Patient::getStudy( QString uid )
{
    int index = this->findStudyIndex(uid);
    if( index != -1 )
        return m_studiesSet[index];
    else
        return NULL;
}

bool Patient::studyExists( QString uid )
{
    if( this->findStudyIndex(uid) != -1 )
        return true;
    else
        return false;
}

int Patient::getNumberOfStudies()
{
    return m_studiesSet.size();
}

QList<Study *> Patient::getStudies() const
{
    return m_studiesSet;
}

Series *Patient::getSeries( QString uid )
{
    Series *result = NULL;
    foreach( Study *study, m_studiesSet )
    {
        result = study->getSeries( uid );
        if( result )
            break;
    }
    return result;
}

bool Patient::hasFile( QString filename )
{
    QList<Study *> studyList = this->getStudies();
    foreach( Study *study, studyList )
    {
        QList<Series *> seriesList = study->getSeries();
        foreach( Series *series, seriesList )
        {
            if( series->getFilesPathList().contains( filename ) )
                return true;
        }
    }
    return false;
}

Patient & Patient::operator =( const Patient &patient )
{
    m_fullName = patient.m_fullName;
    m_patientID = patient.m_patientID;
    m_birthDate = patient.m_birthDate;
    m_sex = patient.m_sex;
    m_identityIsRemoved = patient.m_identityIsRemoved;
    m_studiesSet = patient.m_studiesSet;
    return *this;
}

Patient Patient::operator +( const Patient &patient )
{
    Patient result;
    if( this->isSamePatient( &patient ) )
    {
        // copiem informació estructural en el resultat
        result.copyPatientInformation( &patient );
        result.m_studiesSet = this->m_studiesSet;

        // ara recorrem els estudis que té "l'altre pacient" per afegir-los al resultat si no els té ja
        QList<Study *> studyListToAdd = patient.getStudies();
        QString uid;
        foreach( Study *study, studyListToAdd )
        {
            uid = study->getInstanceUID();
            if( !result.studyExists(uid) )
                result.addStudy( study ); //\TODO al tanto! potser hi ha problemes ja que l'addStudy li assigna el parentPatient! Potser caldria fer una copia de l'study
        }
    }
    else
    {
        DEBUG_LOG("Els pacients no es poden fusionar perquè no s'identifiquen com el mateix");
    }
    return result;
}

Patient Patient::operator +=( const Patient &patient )
{
    // si coincideix nom o ID llavors es poden fusionar TODO mirar de definir aquest criteri
    if( isSamePatient( &patient ) )
    {
        // recorrem els estudis que té "l'altre pacient" per afegir-los al resultat (aquesta mateixa instància) si no els té ja
        QList<Study *> studyListToAdd = patient.getStudies();
        QString uid;
        foreach( Study *study, studyListToAdd )
        {
            uid = study->getInstanceUID();
            if( !this->studyExists(uid) )
                this->addStudy( study ); //\TODO al tanto! potser hi ha problemes ja que l'addStudy li assigna el parentPatient! Potser caldria fer una copia de l'study
        }
    }
    else
    {
        DEBUG_LOG("Els pacients no es poden fusionar perquè no comparteixen ni nom ni ID");
    }
    return this;
}

Patient Patient::operator -( const Patient &patient )
{
    // TODO implementa'm!
    DEBUG_LOG("Mètode per implementar");
}

Patient Patient::operator -=( const Patient &patient )
{
    // TODO implementa'm!
    DEBUG_LOG("Mètode per implementar");
}

QString Patient::patientNameTreatment( QString patientName )
{
    QString name = patientName;
    
    name = name.replace(QString("^"), QString(" "));
    name = name.replace(QString("."), QString(" "));
    name = name.replace(QString(","), QString(" "));
    name = name.replace(QString("-"), QString(" "));
    name = name.replace(QString(";"), QString(" "));
    name = name.replace(QString("_"), QString(" "));
    
    name = name.trimmed();
    
    name = name.toUpper();
    
    return( name );
}

bool Patient::isSamePatient( const Patient *patient )
{
    //si tenen el mateix ID de pacient ja podem dir que són el mateix i no cal mirar res més.
    bool isSame = ( patient->m_patientID == this->m_patientID );
    
    if ( !isSame )
    {
        //Pre-tractament sobre el nom del pacient per treure caràcters extranys
        QString nameOfThis = patientNameTreatment( this->getFullName() );
        QString nameOfParameter = patientNameTreatment( patient->getFullName() );
        
        //mirem si tractant els caràcters extranys i canviant-los per espais són iguals. En aquest cas ja no cal mirar res més.
        isSame = ( nameOfThis == nameOfParameter );
        
//         if ( !isSame )
//         {
//             int distance = LevenshteinDistance( nameOfThis, nameOfParameter );
//         }
    }  
      
    return ( isSame );
}

QString Patient::toString()
{
    QString result;

    result += "- Patient\n";
    result += "    FullName : " + getFullName() + "\n";
    result += "    ID : " + getID() + "\n";

    foreach (Study *study, getStudies())
    {
        result += study->toString();
    }

    return result;
}

void Patient::copyPatientInformation( const Patient *patient )
{
    this->m_fullName = patient->m_fullName;
    this->m_patientID = patient->m_patientID;
    this->m_birthDate = patient->m_birthDate;
    this->m_sex = patient->m_sex;
}

void Patient::insertStudy( Study *study )
{
    int i = 0;
    while( i < m_studiesSet.size() && m_studiesSet.at(i)->getDateTime() < study->getDateTime() )
    {
        i++;
    }
    m_studiesSet.insert( i, study );
}

int Patient::findStudyIndex( QString uid )
{
    int i = 0;
    bool found = false;
    while( i < m_studiesSet.size() && !found )
    {
        if( m_studiesSet.at(i)->getInstanceUID() == uid )
            found = true;
        else
            i++;
    }
    if( !found )
        i = -1;

    return i;
}

double Patient::needlemanWunchDistance (QString s, QString t, int gap ) 
{
    int n = s.length(); 
    int m = t.length(); 
        
    if (n == 0) return 1.;
    else if (m == 0) return 1.;

    int *p = new int[n+1]; 
    int *d = new int[n+1]; 
    int i, j, cost; 

    QChar t_j; 

    for (i = 0; i<=n; i++) p[i] = i;
    
    for (j = 1; j<=m; j++) {
        t_j = t.at(j-1);
        d[0] = j;
        
        for (i=1; i<=n; i++) {
            cost = s.at(i-1)==t_j ? 0 : 1;
                
            d[i] = qMin( qMin( d[i-1]+gap, p[i]+gap),  p[i-1]+cost );  
        }
        
        qSwap( p, d );
    } 

    int min = qMin( n, m );
    int diff = qMax( n, m ) - min;
    return (double)p[n] / (double)(min + diff*gap);
}


double Patient::needlemanWunch2Distance( QString s, QString t ) 
{
    return needlemanWunchDistance( s, t, 2 );
}

double Patient::levenshteinDistance( QString s, QString t) 
{
    return needlemanWunchDistance( s, t, 1 );
}
}
