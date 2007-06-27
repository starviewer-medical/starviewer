/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "keyimagenotefilereader.h"

#include "keyimagenote.h"
#include "logging.h"

#define HAVE_CONFIG_H 1
#include <dcfilefo.h> // per carregar arxius dicom
#include <dcdatset.h>
#include <dsrdoc.h>
#include <dsrtypes.h>

namespace udg {

KeyImageNoteFileReader::KeyImageNoteFileReader() : m_filename(""), m_lastErrorDescription("")
{
}

KeyImageNoteFileReader::KeyImageNoteFileReader(const QString &filename) : m_lastErrorDescription("")
{
    setFilename( filename );
}

void KeyImageNoteFileReader::setFilename(const QString &filename)
{
    m_filename = filename;
}

KeyImageNoteFileReader::~KeyImageNoteFileReader()
{
}

KeyImageNote* KeyImageNoteFileReader::read()
{
    return read(m_filename);
}

KeyImageNote* KeyImageNoteFileReader::read(const QString &filename)
{
    DcmFileFormat file;
    if (! file.loadFile( qPrintable(filename) ).good())
    {
        DEBUG_LOG( "KeyImageNoteFileReader::read: No s'ha pogut llegir el fitxer del KIN" );
        return NULL;
    }

    // Parsejem el KIN
    DSRDocument document;
    if (! document.read(*file.getDataset(), DSRTypes::RF_verboseDebugMode).good())
    {
        DEBUG_LOG( "KeyImageNoteFileReader::read: No s'ha pogut llegir el document SR" );
        return NULL;
    }

    DEBUG_LOG( "KeyImageNoteFileReader::read: Comencem a parsejar el KIN" );

    DSRSOPInstanceReferenceList &currentRequestedList = document.getCurrentRequestedProcedureEvidence();


    DEBUG_LOG( QString("KeyImageNoteFileReader::read: ReferenceList = %1").arg( currentRequestedList.getNumberOfInstances() ) );
    QStringList currentRequested;
    currentRequestedList.gotoFirstItem();
    for(unsigned int i = 0; i < currentRequestedList.getNumberOfInstances(); ++i)
    {
        OFString SOPInstance;
        currentRequestedList.getSOPInstanceUID(SOPInstance);
        currentRequested << SOPInstance.c_str();
        DEBUG_LOG( QString("Afegit: ") + SOPInstance.c_str() );
        if ( ! currentRequestedList.gotoNextItem().good())
        {
            DEBUG_LOG( "KeyImageNoteFileReader::read: Error al intentar avançar en la llista" );
        }
    }

    if (currentRequestedList.getNumberOfInstances() != currentRequested.size())
    {
        DEBUG_LOG( "KeyImageNoteFileReader::read: Error, no s'han carregat tots els possibles UID" );
    }

    if (currentRequested.isEmpty())
    {
        DEBUG_LOG( "KeyImageNoteFileReader::read: Error, no s'ha carregat cap UID!" );
        return NULL;
    }

    DEBUG_LOG( "KeyImageNoteFileReader::read: Continuem carregant el KIN" );

    DSRDocumentTree &tree = document.getTree();
    tree.print(std::cout);

    QString documentTitle = this->searchDocumentTitle(tree);
    DEBUG_LOG( "KeyImageNoteFileReader::read: documentTitle = " + documentTitle );

    QString observableContext = this->searchObservableContext(tree);
    DEBUG_LOG( "KeyImageNoteFileReader::read: observableContext = " + observableContext );

    QString documentTitleExplanation = "";
    if (documentTitle == "Rejected for Quality Reasons" or documentTitle == "Quality Issue")
    {
        documentTitleExplanation = this->searchQualityExplanationDocumentTitle(tree);
        DEBUG_LOG( "KeyImageNoteFileReader::read: documentTitleExplanation = " + documentTitleExplanation );
    }

    QString keyObjectDescription = this->searchObjectDescription(tree);
    DEBUG_LOG( "KeyImageNoteFileReader::read: Key Object Description = " + keyObjectDescription );

    KeyImageNote *keyImageNote = new KeyImageNote();
    keyImageNote->setReferencedSOPInstancesUID( currentRequested );
    keyImageNote->setDocumentTitle( documentTitle );
    keyImageNote->setDocumentTitleQualityReasons( documentTitleExplanation );
    keyImageNote->setKeyObjectDescription( keyObjectDescription );
    keyImageNote->setObservableContext( observableContext );

    return keyImageNote;
}

QString KeyImageNoteFileReader::getLastErrorDescription() const
{
    return m_lastErrorDescription;
}

QString KeyImageNoteFileReader::searchDocumentTitle(DSRDocumentTree &tree)
{
    QString codeValue;
    QString codeMeaning;
    QString schemeDesignator = "DCM";
    QString documentTitle = "";

    codeValue = "113000";
    codeMeaning = "Of Interest";
    if (tree.gotoNamedNode( DSRCodedEntryValue(qPrintable(codeValue),qPrintable(schemeDesignator),qPrintable(codeMeaning)) ))
    {
        documentTitle = codeMeaning;
    }

    codeValue = "113001";
    codeMeaning = "Rejected for Quality Reasons";
    if (tree.gotoNamedNode( DSRCodedEntryValue(qPrintable(codeValue),qPrintable(schemeDesignator),qPrintable(codeMeaning)) ))
    {
        documentTitle = codeMeaning;
    }

    codeValue = "113002";
    codeMeaning = "For Referring Provider";
    if (tree.gotoNamedNode( DSRCodedEntryValue(qPrintable(codeValue),qPrintable(schemeDesignator),qPrintable(codeMeaning)) ))
    {
        documentTitle = codeMeaning;
    }

    codeValue = "113003";
    codeMeaning = "For Surgery";
    if (tree.gotoNamedNode( DSRCodedEntryValue(qPrintable(codeValue),qPrintable(schemeDesignator),qPrintable(codeMeaning)) ))
    {
        documentTitle = codeMeaning;
    }

    codeValue = "113004";
    codeMeaning = "For Teaching";
    if (tree.gotoNamedNode( DSRCodedEntryValue(qPrintable(codeValue),qPrintable(schemeDesignator),qPrintable(codeMeaning)) ))
    {
        documentTitle = codeMeaning;
    }

    codeValue = "113005";
    codeMeaning = "For Conference";
    if (tree.gotoNamedNode( DSRCodedEntryValue(qPrintable(codeValue),qPrintable(schemeDesignator),qPrintable(codeMeaning)) ))
    {
        documentTitle = codeMeaning;
    }

    codeValue = "113006";
    codeMeaning = "For Therapy";
    if (tree.gotoNamedNode( DSRCodedEntryValue(qPrintable(codeValue),qPrintable(schemeDesignator),qPrintable(codeMeaning)) ))
    {
        documentTitle = codeMeaning;
    }

    codeValue = "113007";
    codeMeaning = "For Patient";
    if (tree.gotoNamedNode( DSRCodedEntryValue(qPrintable(codeValue),qPrintable(schemeDesignator),qPrintable(codeMeaning)) ))
    {
        documentTitle = codeMeaning;
    }

    codeValue = "113008";
    codeMeaning = "For Peer Review";
    if (tree.gotoNamedNode( DSRCodedEntryValue(qPrintable(codeValue),qPrintable(schemeDesignator),qPrintable(codeMeaning)) ))
    {
        documentTitle = codeMeaning;
    }

    codeValue = "113009";
    codeMeaning = "For Research";
    if (tree.gotoNamedNode( DSRCodedEntryValue(qPrintable(codeValue),qPrintable(schemeDesignator),qPrintable(codeMeaning)) ))
    {
        documentTitle = codeMeaning;
    }

    codeValue = "113010";
    codeMeaning = "Quality Issue";
    if (tree.gotoNamedNode( DSRCodedEntryValue(qPrintable(codeValue),qPrintable(schemeDesignator),qPrintable(codeMeaning)) ))
    {
        documentTitle = codeMeaning;
    }

    return documentTitle;
}

QString KeyImageNoteFileReader::searchQualityExplanationDocumentTitle(DSRDocumentTree &tree)
{
    QString codeValue;
    QString codeMeaning;
    QString schemeDesignator = "DCM";
    QString explanation = "";

    codeValue = "111207";
    codeMeaning = "Image artifact(s)";
    if (tree.gotoNamedNode( DSRCodedEntryValue(qPrintable(codeValue),qPrintable(schemeDesignator),qPrintable(codeMeaning)) ))
    {
        explanation = codeMeaning;
    }

    codeValue = "111208";
    codeMeaning = "Grid artifact(s)";
    if (tree.gotoNamedNode( DSRCodedEntryValue(qPrintable(codeValue),qPrintable(schemeDesignator),qPrintable(codeMeaning)) ))
    {
        explanation = codeMeaning;
    }

    codeValue = "111209";
    codeMeaning = "Positioning";
    if (tree.gotoNamedNode( DSRCodedEntryValue(qPrintable(codeValue),qPrintable(schemeDesignator),qPrintable(codeMeaning)) ))
    {
        explanation = codeMeaning;
    }

    codeValue = "111210";
    codeMeaning = "Motion blur";
    if (tree.gotoNamedNode( DSRCodedEntryValue(qPrintable(codeValue),qPrintable(schemeDesignator),qPrintable(codeMeaning)) ))
    {
        explanation = codeMeaning;
    }

    codeValue = "111211";
    codeMeaning = "Under exposed";
    if (tree.gotoNamedNode( DSRCodedEntryValue(qPrintable(codeValue),qPrintable(schemeDesignator),qPrintable(codeMeaning)) ))
    {
        explanation = codeMeaning;
    }

    codeValue = "111212";
    codeMeaning = "Over exposed";
    if (tree.gotoNamedNode( DSRCodedEntryValue(qPrintable(codeValue),qPrintable(schemeDesignator),qPrintable(codeMeaning)) ))
    {
        explanation = codeMeaning;
    }

    codeValue = "111213";
    codeMeaning = "No image";
    if (tree.gotoNamedNode( DSRCodedEntryValue(qPrintable(codeValue),qPrintable(schemeDesignator),qPrintable(codeMeaning)) ))
    {
        explanation = codeMeaning;
    }

    codeValue = "111214";
    codeMeaning = "Detector artifacts(s)";
    if (tree.gotoNamedNode( DSRCodedEntryValue(qPrintable(codeValue),qPrintable(schemeDesignator),qPrintable(codeMeaning)) ))
    {
        explanation = codeMeaning;
    }

    codeValue = "111215";
    codeMeaning = "Artifact(s) other than grid or detector artifact";
    if (tree.gotoNamedNode( DSRCodedEntryValue(qPrintable(codeValue),qPrintable(schemeDesignator),qPrintable(codeMeaning)) ))
    {
        explanation = codeMeaning;
    }

    codeValue = "111216";
    codeMeaning = "Mechanical failure";
    if (tree.gotoNamedNode( DSRCodedEntryValue(qPrintable(codeValue),qPrintable(schemeDesignator),qPrintable(codeMeaning)) ))
    {
        explanation = codeMeaning;
    }

    codeValue = "111217";
    codeMeaning = "Electrical failure";
    if (tree.gotoNamedNode( DSRCodedEntryValue(qPrintable(codeValue),qPrintable(schemeDesignator),qPrintable(codeMeaning)) ))
    {
        explanation = codeMeaning;
    }

    codeValue = "111218";
    codeMeaning = "Software failure";
    if (tree.gotoNamedNode( DSRCodedEntryValue(qPrintable(codeValue),qPrintable(schemeDesignator),qPrintable(codeMeaning)) ))
    {
        explanation = codeMeaning;
    }

    codeValue = "111219";
    codeMeaning = "Inappropiate image processing";
    if (tree.gotoNamedNode( DSRCodedEntryValue(qPrintable(codeValue),qPrintable(schemeDesignator),qPrintable(codeMeaning)) ))
    {
        explanation = codeMeaning;
    }

    codeValue = "111220";
    codeMeaning = "Other failure";
    if (tree.gotoNamedNode( DSRCodedEntryValue(qPrintable(codeValue),qPrintable(schemeDesignator),qPrintable(codeMeaning)) ))
    {
        explanation = codeMeaning;
    }

    codeValue = "111221";
    codeMeaning = "Unknown failure";
    if (tree.gotoNamedNode( DSRCodedEntryValue(qPrintable(codeValue),qPrintable(schemeDesignator),qPrintable(codeMeaning)) ))
    {
        explanation = codeMeaning;
    }

    codeValue = "113026";
    codeMeaning = "Double exposure";
    if (tree.gotoNamedNode( DSRCodedEntryValue(qPrintable(codeValue),qPrintable(schemeDesignator),qPrintable(codeMeaning)) ))
    {
        explanation = codeMeaning;
    }

    return explanation;
}

QString KeyImageNoteFileReader::searchObservableContext(DSRDocumentTree &tree)
{
    QString codeValue;
    QString codeMeaning;
    QString schemeDesignator = "DCM";
    QString observableContext = "";

    codeValue = "121006";
    codeMeaning = "Person";
    if (tree.gotoNamedNode( DSRCodedEntryValue(qPrintable(codeValue),qPrintable(schemeDesignator),qPrintable(codeMeaning)) ))
    {
        observableContext = codeMeaning;
    }

    return observableContext;
}

QString KeyImageNoteFileReader::searchObjectDescription(DSRDocumentTree &tree)
{
    QString codeValue = "113012";
    QString codeMeaning = "Key Object Description";
    QString schemeDesignator = "DCM";
    QString keyObjectDescription = "";

    if (tree.gotoNamedNode( DSRCodedEntryValue(qPrintable(codeValue),qPrintable(schemeDesignator),qPrintable(codeMeaning)) ))
    {
        keyObjectDescription = tree.getCurrentContentItem().getStringValue().c_str();
    }

    return keyObjectDescription;
}

}
