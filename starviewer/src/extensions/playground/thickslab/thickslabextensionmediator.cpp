/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "thickslabextensionmediator.h"

#include "extensioncontext.h"
// QT
#include <QMessageBox>

namespace udg{

ThickSlabExtensionMediator::ThickSlabExtensionMediator(QObject *parent)
 : ExtensionMediator(parent)
{
}

ThickSlabExtensionMediator::~ThickSlabExtensionMediator()
{
}

DisplayableID ThickSlabExtensionMediator::getExtensionID() const
{
    return DisplayableID("ThickSlabExtension",tr("Thick Slab"));
}

bool ThickSlabExtensionMediator::initializeExtension(QWidget* extension, const ExtensionContext &extensionContext)
{
    ThickSlabExtension *thickslabExtension;

    if ( !(thickslabExtension = qobject_cast<ThickSlabExtension*>(extension)) )
    {
        return false;
    }

    thickslabExtension->setPatient( extensionContext.getPatient() );
    Volume *input = extensionContext.getDefaultVolume();
    if( !input )
        QMessageBox::information(0,tr("Starviewer"), tr("The selected item is not an image") );
    else
        thickslabExtension->setInput( input );

    return true;
}

} //udg namespace
