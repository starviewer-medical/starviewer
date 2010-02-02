// tot el que tingui prefix d és del dispositiu

#include "cudafiltering.h"

#include <iostream>

#include <cuda.h>
#include <cutil.h>
#include <cutil_math.h>
#include <math_constants.h>

#include <vtkImageData.h>


//Round a / b to nearest higher integer value
__device__ __host__ int iDivUp(int a, int b)
{
    return (a % b != 0) ? (a / b + 1) : (a / b);
}


texture<float, 3> gVolumeTexture;   // el 3r paràmetre pot ser cudaReadModeElementType (valor directe) (predeterminat) o cudaReadModeNormalizedFloat (valor escalat entre 0 i 1)
texture<float, 3> gVolume2Texture;  // el 3r paràmetre pot ser cudaReadModeElementType (valor directe) (predeterminat) o cudaReadModeNormalizedFloat (valor escalat entre 0 i 1)
texture<float, 3> gVolume3Texture;  // el 3r paràmetre pot ser cudaReadModeElementType (valor directe) (predeterminat) o cudaReadModeNormalizedFloat (valor escalat entre 0 i 1)


__global__ void convolutionXKernel(float *result, float *kernel, int radius, cudaExtent dims, bool texture2)
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

    if (!texture2)
    {
        for (int k = -radius; k <= radius; k++) sum += tex3D(gVolumeTexture, fx + k, fy, fz) * kernel[radius - k];
    }
    else
    {
        for (int k = -radius; k <= radius; k++) sum += tex3D(gVolume2Texture, fx + k, fy, fz) * kernel[radius - k];
    }

    uint i = x + y * dims.width + z * dims.width * dims.height;

    result[i] = sum;
}


__global__ void convolutionYKernel(float *result, float *kernel, int radius, cudaExtent dims, bool texture2)
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

    if (!texture2)
    {
        for (int k = -radius; k <= radius; k++) sum += tex3D(gVolumeTexture, fx, fy + k, fz) * kernel[radius - k];
    }
    else
    {
        for (int k = -radius; k <= radius; k++) sum += tex3D(gVolume2Texture, fx, fy + k, fz) * kernel[radius - k];
    }

    uint i = x + y * dims.width + z * dims.width * dims.height;

    result[i] = sum;
}


__global__ void convolutionZKernel(float *result, float *kernel, int radius, cudaExtent dims, bool texture2)
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

    if (!texture2)
    {
        for (int k = -radius; k <= radius; k++) sum += tex3D(gVolumeTexture, fx, fy, fz + k) * kernel[radius - k];
    }
    else
    {
        for (int k = -radius; k <= radius; k++) sum += tex3D(gVolume2Texture, fx, fy, fz + k) * kernel[radius - k];
    }

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


QVector<float> cfGaussianDifference(vtkImageData *image, int radius)
{
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start, 0);

    cudaEvent_t t0, t1;
    float t01 = 0.0f;
    cudaEventCreate(&t0);
    cudaEventCreate(&t1);

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
    const int KERNEL_WIDTH = 2 * radius + 1;
    QVector<float> kernel(KERNEL_WIDTH);
    float kernelSum = 0.0f;
    float sigma = radius / 3.0f;
    for (int i = 0; i < KERNEL_WIDTH; i++)
    {
        float f = static_cast<float>(i - radius) / sigma;
        kernel[i] = expf(-f * f / 2.0f);
        kernelSum += kernel.at(i);
    }
    for (int i = 0; i < KERNEL_WIDTH; i++) kernel[i] /= kernelSum;
    std::cout << "kernel:";
    for (int i = 0; i < KERNEL_WIDTH; i++) std::cout << " " << kernel[i];
    std::cout << std::endl;
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
    cudaEventRecord(t0, 0);
    convolutionXKernel<<<blockGrid, threadBlock>>>(dfResult, dfKernel, radius, volumeDataDims, false);
    CUT_CHECK_ERROR( "convolutionXKernel() execution failed\n" );
    CUDA_SAFE_CALL( cudaThreadSynchronize() );
    cudaEventRecord(t1, 0);
    cudaEventSynchronize(t1);
    cudaEventElapsedTime(&t01, t0, t1);
    std::cout << "X filter: " << t01 << " ms" << std::endl;

    // Copiar el resultat a l'array
    copyParams.srcPtr = make_cudaPitchedPtr(reinterpret_cast<void*>(dfResult), dimensions[0] * sizeof(float), dimensions[0], dimensions[1]);    // data, pitch, width, height
    copyParams.kind = cudaMemcpyDeviceToDevice;
    CUDA_SAFE_CALL( cudaMemcpy3D(&copyParams) );
    CUDA_SAFE_CALL( cudaThreadSynchronize() );

    // Executar per Y
    cudaEventRecord(t0, 0);
    convolutionYKernel<<<blockGrid, threadBlock>>>(dfResult, dfKernel, radius, volumeDataDims, false);
    CUT_CHECK_ERROR( "convolutionYKernel() execution failed\n" );
    CUDA_SAFE_CALL( cudaThreadSynchronize() );
    cudaEventRecord(t1, 0);
    cudaEventSynchronize(t1);
    cudaEventElapsedTime(&t01, t0, t1);
    std::cout << "Y filter: " << t01 << " ms" << std::endl;

    // Copiar el resultat a l'array
    CUDA_SAFE_CALL( cudaMemcpy3D(&copyParams) );
    CUDA_SAFE_CALL( cudaThreadSynchronize() );

    // Executar per Z
    cudaEventRecord(t0, 0);
    convolutionZKernel<<<blockGrid, threadBlock>>>(dfResult, dfKernel, radius, volumeDataDims, false);
    CUT_CHECK_ERROR( "convolutionZKernel() execution failed\n" );
    CUDA_SAFE_CALL( cudaThreadSynchronize() );
    cudaEventRecord(t1, 0);
    cudaEventSynchronize(t1);
    cudaEventElapsedTime(&t01, t0, t1);
    std::cout << "Z filter: " << t01 << " ms" << std::endl;

    // Copiar el volum original un altre cop a l'array
    copyParams.srcPtr = make_cudaPitchedPtr(reinterpret_cast<void*>(data), dimensions[0] * sizeof(float), dimensions[0], dimensions[1]);    // data, pitch, width, height
    copyParams.kind = cudaMemcpyHostToDevice;
    CUDA_SAFE_CALL( cudaMemcpy3D(&copyParams) );
    CUDA_SAFE_CALL( cudaThreadSynchronize() );

    // Resta
    cudaEventRecord(t0, 0);
    substractionKernel<<<blockGrid, threadBlock>>>(dfResult, volumeDataDims);
    CUT_CHECK_ERROR( "substractionKernel() execution failed\n" );
    CUDA_SAFE_CALL( cudaThreadSynchronize() );
    cudaEventRecord(t1, 0);
    cudaEventSynchronize(t1);
    cudaEventElapsedTime(&t01, t0, t1);
    std::cout << "substraction: " << t01 << " ms" << std::endl;

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
    cudaEventDestroy(t0);
    cudaEventDestroy(t1);

    return result;
}


QVector<float> cfBoxMeanDifference(vtkImageData *image, int radius)
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
    const int KERNEL_WIDTH = 2 * radius + 1;
    QVector<float> kernel(KERNEL_WIDTH);
    kernel.fill(1.0f / KERNEL_WIDTH);
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
    convolutionXKernel<<<blockGrid, threadBlock>>>(dfResult, dfKernel, radius, volumeDataDims, false);
    CUT_CHECK_ERROR( "convolutionXKernel() execution failed\n" );
    CUDA_SAFE_CALL( cudaThreadSynchronize() );

    // Copiar el resultat a l'array
    copyParams.srcPtr = make_cudaPitchedPtr(reinterpret_cast<void*>(dfResult), dimensions[0] * sizeof(float), dimensions[0], dimensions[1]);    // data, pitch, width, height
    copyParams.kind = cudaMemcpyDeviceToDevice;
    CUDA_SAFE_CALL( cudaMemcpy3D(&copyParams) );
    CUDA_SAFE_CALL( cudaThreadSynchronize() );

    // Executar per Y
    convolutionYKernel<<<blockGrid, threadBlock>>>(dfResult, dfKernel, radius, volumeDataDims, false);
    CUT_CHECK_ERROR( "convolutionYKernel() execution failed\n" );
    CUDA_SAFE_CALL( cudaThreadSynchronize() );

    // Copiar el resultat a l'array
    CUDA_SAFE_CALL( cudaMemcpy3D(&copyParams) );
    CUDA_SAFE_CALL( cudaThreadSynchronize() );

    // Executar per Z
    convolutionZKernel<<<blockGrid, threadBlock>>>(dfResult, dfKernel, radius, volumeDataDims, false);
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

    std::cout << "box mean difference: " << elapsedTime << " ms" << std::endl;

    cudaEventDestroy(start);
    cudaEventDestroy(stop);

    return result;
}


__global__ void squareKernel(float *result, cudaExtent dims)
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
    result[i] = value * value;
}


__global__ void finalChebychevKernel(float *result, float *result2, cudaExtent dims)
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

    float mean = result[i];         // E[Z]

    if (value > mean)
    {
        float squaresMean = result2[i]; // E[Z²]

        float variance = squaresMean - mean * mean;
        float a = value - mean;  // z - E[Z]
        result[i] = variance / (variance + a * a);  // Chebychev inequality
    }
    else result[i] = 1.0f;
}


