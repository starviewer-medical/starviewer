/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "mpr2dextensionmediator.h"

#include "extensioncontext.h"

// QT
#include <QMessageBox>

namespace udg{

MPR2DExtensionMediator::MPR2DExtensionMediator(QObject *parent)
 : ExtensionMediator(parent)
{
}

MPR2DExtensionMediator::~MPR2DExtensionMediator()
{
}

DisplayableID MPR2DExtensionMediator::getExtensionID() const
{
    return DisplayableID("MPR2DExtension",tr("Enhanced MPR 2D"));
}

bool MPR2DExtensionMediator::initializeExtension(QWidget* extension, const ExtensionContext &extensionContext)
{
    QMPR2DExtension *mprExtension;

    if ( !(mprExtension = qobject_cast<QMPR2DExtension*>(extension)) )
    {
        return false;
    }

    Volume *input = extensionContext.getDefaultVolume();
    if( !input )
        QMessageBox::information(0,tr("Starviewer"), tr("The selected item is not an image") );
    else
        mprExtension->setInput( input );

    return true;
}

} //udg namespace
