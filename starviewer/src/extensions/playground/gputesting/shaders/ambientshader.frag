uniform sampler3D uVolumeTexture;
uniform sampler1D uTransferFunctionTexture;

vec4 ambientShade(vec3 coord)
{
    vec4 intensitySample = texture3D(uVolumeTexture, coord);
    return texture1D(uTransferFunctionTexture, intensitySample.r);
}

vec4 shade(vec3 coord)
{
    return ambientShade(coord);
}
