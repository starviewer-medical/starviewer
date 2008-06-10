/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "optimalviewpointextensionmediator.h"

#include "extensioncontext.h"

// QT
#include <QMessageBox>

namespace udg {

OptimalViewpointExtensionMediator::OptimalViewpointExtensionMediator( QObject * parent )
    : ExtensionMediator( parent )
{
}

OptimalViewpointExtensionMediator::~OptimalViewpointExtensionMediator()
{
}

DisplayableID OptimalViewpointExtensionMediator::getExtensionID() const
{
    return DisplayableID( "OptimalViewpointExtension", tr("Optimal Viewpoint") );
}

bool OptimalViewpointExtensionMediator::initializeExtension(QWidget * extension, const ExtensionContext &extensionContext)
{
    QOptimalViewpointExtension * optimalViewpointExtension;

    if ( !( optimalViewpointExtension = qobject_cast< QOptimalViewpointExtension * >( extension ) ) )
    {
        return false;
    }

    Volume *input = extensionContext.getDefaultVolume();
    if( !input )
        QMessageBox::information(0,tr("Starviewer"), tr("The selected item is not an image") );
    else
        optimalViewpointExtension->setInput( input );

    return true;
}



}
