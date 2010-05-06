/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef CURVEDMPREXTENSIONMEDIATOR_H
#define CURVEDMPREXTENSIONMEDIATOR_H

#include "extensionmediator.h"

#include "installextension.h"
#include "curvedmprextension.h"

namespace udg{

/**
    Mediator per l'extensió de l'MPR Curvilini
*/
class CurvedMPRExtensionMediator: public ExtensionMediator {
Q_OBJECT
public:
    CurvedMPRExtensionMediator(QObject *parent = 0);
    ~CurvedMPRExtensionMediator();

    virtual bool initializeExtension(QWidget* extension, const ExtensionContext &extensionContext);

    virtual DisplayableID getExtensionID() const;
};

static InstallExtension<CurvedMPRExtension, CurvedMPRExtensionMediator> registerCurvedMPRExtension;


} //udg namespace

#endif
