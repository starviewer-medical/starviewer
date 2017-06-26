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

#include "genericvolumedisplayunithandler.h"

#include "mathtools.h"
#include "logging.h"
#include "volumedisplayunit.h"
#include "imagepipeline.h"
#include "transferfunctionmodel.h"
#include "transferfunctionmodelfiller.h"
#include "volume.h"

#include <vtkImageProperty.h>
#include <vtkImageSlice.h>
#include <vtkImageSliceCollection.h>
#include <vtkImageStack.h>

namespace udg {

GenericVolumeDisplayUnitHandler::GenericVolumeDisplayUnitHandler(QObject *parent)
    : QObject(parent)
{
    m_transferFunctionModel = new TransferFunctionModel();
    m_imageStack = vtkImageStack::New();
    TransferFunctionModelFiller().add2DTransferFunctions(getTransferFunctionModel());
}

GenericVolumeDisplayUnitHandler::~GenericVolumeDisplayUnitHandler()
{
    removeDisplayUnits();
    delete m_transferFunctionModel;
    m_imageStack->Delete();
}

void GenericVolumeDisplayUnitHandler::setInput(Volume *input)
{
    removeDisplayUnits();
    addDisplayUnit(input);
    setupDisplayUnits();
}

void GenericVolumeDisplayUnitHandler::setInputs(QList<Volume*> inputs)
{
    removeDisplayUnits();

    int supportedNumberOfInputs = qBound(0, inputs.size(), getMaximumNumberOfInputs());
    
    if (supportedNumberOfInputs < inputs.size())
    {
        DEBUG_LOG(QString("The number of the given inputs (%1) exceeds the maximum of the handler (%2). There will be only %2 handled.")
            .arg(inputs.size()).arg(getMaximumNumberOfInputs()));
    }
    
    for (int i = 0; i < supportedNumberOfInputs; ++i)
    {
        addDisplayUnit(inputs.at(i));
    }
    setupDisplayUnits();
}

bool GenericVolumeDisplayUnitHandler::hasInput() const
{
    return getNumberOfInputs() > 0;
}

int GenericVolumeDisplayUnitHandler::getNumberOfInputs() const
{
    return m_displayUnits.count();
}

VolumeDisplayUnit* GenericVolumeDisplayUnitHandler::getMainVolumeDisplayUnit() const
{
    return getVolumeDisplayUnit(0);
}

VolumeDisplayUnit* GenericVolumeDisplayUnitHandler::getVolumeDisplayUnit(int i) const
{
    if (MathTools::isInsideRange(i, 0, m_displayUnits.count() - 1))
    {
        return m_displayUnits.at(i);
    }
    else
    {
        return 0;
    }
}

QList<VolumeDisplayUnit*> GenericVolumeDisplayUnitHandler::getVolumeDisplayUnitList() const
{
    return m_displayUnits;
}

int GenericVolumeDisplayUnitHandler::getMaximumNumberOfInputs() const
{
    return std::numeric_limits<int>::max();
}

vtkImageSlice* GenericVolumeDisplayUnitHandler::getImageProp() const
{
    return m_imageStack;
}

TransferFunctionModel* GenericVolumeDisplayUnitHandler::getTransferFunctionModel() const
{
    return m_transferFunctionModel;
}

void GenericVolumeDisplayUnitHandler::removeDisplayUnits()
{
    foreach (VolumeDisplayUnit *unit, m_displayUnits)
    {
        removeImageSliceFromStack(unit->getImageStack());
        delete unit;
    }
    m_displayUnits.clear();
}

void GenericVolumeDisplayUnitHandler::addDisplayUnit(Volume *input)
{
    if (!input)
    {
        return;
    }

    VolumeDisplayUnit *displayUnit = new VolumeDisplayUnit();
    // Add the vdu to the list before setting the volume to avoid a memory leak if setVolume throws a bad_alloc
    m_displayUnits << displayUnit;
    connect(displayUnit, &VolumeDisplayUnit::imageStackChanged, this, &GenericVolumeDisplayUnitHandler::rebuildImageStack);
    displayUnit->setVolume(input);
}

void GenericVolumeDisplayUnitHandler::addImageSliceToStack(vtkImageSlice *imageSlice)
{
    if (auto stack = vtkImageStack::SafeDownCast(imageSlice))
    {
        vtkCollectionSimpleIterator it;
        stack->GetImages()->InitTraversal(it);

        while (auto *slice = stack->GetImages()->GetNextImage(it))
        {
            addImageSliceToStack(slice);
        }
    }
    else
    {
        m_imageStack->AddImage(imageSlice);
    }
}

void GenericVolumeDisplayUnitHandler::removeImageSliceFromStack(vtkImageSlice *imageSlice)
{
    if (auto stack = vtkImageStack::SafeDownCast(imageSlice))
    {
        vtkCollectionSimpleIterator it;
        stack->GetImages()->InitTraversal(it);

        while (auto *slice = stack->GetImages()->GetNextImage(it))
        {
            removeImageSliceFromStack(slice);
        }
    }
    else
    {
        m_imageStack->RemoveImage(imageSlice);
    }
}

void GenericVolumeDisplayUnitHandler::setupDisplayUnits()
{
    updateMainDisplayUnitIndex();
    rebuildImageStack();
    setupDefaultOpacities();
    initializeTransferFunctions();
}

void GenericVolumeDisplayUnitHandler::updateLayerNumbers()
{
    vtkCollectionSimpleIterator it;
    m_imageStack->GetImages()->InitTraversal(it);
    int i = 0;

    while (auto *slice = m_imageStack->GetImages()->GetNextImage(it))
    {
        slice->GetProperty()->SetLayerNumber(i);
        i++;
    }
}

void GenericVolumeDisplayUnitHandler::setupDefaultOpacities()
{
    // The default opacities will be 1 for the main volume and 0.5 for the others
    for (int i = 0; i < getNumberOfInputs(); i++)
    {
        m_displayUnits.at(i)->setOpacity(1.0);
    }
}

void GenericVolumeDisplayUnitHandler::initializeTransferFunctions()
{
    // By default, all volumes have no transfer function
    foreach (VolumeDisplayUnit *unit, m_displayUnits)
    {
        unit->clearTransferFunction();
    }

    setupDefaultTransferFunctions();
}

void GenericVolumeDisplayUnitHandler::rebuildImageStack()
{
    m_imageStack->GetImages()->RemoveAllItems();

    foreach (VolumeDisplayUnit *unit, m_displayUnits)
    {
        addImageSliceToStack(unit->getImageStack());
    }

    updateLayerNumbers();
}

} // End namespace udg