QVector<float> cfProbabilisticAmbientOcclusionGaussianChebychev(vtkImageData *image, int radius)
{
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start, 0);

    cudaStream_t stream1, stream2;
    cudaStreamCreate(&stream1);
    cudaStreamCreate(&stream2);

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
    CUDA_SAFE_CALL( cudaMemcpy3D(&copyParams) );    // còpia síncrona perquè si un dels dos és el host ha de ser memòria reservada amb cudaMallocHost
    //gVolumeTexture.normalized = false;                      // false (predeterminat) -> [0,N) | true -> [0,1)
    //gVolumeTexture.filterMode = cudaFilterModePoint;        // cudaFilterModePoint (predeterminat) o cudaFilterModeLinear
    //gVolumeTexture.addressMode[0] = cudaAddressModeClamp;   // cudaAddressModeClamp (retallar) (predeterminat) o cudaAddressModeWrap (fer la volta)
    //gVolumeTexture.addressMode[1] = cudaAddressModeClamp;
    //gVolumeTexture.addressMode[2] = cudaAddressModeClamp;
    CUDA_SAFE_CALL( cudaBindTextureToArray(gVolumeTexture, dVolumeArray, channelDescVolumeArray) );

    // Reservar espai pel resultat
    float *dfResult;
    CUDA_SAFE_CALL( cudaMalloc(reinterpret_cast<void**>(&dfResult), VOLUME_DATA_SIZE * sizeof(float)) );

    // Preparar l'execució
    //Block width should be a multiple of maximum coalesced write size
    //for coalesced memory writes in convolutionRowGPU() and convolutionColumnGPU()
    dim3 threadBlock(16, 8, 4);
    uint blocksX = iDivUp(volumeDataDims.width, threadBlock.x);
    uint blocksY = iDivUp(volumeDataDims.height, threadBlock.y);
    uint blocksZ = iDivUp(volumeDataDims.depth, threadBlock.z);
    dim3 blockGrid(blocksX * blocksY, blocksZ);

    // Calcular volum al quadrat
    squareKernel<<<blockGrid, threadBlock, 0, stream2>>>(dfResult, volumeDataDims); // generem el volum al quadrat a l'stream 2

    // Mentrestant, al host...

    // Crear un segon array pel volum al quadrat, amb la seva textura corresponent
    cudaArray *dVolume2Array;
    cudaChannelFormatDesc channelDescVolume2Array = cudaCreateChannelDesc<float>();
    CUDA_SAFE_CALL( cudaMalloc3DArray(&dVolume2Array, &channelDescVolume2Array, volumeDataDims) );
    //gVolume2Texture.normalized = false;                     // false (predeterminat) -> [0,N) | true -> [0,1)
    //gVolume2Texture.filterMode = cudaFilterModePoint;       // cudaFilterModePoint (predeterminat) o cudaFilterModeLinear
    //gVolume2Texture.addressMode[0] = cudaAddressModeClamp;  // cudaAddressModeClamp (retallar) (predeterminat) o cudaAddressModeWrap (fer la volta)
    //gVolume2Texture.addressMode[1] = cudaAddressModeClamp;
    //gVolume2Texture.addressMode[2] = cudaAddressModeClamp;
    CUDA_SAFE_CALL( cudaBindTextureToArray(gVolume2Texture, dVolume2Array, channelDescVolume2Array) );

    // En aquest temps suposem que ja ha acabat el kernel d'abans (stream 2)
    cudaStreamSynchronize(stream2);

    // Copiem el resultat a l'array del volum al quadrat
    cudaMemcpy3DParms copyParams2 = {0};
    copyParams2.srcPtr = make_cudaPitchedPtr(reinterpret_cast<void*>(dfResult), dimensions[0] * sizeof(float), dimensions[0], dimensions[1]);   // data, pitch, width, height
    copyParams2.dstArray = dVolume2Array;
    copyParams2.extent = volumeDataDims;
    copyParams2.kind = cudaMemcpyDeviceToDevice;
    CUDA_SAFE_CALL_NO_SYNC( cudaMemcpy3DAsync(&copyParams2, stream2) ); // còpia a l'stream 2

    // Reservar espai per l'altre resultat
    float *dfResult2;
    CUDA_SAFE_CALL( cudaMalloc(reinterpret_cast<void**>(&dfResult2), VOLUME_DATA_SIZE * sizeof(float)) );

    // Calcular kernel
    const int KERNEL_WIDTH = 2 * radius + 1;
    QVector<float> kernel(KERNEL_WIDTH);
    float kernelSum = 0.0f;
    float sigma = radius / 3.0f;
    for (int i = 0; i < KERNEL_WIDTH; i++)
    {
        float f = static_cast<float>(i - radius) / sigma;
        kernel[i] = expf(-f * f / 2.0f);
        kernelSum += kernel.at(i);
    }
    for (int i = 0; i < KERNEL_WIDTH; i++) kernel[i] /= kernelSum;
    std::cout << "kernel:";
    for (int i = 0; i < KERNEL_WIDTH; i++) std::cout << " " << kernel[i];
    std::cout << std::endl;
    float *dfKernel;
    CUDA_SAFE_CALL( cudaMalloc(reinterpret_cast<void**>(&dfKernel), KERNEL_WIDTH * sizeof(float)) );
    // Aquest és molt petit, o sigui que el podem fer síncron
    CUDA_SAFE_CALL( cudaMemcpy(reinterpret_cast<void*>(dfKernel), reinterpret_cast<void*>(kernel.data()), KERNEL_WIDTH * sizeof(float), cudaMemcpyHostToDevice) );

    // Aquí ja hauria d'haver acabat la còpia d'abans (stream 2)
    cudaStreamSynchronize(stream2);

    // A partir d'aquí comença la diversió:
    // Executarem els dos filtratges en paral·lel, en streams diferents, perquè mentre un filtra l'altre copiï memòria i viceversa.
    // Així podem aconseguir una execució més ràpida (en teoria).
    // Som-hi doncs...

    // Executar per X1
    convolutionXKernel<<<blockGrid, threadBlock, 0, stream1>>>(dfResult, dfKernel, radius, volumeDataDims, false);
    // Executar per X2
    convolutionXKernel<<<blockGrid, threadBlock, 0, stream2>>>(dfResult2, dfKernel, radius, volumeDataDims, true);

    // Copiar el resultat a l'array (1)
    copyParams.srcPtr = make_cudaPitchedPtr(reinterpret_cast<void*>(dfResult), dimensions[0] * sizeof(float), dimensions[0], dimensions[1]);    // data, pitch, width, height
    copyParams.kind = cudaMemcpyDeviceToDevice;
    cudaStreamSynchronize(stream1);
    CUDA_SAFE_CALL_NO_SYNC( cudaMemcpy3DAsync(&copyParams, stream1) );
    // Copiar el resultat a l'array (2)
    copyParams2.srcPtr = make_cudaPitchedPtr(reinterpret_cast<void*>(dfResult2), dimensions[0] * sizeof(float), dimensions[0], dimensions[1]);  // data, pitch, width, height
    cudaStreamSynchronize(stream2);
    CUDA_SAFE_CALL_NO_SYNC( cudaMemcpy3DAsync(&copyParams2, stream2) );

    // Executar per Y1
    cudaStreamSynchronize(stream1);
    convolutionYKernel<<<blockGrid, threadBlock, 0, stream1>>>(dfResult, dfKernel, radius, volumeDataDims, false);
    // Executar per Y2
    cudaStreamSynchronize(stream2);
    convolutionYKernel<<<blockGrid, threadBlock, 0, stream2>>>(dfResult2, dfKernel, radius, volumeDataDims, true);

    // Copiar el resultat a l'array (1)
    cudaStreamSynchronize(stream1);
    CUDA_SAFE_CALL_NO_SYNC( cudaMemcpy3DAsync(&copyParams, stream1) );
    // Copiar el resultat a l'array (2)
    cudaStreamSynchronize(stream2);
    CUDA_SAFE_CALL_NO_SYNC( cudaMemcpy3DAsync(&copyParams2, stream2) );

    // Executar per Z1
    cudaStreamSynchronize(stream1);
    convolutionZKernel<<<blockGrid, threadBlock, 0, stream1>>>(dfResult, dfKernel, radius, volumeDataDims, false);
    // Executar per Z2
    cudaStreamSynchronize(stream2);
    convolutionZKernel<<<blockGrid, threadBlock, 0, stream2>>>(dfResult2, dfKernel, radius, volumeDataDims, true);

    // Copiar el volum original un altre cop a l'array
    copyParams.srcPtr = make_cudaPitchedPtr(reinterpret_cast<void*>(data), dimensions[0] * sizeof(float), dimensions[0], dimensions[1]);    // data, pitch, width, height
    copyParams.kind = cudaMemcpyHostToDevice;
    cudaStreamSynchronize(stream1);
    CUDA_SAFE_CALL( cudaMemcpy3D(&copyParams) );    // aquesta la fem síncrona
    CUDA_SAFE_CALL( cudaThreadSynchronize() );

    // Ara ja podem fer la passada final
    finalChebychevKernel<<<blockGrid, threadBlock>>>(dfResult, dfResult2, volumeDataDims);
    CUDA_SAFE_CALL( cudaThreadSynchronize() );

    // Copiar el resultat final al host
    QVector<float> result(VOLUME_DATA_SIZE);
    CUDA_SAFE_CALL( cudaMemcpy(reinterpret_cast<void*>(result.data()), reinterpret_cast<void*>(dfResult), VOLUME_DATA_SIZE * sizeof(float), cudaMemcpyDeviceToHost) );

    // Neteja
    CUDA_SAFE_CALL( cudaFree(dfKernel) );
    CUDA_SAFE_CALL( cudaFree(dfResult) );
    CUDA_SAFE_CALL( cudaFree(dfResult2) );
    CUDA_SAFE_CALL( cudaUnbindTexture(gVolumeTexture) );
    CUDA_SAFE_CALL( cudaUnbindTexture(gVolume2Texture) );
    CUDA_SAFE_CALL( cudaFreeArray(dVolumeArray) );
    CUDA_SAFE_CALL( cudaFreeArray(dVolume2Array) );

    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    float elapsedTime = 0.0f;
    cudaEventElapsedTime(&elapsedTime, start, stop);

    std::cout << "pao gaussian chebychev: " << elapsedTime << " ms" << std::endl;

    cudaStreamDestroy(stream1);
    cudaStreamDestroy(stream2);

    cudaEventDestroy(start);
    cudaEventDestroy(stop);

    return result;
}


