/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef MPR3D2DEXTENSIONMEDIATOR_H
#define MPR3D2DEXTENSIONMEDIATOR_H

#include "extensionmediator.h"

#include "installextension.h"
#include "qmpr3d2dextension.h"

namespace udg{

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class MPR3D2DExtensionMediator: public ExtensionMediator
{
public:
    MPR3D2DExtensionMediator(QObject *parent = 0);

    ~MPR3D2DExtensionMediator();

    virtual bool initializeExtension(QWidget* extension, ExtensionHandler* extensionHandler, Identifier mainVolumeID);

    virtual DisplayableID getExtensionID() const;
};

static InstallExtension<QMPR3D2DExtension, MPR3D2DExtensionMediator> registerMPR3D2DExtension;


} //udg namespace

#endif
