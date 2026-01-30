#ifndef MESH_H
#define MESH_H
#include <vector>
#include "Object3D.h"
#include "Triangle.h"
#include "Vector2f.h"
#include "Vector3f.h"
//by default counterclockwise winding is front face
struct Trig{
  Trig(){x[0]=0;x[1]=0;x[2]=0;}
  int & operator[](const int i) {return x[i];}
  int x[3];
  int texID[3];
};

class Mesh:public Object3D{
public:
  Mesh(const char * filename, Material* m);
  ~Mesh();
  std::vector<Vector3f>v;
  std::vector<Trig>t;
  std::vector<Vector3f>n;
  std::vector<Vector2f>texCoord; 
  void destructorHelper(BVH* bvh);
  void initializeBVH(BVH* bvh, int depth);
  void loadBvh();
  virtual bool intersect( const Ray& r , Hit& h , float tmin );

  //TODO criar um metodo para criar um triangulo e retornalo a partir de index para t 
  //tera um overload de retorno para que um retorne um triangulo na stack e outro na heap(ponteiro)
private:
  void compute_norm();
};

#endif
