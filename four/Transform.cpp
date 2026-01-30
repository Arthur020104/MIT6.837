
#include <vecmath.h>
#include "Object3D.h"
#include "./Transform.h"

Transform::Transform( const Matrix4f& m, Object3D* obj ):o(obj)
{
  ivM = m.inverse();
  this->m = m;
  this->hasBv = false;
}
bool Transform::intersect( const Ray& r , Hit& h , float tmin)
{
  //colocando os vetores no spaco do objetoo
  Vector4f oT = this->ivM * Vector4f(r.getOrigin(), 1.0f);
  Vector4f dT = this->ivM * Vector4f(r.getDirection(), 0.0f);
  
  Ray ray = Ray(oT.xyz(), dT.xyz().normalized());

  float prev = h.getT();

  bool result = o->intersect( ray , h , tmin);

  if(h.getT() != prev)
  { 
    Vector4f normal = ivM.transposed() * Vector4f(h.getNormal(), 0.0f);
    h.set(h.getT(), h.getMaterial(), normal.xyz().normalized());
  } 
  return result;
}
void Transform::loadBvh()
{
  o->loadBvh();
}