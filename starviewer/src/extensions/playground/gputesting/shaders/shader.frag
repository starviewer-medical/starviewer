uniform sampler2D uFramebufferTexture;
uniform vec3 uDimensions;
uniform float uRayStep;
uniform vec3 uBackgroundColor;

// gl_TexCoord[0] és la posició del vèrtex

// Retorna un color RGBA a partir d'una coordenada.
vec4 shade(vec3 coord);

void main()
{
    const float OPAQUE_ALPHA = 0.9;

    vec3 startCoord = gl_Color.rgb;
    vec2 framebufferCoord = ((gl_TexCoord[0].xy / gl_TexCoord[0].w) + 1.0) / 2.0;   // no sé ben bé d'on baixa això però funciona
    vec3 endCoord = texture2D(uFramebufferTexture, framebufferCoord).rgb;
    vec3 startPoint = startCoord * uDimensions;
    vec3 endPoint = endCoord * uDimensions;
    vec3 direction = normalize(endPoint - startPoint);
    vec3 pointStep = direction * uRayStep;
    vec3 coordStep = pointStep / uDimensions;

    vec4 color = vec4(0.0);
    float remainingOpacity = 1.0;
    vec3 coord = startCoord;

    for (int i = 0; i < 512; i++)
    {
        vec4 colorSample = shade(coord);
        color.rgb += colorSample.rgb * colorSample.a * remainingOpacity;
        remainingOpacity *= 1.0 - colorSample.a;
        color.a = 1.0 - remainingOpacity;

        if (color.a >= OPAQUE_ALPHA) break;

        coord += coordStep;

        if (any(lessThan(coord, vec3(0.0))) || any(greaterThan(coord, vec3(1.0)))) break;
    }

    if (color.a < OPAQUE_ALPHA) color.rgb += uBackgroundColor * remainingOpacity;

    gl_FragColor = color;
}
