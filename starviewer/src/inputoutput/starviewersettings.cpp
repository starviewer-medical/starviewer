/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "starviewersettings.h"

#include <QDir>
#include <QApplication>
#include <QString>

#include "logging.h"


const QString databaseRootKey("/cache/sdatabasePath" ); //indica on es troba la bd
const QString poolSizeKey("/cache/poolSize" );
const QString cacheImagePathKey("/cache/imagePath" );
const QString AETitleMachineKey("/pacsparam/AETitle" );
const QString localPortKey("/pacsparam/localPort" );
const QString timeoutPacsKey("/pacsparam/timeout" );
const QString maxConnectionsKey("/pacsparam/MaxConnects" );
const QString pacsColumnWidthKey("/interface/studyPacsList/columnWidth" );//en aquesta clau a darrera s'hi concatena el número de columna ,  per diferenciar cada columna
const QString cacheColumnWidthKey("/interface/studyCacheList/columnWidth" );//en aquesta clau a darrera s'hi concatena el número de columna ,  per diferenciar cada columna
const QString dicomdirColumnWidthKey("/interface/studyDicomdirList/columnWidth" );//en aquesta clau a darrera s'hi concatena el número de columna ,  per diferenciar cada columna
const QString MaximumDaysNotViewedStudy("/cache/MaximumDaysNotViewedStudy" );
const QString InstitutionName("/institution/name");
const QString InstitutionAddress("/institution/address");
const QString InstitutionTown("/institution/town");
const QString InstitutionZipCode("/institution/zipCode");
const QString InstitutionCountry("/institution/country");
const QString InstitutionEmail("/institution/Email");
const QString InstitutionPhoneNumber("/institution/phoneNumber");
const QString queryScreenWindowPositionX("/interface/queryscreen/windowPositionX");
const QString queryScreenWindowPositionY("/interface/queryscreen/windowPositionY");
const QString queryScreenWindowWidth("/interface/queryscreen/windowWidth");
const QString queryScreenWindowHeight("/interface/queryscreen/windowHeigth");
const QString queryScreenStudyTreeSeriesListQSplitterState("/interface/queryscreen/StudyTreeSeriesListQSplitterState");
const QString logsPacsCommunicationModeVerbose("/logs/pacsCommunicationmodeVerbose");
const QString qOperationStateColumnWidthKey("/interface/qOperationState/columnWidth" );//en aquesta clau a darrera s'hi concatena el número de columna ,  per diferenciar cada columna
const QString qCreateDicomdirColumnWidthKey("/interface/qCreateDicomdir/columnWidth" );//en aquesta clau a darrera s'hi concatena el número de columna ,  per diferenciar cada columna
const QString qConfigurationPacsDeviceColumnWidthKey("/interface/qConfigurationPacsDevice/columnWidth" );//en aquesta clau a darrera s'hi concatena el número de columna ,  per diferenciar cada columna

