#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

// This will be the source for the vertex shader. For now, it'll just be
// stored in a C string so we don't have to compile it and OpenGL will just
// dynamically compile it at runtime
const char *vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

// This will be the source for the fragment shader. For now, it'll just be
// stored in a C string so we don't have to compile it and OpenGL will just
// dynamically compile it at runtime.
const char *fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0";

unsigned int shaderProgram;
unsigned int VAO;

void renderTriangleSetup() {
    /// ~~~ Vertex Input ~~~ 
    //
    // First, we need to give OpenGL some vertex data. OpenGL is a 3D graphics
    // library, so all coordinates are in 3 dimensions. OpenGL only processes
    // 3D coordinates in the range of -1.0 to 1.0 on all 3 axes (x,y, and z).
    // This is called the "normalized device coordinates" which will be the
    // range of coordinates that can appear on your screen.
    
    // this float array specifies a total of three vertices with each vertex
    // having a 3D position. It's basically just three sets of coordinates.
    // They are defined using the normalized device coordinates:
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    // After creating vertex data, we need to send it as input to the first
    // process of the graphics pipeline, the vertex shader. This is done by
    // creating memory on the GPU where we store the vertex data.

    // This memory is created using Vertex Buffer Objects (VBO) which can store
    // a large number of vertices in the GPU's memory. This allows us to send
    // large quantities of data to the GPU all at once. Sending data to the GPU
    // is relatively slow, but once it's on the GPU's memory, accessing it is
    // extremely fast, so we want to send as much data as we can all at once.

    // A vertex buffer object is an OpenGL object which has a unique ID
    // corresponding to that buffer, so we can generate one with a buffer ID
    // using glGenBuffers
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    // OpenGL has many types of buffer objects and the buffer type of a vertex
    // buffer object is GL_ARRAY_BUFFER. OpenGL allows us to bind to several
    // buffers at once as long as they have different types. We can bind the
    // newly created buffer to GL_ARRAY_BUFFER target with glBindBuffer:
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // from now on, any buffer calls we make will be used to configure the
    // currently bound buffer, which is VBO. Then, we can make a call to the
    // glBufferData function to copy the vertex data into the buffer's memory.
    // For the 4th argument, the options are GL_STREAM_DRAW (the data is set
    // only once and used by the GPU only a few times), GL_STATIC_DRAW (the data
    // is sent by the GPU and used many times), and GL_DYNAMIC_DRAW (the data is
    // changed a lot, and is used many times.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // This will create a shader object, which will be referenced by a numeric ID
    // just like the VBO. The vertex shader's id will be stored as an unsigned
    // int, and the shader can be created with glCreateShader. Since we are
    // making a vertex shader, GL_VERTEX_SHADER is used.
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // this sets the source for the vertexShader (which we defined in a C string)
    // and compiles it. The second argument specifies how many strings, the third
    // argument is the actual source code, and the fourth can just be left NULL.
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // This is how we can test if the compilation was successful. First, the
    // success variable will store whether or not it was actually successful. If
    // compilation was not successful, we can retreive an error message, which
    // will be stored in infoLog. glGetShaderiv will get the success variable.
    int success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    
    // if the compilation was unsuccessful
    if(!success) {
        // get error message and store it in infoLog, then print an error message
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog 
                  << std::endl;
    }

    /// ~~~ Fragment Shader ~~~
    //
    // The fragment shader is the final shader we need to create for rendering
    // a triangle. It's all about calculating the color output of the pixels. To
    // keep things simple, this fragment shader will just output an orange-ish
    // color.

    // same process as with the vertex shader...
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
 
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        // get error message and store it in infoLog, then print error message
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog
                  << std::endl;
    }

    /// ~~~ Shader Program ~~~
    //
    // A shader program object is the final linked version of multiple shaders.
    // We need to link the recently compiled shaders to a shader program object,
    // which will be activated when rendering objects.
    //
    // When linking the shaders into a program, it links the outputs of each
    // shader to the inputs of the next shader. This is also where you'll get
    // linking errors if your outputs and inputs do not match.

    // create a program and return the ID to reference it with.
    //unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    // attach previously compiled shaders to the program
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    // handle errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success){
        // get error message and store it in infolog, then print
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog
                  << std::endl;
    }

    // The result will be a shader program we can activate by calling the
    // function glUseProgram. Everything after that will now use this shader
    // program (and thus the shaders).
    glUseProgram(shaderProgram);

    // Finally, delete shader objects after they're linked. Don't need them
    // anymore:
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    /// ~~~ Linking Vertex Attributes ~~~
    //
    // The vertex shader allows us to specify any input in the form of vertex
    // attributes, which allows for great flexibility, but also means we have
    // to manually specify what part of our input data goes to which vertex
    // attribute in the vertex shader. This means we have to specify how OpenGL
    // should interpret the vertex data before rendering.

    // The function glVertexAttribPointer allows us to tell OpenGL how it should
    // interpret the vertex data. It has 6 parameters:
    //
    // Param 1: which vertex attribute to configure. Remember that the location
    // of the position vertex attribute was specified with `layout (location = 0)`
    // This sets the location of the vertex attribute to 0;
    //
    // Param 2: Size of each vertex attribute. It's a vec3 so it's comprised of
    // 3 values.
    //
    // Param 3: GL_FLOAT, the type of data being passed
    //
    // Param 4: If we want the data to be normalized (used when using integers)
    //
    // Param 5: `stride` which tells us consecutive distance between the vertex
    // attributes in memory. Remember that the 3 vertices are stored in a float
    // array with length 9; 0, 1, 2 are vertex 1. 3, 4, 5 are vertex 2. 6, 7, 8
    // are vertex 3.
    //
    // Param 6: This is the offset of where the position data begins in the buffer.
    // since the position data begins at just 0, can just pass it 0.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //finally, generate a VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);
}


void renderTriangle() {
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}




















