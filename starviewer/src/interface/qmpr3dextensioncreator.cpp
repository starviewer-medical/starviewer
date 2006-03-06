/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qmpr3dextensioncreator.h"
#include "qmpr3dextension.h"
namespace udg {

QMPR3DExtensionCreator::QMPR3DExtensionCreator(QObject *parent, const char *name)
 : ExtensionCreator(parent, name)
{
}

QMPR3DExtensionCreator::~QMPR3DExtensionCreator()
{
}

QWidget *QMPR3DExtensionCreator::createExtension( QWidget *parent , QString name )
{
    return new QMPR3DExtension( parent , name );
}

bool QMPR3DExtensionCreator::initializeExtension( QWidget *extension /* , Resource *resources */ )
{
    bool ok = true;
    
    if( extension != 0 )
    {
        ok = false;
    }
    else
    {
        QMPR3DExtension *widget = dynamic_cast< QMPR3DExtension * >( extension );
        // fer inits
        // widget->setVolumeRepository( resources->getVolumeRepository() );
        // widget->setMainCLUT( resources->getMainCLUT() );
        // widget->setMainVolumeID( resources->getMainVolumeID() );
        
    }
    
    return ok;
}

};  // end namespace udg 



