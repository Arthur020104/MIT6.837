#include "Object3D.h"
#include <vecmath.h>
#include <cmath>
#include "./Plane.h"
Plane::Plane( const Vector3f& normal , float d , Material* m):Object3D(m)
{
  this->d = d;
  this->normal = normal;
}

bool Plane::intersect( const Ray& r , Hit& h , float tmin)
{
  float v = Vector3f::dot(normal, r.getDirection());
  if(v == 0)
    v -= tmin;
  const float t = ( Vector3f::dot(normal, (normal * d * 2) -r.getOrigin())) / v;

  if(t >= tmin && t < h.getT())
    h.set(t, material, normal);
  
  return t >= tmin;
}