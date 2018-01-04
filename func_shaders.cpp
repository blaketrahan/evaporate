// -----------------------------------------------------------------------------
static const GLchar *polygon_shader_vert = 
"layout (location = 0) in vec4 position;"
"flat out vec4 in_color;" 
"uniform vec2 offset;"
"uniform vec4 color;"
"uniform vec2 scale;" 
"uniform mat4 rotation;"
"void main()"
"{" 
    "vec4 vertex = gl_Vertex;"
    "vertex.x *= scale.x;"
    "vertex.y *= scale.y;" 
    "vertex *= rotation;"
    "vec4 totalOffset = vec4(offset.x, offset.y, 0.0, 0.0);"
    "gl_Position = (gl_ModelViewProjectionMatrix * vertex) + totalOffset;"
    "in_color = vec4(color.r,color.g,color.b,color.a);"
"}\0";

static const GLchar *polygon_shader_frag =  
"layout(location = 0) out vec4 color;"
"flat in vec4 in_color;"
"void main()"
"{" 
    "color = in_color;"
"}\0";
// -------------------------------------------------------------
static const GLchar* texture_shader_vert =  
"layout(location = 0) in vec4 vertexPosition_modelspace;"
"layout(location = 1) in vec2 vertexUV;" 
"out vec2 UV;" 
"uniform vec2 scale;"
"uniform vec2 offset;"
"void main(){"
    "vec4 vertex = vertexPosition_modelspace;"
    "vertex.x *= scale.x;"
    "vertex.y *= scale.y;" 
    "vec4 totalOffset = vec4(offset.x, offset.y, 0.0, 0.0);"
    "gl_Position = (gl_ModelViewProjectionMatrix * vertex) + totalOffset;" 
    "UV = vertexUV;"
"}\0"; 

static const GLchar* texture_shader_frag = 
"#version 330 core\n" 
"in vec2 UV;" 
"out vec4 color;" 
"uniform sampler2D myTextureSampler;"
"void main(){" 
    "color = texture( myTextureSampler, UV ).rgba;"
"}\0";
// -----------------------------------------------------------------------------

// Color wheel drawing ---------------------------------------------------------
static const GLchar* colorwheel_SMOOTH_vert = 
"layout (location = 0) in vec4 position;"
"layout (location = 1) in vec4 color;"
"uniform vec2 offset;"
"uniform vec2 scale;"
"uniform mat4 rotation;"
"smooth out vec4 theColor;"
"void main()"
"{"
    "vec4 vertex = gl_Vertex;" 
    "vertex.x *= scale.x;"
    "vertex.y *= scale.y;"
    "vertex *= rotation;"
    "vec4 totalOffset = vec4(offset.x, offset.y, 0.0, 0.0);"
    "gl_Position = (gl_ModelViewProjectionMatrix * vertex) + totalOffset;"

    // TODO: create color from position on circle
    "theColor = color;" 
"}\0";

static const GLchar* colorwheel_SMOOTH_frag = 
"smooth in vec4 theColor;"
"out vec4 outputColor;"
"void main()"
"{"
    "outputColor = theColor;"
"}\0";
// ----------------------------------------------------------------------------- 0-

// RGB Spectrum Circle
static const GLchar* colorwheel_FLAT_vert = 
"layout (location = 0) in vec4 position;"
"layout (location = 1) in vec4 color;"
"uniform vec2 offset;"
"uniform vec2 scale;"
"uniform mat4 rotation;"
"flat out vec4 theColor;"
"void main()"
"{"
    "vec4 vertex = gl_Vertex;" 
    "vertex.x *= scale.x;"
    "vertex.y *= scale.y;"
    "vertex *= rotation;"
    "vec4 totalOffset = vec4(offset.x, offset.y, 0.0, 0.0);"
    "gl_Position = (gl_ModelViewProjectionMatrix * vertex) + totalOffset;"
    "theColor = color;" 
"}\0";

static const GLchar* colorwheel_FLAT_frag = 
"flat in vec4 theColor;"
"out vec4 outputColor;"
"void main()"
"{"
    "outputColor = theColor;"
"}\0";