//
// Created by LEI XU on 4/11/19.
//

#ifndef RASTERIZER_TRIANGLE_H
#define RASTERIZER_TRIANGLE_H

#include <eigen3/Eigen/Eigen>


using namespace Eigen;
class Triangle{

public:
    std::array<Vector3f, 3> v; /*the original coordinates of the triangle, v0, v1, v2 in counter clockwise order*/
    /*Per vertex values*/
    std::array<Vector3f, 3> color; //color at each vertex;
    std::array<Vector2f, 3> tex_coords; //texture u,v
    std::array<Vector3f, 3> normal; //normal vector for each vertex

    //Texture *tex;
    Triangle();

    void setVertex(int ind, Vector3f ver); /*set i-th vertex coordinates */
    void setNormal(int ind, Vector3f n); /*set i-th vertex normal vector*/
    void setColor(int ind, float r, float g, float b); /*set i-th vertex color*/
    void setColor(int ind, Vector3f rgb);
    void setTexCoord(int ind, float s, float t); /*set i-th vertex texture coordinate*/
    std::array<Vector4f, 3> toVector4() const;
};






#endif //RASTERIZER_TRIANGLE_H
