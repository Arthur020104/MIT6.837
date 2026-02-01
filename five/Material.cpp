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
                const Vector3f& dirToLight, const Vector3f& lightColor, const float distance ) 
{
  Vector3f N = hit.getNormal();
  Vector3f L = dirToLight.normalized();
  Vector3f V = -ray.getDirection().normalized();

  Vector3f r = (-L + 2 * (Vector3f::dot(L, N) * N)).normalized();

  float cos = max(Vector3f::dot(N, L), 0.0f);
  
  float s = pow(max(Vector3f::dot(V, r), 0.0f), shininess);

  Vector3f diffuseC = diffuseColor;
  if(hit.hasTex && t.valid())
    diffuseC = t(hit.texCoord.x(),hit.texCoord.y());

  if(hasNoise && noise.valid())
  {
    Vector3f p = ray.getOrigin() + ray.getDirection() * hit.getT();
    diffuseC = noise.getColor(p);
  }
  //if(distance >= I)
  
  return (( cos *  diffuseC)) * lightColor;

  
}

void Material::loadTexture(const char * filename){
  t.load(filename);
}
void Material::setNoise(const Noise & n)
{
	noise=n;
  hasNoise = true;
}