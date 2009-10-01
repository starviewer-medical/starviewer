/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDTIEXTENSIONMEDIATOR_H
#define UDGDTIEXTENSIONMEDIATOR_H

#include "extensionmediator.h"

#include "installextension.h"
#include "dtiextension.h"


namespace udg {


/**
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class DTIExtensionMediator : public ExtensionMediator
{
Q_OBJECT
public:
    DTIExtensionMediator(QObject *parent = 0);

    ~DTIExtensionMediator();

    virtual bool initializeExtension(QWidget* extension, const ExtensionContext &extensionContext);

    virtual DisplayableID getExtensionID() const;
};

static InstallExtension<DTIExtension, DTIExtensionMediator> registerDTIExtension;

}

#endif
