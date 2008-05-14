/**************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "lesionvisualizationextensionmediator.h"
#include "extensioncontext.h"

// QT
#include <QMessageBox>

namespace udg {

LesionVisualizationExtensionMediator::LesionVisualizationExtensionMediator(QObject *parent)
 : ExtensionMediator(parent)
{
}

LesionVisualizationExtensionMediator::~LesionVisualizationExtensionMediator()
{
}

DisplayableID LesionVisualizationExtensionMediator::getExtensionID() const
{
    return DisplayableID("LesionVisualizationExtension",tr("Lesion Visualization"));
}

bool LesionVisualizationExtensionMediator::initializeExtension(QWidget* extension, const ExtensionContext &extensionContext)
{
    QLesionVisualizationExtension *lesionVisualizationExtension;

    if ( !(lesionVisualizationExtension = qobject_cast<QLesionVisualizationExtension*>(extension)) )
    {
        return false;
    }

    Volume *input = extensionContext.getDefaultVolume();
    if( !input )
        QMessageBox::information(0,tr("Starviewer"), tr("The selected item is not an image") );
    else
        lesionVisualizationExtension->setInput( input );

    return true;
}


}
