#include "Material.h"

#include <cassert>
#include <vecmath.h>

#include "Ray.h"
#include "Hit.h"
#include "texture.hpp"


Vector3f Material::getDiffuseColor() 
{ 
  return  diffuseColor;
}
Vector3f Material::getDiffuseColor(Hit &h) 
{ 
  if(h.hasTex && this->t.valid())
    return this->t(h.texCoord.x(),h.texCoord.y());
  return  diffuseColor;
}
  

Vector3f Material::Shade( const Ray& ray, const Hit& hit,
                const Vector3f& dirToLight, const Vector3f& lightColor ) 
{
  Vector3f r = (ray.getDirection() - 2 * Vector3f::dot(ray.getDirection(), hit.getNormal()) * hit.getNormal()).normalized();

  float cos = clamp(Vector3f::dot(hit.getNormal(), dirToLight.normalized()), 0.0f, 1.0f);
  float s = pow(clamp(Vector3f::dot(dirToLight.normalized(), r), 0.0f, 1.0f), shininess);

  Vector3f diffuseC = diffuseColor;
  if(hit.hasTex && t.valid())
    diffuseC = t(hit.texCoord.x(),hit.texCoord.y());
  return ((lightColor * cos *  diffuseC) + (lightColor * s * specularColor));

  
}

void Material::loadTexture(const char * filename){
  t.load(filename);
}
