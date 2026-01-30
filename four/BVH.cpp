#include "BVH.h"
#include "Ray.h"
#include "Hit.h"
#include "Material.h"
#include <vector>
#include <algorithm>
#include "Object3D.h" 

BVH::BVH(bool leaf, Vector3f min, Vector3f max, std::vector<Object3D*> objs)
{
  bv[0] = min;
  bv[1] = max;
  left = nullptr;
  right = nullptr;
  isLeaf = leaf;
  objects  = std::vector<Object3D*>(objs);
  
  if(isLeaf && objects.empty())
    std::cout << "Leaf BVH must have object" << std::endl;
}
BVH::~BVH(){
  delete left;
  delete right;
}

std::vector<Object3D*> BVH::intersectBv(const Ray& r, float tmin, float& closest)
{
  float t1x = (bv[0].x() - r.getOrigin().x()) * r.invDrx;
  float t2x = (bv[1].x() - r.getOrigin().x()) * r.invDrx;

  if(t1x > t2x)
    swp(t1x, t2x);

  float t1y = (bv[0].y() - r.getOrigin().y()) * r.invDry;
  float t2y = (bv[1].y() - r.getOrigin().y()) * r.invDry;

  if(t1y > t2y)
    swp(t1y, t2y);
  
  float t1z = (bv[0].z() - r.getOrigin().z()) * r.invDrz;
  float t2z = (bv[1].z() - r.getOrigin().z()) * r.invDrz;

  if(t1z > t2z)
    swp(t1z, t2z);
  
  float maxOfMin = max(t1x, t1y);
  maxOfMin = max(maxOfMin, t1z);
  float minOfMax = min(t2x, t2y);
  minOfMax = min(minOfMax, t2z);


  if(maxOfMin > minOfMax || minOfMax < tmin)
    return std::vector<Object3D*>();
  
  closest = maxOfMin;  

  if(isLeaf)
    return objects;
  else
  {
    float lClosest = +INFINITY;
    std::vector<Object3D*> leftObj = left->intersectBv(r, tmin, lClosest);
    float rClosest = +INFINITY;
    std::vector<Object3D*> rightObj = right->intersectBv(r, tmin, rClosest);

    if (!leftObj.empty() && !rightObj.empty()) 
    {
      leftObj.insert(leftObj.end(), rightObj.begin(), rightObj.end());
      return leftObj;
    }
    else if (!leftObj.empty()) 
      return leftObj;
    else 
      return rightObj;
  }
}