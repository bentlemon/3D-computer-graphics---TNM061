#if defined(WIN32) && !defined(_USE_MATH_DEFINES)
#define _USE_MATH_DEFINES
#endif

#include <GL/glew.h>

#include <cstdio>   // For file input in soupReadOBJ()
#include <cstdlib>  // For malloc() and free()
#include <cmath>    // For sin() and cos() in soupCreateSphere()
#include <cstring>  // For strcmp()

#include "TriangleSoup.hpp"

/* Constructor: initialize a TriangleSoup object to all zeros */
TriangleSoup::TriangleSoup() {
    vao = 0;
    vertexbuffer = 0;
    indexbuffer = 0;
    vertexarray = NULL;
    indexarray = NULL;
    nverts = 0;
    ntris = 0;
}

/* Destructor: clean up allocated data in a TriangleSoup object */
TriangleSoup::~TriangleSoup() { clean(); }

void TriangleSoup::clean() {

    if (glIsVertexArray(vao)) {
        glDeleteVertexArrays(1, &vao);
    }
    vao = 0;

    if (glIsBuffer(vertexbuffer)) {
        glDeleteBuffers(1, &vertexbuffer);
    }
    vertexbuffer = 0;

    if (glIsBuffer(indexbuffer)) {
        glDeleteBuffers(1, &indexbuffer);
    }
    indexbuffer = 0;

    if (vertexarray) {
        delete[] vertexarray;
    }
    if (indexarray) {
        delete[] indexarray;
    }
    nverts = 0;
    ntris = 0;
}

/* Create a simple box geometry */
void TriangleSoup::createBox(float, float, float) { /* Not yet implemented */ }

/*
 * createSphere(float radius, int segments)
 *
 * Create a TriangleSoup objectwith vertex and index arrays
 * to draw a textured sphere with normals.
 * Increasing the parameter 'segments' yields more triangles.
 * The vertex array is on interleaved format. For each vertex, there
 * are 8 floats: three for the vertex coordinates (x, y, z), three
 * for the normal vector (n_x, n_y, n_z) and finally two for texture
 * coordinates (s, t). The arrays are allocated by malloc() inside the
 * function and should be disposed of using free() when they are no longer
 * needed, e.g with the function soupDelete().
 *
 * Author: Stefan Gustavson (stegu@itn.liu.se) 2014.
 * This code is in the public domain.
 */
