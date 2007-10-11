/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGSEGMENTATIONFRAMEWORKEXTENSIONMEDIATOR_H
#define UDGSEGMENTATIONFRAMEWORKEXTENSIONMEDIATOR_H

#include "extensionmediator.h"

#include "installextension.h"
#include "qsegmentationframeworkextension.h"

namespace udg {

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class SegmentationFrameworkExtensionMediator : public ExtensionMediator
{
Q_OBJECT
public:
    SegmentationFrameworkExtensionMediator(QObject *parent = 0);

    ~SegmentationFrameworkExtensionMediator();

    virtual bool initializeExtension(QWidget* extension, const ExtensionContext &extensionContext);

    virtual DisplayableID getExtensionID() const;
};

static InstallExtension<QSegmentationFrameworkExtension, SegmentationFrameworkExtensionMediator> registerSegmentationFrameworkExtension;

}

#endif
