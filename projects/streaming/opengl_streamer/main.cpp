#include <GL/glew.h>

#include "decoder.hpp"
#include "encoder.hpp"

#include "tools/sdl/sdl_animation.hpp"
#include "tools/sdl/sdl_system.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SDL2/SDL.h>

#include <boost/program_options.hpp>

#include <iostream>
#include <memory>

namespace {
constexpr auto CHANNELS_NUM = std::size_t{4u};
} // namespace

struct ProgramSetup {
  bool exit{};

  int           width{};
  int           height{};
  std::uint16_t fps{};
  unsigned int  seconds{};
};

ProgramSetup process_args(const int argc, const char *const argv[]) {
  boost::program_options::options_description desc("Options");
  desc.add_options()("help", "This help message");
  desc.add_options()("width", boost::program_options::value<int>()->default_value(512), "Width of the frame buffer");
  desc.add_options()("height", boost::program_options::value<int>()->default_value(384), "Height of the frame buffer");
  desc.add_options()("fps",
                     boost::program_options::value<std::uint16_t>()->default_value(30u),
                     "Number of frames per second");
  desc.add_options()("seconds",
                     boost::program_options::value<unsigned int>()->default_value(3u),
                     "Length of the stream in seconds");

  boost::program_options::variables_map vm;
  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
  boost::program_options::notify(vm);

  if (vm.count("help")) {
    desc.print(std::cout);
    return {true};
  }

  return {false,
          vm["width"].as<int>(),
          vm["height"].as<int>(),
          vm["fps"].as<std::uint16_t>(),
          vm["seconds"].as<unsigned int>()};
}

