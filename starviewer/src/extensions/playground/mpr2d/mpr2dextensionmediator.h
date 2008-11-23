/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef MPR2DEXTENSIONMEDIATOR_H
#define MPR2DEXTENSIONMEDIATOR_H

#include "extensionmediator.h"

#include "installextension.h"
#include "qmpr2dextension.h"

namespace udg{

/**
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class MPR2DExtensionMediator: public ExtensionMediator
{
    Q_OBJECT
public:
    MPR2DExtensionMediator(QObject *parent = 0);

    ~MPR2DExtensionMediator();

    virtual bool initializeExtension(QWidget* extension, const ExtensionContext &extensionContext);

    virtual DisplayableID getExtensionID() const;
};

static InstallExtension<QMPR2DExtension, MPR2DExtensionMediator> registerMPR2DExtension;


} //udg namespace

#endif
