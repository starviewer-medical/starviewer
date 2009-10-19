uniform sampler3D uVolumeTexture;
uniform sampler1D uTransferFunctionTexture;

vec4 diffuseShade(vec3 coord, vec3 unit, vec3 direction)
{
    float xp = texture3D(uVolumeTexture, coord + vec3(unit.x, 0.0, 0.0)).r;
    float xm = texture3D(uVolumeTexture, coord - vec3(unit.x, 0.0, 0.0)).r;
    float yp = texture3D(uVolumeTexture, coord + vec3(0.0, unit.y, 0.0)).r;
    float ym = texture3D(uVolumeTexture, coord - vec3(0.0, unit.y, 0.0)).r;
    float zp = texture3D(uVolumeTexture, coord + vec3(0.0, 0.0, unit.z)).r;
    float zm = texture3D(uVolumeTexture, coord - vec3(0.0, 0.0, unit.z)).r;
    vec3 gradient = vec3(xp - xm, yp - ym, zp - zm);
    vec3 normal = normalize(gradient);

    vec4 intensitySample = texture3D(uVolumeTexture, coord);
    vec4 colorSample = texture1D(uTransferFunctionTexture, intensitySample.r);
    vec3 diffuseColor = dot(direction, normal) * colorSample.rgb;
    return vec4(diffuseColor, colorSample.a);
}
