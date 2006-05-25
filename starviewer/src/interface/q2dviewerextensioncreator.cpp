/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "q2dviewerextensioncreator.h"
#include "q2dviewerextension.h"

namespace udg {

Q2DViewerExtensionCreator::Q2DViewerExtensionCreator(QObject* parent, const char* name)
: ExtensionCreator(parent, name)
{
}


Q2DViewerExtensionCreator::~Q2DViewerExtensionCreator()
{
}

QWidget *Q2DViewerExtensionCreator::createExtension( QWidget *parent , QString name )
{
    Q2DViewerExtension *newWidget = new Q2DViewerExtension( parent );
    newWidget->setObjectName( name );
    return newWidget;
}

bool Q2DViewerExtensionCreator::initializeExtension( QWidget *extension /* , Resource *resources */ )
{
    bool ok = true;
    
    if( extension != 0 )
    {
        ok = false;
    }
    else
    {
        Q2DViewerExtension *widget = dynamic_cast< Q2DViewerExtension * >( extension );
        // fer inits
        // widget->setVolumeRepository( resources->getVolumeRepository() );
        // widget->setMainCLUT( resources->getMainCLUT() );
        // widget->setMainVolumeID( resources->getMainVolumeID() );
        
    }
    
    return ok;
    
}

} // end namespace udg
