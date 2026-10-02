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
    glfwSwapInterval( 1 ); // Vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION( );
    ImGui::CreateContext( );
    ImGuiIO& io = ImGui::GetIO( );
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

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
    bool show_demo_window = false;
    bool show_another_window = false;
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
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your
        // inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or
        // clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or
        // clear/overwrite your copy of the keyboard data. Generally you may always pass all inputs to dear imgui, and
        // hide them from your application based on those two flags.
        glfwPollEvents( );
        if ( glfwGetWindowAttrib( window, GLFW_ICONIFIED ) != 0 ) {
            ImGui_ImplGlfw_Sleep( 10 );
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame( );
        ImGui_ImplGlfw_NewFrame( );
        ImGui::NewFrame( );

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code
        // to learn more about Dear ImGui!).
        if ( show_demo_window ) ImGui::ShowDemoWindow( &show_demo_window );

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin( "Hello, world!" ); // Create a window called "Hello, world!" and append into it.

            ImGui::Text( "This is some useful text." );          // Display some text (you can use a format strings too)
            ImGui::Checkbox( "Demo Window", &show_demo_window ); // Edit bools storing our window open/close state
            ImGui::Checkbox( "Another Window", &show_another_window );

            ImGui::SliderFloat( "float", &f, 0.0f, 1.0f );            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3( "clear color", (float*)&clear_color ); // Edit 3 floats representing a color

            if ( ImGui::Button(
                     "Button" ) ) // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine( );
            ImGui::Text( "counter = %d", counter );

            ImGui::Text( "Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate );
            ImGui::End( );
        }

        // 3. Show another simple window.
        if ( show_another_window ) {
            ImGui::Begin(
                "Another Window", &show_another_window ); // Pass a pointer to our bool variable (the window will have a
                                                          // closing button that will clear the bool when clicked)
            ImGui::Text( "Hello from another window!" );
            if ( ImGui::Button( "Close Me" ) ) show_another_window = false;
            ImGui::End( );
        }

        // Rendering
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

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown( );
    ImGui_ImplGlfw_Shutdown( );
    ImGui::DestroyContext( );

    glfwDestroyWindow( window );
    glfwTerminate( );

    return 0;
}
