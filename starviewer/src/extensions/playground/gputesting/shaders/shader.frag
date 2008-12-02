uniform sampler2D tex;
uniform sampler3D volume_tex;
uniform float stepsize;

// IN.TexCoord -> gl_TexCoord[0]
// IN.Color -> gl_TexCoord[1]
// IN.Pos -> gl_TexCoord[2]

void main()
{
    vec2 texc = ((gl_TexCoord[2].xy / gl_TexCoord[2].w) + 1.0) / 2.0;   // find the right place to lookup in the backside buffer
    vec4 start = gl_TexCoord[0];                                        // the start position of the ray is stored in the texturecoordinate
    vec4 back_position = texture2D(tex, texc);
    vec3 dir = back_position.xyz - start.xyz;
    float len = length(dir);
    vec3 norm_dir = normalize(dir);
    float delta = 1.0 / 50.0;
    vec3 delta_dir = norm_dir * delta;
    float delta_dir_len = length(delta_dir);
    vec3 vec = start.xyz;
    vec4 col_acc = vec4(0, 0, 0, 0);
    float alpha_acc = 0.0;
    float length_acc = 0.0;
    vec4 color_sample = vec4(0, 0, 0, 0);
    float alpha_sample;
    float steps = 0.0;
    bool found = false;
    int i;
    vec4 maxColorSample = vec4(0.0);

    for(i = 0; i < 450; i++)
    {
        color_sample = texture3D(volume_tex, vec);
        if (color_sample.r > maxColorSample.r) maxColorSample = color_sample;
//         found = found && color_sample.r == color_sample.g && color_sample.g == color_sample.b && color_sample.b == color_sample.a;  // no
        found = found || color_sample.r > 0.0;
        alpha_sample = color_sample.a;// * delta;
        col_acc   += (1.0 - alpha_acc) * color_sample * alpha_sample;// * 3.0;
        alpha_acc += alpha_sample;
        vec += delta_dir;
        length_acc += delta_dir_len;
        if(length_acc >= len || alpha_acc > 1.0) break; // terminate if opacity > 1 or the ray is outside the volume
    }

    // Fragment color
//     if (found) gl_FragColor = vec4(1, 1, 1, 1);
//     else gl_FragColor = vec4(1, 0, 0, 1);
//     gl_FragColor = maxColorSample;
//     gl_FragColor = vec4(-dir, 1);
    gl_FragColor = col_acc;
}
/*

{
  fragment_out OUT;
  float2 texc = ((IN.Pos.xy / IN.Pos.w) + 1) / 2; // find the right place to lookup in the backside buffer
  float4 start = IN.TexCoord; // the start position of the ray is stored in the texturecoordinate
  float4 back_position  = tex2D(tex, texc);
  float3 dir = float3(0,0,0);
  dir.x = back_position.x - start.x;
  dir.y = back_position.y - start.y;
  dir.z = back_position.z - start.z;
  float len = length(dir.xyz); // the length from front to back is calculated and used to terminate the ray
  float3 norm_dir = normalize(dir);
  float delta = stepsize;
  float3 delta_dir = norm_dir * delta;
  float delta_dir_len = length(delta_dir);
  float3 vec = start;
  float4 col_acc = float4(0,0,0,0);
  float alpha_acc = 0;
  float length_acc = 0;
  float4 color_sample;
  float alpha_sample;

  for(int i = 0; i < 450; i++)
    {
      color_sample = tex3D(volume_tex,vec);
      alpha_sample = color_sample.a * stepsize;
      col_acc   += (1.0 - alpha_acc) * color_sample * alpha_sample * 3;
      alpha_acc += alpha_sample;
      vec += delta_dir;
      length_acc += delta_dir_len;
      if(length_acc >= len || alpha_acc > 1.0) break; // terminate if opacity > 1 or the ray is outside the volume
    }

  OUT.Color =  col_acc;
  return OUT;
}
*/
