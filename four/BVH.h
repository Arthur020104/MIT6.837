#ifndef BVH_H
#define BVH_H

#include "Ray.h"
#include "Hit.h"
#include "Material.h"
#include <vector>
#include <algorithm>
#include <iostream>
class Object3D;
inline void swp(float& t1, float& t2)
{
  float tmp = t1;
  t1 = t2;
  t2 = tmp;
}
class BVH
{
public:
  BVH(Vector3f min, Vector3f max)
  {
    BVH(false, min, max, std::vector<Object3D*>());
  }
	BVH(bool leaf = false, Vector3f min = Vector3f(), Vector3f max = Vector3f(), std::vector<Object3D*> objs = std::vector<Object3D*>());
  ~BVH();

  std::vector<Object3D*> intersectBv(const Ray& r, float tmin, float& closest);

  Vector3f bv[2];
  BVH* left;
  BVH* right;
  bool isLeaf;
  std::vector<Object3D*> objects;
  
protected:
  
  
	Material* material;
};

#endif

