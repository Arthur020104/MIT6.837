#include "Mesh.hpp"
#include "Object3D.h"
#include "Triangle.h"
#include "Vector2f.h"
#include "Vector3f.h"
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <utility>
#include <sstream>


bool Mesh::intersect( const Ray& r , Hit& h , float tmin ) {
  bool result = false;
  float infinity = INFINITY;
  std::vector<Object3D*> objs = bvh.intersectBv(r, tmin, infinity);
  
  if(this->hasBv && objs.empty())
    return false;
  else if(this->hasBv)
  {
    for(auto obj : objs)
    {
      result |= obj->intersect(r, h, tmin);
    }
    return result;
  }

  //codigo antigo, que se estivermos sem bvh vai rodar normalmente
  for( unsigned int i = 0 ; i < t.size() ; i++){
    Triangle triangle(v[t[i][0]],
                      v[t[i][1]],v[t[i][2]],material);
    for(int jj=0;jj<3;jj++){
      triangle.normals[jj] = n[t[i][jj]];
            
    }
    if(texCoord.size()>0){
      for(int jj=0;jj<3;jj++){
        triangle.texCoords[jj] = texCoord[t[i].texID[jj]];
      }
      triangle.hasTex=true;
    }
    result |= triangle.intersect( r , h , tmin);
  }
  return result;
}
Mesh::~Mesh()
{
  destructorHelper(&bvh);
}
void Mesh::destructorHelper(BVH* bvh)
{
  if(bvh == nullptr)
    return;
  
  for(auto obj: bvh->objects)
  {
    delete obj;
  }
  destructorHelper(bvh->left);
  destructorHelper(bvh->right);

  delete bvh;
}
bool overlapsBV(Triangle* t, Vector3f& minbv, Vector3f& maxbv)
{
  float minX = min(t->vertexPos[0].x(), t->vertexPos[1].x());
  minX = min(minX, t->vertexPos[2].x());

  float minY = min(t->vertexPos[0].y(), t->vertexPos[1].y());
  minY = min(minY, t->vertexPos[2].y());

  float minZ = min(t->vertexPos[0].z(), t->vertexPos[1].z());
  minZ = min(minZ, t->vertexPos[2].z());

  float maxX = max(t->vertexPos[0].x(), t->vertexPos[1].x());
  maxX = max(maxX, t->vertexPos[2].x());

  float maxY = max(t->vertexPos[0].y(), t->vertexPos[1].y());
  maxY = max(maxY, t->vertexPos[2].y());

  float maxZ = max(t->vertexPos[0].z(), t->vertexPos[1].z());
  maxZ = max(maxZ, t->vertexPos[2].z());


  return (minX <= maxbv.x() && maxX >= minbv.x()) &&
         (minY <= maxbv.y() && maxY >= minbv.y()) &&
         (minZ <= maxbv.z() && maxZ >= minbv.z());
}
void Mesh::initializeBVH(BVH* bvh, int depth)
{
  bool usePlaneNormal = t.size() <= 12;
  if(depth == 0)
  {
    for( unsigned int i = 0 ; i < t.size() ; i++)
    {
      Triangle* triangle = new Triangle(v[t[i][0]],
                        v[t[i][1]],v[t[i][2]],material);
      triangle->usePlaneNormal = usePlaneNormal;
      for(int jj=0;jj<3;jj++)
      {
        triangle->normals[jj] = n[t[i][jj]];      
      }
      if(texCoord.size()>0)
      {
        for(int jj=0;jj<3;jj++)
        {
          triangle->texCoords[jj] = texCoord[t[i].texID[jj]];
        }
        triangle->hasTex=true;
      }


      if(overlapsBV(triangle, bvh->bv[0], bvh->bv[1]))
        bvh->objects.push_back(triangle);
      else
        delete triangle;
    }
    bvh->isLeaf = true;
    return;
  }
  
  if(depth % 3 == 0)
  {
    float midpointX = (bvh->bv[0].x() + bvh->bv[1].x()) * 0.5f;
    bvh->left = new BVH(false, bvh->bv[0], Vector3f(midpointX, bvh->bv[1].y(), bvh->bv[1].z()));
    bvh->right = new BVH(false, Vector3f(midpointX, bvh->bv[0].y(), bvh->bv[0].z()), bvh->bv[1]);
  }
  else if(depth % 3 == 1)
  {
    float midpointZ = (bvh->bv[0].z() + bvh->bv[1].z()) * 0.5f;
    
    bvh->left = new BVH(false, bvh->bv[0], Vector3f(bvh->bv[1].x(), bvh->bv[1].y(), midpointZ));

    bvh->right = new BVH(false, Vector3f(bvh->bv[0].x(), bvh->bv[0].y(), midpointZ), bvh->bv[1]);
  }
  else
  {
    //split y
    float midpointY = (bvh->bv[0].y() + bvh->bv[1].y()) * 0.5f;
    bvh->left = new BVH(false, bvh->bv[0], Vector3f(bvh->bv[1].x(), midpointY, bvh->bv[1].z()));
    bvh->right = new BVH(false, Vector3f(bvh->bv[0].x(), midpointY, bvh->bv[0].z()), bvh->bv[1]);
  }
  initializeBVH(bvh->left, depth - 1);
  initializeBVH(bvh->right, depth - 1);
  
}
void Mesh::loadBvh()
{
  Vector3f min = Vector3f(INFINITY, INFINITY, INFINITY);
  Vector3f max = Vector3f(-INFINITY, -INFINITY, -INFINITY);
  for(Vector3f vertex: v)
  {
    if(min.x() > vertex.x())
      min.x() = vertex.x();
    if(min.y() > vertex.y())
      min.y() = vertex.y();
    if(min.z() > vertex.z())
      min.z() = vertex.z();
    
    if(max.x() < vertex.x())
      max.x() = vertex.x();
    if(max.y() < vertex.y())
      max.y() = vertex.y();
    if(max.z() < vertex.z())
      max.z() = vertex.z();
  }
  
  float boudingTriangleGoal = 5.0f;
  int depth = (int)std::ceil(std::log2(t.size() / boudingTriangleGoal));
  if(depth < 1)
    depth = 1;
  bvh = BVH(false, min, max);

  initializeBVH(&bvh, depth);
  hasBv = true;
  
}

