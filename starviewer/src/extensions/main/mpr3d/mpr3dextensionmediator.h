/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef MPR3DEXTENSIONMEDIATOR_H
#define MPR3DEXTENSIONMEDIATOR_H

#include "extensionmediator.h"

#include "installextension.h"
#include "qmpr3dextension.h"

namespace udg{

/**
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class MPR3DExtensionMediator: public ExtensionMediator
{
public:
    MPR3DExtensionMediator(QObject *parent = 0);

    ~MPR3DExtensionMediator();

    virtual bool initializeExtension(QWidget* extension, const ExtensionContext &extensionContext);

    virtual DisplayableID getExtensionID() const;
};

static InstallExtension<QMPR3DExtension, MPR3DExtensionMediator> registerMPR3DExtension;


} //udg namespace

#endif
