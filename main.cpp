#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <string>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#if defined( _MSC_VER ) && ( _MSC_VER >= 1900 ) && !defined( IMGUI_DISABLE_WIN32_FUNCTIONS )
    #pragma comment( lib, "legacy_stdio_definitions" )
#endif

#ifdef __EMSCRIPTEN__
    #include "vendor/imgui/examples/libs/emscripten/emscripten_mainloop_stub.h"
#endif

constexpr std::string_view g_title_text = "Marco's Portfolio";

static void glfw_error_callback( int error, const char* description ) {
    fprintf( stderr, "GLFW Error %d: %s\n", error, description );
}

auto main( ) -> int {
    glfwSetErrorCallback( glfw_error_callback );
    if ( !glfwInit( ) ) return 1;

    // Select GL version + let the backend select a GLSL version
    const char* glsl_version = nullptr;
#if defined( IMGUI_IMPL_OPENGL_ES2 )
    // GL ES 2.0 + GLSL 100 (WebGL 1.0)
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 2 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 0 );
    glfwWindowHint( GLFW_CLIENT_API, GLFW_OPENGL_ES_API );
#elif defined( IMGUI_IMPL_OPENGL_ES3 )
    // GL ES 3.0 + GLSL 300 es (WebGL 2.0)
#elif defined( __APPLE__ )
    // GL 3.2 + generally GLSL 150
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 2 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE ); // 3.2+ only
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );           // Required on Mac
#else
    // GL 3.0 + generally GLSL 130
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 0 );
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor( glfwGetPrimaryMonitor( ) ); // Valid on GLFW 3.3+ only
    GLFWwindow* window = glfwCreateWindow(
        (int)( 750 * main_scale ), (int)( 800 * main_scale ), g_title_text.data( ), nullptr, nullptr );
    if ( window == nullptr ) return 1;

    glfwMakeContextCurrent( window );
#ifndef __EMSCRIPTEN__
    glfwSwapInterval( 1 ); // Vsync
#endif

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION( );
    ImGui::CreateContext( );
    ImGuiIO& io = ImGui::GetIO( );
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark( );
    // ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle( );
    style.ScaleAllSizes( main_scale ); // Bake a fixed style scale. (until we have a solution for dynamic style scaling,
                                       // changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;   // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true
                                     // automatically overrides this for every window depending on the current monitor)

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL( window, true );
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCallbacks( window, "#canvas" );
#endif
    ImGui_ImplOpenGL3_Init( glsl_version );

    // Our state
    ImVec4 clear_color = ImVec4( 0.0f, 0.0f, 0.0f, 1.00f );

    // Main loop
#ifdef __EMSCRIPTEN__
    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the
    // imgui.ini file. You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = nullptr;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while ( !glfwWindowShouldClose( window ) )
#endif
    {
        glfwPollEvents( );
        if ( glfwGetWindowAttrib( window, GLFW_ICONIFIED ) != 0 ) {
            ImGui_ImplGlfw_Sleep( 10 );
            continue;
        }

        ImGui_ImplOpenGL3_NewFrame( );
        ImGui_ImplGlfw_NewFrame( );
        ImGui::NewFrame( );

        {
            ImGui::Begin( "Hello, world!" );

            ImGui::Text( "Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate );
            ImGui::End( );
        }

        ImGui::Render( );
        int display_w, display_h;
        glfwGetFramebufferSize( window, &display_w, &display_h );
        glViewport( 0, 0, display_w, display_h );
        glClearColor(
            clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w,
            clear_color.w );
        glClear( GL_COLOR_BUFFER_BIT );
        ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData( ) );

        glfwSwapBuffers( window );
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    ImGui_ImplOpenGL3_Shutdown( );
    ImGui_ImplGlfw_Shutdown( );
    ImGui::DestroyContext( );

    glfwDestroyWindow( window );
    glfwTerminate( );

    return 0;
}
