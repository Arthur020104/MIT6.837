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
  if(index >= this->objs.size())
    objs.push_back(obj);
  else
    objs[index] = obj;
  
}

int Group::getGroupSize(){ 
  return (int)this->objs.size();
}