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
texture<float, 3> gVolume2Texture;  // el 3r paràmetre pot ser cudaReadModeElementType (valor directe) (predeterminat) o cudaReadModeNormalizedFloat (valor escalat entre 0 i 1)


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

    std::cout << "box mean chebychev: " << elapsedTime << " ms" << std::endl;

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

    std::cout << "box mean chebychev: " << elapsedTime << " ms" << std::endl;

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

    std::cout << "box mean chebychev: " << elapsedTime << " ms" << std::endl;

    cudaStreamDestroy(stream1);
    cudaStreamDestroy(stream2);

    cudaEventDestroy(start);
    cudaEventDestroy(stop);

    return result;
}
