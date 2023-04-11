#include <GL/glew.h>
#include <GLFW/glfw3.h>

int main()
{
    // Initialize GLFW
    if (!glfwInit())
        return -1;

    // Create a window and OpenGL context
    GLFWwindow* window = glfwCreateWindow(800, 600, "Empty Window", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    if (glewInit() != GLEW_OK)
    {
        glfwTerminate();
        return -1;
    }

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glfwTerminate();
    return 0;
}

