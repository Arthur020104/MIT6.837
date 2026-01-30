
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
  
  float scale = dT.xyz().abs();
  Ray ray = Ray(oT.xyz(), dT.xyz().normalized());

  Hit lh = Hit();


  if(o->intersect( ray , lh , tmin * scale))
  { 
    float nt = lh.getT() / scale;

    if(nt < h.getT())
    {
      Vector4f normal = ivM.transposed() * Vector4f(lh.getNormal(), 0.0f);
      h.set(nt, lh.getMaterial(), normal.xyz().normalized());
      h.hasTex = lh.hasTex;
      h.texCoord = lh.texCoord;
      
      return true;
    
    }
    
  } 
  return false;
}
void Transform::loadBvh()
{
  o->loadBvh();
}