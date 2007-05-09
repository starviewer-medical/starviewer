/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef DIFFUSIONPERFUSIONSEGMENTATIONEXTENSIONMEDIATOR_H
#define DIFFUSIONPERFUSIONSEGMENTATIONEXTENSIONMEDIATOR_H

#include "extensionmediator.h"

#include "installextension.h"
#include "qdifuperfuextension.h"

namespace udg{

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class DiffusionPerfusionSegmentationExtensionMediator: public ExtensionMediator
{
public:
    DiffusionPerfusionSegmentationExtensionMediator(QObject *parent = 0);

    ~DiffusionPerfusionSegmentationExtensionMediator();

    virtual bool initializeExtension(QWidget* extension, ExtensionHandler* extensionHandler, Identifier mainVolumeID);

    virtual DisplayableID getExtensionID() const;
};

static InstallExtension<QDifuPerfuSegmentationExtension, DiffusionPerfusionSegmentationExtensionMediator> registerDiffuPerfuExtension;


} //udg namespace

#endif
