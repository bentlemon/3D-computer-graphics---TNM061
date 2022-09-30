/*
 * Framework library functions for the course TNM061
 * Author: Stefan Gustavson 2013-2014 (stefan.gustavson@liu.se)
 * updated 2021 by Martin Falk
 *
 * This code is in the public domain.
 *
 * The function pointer loading for all OpenGL extensions,
 * the code in loadExtensions(), is forced upon us by
 * Microsoft Windows. MacOS and Linux don't need that.
 * A more general project would use GLEW, but we choose to
 * stay clear of that dependence. GLEW is a large library
 * that hides a lot of details from the programmer, and it
 * is instructive to see what it hides - if nothing else to
 * better appreciate GLEW as a tool for OpenGL programming
 * under Windows.
 *
 */

#include <stdio.h>  // For shader files and console messages

// In MacOS X, tell GLFW to include the modern OpenGL headers.
// Windows does not want this, so we make this Mac-only.
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#endif

// In Linux, tell GLFW to include the modern OpenGL functions.
// Windows does not want this, so we make this Linux-only.
#ifdef __linux__
#define GL_GLEXT_PROTOTYPES
#endif

#include <GLFW/glfw3.h>

#include "tnm061.hpp"

/*
 * printError() - Signal an error.
 * Simple printf() to console for portability.
 */
void tnm061::printError(const char* errtype, const char* errmsg) {
    fprintf(stderr, "%s: %s\n", errtype, errmsg);
}

/*
 * displayFPS() - Calculate, display and return frame rate statistics.
 * Called every frame, but statistics are updated only once per second.
 * The time per frame is a better measure of performance than the
 * number of frames per second, so both are displayed.
 *
 * NOTE: This function doesn't work properly if you call it for multiple
 * windows. Call it only for one window, and only once for each frame.
 */
double tnm061::displayFPS(GLFWwindow* window) {

    static double t0 = 0.0;
    static int frames = 0;
    double fps = 0.0;
    double frametime = 0.0;
    static char titlestring[200];

    double t;

    // Get current time
    t = glfwGetTime();  // Gets number of seconds since glfwInit()
    // If one second has passed, or if this is the very first frame
    if ((t - t0) > 1.0 || frames == 0) {
        fps = (double)frames / (t - t0);
        if (frames > 0) frametime = 1000.0 * (t - t0) / frames;
        sprintf(titlestring, "TNM061, %.2f ms/frame (%.1f FPS)", frametime, fps);
        glfwSetWindowTitle(window, titlestring);
        // printf("Speed: %.1f FPS\n", fps);
        t0 = t;
        frames = 0;
    }
    frames++;
    return fps;
}
