#include "keyimagenote.h"
#include "series.h"
#include "study.h"
#include "image.h"
#include "dicomreferencedimage.h"
#include "logging.h"

namespace udg {

KeyImageNote::KeyImageNote()
{
}

KeyImageNote::~KeyImageNote()
{
}

KeyImageNote::DocumentTitle KeyImageNote::getDocumentTitle() const
{
    return m_documentTitle;
}

void KeyImageNote::setDocumentTitle(KeyImageNote::DocumentTitle documentTitle)
{
    m_documentTitle = documentTitle;
}

KeyImageNote::RejectedForQualityReasons KeyImageNote::getRejectedForQualityReasons() const
{
    return m_rejectedForQualityReasons;
}

void KeyImageNote::setRejectedForQualityReasons(KeyImageNote::RejectedForQualityReasons rejectedForQualityReasons)
{
    m_rejectedForQualityReasons = rejectedForQualityReasons;
}

QString KeyImageNote::getKeyObjectDescription() const
{
    return m_keyObjectDescription;
}

void KeyImageNote::setKeyObjectDescription(const QString &description)
{
    m_keyObjectDescription = description;
}

KeyImageNote::ObserverType KeyImageNote::getObserverContextType() const
{
    return m_observerContextType;
}

void KeyImageNote::setObserverContextType(KeyImageNote::ObserverType contextType)
{
    m_observerContextType = contextType;
}

QList<DICOMReferencedImage*> KeyImageNote::getDICOMReferencedImages() const
{
    return m_DICOMReferencedImages;
}

void KeyImageNote::setReferencedSOPInstancesUID(QList<DICOMReferencedImage*> &referencedImageList)
{
    m_DICOMReferencedImages = referencedImageList;
    foreach (DICOMReferencedImage *referencedImage, m_DICOMReferencedImages) 
    {
        referencedImage->setReferenceParentSOPInstanceUID(m_SOPInstanceUID);
    }
}

QString KeyImageNote::getObserverContextName() const
{
    return m_observerContextName;
}

void KeyImageNote::setObserverContextName(const QString &contextName)
{
    m_observerContextName = contextName;
}

void KeyImageNote::setParentSeries(Series *series)
{
    m_parentSeries = series;
    this->setParent(m_parentSeries);
}

Series* KeyImageNote::getParentSeries() const
{
    return m_parentSeries;
}

QString KeyImageNote::getInstanceUID() const
{
    return m_SOPInstanceUID;
}

void KeyImageNote::setInstanceUID(const QString &uid)
{
    m_SOPInstanceUID = uid;
}

bool KeyImageNote::setContentDate(QString date)
{
    return this->setContentDate(QDate::fromString(date.remove("."), "yyyyMMdd"));
}

bool KeyImageNote::setContentDate(QDate date)
{
    bool ok = true;
    if(date.isValid())
    {
        m_contentDate = date;
        ok = true;
    }
    else if(!date.isNull())
    {
        DEBUG_LOG("La data està en un mal format: " + date.toString( Qt::LocaleDate ) );
        ok = false;
    }
    return ok;
}

bool KeyImageNote::setContentTime(QString time)
{
    time = time.remove(":");

    QStringList split = time.split(".");
    QTime convertedTime = QTime::fromString(split[0], "hhmmss");

    if (split.size() == 2)
    {
        convertedTime = convertedTime.addMSecs( split[1].leftJustified(3,'0',true).toInt() );
    }

    return this->setContentTime(convertedTime);
}

bool KeyImageNote::setContentTime(QTime time)
{
    bool ok = true;
    if (time.isValid())
    {
        m_contentTime = time;
        ok = true;
    }
    else if(!time.isNull())
    {
        DEBUG_LOG( "El time està en un mal format" );
        ok = false;
    }

    return ok;
}

QDate KeyImageNote::getContentDate()
{
    return m_contentDate;
}

QTime KeyImageNote::getContentTime()
{
    return m_contentTime;
}

QList<Image*> KeyImageNote::getReferencedImages()
{
    QList<Image*> referencedImages;

    foreach (DICOMReferencedImage *referencedImage, m_DICOMReferencedImages) 
    {
        Image *image = getImage(referencedImage);
        referencedImages.append(image);
    }

    return referencedImages;
}

Image* KeyImageNote::getImage(DICOMReferencedImage *referencedImage)
{
    bool found;
    int i;
    int j;
    
    Image *image;
    Study *study = m_parentSeries->getParentStudy();
    found = false;
    i = 0;
    
    while (!found && i < study->getSeries().size())
    {
        Series *serie = study->getSeries().value(i);
        j = 0;
        
        while (!found && j<serie->getImages().size())
        {
            Image *currentImage = serie->getImages().value(j);
            
            if (currentImage->getSOPInstanceUID() == referencedImage->getDICOMReferencedImageSOPInstanceUID() && currentImage->getFrameNumber() == referencedImage->getFrameNumber())
            {
                found = true;
                image = currentImage;
            }
            else
            {
                j++;
            }
        }
        i++;
    }

    if (!found) // Si no la trobem generem una imatge emplenada amb tota la informació de la que disposem
    {
        image = new Image();
        image->setSOPInstanceUID(referencedImage->getDICOMReferencedImageSOPInstanceUID());
        image->setFrameNumber(referencedImage->getFrameNumber());
    }

    return image;
}

QString KeyImageNote::getInstanceNumber() const
{
    return m_instanceNumber;
}

void KeyImageNote::setInstanceNumber(const QString &instanceNumber)
{
    m_instanceNumber = instanceNumber;
}

void KeyImageNote::setRetrievedDate(QDate retrievedDate)
{
    m_retrievedDate = retrievedDate;
}

void KeyImageNote::setRetrievedTime(QTime retrievedTime)
{
    m_retrieveTime = retrievedTime;
}

QDate KeyImageNote::getRetrievedDate() const
{
    return m_retrievedDate;
}

QTime KeyImageNote::getRetrievedTime() const
{
    return m_retrieveTime;
}

QString KeyImageNote::getObserverTypeAsString(ObserverType observerType) const
{
  switch (observerType)
  {
    case KeyImageNote::Person:              
        return tr("Person");
    case KeyImageNote::Device:              
        return tr("Device");
    case KeyImageNote::NoneObserverType:    
        return tr("None ObserverType");
    default:                                
        return tr("Bad ObserverType");
  }
}

QString KeyImageNote::getDocumentTitleAsString(DocumentTitle documentTitle) const
{
  switch (documentTitle)
  {
    case KeyImageNote::OfInterest:                               
        return tr("Of Interest");
    case KeyImageNote::RejectedForQualityReasonsDocumentTitle:   
        return tr("Rejected for Quality Reasons");
    case KeyImageNote::ForReferringProvider:                     
        return tr("For Referring Provider");
    case KeyImageNote::ForSurgery:                               
        return tr("For Surgery");
    case KeyImageNote::ForTeaching:                              
        return tr("For Teaching");
    case KeyImageNote::ForConference:                            
        return tr("For Conference");
    case KeyImageNote::ForTherapy:                               
        return tr("For Therapy");
    case KeyImageNote::ForPatient:                               
        return tr("For Patient");
    case KeyImageNote::ForPeerReview:                            
        return tr("For Peer Review");
    case KeyImageNote::ForResearch:                              
        return tr("For Research");
    case KeyImageNote::QualityIssue:                             
        return tr("Quality Issue");
    case KeyImageNote::NoneDocumentTitle:                        
        return tr("None Document Title");
    default:                                                     
        return tr("Bad DocumentTitle");
  }
}

QString KeyImageNote::getRejectedForQualityReasonsAsString(RejectedForQualityReasons rejectedForQualityReasons) const
{
  switch (rejectedForQualityReasons)
  {
    case KeyImageNote::ImageArtifacts:                              
        return tr("Image artifact(s)");
    case KeyImageNote::GridArtifacts:                               
        return tr("Grid artifact(s)");
    case KeyImageNote::Positioning:                                 
        return tr("Positioning");
    case KeyImageNote::MotionBlur:                                  
        return tr("Motion blur");
    case KeyImageNote::UnderExposed:                                
        return tr("Under exposed");
    case KeyImageNote::OverExposed:                                 
        return tr("Over exposed");
    case KeyImageNote::NoImage:                                     
        return tr("No image");
    case KeyImageNote::DetectorArtifacts:                           
        return tr("Detector artifact(s)");
    case KeyImageNote::ArtifactsOtherThanGridOrDetectorArtifact:    
        return tr("Artifact(s) other than grid or detector artifact");
    case KeyImageNote::MechanicalFailure:                           
        return tr("Mechanical failure");
    case KeyImageNote::ElectricalFailure:                           
        return tr("Electrical failure");
    case KeyImageNote::SoftwareFailure:                             
        return tr("Software failure");
    case KeyImageNote::InappropiateImageProcessing:                 
        return tr("Inappropiate image processing");
    case KeyImageNote::OtherFailure:                                
        return tr("Other failure");
    case KeyImageNote::UnknownFailure:                              
        return tr("Unknown failure");
    case KeyImageNote::DoubleExposure:                              
        return tr("Double exposure");
    case KeyImageNote::NoneRejectedForQualityReasons:               
        return tr("None rejected for quality reasons");
    default:                                                        
        return tr("Bad RejectedForQualityReasons");
  }
}

}
