/**************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGRECTUMSEGMENTATIONEXTENSIONMEDIATOR_H
#define UDGRECTUMSEGMENTATIONEXTENSIONMEDIATOR_H

#include "extensionmediator.h"

#include "installextension.h"
#include "qrectumsegmentationextension.h"

namespace udg {

/**
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class RectumSegmentationExtensionMediator : public ExtensionMediator
{
public:
    RectumSegmentationExtensionMediator(QObject *parent = 0);

    ~RectumSegmentationExtensionMediator();

    virtual bool initializeExtension(QWidget* extension, const ExtensionContext &extensionContext, ExtensionHandler* extensionHandler);

    virtual DisplayableID getExtensionID() const;
};

static InstallExtension<QRectumSegmentationExtension, RectumSegmentationExtensionMediator> registerRectumSegmentationExtension;

}

#endif
