#include "ClothSystem.h"
#include <stdexcept> 

#include<iostream>
//TODO: Initialize here
const float g = -9.80665f;
const float airDrag = 1.9725f;
const float ropeK = 200.0f;
ClothSystem::ClothSystem(unsigned int numParticlesWidth, unsigned int numParticlesHeight)
{
  if(numParticlesWidth <= 2 || numParticlesHeight <= 2)
  {
    throw std::runtime_error("Cloth must be bigger.\n");
  }
  this->width = numParticlesWidth;
  this->height = numParticlesHeight;
  this->length = 0.25f;
  this->mass = 0.8f;
  this->initialOffset = Vector3f(-1.0f, 1.25f, 0.0f);
  this->diagonalDistance = sqrt(this->length * this->length + this->length * this->length);
  this->drawState = 2;
  Vector3f v;
  for(size_t j = 0; j < numParticlesHeight; j++)
  {
    for(size_t i = 0; i < numParticlesWidth; i++)
    {
      v = Vector3f((float)i * this->length, 0.0f, -(float)j * this->length) + initialOffset;
      m_vVecState.push_back(v);
      m_vVecState.push_back(Vector3f(0.0f, 0.0f, 0.0f));
    }
  }
}
int ClothSystem::indexOf(unsigned int i, unsigned int j)
{
  if(i >= this->width || j >= this->height)
    return -1;
  int index = j * 2 * width + i * 2; 

  return index;
}
void ClothSystem::addRopeForce(int statei, int statej, Vector3f* rope, float ropeLength)
{
  if(statei < 0 || statej < 0 || statei >= this->m_vVecState.size() || statej >= this->m_vVecState.size())
    return;
  
  Vector3f d = this->m_vVecState[statei] - this->m_vVecState[statej];
  if(d.abs() == 0.0f)
    return;
  *rope += -ropeK * (d.abs() - ropeLength) * (d.normalized());
}
void ClothSystem::addRopeForce(int statei, int statej, Vector3f* rope)
{
  addRopeForce(statei, statej, rope, this->length);
}
// TODO: implement evalF
// for a given state, evaluate f(X,t)
vector<Vector3f> ClothSystem::evalF(vector<Vector3f> state)
{
	vector<Vector3f> f;
  for(size_t j = 0; j < this->height; j++)
  {
    for(size_t i = 0; i < this->width; i++)
    {
      
      
      Vector3f gravity = Vector3f(0.0f, g * mass, 0.0f);
      Vector3f drag = state[indexOf(i, j) + 1] * -(mass * airDrag);
      Vector3f rope = Vector3f(0.0f, 0.0f, 0.0f);

      if(j == 0 && (i == 0 || i == this->width - 1))
      {
        f.push_back(Vector3f(0.0f, 0.0f, 0.0f));
        f.push_back(Vector3f(0.0f, 0.0f, 0.0f));
        continue;
      }
      //structural
      addRopeForce(indexOf(i, j), indexOf(i, j + 1), &rope);
      addRopeForce(indexOf(i, j), indexOf(i + 1, j), &rope);
      addRopeForce(indexOf(i, j), indexOf(i, j - 1), &rope);
      addRopeForce(indexOf(i, j), indexOf(i - 1, j), &rope);
      addRopeForce(indexOf(i, j), indexOf(i + 1, j + 1), &rope,this->diagonalDistance);
      addRopeForce(indexOf(i, j), indexOf(i - 1, j - 1), &rope,this->diagonalDistance);
      addRopeForce(indexOf(i, j), indexOf(i - 1, j + 1), &rope,this->diagonalDistance);
      addRopeForce(indexOf(i, j), indexOf(i + 1, j - 1), &rope,this->diagonalDistance);

      addRopeForce(indexOf(i, j), indexOf(i + 2, j), &rope, this->length * 2.0f);
      addRopeForce(indexOf(i, j), indexOf(i, j + 2), &rope, this->length * 2.0f);
      addRopeForce(indexOf(i, j), indexOf(i - 2, j), &rope, this->length * 2.0f);
      addRopeForce(indexOf(i, j), indexOf(i, j - 2), &rope, this->length * 2.0f);
      
      Vector3f sum = (rope + gravity + drag) / mass;

      f.push_back(state[indexOf(i, j) + 1]);
      f.push_back((sum));
    }
  }
	// YOUR CODE HERE

	return f;
}
void ClothSystem::addForceToClosestVertex(Vector4f origin, Vector4f direction, Vector3f forceVector)
{
  int biggerIdx;
  float closestValue = 0.0f;
  
  float threshold = 0.98f;

  GLfloat modelview[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
  Matrix4f m = Matrix4f(modelview[0], modelview[1], modelview[2], modelview[3],
                      modelview[4], modelview[5], modelview[6], modelview[7],
                      modelview[8], modelview[9], modelview[10], modelview[11],
                      modelview[12], modelview[13], modelview[14], modelview[15]);
  //m.transpose();
  for(size_t j = 0; j < this->height; j++)
  {
    for(size_t i = 0; i < this->width; i++)
    { 
      
      Vector3f v = (m * Vector4f(this->m_vVecState[indexOf(i, j)], 1.0f)).xyz();


      Vector3f relativeDir = (v - origin.xyz()).normalized();

      float cos = Vector3f::dot(relativeDir, direction.xyz());
      if(cos > closestValue)
      {
        biggerIdx = indexOf(i, j);
        closestValue = cos;
      }
    }
    
  }
  if(closestValue < threshold)
    return;
  this->m_vVecState[biggerIdx+1] += forceVector / this->mass;
  Vector3f v = (m * Vector4f(this->m_vVecState[biggerIdx], 1.0f)).xyz();
  Vector3f relativeDir = (v - origin.xyz()).normalized();
}
///TODO: render the system (ie draw the particles)
void ClothSystem::drawWireFrame()
{
  glLineWidth(3);
  glBegin(GL_LINES);
  for(size_t j = 0; j < this->height; j++)
  {
    for(size_t i = 0; i < this->width; i++)
    {      
      int indexR = indexOf(i+1, j);
      int indexUp = indexOf(i, j+1);
      if(indexR != -1)
      {
        glPushMatrix();
        glVertex3fv(this->m_vVecState[indexOf(i, j)]);
        glVertex3fv(this->m_vVecState[indexR]);
        glPopMatrix();
      }
        
      
      if(indexUp != -1)
      {
        glPushMatrix();
        glVertex3fv(this->m_vVecState[indexOf(i, j)]); 
        glVertex3fv(this->m_vVecState[indexUp]);
        glPopMatrix();
      }
    }
    
  }
  glEnd();
}
void ClothSystem::drawParticles()
{
  for(size_t j = 0; j < this->height; j++)
  {
    for(size_t i = 0; i < this->width; i++)
    {
      Vector3f pos = this->m_vVecState[indexOf(i, j)] ;//  position of particle i. YOUR CODE HERE
      glPushMatrix();
      glTranslatef(pos[0], pos[1], pos[2] );
      glutSolidSphere(0.075f,10.0f,10.0f);
      glPopMatrix();
     
    }
  }
} 
void ClothSystem::triangleDrawHelper(int v0Idx, int v1Idx, int v2Idx)
{
  if(v0Idx != -1 && v1Idx != -1 && v2Idx != -1)
  {
    Vector3f v0 = this->m_vVecState[v0Idx] ;
    Vector3f v1 = this->m_vVecState[v1Idx];
    Vector3f v2 = this->m_vVecState[v2Idx];
    Vector3f normal = Vector3f::cross(v0 - v1, v0 - v2);

    glPushMatrix();

    glNormal3fv(normal);
    glVertex3fv(v0);
    glNormal3fv(normal);
    glVertex3fv(v1);
    glNormal3fv(normal);
    glVertex3fv(v2);

    glPopMatrix();
  }
}

    
void ClothSystem::drawRaster()
{
  glDisable(GL_CULL_FACE);

  GLfloat color[] = {0.3f, 0.5f, 0.8f, 1.0f};
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
  
  glBegin(GL_TRIANGLES);
  for(size_t j = 0; j < this->height; j++)
  {
    for(size_t i = 0; i < this->width; i++)
    {
      triangleDrawHelper(indexOf(i, j), indexOf(i, j + 1), indexOf(i + 1, j));
      triangleDrawHelper(indexOf(i + 1, j), indexOf(i, j + 1), indexOf(i + 1, j + 1));
    }
  }
  glEnd();
}
void ClothSystem::draw()
{

  switch(this->drawState)
  {
    case 0:
      this->drawWireFrame();
      break;
    case 1:
      this->drawParticles();
      break;
    case 2:
      this->drawRaster();
      break;
    default:
      this->drawWireFrame();
      this->drawState = 0;
      break;
  }
  
  
}

