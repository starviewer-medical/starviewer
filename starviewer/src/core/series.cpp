/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "series.h"

#include "image.h"
#include "logging.h"
#include "volumerepository.h"

#include <QStringList>

#include <QDebug>

namespace udg {

Series::Series(QObject *parent)
 : QObject(parent)
{
}

Series::~Series()
{
}

void Series::setInstanceUID( QString uid )
{
    m_seriesInstanceUID = uid;
}

QString Series::getInstanceUID() const
{
    return m_seriesInstanceUID;
}

void Series::setParentStudy( Study *study )
{
    m_parentStudy = study;
}

Study *Series::getParentStudy() const
{
    return m_parentStudy;
}

bool Series::addImage( Image *image )
{
    bool ok = true;
    QString uid = image->getSOPInstanceUID();
    if( uid.isEmpty() )
    {
        ok = false;
        DEBUG_LOG("L'uid de la imatge està buit! No la podem insertar per inconsistent");
    }
    else if( this->imageExists(uid) )
    {
        ok = false;
        DEBUG_LOG("Ja existeix una imatge amb aquest mateix SOPInstanceUID:: " + uid );
    }
    else
    {
        image->setParentSeries( this );
        this->insertImage( image );
    }

    return ok;
}

Image *Series::getImage( QString SOPInstanceUID )
{
    int index = this->findImageIndex(SOPInstanceUID);
    if( index != -1 )
        return m_imageSet[index];
    else
        return NULL;
}

bool Series::imageExists( QString sopInstanceUID )
{
    if( this->findImageIndex(sopInstanceUID) != -1 )
        return true;
    else
        return false;
}

QList<Image *> Series::getImages() const
{
    return m_imageSet;
}

int Series::getNumberOfImages()
{
    return m_imageSet.size();
}

bool Series::hasImages() const
{
    return ! m_imageSet.isEmpty();
}

void Series::setModality( QString modality )
{
    m_modality = modality;
}

QString Series::getModality() const
{
    return m_modality;
}

void Series::setSeriesNumber( QString number )
{
    m_seriesNumber = number;
}

QString Series::getSeriesNumber() const
{
    return m_seriesNumber;
}

void Series::setFrameOfReferenceUID( QString uid )
{
    m_frameOfReferenceUID = uid;
}

QString Series::getFrameOfReferenceUID() const
{
    return m_frameOfReferenceUID;
}

void Series::setPositionReferenceIndicator( QString position )
{
    m_positionReferenceIndicator = position;
}

QString Series::getPositionReferenceIndicator() const
{
    return m_positionReferenceIndicator;
}

void Series::setDescription( QString description )
{
    m_description = description;
}

QString Series::getDescription() const
{
    return m_description;
}

void Series::setPatientPosition( QString position )
{
    m_patientPosition = position;
}

QString Series::getPatientPosition() const
{
    return m_patientPosition;
}

void Series::setProtocolName( QString protocolName )
{
    m_protocolName = protocolName;
}

QString Series::getProtocolName() const
{
    return m_protocolName;
}

void Series::setImagesPath( QString imagesPath )
{
    m_imagesPath = imagesPath;
}

QString Series::getImagesPath() const
{
    return m_imagesPath;
}

bool Series::setDateTime( int day , int month , int year , int hour , int minute, int second )
{
    return this->setDate( day, month, year ) && this->setTime( hour, minute, second );
}

bool Series::setDateTime( QString date , QString time )
{
    return this->setDate( date ) && this->setTime( time );
}

bool Series::setDate( int day , int month , int year )
{
    return this->setDate( QDate( year , month , day ) );
}

bool Series::setDate( QString date )
{
    // Seguim la suggerència de la taula 6.2-1 de la Part 5 del DICOM standard de tenir en compte el format yyyy.MM.dd
    return this->setDate( QDate::fromString(date.remove("."), "yyyyMMdd") );
}

bool Series::setDate( QDate date )
{
    if( date.isValid() )
    {
        m_date = date;
        return true;
    }
    else if( !date.isNull() )
    {
        DEBUG_LOG("La data està en un mal format: " + date.toString( Qt::LocaleDate ) );
        return false;
    }
}

bool Series::setTime( int hour , int minute, int second )
{
    return this->setTime( QTime(hour, minute, second) );
}

bool Series::setTime( QString time )
{
    // Seguim la suggerència de la taula 6.2-1 de la Part 5 del DICOM standard de tenir en compte el format hh:mm:ss.frac
    time = time.remove(":");

    QStringList split = time.split(".");
    QTime convertedTime = QTime::fromString(split[0], "hhmmss");

    if (split.size() == 2) //té fracció al final
    {
        // Trunquem a milisegons i no a milionèssimes de segons
        convertedTime = convertedTime.addMSecs( split[1].leftJustified(3,'0',true).toInt() );
    }

    return this->setTime( convertedTime );
}

bool Series::setTime(QTime time)
{
    if (time.isValid())
    {
        m_time = time;
        return true;
    }
    else if( !time.isNull() )
    {
        DEBUG_LOG( "El time està en un mal format" );
        return false;
    }
}

QDate Series::getDate()
{
    return m_date;
}

QString Series::getDateAsString()
{
    return m_date.toString( Qt::LocaleDate );
}

QTime Series::getTime()
{
    return m_time;
}

QString Series::getTimeAsString()
{
    return m_time.toString( "HH:mm:ss" );
}

void Series::setInstitutionName( QString institutionName )
{
    m_institutionName = institutionName;
}

QString Series::getInstitutionName() const
{
    return m_institutionName;
}

void Series::setBodyPartExamined( QString bodyPart )
{
    m_bodyPartExamined = bodyPart;
}

QString Series::getBodyPartExamined() const
{
    return m_bodyPartExamined;
}

void Series::setViewPosition( QString viewPosition )
{
    m_viewPosition = viewPosition;
}

QString Series::getViewPosition() const
{
    return m_viewPosition;
}

int Series::getNumberOfVolumes()
{
    return m_volumesList.size();
}

Volume* Series::getVolume(Identifier id)
{
    int index = m_volumesList.indexOf(id);
    return index != -1 ? VolumeRepository::getRepository()->getVolume(m_volumesList[index]) : NULL;
}

Volume* Series::getFirstVolume()
{
    return m_volumesList.isEmpty() ? NULL : this->getVolume(m_volumesList[0]);
}

Identifier Series::addVolume(Volume *volume)
{
    Identifier volumeID = VolumeRepository::getRepository()->addVolume(volume);
    m_volumesList.append(volumeID);
    return volumeID;
}

QList<Volume*> Series::getVolumesList()
{
    QList<Volume*> volumesList;
    foreach (Identifier id, m_volumesList)
    {
        volumesList << VolumeRepository::getRepository()->getVolume(id);
    }
    return volumesList;
}

QList<Identifier> Series::getVolumesIDList() const
{
    return m_volumesList;
}

void Series::addFilePath(QString filePath)
{
    if (!m_filesPathList.contains(filePath))
    {
        m_filesPathList.append(filePath);
    }
}

void Series::removeFilePath(QString filePath)
{
    int i = m_filesPathList.indexOf(filePath);
    if (i != -1)
    {
        m_filesPathList.removeAt(i);
    }
}

QStringList Series::getFilesPathList()
{
    return m_filesPathList;
}

QStringList Series::getImagesPathList()
{
    QStringList pathList;
    foreach( Image *image, m_imageSet )
    {
        pathList << image->getPath();
    }

    return pathList;
}

bool Series::isSelected() const
{
    return m_selected;
}

QString Series::toString(bool verbose)
{
    QString result;

    result += "        - Series\n";
    result += "            SeriesNumber : " + getSeriesNumber() + "\n";
    result += "            Modality : " + getModality() + "\n";
    result += "            Description : " + getDescription() + "\n";
    result += "            ProtocolName : " + getProtocolName() + "\n";
    result += "            Num.Images : " + QString::number( getImages().size() ) + "\n";

    if (verbose)
    {
        foreach (Image *image, getImages())
        {
            result += "            - Image " + image->getPath() + "\n";
        }
    }
    // TODO Idem per PS, KIN....

    return result;
}

void Series::select()
{
    setSelectStatus( true );
}

void Series::unSelect()
{
    setSelectStatus( false );
}

void Series::setSelectStatus( bool select )
{
    m_selected = select;
}

QPixmap Series::getThumbnail() const
{
    return getImages()[ getImages().size() / 2 ]->getThumbnail();;
}

void Series::setNumberOfPhases( int phases )
{
    m_numberOfPhases = phases;
}

int Series::getNumberOfPhases() const
{
    return m_numberOfPhases;
}

void Series::setNumberOfSlicesPerPhase( int slices )
{
    m_numberOfSlicesPerPhase = slices;
}

int Series::getNumberOfSlicesPerPhase() const
{
    return m_numberOfSlicesPerPhase;
}

void Series::insertImage( Image *image )
{
    int i = 0;
    while( i < m_imageSet.size() && m_imageSet.at(i)->getInstanceNumber().toInt() < image->getInstanceNumber().toInt() )
    {
        i++;
    }
    m_imageSet.insert( i, image );
}

int Series::findImageIndex( QString sopInstanceUID )
{
    int i = 0;
    bool found = false;
    while( i < m_imageSet.size() && !found )
    {
        if( m_imageSet.at(i)->getSOPInstanceUID() == sopInstanceUID )
            found = true;
        else
            i++;
    }
    if( !found )
        i = -1;

    return i;
}

}
