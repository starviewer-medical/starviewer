void main()
{
    gl_Position = ftransform();
    gl_FrontColor = gl_Color;
    gl_TexCoord[0] = ftransform();
}
