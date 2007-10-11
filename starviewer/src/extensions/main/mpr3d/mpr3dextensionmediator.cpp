/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "mpr3dextensionmediator.h"

#include "extensioncontext.h"

// QT
#include <QMessageBox>

namespace udg{

MPR3DExtensionMediator::MPR3DExtensionMediator(QObject *parent)
 : ExtensionMediator(parent)
{
}

MPR3DExtensionMediator::~MPR3DExtensionMediator()
{
}

DisplayableID MPR3DExtensionMediator::getExtensionID() const
{
    return DisplayableID("MPR3DExtension",tr("MPR 3D"));
}

bool MPR3DExtensionMediator::initializeExtension(QWidget* extension, const ExtensionContext &extensionContext)
{
    QMPR3DExtension *mpr3dExtension;

    if ( !(mpr3dExtension = qobject_cast<QMPR3DExtension*>(extension)) )
    {
        return false;
    }

    Volume *input = extensionContext.getDefaultVolume();
    if( !input )
        QMessageBox::information(0,tr("Starviewer"), tr("The selected item is not an image") );
    else
        mpr3dExtension->setInput( input );

    return true;
}

} //udg namespace
