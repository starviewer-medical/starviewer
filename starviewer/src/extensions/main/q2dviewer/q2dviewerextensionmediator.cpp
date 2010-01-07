/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "q2dviewerextensionmediator.h"

#include "extensioncontext.h"
#include "starviewerapplication.h"
// QT
#include <QMessageBox>

namespace udg{

Q2DViewerExtensionMediator::Q2DViewerExtensionMediator(QObject *parent)
 : ExtensionMediator(parent)
{
}

Q2DViewerExtensionMediator::~Q2DViewerExtensionMediator()
{
}

DisplayableID Q2DViewerExtensionMediator::getExtensionID() const
{
    return DisplayableID("Q2DViewerExtension",tr("2D Viewer"));
}

bool Q2DViewerExtensionMediator::initializeExtension(QWidget* extension, const ExtensionContext &extensionContext)
{
    Q2DViewerExtension *q2dviewerExtension;

    if ( !(q2dviewerExtension = qobject_cast<Q2DViewerExtension*>(extension)) )
    {
        return false;
    }

    q2dviewerExtension->setPatient( extensionContext.getPatient() );
    Volume *input = extensionContext.getDefaultVolume();
    if( !input )
        QMessageBox::information(0,ApplicationNameString, tr("There is not any viewable or supported Series in the selected Studies") );
    else
        q2dviewerExtension->setInput( input );

    return true;
}

bool Q2DViewerExtensionMediator::reinitializeExtension(QWidget* extension)
{
    Q2DViewerExtension *q2dviewerExtension;

    if ( !(q2dviewerExtension = qobject_cast<Q2DViewerExtension*>(extension)) )
    {
        return false;
    }
    else
    {
        // Busquem i apliquem el millor hanging protocol possible
        q2dviewerExtension->searchAndApplyBestHangingProtocol();
		//Actualitzem el widget de prèvies.
		q2dviewerExtension->searchPreviousStudiesOfMostRecentStudy();

        return true;
    }
}

} //udg namespace
