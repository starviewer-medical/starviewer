#include "genericvolumedisplayunithandler.h"

#include "mathtools.h"
#include "logging.h"
#include "volumedisplayunit.h"
#include "imagepipeline.h"

#include <vtkImageActor.h>

namespace udg {

GenericVolumeDisplayUnitHandler::GenericVolumeDisplayUnitHandler()
{
}

GenericVolumeDisplayUnitHandler::~GenericVolumeDisplayUnitHandler()
{
    removeDisplayUnits();
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

void GenericVolumeDisplayUnitHandler::removeDisplayUnits()
{
    foreach (VolumeDisplayUnit *unit, m_displayUnits)
    {
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
    displayUnit->setVolume(input);
}

void GenericVolumeDisplayUnitHandler::setupDisplayUnits()
{
    updateMainDisplayUnitIndex();
    setupDefaultOpacities();
    initializeTransferFunctions();
}

void GenericVolumeDisplayUnitHandler::setupDefaultOpacities()
{
    // The default opacities will be 1 for the main volume and 0.5 for the others
    for (int i = 0; i < getNumberOfInputs(); i++)
    {
        vtkImageActor *actor = m_displayUnits.at(i)->getImageActor();
        double opacity = 0.5;
        if (i == 0)
        {
            opacity = 1.0;
        }
        
        actor->SetOpacity(opacity);
    }
}

void GenericVolumeDisplayUnitHandler::initializeTransferFunctions()
{
    // By default, all volumes have no transfer function
    foreach (VolumeDisplayUnit *unit, m_displayUnits)
    {
        unit->getImagePipeline()->clearTransferFunction();
    }

    setupDefaultTransferFunctions();
}

} // End namespace udg
