#include "keyimagenote.h"
#include "series.h"
#include "study.h"
#include "image.h"
#include "dicomreferencedimage.h"
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

QList <DICOMReferencedImage*> KeyImageNote::getReferencedSOPInstancesUID() const
{
    return m_referencedSOPInstancesUIDList;
}

void KeyImageNote::setReferencedSOPInstancesUID(QList<DICOMReferencedImage*> &referencedImageList)
{
    m_referencedSOPInstancesUIDList = referencedImageList;
    foreach(DICOMReferencedImage *referencedImage, m_referencedSOPInstancesUIDList) 
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

QString KeyImageNote::getContentDate() const
{
    return m_contentDate;
}

void KeyImageNote::setContentDate(const QString &contentDate)
{
    m_contentDate = contentDate;
}

QString KeyImageNote::getContentTime() const
{
    return m_contentTime;
}

void KeyImageNote::setContentTime(const QString &contentTime)
{
    m_contentTime = contentTime;
}

QList<Image*> KeyImageNote::getReferencedImages()
{
    QList<Image*> referencedImages;

    foreach(DICOMReferencedImage *referencedImage, m_referencedSOPInstancesUIDList) 
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
    
    while(!found && i <study->getSeries().size())
    {
        Series *serie = study->getSeries().value(i);
        j = 0;
        
        while(!found && j<serie->getImages().size())
        {
            Image *currentImage = serie->getImages().value(j);
            
            if(currentImage->getSOPInstanceUID() == referencedImage->getDICOMReferencedImageSOPInstanceUID() && currentImage->getFrameNumber() == referencedImage->getFrameNumber())
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

    if(!found) // Si no la trobem generem una imatge emplenada amb tota la informació de la que disposem
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

QDate KeyImageNote::getRetrievedDate()
{
    return m_retrievedDate;
}

QTime KeyImageNote::getRetrievedTime()
{
    return m_retrieveTime;
}

QString KeyImageNote::getObserverTypeAsString(ObserverType observerType)
{
  switch(observerType)
  {
    case KeyImageNote::Person:              return "Person";
    case KeyImageNote::Device:              return "Device";
    case KeyImageNote::NoneObserverType:    return "None ObserverType";
    default:                                return "Bad ObserverType";
  }
}

QString KeyImageNote::getDocumentTitleAsString(DocumentTitle documentTitle)
{
  switch(documentTitle)
  {
    case KeyImageNote::OfInterest:                               return "Of Interest";
    case KeyImageNote::RejectedForQualityReasonsDocumentTitle:   return "Rejected for Quality Reasons";
    case KeyImageNote::ForReferringProvider:                     return "For Referring Provider";
    case KeyImageNote::ForSurgery:                               return "For Surgery";
    case KeyImageNote::ForTeaching:                              return "For Teaching";
    case KeyImageNote::ForConference:                            return "For Conference";
    case KeyImageNote::ForTherapy:                               return "For Therapy";
    case KeyImageNote::ForPatient:                               return "For Patient";
    case KeyImageNote::ForPeerReview:                            return "For Peer Review";
    case KeyImageNote::ForResearch:                              return "For Research";
    case KeyImageNote::QualityIssue:                             return "Quality Issue";
    case KeyImageNote::NoneDocumentTitle:                        return "None Document Title";
    default:                                                     return "Bad DocumentTitle";
  }
}

QString KeyImageNote::getRejectedForQualityReasonsAsString(RejectedForQualityReasons rejectedForQualityReasons)
{
  switch(rejectedForQualityReasons)
  {
    case KeyImageNote::ImageArtifacts:                              return "Image artifact(s)";
    case KeyImageNote::GridArtifacts:                               return "Grid artifact(s)";
    case KeyImageNote::Positioning:                                 return "Positioning";
    case KeyImageNote::MotionBlur:                                  return "Motion blur";
    case KeyImageNote::UnderExposed:                                return "Under exposed";
    case KeyImageNote::OverExposed:                                 return "Over exposed";
    case KeyImageNote::NoImage:                                     return "No image";
    case KeyImageNote::DetectorArtifacts:                           return "Detector artifact(s)";
    case KeyImageNote::ArtifactsOtherThanGridOrDetectorArtifact:    return "Artifact(s) other than grid or detector artifact";
    case KeyImageNote::MechanicalFailure:                           return "Mechanical failure";
    case KeyImageNote::ElectricalFailure:                           return "Electrical failure";
    case KeyImageNote::SoftwareFailure:                             return "Software failure";
    case KeyImageNote::InappropiateImageProcessing:                 return "Inappropiate image processing";
    case KeyImageNote::OtherFailure:                                return "Other failure";
    case KeyImageNote::UnknownFailure:                              return "Unknown failure";
    case KeyImageNote::DoubleExposure:                              return "Double exposure";
    case KeyImageNote::NoneRejectedForQualityReasons:               return "None rejected for quality reasons";
    default:                                                        return "Bad RejectedForQualityReasons";
  }
}
}