QVector<float> cfProbabilisticAmbientOcclusionBoxMeanChebychev(vtkImageData *image, int radius)
{
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start, 0);

    cudaStream_t stream1, stream2;
    cudaStreamCreate(&stream1);
    cudaStreamCreate(&stream2);

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
    CUDA_SAFE_CALL( cudaMemcpy3D(&copyParams) );    // còpia síncrona perquè si un dels dos és el host ha de ser memòria reservada amb cudaMallocHost
    //gVolumeTexture.normalized = false;                      // false (predeterminat) -> [0,N) | true -> [0,1)
    //gVolumeTexture.filterMode = cudaFilterModePoint;        // cudaFilterModePoint (predeterminat) o cudaFilterModeLinear
    //gVolumeTexture.addressMode[0] = cudaAddressModeClamp;   // cudaAddressModeClamp (retallar) (predeterminat) o cudaAddressModeWrap (fer la volta)
    //gVolumeTexture.addressMode[1] = cudaAddressModeClamp;
    //gVolumeTexture.addressMode[2] = cudaAddressModeClamp;
    CUDA_SAFE_CALL( cudaBindTextureToArray(gVolumeTexture, dVolumeArray, channelDescVolumeArray) );

    // Reservar espai pel resultat
    float *dfResult;
    CUDA_SAFE_CALL( cudaMalloc(reinterpret_cast<void**>(&dfResult), VOLUME_DATA_SIZE * sizeof(float)) );

    // Preparar l'execució
    //Block width should be a multiple of maximum coalesced write size
    //for coalesced memory writes in convolutionRowGPU() and convolutionColumnGPU()
    dim3 threadBlock(16, 8, 4);
    uint blocksX = iDivUp(volumeDataDims.width, threadBlock.x);
    uint blocksY = iDivUp(volumeDataDims.height, threadBlock.y);
    uint blocksZ = iDivUp(volumeDataDims.depth, threadBlock.z);
    dim3 blockGrid(blocksX * blocksY, blocksZ);

    // Calcular volum al quadrat
    squareKernel<<<blockGrid, threadBlock, 0, stream2>>>(dfResult, volumeDataDims); // generem el volum al quadrat a l'stream 2

    // Mentrestant, al host...

    // Crear un segon array pel volum al quadrat, amb la seva textura corresponent
    cudaArray *dVolume2Array;
    cudaChannelFormatDesc channelDescVolume2Array = cudaCreateChannelDesc<float>();
    CUDA_SAFE_CALL( cudaMalloc3DArray(&dVolume2Array, &channelDescVolume2Array, volumeDataDims) );
    //gVolume2Texture.normalized = false;                     // false (predeterminat) -> [0,N) | true -> [0,1)
    //gVolume2Texture.filterMode = cudaFilterModePoint;       // cudaFilterModePoint (predeterminat) o cudaFilterModeLinear
    //gVolume2Texture.addressMode[0] = cudaAddressModeClamp;  // cudaAddressModeClamp (retallar) (predeterminat) o cudaAddressModeWrap (fer la volta)
    //gVolume2Texture.addressMode[1] = cudaAddressModeClamp;
    //gVolume2Texture.addressMode[2] = cudaAddressModeClamp;
    CUDA_SAFE_CALL( cudaBindTextureToArray(gVolume2Texture, dVolume2Array, channelDescVolume2Array) );

    // En aquest temps suposem que ja ha acabat el kernel d'abans (stream 2)
    cudaStreamSynchronize(stream2);

    // Copiem el resultat a l'array del volum al quadrat
    cudaMemcpy3DParms copyParams2 = {0};
    copyParams2.srcPtr = make_cudaPitchedPtr(reinterpret_cast<void*>(dfResult), dimensions[0] * sizeof(float), dimensions[0], dimensions[1]);   // data, pitch, width, height
    copyParams2.dstArray = dVolume2Array;
    copyParams2.extent = volumeDataDims;
    copyParams2.kind = cudaMemcpyDeviceToDevice;
    CUDA_SAFE_CALL_NO_SYNC( cudaMemcpy3DAsync(&copyParams2, stream2) ); // còpia a l'stream 2

    // Reservar espai per l'altre resultat
    float *dfResult2;
    CUDA_SAFE_CALL( cudaMalloc(reinterpret_cast<void**>(&dfResult2), VOLUME_DATA_SIZE * sizeof(float)) );

    // Calcular kernel
    const int KERNEL_WIDTH = 2 * radius + 1;
    QVector<float> kernel(KERNEL_WIDTH);
    kernel.fill(1.0f / KERNEL_WIDTH);
    std::cout << "kernel:";
    for (int i = 0; i < KERNEL_WIDTH; i++) std::cout << " " << kernel[i];
    std::cout << std::endl;
    float *dfKernel;
    CUDA_SAFE_CALL( cudaMalloc(reinterpret_cast<void**>(&dfKernel), KERNEL_WIDTH * sizeof(float)) );
    // Aquest és molt petit, o sigui que el podem fer síncron
    CUDA_SAFE_CALL( cudaMemcpy(reinterpret_cast<void*>(dfKernel), reinterpret_cast<void*>(kernel.data()), KERNEL_WIDTH * sizeof(float), cudaMemcpyHostToDevice) );

    // Aquí ja hauria d'haver acabat la còpia d'abans (stream 2)
    cudaStreamSynchronize(stream2);

    // A partir d'aquí comença la diversió:
    // Executarem els dos filtratges en paral·lel, en streams diferents, perquè mentre un filtra l'altre copiï memòria i viceversa.
    // Així podem aconseguir una execució més ràpida (en teoria).
    // Som-hi doncs...

    // Executar per X1
    convolutionXKernel<<<blockGrid, threadBlock, 0, stream1>>>(dfResult, dfKernel, radius, volumeDataDims, false);
    // Executar per X2
    convolutionXKernel<<<blockGrid, threadBlock, 0, stream2>>>(dfResult2, dfKernel, radius, volumeDataDims, true);

    // Copiar el resultat a l'array (1)
    copyParams.srcPtr = make_cudaPitchedPtr(reinterpret_cast<void*>(dfResult), dimensions[0] * sizeof(float), dimensions[0], dimensions[1]);    // data, pitch, width, height
    copyParams.kind = cudaMemcpyDeviceToDevice;
    cudaStreamSynchronize(stream1);
    CUDA_SAFE_CALL_NO_SYNC( cudaMemcpy3DAsync(&copyParams, stream1) );
    // Copiar el resultat a l'array (2)
    copyParams2.srcPtr = make_cudaPitchedPtr(reinterpret_cast<void*>(dfResult2), dimensions[0] * sizeof(float), dimensions[0], dimensions[1]);  // data, pitch, width, height
    cudaStreamSynchronize(stream2);
    CUDA_SAFE_CALL_NO_SYNC( cudaMemcpy3DAsync(&copyParams2, stream2) );

    // Executar per Y1
    cudaStreamSynchronize(stream1);
    convolutionYKernel<<<blockGrid, threadBlock, 0, stream1>>>(dfResult, dfKernel, radius, volumeDataDims, false);
    // Executar per Y2
    cudaStreamSynchronize(stream2);
    convolutionYKernel<<<blockGrid, threadBlock, 0, stream2>>>(dfResult2, dfKernel, radius, volumeDataDims, true);

    // Copiar el resultat a l'array (1)
    cudaStreamSynchronize(stream1);
    CUDA_SAFE_CALL_NO_SYNC( cudaMemcpy3DAsync(&copyParams, stream1) );
    // Copiar el resultat a l'array (2)
    cudaStreamSynchronize(stream2);
    CUDA_SAFE_CALL_NO_SYNC( cudaMemcpy3DAsync(&copyParams2, stream2) );

    // Executar per Z1
    cudaStreamSynchronize(stream1);
    convolutionZKernel<<<blockGrid, threadBlock, 0, stream1>>>(dfResult, dfKernel, radius, volumeDataDims, false);
    // Executar per Z2
    cudaStreamSynchronize(stream2);
    convolutionZKernel<<<blockGrid, threadBlock, 0, stream2>>>(dfResult2, dfKernel, radius, volumeDataDims, true);

    // Copiar el volum original un altre cop a l'array
    copyParams.srcPtr = make_cudaPitchedPtr(reinterpret_cast<void*>(data), dimensions[0] * sizeof(float), dimensions[0], dimensions[1]);    // data, pitch, width, height
    copyParams.kind = cudaMemcpyHostToDevice;
    cudaStreamSynchronize(stream1);
    CUDA_SAFE_CALL( cudaMemcpy3D(&copyParams) );    // aquesta la fem síncrona
    CUDA_SAFE_CALL( cudaThreadSynchronize() );

    // Ara ja podem fer la passada final
    finalChebychevKernel<<<blockGrid, threadBlock>>>(dfResult, dfResult2, volumeDataDims);
    CUDA_SAFE_CALL( cudaThreadSynchronize() );

    // Copiar el resultat final al host
    QVector<float> result(VOLUME_DATA_SIZE);
    CUDA_SAFE_CALL( cudaMemcpy(reinterpret_cast<void*>(result.data()), reinterpret_cast<void*>(dfResult), VOLUME_DATA_SIZE * sizeof(float), cudaMemcpyDeviceToHost) );

    // Neteja
    CUDA_SAFE_CALL( cudaFree(dfKernel) );
    CUDA_SAFE_CALL( cudaFree(dfResult) );
    CUDA_SAFE_CALL( cudaFree(dfResult2) );
    CUDA_SAFE_CALL( cudaUnbindTexture(gVolumeTexture) );
    CUDA_SAFE_CALL( cudaUnbindTexture(gVolume2Texture) );
    CUDA_SAFE_CALL( cudaFreeArray(dVolumeArray) );
    CUDA_SAFE_CALL( cudaFreeArray(dVolume2Array) );

    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    float elapsedTime = 0.0f;
    cudaEventElapsedTime(&elapsedTime, start, stop);

    std::cout << "pao box mean chebychev: " << elapsedTime << " ms" << std::endl;

    cudaStreamDestroy(stream1);
    cudaStreamDestroy(stream2);

    cudaEventDestroy(start);
    cudaEventDestroy(stop);

    return result;
}


__global__ void finalGaussianKernel(float *result, float *result2, cudaExtent dims)
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

    float mean = result[i];         // E[Z]
    float squaresMean = result2[i]; // E[Z²]
    float variance = squaresMean - mean * mean;
    // Fórmules tretes de:
    // http://en.wikipedia.org/wiki/Gaussian_distribution#Cumulative_distribution_function
    // http://en.wikipedia.org/wiki/Q-function
    // P(Z >= z) = 1/2 * erfc((z - mean) / sqrt(2 * variance))
    result[i] = 0.5f * erfcf((value - mean) / sqrtf(2.0f * variance));
}


QVector<float> cfProbabilisticAmbientOcclusionGaussian(vtkImageData *image, int radius)
{
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start, 0);

    cudaStream_t stream1, stream2;
    cudaStreamCreate(&stream1);
    cudaStreamCreate(&stream2);

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
    CUDA_SAFE_CALL( cudaMemcpy3D(&copyParams) );    // còpia síncrona perquè si un dels dos és el host ha de ser memòria reservada amb cudaMallocHost
    //gVolumeTexture.normalized = false;                      // false (predeterminat) -> [0,N) | true -> [0,1)
    //gVolumeTexture.filterMode = cudaFilterModePoint;        // cudaFilterModePoint (predeterminat) o cudaFilterModeLinear
    //gVolumeTexture.addressMode[0] = cudaAddressModeClamp;   // cudaAddressModeClamp (retallar) (predeterminat) o cudaAddressModeWrap (fer la volta)
    //gVolumeTexture.addressMode[1] = cudaAddressModeClamp;
    //gVolumeTexture.addressMode[2] = cudaAddressModeClamp;
    CUDA_SAFE_CALL( cudaBindTextureToArray(gVolumeTexture, dVolumeArray, channelDescVolumeArray) );

    // Reservar espai pel resultat
    float *dfResult;
    CUDA_SAFE_CALL( cudaMalloc(reinterpret_cast<void**>(&dfResult), VOLUME_DATA_SIZE * sizeof(float)) );

    // Preparar l'execució
    //Block width should be a multiple of maximum coalesced write size
    //for coalesced memory writes in convolutionRowGPU() and convolutionColumnGPU()
    dim3 threadBlock(16, 8, 4);
    uint blocksX = iDivUp(volumeDataDims.width, threadBlock.x);
    uint blocksY = iDivUp(volumeDataDims.height, threadBlock.y);
    uint blocksZ = iDivUp(volumeDataDims.depth, threadBlock.z);
    dim3 blockGrid(blocksX * blocksY, blocksZ);

    // Calcular volum al quadrat
    squareKernel<<<blockGrid, threadBlock, 0, stream2>>>(dfResult, volumeDataDims); // generem el volum al quadrat a l'stream 2

    // Mentrestant, al host...

    // Crear un segon array pel volum al quadrat, amb la seva textura corresponent
    cudaArray *dVolume2Array;
    cudaChannelFormatDesc channelDescVolume2Array = cudaCreateChannelDesc<float>();
    CUDA_SAFE_CALL( cudaMalloc3DArray(&dVolume2Array, &channelDescVolume2Array, volumeDataDims) );
    //gVolume2Texture.normalized = false;                     // false (predeterminat) -> [0,N) | true -> [0,1)
    //gVolume2Texture.filterMode = cudaFilterModePoint;       // cudaFilterModePoint (predeterminat) o cudaFilterModeLinear
    //gVolume2Texture.addressMode[0] = cudaAddressModeClamp;  // cudaAddressModeClamp (retallar) (predeterminat) o cudaAddressModeWrap (fer la volta)
    //gVolume2Texture.addressMode[1] = cudaAddressModeClamp;
    //gVolume2Texture.addressMode[2] = cudaAddressModeClamp;
    CUDA_SAFE_CALL( cudaBindTextureToArray(gVolume2Texture, dVolume2Array, channelDescVolume2Array) );

    // En aquest temps suposem que ja ha acabat el kernel d'abans (stream 2)
    cudaStreamSynchronize(stream2);

    // Copiem el resultat a l'array del volum al quadrat
    cudaMemcpy3DParms copyParams2 = {0};
    copyParams2.srcPtr = make_cudaPitchedPtr(reinterpret_cast<void*>(dfResult), dimensions[0] * sizeof(float), dimensions[0], dimensions[1]);   // data, pitch, width, height
    copyParams2.dstArray = dVolume2Array;
    copyParams2.extent = volumeDataDims;
    copyParams2.kind = cudaMemcpyDeviceToDevice;
    CUDA_SAFE_CALL_NO_SYNC( cudaMemcpy3DAsync(&copyParams2, stream2) ); // còpia a l'stream 2

    // Reservar espai per l'altre resultat
    float *dfResult2;
    CUDA_SAFE_CALL( cudaMalloc(reinterpret_cast<void**>(&dfResult2), VOLUME_DATA_SIZE * sizeof(float)) );

    // Calcular kernel
    const int KERNEL_WIDTH = 2 * radius + 1;
    QVector<float> kernel(KERNEL_WIDTH);
    kernel.fill(1.0f / KERNEL_WIDTH);
    std::cout << "kernel:";
    for (int i = 0; i < KERNEL_WIDTH; i++) std::cout << " " << kernel[i];
    std::cout << std::endl;
    float *dfKernel;
    CUDA_SAFE_CALL( cudaMalloc(reinterpret_cast<void**>(&dfKernel), KERNEL_WIDTH * sizeof(float)) );
    // Aquest és molt petit, o sigui que el podem fer síncron
    CUDA_SAFE_CALL( cudaMemcpy(reinterpret_cast<void*>(dfKernel), reinterpret_cast<void*>(kernel.data()), KERNEL_WIDTH * sizeof(float), cudaMemcpyHostToDevice) );

    // Aquí ja hauria d'haver acabat la còpia d'abans (stream 2)
    cudaStreamSynchronize(stream2);

    // A partir d'aquí comença la diversió:
    // Executarem els dos filtratges en paral·lel, en streams diferents, perquè mentre un filtra l'altre copiï memòria i viceversa.
    // Així podem aconseguir una execució més ràpida (en teoria).
    // Som-hi doncs...

    // Executar per X1
    convolutionXKernel<<<blockGrid, threadBlock, 0, stream1>>>(dfResult, dfKernel, radius, volumeDataDims, false);
    // Executar per X2
    convolutionXKernel<<<blockGrid, threadBlock, 0, stream2>>>(dfResult2, dfKernel, radius, volumeDataDims, true);

    // Copiar el resultat a l'array (1)
    copyParams.srcPtr = make_cudaPitchedPtr(reinterpret_cast<void*>(dfResult), dimensions[0] * sizeof(float), dimensions[0], dimensions[1]);    // data, pitch, width, height
    copyParams.kind = cudaMemcpyDeviceToDevice;
    cudaStreamSynchronize(stream1);
    CUDA_SAFE_CALL_NO_SYNC( cudaMemcpy3DAsync(&copyParams, stream1) );
    // Copiar el resultat a l'array (2)
    copyParams2.srcPtr = make_cudaPitchedPtr(reinterpret_cast<void*>(dfResult2), dimensions[0] * sizeof(float), dimensions[0], dimensions[1]);  // data, pitch, width, height
    cudaStreamSynchronize(stream2);
    CUDA_SAFE_CALL_NO_SYNC( cudaMemcpy3DAsync(&copyParams2, stream2) );

    // Executar per Y1
    cudaStreamSynchronize(stream1);
    convolutionYKernel<<<blockGrid, threadBlock, 0, stream1>>>(dfResult, dfKernel, radius, volumeDataDims, false);
    // Executar per Y2
    cudaStreamSynchronize(stream2);
    convolutionYKernel<<<blockGrid, threadBlock, 0, stream2>>>(dfResult2, dfKernel, radius, volumeDataDims, true);

    // Copiar el resultat a l'array (1)
    cudaStreamSynchronize(stream1);
    CUDA_SAFE_CALL_NO_SYNC( cudaMemcpy3DAsync(&copyParams, stream1) );
    // Copiar el resultat a l'array (2)
    cudaStreamSynchronize(stream2);
    CUDA_SAFE_CALL_NO_SYNC( cudaMemcpy3DAsync(&copyParams2, stream2) );

    // Executar per Z1
    cudaStreamSynchronize(stream1);
    convolutionZKernel<<<blockGrid, threadBlock, 0, stream1>>>(dfResult, dfKernel, radius, volumeDataDims, false);
    // Executar per Z2
    cudaStreamSynchronize(stream2);
    convolutionZKernel<<<blockGrid, threadBlock, 0, stream2>>>(dfResult2, dfKernel, radius, volumeDataDims, true);

    // Copiar el volum original un altre cop a l'array
    copyParams.srcPtr = make_cudaPitchedPtr(reinterpret_cast<void*>(data), dimensions[0] * sizeof(float), dimensions[0], dimensions[1]);    // data, pitch, width, height
    copyParams.kind = cudaMemcpyHostToDevice;
    cudaStreamSynchronize(stream1);
    CUDA_SAFE_CALL( cudaMemcpy3D(&copyParams) );    // aquesta la fem síncrona
    CUDA_SAFE_CALL( cudaThreadSynchronize() );

    // Ara ja podem fer la passada final
    finalGaussianKernel<<<blockGrid, threadBlock>>>(dfResult, dfResult2, volumeDataDims);
    CUDA_SAFE_CALL( cudaThreadSynchronize() );

    // Copiar el resultat final al host
    QVector<float> result(VOLUME_DATA_SIZE);
    CUDA_SAFE_CALL( cudaMemcpy(reinterpret_cast<void*>(result.data()), reinterpret_cast<void*>(dfResult), VOLUME_DATA_SIZE * sizeof(float), cudaMemcpyDeviceToHost) );

    // Neteja
    CUDA_SAFE_CALL( cudaFree(dfKernel) );
    CUDA_SAFE_CALL( cudaFree(dfResult) );
    CUDA_SAFE_CALL( cudaFree(dfResult2) );
    CUDA_SAFE_CALL( cudaUnbindTexture(gVolumeTexture) );
    CUDA_SAFE_CALL( cudaUnbindTexture(gVolume2Texture) );
    CUDA_SAFE_CALL( cudaFreeArray(dVolumeArray) );
    CUDA_SAFE_CALL( cudaFreeArray(dVolume2Array) );

    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    float elapsedTime = 0.0f;
    cudaEventElapsedTime(&elapsedTime, start, stop);

    std::cout << "pao gaussian: " << elapsedTime << " ms" << std::endl;

    cudaStreamDestroy(stream1);
    cudaStreamDestroy(stream2);

    cudaEventDestroy(start);
    cudaEventDestroy(stop);

    return result;
}