void encode(const ProgramSetup &program_setup) {
  static GLboolean autoRotate = GL_TRUE;

  static float reqAngleX = 0.0;
  static float reqAngleY = 0.0;
  static float reqAngleZ = 0.0;

  static float currentAngleX = 45.0;
  static float currentAngleY = 45.0;
  static float currentAngleZ = 45.0;

  static float autodelay = 1.0;

  static const GLfloat vertexData[] = {
      -1, -1, -1, 1, +1, -1, -1, 1, +1, +1, -1, 1, -1, +1, -1, 1,
      -1, -1, +1, 1, -1, +1, +1, 1, +1, +1, +1, 1, +1, -1, +1, 1,
  };

  static GLuint elements[] = {
      0, 1, 2, 2, 3, 0, 0, 3, 4, 4, 3, 5, 5, 6, 4, 6, 7, 4, 4, 7, 0, 0, 7, 1, 1, 7, 6, 6, 2, 1, 3, 2, 6, 6, 5, 3,
  };

  static const GLfloat colorData[] = {
      1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1,
  };

  const auto wnd_title = "OpenGL Encoder";

  auto sdl_sys = tools::sdl::SDLSystem{SDL_INIT_EVERYTHING,
                                       wnd_title,
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       program_setup.width,
                                       program_setup.height,
                                       SDL_WINDOW_OPENGL,
                                       -1,
                                       SDL_RENDERER_SOFTWARE};

  glewInit();

  GLuint    elementsBuf;
  GLuint    vertexBuf;
  GLuint    colorBuffer;
  GLuint    program;
  int       width  = program_setup.width;
  int       height = program_setup.height;
  glm::mat4 viewPortMat;
  glm::mat4 Projection;
  glm::mat4 View;
  glm::mat4 identityMat;
  glm::mat4 cameraRotMat;
  glm::vec3 cameraPos;
  GLuint    viewPortLoc;
  GLuint    cameraRotLoc;

  glEnable(GL_DEPTH_TEST);
  glFrontFace(GL_CW);
  glEnable(GL_CULL_FACE);
  glClearColor(0, 0, 0, 0);
  glViewport(0, 0, width, height);

  Projection   = glm::perspective(glm::radians(60.0f), (float)width / (float)height, 1.0f, 1024.0f);
  identityMat  = glm::mat4(1.0f);
  cameraRotMat = identityMat;
  cameraPos    = glm::vec3(4, 4, 0);
  View         = glm::lookAt(cameraPos, glm::vec3(0, 0, 0), glm::vec3(0, 0, 1));
  viewPortMat  = Projection * View;

  program = glCreateProgram();

  glGenBuffers(1, &elementsBuf);
  glGenBuffers(1, &vertexBuf);
  glGenBuffers(1, &colorBuffer);

  // Camera matrix
  View = glm::lookAt(cameraPos,          // camera location
                     glm::vec3(0, 0, 0), // location of origin
                     glm::vec3(0, 0, 1)  // up vector
  );

  viewPortMat = Projection * View;

  GLuint VertexShaderID   = glCreateShader(GL_VERTEX_SHADER);
  GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

  std::string VertexShaderCode;
  VertexShaderCode = R"(
											//fragmentColor=gl_Position*0.5+0.5;
											attribute vec4 vertexBuf;
											uniform mat4 viewPort;
											uniform mat4 cameraRot;
											attribute vec4 colorBuffer;
											varying vec4 fragmentColor;
											void main(){
												gl_Position=viewPort*cameraRot*vertexBuf;
												//gl_Position=cameraRot*vertexBuf;
												fragmentColor=colorBuffer;
											}
)";

  std::string FragmentShaderCode;
  /*
  char cwd[256];
  getcwd(cwd, sizeof(cwd));
  printf("Current working dir: %s\n", cwd);
  DIR *dirp = opendir(".");
  dirent *dp;
  while ((dp = readdir(dirp)) != NULL)
  printf("%s\n",dp->d_name);
  closedir(dirp);
  */
  FragmentShaderCode = R"(
											#ifdef GL_ES
											precision mediump float;
											#endif
											varying vec4 fragmentColor;
											void main(){
												//gl_FragColor=vec4(0,0,1,1);
												gl_FragColor=fragmentColor;
											}
)";

  GLint Result = GL_FALSE;
  int   InfoLogLength;

  printf("Compiling vertex shader\n");
  const char *VertexSourcePointer = VertexShaderCode.c_str();
  glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
  glCompileShader(VertexShaderID);

  // Check Vertex Shader
  glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if (InfoLogLength > 0) {
    std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
    printf("%s\n", &VertexShaderErrorMessage[0]);
  }

  printf("Compiling fragment shader\n");
  const char *FragmentSourcePointer = FragmentShaderCode.c_str();
  glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
  glCompileShader(FragmentShaderID);

  // Check Fragment Shader
  glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if (InfoLogLength > 0) {
    std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
    printf("%s\n", &FragmentShaderErrorMessage[0]);
  }

  // Link the program
  printf("Linking program\n");
  glAttachShader(program, VertexShaderID);
  glAttachShader(program, FragmentShaderID);
  glLinkProgram(program);

  // Check the program
  glGetProgramiv(program, GL_LINK_STATUS, &Result);
  glGetProgramiv(program, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if (InfoLogLength > 0) {
    std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
    glGetProgramInfoLog(program, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    printf("%s\n", &ProgramErrorMessage[0]);
  }

  glDetachShader(program, VertexShaderID);
  glDetachShader(program, FragmentShaderID);

  glDeleteShader(VertexShaderID);
  glDeleteShader(FragmentShaderID);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsBuf);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

  glEnableVertexAttribArray(glGetAttribLocation(program, "vertexBuf"));

  glBindBuffer(GL_ARRAY_BUFFER, vertexBuf);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
  glVertexAttribPointer(glGetAttribLocation(program, "vertexBuf"),
                        4,        // coordinates per vertex
                        GL_FLOAT, // type
                        GL_FALSE, // normalized?
                        0,        // stride
                        0         // buffer offset
  );

  glEnableVertexAttribArray(glGetAttribLocation(program, "colorBuffer"));

  glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
  // glBindAttribLocation(program,1,"colorBuffer");
  glBufferData(GL_ARRAY_BUFFER, sizeof(colorData), colorData, GL_STATIC_DRAW);
  glVertexAttribPointer(glGetAttribLocation(program, "colorBuffer"), 4, GL_FLOAT, GL_FALSE, 0, 0);

  viewPortLoc  = glGetUniformLocation(program, "viewPort");
  cameraRotLoc = glGetUniformLocation(program, "cameraRot");

  auto last_timestamp_ms = SDL_GetTicks();
  auto animation         = tools::sdl::SDLAnimation(program_setup.fps);
  auto quit              = false;

  glReadBuffer(GL_BACK);

  auto gl_frame = std::make_shared<std::vector<GLubyte>>(program_setup.width * program_setup.height * 4);
  auto encoder =
      streaming::Encoder(program_setup.width, program_setup.height, gl_frame, program_setup.fps, AV_CODEC_ID_H264);
  unsigned int frames = program_setup.seconds * program_setup.fps + 1;
  while (!quit) {
    SDL_Event event;
    while (!quit && SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT: quit = true; break;
      case SDL_USEREVENT:
        SDL_PumpEvents();
        {
          const auto time_elapsed_ms = event.user.timestamp - last_timestamp_ms;

          // renderer->animate(time_elapsed_ms);

          last_timestamp_ms = event.user.timestamp;
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(program);

        if (reqAngleX != currentAngleX) {
          cameraRotMat  = glm::rotate(cameraRotMat, glm::radians(reqAngleX - currentAngleX), glm::vec3(0, 1, 0));
          currentAngleX = reqAngleX;
        }
        if (reqAngleY != currentAngleY) {
          cameraRotMat  = glm::rotate(cameraRotMat, glm::radians(reqAngleY - currentAngleY), glm::vec3(0, 1, 0));
          currentAngleY = reqAngleY;
        }
        if (reqAngleZ != currentAngleZ) {
          cameraRotMat  = glm::rotate(cameraRotMat, glm::radians(reqAngleZ - currentAngleZ), glm::vec3(0, 0, 1));
          currentAngleZ = reqAngleZ;
        }
        if (autoRotate) { cameraRotMat = glm::rotate(cameraRotMat, glm::radians(autodelay), glm::vec3(1, 0, 0)); }
        glUniformMatrix4fv(cameraRotLoc, 1, GL_FALSE, glm::value_ptr(cameraRotMat));

        glUniformMatrix4fv(viewPortLoc, 1, GL_FALSE, glm::value_ptr(viewPortMat));

        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glFlush();
        glReadPixels(0, 0, program_setup.width, program_setup.height, GL_RGBA, GL_UNSIGNED_BYTE, gl_frame->data());
        encoder.encode_frame();
        SDL_GL_SwapWindow(sdl_sys.wnd());
        frames--;
        if (frames == 0) { quit = true; }
        break;
      default: break;
      }
    }
  }
}

