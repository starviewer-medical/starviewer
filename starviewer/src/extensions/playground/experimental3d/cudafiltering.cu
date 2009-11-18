// tot el que tingui prefix d és del dispositiu

#include "cudafiltering.h"

#include <iostream>

#include <cuda.h>
#include <cutil.h>

#include <vtkImageData.h>


//Round a / b to nearest higher integer value
__device__ __host__ int iDivUp(int a, int b)
{
    return (a % b != 0) ? (a / b + 1) : (a / b);
}


texture<float, 3> gVolumeTexture;   // el 3r paràmetre pot ser cudaReadModeElementType (valor directe) (predeterminat) o cudaReadModeNormalizedFloat (valor escalat entre 0 i 1)


__global__ void convolutionXKernel(float *result, float *kernel, int radius, cudaExtent dims)
{
    uint blocksX = iDivUp(dims.width, blockDim.x);
    uint blockX = blockIdx.x % blocksX;
    uint blockY = blockIdx.x / blocksX;
    uint blockZ = blockIdx.y;

    uint x = blockX * blockDim.x + threadIdx.x;
    if (x >= dims.width) return;
    uint y = blockY * blockDim.y + threadIdx.y;
    if (y >= dims.height) return;
    uint z = blockZ * blockDim.z + threadIdx.z;
    if (z >= dims.depth) return;

    float fx = x + 0.5f, fy = y + 0.5f, fz = z + 0.5f;

    float sum = 0.0f;

    for (int k = -radius; k <= radius; k++) sum += tex3D(gVolumeTexture, fx + k, fy, fz) * kernel[radius - k];

    uint i = x + y * dims.width + z * dims.width * dims.height;

    result[i] = sum;
}


__global__ void convolutionYKernel(float *result, float *kernel, int radius, cudaExtent dims)
{
    uint blocksX = iDivUp(dims.width, blockDim.x);
    uint blockX = blockIdx.x % blocksX;
    uint blockY = blockIdx.x / blocksX;
    uint blockZ = blockIdx.y;

    uint x = blockX * blockDim.x + threadIdx.x;
    if (x >= dims.width) return;
    uint y = blockY * blockDim.y + threadIdx.y;
    if (y >= dims.height) return;
    uint z = blockZ * blockDim.z + threadIdx.z;
    if (z >= dims.depth) return;

    float fx = x + 0.5f, fy = y + 0.5f, fz = z + 0.5f;

    float sum = 0.0f;

    for (int k = -radius; k <= radius; k++) sum += tex3D(gVolumeTexture, fx, fy + k, fz) * kernel[radius - k];

    uint i = x + y * dims.width + z * dims.width * dims.height;

    result[i] = sum;
}


__global__ void convolutionZKernel(float *result, float *kernel, int radius, cudaExtent dims)
{
    uint blocksX = iDivUp(dims.width, blockDim.x);
    uint blockX = blockIdx.x % blocksX;
    uint blockY = blockIdx.x / blocksX;
    uint blockZ = blockIdx.y;

    uint x = blockX * blockDim.x + threadIdx.x;
    if (x >= dims.width) return;
    uint y = blockY * blockDim.y + threadIdx.y;
    if (y >= dims.height) return;
    uint z = blockZ * blockDim.z + threadIdx.z;
    if (z >= dims.depth) return;

    float fx = x + 0.5f, fy = y + 0.5f, fz = z + 0.5f;

    float sum = 0.0f;

    for (int k = -radius; k <= radius; k++) sum += tex3D(gVolumeTexture, fx, fy, fz + k) * kernel[radius - k];

    uint i = x + y * dims.width + z * dims.width * dims.height;

    result[i] = sum;
}