__global__ void cubeFrequencyKernel(float *result, int radius, cudaExtent dims)
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
    int greater = 0;    // compta quants n'hi ha de més grans o iguals que value al seu entorn

    for (int dx = -radius; dx <= radius; dx++)
    {
        float fx2 = fx + dx;

        for (int dy = -radius; dy <= radius; dy++)
        {
            float fy2 = fy + dy;

            for (int dz = -radius; dz <= radius; dz++)
            {
                float fz2 = fz + dz;
                float value2 = tex3D(gVolumeTexture, fx2, fy2, fz2);
                if (value2 >= value) greater++;
            }
        }
    }

    uint i = x + y * dims.width + z * dims.width * dims.height;
    int length = 2 * radius + 1;
    float count = length * length * length;
    result[i] = greater / count;
}


QVector<float> cfProbabilisticAmbientOcclusionCube(vtkImageData *image, int radius)
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
    CUDA_SAFE_CALL( cudaMemcpy3D(&copyParams) );    // còpia síncrona perquè si un dels dos és el host ha de ser memòria reservada amb cudaMallocHost
    //gVolumeTexture.normalized = false;                      // false (predeterminat) -> [0,N) | true -> [0,1)
    //gVolumeTexture.filterMode = cudaFilterModePoint;        // cudaFilterModePoint (predeterminat) o cudaFilterModeLinear
    //gVolumeTexture.addressMode[0] = cudaAddressModeClamp;   // cudaAddressModeClamp (retallar) (predeterminat) o cudaAddressModeWrap (fer la volta)
    //gVolumeTexture.addressMode[1] = cudaAddressModeClamp;
    //gVolumeTexture.addressMode[2] = cudaAddressModeClamp;
    CUDA_SAFE_CALL( cudaBindTextureToArray(gVolumeTexture, dVolumeArray, channelDescVolumeArray) );

    // Reservar espai pel resultat
    float *dfResult;
    CUDA_SAFE_CALL( cudaMalloc(reinterpret_cast<void**>(&dfResult), VOLUME_DATA_SIZE * sizeof(float)) );

    // Preparar l'execució
    //Block width should be a multiple of maximum coalesced write size
    //for coalesced memory writes in convolutionRowGPU() and convolutionColumnGPU()
    dim3 threadBlock(16, 8, 4);
    uint blocksX = iDivUp(volumeDataDims.width, threadBlock.x);
    uint blocksY = iDivUp(volumeDataDims.height, threadBlock.y);
    uint blocksZ = iDivUp(volumeDataDims.depth, threadBlock.z);
    dim3 blockGrid(blocksX * blocksY, blocksZ);

    // Llancem el kernel
    cubeFrequencyKernel<<<blockGrid, threadBlock>>>(dfResult, radius, volumeDataDims);
    CUDA_SAFE_CALL( cudaThreadSynchronize() );

    // Copiar el resultat final al host
    QVector<float> result(VOLUME_DATA_SIZE);
    CUDA_SAFE_CALL( cudaMemcpy(reinterpret_cast<void*>(result.data()), reinterpret_cast<void*>(dfResult), VOLUME_DATA_SIZE * sizeof(float), cudaMemcpyDeviceToHost) );

    // Neteja
    CUDA_SAFE_CALL( cudaFree(dfResult) );;
    CUDA_SAFE_CALL( cudaUnbindTexture(gVolumeTexture) );
    CUDA_SAFE_CALL( cudaFreeArray(dVolumeArray) );

    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    float elapsedTime = 0.0f;
    cudaEventElapsedTime(&elapsedTime, start, stop);

    std::cout << "pao cube: " << elapsedTime << " ms" << std::endl;

    cudaEventDestroy(start);
    cudaEventDestroy(stop);

    return result;
}


__global__ void sphereFrequencyKernel(float *result, int radius, cudaExtent dims)
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
    int greater = 0;    // compta quants n'hi ha de més grans o iguals que value al seu entorn
    int count = 0;

    for (int dx = -radius; dx <= radius; dx++)
    {
        float fx2 = fx + dx;

        for (int dy = -radius; dy <= radius; dy++)
        {
            if (sqrtf(dx * dx + dy * dy) > radius) continue;

            float fy2 = fy + dy;

            for (int dz = -radius; dz <= radius; dz++)
            {
                if (sqrtf(dx * dx + dy * dy + dz * dz) > radius) continue;

                float fz2 = fz + dz;
                float value2 = tex3D(gVolumeTexture, fx2, fy2, fz2);
                if (value2 >= value) greater++;
                count++;
            }
        }
    }

    uint i = x + y * dims.width + z * dims.width * dims.height;
    result[i] = ((float) greater) / count;
}


QVector<float> cfProbabilisticAmbientOcclusionSphere(vtkImageData *image, int radius)
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
    CUDA_SAFE_CALL( cudaMemcpy3D(&copyParams) );    // còpia síncrona perquè si un dels dos és el host ha de ser memòria reservada amb cudaMallocHost
    //gVolumeTexture.normalized = false;                      // false (predeterminat) -> [0,N) | true -> [0,1)
    //gVolumeTexture.filterMode = cudaFilterModePoint;        // cudaFilterModePoint (predeterminat) o cudaFilterModeLinear
    //gVolumeTexture.addressMode[0] = cudaAddressModeClamp;   // cudaAddressModeClamp (retallar) (predeterminat) o cudaAddressModeWrap (fer la volta)
    //gVolumeTexture.addressMode[1] = cudaAddressModeClamp;
    //gVolumeTexture.addressMode[2] = cudaAddressModeClamp;
    CUDA_SAFE_CALL( cudaBindTextureToArray(gVolumeTexture, dVolumeArray, channelDescVolumeArray) );

    // Reservar espai pel resultat
    float *dfResult;
    CUDA_SAFE_CALL( cudaMalloc(reinterpret_cast<void**>(&dfResult), VOLUME_DATA_SIZE * sizeof(float)) );

    // Preparar l'execució
    //Block width should be a multiple of maximum coalesced write size
    //for coalesced memory writes in convolutionRowGPU() and convolutionColumnGPU()
    dim3 threadBlock(16, 8, 4);
    uint blocksX = iDivUp(volumeDataDims.width, threadBlock.x);
    uint blocksY = iDivUp(volumeDataDims.height, threadBlock.y);
    uint blocksZ = iDivUp(volumeDataDims.depth, threadBlock.z);
    dim3 blockGrid(blocksX * blocksY, blocksZ);

    // Llancem el kernel
    sphereFrequencyKernel<<<blockGrid, threadBlock>>>(dfResult, radius, volumeDataDims);
    CUDA_SAFE_CALL( cudaThreadSynchronize() );

    // Copiar el resultat final al host
    QVector<float> result(VOLUME_DATA_SIZE);
    CUDA_SAFE_CALL( cudaMemcpy(reinterpret_cast<void*>(result.data()), reinterpret_cast<void*>(dfResult), VOLUME_DATA_SIZE * sizeof(float), cudaMemcpyDeviceToHost) );

    // Neteja
    CUDA_SAFE_CALL( cudaFree(dfResult) );;
    CUDA_SAFE_CALL( cudaUnbindTexture(gVolumeTexture) );
    CUDA_SAFE_CALL( cudaFreeArray(dVolumeArray) );

    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    float elapsedTime = 0.0f;
    cudaEventElapsedTime(&elapsedTime, start, stop);

    std::cout << "pao sphere: " << elapsedTime << " ms" << std::endl;

    cudaEventDestroy(start);
    cudaEventDestroy(stop);

    return result;
}


__global__ void finalTangentSphereVarianceKernel(float *result, int radius, cudaExtent dims)
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
    float value = tex3D(gVolume3Texture, fx, fy, fz);
    float3 normal = normalize(make_float3(tex3D(gVolume3Texture, fx + 1.0f, fy, fz) - tex3D(gVolume3Texture, fx - 1.0f, fy, fz),
                                          tex3D(gVolume3Texture, fx, fy + 1.0f, fz) - tex3D(gVolume3Texture, fx, fy - 1.0f, fz),
                                          tex3D(gVolume3Texture, fx, fy, fz + 1.0f) - tex3D(gVolume3Texture, fx, fy, fz - 1.0f)));

    float3 c = normal * radius / 2.0f;
    float cx = fx + c.x, cy = fy + c.y, cz = fz + c.z;

    float mean = tex3D(gVolumeTexture, cx, cy, cz); // E[Z]

    uint i = x + y * dims.width + z * dims.width * dims.height;

    if (value > mean)
    {
        float squaresMean = tex3D(gVolume2Texture, cx, cy, cz); // E[Z²]
        float variance = squaresMean - mean * mean;
        float a = value - mean;  // z - E[Z]
        result[i] = variance / (variance + a * a);
    }
    else result[i] = 0.0f;
}


