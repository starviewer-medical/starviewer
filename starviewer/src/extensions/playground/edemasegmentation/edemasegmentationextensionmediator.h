/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGEDEMASEGMENTATIONEXTENSIONMEDIATOR_H
#define UDGEDEMASEGMENTATIONEXTENSIONMEDIATOR_H

#include "extensionmediator.h"

#include "installextension.h"
#include "qedemasegmentationextension.h"

namespace udg {

/**
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class EdemaSegmentationExtensionMediator : public ExtensionMediator
{
public:
    EdemaSegmentationExtensionMediator(QObject *parent = 0);

    ~EdemaSegmentationExtensionMediator();

    virtual bool initializeExtension(QWidget* extension, const ExtensionContext &extensionContext);

    virtual DisplayableID getExtensionID() const;
};

static InstallExtension<QEdemaSegmentationExtension, EdemaSegmentationExtensionMediator> registerEdemaSegmentationExtension;

}

#endif
