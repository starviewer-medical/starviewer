/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#ifndef UDGVOLUMEDISPLAYUNITHANDLERFACTORY_H
#define UDGVOLUMEDISPLAYUNITHANDLERFACTORY_H

#include <QList>
#include <QSharedPointer>

namespace udg {

class Volume;
class GenericVolumeDisplayUnitHandler;
class SingleVolumeDisplayUnitHandler;
class PairedVolumeDisplayUnitHandler;

/**
    Class to create the *VolumeDisplayUnitHandler that best fits for the given input(s)
 */
class VolumeDisplayUnitHandlerFactory {
public:
    VolumeDisplayUnitHandlerFactory();
    ~VolumeDisplayUnitHandlerFactory();

    /// Returns the proper display unit handler for the given input(s)
    QSharedPointer<GenericVolumeDisplayUnitHandler> createVolumeDisplayUnitHandler(Volume *input);
    QSharedPointer<GenericVolumeDisplayUnitHandler> createVolumeDisplayUnitHandler(const QList<Volume*> &inputs);

private:
    /// Concrete creators for the different kind of handlers
    QSharedPointer<SingleVolumeDisplayUnitHandler> createSingleVolumeDisplayUnitHandler(Volume *input);
    QSharedPointer<GenericVolumeDisplayUnitHandler> createGenericVolumeDisplayUnitHandler(const QList<Volume*> &inputs);
    QSharedPointer<PairedVolumeDisplayUnitHandler> createPairedVolumeDisplayUnitHandler(const QList<Volume*> &inputs);

    /// Chooses the best handler among the family of single handlers
    QSharedPointer<SingleVolumeDisplayUnitHandler> chooseBestSingleVolumeDisplayUnitHandler(Volume *input);

    /// Chooses the best handler among the family of paired handlers
    QSharedPointer<PairedVolumeDisplayUnitHandler> chooseBestPairedVolumeDisplayUnitHandler(const QList<Volume*> &inputs);
};

} // End namespace udg

#endif
