/**************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "rectumlesionvisualizationextensionmediator.h"

#include "extensioncontext.h"

// QT
#include <QMessageBox>

namespace udg {

RectumLesionVisualizationExtensionMediator::RectumLesionVisualizationExtensionMediator(QObject *parent)
 : ExtensionMediator(parent)
{
}

RectumLesionVisualizationExtensionMediator::~RectumLesionVisualizationExtensionMediator()
{
}

DisplayableID RectumLesionVisualizationExtensionMediator::getExtensionID() const
{
    return DisplayableID("RectumLesionVisualizationExtension",tr("Rectum Lesion Visualization"));
}

bool RectumLesionVisualizationExtensionMediator::initializeExtension(QWidget* extension, const ExtensionContext &extensionContext)
{
    QRectumLesionVisualizationExtension *rectumLesionVisualizationExtension;

    if ( !(rectumLesionVisualizationExtension = qobject_cast<QRectumLesionVisualizationExtension*>(extension)) )
    {
        return false;
    }

    Volume *input = extensionContext.getDefaultVolume();
    if( !input )
        QMessageBox::information(0,tr("Starviewer"), tr("The selected item is not an image") );
    else
        rectumLesionVisualizationExtension->setInput( input );

    return true;
}


}
