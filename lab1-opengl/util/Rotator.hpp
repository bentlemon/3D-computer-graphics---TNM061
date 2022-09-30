#pragma once

struct GLFWwindow;

class KeyRotator {
public:
    double phi;
    double theta;

private:
    double lastTime;

public:
    void init(GLFWwindow* window);
    void poll(GLFWwindow* window);
};

class MouseRotator {
public:
    double phi;
    double theta;

private:
    double lastX;
    double lastY;
    int lastLeft;
    int lastRight;

public:
    void init(GLFWwindow* window);
    void poll(GLFWwindow* window);
};
