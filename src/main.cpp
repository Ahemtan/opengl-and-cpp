#include "include/main.h"
#include <cstddef>
#include <gl/gl.h>
#include <glm/ext/vector_float2.hpp>

Vertex verticies[] = 
{
    glm::vec3(0.0f, 0.5f, 0.0f),    glm::vec3(1.0f, 0.0f, 0.0f),    glm::vec2(0.0f, 0.0f),
    glm::vec3(-0.5f, -0.5f, 0.0f),  glm::vec3(0.0f, 1.0f, 0.0f),    glm::vec2(0.0f, 0.0f),
    glm::vec3(0.5f, -0.5f, 0.0f),   glm::vec3(0.0f, 0.0f, 1.0f),    glm::vec2(0.0f, 0.0f)
};

unsigned nrOfVertices = sizeof(verticies) / sizeof(Vertex);

GLuint indices[] =
{
    0, 1, 2
};

void errorCallBack(int error, const char *description) {
  std::cerr << "GLFW error " << error << ": " << description << std::endl;
}

void updateInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

bool loadShader(GLuint &program) {
  bool loadSuccess = true;
  char infoLog[512];
  GLint success;

  std::string temp, src;
  std::ifstream in_file;

  // === Load Vertex Shader ===
  src = "";
  std::cout << "[INFO] Loading vertex shader..." << std::endl;

  in_file.open("./shaders/VertextShader.glsl");
  if (in_file.is_open()) {
    while (std::getline(in_file, temp)) {
      src += temp + "\n";
    }
    in_file.close();
  } else {
    std::cerr << "Failed to open vertex shader file" << std::endl;
    return false;
  }

  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  const GLchar *vectSrc = src.c_str();
  glShaderSource(vertexShader, 1, &vectSrc, NULL);
  glCompileShader(vertexShader);

  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
              << infoLog << std::endl;
    loadSuccess = false;
  }

  // === Load Fragment Shader ===
  src = "";
  std::cout << "[INFO] Loading fragment shader..." << std::endl;
  in_file.open("./shaders/FragmentShader.glsl");
  if (in_file.is_open()) {
    while (std::getline(in_file, temp)) {
      src += temp + "\n";
    }
    in_file.close();
  } else {
    std::cerr << "Failed to open fragment shader file" << std::endl;
    return false;
  }

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  const GLchar *fragSrc = src.c_str();
  glShaderSource(fragmentShader, 1, &fragSrc, NULL);
  glCompileShader(fragmentShader);

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
              << infoLog << std::endl;
    loadSuccess = false;
  }

  // === Link Program ===
  program = glCreateProgram();
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);
  glLinkProgram(program);

  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program, 512, NULL, infoLog);
    std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
              << infoLog << std::endl;
    loadSuccess = false;
  }

  glDeleteShader(fragmentShader);
  glDeleteShader(vertexShader);

  return loadSuccess;
}

int main() {

  glfwSetErrorCallback(errorCallBack);
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For Mac

  GLFWwindow *window = glfwCreateWindow(800, 600, "Virtuniverse", NULL, NULL);
  if (!window) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  glewExperimental = GL_TRUE;

  GLenum err = glewInit();

  if (err != GLEW_OK) {
    std::cerr << "GLEW initialization failed: " << glewGetErrorString(err)
              << std::endl;
    glfwDestroyWindow(window);
    glfwTerminate();
    return -1;
  }

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  // Opengl options

  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  // === Load Shaders ===
  GLuint coreProgram;
  if (!loadShader(coreProgram)) {
    std::cerr << "Shader loading failed. Exiting." << std::endl;
    glfwDestroyWindow(window);
    glfwTerminate();
    return -1;
  }

  // VAO VBO VCO

  GLuint VAO;
  glCreateVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  GLuint VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);

  GLuint EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // Position
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, position));
  glEnableVertexAttribArray(0);

  // Color
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, color));
  glEnableVertexAttribArray(1);

  // TerCoord
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, texcoord));
  glEnableVertexAttribArray(2);


  //Binding
  glBindVertexArray(0);

  // === Render Loop ===    
  while (!glfwWindowShouldClose(window)) {
    float time = (float)glfwGetTime();

    updateInput(window);


    // NOTE: for color blend animation

    // float red = (std::sin(time * 0.5f) + 1.0f) / 2.0f;
    // float green = (std::sin(time * 0.3f) + 1.0f) / 2.0f;
    // float blue = (std::sin(time * 0.7f) + 1.0f) / 2.0f;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glUseProgram(coreProgram);
    glBindVertexArray(VAO);

    glDrawElements(GL_TRIANGLES, nrOfVertices, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