__global__ void substractionKernel(float *result, cudaExtent dims)
{
    uint blocksX = iDivUp(dims.width, blockDim.x);
    uint blockX = blockIdx.x % blocksX;
    uint blockY = blockIdx.x / blocksX;
    uint blockZ = blockIdx.y;

    uint x = blockX * blockDim.x + threadIdx.x;
    if (x >= dims.width) return;
    uint y = blockY * blockDim.y + threadIdx.y;
    if (y >= dims.height) return;
    uint z = blockZ * blockDim.z + threadIdx.z;
    if (z >= dims.depth) return;

    float fx = x + 0.5f, fy = y + 0.5f, fz = z + 0.5f;
    float value = tex3D(gVolumeTexture, fx, fy, fz);
    uint i = x + y * dims.width + z * dims.width * dims.height;
    result[i] -= value;
}


QVector<float> cfGaussianDifference(vtkImageData *image, float radius)
{
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start, 0);

    float *data = reinterpret_cast<float*>(image->GetScalarPointer());
    const uint VOLUME_DATA_SIZE = image->GetNumberOfPoints();
    int *dimensions = image->GetDimensions();
    cudaExtent volumeDataDims = make_cudaExtent(dimensions[0], dimensions[1], dimensions[2]);

    // Copiar el volum a un array i associar-hi una textura
    cudaArray *dVolumeArray;
    cudaChannelFormatDesc channelDescVolumeArray = cudaCreateChannelDesc<float>();
    CUDA_SAFE_CALL( cudaMalloc3DArray(&dVolumeArray, &channelDescVolumeArray, volumeDataDims) );
    cudaMemcpy3DParms copyParams = {0};
    copyParams.srcPtr = make_cudaPitchedPtr(reinterpret_cast<void*>(data), dimensions[0] * sizeof(float), dimensions[0], dimensions[1]);    // data, pitch, width, height
    copyParams.dstArray = dVolumeArray;
    copyParams.extent = volumeDataDims;
    copyParams.kind = cudaMemcpyHostToDevice;
    CUDA_SAFE_CALL( cudaMemcpy3D(&copyParams) );
    //gVolumeTexture.normalized = false;                      // false (predeterminat) -> [0,N) | true -> [0,1)
    //gVolumeTexture.filterMode = cudaFilterModePoint;        // cudaFilterModePoint (predeterminat) o cudaFilterModeLinear
    //gVolumeTexture.addressMode[0] = cudaAddressModeClamp;   // cudaAddressModeClamp (retallar) (predeterminat) o cudaAddressModeWrap (fer la volta)
    //gVolumeTexture.addressMode[1] = cudaAddressModeClamp;
    //gVolumeTexture.addressMode[2] = cudaAddressModeClamp;
    CUDA_SAFE_CALL( cudaBindTextureToArray(gVolumeTexture, dVolumeArray, channelDescVolumeArray) );

    // Reservar espai pel resultat
    float *dfResult;
    CUDA_SAFE_CALL( cudaMalloc(reinterpret_cast<void**>(&dfResult), VOLUME_DATA_SIZE * sizeof(float)) );

    // Calcular kernel
    const int RADIUS = static_cast<int>(ceil(radius));
    const int KERNEL_WIDTH = 2 * RADIUS + 1;
    QVector<float> kernel(KERNEL_WIDTH);
    float kernelSum = 0.0f;
    for (int i = 0; i < KERNEL_WIDTH; i++)
    {
        float distance = (i - radius) / radius;
        kernel[i] = expf(-distance * distance / 2.0f);
        kernelSum += kernel.at(i);
    }
    for (int i = 0; i < KERNEL_WIDTH; i++) kernel[i] /= kernelSum;
    float *dfKernel;
    CUDA_SAFE_CALL( cudaMalloc(reinterpret_cast<void**>(&dfKernel), KERNEL_WIDTH * sizeof(float)) );
    CUDA_SAFE_CALL( cudaMemcpy(reinterpret_cast<void*>(dfKernel), reinterpret_cast<void*>(kernel.data()), KERNEL_WIDTH * sizeof(float), cudaMemcpyHostToDevice) );

    // Preparar l'execució
    //Block width should be a multiple of maximum coalesced write size
    //for coalesced memory writes in convolutionRowGPU() and convolutionColumnGPU()
    dim3 threadBlock(16, 8, 4);
    uint blocksX = iDivUp(volumeDataDims.width, threadBlock.x);
    uint blocksY = iDivUp(volumeDataDims.height, threadBlock.y);
    uint blocksZ = iDivUp(volumeDataDims.depth, threadBlock.z);
    dim3 blockGrid(blocksX * blocksY, blocksZ);

    // Executar per X
    CUDA_SAFE_CALL( cudaThreadSynchronize() );
    convolutionXKernel<<<blockGrid, threadBlock>>>(dfResult, dfKernel, RADIUS, volumeDataDims);
    CUT_CHECK_ERROR( "convolutionXKernel() execution failed\n" );
    CUDA_SAFE_CALL( cudaThreadSynchronize() );

    // Copiar el resultat a l'array
    copyParams.srcPtr = make_cudaPitchedPtr(reinterpret_cast<void*>(dfResult), dimensions[0] * sizeof(float), dimensions[0], dimensions[1]);    // data, pitch, width, height
    copyParams.kind = cudaMemcpyDeviceToDevice;
    CUDA_SAFE_CALL( cudaMemcpy3D(&copyParams) );
    CUDA_SAFE_CALL( cudaThreadSynchronize() );

    // Executar per Y
    convolutionYKernel<<<blockGrid, threadBlock>>>(dfResult, dfKernel, RADIUS, volumeDataDims);
    CUT_CHECK_ERROR( "convolutionYKernel() execution failed\n" );
    CUDA_SAFE_CALL( cudaThreadSynchronize() );

    // Copiar el resultat a l'array
    CUDA_SAFE_CALL( cudaMemcpy3D(&copyParams) );
    CUDA_SAFE_CALL( cudaThreadSynchronize() );

    // Executar per Z
    convolutionZKernel<<<blockGrid, threadBlock>>>(dfResult, dfKernel, RADIUS, volumeDataDims);
    CUT_CHECK_ERROR( "convolutionZKernel() execution failed\n" );
    CUDA_SAFE_CALL( cudaThreadSynchronize() );

    // Copiar el volum original un altre cop a l'array
    copyParams.srcPtr = make_cudaPitchedPtr(reinterpret_cast<void*>(data), dimensions[0] * sizeof(float), dimensions[0], dimensions[1]);    // data, pitch, width, height
    copyParams.kind = cudaMemcpyHostToDevice;
    CUDA_SAFE_CALL( cudaMemcpy3D(&copyParams) );
    CUDA_SAFE_CALL( cudaThreadSynchronize() );

    // Resta
    substractionKernel<<<blockGrid, threadBlock>>>(dfResult, volumeDataDims);
    CUT_CHECK_ERROR( "substractionKernel() execution failed\n" );
    CUDA_SAFE_CALL( cudaThreadSynchronize() );

    // Copiar el resultat final al host
    QVector<float> result(VOLUME_DATA_SIZE);
    CUDA_SAFE_CALL( cudaMemcpy(reinterpret_cast<void*>(result.data()), reinterpret_cast<void*>(dfResult), VOLUME_DATA_SIZE * sizeof(float), cudaMemcpyDeviceToHost) );

    // Neteja
    CUDA_SAFE_CALL( cudaFree(dfKernel) );
    CUDA_SAFE_CALL( cudaFree(dfResult) );
    CUDA_SAFE_CALL( cudaUnbindTexture(gVolumeTexture) );
    CUDA_SAFE_CALL( cudaFreeArray(dVolumeArray) );

    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    float elapsedTime = 0.0f;
    cudaEventElapsedTime(&elapsedTime, start, stop);

    std::cout << "gaussian difference: " << elapsedTime << " ms" << std::endl;

    cudaEventDestroy(start);
    cudaEventDestroy(stop);

    return result;
}