void decode(const ProgramSetup &program_setup) {
  const auto wnd_title = "Video Decoder";

  auto sdl_sys = tools::sdl::SDLSystem{SDL_INIT_EVERYTHING,
                                       wnd_title,
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       program_setup.width,
                                       program_setup.height,
                                       0,
                                       -1,
                                       SDL_RENDERER_SOFTWARE};

  auto screen = SDL_GetWindowSurface(sdl_sys.wnd());

  auto last_timestamp_ms = SDL_GetTicks();
  auto animation         = tools::sdl::SDLAnimation(program_setup.fps);
  auto quit              = false;

  auto       decoder   = streaming::Decoder(program_setup.width, program_setup.height, AV_CODEC_ID_H264);
  auto       rgb_frame = decoder.rgb_frame();
  const auto pitch     = CHANNELS_NUM * program_setup.width;

  auto         frame_surface = SDL_CreateRGBSurfaceFrom(rgb_frame->data(),
                                                program_setup.width,
                                                program_setup.height,
                                                8 * CHANNELS_NUM,
                                                pitch,
                                                0x000000FF,
                                                0x0000FF00,
                                                0x00FF0000,
                                                0x00000000);
  unsigned int frames        = program_setup.seconds * program_setup.fps + 1;
  while (!quit) {
    SDL_Event event;
    while (!quit && SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT: quit = true; break;
      case SDL_USEREVENT:
        SDL_PumpEvents();
        if (decoder.prepare_frame()) {
          SDL_BlitSurface(frame_surface, NULL, screen, NULL);
          SDL_UpdateWindowSurface(sdl_sys.wnd());
          frames--;
          if (frames == 0) { quit = true; }
        }
        break;
      default: break;
      }
    }
  }
  SDL_FreeSurface(frame_surface);
}

int main(int argc, char *argv[]) {
  const auto program_setup = process_args(argc, argv);
  if (program_setup.exit) { return 1; }

  encode(program_setup);
  decode(program_setup);

  return 0;
}
