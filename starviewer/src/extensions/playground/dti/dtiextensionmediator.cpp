/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "dtiextensionmediator.h"

#include "extensioncontext.h"

// QT
#include <QMessageBox>

namespace udg {

DTIExtensionMediator::DTIExtensionMediator(QObject *parent)
 : ExtensionMediator(parent)
{
}

DTIExtensionMediator::~DTIExtensionMediator()
{
}

DisplayableID DTIExtensionMediator::getExtensionID() const
{
    return DisplayableID("DTIExtension",tr("DTI"));
}

bool DTIExtensionMediator::initializeExtension(QWidget* extension, const ExtensionContext &extensionContext)
{
    DTIExtension *dtiExtension;

    if ( !(dtiExtension = qobject_cast<DTIExtension*>(extension)) )
    {
        return false;
    }

    Volume *input = extensionContext.getDefaultVolume();
    if( !input )
        QMessageBox::information(0,tr("Starviewer"), tr("The selected item is not an image") );
    else
        dtiExtension->setInput( input );

    return true;
}

}