Mesh::Mesh(const char * filename,Material * material):Object3D(material)
{
  std::ifstream f ;
  f.open(filename);
  if(!f.is_open()) {
    std::cout<<"Cannot open "<<filename<<"\n";
    return;
  }
  std::string line;
  std::string vTok("v");
  std::string fTok("f");
  std::string texTok("vt");
  char bslash='/',space=' ';
  std::string tok;
  while(1) {
    std::getline(f,line);
    if(f.eof()) {
      break;
    }
    if(line.size()<3) {
      continue;
    }
    if(line.at(0)=='#') {
      continue;
    }
    std::stringstream ss(line);
    ss>>tok;
    if(tok==vTok) {
      Vector3f vec;
      ss>>vec[0]>>vec[1]>>vec[2];
      v.push_back(vec);
    } else if(tok==fTok) {
      if(line.find(bslash)!=std::string::npos) {
        std::replace(line.begin(),line.end(),bslash,space);
        std::stringstream facess(line);
        Trig trig;
        facess>>tok;
        for(int ii=0; ii<3; ii++) {
          facess>>trig[ii]>>trig.texID[ii];
          trig[ii]--;
          trig.texID[ii]--;
        }
        t.push_back(trig);
      } else {
        Trig trig;
        for(int ii=0; ii<3; ii++) {
          ss>>trig[ii];
          trig[ii]--;
          trig.texID[ii]=0;
        }
        t.push_back(trig);
      }
    } else if(tok==texTok) {
      Vector2f texcoord;
      ss>>texcoord[0];
      ss>>texcoord[1];
      texCoord.push_back(texcoord);
    }
  }
  compute_norm();

  f.close();
  hasBv = false;
}

void Mesh::compute_norm()
{
  n.resize(v.size());
  for(unsigned int ii=0; ii<t.size(); ii++) {
    Vector3f a = v[t[ii][1]] - v[t[ii][0]];
    Vector3f b = v[t[ii][2]] - v[t[ii][0]];
    b=Vector3f::cross(a,b);
    for(int jj=0; jj<3; jj++) {
      n[t[ii][jj]]+=b;
    }
  }
  for(unsigned int ii=0; ii<v.size(); ii++) {
	  n[ii] = n[ii]/ n[ii].abs();
  }
}
