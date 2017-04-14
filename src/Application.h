#ifndef APPLICATION_H
#define APPLICATION_H

#include <glad/glad.h>

#define USE_SDL

#ifndef USE_SDL
#include <GLFW/glfw3.h>
#else 
#include <SDL.h>
#endif 

#ifndef USE_SDL
#define APP_PRESS GLFW_PRESS
#define APP_KEY_ESCAPE GLFW_KEY_ESCAPE
#define WINDOW GLFWwindow
#else 
#define APP_PRESS 1
#define WINDOW SDL_Window
#endif 

#ifdef GLAD_DEBUG
//logs every gl call to the console
static void pre_gl_call(const char *name, void *funcptr, int len_args, ...) {
     //printf("Calling: %s (%d arguments)\n", name, len_args);
}

static void post_gl_call(const char *name, void *funcptr, int len_args, ...) {
    GLenum error_code;
    error_code = glad_glGetError();

    if (error_code != GL_NO_ERROR) {
        fprintf(stderr, "ERROR %d in %s\n", error_code, name);
        exit(1);
    }
}
#endif

class Application {
    WINDOW *window;
#ifdef USE_SDL
    SDL_GLContext context;
#endif 
public:
    static void error_callback(int error, const char* description) {
        fprintf(stderr, "Error: %s\n", description);
    }

    static void focus_callback(WINDOW *window, int focus) {
#ifndef USE_SDL
        switch (focus) {
        case GLFW_FALSE:
            cout << "!!! LOSING FOCUS !!!" << endl;
            break;
        case GLFW_TRUE:
            cout << "!!! GAINING FOCUS !!!" << endl;
            break;
        }
#endif 
    }

    void swapBuffers() {
#ifndef USE_SDL
        glfwSwapBuffers(window);
#else 
        SDL_GL_SwapWindow(window);
#endif 
    }

    WINDOW *createWindow() {
#ifndef USE_SDL
        glfwSetErrorCallback(Application::error_callback);
        if (!glfwInit())
#else 
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
#endif 
            exit(EXIT_FAILURE);

#ifndef USE_SDL
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_FOCUSED, 1);

        window = glfwCreateWindow(1024, 768, "Simple example", NULL, NULL);
#else 
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
            
        window = SDL_CreateWindow("SpellCaster",
                                  SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED,
                                  1024,
                                  768,
                                  SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
                                  //SDL_WINDOW_OPENGL|SDL_WINDOW_FULLSCREEN_DESKTOP|SDL_WINDOW_RESIZABLE);
#endif 
        if (!window) {
#ifndef USE_SDL
            glfwTerminate();
#else 
            cout << "Couldn't get window" << endl;
#endif 
            exit(EXIT_FAILURE);
        }

        return window;
    }
    static void mouse_callback(WINDOW* window, int button, int action, int mode) {
#ifndef USE_SDL
        Application *application = (Application *)glfwGetWindowUserPointer(window);
#else 
        Application *application = (Application *)SDL_GetWindowData(window, "app");
#endif 
        application->mouse(button, action, mode);
    }
    static void key_callback(WINDOW *window, int key, int scancode, int action, int mods) {
#ifndef USE_SDL
        Application *application = (Application *)glfwGetWindowUserPointer(window);
#else 
        Application *application = (Application *)SDL_GetWindowData(window, "app");
#endif 
        application->key(key, scancode, action, mods);
    }
    void install() {
#ifndef USE_SDL
        glfwSetWindowUserPointer(window, this);
        glfwSetMouseButtonCallback(window, Application::mouse_callback);
        glfwSetKeyCallback(window, Application::key_callback);
        glfwSetWindowFocusCallback(window, Application::focus_callback);
#else 
        SDL_SetWindowData(window, "app", this);
#endif 
    }
    virtual void mouse(int button, int action, int mode) = 0;
    virtual void motion() = 0;
    virtual void key(int key, int scancode, int action, int mods) = 0;
    virtual void idle() = 0;
    virtual void draw() = 0;

    Point getMousePixels() {
#ifdef USE_SDL
        int xpos, ypos;
        SDL_GetMouseState(&xpos, &ypos);
        return Point {float(xpos), float(ypos)};
#else
        double xpos, ypos;
        int buf_width, buf_height, win_width, win_height;
        glfwGetFramebufferSize(window, &buf_width, &buf_height);
        glfwGetWindowSize(window, &win_width, &win_height);
        glfwGetCursorPos(window, &xpos, &ypos);
        return Point{float(xpos*buf_width/win_width), float(ypos*buf_height/win_height)};
#endif 
    }

    void start() {
#ifndef USE_SDL
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            idle();
            draw();
        }
#else 
        SDL_Event event;
        while (true) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    exit(1);
                } else if (event.type == SDL_MOUSEMOTION) {
                    motion();
                } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                    mouse(0, APP_PRESS, 0);
                }
            }
            idle();
            draw();
        }
#endif 
    }

    void closeWindow() {
#ifndef USE_SDL
        glfwSetWindowShouldClose(window, GL_TRUE);
#endif 
    }

    void getFrameBufferSize(int &width, int &height) {
#ifndef USE_SDL
        glfwGetFramebufferSize(window, &width, &height);
#else 
        //glfwGetFramebufferSize(window, &width, &height);
        SDL_GetWindowSize(window, &width, &height);
#endif 
    }

    void destroyWindow() {
#ifndef USE_SDL
        glfwDestroyWindow(window);
#endif 
    }

    void terminate() {
#ifndef USE_SDL
        glfwTerminate();
#endif 
    }

    void initOpenGL() {
#ifndef USE_SDL
        glfwMakeContextCurrent(window);
        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        glfwSwapInterval(1);
        GLFWcursor* cursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
        glfwSetCursor(window, cursor);
#else 
        context = SDL_GL_CreateContext(window);
        if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
            cout << "Failed to initialise OpenGL context" << endl;
            cout << GLVersion.major << ' ' << GLVersion.minor << endl;
            cout << GLAD_GL_VERSION_3_1 << endl;
            exit(1);
        }
#ifdef GLAD_DEBUG
        // before every opengl call call pre_gl_call
        glad_set_pre_callback(pre_gl_call);
        glad_set_post_callback(post_gl_call);
        
        // post callback checks for glGetError by default
        
        // don't use the callback for glClear
        // (glClear could be replaced with your own function)
        glad_debug_glClear = glad_glClear;
#endif
        cout << glGetString(GL_VERSION) << endl;;
        cout << GLVersion.major << ' ' << GLVersion.minor << endl;
        SDL_GL_SetSwapInterval(1);
        SDL_Cursor* cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
        SDL_SetCursor(cursor);
#endif 
    }
};

#endif