namespace udg {

StarviewerSettings::StarviewerSettings()
    :GroupSettingsName("PACS")
{
}

void StarviewerSettings::setDatabasePath( QString path )
{
    m_starviewerSettings.setValue( GroupSettingsName + databaseRootKey , path );
}

void StarviewerSettings::setCacheImagePath( QString path )
{
    m_starviewerSettings.setValue( GroupSettingsName + cacheImagePathKey , path );
}

void StarviewerSettings::setPoolSize(QString size )
{
    m_starviewerSettings.setValue( GroupSettingsName + poolSizeKey , size );
}

void StarviewerSettings::setMaximumDaysNotViewedStudy( QString  days )
{
    m_starviewerSettings.setValue( GroupSettingsName + MaximumDaysNotViewedStudy  ,  days );
}

QString StarviewerSettings::getDatabasePath()
{
    QDir dir;
    QString defaultDir;

    //construim directori per defecte
    defaultDir.append( dir.homePath() + "/.starviewer/pacs/database/dicom.sdb" );

    return m_starviewerSettings.value( GroupSettingsName + databaseRootKey , defaultDir ).toString();
}

QString StarviewerSettings::getPoolSize()
{
    return m_starviewerSettings.value( GroupSettingsName + poolSizeKey , "20" ).toString();
}

QString StarviewerSettings::getCacheImagePath()
{
    QString defaultDir;
    QDir dir;

    //construim directori per defecte
    defaultDir.append( dir.homePath() + "/.starviewer/pacs/dicom/" );

    return m_starviewerSettings.value( GroupSettingsName + cacheImagePathKey , defaultDir ).toString();
}

QString StarviewerSettings::getMaximumDaysNotViewedStudy()
{
    return m_starviewerSettings.value( GroupSettingsName + MaximumDaysNotViewedStudy  ,  "7" ).toString();
}

/************************ CONFIGURACIO PACS************************************************/

void StarviewerSettings::setAETitleMachine( QString AETitle )
{
    m_starviewerSettings.setValue( GroupSettingsName + AETitleMachineKey , AETitle );
}

void StarviewerSettings::setTimeout( QString time )
{
    m_starviewerSettings.setValue( GroupSettingsName + timeoutPacsKey , time );
}

void StarviewerSettings::setLocalPort( QString port )
{
    m_starviewerSettings.setValue( GroupSettingsName + localPortKey , port );
}

void StarviewerSettings::setMaxConnections( QString maxConn )
{
    m_starviewerSettings.setValue( GroupSettingsName + maxConnectionsKey , maxConn );
}

QString StarviewerSettings::getAETitleMachine()
{
    return m_starviewerSettings.value( GroupSettingsName + AETitleMachineKey , GroupSettingsName ).toString();
}

QString StarviewerSettings::getTimeout()
{
    return m_starviewerSettings.value( GroupSettingsName + timeoutPacsKey , "20000" ).toString();
}

QString StarviewerSettings::getLocalPort()
{
    return m_starviewerSettings.value( GroupSettingsName + localPortKey , "4006" ).toString();
}

QString StarviewerSettings::getMaxConnections()
{
    return m_starviewerSettings.value( GroupSettingsName + maxConnectionsKey , "3" ).toString();
}

/************************ INTERFICIE ************************************************/
void StarviewerSettings::setStudyPacsListColumnWidth( int col , int width )
{
    QString key = GroupSettingsName + pacsColumnWidthKey + QString::number( col );
    m_starviewerSettings.setValue( key , width );
}

void StarviewerSettings::setStudyCacheListColumnWidth( int col , int width )
{
    QString key = GroupSettingsName + cacheColumnWidthKey + QString::number( col );
    m_starviewerSettings.setValue( key , width );
}

void StarviewerSettings::setStudyDicomdirListColumnWidth( int col , int width )
{
    QString key = GroupSettingsName + dicomdirColumnWidthKey + QString::number( col );
    m_starviewerSettings.setValue( key , width );
}

void StarviewerSettings::setQueryScreenWindowPositionX( int positionX )
{
    QString key = GroupSettingsName + queryScreenWindowPositionX;
    m_starviewerSettings.setValue( key , positionX );
}

void StarviewerSettings::setQueryScreenWindowPositionY( int positionY )
{
    QString key = GroupSettingsName + queryScreenWindowPositionY;
    m_starviewerSettings.setValue( key , positionY );
}

void StarviewerSettings::setQueryScreenWindowWidth( int width )
{
    QString key = GroupSettingsName + queryScreenWindowWidth;
    m_starviewerSettings.setValue( key , width );
}

void StarviewerSettings::setQueryScreenWindowHeight( int height )
{
    QString key = GroupSettingsName + queryScreenWindowHeight;
    m_starviewerSettings.setValue( key , height );
}

void StarviewerSettings::setQueryScreenStudyTreeSeriesListQSplitterState( QByteArray state )
{
    QString key = GroupSettingsName + queryScreenStudyTreeSeriesListQSplitterState;
    m_starviewerSettings.setValue( key , state );
}

void StarviewerSettings::setQOperationStateColumnWidth( int col , int width )
{
    QString key = GroupSettingsName + qOperationStateColumnWidthKey + QString::number( col );
    m_starviewerSettings.setValue( key , width );
}

void StarviewerSettings::setQCreateDicomdirColumnWidth( int col , int width )
{
    QString key = GroupSettingsName + qCreateDicomdirColumnWidthKey + QString::number( col );
    m_starviewerSettings.setValue( key , width );
}

void StarviewerSettings::setQConfigurationPacsDeviceColumnWidth( int col , int width )
{
    QString key = GroupSettingsName + qConfigurationPacsDeviceColumnWidthKey + QString::number( col );
    m_starviewerSettings.setValue( key , width );
}

int StarviewerSettings::getStudyPacsListColumnWidth( int col )
{
    QString key = GroupSettingsName + pacsColumnWidthKey + QString::number(col);
    return m_starviewerSettings.value( key , 100) .toInt();
}

int StarviewerSettings::getStudyCacheListColumnWidth( int col )
{
    QString key = GroupSettingsName + cacheColumnWidthKey + QString::number( col );
    return m_starviewerSettings.value( key , 100 ).toInt();
}

int StarviewerSettings::getStudyDicomdirListColumnWidth( int col )
{
    QString key = GroupSettingsName + dicomdirColumnWidthKey + QString::number( col );
    return m_starviewerSettings.value( key , 100 ).toInt();
}

int StarviewerSettings::getQueryScreenWindowPositionX()
{
    QString key = GroupSettingsName + queryScreenWindowPositionX;
    return m_starviewerSettings.value( key , 50 ).toInt();
}

int StarviewerSettings::getQueryScreenWindowPositionY()
{
    QString key = GroupSettingsName + queryScreenWindowPositionY;
    return m_starviewerSettings.value( key , 50 ).toInt();
}

int StarviewerSettings::getQueryScreenWindowWidth()
{
    QString key = GroupSettingsName + queryScreenWindowWidth;
    return m_starviewerSettings.value( key , 800 ).toInt();
}

int StarviewerSettings::getQueryScreenWindowHeight()
{
    QString key = GroupSettingsName + queryScreenWindowHeight;
    return m_starviewerSettings.value( key , 770 ).toInt();
}

QByteArray StarviewerSettings::getQueryScreenStudyTreeSeriesListQSplitterState()
{
    QString key = GroupSettingsName + queryScreenStudyTreeSeriesListQSplitterState;
    return m_starviewerSettings.value( key ).toByteArray();
}

int StarviewerSettings::getQOperationStateColumnWidth( int col )
{
    QString key = GroupSettingsName + qOperationStateColumnWidthKey + QString::number( col );
    return m_starviewerSettings.value( key , 100 ).toInt();
}

int StarviewerSettings::getQCreateDicomdirColumnWidth( int col )
{
    QString key = GroupSettingsName + qCreateDicomdirColumnWidthKey + QString::number( col );
    return m_starviewerSettings.value( key , 100 ).toInt();
}

int StarviewerSettings::getQConfigurationPacsDeviceColumnWidth( int col )
{
    QString key = GroupSettingsName + qConfigurationPacsDeviceColumnWidthKey + QString::number( col );
    return m_starviewerSettings.value( key , 100 ).toInt();
}

/*** Dades de la institució*/

void StarviewerSettings::setInstitutionName( QString institutionNameString )
{
    m_starviewerSettings.setValue( GroupSettingsName + InstitutionName , institutionNameString );
}

void StarviewerSettings::setInstitutionAddress ( QString institutionAddressString )
{
    m_starviewerSettings.setValue( GroupSettingsName + InstitutionAddress , institutionAddressString );
}

void StarviewerSettings::setInstitutionTown( QString institutionTownString )
{
    m_starviewerSettings.setValue( GroupSettingsName + InstitutionTown , institutionTownString );
}

void StarviewerSettings::setInstitutionZipCode( QString institutionZipCodeString )
{
    m_starviewerSettings.setValue( GroupSettingsName + InstitutionZipCode , institutionZipCodeString );
}

void StarviewerSettings::setInstitutionCountry( QString institutionCountryString )
{
    m_starviewerSettings.setValue( GroupSettingsName + InstitutionCountry , institutionCountryString );
}

void StarviewerSettings::setInstitutionPhoneNumber( QString institutionPhoneNumberString )
{
    m_starviewerSettings.setValue( GroupSettingsName + InstitutionPhoneNumber , institutionPhoneNumberString );
}

void StarviewerSettings::setInstitutionEmail( QString institutionEmailString )
{
    m_starviewerSettings.setValue( GroupSettingsName + InstitutionEmail , institutionEmailString );
}

QString StarviewerSettings::getInstitutionName()
{
    return m_starviewerSettings.value( GroupSettingsName + InstitutionName , "" ).toString();
}

QString StarviewerSettings::getInstitutionAddress()
{
    return m_starviewerSettings.value( GroupSettingsName + InstitutionAddress , "" ).toString();
}

QString StarviewerSettings::getInstitutionTown()
{
    return m_starviewerSettings.value( GroupSettingsName + InstitutionTown , "" ).toString();
}

QString StarviewerSettings::getInstitutionCountry()
{
    return m_starviewerSettings.value( GroupSettingsName + InstitutionCountry , "" ).toString();
}

QString StarviewerSettings::getInstitutionZipCode()
{
    return m_starviewerSettings.value( GroupSettingsName + InstitutionZipCode , "" ).toString();
}

QString StarviewerSettings::getInstitutionPhoneNumber()
{
    return m_starviewerSettings.value( GroupSettingsName + InstitutionPhoneNumber , "" ).toString();
}

QString StarviewerSettings::getInstitutionEmail()
{
    return m_starviewerSettings.value( GroupSettingsName + InstitutionEmail , "" ).toString();
}

void StarviewerSettings::setLogCommunicationPacsVerboseMode(bool verboseMode)
{
    m_starviewerSettings.setValue( GroupSettingsName + logsPacsCommunicationModeVerbose , verboseMode );
}

bool StarviewerSettings::getLogCommunicationPacsVerboseMode()
{
    return m_starviewerSettings.value( GroupSettingsName + logsPacsCommunicationModeVerbose , false ).toBool();
}


StarviewerSettings::~StarviewerSettings()
{
}

};
