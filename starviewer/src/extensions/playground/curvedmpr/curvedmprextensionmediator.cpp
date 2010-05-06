/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "curvedmprextensionmediator.h"

#include "extensioncontext.h"

// Qt
#include <QMessageBox>

namespace udg{

CurvedMPRExtensionMediator::CurvedMPRExtensionMediator(QObject *parent)
 : ExtensionMediator(parent)
{
}

CurvedMPRExtensionMediator::~CurvedMPRExtensionMediator()
{
}

DisplayableID CurvedMPRExtensionMediator::getExtensionID() const
{
    return DisplayableID("CurvedMPRExtension",tr("Curved MPR"));
}

bool CurvedMPRExtensionMediator::initializeExtension(QWidget* extension, const ExtensionContext &extensionContext)
{
    CurvedMPRExtension *curvedMPRExtension;

    if ( !(curvedMPRExtension = qobject_cast<CurvedMPRExtension*>(extension)) )
    {
        return false;
    }

    Volume *input = extensionContext.getDefaultVolume();
    if( !input )
        QMessageBox::information(0,tr("Starviewer"), tr("The selected item is not an image") );
    else
        curvedMPRExtension->setInput( input );

    return true;
}

} //udg namespace
