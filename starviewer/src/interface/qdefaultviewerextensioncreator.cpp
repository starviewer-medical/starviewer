/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qdefaultviewerextensioncreator.h"
#include "qdefaultviewerextension.h"

namespace udg {

QDefaultViewerExtensionCreator::QDefaultViewerExtensionCreator(QObject* parent, const char* name)
: ExtensionCreator(parent, name)
{
}


QDefaultViewerExtensionCreator::~QDefaultViewerExtensionCreator()
{
}

QWidget *QDefaultViewerExtensionCreator::createExtension( QWidget *parent , QString name )
{
    QDefaultViewerExtension *newWidget = new QDefaultViewerExtension( parent );
    newWidget->setObjectName( name );
    return newWidget;
}

bool QDefaultViewerExtensionCreator::initializeExtension( QWidget *extension /* , Resource *resources */ )
{
    bool ok = true;
    
    if( extension != 0 )
    {
        ok = false;
    }
    else
    {
        QDefaultViewerExtension *widget = dynamic_cast< QDefaultViewerExtension * >( extension );
        // fer inits
        // widget->setVolumeRepository( resources->getVolumeRepository() );
        // widget->setMainCLUT( resources->getMainCLUT() );
        // widget->setMainVolumeID( resources->getMainVolumeID() );
        
    }
    
    return ok;
    
}

} // end namespace udg
