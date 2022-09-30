#pragma once

class Shader {

public:
    GLuint programID;

    /* Argument-less constructor. Creates an empty shader program. */
    Shader();

    /* Constructor to create, load and compile a Shader program in one blow. */
    Shader(const char* vertexshaderfile, const char* fragmentshaderfile);

    /* Destructor */
    ~Shader();

    /*
     * createShader() - create, load, compile and link the GLSL shader objects.
     */
    void createShader(const char* vertexshaderfile, const char* fragmentshaderfile);

private:
    /*
     * Override the Win32 filelength() function with
     * a version that takes a Unix-style file handle as
     * input instead of a file ID number, and which works
     * on platforms other than Windows.
     */
    long filelength(FILE* file);

    /*
     * readShaderFile(filename) - read a shader source string from a file
     */
    unsigned char* readShaderFile(const char* filename);

    void printError(const char* errtype, const char* errmsg);
};
