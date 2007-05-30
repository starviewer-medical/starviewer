/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef MPREXTENSIONMEDIATOR_H
#define MPREXTENSIONMEDIATOR_H

#include "extensionmediator.h"

#include "installextension.h"
#include "qmprextension.h"

namespace udg{

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class MPRExtensionMediator: public ExtensionMediator
{
public:
    MPRExtensionMediator(QObject *parent = 0);

    ~MPRExtensionMediator();

    virtual bool initializeExtension(QWidget* extension, ExtensionHandler* extensionHandler, Identifier mainVolumeID);

    virtual DisplayableID getExtensionID() const;
};

static InstallExtension<QMPRExtension, MPRExtensionMediator> registerMPRExtension;


} //udg namespace

#endif
