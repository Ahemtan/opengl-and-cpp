#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>  // for sin

void errorCallBack(int error, const char* description)
{
    std::cerr << "GLFW error " << error << ": " << description << std::endl;
}

bool loadShader(GLuint& program)
{
    bool loadSuccess = true;
    char infoLog[512];
    GLint success;

    std::string temp, src;
    std::ifstream in_file;

    // === Load Vertex Shader ===
    src = "";
    std::cout << "[INFO] Loading vertex shader..." << std::endl;
    in_file.open("./shaders/VertextShader.glsl");
    if (in_file.is_open())
    {
        while (std::getline(in_file, temp)) {
            src += temp + "\n";
        }
        in_file.close();
    }
    else
    {
        std::cerr << "Failed to open vertex shader file" << std::endl;
        return false;
    }

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const GLchar* vectSrc = src.c_str();
    glShaderSource(vertexShader, 1, &vectSrc, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        loadSuccess = false;
    }

    // === Load Fragment Shader ===
    src = "";
    std::cout << "[INFO] Loading fragment shader..." << std::endl;
    in_file.open("./shaders/FragmentShader.glsl");
    if (in_file.is_open())
    {
        while (std::getline(in_file, temp)) {
            src += temp + "\n";
        }
        in_file.close();
    }
    else
    {
        std::cerr << "Failed to open fragment shader file" << std::endl;
        return false;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const GLchar* fragSrc = src.c_str();
    glShaderSource(fragmentShader, 1, &fragSrc, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        loadSuccess = false;
    }

    // === Link Program ===
    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        loadSuccess = false;
    }

    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);

    return loadSuccess;
}

int main()
{

    glfwSetErrorCallback(errorCallBack);
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For Mac

    GLFWwindow* window = glfwCreateWindow(800, 600, "Virtuniverse", NULL, NULL);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        std::cerr << "GLEW initialization failed: " << glewGetErrorString(err) << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // === Load Shaders ===
    GLuint coreProgram;
    if (!loadShader(coreProgram))
    {
        std::cerr << "Shader loading failed. Exiting." << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // === Render Loop ===
    while (!glfwWindowShouldClose(window))
    {
        float time = (float)glfwGetTime();

        float red = (std::sin(time * 0.5f) + 1.0f) / 2.0f;
        float green = (std::sin(time * 0.3f) + 1.0f) / 2.0f;
        float blue = (std::sin(time * 0.7f) + 1.0f) / 2.0f;

        glClearColor(red, green, blue, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
