#include "./Sphere.h"
#include "Object3D.h"
#include <vecmath.h>
#include <cmath>

bool Sphere::intersect( const Ray& r , Hit& h , float tmin) {
 // std::cout<<"on sphere intersect\n";

  float infinity = INFINITY;
  if(this->hasBv && bvh.intersectBv(r, tmin, infinity).empty())
    return false;

  Vector3f o = this->center - r.getOrigin();

  float oDotD = Vector3f::dot(o, r.getDirection());
  float minusODotDir = oDotD;
  
  float d = (oDotD * oDotD) - o.absSquared() + radius * radius;
  if(d < 0)
    return false;
  
  float quadraticPart = sqrt(d);
  
  float t1 = minusODotDir + quadraticPart;
  float t2 = minusODotDir - quadraticPart;

  float best = INFINITY;
  
  if(t1 >= tmin && t1 < best)
    best = t1;
  if(t2 >= tmin && t2 < best)
    best = t2;

  if(best < h.getT())
  {
    Vector3f normal = (r.getOrigin() + r.getDirection() * best) - this->center;
    h.set(best, this->material, normal);//-rdirection eh placeholder para normal
  }
  
  return best != INFINITY;
}
void Sphere::loadBvh()
{
  Vector3f min  = center - Vector3f(radius, radius, radius);
  Vector3f max = center + Vector3f(radius, radius, radius);

  std::vector<Object3D*> thisObj;
  thisObj.push_back(this);
  bvh = BVH(true, min, max, thisObj);

  hasBv = true;
   
}