// implementació barroera
QVector<float> cfProbabilisticAmbientOcclusionTangentSphereVariance(vtkImageData *image, int radius)
{
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start, 0);

    cudaStream_t stream1, stream2;
    cudaStreamCreate(&stream1);
    cudaStreamCreate(&stream2);

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
    CUDA_SAFE_CALL( cudaMemcpy3D(&copyParams) );    // còpia síncrona perquè si un dels dos és el host ha de ser memòria reservada amb cudaMallocHost
    //gVolumeTexture.normalized = false;                      // false (predeterminat) -> [0,N) | true -> [0,1)
    //gVolumeTexture.filterMode = cudaFilterModePoint;        // cudaFilterModePoint (predeterminat) o cudaFilterModeLinear
    //gVolumeTexture.addressMode[0] = cudaAddressModeClamp;   // cudaAddressModeClamp (retallar) (predeterminat) o cudaAddressModeWrap (fer la volta)
    //gVolumeTexture.addressMode[1] = cudaAddressModeClamp;
    //gVolumeTexture.addressMode[2] = cudaAddressModeClamp;
    CUDA_SAFE_CALL( cudaBindTextureToArray(gVolumeTexture, dVolumeArray, channelDescVolumeArray) );

    // Reservar espai pel resultat
    float *dfResult;
    CUDA_SAFE_CALL( cudaMalloc(reinterpret_cast<void**>(&dfResult), VOLUME_DATA_SIZE * sizeof(float)) );

    // Preparar l'execució
    //Block width should be a multiple of maximum coalesced write size
    //for coalesced memory writes in convolutionRowGPU() and convolutionColumnGPU()
    dim3 threadBlock(16, 8, 4);
    uint blocksX = iDivUp(volumeDataDims.width, threadBlock.x);
    uint blocksY = iDivUp(volumeDataDims.height, threadBlock.y);
    uint blocksZ = iDivUp(volumeDataDims.depth, threadBlock.z);
    dim3 blockGrid(blocksX * blocksY, blocksZ);

    // Calcular volum al quadrat
    squareKernel<<<blockGrid, threadBlock, 0, stream2>>>(dfResult, volumeDataDims); // generem el volum al quadrat a l'stream 2

    // Mentrestant, al host...

    // Crear un segon array pel volum al quadrat, amb la seva textura corresponent
    cudaArray *dVolume2Array;
    cudaChannelFormatDesc channelDescVolume2Array = cudaCreateChannelDesc<float>();
    CUDA_SAFE_CALL( cudaMalloc3DArray(&dVolume2Array, &channelDescVolume2Array, volumeDataDims) );
    //gVolume2Texture.normalized = false;                     // false (predeterminat) -> [0,N) | true -> [0,1)
    //gVolume2Texture.filterMode = cudaFilterModePoint;       // cudaFilterModePoint (predeterminat) o cudaFilterModeLinear
    //gVolume2Texture.addressMode[0] = cudaAddressModeClamp;  // cudaAddressModeClamp (retallar) (predeterminat) o cudaAddressModeWrap (fer la volta)
    //gVolume2Texture.addressMode[1] = cudaAddressModeClamp;
    //gVolume2Texture.addressMode[2] = cudaAddressModeClamp;
    CUDA_SAFE_CALL( cudaBindTextureToArray(gVolume2Texture, dVolume2Array, channelDescVolume2Array) );

    // En aquest temps suposem que ja ha acabat el kernel d'abans (stream 2)
    cudaStreamSynchronize(stream2);

    // Copiem el resultat a l'array del volum al quadrat
    cudaMemcpy3DParms copyParams2 = {0};
    copyParams2.srcPtr = make_cudaPitchedPtr(reinterpret_cast<void*>(dfResult), dimensions[0] * sizeof(float), dimensions[0], dimensions[1]);   // data, pitch, width, height
    copyParams2.dstArray = dVolume2Array;
    copyParams2.extent = volumeDataDims;
    copyParams2.kind = cudaMemcpyDeviceToDevice;
    CUDA_SAFE_CALL_NO_SYNC( cudaMemcpy3DAsync(&copyParams2, stream2) ); // còpia a l'stream 2

    // Reservar espai per l'altre resultat
    float *dfResult2;
    CUDA_SAFE_CALL( cudaMalloc(reinterpret_cast<void**>(&dfResult2), VOLUME_DATA_SIZE * sizeof(float)) );

    // Calcular kernel
    const int KERNEL_WIDTH = 2 * radius + 1;
    QVector<float> kernel(KERNEL_WIDTH);
    kernel.fill(1.0f / KERNEL_WIDTH);
    std::cout << "kernel:";
    for (int i = 0; i < KERNEL_WIDTH; i++) std::cout << " " << kernel[i];
    std::cout << std::endl;
    float *dfKernel;
    CUDA_SAFE_CALL( cudaMalloc(reinterpret_cast<void**>(&dfKernel), KERNEL_WIDTH * sizeof(float)) );
    // Aquest és molt petit, o sigui que el podem fer síncron
    CUDA_SAFE_CALL( cudaMemcpy(reinterpret_cast<void*>(dfKernel), reinterpret_cast<void*>(kernel.data()), KERNEL_WIDTH * sizeof(float), cudaMemcpyHostToDevice) );

    // Aquí ja hauria d'haver acabat la còpia d'abans (stream 2)
    cudaStreamSynchronize(stream2);

    // A partir d'aquí comença la diversió:
    // Executarem els dos filtratges en paral·lel, en streams diferents, perquè mentre un filtra l'altre copiï memòria i viceversa.
    // Així podem aconseguir una execució més ràpida (en teoria).
    // Som-hi doncs...

    // Executar per X1
    convolutionXKernel<<<blockGrid, threadBlock, 0, stream1>>>(dfResult, dfKernel, radius, volumeDataDims, false);
    // Executar per X2
    convolutionXKernel<<<blockGrid, threadBlock, 0, stream2>>>(dfResult2, dfKernel, radius, volumeDataDims, true);

    // Copiar el resultat a l'array (1)
    copyParams.srcPtr = make_cudaPitchedPtr(reinterpret_cast<void*>(dfResult), dimensions[0] * sizeof(float), dimensions[0], dimensions[1]);    // data, pitch, width, height
    copyParams.kind = cudaMemcpyDeviceToDevice;
    cudaStreamSynchronize(stream1);
    CUDA_SAFE_CALL_NO_SYNC( cudaMemcpy3DAsync(&copyParams, stream1) );
    // Copiar el resultat a l'array (2)
    copyParams2.srcPtr = make_cudaPitchedPtr(reinterpret_cast<void*>(dfResult2), dimensions[0] * sizeof(float), dimensions[0], dimensions[1]);  // data, pitch, width, height
    cudaStreamSynchronize(stream2);
    CUDA_SAFE_CALL_NO_SYNC( cudaMemcpy3DAsync(&copyParams2, stream2) );

    // Executar per Y1
    cudaStreamSynchronize(stream1);
    convolutionYKernel<<<blockGrid, threadBlock, 0, stream1>>>(dfResult, dfKernel, radius, volumeDataDims, false);
    // Executar per Y2
    cudaStreamSynchronize(stream2);
    convolutionYKernel<<<blockGrid, threadBlock, 0, stream2>>>(dfResult2, dfKernel, radius, volumeDataDims, true);

    // Copiar el resultat a l'array (1)
    cudaStreamSynchronize(stream1);
    CUDA_SAFE_CALL_NO_SYNC( cudaMemcpy3DAsync(&copyParams, stream1) );
    // Copiar el resultat a l'array (2)
    cudaStreamSynchronize(stream2);
    CUDA_SAFE_CALL_NO_SYNC( cudaMemcpy3DAsync(&copyParams2, stream2) );

    // Executar per Z1
    cudaStreamSynchronize(stream1);
    convolutionZKernel<<<blockGrid, threadBlock, 0, stream1>>>(dfResult, dfKernel, radius, volumeDataDims, false);
    // Executar per Z2
    cudaStreamSynchronize(stream2);
    convolutionZKernel<<<blockGrid, threadBlock, 0, stream2>>>(dfResult2, dfKernel, radius, volumeDataDims, true);

    // Copiar el resultat a l'array (1)
    cudaStreamSynchronize(stream1);
    CUDA_SAFE_CALL_NO_SYNC( cudaMemcpy3DAsync(&copyParams, stream1) );
    // Copiar el resultat a l'array (2)
    cudaStreamSynchronize(stream2);
    CUDA_SAFE_CALL_NO_SYNC( cudaMemcpy3DAsync(&copyParams2, stream2) );

    // Copiar el volum a un array i associar-hi una textura
    cudaArray *dVolume3Array;
    cudaChannelFormatDesc channelDescVolume3Array = cudaCreateChannelDesc<float>();
    CUDA_SAFE_CALL( cudaMalloc3DArray(&dVolume3Array, &channelDescVolume3Array, volumeDataDims) );
    cudaMemcpy3DParms copyParams3 = {0};
    copyParams3.srcPtr = make_cudaPitchedPtr(reinterpret_cast<void*>(data), dimensions[0] * sizeof(float), dimensions[0], dimensions[1]);    // data, pitch, width, height
    copyParams3.dstArray = dVolume3Array;
    copyParams3.extent = volumeDataDims;
    copyParams3.kind = cudaMemcpyHostToDevice;
    CUDA_SAFE_CALL( cudaMemcpy3D(&copyParams3) );    // còpia síncrona perquè si un dels dos és el host ha de ser memòria reservada amb cudaMallocHost
    //gVolume3Texture.normalized = false;                      // false (predeterminat) -> [0,N) | true -> [0,1)
    //gVolume3Texture.filterMode = cudaFilterModePoint;        // cudaFilterModePoint (predeterminat) o cudaFilterModeLinear
    //gVolume3Texture.addressMode[0] = cudaAddressModeClamp;   // cudaAddressModeClamp (retallar) (predeterminat) o cudaAddressModeWrap (fer la volta)
    //gVolume3Texture.addressMode[1] = cudaAddressModeClamp;
    //gVolume3Texture.addressMode[2] = cudaAddressModeClamp;
    CUDA_SAFE_CALL( cudaBindTextureToArray(gVolume3Texture, dVolume3Array, channelDescVolumeArray) );
    CUDA_SAFE_CALL( cudaThreadSynchronize() );

    // Ara ja podem fer la passada final
    finalTangentSphereVarianceKernel<<<blockGrid, threadBlock>>>(dfResult, radius, volumeDataDims);
    CUDA_SAFE_CALL( cudaThreadSynchronize() );

    // Copiar el resultat final al host
    QVector<float> result(VOLUME_DATA_SIZE);
    CUDA_SAFE_CALL( cudaMemcpy(reinterpret_cast<void*>(result.data()), reinterpret_cast<void*>(dfResult), VOLUME_DATA_SIZE * sizeof(float), cudaMemcpyDeviceToHost) );

    // Neteja
    CUDA_SAFE_CALL( cudaFree(dfKernel) );
    CUDA_SAFE_CALL( cudaFree(dfResult) );
    CUDA_SAFE_CALL( cudaFree(dfResult2) );
    CUDA_SAFE_CALL( cudaUnbindTexture(gVolumeTexture) );
    CUDA_SAFE_CALL( cudaUnbindTexture(gVolume2Texture) );
    CUDA_SAFE_CALL( cudaUnbindTexture(gVolume3Texture) );
    CUDA_SAFE_CALL( cudaFreeArray(dVolumeArray) );
    CUDA_SAFE_CALL( cudaFreeArray(dVolume2Array) );
    CUDA_SAFE_CALL( cudaFreeArray(dVolume3Array) );

    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    float elapsedTime = 0.0f;
    cudaEventElapsedTime(&elapsedTime, start, stop);

    std::cout << "paots variance: " << elapsedTime << " ms" << std::endl;

    cudaStreamDestroy(stream1);
    cudaStreamDestroy(stream2);

    cudaEventDestroy(start);
    cudaEventDestroy(stop);

    return result;
}


// separem els mínims i màxims en un volum i les mitjanes en un altre perquè CUDA no accepta volums de float3
texture<float2, 3> gMinMaxVolumeTexture;    // el 3r paràmetre pot ser cudaReadModeElementType (valor directe) (predeterminat) o cudaReadModeNormalizedFloat (valor escalat entre 0 i 1)
texture<float, 3> gMeanVolumeTexture;       // el 3r paràmetre pot ser cudaReadModeElementType (valor directe) (predeterminat) o cudaReadModeNormalizedFloat (valor escalat entre 0 i 1)


