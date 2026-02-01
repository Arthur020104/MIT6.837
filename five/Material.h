#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "Ray.h"
#include "Hit.h"
#include "texture.hpp"
#include "Noise.h"
///TODO:
///Implement Shade function that uses ambient, diffuse, specular and texture
class Material
{
public:
	
 Material( const Vector3f& d_color ,const Vector3f& s_color=Vector3f::ZERO, float s=0, float refractionIndexm = 0):
  diffuseColor( d_color),specularColor(s_color), shininess(s), refractionIndex(refractionIndexm)
  {
        	
  }

  virtual ~Material()
    {

    }

  virtual Vector3f getDiffuseColor();
  virtual Vector3f getDiffuseColor(Hit &h);
  Vector3f getSpecular(){return specularColor;}
  void setNoise(const Noise & n);
  float getRefraction(){return refractionIndex;};
  Vector3f Shade( const Ray& ray, const Hit& hit,
                  const Vector3f& dirToLight, const Vector3f& lightColor, const float distance  );

  void loadTexture(const char * filename);
  float shininess;
 protected:
  Vector3f diffuseColor;
  Vector3f specularColor;
  
  Texture t;
  float refractionIndex;
  Noise noise;
  bool hasNoise;
};



#endif // MATERIAL_H
