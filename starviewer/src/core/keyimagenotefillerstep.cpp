/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "keyimagenotefillerstep.h"
#include "logging.h"
#include "patientfillerinput.h"
#include "keyimagenote.h"
#include "dicomtagreader.h"
#include "series.h"

namespace udg {

KeyImageNoteFillerStep::KeyImageNoteFillerStep(): PatientFillerStep()
{
    m_requiredLabelsList << "DICOMFileClassifierFillerStep";
}

KeyImageNoteFillerStep::~KeyImageNoteFillerStep()
{
}

bool KeyImageNoteFillerStep::fillIndividually()
{
    Q_ASSERT(m_input);
    bool ok = false;
    
    if (isKeyImageNoteSeries(m_input->getCurrentSeries()))
    {
        KeyImageNote *keyImageNote = new KeyImageNote();
        ok = processKeyImageNote(keyImageNote);
        m_input->getCurrentSeries()->addKeyImageNote(keyImageNote);
        m_input->addLabelToSeries("KeyImageNoteFillerStep", m_input->getCurrentSeries());
    }
    
    return ok;
}

bool KeyImageNoteFillerStep::processKeyImageNote(KeyImageNote * keyImageNote)
{
    DICOMTagReader *dicomReader = m_input->getDICOMFile();
    bool ok = false;

    if(dicomReader)
    {
        ok = true;
        readSRTemplateKIN(keyImageNote, dicomReader);
    }
    else
    {
        ERROR_LOG("Error en el processament del KeyImageNote , no s'ha pogut obtenir el fitxer DICOM" );
    }

    return ok;
}

void KeyImageNoteFillerStep::readSRTemplateKIN(KeyImageNote *keyImageNote, DICOMTagReader *reader)
{
    QString sopInstanceUID = reader->getValueAttributeAsQString(DICOMSOPInstanceUID);
    keyImageNote->setInstanceUID(sopInstanceUID);
    
    QString contentDate = reader->getValueAttributeAsQString(DICOMContentDate);
    keyImageNote->setContentDate(contentDate);
    
    QString contentTime = reader->getValueAttributeAsQString(DICOMContentTime);
    keyImageNote->setContentTime(contentTime);

    QString instanceNumber = reader->getValueAttributeAsQString(DICOMInstanceNumber);
    keyImageNote->setInstanceNumber(instanceNumber);
    KeyImageNote::DocumentTitle documentTitle = readKeyObjectSelectionDocumentTitle(reader);
    keyImageNote->setDocumentTitle(documentTitle);

    KeyImageNote::RejectedForQualityReasons documentTitleModifier = KeyImageNote::NoneRejectedForQualityReasons;
    if (documentTitle == KeyImageNote::RejectedForQualityReasonsDocumentTitle || documentTitle == KeyImageNote::QualityIssue)
    {
        documentTitleModifier = readRejectedForQualityReasons(reader);
    }
    keyImageNote->setRejectedForQualityReasons(documentTitleModifier);

    QString keyObjectDescription = readKeyObjectDescription(reader);
    keyImageNote->setKeyObjectDescription(keyObjectDescription);

    QList<DICOMReferencedImage*> referencedSOPInstancesUID = readReferencedImagesInKIN(reader);
    keyImageNote->setReferencedSOPInstancesUID(referencedSOPInstancesUID);

    KeyImageNote::ObserverType observerContextType = readObserverContextType(reader);
    keyImageNote->setObserverContextType(observerContextType);

    QString observerContextName = readObserverContextName(reader, observerContextType);
    keyImageNote->setObserverContextName(observerContextName);
}

KeyImageNote::DocumentTitle KeyImageNoteFillerStep::readKeyObjectSelectionDocumentTitle(DICOMTagReader *reader)
{
    QString codeValue;
    QString codeMeaning;
    QString schemeDesignator = "DCM";
    KeyImageNote::DocumentTitle documentTitle = KeyImageNote::NoneDocumentTitle;
    
    codeValue = "113000";
    codeMeaning = "Of Interest";
    if (reader->existStructuredReportNode(codeValue, codeMeaning, schemeDesignator))
    {
        documentTitle = KeyImageNote::OfInterest;
    }

    codeValue = "113001";
    codeMeaning = "Rejected for Quality Reasons";
    if (reader->existStructuredReportNode(codeValue, codeMeaning, schemeDesignator))
    {
        documentTitle = KeyImageNote::RejectedForQualityReasonsDocumentTitle;
    }

    codeValue = "113002";
    codeMeaning = "For Referring Provider";
    if (reader->existStructuredReportNode(codeValue, codeMeaning, schemeDesignator))
    {
        documentTitle = KeyImageNote::ForReferringProvider;
    }

    codeValue = "113003";
    codeMeaning = "For Surgery";
    if (reader->existStructuredReportNode(codeValue, codeMeaning, schemeDesignator))
    {
        documentTitle = KeyImageNote::ForSurgery;
    }

    codeValue = "113004";
    codeMeaning = "For Teaching";
    if (reader->existStructuredReportNode(codeValue, codeMeaning, schemeDesignator))
    {
        documentTitle = KeyImageNote::ForTeaching;
    }

    codeValue = "113005";
    codeMeaning = "For Conference";
    if (reader->existStructuredReportNode(codeValue, codeMeaning, schemeDesignator))
    {
        documentTitle = KeyImageNote::ForConference;
    }

    codeValue = "113006";
    codeMeaning = "For Therapy";
    if (reader->existStructuredReportNode(codeValue, codeMeaning, schemeDesignator))
    {
        documentTitle = KeyImageNote::ForTherapy;
    }

    codeValue = "113007";
    codeMeaning = "For Patient";
    if (reader->existStructuredReportNode(codeValue, codeMeaning, schemeDesignator))
    {
        documentTitle = KeyImageNote::ForPatient;
    }

    codeValue = "113008";
    codeMeaning = "For Peer Review";
    if (reader->existStructuredReportNode(codeValue, codeMeaning, schemeDesignator))
    {
        documentTitle = KeyImageNote::ForPeerReview;
    }

    codeValue = "113009";
    codeMeaning = "For Research";
    if (reader->existStructuredReportNode(codeValue, codeMeaning, schemeDesignator))
    {
        documentTitle = KeyImageNote::ForResearch;
    }

    codeValue = "113010";
    codeMeaning = "Quality Issue";
    if (reader->existStructuredReportNode(codeValue, codeMeaning, schemeDesignator))
    {
        documentTitle = KeyImageNote::QualityIssue;
    }

    return documentTitle;
}

KeyImageNote::RejectedForQualityReasons KeyImageNoteFillerStep::readRejectedForQualityReasons(DICOMTagReader *reader)
{
    QString codeValue;
    QString codeMeaning;
    QString schemeDesignator = "DCM";
    KeyImageNote::RejectedForQualityReasons explanation = KeyImageNote::NoneRejectedForQualityReasons;

    codeValue = "111207";
    codeMeaning = "Image artifact(s)";
    if (reader->existStructuredReportNode(codeValue, codeMeaning, schemeDesignator))
    {
        explanation = KeyImageNote::ImageArtifacts;
    }

    codeValue = "111208";
    codeMeaning = "Grid artifact(s)";
    if (reader->existStructuredReportNode(codeValue, codeMeaning, schemeDesignator))
    {
        explanation = KeyImageNote::GridArtifacts;
    }

    codeValue = "111209";
    codeMeaning = "Positioning";
    if (reader->existStructuredReportNode(codeValue, codeMeaning, schemeDesignator))
    {
        explanation = KeyImageNote::Positioning;
    }

    codeValue = "111210";
    codeMeaning = "Motion blur";
    if (reader->existStructuredReportNode(codeValue, codeMeaning, schemeDesignator))
    {
        explanation = KeyImageNote::MotionBlur;
    }

    codeValue = "111211";
    codeMeaning = "Under exposed";
    if (reader->existStructuredReportNode(codeValue, codeMeaning, schemeDesignator))
    {
        explanation = KeyImageNote::UnderExposed;
    }

    codeValue = "111212";
    codeMeaning = "Over exposed";
    if (reader->existStructuredReportNode(codeValue, codeMeaning, schemeDesignator))
    {
        explanation = KeyImageNote::OverExposed;
    }

    codeValue = "111213";
    codeMeaning = "No image";
    if (reader->existStructuredReportNode(codeValue, codeMeaning, schemeDesignator))
    {
        explanation = KeyImageNote::NoImage;
    }

    codeValue = "111214";
    codeMeaning = "Detector artifacts(s)";
    if (reader->existStructuredReportNode(codeValue, codeMeaning, schemeDesignator))
    {
        explanation = KeyImageNote::DetectorArtifacts;
    }

    codeValue = "111215";
    codeMeaning = "Artifact(s) other than grid or detector artifact";
    if (reader->existStructuredReportNode(codeValue, codeMeaning, schemeDesignator))
    {
        explanation = KeyImageNote::ArtifactsOtherThanGridOrDetectorArtifact;
    }

    codeValue = "111216";
    codeMeaning = "Mechanical failure";
    if (reader->existStructuredReportNode(codeValue, codeMeaning, schemeDesignator))
    {
        explanation = KeyImageNote::MechanicalFailure;
    }

    codeValue = "111217";
    codeMeaning = "Electrical failure";
    if (reader->existStructuredReportNode(codeValue, codeMeaning, schemeDesignator))
    {
        explanation = KeyImageNote::ElectricalFailure;
    }

    codeValue = "111218";
    codeMeaning = "Software failure";
    if (reader->existStructuredReportNode(codeValue, codeMeaning, schemeDesignator))
    {
        explanation = KeyImageNote::SoftwareFailure;
    }

    codeValue = "111219";
    codeMeaning = "Inappropiate image processing";
    if (reader->existStructuredReportNode(codeValue, codeMeaning, schemeDesignator))
    {
        explanation = KeyImageNote::InappropiateImageProcessing;
    }

    codeValue = "111220";
    codeMeaning = "Other failure";
    if (reader->existStructuredReportNode(codeValue, codeMeaning, schemeDesignator))
    {
        explanation = KeyImageNote::OtherFailure;
    }

    codeValue = "111221";
    codeMeaning = "Unknown failure";
    if (reader->existStructuredReportNode(codeValue, codeMeaning, schemeDesignator))
    {
        explanation = KeyImageNote::UnknownFailure;
    }

    codeValue = "113026";
    codeMeaning = "Double exposure";
    if (reader->existStructuredReportNode(codeValue, codeMeaning, schemeDesignator))
    {
        explanation = KeyImageNote::DoubleExposure;
    }
    
    return explanation;
}

QString KeyImageNoteFillerStep::readKeyObjectDescription(DICOMTagReader *reader)
{
    QString codeValue = "113012";
    QString codeMeaning = "Key Object Description";
    QString schemeDesignator = "DCM";

    return reader->getStructuredReportNodeContent(codeValue, codeMeaning, schemeDesignator);
}

KeyImageNote::ObserverType KeyImageNoteFillerStep::readObserverContextType(DICOMTagReader *reader)
{
    QString codeValue = "121005";
    QString codeMeaning = "Observer Type";
    QString schemeDesignator = "DCM";

    QString typeCodeValue = reader->getStructuredReportCodeValueOfContentItem(codeValue, codeMeaning, schemeDesignator);
    KeyImageNote::ObserverType type = KeyImageNote::NoneObserverType;

    if(typeCodeValue == "121006")
    {
        type = KeyImageNote::Person;
    }
    else if (typeCodeValue == "121007")
    {
        type = KeyImageNote::Device;
    }

    return type;
}

QString KeyImageNoteFillerStep::readObserverContextName(DICOMTagReader *reader, KeyImageNote::ObserverType type)
{
    QString contextName;
    QString observerType;
    QString codeMeaning;
    QString schemeDesignator = "DCM";

    if (type == KeyImageNote::Person)
    {
        observerType = "121008"; 
        codeMeaning = "Person Observer Name";
        contextName = reader->getStructuredReportNodeContent(observerType, codeMeaning, schemeDesignator);
    }
    else if (type == KeyImageNote::Device)
    {
        observerType = "121012";
        codeMeaning = "Device Observer UID";
        contextName = reader->getStructuredReportNodeContent(observerType, codeMeaning, schemeDesignator);
    }
    
    return contextName;
}

QList<DICOMReferencedImage*> KeyImageNoteFillerStep::readReferencedImagesInKIN(DICOMTagReader *reader)
{
    return reader->getDICOMReferencedImagesOfStructedReport();
}

}
