/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qtabaxisviewextensioncreator.h"

#include "qtabaxisview.h" // \TODO de moment deixem així el nom ja que encara estem en fase de proves, més endavant canviaríem el nom per el convingut, com podria ser QTabAxisViewWidget o QTabAxisViewExtension
namespace udg {

QTabAxisViewExtensionCreator::QTabAxisViewExtensionCreator(QObject *parent, const char *name)
 : ExtensionCreator(parent, name)
{
}

QTabAxisViewExtensionCreator::~QTabAxisViewExtensionCreator()
{
}

QWidget *QTabAxisViewExtensionCreator::createExtension( QWidget *parent , QString name )
{
    QTabAxisView *newWidget = new QTabAxisView( parent );
    newWidget->setObjectName( name );
    return newWidget;
}

bool QTabAxisViewExtensionCreator::initializeExtension( QWidget *extension /* , Resource *resources */ )
{
    bool ok = true;
    
    if( extension != 0 )
    {
        ok = false;
    }
    else
    {
        QTabAxisView *widget = dynamic_cast< QTabAxisView * >( extension );
        // fer inits
        // widget->setVolumeRepository( resources->getVolumeRepository() );
        // widget->setMainCLUT( resources->getMainCLUT() );
        // widget->setMainVolumeID( resources->getMainVolumeID() );
        
    }
    
    return ok;
    
}

};  // end namespace udg {
