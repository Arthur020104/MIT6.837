#ifndef CLOTHSYSTEM_H
#define CLOTHSYSTEM_H

#include <vecmath.h>
#include <vector>
#include <GL/glut.h>
#include "particleSystem.h"

class ClothSystem: public ParticleSystem
{
///ADD MORE FUNCTION AND FIELDS HERE
public:
	ClothSystem(unsigned int numParticlesWidth, unsigned int numParticlesHeight);
	vector<Vector3f> evalF(vector<Vector3f> state);
	
  Vector3f initialOffset;
	void draw();
  void addForceToClosestVertex(Vector4f origin, Vector4f direction, Vector3f forceVector);

  int drawState;
private:
  unsigned int width;
  unsigned int height;

  int indexOf(unsigned int i, unsigned int j);

  float length;
  float mass;
  float diagonalDistance;
  

  void addRopeForce(int statei, int statej, Vector3f* rope, float ropeLength);
  void addRopeForce(int statei, int statej, Vector3f* rope);

  void drawWireFrame();
  void drawParticles();
  void drawRaster();

  void triangleDrawHelper(int v0Idx, int v1Idx, int v2Idx);

  
};


#endif
