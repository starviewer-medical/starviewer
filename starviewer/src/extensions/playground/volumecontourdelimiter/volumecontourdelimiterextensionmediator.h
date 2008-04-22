/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef VOLUMECONTOURDELIMITEREREXTENSIONMEDIATOR_H
#define VOLUMECONTOURDELIMITEREREXTENSIONMEDIATOR_H

#include "extensionmediator.h"

#include "installextension.h"
#include "qvolumecontourdelimiterextension.h"

namespace udg{

/**
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class VolumeContourDelimiterExtensionMediator: public ExtensionMediator
{
    Q_OBJECT
public:
    VolumeContourDelimiterExtensionMediator(QObject *parent = 0);

    ~VolumeContourDelimiterExtensionMediator();

    virtual bool initializeExtension(QWidget* extension, const ExtensionContext &extensionContext);

    virtual DisplayableID getExtensionID() const;
};

static InstallExtension<QVolumeContourDelimiterExtension, VolumeContourDelimiterExtensionMediator> VolumeContourDelimiterExtension;


} //udg namespace

#endif