void TriangleSoup::createSphere(float radius, int segments) {
    int i, j, base, i0;
    int vsegs, hsegs;
    int stride = 8;

    // Delete any previous content in the TriangleSoup object
    clean();

    vsegs = segments;
    if (vsegs < 2) vsegs = 2;
    hsegs = vsegs * 2;
    nverts = 1 + (vsegs - 1) * (hsegs + 1) + 1;       // top + middle + bottom
    ntris = hsegs + (vsegs - 2) * hsegs * 2 + hsegs;  // top + middle + bottom
    vertexarray = new float[nverts * 8];
    indexarray = new GLuint[ntris * 3];

    // The vertex array: 3D xyz, 3D normal, 2D st (8 floats per vertex)
    // First vertex: top pole (+z is "up" in object local coords)
    vertexarray[0] = 0.0f;
    vertexarray[1] = 0.0f;
    vertexarray[2] = radius;
    vertexarray[3] = 0.0f;
    vertexarray[4] = 0.0f;
    vertexarray[5] = 1.0f;
    vertexarray[6] = 0.5f;
    vertexarray[7] = 1.0f;
    // Last vertex: bottom pole
    base = (nverts - 1) * stride;
    vertexarray[base] = 0.0f;
    vertexarray[base + 1] = 0.0f;
    vertexarray[base + 2] = -radius;
    vertexarray[base + 3] = 0.0f;
    vertexarray[base + 4] = 0.0f;
    vertexarray[base + 5] = -1.0f;
    vertexarray[base + 6] = 0.5f;
    vertexarray[base + 7] = 0.0f;
    // All other vertices:
    // vsegs-1 latitude rings of hsegs+1 vertices each
    // (duplicates at texture seam s=0 / s=1)
    for (j = 0; j < vsegs - 1; j++) {  // vsegs-1 latitude rings of vertices
        float theta = static_cast<float>(static_cast<double>(j + 1) / vsegs * M_PI);
        const float z = cosf(theta);
        const float R = sinf(theta);
        for (i = 0; i <= hsegs; i++) {  // hsegs+1 vertices in each ring (duplicate for texcoords)
            float phi = static_cast<float>(static_cast<double>(i) / hsegs * 2.0 * M_PI);
            const float x = R * cosf(phi);
            const float y = R * sinf(phi);
            base = (1 + j * (hsegs + 1) + i) * stride;
            vertexarray[base] = radius * x;
            vertexarray[base + 1] = radius * y;
            vertexarray[base + 2] = radius * z;
            vertexarray[base + 3] = x;
            vertexarray[base + 4] = y;
            vertexarray[base + 5] = z;
            vertexarray[base + 6] = (float)i / hsegs;
            vertexarray[base + 7] = 1.0f - (float)(j + 1) / vsegs;
        }
    }

    // The index array: triplets of integers, one for each triangle
    // Top cap
    for (i = 0; i < hsegs; i++) {
        indexarray[3 * i] = 0;
        indexarray[3 * i + 1] = 1 + i;
        indexarray[3 * i + 2] = 2 + i;
    }
    // Middle part (possibly empty if vsegs=2)
    for (j = 0; j < vsegs - 2; j++) {
        for (i = 0; i < hsegs; i++) {
            base = 3 * (hsegs + 2 * (j * hsegs + i));
            i0 = 1 + j * (hsegs + 1) + i;
            indexarray[base] = i0;
            indexarray[base + 1] = i0 + hsegs + 1;
            indexarray[base + 2] = i0 + 1;
            indexarray[base + 3] = i0 + 1;
            indexarray[base + 4] = i0 + hsegs + 1;
            indexarray[base + 5] = i0 + hsegs + 2;
        }
    }
    // Bottom cap
    base = 3 * (hsegs + 2 * (vsegs - 2) * hsegs);
    for (i = 0; i < hsegs; i++) {
        indexarray[base + 3 * i] = nverts - 1;
        indexarray[base + 3 * i + 1] = nverts - 2 - i;
        indexarray[base + 3 * i + 2] = nverts - 3 - i;
    }

    // Generate one vertex array object (VAO) and bind it
    glGenVertexArrays(1, &(vao));
    glBindVertexArray(vao);

    // Generate two buffer IDs
    glGenBuffers(1, &vertexbuffer);
    glGenBuffers(1, &indexbuffer);

    // Activate the vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // Present our vertex coordinates to OpenGL
    glBufferData(GL_ARRAY_BUFFER, 8 * nverts * sizeof(GLfloat), vertexarray, GL_STATIC_DRAW);
    // Specify how many attribute arrays we have in our VAO
    glEnableVertexAttribArray(0);  // Vertex coordinates
    glEnableVertexAttribArray(1);  // Normals
    glEnableVertexAttribArray(2);  // Texture coordinates
    // Specify how OpenGL should interpret the vertex buffer data:
    // Attributes 0, 1, 2 (must match the lines above and the layout in the shader)
    // Number of dimensions (3 means vec3 in the shader, 2 means vec2)
    // Type GL_FLOAT
    // Not normalized (GL_FALSE)
    // Stride 8 (interleaved array with 8 floats per vertex)
    // Array buffer offset 0, 3, 6 (offset into first vertex)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (void*)0);  // xyz coordinates
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (void*)(3 * sizeof(GLfloat)));  // normals
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (void*)(6 * sizeof(GLfloat)));  // texcoords

    // Activate the index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
    // Present our vertex indices to OpenGL
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * ntris * sizeof(GLuint), indexarray, GL_STATIC_DRAW);

    // Deactivate (unbind) the VAO and the buffers again.
    // Do NOT unbind the buffers while the VAO is still bound.
    // The index buffer is an essential part of the VAO state.
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

/*
 * readObj(const char* filename)
 *
 * Load TriangleSoup geometry data from an OBJ file.
 * The vertex array is on interleaved format. For each vertex, there
 * are 8 floats: three for the vertex coordinates (x, y, z), three
 * for the normal vector (n_x, n_y, n_z) and finally two for texture
 * coordinates (s, t). The returned arrays are allocated by malloc()
 * inside the function and should be disposed of using free() when
 * they are no longer needed, e.g. by calling soupDelete().
 *
 * Author: Stefan Gustavson (stegu@itn.liu.se) 2014.
 * This code is in the public domain.
 */