// separem els mínims i màxims en un volum i les mitjanes en un altre perquè CUDA no accepta volums de float3
__global__ void minMaxMeanFilteringXKernel(float2 *minMax, float *mean, int radius, cudaExtent dims)
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

    float min = CUDART_INF_F, max = 0.0f, sum = 0.0f;

    for (int k = -radius; k <= radius; k++)
    {
        float value = tex3D(gVolumeTexture, fx + k, fy, fz);
        min = fminf(min, value);
        max = fmaxf(max, value);
        sum += value;
    }

    uint i = x + y * dims.width + z * dims.width * dims.height;

    minMax[i].x = min;
    minMax[i].y = max;
    mean[i] = sum / (2 * radius + 1);
}


// separem els mínims i màxims en un volum i les mitjanes en un altre perquè CUDA no accepta volums de float3
__global__ void minMaxMeanFilteringYKernel(float2 *minMax, float *mean, int radius, cudaExtent dims)
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

    float min = CUDART_INF_F, max = 0.0f, sum = 0.0f;

    for (int k = -radius; k <= radius; k++)
    {
        float2 value = tex3D(gMinMaxVolumeTexture, fx, fy + k, fz);
        min = fminf(min, value.x);
        max = fmaxf(max, value.y);
        sum += tex3D(gMeanVolumeTexture, fx, fy + k, fz);
    }

    uint i = x + y * dims.width + z * dims.width * dims.height;

    minMax[i].x = min;
    minMax[i].y = max;
    mean[i] = sum / (2 * radius + 1);
}


// separem els mínims i màxims en un volum i les mitjanes en un altre perquè CUDA no accepta volums de float3
__global__ void minMaxMeanFilteringZKernel(float2 *minMax, float *mean, int radius, cudaExtent dims)
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

    float min = CUDART_INF_F, max = 0.0f, sum = 0.0f;

    for (int k = -radius; k <= radius; k++)
    {
        float2 value = tex3D(gMinMaxVolumeTexture, fx, fy, fz + k);
        min = fminf(min, value.x);
        max = fmaxf(max, value.y);
        sum += tex3D(gMeanVolumeTexture, fx, fy, fz + k);
    }

    uint i = x + y * dims.width + z * dims.width * dims.height;

    minMax[i].x = min;
    minMax[i].y = max;
    mean[i] = sum / (2 * radius + 1);
}


// separem els mínims i màxims en un volum i les mitjanes en un altre perquè CUDA no accepta volums de float3
__global__ void finalTangentSphereCdfKernel(float *result, int radius, cudaExtent dims)
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

    float vstar = tex3D(gVolumeTexture, fx, fy, fz);
    float3 normal = normalize(make_float3(tex3D(gVolumeTexture, fx + 1.0f, fy, fz) - tex3D(gVolumeTexture, fx - 1.0f, fy, fz),
                                          tex3D(gVolumeTexture, fx, fy + 1.0f, fz) - tex3D(gVolumeTexture, fx, fy - 1.0f, fz),
                                          tex3D(gVolumeTexture, fx, fy, fz + 1.0f) - tex3D(gVolumeTexture, fx, fy, fz - 1.0f)));
    float3 c = normal * radius / 2.0f;
    float cx = fx + c.x, cy = fy + c.y, cz = fz + c.z;
    float2 minMax = tex3D(gMinMaxVolumeTexture, cx, cy, cz);
    float vmin = minMax.x;
    float vmax = minMax.y;
    float mean = tex3D(gMeanVolumeTexture, cx, cy, cz);
    float Dv = vmax - vmin;

    float a2 = 3 * (2 * mean - vmax - vmin) / Dv;
    if (a2 < -1) a2 = -1;
    if (a2 > 1) a2 = 1;

    float a1 = 1 - a2;

    float ao = 1;   // obscurance
    if (vstar < vmin) ao = 0;
    else if (vstar < vmax )
    {
        float t = (vstar - vmin) / Dv;
        ao = a2 * t * t + a1 * t;
    }

    uint i = x + y * dims.width + z * dims.width * dims.height;
    result[i] = 1 - ao;
}


// separem els mínims i màxims en un volum i les mitjanes en un altre perquè CUDA no accepta volums de float3
QVector<float> cfProbabilisticAmbientOcclusionTangentSphereCdf( vtkImageData *image, int radius )
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
    CUDA_SAFE_CALL( cudaMemcpy3D(&copyParams) );    // còpia síncrona perquè si un dels dos és el host ha de ser memòria reservada amb cudaMallocHost
    //gVolumeTexture.normalized = false;                      // false (predeterminat) -> [0,N) | true -> [0,1)
    //gVolumeTexture.filterMode = cudaFilterModePoint;        // cudaFilterModePoint (predeterminat) o cudaFilterModeLinear
    //gVolumeTexture.addressMode[0] = cudaAddressModeClamp;   // cudaAddressModeClamp (retallar) (predeterminat) o cudaAddressModeWrap (fer la volta)
    //gVolumeTexture.addressMode[1] = cudaAddressModeClamp;
    //gVolumeTexture.addressMode[2] = cudaAddressModeClamp;
    CUDA_SAFE_CALL( cudaBindTextureToArray(gVolumeTexture, dVolumeArray, channelDescVolumeArray) );

    // Crear un array pel volum de mínims i màxims, amb la seva textura corresponent
    cudaArray *dMinMaxVolumeArray;
    cudaChannelFormatDesc channelDescMinMaxVolumeArray = cudaCreateChannelDesc<float2>();
    CUDA_SAFE_CALL( cudaMalloc3DArray(&dMinMaxVolumeArray, &channelDescMinMaxVolumeArray, volumeDataDims) );
    //gMinMaxVolumeTexture.normalized = false;                    // false (predeterminat) -> [0,N) | true -> [0,1)
    //gMinMaxVolumeTexture.filterMode = cudaFilterModePoint;      // cudaFilterModePoint (predeterminat) o cudaFilterModeLinear
    //gMinMaxVolumeTexture.addressMode[0] = cudaAddressModeClamp; // cudaAddressModeClamp (retallar) (predeterminat) o cudaAddressModeWrap (fer la volta)
    //gMinMaxVolumeTexture.addressMode[1] = cudaAddressModeClamp;
    //gMinMaxVolumeTexture.addressMode[2] = cudaAddressModeClamp;
    CUDA_SAFE_CALL( cudaBindTextureToArray(gMinMaxVolumeTexture, dMinMaxVolumeArray, channelDescMinMaxVolumeArray) );

    // Reservar espai pels resultats intermitjos de mínims i màxims del filtratge
    float2 *dfMinMaxResult;
    CUDA_SAFE_CALL( cudaMalloc(reinterpret_cast<void**>(&dfMinMaxResult), VOLUME_DATA_SIZE * sizeof(float2)) );

    // Crear un array pel volum de mitjanes, amb la seva textura corresponent
    cudaArray *dMeanVolumeArray;
    cudaChannelFormatDesc channelDescMeanVolumeArray = cudaCreateChannelDesc<float>();
    CUDA_SAFE_CALL( cudaMalloc3DArray(&dMeanVolumeArray, &channelDescMeanVolumeArray, volumeDataDims) );
    //gMeanVolumeTexture.normalized = false;                      // false (predeterminat) -> [0,N) | true -> [0,1)
    //gMeanVolumeTexture.filterMode = cudaFilterModePoint;        // cudaFilterModePoint (predeterminat) o cudaFilterModeLinear
    //gMeanVolumeTexture.addressMode[0] = cudaAddressModeClamp;   // cudaAddressModeClamp (retallar) (predeterminat) o cudaAddressModeWrap (fer la volta)
    //gMeanVolumeTexture.addressMode[1] = cudaAddressModeClamp;
    //gMeanVolumeTexture.addressMode[2] = cudaAddressModeClamp;
    CUDA_SAFE_CALL( cudaBindTextureToArray(gMeanVolumeTexture, dMeanVolumeArray, channelDescMeanVolumeArray) );

    // Reservar espai pels resultats intermitjos de mitjanes del filtratge
    float *dfMeanResult;
    CUDA_SAFE_CALL( cudaMalloc(reinterpret_cast<void**>(&dfMeanResult), VOLUME_DATA_SIZE * sizeof(float)) );

    // Preparar l'execució
    //Block width should be a multiple of maximum coalesced write size
    //for coalesced memory writes in convolutionRowGPU() and convolutionColumnGPU()
    dim3 threadBlock(16, 8, 4);
    uint blocksX = iDivUp(volumeDataDims.width, threadBlock.x);
    uint blocksY = iDivUp(volumeDataDims.height, threadBlock.y);
    uint blocksZ = iDivUp(volumeDataDims.depth, threadBlock.z);
    dim3 blockGrid(blocksX * blocksY, blocksZ);

    // Executar per X
    minMaxMeanFilteringXKernel<<<blockGrid, threadBlock>>>(dfMinMaxResult, dfMeanResult, radius, volumeDataDims);
    CUDA_SAFE_CALL( cudaThreadSynchronize() );

    // Copiar els resultats als arrays
    copyParams.srcPtr = make_cudaPitchedPtr(reinterpret_cast<void*>(dfMinMaxResult), dimensions[0] * sizeof(float2), dimensions[0], dimensions[1]); // data, pitch, width, height
    copyParams.dstArray = dMinMaxVolumeArray;
    copyParams.kind = cudaMemcpyDeviceToDevice;
    CUDA_SAFE_CALL( cudaMemcpy3D(&copyParams) );
    copyParams.srcPtr = make_cudaPitchedPtr(reinterpret_cast<void*>(dfMeanResult), dimensions[0] * sizeof(float), dimensions[0], dimensions[1]);    // data, pitch, width, height
    copyParams.dstArray = dMeanVolumeArray;
    CUDA_SAFE_CALL( cudaMemcpy3D(&copyParams) );

    // Executar per Y
    minMaxMeanFilteringYKernel<<<blockGrid, threadBlock>>>(dfMinMaxResult, dfMeanResult, radius, volumeDataDims);
    CUDA_SAFE_CALL( cudaThreadSynchronize() );

    // Copiar els resultats als arrays
    copyParams.srcPtr = make_cudaPitchedPtr(reinterpret_cast<void*>(dfMinMaxResult), dimensions[0] * sizeof(float2), dimensions[0], dimensions[1]); // data, pitch, width, height
    copyParams.dstArray = dMinMaxVolumeArray;
    CUDA_SAFE_CALL( cudaMemcpy3D(&copyParams) );
    copyParams.srcPtr = make_cudaPitchedPtr(reinterpret_cast<void*>(dfMeanResult), dimensions[0] * sizeof(float), dimensions[0], dimensions[1]);    // data, pitch, width, height
    copyParams.dstArray = dMeanVolumeArray;
    CUDA_SAFE_CALL( cudaMemcpy3D(&copyParams) );

    // Executar per Z
    minMaxMeanFilteringZKernel<<<blockGrid, threadBlock>>>(dfMinMaxResult, dfMeanResult, radius, volumeDataDims);
    CUDA_SAFE_CALL( cudaThreadSynchronize() );

    // Copiar els resultats als arrays
    copyParams.srcPtr = make_cudaPitchedPtr(reinterpret_cast<void*>(dfMinMaxResult), dimensions[0] * sizeof(float2), dimensions[0], dimensions[1]); // data, pitch, width, height
    copyParams.dstArray = dMinMaxVolumeArray;
    CUDA_SAFE_CALL( cudaMemcpy3D(&copyParams) );
    copyParams.srcPtr = make_cudaPitchedPtr(reinterpret_cast<void*>(dfMeanResult), dimensions[0] * sizeof(float), dimensions[0], dimensions[1]);    // data, pitch, width, height
    copyParams.dstArray = dMeanVolumeArray;
    CUDA_SAFE_CALL( cudaMemcpy3D(&copyParams) );

    // Neteja parcial
    CUDA_SAFE_CALL( cudaFree(dfMinMaxResult) );
    CUDA_SAFE_CALL( cudaFree(dfMeanResult) );

    // Reservar espai pel resultat
    float *dfResult;
    CUDA_SAFE_CALL( cudaMalloc(reinterpret_cast<void**>(&dfResult), VOLUME_DATA_SIZE * sizeof(float)) );

    // Ara ja podem fer la passada final
    finalTangentSphereCdfKernel<<<blockGrid, threadBlock>>>(dfResult, radius, volumeDataDims);
    CUDA_SAFE_CALL( cudaThreadSynchronize() );

    // Copiar el resultat final al host
    QVector<float> result(VOLUME_DATA_SIZE);
    CUDA_SAFE_CALL( cudaMemcpy(reinterpret_cast<void*>(result.data()), reinterpret_cast<void*>(dfResult), VOLUME_DATA_SIZE * sizeof(float), cudaMemcpyDeviceToHost) );

    // Neteja
    CUDA_SAFE_CALL( cudaFree(dfResult) );
    CUDA_SAFE_CALL( cudaUnbindTexture(gVolumeTexture) );
    CUDA_SAFE_CALL( cudaUnbindTexture(gMinMaxVolumeTexture) );
    CUDA_SAFE_CALL( cudaUnbindTexture(gMeanVolumeTexture) );
    CUDA_SAFE_CALL( cudaFreeArray(dVolumeArray) );
    CUDA_SAFE_CALL( cudaFreeArray(dMinMaxVolumeArray) );
    CUDA_SAFE_CALL( cudaFreeArray(dMeanVolumeArray) );

    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    float elapsedTime = 0.0f;
    cudaEventElapsedTime(&elapsedTime, start, stop);

    std::cout << "paots cdf: " << elapsedTime << " ms" << std::endl;

    cudaEventDestroy(start);
    cudaEventDestroy(stop);

    return result;
}


