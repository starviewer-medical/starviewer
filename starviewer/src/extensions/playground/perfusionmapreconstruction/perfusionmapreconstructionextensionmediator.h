/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGPERFUSIONMAPRECONSTRUCTIONEXTENSIONMEDIATOR_H
#define UDGPERFUSIONMAPRECONSTRUCTIONEXTENSIONMEDIATOR_H

#include "extensionmediator.h"

#include "installextension.h"
#include "qperfusionmapreconstructionextension.h"

namespace udg {

/**
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class PerfusionMapReconstructionExtensionMediator : public ExtensionMediator
{
Q_OBJECT
public:
    PerfusionMapReconstructionExtensionMediator(QObject *parent = 0);

    ~PerfusionMapReconstructionExtensionMediator();

    virtual bool initializeExtension(QWidget* extension, const ExtensionContext &extensionContext);

    virtual DisplayableID getExtensionID() const;
};

static InstallExtension<QPerfusionMapReconstructionExtension, PerfusionMapReconstructionExtensionMediator> registerPerfusionMapReconstructionExtension;

}

#endif
