#ifndef OBJECT3D_H
#define OBJECT3D_H

#include "Ray.h"
#include "Hit.h"
#include "Material.h"
#include <vector>
#include "BVH.h"

class Object3D
{
public:
	Object3D()
	{
		material = NULL;
    hasBv = false;
	}
	virtual ~Object3D(){

  }

	Object3D( Material* material){
	this->material = material ; 
  hasBv = false;
	}
	
	virtual bool intersect( const Ray& r , Hit& h, float tmin) = 0;
  virtual void loadBvh(){};

	char* type;
  
  bool hasBv;
  BVH bvh;
protected:
  
  
	Material* material;
};

#endif

