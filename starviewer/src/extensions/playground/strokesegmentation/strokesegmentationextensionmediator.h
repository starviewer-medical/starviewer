/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGSTROKESEGMENTATIONEXTENSIONMEDIATOR_H
#define UDGSTROKESEGMENTATIONEXTENSIONMEDIATOR_H

#include "extensionmediator.h"

#include "installextension.h"
#include "qstrokesegmentationextension.h"

namespace udg {

/**
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class StrokeSegmentationExtensionMediator : public ExtensionMediator
{
Q_OBJECT
public:
    StrokeSegmentationExtensionMediator(QObject *parent = 0);

    ~StrokeSegmentationExtensionMediator();

    virtual bool initializeExtension(QWidget* extension, const ExtensionContext &extensionContext, ExtensionHandler* extensionHandler);

    virtual DisplayableID getExtensionID() const;
};

static InstallExtension<QStrokeSegmentationExtension, StrokeSegmentationExtensionMediator> registerStrokeSegmentationExtension;

}

#endif
