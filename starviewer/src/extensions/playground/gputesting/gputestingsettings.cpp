#include "gputestingsettings.h"

namespace udg {

// Definició de les claus
const QString KeyPrefix("GpuTesting/");
const QString GPUTestingSettings::TransferFunctionFilesPath( KeyPrefix + "transferFunctionDir" );
const QString GPUTestingSettings::CameraFilesPath( KeyPrefix + "cameraDir" );

GPUTestingSettings::GPUTestingSettings()
{
}

GPUTestingSettings::~GPUTestingSettings()
{
}

void GPUTestingSettings::init()
{    
}

} // end namespace udg 