void TriangleSoup::readOBJ(const char* filename) {

    FILE* objfile;

    int numverts = 0;
    int numnormals = 0;
    int numtexcoords = 0;
    int numfaces = 0;
    int i_v = 0;
    int i_n = 0;
    int i_t = 0;
    int i_f = 0;
    float *verts, *normals, *texcoords;

    char line[256];
    char tag[3];
    int v1, v2, v3, n1, n2, n3, t1, t2, t3;
    int numargs, readerror, currentv;

    readerror = 0;

    objfile = fopen(filename, "r");

    if (!objfile) {
        printError("File not found", filename);
        readerror = 1;
    }

    // Scan through the file to count the number of data elements
    while (fgets(line, 256, objfile)) {
        sscanf(line, "%2s ", tag);
        if (!strcmp(tag, "v"))
            numverts++;
        else if (!strcmp(tag, "vn"))
            numnormals++;
        else if (!strcmp(tag, "vt"))
            numtexcoords++;
        else if (!strcmp(tag, "f"))
            numfaces++;
        // else printf("Ignoring line starting with \"%s\"\n", tag);
    }

    printf("loadObj(\"%s\"): found %d vertices, %d normals, %d texcoords, %d faces.\n", filename,
           numverts, numnormals, numtexcoords, numfaces);

    verts = new float[3 * numverts];
    normals = new float[3 * numnormals];
    texcoords = new float[2 * numtexcoords];

    vertexarray = new float[8 * 3 * numfaces];
    indexarray = new unsigned int[3 * numfaces];
    nverts = 3 * numfaces;
    ntris = numfaces;

    rewind(objfile);  // Start from the top again to read data

    while (fgets(line, 256, objfile)) {
        tag[0] = '\0';
        sscanf(line, "%2s ", tag);
        if (!strcmp(tag, "v")) {
            //			printf("Reading vertex %d\n", i_v+1);
            numargs = sscanf(line, "v %f %f %f", &verts[3 * i_v], &verts[3 * i_v + 1],
                             &verts[3 * i_v + 2]);
            if (numargs != 3) {
                printf("Malformed vertex data found at vertex %d.\n", i_v + 1);
                printf("Aborting.\n");
                readerror = 1;
                break;
            }
            //			printf("Read vertex coord %d: %8.2f %8.2f %8.2f\n",
            //			i_v, verts[3*i_v], verts[3*i_v+1], verts[3*i_v+2]);
            i_v++;
        } else if (!strcmp(tag, "vn")) {
            //			printf("Reading normal %d\n", i_n+1);
            numargs = sscanf(line, "vn %f %f %f", &normals[3 * i_n], &normals[3 * i_n + 1],
                             &normals[3 * i_n + 2]);
            if (numargs != 3) {
                printf("Malformed normal data found at normal %d.\n", i_n + 1);
                printf("Aborting.\n");
                readerror = 1;
                break;
            }
            i_n++;
        } else if (!strcmp(tag, "vt")) {
            //          printf("Reading texcoord %d\n", i_t+1);
            numargs = sscanf(line, "vt %f %f", &texcoords[2 * i_t], &texcoords[2 * i_t + 1]);
            if (numargs != 2) {
                printf("Malformed texcoord data found at texcoord %d.\n", i_t + 1);
                printf("Aborting.\n");
                readerror = 1;
                break;
            }
            i_t++;
        } else if (!strcmp(tag, "f")) {
            //			printf("Reading face %d\n", i_f+1);
            numargs = sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", &v1, &t1, &n1, &v2, &t2, &n2,
                             &v3, &t3, &n3);
            if (numargs != 9) {
                printf("Malformed face data found at face %d.\n", i_f + 1);
                printf("Aborting.\n");
                readerror = 1;
                break;
            }
            //			printf("Read vertex data %d/%d/%d %d/%d/%d %d/%d/%d\n",
            //			v1, t1, n1, v2, t2, n2, v3, t3, n3);
            v1--;
            v2--;
            v3--;
            n1--;
            n2--;
            n3--;
            t1--;
            t2--;
            t3--;
            currentv = 8 * 3 * i_f;
            vertexarray[currentv] = verts[3 * v1];
            vertexarray[currentv + 1] = verts[3 * v1 + 1];
            vertexarray[currentv + 2] = verts[3 * v1 + 2];
            vertexarray[currentv + 3] = normals[3 * n1];
            vertexarray[currentv + 4] = normals[3 * n1 + 1];
            vertexarray[currentv + 5] = normals[3 * n1 + 2];
            vertexarray[currentv + 6] = texcoords[2 * t1];
            vertexarray[currentv + 7] = texcoords[2 * t1 + 1];
            vertexarray[currentv + 8] = verts[3 * v2];
            vertexarray[currentv + 9] = verts[3 * v2 + 1];
            vertexarray[currentv + 10] = verts[3 * v2 + 2];
            vertexarray[currentv + 11] = normals[3 * n2];
            vertexarray[currentv + 12] = normals[3 * n2 + 1];
            vertexarray[currentv + 13] = normals[3 * n2 + 2];
            vertexarray[currentv + 14] = texcoords[2 * t2];
            vertexarray[currentv + 15] = texcoords[2 * t2 + 1];
            vertexarray[currentv + 16] = verts[3 * v3];
            vertexarray[currentv + 17] = verts[3 * v3 + 1];
            vertexarray[currentv + 18] = verts[3 * v3 + 2];
            vertexarray[currentv + 19] = normals[3 * n3];
            vertexarray[currentv + 20] = normals[3 * n3 + 1];
            vertexarray[currentv + 21] = normals[3 * n3 + 2];
            vertexarray[currentv + 22] = texcoords[2 * t3];
            vertexarray[currentv + 23] = texcoords[2 * t3 + 1];
            indexarray[3 * i_f] = 3 * i_f;
            indexarray[3 * i_f + 1] = 3 * i_f + 1;
            indexarray[3 * i_f + 2] = 3 * i_f + 2;
            i_f++;
        };
    }

    // Free the temporary arrays we created
    delete[] verts;
    delete[] normals;
    delete[] texcoords;
    delete[] objfile;

    if (readerror) {  // Delete corrupt data and bail out if a read error occured
        printError("Mesh read error", "No mesh data generated");
        clean();
        return;
    }

    // Generate one vertex array object (VAO) and bind it
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Generate two buffer IDs
    glGenBuffers(1, &vertexbuffer);
    glGenBuffers(1, &indexbuffer);

    // Activate the vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // Present our vertex coordinates to OpenGL
    glBufferData(GL_ARRAY_BUFFER, 8 * nverts * sizeof(GLfloat), vertexarray, GL_STATIC_DRAW);

    // Specify how many attribute arrays we have in our VAO
    glEnableVertexAttribArray(0);  // Vertex coordinates
    glEnableVertexAttribArray(1);  // Normals
    glEnableVertexAttribArray(2);  // Texture coordinates
    // Specify how OpenGL should interpret the vertex buffer data:
    // Attributes 0, 1, 2 (must match the lines above and the layout in the shader)
    // Number of dimensions (3 means vec3 in the shader, 2 means vec2)
    // Type GL_FLOAT
    // Not normalized (GL_FALSE)
    // Stride 8 (interleaved array with 8 floats per vertex)
    // Array buffer offset 0, 3, 6 (offset into first vertex)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (void*)0);  // xyz coordinates
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (void*)(3 * sizeof(GLfloat)));  // normals
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (void*)(6 * sizeof(GLfloat)));  // texcoords

    // Activate the index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
    // Present our vertex indices to OpenGL
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * ntris * sizeof(GLuint), indexarray, GL_STATIC_DRAW);

    // Deactivate (unbind) the VAO and the buffers again.
    // Do NOT unbind the buffers while the VAO is still bound.
    // The index buffer is an essential part of the VAO state.
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return;
}

