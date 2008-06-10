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

QList<Series *> Patient::getSelectedSeries()
{
    QList<Series *> selectedSeries;
    foreach( Study *study, m_studiesSet )
    {
        QList<Series *> list = study->getSelectedSeries();
        if( !list.empty() )
            selectedSeries << list;
    }
    return selectedSeries;
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

void Patient::patientFusionLogMessage( const Patient &patient )
{
    switch( compareTo( &patient ) )
    {
        case SamePatients:
            INFO_LOG("Fusionem dos pacients iguals: >>" + m_patientID + ":" + m_fullName + " >>" + patient.m_patientID + ":" + patient.m_fullName );
            break;
        case IndeterminableSimilarity:
            INFO_LOG("Fusionem dos pacients amb similitut indeterminable: >>" + m_patientID + ":" + m_fullName + " >>" + patient.m_patientID + ":" + patient.m_fullName );
            break;
        case DifferentPatients:
            INFO_LOG("Fusionem dos pacients diferents: >>" + m_patientID + ":" + m_fullName + " >>" + patient.m_patientID + ":" + patient.m_fullName );
            break;
    }
}

Patient Patient::operator +( const Patient &patient )
{
    Patient result;
    patientFusionLogMessage(patient);
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

    return result;
}

Patient Patient::operator +=( const Patient &patient )
{
    // deixem el criteri de fusionar en mans de qui "fusioni els pacients", de totes formes fem log per
    // poder fer estadístiques del tema de fusió
    // en tots casos, la informació del pacient "original" és la que preval i mai es matxaca.
    patientFusionLogMessage(patient);

    // recorrem els estudis que té "l'altre pacient" per afegir-los al resultat (aquesta mateixa instància) si no els té ja
    QList<Study *> studyListToAdd = patient.getStudies();
    QString uid;
    foreach( Study *study, studyListToAdd )
    {
        uid = study->getInstanceUID();
        if( !this->studyExists(uid) )
            this->addStudy( study ); //\TODO al tanto! potser hi ha problemes ja que l'addStudy li assigna el parentPatient! Potser caldria fer una copia de l'study
        else
        {
            // és el mateix estudi, però podria ser que tingués sèries noves
            // obtenim les series actuals
            QList<Series *> seriesList = study->getSeries();
            foreach( Series *series, seriesList )
            {
                // si la sèrie no existeix actualment, l'afegim
                if( !this->getStudy(uid)->seriesExists( series->getInstanceUID() ) )
                    this->getStudy(uid)->addSeries( series );
            }
        }
    }

    return this;
}

QString Patient::clearStrangeSymbols( QString patientName )
{
    return patientName.toUpper().replace(QRegExp("[^A-Z ^\\d]"), " ").trimmed();
}

QString Patient::clearPatientName( QString patientName )
{
    return patientName.toUpper().replace(QRegExp("[^A-Z]"), " ").trimmed();
}

bool Patient::containtsNumericalSymbols( QString patientName )
{
    QRegExp rx("\\d\\d?\\d?\\d?");
    return ( rx.indexIn( patientName ) != -1 );
}

Patient::PatientsSimilarity Patient::compareTo( const Patient *patient )
{
    //si tenen el mateix ID de pacient ja podem dir que són el mateix i no cal mirar res més.
    if( patient->m_patientID == this->m_patientID )
    {
        return SamePatients;
    }


    //Pre-tractament sobre el nom del pacient per treure caràcters extranys
    QString nameOfThis = clearStrangeSymbols( this->getFullName() );
    QString nameOfParameter = clearStrangeSymbols( patient->getFullName() );

    //tractament especial en el cas de que els noms continguin números
    if ( containtsNumericalSymbols( nameOfThis ) && containtsNumericalSymbols( nameOfParameter ) )
    {
        if ( nameOfThis == nameOfParameter )
        {
            return SamePatients;
        }
        else
        {
            return DifferentPatients;
        }
    }

    //si passem del condicional anterior és que algun o cap dels noms tenia dades numèriques; pertant fem el tractament normal.
    //mirem si tractant els caràcters extranys i canviant-los per espais són iguals. En aquest cas ja no cal mirar res més.
    if ( nameOfThis == nameOfParameter )
    {
        return SamePatients;
    }

    PatientsSimilarity namesSimilarity = metricToSimilarity(needlemanWunch2Distance( nameOfThis, nameOfParameter ));
    if (namesSimilarity != IndeterminableSimilarity)
    {
        return namesSimilarity; //si tenen molta similitud, retornem aquest valor
    }

    return metricToSimilarity( needlemanWunch2Distance(patient->m_patientID, this->m_patientID) );
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

Patient::PatientsSimilarity Patient::metricToSimilarity(double measure)
{
    if (measure < 0.25)
        return SamePatients;
    else if (measure < 0.31)
        return IndeterminableSimilarity;
    else
        return DifferentPatients;
}

}
