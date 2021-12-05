#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <chrono>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <thread>

const char *vertexShaderSrc = R"(
#version 450

layout (location = 0) in vec2 aPos;

void main() {
    gl_Position = vec4(aPos, 1, 1);
}

)";

const char *fragmentShaderSrc = R"(
#version 450

layout (location = 0) out vec4 outColor;

void main() {
    outColor = vec4(1, 0, 0, 1);
}

)";

void message_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
                      GLsizei length, GLchar const *message,
                      void const *user_param) {
  if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
    return;

  auto const src_str = [source]() {
    switch (source) {
    case GL_DEBUG_SOURCE_API:
      return "API";
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      return "WINDOW SYSTEM";
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      return "SHADER COMPILER";
    case GL_DEBUG_SOURCE_THIRD_PARTY:
      return "THIRD PARTY";
    case GL_DEBUG_SOURCE_APPLICATION:
      return "APPLICATION";
    case GL_DEBUG_SOURCE_OTHER:
      return "OTHER";
    default:
      return "UNKNOWN SOURCE";
    }
  }();

  auto const type_str = [type]() {
    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
      return "ERROR";
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      return "DEPRECATED_BEHAVIOR";
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      return "UNDEFINED_BEHAVIOR";
    case GL_DEBUG_TYPE_PORTABILITY:
      return "PORTABILITY";
    case GL_DEBUG_TYPE_PERFORMANCE:
      return "PERFORMANCE";
    case GL_DEBUG_TYPE_MARKER:
      return "MARKER";
    case GL_DEBUG_TYPE_OTHER:
      return "OTHER";
    default:
      return "UNKNOWN TYPE";
    }
  }();

  auto const severity_str = [severity]() {
    switch (severity) {
    case GL_DEBUG_SEVERITY_NOTIFICATION:
      return "NOTIFICATION";
    case GL_DEBUG_SEVERITY_LOW:
      return "LOW";
    case GL_DEBUG_SEVERITY_MEDIUM:
      return "MEDIUM";
    case GL_DEBUG_SEVERITY_HIGH:
      return "HIGH";
    default:
      return "UNKNOWN SEVERITY";
    }
  }();
  std::cout << src_str << ", " << type_str << ", " << severity_str << ", " << id
            << ": " << message << '\n';
}

int main(int argc, char *argv[]) {

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  auto window = glfwCreateWindow(800, 600, "Example", nullptr, nullptr);
  if (!window)
    throw std::runtime_error("Error creating glfw window");
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  if (!gladLoaderLoadGL())
    throw std::runtime_error("Error initializing glad");

  /**
   * Initialize ImGui
   */
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 450 core");
  ImGui::StyleColorsClassic();

  glEnable(GL_CULL_FACE);
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(message_callback, nullptr);

  /**
   * Compile shader
   */
  int success;
  char infoLog[512];
  auto vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSrc, 0);
  glCompileShader(vertexShader);

  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
    std::cerr << "Vertex shader compilation failed:" << std::endl;
    std::cerr << infoLog << std::endl;
    return 0;
  }

  auto fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSrc, 0);
  glCompileShader(fragmentShader);

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
    std::cerr << "Fragment shader compilation failed:" << std::endl;
    std::cerr << infoLog << std::endl;
    return 0;
  }

  auto program = glCreateProgram();
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);
  glLinkProgram(program);

  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(program, 512, nullptr, infoLog);
    std::cerr << "Shader linking failed:" << std::endl;
    std::cerr << infoLog << std::endl;
    return 0;
  }

  glDetachShader(program, vertexShader);
  glDetachShader(program, fragmentShader);

  /**
   * Create vertex array and buffers
   */
  GLuint vao;
  glCreateVertexArrays(1, &vao);

  glEnableVertexArrayAttrib(vao, 0);
  glVertexArrayAttribFormat(vao, 0, 2, GL_FLOAT, GL_FALSE,
                            offsetof(glm::vec2, x));

  glVertexArrayAttribBinding(vao, 0, 0);

  glm::vec2 vertices[] = {{-0.2, -0.2}, {-0.2, 0.2}, {0.2, 0.2}, {0.2, -0.2}};

  GLuint vbo;
  glCreateBuffers(1, &vbo);
  glNamedBufferStorage(vbo, sizeof(glm::vec2) * 4, vertices,
                       GL_DYNAMIC_STORAGE_BIT);

  std::uint32_t indices[] = {0, 2, 1, 2, 0, 3};

  GLuint ibo;
  glCreateBuffers(1, &ibo);
  glNamedBufferStorage(ibo, sizeof(std::uint32_t) * 6, indices,
                       GL_DYNAMIC_STORAGE_BIT);

  glBindVertexArray(vao);
  glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(glm::vec2));
  glVertexArrayElementBuffer(vao, ibo);
  glUseProgram(program);
  glClearColor(1, 1, 1, 1);

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    static bool showDemo = false;
    ImGui::Begin("Example");
    if (ImGui::Button("Show/Hide ImGui demo"))
      showDemo = !showDemo;
    ImGui::End();
    if (showDemo)
      ImGui::ShowDemoWindow(&showDemo);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
    glClear(GL_COLOR_BUFFER_BIT);

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}