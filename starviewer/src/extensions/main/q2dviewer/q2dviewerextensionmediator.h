/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef Q2DVIEWEREXTENSIONMEDIATOR_H
#define Q2DVIEWEREXTENSIONMEDIATOR_H

#include "extensionmediator.h"

#include "installextension.h"
#include "q2dviewerextension.h"

namespace udg{

/**
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class Q2DViewerExtensionMediator: public ExtensionMediator
{
public:
    Q2DViewerExtensionMediator(QObject *parent = 0);

    ~Q2DViewerExtensionMediator();

    virtual bool initializeExtension(QWidget* extension, const ExtensionContext &extensionContext);

    virtual DisplayableID getExtensionID() const;
};

static InstallExtension<Q2DViewerExtension, Q2DViewerExtensionMediator> registerQ2DViewerExtension;


} //udg namespace

#endif
