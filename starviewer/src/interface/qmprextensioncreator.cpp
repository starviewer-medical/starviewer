/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qmprextensioncreator.h"
#include "qmprextension.h"

namespace udg {

QMPRExtensionCreator::QMPRExtensionCreator(QObject *parent, const char *name)
 : ExtensionCreator(parent, name)
{
}


QMPRExtensionCreator::~QMPRExtensionCreator()
{
}

QWidget *QMPRExtensionCreator::createExtension( QWidget *parent , QString name )
{
    QMPRExtension *newWidget = new QMPRExtension( parent );
    newWidget->setObjectName( name );
    return newWidget;
}

bool QMPRExtensionCreator::initializeExtension( QWidget *extension /* , Resource *resources */ )
{
    bool ok = true;
    
    if( extension != 0 )
    {
        ok = false;
    }
    else
    {
        QMPRExtension *widget = dynamic_cast< QMPRExtension * >( extension );
        // fer inits
        // widget->setVolumeRepository( resources->getVolumeRepository() );
        // widget->setMainCLUT( resources->getMainCLUT() );
        // widget->setMainVolumeID( resources->getMainVolumeID() );
        
    }
    
    return ok;
    
}

};  // end namespace udg {
