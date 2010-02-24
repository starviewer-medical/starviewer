/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "angiosubstractionextensionmediator.h"

#include "extensioncontext.h"

// QT
#include <QMessageBox>

namespace udg {

AngioSubstractionExtensionMediator::AngioSubstractionExtensionMediator(QObject *parent)
 : ExtensionMediator(parent)
{
}

AngioSubstractionExtensionMediator::~AngioSubstractionExtensionMediator()
{
}

DisplayableID AngioSubstractionExtensionMediator::getExtensionID() const
{
    return DisplayableID("AngioSubstractionExtension",tr("Angio Substraction"));
}

bool AngioSubstractionExtensionMediator::initializeExtension(QWidget* extension, const ExtensionContext &extensionContext)
{
    QAngioSubstractionExtension *angioSubstractionExtension;

    if ( !(angioSubstractionExtension = qobject_cast<QAngioSubstractionExtension*>(extension)) )
    {
        return false;
    }

    Volume *input = extensionContext.getDefaultVolume();
    if( !input )
        QMessageBox::information(0,tr("Starviewer"), tr("The selected item is not an image") );
    else
        angioSubstractionExtension->setInput( input );

    return true;
}

}