__global__ void finalTangentSphereGaussianKernel(float *result, int radius, cudaExtent dims)
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
    float value = tex3D(gVolume3Texture, fx, fy, fz);
    float3 normal = normalize(make_float3(tex3D(gVolume3Texture, fx + 1.0f, fy, fz) - tex3D(gVolume3Texture, fx - 1.0f, fy, fz),
                                          tex3D(gVolume3Texture, fx, fy + 1.0f, fz) - tex3D(gVolume3Texture, fx, fy - 1.0f, fz),
                                          tex3D(gVolume3Texture, fx, fy, fz + 1.0f) - tex3D(gVolume3Texture, fx, fy, fz - 1.0f)));

    float3 c = normal * radius / 2.0f;
    float cx = fx + c.x, cy = fy + c.y, cz = fz + c.z;

    float mean = tex3D(gVolumeTexture, cx, cy, cz); // E[Z]
    float squaresMean = tex3D(gVolume2Texture, cx, cy, cz); // E[Z²]
    float variance = squaresMean - mean * mean;
    if (variance > -0.001f && variance < 0.0f) variance = 0.0f; // per evitar nans

    uint i = x + y * dims.width + z * dims.width * dims.height;

    // Fórmules tretes de:
    // http://en.wikipedia.org/wiki/Gaussian_distribution#Cumulative_distribution_function
    // http://en.wikipedia.org/wiki/Q-function
    // P(Z >= z) = 1/2 * erfc((z - mean) / sqrt(2 * variance))
    result[i] = 0.5f * erfcf((value - mean) / sqrtf(2.0f * variance));
}


QVector<float> cfProbabilisticAmbientOcclusionTangentSphereGaussian(vtkImageData *image, int radius)
{
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start, 0);

    cudaStream_t stream1, stream2;
    cudaStreamCreate(&stream1);
    cudaStreamCreate(&stream2);

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
    CUDA_SAFE_CALL( cudaMemcpy3D(&copyParams) );    // còpia síncrona perquè si un dels dos és el host ha de ser memòria reservada amb cudaMallocHost
    //gVolumeTexture.normalized = false;                      // false (predeterminat) -> [0,N) | true -> [0,1)
    //gVolumeTexture.filterMode = cudaFilterModePoint;        // cudaFilterModePoint (predeterminat) o cudaFilterModeLinear
    //gVolumeTexture.addressMode[0] = cudaAddressModeClamp;   // cudaAddressModeClamp (retallar) (predeterminat) o cudaAddressModeWrap (fer la volta)
    //gVolumeTexture.addressMode[1] = cudaAddressModeClamp;
    //gVolumeTexture.addressMode[2] = cudaAddressModeClamp;
    CUDA_SAFE_CALL( cudaBindTextureToArray(gVolumeTexture, dVolumeArray, channelDescVolumeArray) );

    // Reservar espai pel resultat
    float *dfResult;
    CUDA_SAFE_CALL( cudaMalloc(reinterpret_cast<void**>(&dfResult), VOLUME_DATA_SIZE * sizeof(float)) );

    // Preparar l'execució
    //Block width should be a multiple of maximum coalesced write size
    //for coalesced memory writes in convolutionRowGPU() and convolutionColumnGPU()
    dim3 threadBlock(16, 8, 4);
    uint blocksX = iDivUp(volumeDataDims.width, threadBlock.x);
    uint blocksY = iDivUp(volumeDataDims.height, threadBlock.y);
    uint blocksZ = iDivUp(volumeDataDims.depth, threadBlock.z);
    dim3 blockGrid(blocksX * blocksY, blocksZ);

    // Calcular volum al quadrat
    squareKernel<<<blockGrid, threadBlock, 0, stream2>>>(dfResult, volumeDataDims); // generem el volum al quadrat a l'stream 2

    // Mentrestant, al host...

    // Crear un segon array pel volum al quadrat, amb la seva textura corresponent
    cudaArray *dVolume2Array;
    cudaChannelFormatDesc channelDescVolume2Array = cudaCreateChannelDesc<float>();
    CUDA_SAFE_CALL( cudaMalloc3DArray(&dVolume2Array, &channelDescVolume2Array, volumeDataDims) );
    //gVolume2Texture.normalized = false;                     // false (predeterminat) -> [0,N) | true -> [0,1)
    //gVolume2Texture.filterMode = cudaFilterModePoint;       // cudaFilterModePoint (predeterminat) o cudaFilterModeLinear
    //gVolume2Texture.addressMode[0] = cudaAddressModeClamp;  // cudaAddressModeClamp (retallar) (predeterminat) o cudaAddressModeWrap (fer la volta)
    //gVolume2Texture.addressMode[1] = cudaAddressModeClamp;
    //gVolume2Texture.addressMode[2] = cudaAddressModeClamp;
    CUDA_SAFE_CALL( cudaBindTextureToArray(gVolume2Texture, dVolume2Array, channelDescVolume2Array) );

    // En aquest temps suposem que ja ha acabat el kernel d'abans (stream 2)
    cudaStreamSynchronize(stream2);

    // Copiem el resultat a l'array del volum al quadrat
    cudaMemcpy3DParms copyParams2 = {0};
    copyParams2.srcPtr = make_cudaPitchedPtr(reinterpret_cast<void*>(dfResult), dimensions[0] * sizeof(float), dimensions[0], dimensions[1]);   // data, pitch, width, height
    copyParams2.dstArray = dVolume2Array;
    copyParams2.extent = volumeDataDims;
    copyParams2.kind = cudaMemcpyDeviceToDevice;
    CUDA_SAFE_CALL_NO_SYNC( cudaMemcpy3DAsync(&copyParams2, stream2) ); // còpia a l'stream 2

    // Reservar espai per l'altre resultat
    float *dfResult2;
    CUDA_SAFE_CALL( cudaMalloc(reinterpret_cast<void**>(&dfResult2), VOLUME_DATA_SIZE * sizeof(float)) );

    // Calcular kernel
    const int KERNEL_WIDTH = 2 * radius + 1;
    QVector<float> kernel(KERNEL_WIDTH);
    kernel.fill(1.0f / KERNEL_WIDTH);
    std::cout << "kernel:";
    for (int i = 0; i < KERNEL_WIDTH; i++) std::cout << " " << kernel[i];
    std::cout << std::endl;
    float *dfKernel;
    CUDA_SAFE_CALL( cudaMalloc(reinterpret_cast<void**>(&dfKernel), KERNEL_WIDTH * sizeof(float)) );
    // Aquest és molt petit, o sigui que el podem fer síncron
    CUDA_SAFE_CALL( cudaMemcpy(reinterpret_cast<void*>(dfKernel), reinterpret_cast<void*>(kernel.data()), KERNEL_WIDTH * sizeof(float), cudaMemcpyHostToDevice) );

    // Aquí ja hauria d'haver acabat la còpia d'abans (stream 2)
    cudaStreamSynchronize(stream2);

    // A partir d'aquí comença la diversió:
    // Executarem els dos filtratges en paral·lel, en streams diferents, perquè mentre un filtra l'altre copiï memòria i viceversa.
    // Així podem aconseguir una execució més ràpida (en teoria).
    // Som-hi doncs...

    // Executar per X1
    convolutionXKernel<<<blockGrid, threadBlock, 0, stream1>>>(dfResult, dfKernel, radius, volumeDataDims, false);
    // Executar per X2
    convolutionXKernel<<<blockGrid, threadBlock, 0, stream2>>>(dfResult2, dfKernel, radius, volumeDataDims, true);

    // Copiar el resultat a l'array (1)
    copyParams.srcPtr = make_cudaPitchedPtr(reinterpret_cast<void*>(dfResult), dimensions[0] * sizeof(float), dimensions[0], dimensions[1]);    // data, pitch, width, height
    copyParams.kind = cudaMemcpyDeviceToDevice;
    cudaStreamSynchronize(stream1);
    CUDA_SAFE_CALL_NO_SYNC( cudaMemcpy3DAsync(&copyParams, stream1) );
    // Copiar el resultat a l'array (2)
    copyParams2.srcPtr = make_cudaPitchedPtr(reinterpret_cast<void*>(dfResult2), dimensions[0] * sizeof(float), dimensions[0], dimensions[1]);  // data, pitch, width, height
    cudaStreamSynchronize(stream2);
    CUDA_SAFE_CALL_NO_SYNC( cudaMemcpy3DAsync(&copyParams2, stream2) );

    // Executar per Y1
    cudaStreamSynchronize(stream1);
    convolutionYKernel<<<blockGrid, threadBlock, 0, stream1>>>(dfResult, dfKernel, radius, volumeDataDims, false);
    // Executar per Y2
    cudaStreamSynchronize(stream2);
    convolutionYKernel<<<blockGrid, threadBlock, 0, stream2>>>(dfResult2, dfKernel, radius, volumeDataDims, true);

    // Copiar el resultat a l'array (1)
    cudaStreamSynchronize(stream1);
    CUDA_SAFE_CALL_NO_SYNC( cudaMemcpy3DAsync(&copyParams, stream1) );
    // Copiar el resultat a l'array (2)
    cudaStreamSynchronize(stream2);
    CUDA_SAFE_CALL_NO_SYNC( cudaMemcpy3DAsync(&copyParams2, stream2) );

    // Executar per Z1
    cudaStreamSynchronize(stream1);
    convolutionZKernel<<<blockGrid, threadBlock, 0, stream1>>>(dfResult, dfKernel, radius, volumeDataDims, false);
    // Executar per Z2
    cudaStreamSynchronize(stream2);
    convolutionZKernel<<<blockGrid, threadBlock, 0, stream2>>>(dfResult2, dfKernel, radius, volumeDataDims, true);
    
    // Copiar el resultat a l'array (1)
    cudaStreamSynchronize(stream1);
    CUDA_SAFE_CALL_NO_SYNC( cudaMemcpy3DAsync(&copyParams, stream1) );
    // Copiar el resultat a l'array (2)
    cudaStreamSynchronize(stream2);
    CUDA_SAFE_CALL_NO_SYNC( cudaMemcpy3DAsync(&copyParams2, stream2) );

    // Copiar el volum a un array i associar-hi una textura
    cudaArray *dVolume3Array;
    cudaChannelFormatDesc channelDescVolume3Array = cudaCreateChannelDesc<float>();
    CUDA_SAFE_CALL( cudaMalloc3DArray(&dVolume3Array, &channelDescVolume3Array, volumeDataDims) );
    cudaMemcpy3DParms copyParams3 = {0};
    copyParams3.srcPtr = make_cudaPitchedPtr(reinterpret_cast<void*>(data), dimensions[0] * sizeof(float), dimensions[0], dimensions[1]);    // data, pitch, width, height
    copyParams3.dstArray = dVolume3Array;
    copyParams3.extent = volumeDataDims;
    copyParams3.kind = cudaMemcpyHostToDevice;
    CUDA_SAFE_CALL( cudaMemcpy3D(&copyParams3) );    // còpia síncrona perquè si un dels dos és el host ha de ser memòria reservada amb cudaMallocHost
    //gVolume3Texture.normalized = false;                      // false (predeterminat) -> [0,N) | true -> [0,1)
    //gVolume3Texture.filterMode = cudaFilterModePoint;        // cudaFilterModePoint (predeterminat) o cudaFilterModeLinear
    //gVolume3Texture.addressMode[0] = cudaAddressModeClamp;   // cudaAddressModeClamp (retallar) (predeterminat) o cudaAddressModeWrap (fer la volta)
    //gVolume3Texture.addressMode[1] = cudaAddressModeClamp;
    //gVolume3Texture.addressMode[2] = cudaAddressModeClamp;
    CUDA_SAFE_CALL( cudaBindTextureToArray(gVolume3Texture, dVolume3Array, channelDescVolumeArray) );
    CUDA_SAFE_CALL( cudaThreadSynchronize() );

    // Ara ja podem fer la passada final
    finalTangentSphereGaussianKernel<<<blockGrid, threadBlock>>>(dfResult, radius, volumeDataDims);
    CUDA_SAFE_CALL( cudaThreadSynchronize() );

    // Copiar el resultat final al host
    QVector<float> result(VOLUME_DATA_SIZE);
    CUDA_SAFE_CALL( cudaMemcpy(reinterpret_cast<void*>(result.data()), reinterpret_cast<void*>(dfResult), VOLUME_DATA_SIZE * sizeof(float), cudaMemcpyDeviceToHost) );

    // Neteja
    CUDA_SAFE_CALL( cudaFree(dfKernel) );
    CUDA_SAFE_CALL( cudaFree(dfResult) );
    CUDA_SAFE_CALL( cudaFree(dfResult2) );
    CUDA_SAFE_CALL( cudaUnbindTexture(gVolumeTexture) );
    CUDA_SAFE_CALL( cudaUnbindTexture(gVolume2Texture) );
    CUDA_SAFE_CALL( cudaUnbindTexture(gVolume3Texture) );
    CUDA_SAFE_CALL( cudaFreeArray(dVolumeArray) );
    CUDA_SAFE_CALL( cudaFreeArray(dVolume2Array) );
    CUDA_SAFE_CALL( cudaFreeArray(dVolume3Array) );

    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    float elapsedTime = 0.0f;
    cudaEventElapsedTime(&elapsedTime, start, stop);

    std::cout << "paots gaussian: " << elapsedTime << " ms" << std::endl;

    cudaStreamDestroy(stream1);
    cudaStreamDestroy(stream2);

    cudaEventDestroy(start);
    cudaEventDestroy(stop);

    return result;
}


