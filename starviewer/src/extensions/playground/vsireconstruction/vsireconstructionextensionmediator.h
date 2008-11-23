/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGVSIRECONSTRUCTIONEXTENSIONMEDIATOR_H
#define UDGVSIRECONSTRUCTIONEXTENSIONMEDIATOR_H

#include "extensionmediator.h"

#include "installextension.h"
#include "qvsireconstructionextension.h"

namespace udg {

/**
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class VSIReconstructionExtensionMediator : public ExtensionMediator
{
Q_OBJECT
public:
    VSIReconstructionExtensionMediator(QObject *parent = 0);

    ~VSIReconstructionExtensionMediator();

    virtual bool initializeExtension(QWidget* extension, const ExtensionContext &extensionContext);

    virtual DisplayableID getExtensionID() const;
};

static InstallExtension<QVSIReconstructionExtension, VSIReconstructionExtensionMediator> registerVSIReconstructionExtension;

}

#endif
