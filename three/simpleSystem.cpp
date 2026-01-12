
#include "simpleSystem.h"
#include <iostream>  
using namespace std;

SimpleSystem::SimpleSystem()
{
}

// TODO: implement evalF
// for a given state, evaluate f(X,t)
vector<Vector3f> SimpleSystem::evalF(vector<Vector3f> state)
{
	vector<Vector3f> f;
  
  for(size_t i = 0;i < state.size(); i++)
  {
    f.push_back(Vector3f(-state[i].y(), state[i].x(), 0.0f));
  }

	return f;
}

// render the system (ie draw the particles)
void SimpleSystem::draw()
{
  vector<Vector3f> state = this->getState();
  for(size_t i = 0;i < state.size(); i++)
  {
		Vector3f pos   = state[i];//YOUR PARTICLE POSITION
	  glPushMatrix();
		glTranslatef(pos[0], pos[1], pos[2] );
		glutSolidSphere(0.075f,10.0f,10.0f);
		glPopMatrix();
  }
}
