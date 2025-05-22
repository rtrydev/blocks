#version 330 core

// No color output needed for depth map
// If your OpenGL version requires an output, you can write:
// out float fragmentdepth;
// void main() { fragmentdepth = gl_FragCoord.z; }

void main()
{
    // The depth value is automatically written by the rasterizer
    // when the fragment shader doesn't explicitly write to gl_FragDepth.
}
