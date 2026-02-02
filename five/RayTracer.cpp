#include "RayTracer.h"

Vector3f RayTracer::traceRay(Ray& r, SceneParser& scene, int depth, bool castShadows)
{
  float tmin = 10e-5;
  if(depth == 0)
    return Vector3f(0.0f, 0.0f, 0.0f);
  
  Vector3f ldir;
  Vector3f lcolor;
  Light* L;
  Hit h;
  float ambient = 0.1f;
  bool t = scene.getGroup()->intersect(r, h,tmin);
  if(t)
  {
    Vector3f finalColor = Vector3f(0.0f, 0.0f, 0.0f);
    Vector3f reflectionColor = Vector3f(0.0f, 0.0f, 0.0f);
    Vector3f refracColor = Vector3f(0.0f, 0.0f, 0.0f);

    Vector3f N = h.getNormal().normalized();
    Vector3f P = r.getOrigin() + r.getDirection() * h.getT();
    Vector3f D = (P - r.getOrigin()).normalized();
    

    float n = r.refraction;
    float nt = h.getMaterial()->getRefraction();

    float nDotDir = Vector3f::dot(N, D);

    if(nDotDir > 0)
    {
      N = -N;
      nt = 1.0f;
      nDotDir = Vector3f::dot(N, D);
    }
    Vector3f R = (D - 2.0f * (Vector3f::dot(D, N) * N)).normalized();
    Ray reflectionRay(P + (N * 0.001f), R);
    reflectionRay.refraction = n;

    

    float sqrtV = 1.0f - ( pow(n, 2.0f) * ( 1.0f - pow(nDotDir, 2.0f) ) )/pow(nt, 2.0f);
    
    float rContrib = 1.0f;
    if(sqrtV >= 0.0f && h.getMaterial()->getRefraction() > 0.0f)
    {
      Vector3f lPart = ( n * (D - N * nDotDir) )/nt;
      Vector3f rPart = N * sqrt(sqrtV);

      Vector3f refractionvec = lPart - rPart;
      Ray refracRay(P + refractionvec * tmin, refractionvec);
      refracRay.refraction = nt;

      refracColor = traceRay(refracRay, scene, depth-1, castShadows);

      float R0 = pow((nt - n) / (nt + n), 2.0f);
      float c;

      if(n <= nt)
        c = abs(nDotDir);
      else
        c = abs(Vector3f::dot(refractionvec, N));

    
      rContrib = R0 + (1.0f - R0) * pow(1.0f - c, 5.0f);
      //rContrib = pow(rContrib, h.getMaterial()->shininess);

      finalColor += refracColor * (1.0f - rContrib) * h.getMaterial()->getSpecular();
    }
    
    reflectionColor = traceRay(reflectionRay, scene, depth-1, castShadows) * h.getMaterial()->getSpecular();
    
    

    //material of hit must be the new ray refraction n to carry
    float distance = -1;
    for(size_t l = 0; l < scene.getNumLights();l++)
    {
      
      L = scene.getLight(l);

      L->getIllumination(P, ldir, lcolor, distance);

      if(castShadows)
      {
        Hit shadowHit = Hit(distance + tmin , NULL, NULL);
        Ray rShadow = Ray(P + ldir * tmin, ldir);

        bool tShadow = scene.getGroup()->intersect(rShadow, shadowHit, tmin);
      
        if(!tShadow || shadowHit.getT() > distance)
          finalColor += h.getMaterial()->Shade(r, h, ldir, lcolor, distance);
      }
      else
        finalColor += h.getMaterial()->Shade(r, h, ldir, lcolor, distance);
    }

    


    return  reflectionColor * rContrib  + finalColor + scene.getAmbientLight() * h.getMaterial()->getDiffuseColor();
  }
  else
    return scene.getBackgroundColor(r.getDirection());
}