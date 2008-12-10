void main()
{
    // Transform and output vertex position
    gl_Position = ftransform();
    gl_FrontColor = gl_Color;
    //gl_TexCoord[0] = gl_MultiTexCoord1; // TexCoord
    gl_TexCoord[0] = gl_Vertex; // TexCoord
    gl_TexCoord[1] = gl_Color;          // Color
    gl_TexCoord[2] = ftransform();      // Pos
}
