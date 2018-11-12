#ifndef VBOMESH_H
#define VBOMESH_H

#include <nanogui/opengl.h>
#include <nanogui/glutil.h>
#include <vector>
#include <glm.hpp>
#include <string>


class VBOMesh
{
private:
    unsigned int faces;
    unsigned int vaoHandle;

    bool reCenterMesh, loadTex, genTang;

    void trimString( std::string & str );
    void storeVBO( const std::vector<glm::vec3> & points,
                            const std::vector<glm::vec3> & normals,
                            const std::vector<glm::vec2> &texCoords,
                            const std::vector<glm::vec4> &tangents,
                            const std::vector<int> &elements );
    void generateAveragedNormals(
            const std::vector<glm::vec3> & points,
			std::vector<glm::vec3> & normals,
            const std::vector<int> & faces );
    void generateTangents(
            const std::vector<glm::vec3> & points,
            const std::vector<glm::vec3> & normals,
            const std::vector<int> & faces,
            const std::vector<glm::vec2> & texCoords,
			std::vector<glm::vec4> & tangents);
    void center(std::vector<glm::vec3> &);

public:
    VBOMesh( const char * fileName, bool reCenterMesh = false, bool loadTc = false, bool genTangents = false );
    void render() const;
    void loadOBJ( const char * fileName );
};

#endif // VBOMESH_H
