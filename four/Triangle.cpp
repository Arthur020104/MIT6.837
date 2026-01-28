#include "Object3D.h"
#include <vecmath.h>
#include <cmath>
#include <iostream>
#include "./Triangle.h"



Triangle::Triangle( const Vector3f& a, const Vector3f& b, const Vector3f& c, Material* m):Object3D(m){
    //hasTex = false;
    
    Vector3f cr = Vector3f::cross(b-a, c-a);
    planeNormal = cr.normalized();
    area = cr.abs() * 0.5;

    vertexPos[0] = a; vertexPos[1] = b; vertexPos[2] = c;

    centerPoint = (a + b + c) / 3.0;
}

bool Triangle::intersect( const Ray& r,  Hit& hit , float tmin){
  double v = Vector3f::dot(planeNormal, r.getDirection());
  
  if(v == 0)
    v += 0.01f;
  
  const double t = (Vector3f::dot(planeNormal, centerPoint - r.getOrigin())) / v;
  
  if(t < 0.01)
    return false;
  
  Vector3f p = r.getOrigin() + t * r.getDirection();
  double alpha = (Vector3f::cross( vertexPos[1] - p, vertexPos[2] - p).abs() * 0.5) / area;
  double beta = (Vector3f::cross( vertexPos[2] - p, vertexPos[0] - p).abs() * 0.5) / area;
  double gamma = (Vector3f::cross( vertexPos[0] - p, vertexPos[1] - p).abs() * 0.5) / area;
  double total = alpha + beta + gamma;

  if(abs(total - 1.0) < 0.01)
  {
    if(t < hit.getT())
    {
      Vector3f normalf = (alpha * normals[0] + beta * normals[1] + gamma * normals[2]).normalized();
      Vector2f texCoord =  alpha * texCoords[0] + beta * texCoords[1] + gamma * texCoords[2];
      hit.set(t, material, normalf);
      if(hasTex)
        hit.setTexCoord(texCoord);
    }
    return true;
  }
 
  return false;
}