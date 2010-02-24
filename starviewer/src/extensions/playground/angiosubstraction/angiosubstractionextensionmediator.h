/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGANGIOSUBSTRACTIONEXTENSIONMEDIATOR_H
#define UDGANGIOSUBSTRACTIONEXTENSIONMEDIATOR_H

#include "extensionmediator.h"

#include "installextension.h"
#include "qangiosubstractionextension.h"

namespace udg {

/**
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class AngioSubstractionExtensionMediator : public ExtensionMediator
{
Q_OBJECT
public:
    AngioSubstractionExtensionMediator(QObject *parent = 0);

    ~AngioSubstractionExtensionMediator();

    virtual bool initializeExtension(QWidget* extension, const ExtensionContext &extensionContext);

    virtual DisplayableID getExtensionID() const;
};

static InstallExtension<QAngioSubstractionExtension, AngioSubstractionExtensionMediator> registerAngioSubstractionExtension;

}

#endif
