/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "multiq2dviewerextensionmediator.h"
#include "volume.h"
#include "extensionhandler.h"
#include "extensioncontext.h"

namespace udg{

MultiQ2DViewerExtensionMediator::MultiQ2DViewerExtensionMediator(QObject *parent)
 : ExtensionMediator(parent)
{
}


MultiQ2DViewerExtensionMediator::~MultiQ2DViewerExtensionMediator()
{
}

DisplayableID MultiQ2DViewerExtensionMediator::getExtensionID() const
{
    return DisplayableID("MultiQ2DViewerExtension",tr("MultiQ2DViewer"));
}

bool MultiQ2DViewerExtensionMediator::initializeExtension(QWidget* extension, const ExtensionContext &extensionContext, ExtensionHandler* extensionHandler)
{
    InteractiveWidget *interactiveWidgetExtension;

    if ( !(interactiveWidgetExtension = qobject_cast<InteractiveWidget*>(extension)) )
    {
        return false;
    }

    interactiveWidgetExtension->setPatient( extensionContext.getPatient() );

    return true;
}

} //udg namespace
