#include "optimizetransferfunctioncommand.h"

#include "qexperimental3dextension.h"


namespace udg {


OptimizeTransferFunctionCommand* OptimizeTransferFunctionCommand::New()
{
    return new OptimizeTransferFunctionCommand();
}


void OptimizeTransferFunctionCommand::setExtension(QExperimental3DExtension *extension)
{
    m_extension = extension;
}


void OptimizeTransferFunctionCommand::Execute(vtkObject *caller, unsigned long eventId, void *callData)
{
    Q_UNUSED(caller);
    Q_UNUSED(callData);

    Q_ASSERT(m_extension);
    Q_ASSERT(eventId == ResetCameraEvent || eventId == ResetCameraClippingRangeEvent);

    m_extension->optimizeTransferFunctionForOneViewpoint();
}


OptimizeTransferFunctionCommand::OptimizeTransferFunctionCommand()
    : m_extension(0)
{
}


} // namespace udg
