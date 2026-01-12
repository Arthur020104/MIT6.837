
#include "pendulumSystem.h"
#include <iostream>
const float g = -9.80665f;
const float airDrag = 0.825f;
const float ropeK = 300.0f;
PendulumSystem::PendulumSystem(int numParticles):ParticleSystem(numParticles)
{
	m_numParticles = numParticles;
	this->length = 0.07f;
  this->mass = 0.1f;
  

	// fill in code for initializing the state based on the number of particles
	for (int i = 0; i < m_numParticles; i++) {
		
		// for this system, we care about the position and the velocity
    //par eh pos e impar eh a velocidade direcional
    //iniciar todos em uma linha vertical iniciando em (n*0.25,0.5,0)
    m_vVecState.push_back(Vector3f((i+1)*length, 1.5f, 0.0f));
    m_vVecState.push_back(Vector3f(0.0f, 0.0f, 0.0f));
	}
}


// TODO: implement evalF
// for a given state, evaluate f(X,t)
vector<Vector3f> PendulumSystem::evalF(vector<Vector3f> state)
{
	vector<Vector3f> f;
  for(size_t i = 0; i < state.size(); i+=2)
  {
    //pos -> V
    //V -> A
    if(i == 0)
    {
      f.push_back(Vector3f(0.0f, 0.0f, 0.0f));
      f.push_back(Vector3f(0.0f, 0.0f, 0.0f));
      continue;
    }
    Vector3f gravity = Vector3f(0.0f, g * mass, 0.0f);
    Vector3f drag = state[i+1] * -(mass * airDrag);
    Vector3f rope = Vector3f(0.0f, 0.0f, 0.0f);

    if(i > 0)
    {
      Vector3f d = state[i] - state[i-2];
      rope += -ropeK * (d.abs() - length) * (d.normalized());
    }
    if(i+2 < state.size())
    {
      Vector3f d = state[i] - state[i+2];
      rope += -ropeK * (d.abs() - length) * (d.normalized());
    }
    
    Vector3f sum = (gravity + drag + rope) / mass;
    
    f.push_back(state[i+1]);
    f.push_back((sum));
  }
	// YOUR CODE HERE

	return f;
}

// render the system (ie draw the particles)
void PendulumSystem::draw()
{
  vector<Vector3f> state = this->getState();
	for (size_t i = 0; i < state.size(); i+=2) {
		Vector3f pos = state[i] ;//  position of particle i. YOUR CODE HERE
		glPushMatrix();
		glTranslatef(pos[0], pos[1], pos[2] );
		glutSolidSphere(0.075f,10.0f,10.0f);
		glPopMatrix();
	}
}
