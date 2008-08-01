/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "glialestimationextensionmediator.h"

#include "extensioncontext.h"

// QT
#include <QMessageBox>

namespace udg {

GlialEstimationExtensionMediator::GlialEstimationExtensionMediator(QObject *parent)
 : ExtensionMediator(parent)
{
}

GlialEstimationExtensionMediator::~GlialEstimationExtensionMediator()
{
}

DisplayableID GlialEstimationExtensionMediator::getExtensionID() const
{
    return DisplayableID("GlialEstimationExtension",tr("Glial Estimation"));
}

bool GlialEstimationExtensionMediator::initializeExtension(QWidget* extension, const ExtensionContext &extensionContext)
{
    QGlialEstimationExtension *glialEstimationExtension;

    if ( !(glialEstimationExtension = qobject_cast<QGlialEstimationExtension*>(extension)) )
    {
        return false;
    }

    Volume *input = extensionContext.getDefaultVolume();
    if( !input )
        QMessageBox::information(0,tr("Starviewer"), tr("The selected item is not an image") );
    else
        glialEstimationExtension->setInput( input );

    return true;
}

}
