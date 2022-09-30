/*
 * Framework library functions for the course TNM061
 * Author: Stefan Gustavson 2013-2014 (stefan.gustavson@liu.se)
 * updated 2021 by Martin Falk (martin.falk@liu.se)
 * 
 * This code is in the public domain.
 */

#pragma once

namespace tnm061 {
/*
 * printError() - Signal an error.
 * Simple printf() to console for portability.
 */
void printError(const char* errtype, const char* errmsg);

/*
 * displayFPS() - Calculate, display and return frame rate statistics.
 * Called every frame, but statistics are updated only once per second.
 * The time per frame is a better measure of performance than the
 * number of frames per second, so both are displayed.
 *
 * NOTE: This function doesn't work properly if you call it for multiple
 * windows. Call it only for one window, and only once for each frame.
 */
double displayFPS(GLFWwindow* window);

}  // namespace tnm061
