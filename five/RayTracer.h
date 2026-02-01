#ifndef RayTracer_H
#define RayTracer_H

#include <cassert>
#include <vecmath.h>

#include "Ray.h"
#include "Hit.h"
#include "SceneParser.h"
///TODO:
///Implement Shade function that uses ambient, diffuse, specular and texture
class RayTracer
{
public:
	
  RayTracer(){}

  virtual ~RayTracer()
  {

  }

  static Vector3f traceRay(Ray& r, SceneParser& scene, int depth);
 
 protected:
  
  //
};



#endif // RayTracer_H
