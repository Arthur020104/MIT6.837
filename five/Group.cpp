#include "Object3D.h"
#include "Ray.h"
#include "Hit.h"
#include <iostream>
#include <vector>
#include "./Group.h"


Group::Group(){
    this->objs = std::vector<Object3D*>();
  }
	
Group::Group( int num_objects ){
  this->objs = std::vector<Object3D*>(num_objects);
}

bool Group::intersect( const Ray& r , Hit& h , float tmin ) {
  bool hit = false;
  float infinity = INFINITY;
  std::vector<Object3D*> objsBv = bvh.intersectBv(r, tmin, infinity);
  if(this->hasBv && objsBv.empty())
    return false;

  for(auto obj: this->objs)
  {
    if(!hit)
      hit = obj->intersect(r, h, tmin);
    else
      obj->intersect(r, h, tmin);
  }
    
  
  return hit;
}

void Group::addObject( int index , Object3D* obj ){
  if(index >= (int)this->objs.size())
    objs.push_back(obj);
  else
    objs[index] = obj;
  
}

int Group::getGroupSize(){ 
  return (int)this->objs.size();
}

void Group::loadBvh()
{
  for(auto obj: this->objs)
  {
    if(!obj->hasBv)
      obj->loadBvh();
  }

  if(objs.size() == 0)
  {
    //so pra nao rendrizar nada se nao tiver objeto
    Vector3f bvec = Vector3f(-INFINITY, -INFINITY, -INFINITY);
    bvh = BVH(true, bvec, bvec, std::vector<Object3D*>());
    hasBv = true;
    return;
  }
    
  Vector3f min = Vector3f(INFINITY, INFINITY, INFINITY);
  Vector3f max = Vector3f(-INFINITY, -INFINITY, -INFINITY);
  for(Object3D* object: objs)
  {
    if(!object->hasBv)
      continue;

    Vector3f objMin = object->bvh.bv[0];
    Vector3f objMax = object->bvh.bv[1];

    if(min.x() > objMin.x())
      min.x() = objMin.x();
    if(min.y() > objMin.y())
      min.y() = objMin.y();
    if(min.z() > objMin.z())
      min.z() = objMin.z();
    
    if(max.x() < objMax.x())
      max.x() = objMax.x();
    if(max.y() < objMax.y())
      max.y() = objMax.y();
    if(max.z() < objMax.z())
      max.z() = objMax.z();
  }
  std::vector<Object3D*> thisObj;
  thisObj.push_back(this);
  bvh = BVH(true, min, max, thisObj);

  hasBv = true;
}