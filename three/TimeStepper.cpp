#include "TimeStepper.hpp"

///TODO: implement Explicit Euler time integrator here
void ForwardEuler::takeStep(ParticleSystem* particleSystem, float stepSize)
{
  vector<Vector3f> currentState = particleSystem->getState();

  //derivada para o passo
  vector<Vector3f> derivative = particleSystem->evalF(currentState);

  for(size_t i = 0; i < currentState.size(); i++)
  {
    //Andando na direcao da derivada
    currentState[i] =  derivative[i] * stepSize + currentState[i];
  }

  particleSystem->setState(currentState);
}

///TODO: implement Trapzoidal rule here
void Trapzoidal::takeStep(ParticleSystem* particleSystem, float stepSize)
{
  vector<Vector3f> initialState = particleSystem->getState();
  vector<Vector3f> currentState = initialState;

  //derivada para primeiro passo
  vector<Vector3f> firstStepDerivatives = particleSystem->evalF(initialState);

  for(size_t i = 0; i < currentState.size(); i++)
  {
    //Andando na direcao da derivada
    currentState[i] =  firstStepDerivatives[i] * stepSize + initialState[i];
  }

  //Apos tomar o primeiro passo, calcular a derivada para o segundo passo
  vector<Vector3f> secondStepDerivatives = particleSystem->evalF(currentState);
  
  float halfStep = stepSize / 2.0f;
  for(size_t i = 0; i < currentState.size(); i++)
  {
    //media das derivadas de cada passo
    currentState[i] =  firstStepDerivatives[i] * halfStep + secondStepDerivatives[i] * halfStep + initialState[i];
  }

  particleSystem->setState(currentState);
}