QVector<float> cfVolumeVariance(vtkImageData *image, int radius)
{
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start, 0);

    cudaStream_t stream1, stream2;
    cudaStreamCreate(&stream1);
    cudaStreamCreate(&stream2);

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
    CUDA_SAFE_CALL( cudaMemcpy3D(&copyParams) );    // còpia síncrona perquè si un dels dos és el host ha de ser memòria reservada amb cudaMallocHost
    //gVolumeTexture.normalized = false;                      // false (predeterminat) -> [0,N) | true -> [0,1)
    //gVolumeTexture.filterMode = cudaFilterModePoint;        // cudaFilterModePoint (predeterminat) o cudaFilterModeLinear
    //gVolumeTexture.addressMode[0] = cudaAddressModeClamp;   // cudaAddressModeClamp (retallar) (predeterminat) o cudaAddressModeWrap (fer la volta)
    //gVolumeTexture.addressMode[1] = cudaAddressModeClamp;
    //gVolumeTexture.addressMode[2] = cudaAddressModeClamp;
    CUDA_SAFE_CALL( cudaBindTextureToArray(gVolumeTexture, dVolumeArray, channelDescVolumeArray) );

    // Reservar espai pel resultat
    float *dfResult;
    CUDA_SAFE_CALL( cudaMalloc(reinterpret_cast<void**>(&dfResult), VOLUME_DATA_SIZE * sizeof(float)) );

    // Preparar l'execució
    //Block width should be a multiple of maximum coalesced write size
    //for coalesced memory writes in convolutionRowGPU() and convolutionColumnGPU()
    dim3 threadBlock(16, 8, 4);
    uint blocksX = iDivUp(volumeDataDims.width, threadBlock.x);
    uint blocksY = iDivUp(volumeDataDims.height, threadBlock.y);
    uint blocksZ = iDivUp(volumeDataDims.depth, threadBlock.z);
    dim3 blockGrid(blocksX * blocksY, blocksZ);

    // Calcular volum al quadrat
    squareKernel<<<blockGrid, threadBlock, 0, stream2>>>(dfResult, volumeDataDims); // generem el volum al quadrat a l'stream 2

    // Mentrestant, al host...

    // Crear un segon array pel volum al quadrat, amb la seva textura corresponent
    cudaArray *dVolume2Array;
    cudaChannelFormatDesc channelDescVolume2Array = cudaCreateChannelDesc<float>();
    CUDA_SAFE_CALL( cudaMalloc3DArray(&dVolume2Array, &channelDescVolume2Array, volumeDataDims) );
    //gVolume2Texture.normalized = false;                     // false (predeterminat) -> [0,N) | true -> [0,1)
    //gVolume2Texture.filterMode = cudaFilterModePoint;       // cudaFilterModePoint (predeterminat) o cudaFilterModeLinear
    //gVolume2Texture.addressMode[0] = cudaAddressModeClamp;  // cudaAddressModeClamp (retallar) (predeterminat) o cudaAddressModeWrap (fer la volta)
    //gVolume2Texture.addressMode[1] = cudaAddressModeClamp;
    //gVolume2Texture.addressMode[2] = cudaAddressModeClamp;
    CUDA_SAFE_CALL( cudaBindTextureToArray(gVolume2Texture, dVolume2Array, channelDescVolume2Array) );

    // En aquest temps suposem que ja ha acabat el kernel d'abans (stream 2)
    cudaStreamSynchronize(stream2);

    // Copiem el resultat a l'array del volum al quadrat
    cudaMemcpy3DParms copyParams2 = {0};
    copyParams2.srcPtr = make_cudaPitchedPtr(reinterpret_cast<void*>(dfResult), dimensions[0] * sizeof(float), dimensions[0], dimensions[1]);   // data, pitch, width, height
    copyParams2.dstArray = dVolume2Array;
    copyParams2.extent = volumeDataDims;
    copyParams2.kind = cudaMemcpyDeviceToDevice;
    CUDA_SAFE_CALL_NO_SYNC( cudaMemcpy3DAsync(&copyParams2, stream2) ); // còpia a l'stream 2

    // Reservar espai per l'altre resultat
    float *dfResult2;
    CUDA_SAFE_CALL( cudaMalloc(reinterpret_cast<void**>(&dfResult2), VOLUME_DATA_SIZE * sizeof(float)) );

    // Calcular kernel
    const int KERNEL_WIDTH = 2 * radius + 1;
    QVector<float> kernel(KERNEL_WIDTH);
    kernel.fill(1.0f / KERNEL_WIDTH);
    std::cout << "kernel:";
    for (int i = 0; i < KERNEL_WIDTH; i++) std::cout << " " << kernel[i];
    std::cout << std::endl;
    float *dfKernel;
    CUDA_SAFE_CALL( cudaMalloc(reinterpret_cast<void**>(&dfKernel), KERNEL_WIDTH * sizeof(float)) );
    // Aquest és molt petit, o sigui que el podem fer síncron
    CUDA_SAFE_CALL( cudaMemcpy(reinterpret_cast<void*>(dfKernel), reinterpret_cast<void*>(kernel.data()), KERNEL_WIDTH * sizeof(float), cudaMemcpyHostToDevice) );

    // Aquí ja hauria d'haver acabat la còpia d'abans (stream 2)
    cudaStreamSynchronize(stream2);

    // A partir d'aquí comença la diversió:
    // Executarem els dos filtratges en paral·lel, en streams diferents, perquè mentre un filtra l'altre copiï memòria i viceversa.
    // Així podem aconseguir una execució més ràpida (en teoria).
    // Som-hi doncs...

    // Executar per X1
    convolutionXKernel<<<blockGrid, threadBlock, 0, stream1>>>(dfResult, dfKernel, radius, volumeDataDims, false);
    // Executar per X2
    convolutionXKernel<<<blockGrid, threadBlock, 0, stream2>>>(dfResult2, dfKernel, radius, volumeDataDims, true);

    // Copiar el resultat a l'array (1)
    copyParams.srcPtr = make_cudaPitchedPtr(reinterpret_cast<void*>(dfResult), dimensions[0] * sizeof(float), dimensions[0], dimensions[1]);    // data, pitch, width, height
    copyParams.kind = cudaMemcpyDeviceToDevice;
    cudaStreamSynchronize(stream1);
    CUDA_SAFE_CALL_NO_SYNC( cudaMemcpy3DAsync(&copyParams, stream1) );
    // Copiar el resultat a l'array (2)
    copyParams2.srcPtr = make_cudaPitchedPtr(reinterpret_cast<void*>(dfResult2), dimensions[0] * sizeof(float), dimensions[0], dimensions[1]);  // data, pitch, width, height
    cudaStreamSynchronize(stream2);
    CUDA_SAFE_CALL_NO_SYNC( cudaMemcpy3DAsync(&copyParams2, stream2) );

    // Executar per Y1
    cudaStreamSynchronize(stream1);
    convolutionYKernel<<<blockGrid, threadBlock, 0, stream1>>>(dfResult, dfKernel, radius, volumeDataDims, false);
    // Executar per Y2
    cudaStreamSynchronize(stream2);
    convolutionYKernel<<<blockGrid, threadBlock, 0, stream2>>>(dfResult2, dfKernel, radius, volumeDataDims, true);

    // Copiar el resultat a l'array (1)
    cudaStreamSynchronize(stream1);
    CUDA_SAFE_CALL_NO_SYNC( cudaMemcpy3DAsync(&copyParams, stream1) );
    // Copiar el resultat a l'array (2)
    cudaStreamSynchronize(stream2);
    CUDA_SAFE_CALL_NO_SYNC( cudaMemcpy3DAsync(&copyParams2, stream2) );

    // Executar per Z1
    cudaStreamSynchronize(stream1);
    convolutionZKernel<<<blockGrid, threadBlock, 0, stream1>>>(dfResult, dfKernel, radius, volumeDataDims, false);
    // Executar per Z2
    cudaStreamSynchronize(stream2);
    convolutionZKernel<<<blockGrid, threadBlock, 0, stream2>>>(dfResult2, dfKernel, radius, volumeDataDims, true);

    // Ara ja podem fer la passada final
    CUDA_SAFE_CALL( cudaThreadSynchronize() );
    finalChebychevKernel<<<blockGrid, threadBlock>>>(dfResult, dfResult2, volumeDataDims);
    CUDA_SAFE_CALL( cudaThreadSynchronize() );

    // Copiar el resultat final al host
    QVector<float> result(VOLUME_DATA_SIZE);
    CUDA_SAFE_CALL( cudaMemcpy(reinterpret_cast<void*>(result.data()), reinterpret_cast<void*>(dfResult), VOLUME_DATA_SIZE * sizeof(float), cudaMemcpyDeviceToHost) );

    // Neteja
    CUDA_SAFE_CALL( cudaFree(dfKernel) );
    CUDA_SAFE_CALL( cudaFree(dfResult) );
    CUDA_SAFE_CALL( cudaFree(dfResult2) );
    CUDA_SAFE_CALL( cudaUnbindTexture(gVolumeTexture) );
    CUDA_SAFE_CALL( cudaUnbindTexture(gVolume2Texture) );
    CUDA_SAFE_CALL( cudaFreeArray(dVolumeArray) );
    CUDA_SAFE_CALL( cudaFreeArray(dVolume2Array) );

    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    float elapsedTime = 0.0f;
    cudaEventElapsedTime(&elapsedTime, start, stop);

    std::cout << "volume variance: " << elapsedTime << " ms" << std::endl;

    cudaStreamDestroy(stream1);
    cudaStreamDestroy(stream2);

    cudaEventDestroy(start);
    cudaEventDestroy(stop);

    return result;
}
