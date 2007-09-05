/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGCARDIAC2DVIEWEREXTENSIONMEDIATOR_H
#define UDGCARDIAC2DVIEWEREXTENSIONMEDIATOR_H

#include "extensionmediator.h"

#include "installextension.h"
#include "qcardiac2dviewerextension.h"

namespace udg {

/**
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class Cardiac2DViewerExtensionMediator : public ExtensionMediator
{
public:
    Cardiac2DViewerExtensionMediator(QObject *parent = 0);

    ~Cardiac2DViewerExtensionMediator();

    virtual bool initializeExtension(QWidget* extension, const ExtensionContext &extensionContext);

    virtual DisplayableID getExtensionID() const;
};

static InstallExtension<QCardiac2DViewerExtension, Cardiac2DViewerExtensionMediator> registerCardiac2DViewerExtension;

}

#endif
