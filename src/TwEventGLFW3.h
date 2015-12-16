//
// Created by drluke on 12/16/15.
//

#ifndef COCKAMAMIE_TWEVENTGLFW3_H
#define COCKAMAMIE_TWEVENTGLFW3_H

#include <GLFW/glfw3.h>
#include <AntTweakBar.h>

int TW_CALL TwEventMouseButtonGLFW3(GLFWwindow *window, int glfwButton, int glfwAction, int glfwMods);
static int convertModifiers(int glfwMods);
int TW_CALL TwEventKeyGLFW3(GLFWwindow *window, int glfwKey, int glfwScancode, int glfwAction, int glfwMods);
int TW_CALL TwEventCharModsGLFW3(GLFWwindow *window, int glfwChar, int glfwMods);
int TW_CALL TwEventCursorPosGLFW3(GLFWwindow *window, double xpos, double ypos);
int TW_CALL TwEventScrollGLFW3(GLFWwindow *window, double xoffset, double yoffset);


#endif //COCKAMAMIE_TWEVENTGLFW3_H
