/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qmpr3d2dextensioncreator.h"
#include "qmpr3d2dextension.h"

namespace udg {

QMPR3D2DExtensionCreator::QMPR3D2DExtensionCreator(QObject *parent, const char *name)
 : ExtensionCreator(parent, name)
{
}


QMPR3D2DExtensionCreator::~QMPR3D2DExtensionCreator()
{
}

QWidget *QMPR3D2DExtensionCreator::createExtension( QWidget *parent , QString name )
{
    QMPR3D2DExtension *newWidget = new QMPR3D2DExtension( parent );
    newWidget->setObjectName( name );
    return newWidget;
}

bool QMPR3D2DExtensionCreator::initializeExtension( QWidget *extension /* , Resource *resources */ )
{
    bool ok = true;
    
    if( extension != 0 )
    {
        ok = false;
    }
    else
    {
        QMPR3D2DExtension *widget = dynamic_cast< QMPR3D2DExtension * >( extension );
        // fer inits
        // widget->setVolumeRepository( resources->getVolumeRepository() );
        // widget->setMainCLUT( resources->getMainCLUT() );
        // widget->setMainVolumeID( resources->getMainVolumeID() );        
    }
    
    return ok;
    
}

};  // end namespace udg 
