#ifndef RayTracer_H
#define RayTracer_H

#include <cassert>
#include <vecmath.h>

#include "Ray.h"
#include "Hit.h"
#include "SceneParser.h"
class RayTracer
{
public:
	
  RayTracer(){}

  virtual ~RayTracer()
  {

  }

  static Vector3f traceRay(Ray& r, SceneParser& scene, int depth, bool castShadows);
 
 protected:
  
  //
};



#endif // RayTracer_H