/* Print data from a TriangleSoup object, for debugging purposes */
void TriangleSoup::print() {
    int i;

    printf("TriangleSoup vertex data:\n\n");
    for (i = 0; i < nverts; i++) {
        printf("%d: %8.2f %8.2f %8.2f\n", i, vertexarray[8 * i], vertexarray[8 * i + 1],
               vertexarray[8 * i + 2]);
    }
    printf("\nTriangleSoup face index data:\n\n");
    for (i = 0; i < ntris; i++) {
        printf("%d: %d %d %d\n", i, indexarray[3 * i], indexarray[3 * i + 1],
               indexarray[3 * i + 2]);
    }
}

/* Print information about a TriangleSoup object (stats and extents) */
void TriangleSoup::printInfo() {
    int i;
    float x, y, z, xmin, xmax, ymin, ymax, zmin, zmax;

    printf("TriangleSoup information:\n");
    printf("vertices : %d\n", nverts);
    printf("triangles: %d\n", ntris);
    xmin = xmax = vertexarray[0];
    ymin = ymax = vertexarray[1];
    zmin = zmax = vertexarray[2];
    for (i = 1; i < nverts; i++) {
        x = vertexarray[8 * i];
        y = vertexarray[8 * i + 1];
        z = vertexarray[8 * i + 2];
        //         printf("x y z : %8.2f %8.2f %8.2f\n", x, y, z);
        if (x < xmin) xmin = x;
        if (x > xmax) xmax = x;
        if (y < ymin) ymin = y;
        if (y > ymax) ymax = y;
        if (z < zmin) zmin = z;
        if (z > zmax) zmax = z;
    }
    printf("xmin: %8.2f\n", xmin);
    printf("xmax: %8.2f\n", xmax);
    printf("ymin: %8.2f\n", ymin);
    printf("ymax: %8.2f\n", ymax);
    printf("zmin: %8.2f\n", zmin);
    printf("zmax: %8.2f\n", zmax);
}

/* Render the geometry in a TriangleSoup object */
void TriangleSoup::render() {

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 3 * ntris, GL_UNSIGNED_INT, (void*)0);
    // (mode, vertex count, type, element array buffer offset)
    glBindVertexArray(0);
}

/*
 * private
 * printError() - Signal an error.
 * Simple printf() to console for portability.
 */
void TriangleSoup::printError(const char* errtype, const char* errmsg) {
    fprintf(stderr, "%s: %s\n", errtype, errmsg);
}
