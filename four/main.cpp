#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>

#include "SceneParser.h"
#include "Image.h"
#include "Camera.h"
#include <string.h>
#include "bitmap_image.hpp"
using namespace std;

float clampedDepth ( float depthInput, float depthMin , float depthMax);
bool readInput(int &argc, char* argv[], char* &filename, char* &newFileName, int& W, int& H) ;


const float CA = 1.1f;
int main( int argc, char* argv[] )
{
  // Fill in your implementation here.

  // This loop loops over each of the input arguments.
  // argNum is initialized to 1 because the first
  // "argument" provided to the program is actually the
  // name of the executable (in our case, "a4").
  int H = -1, W = -1;
  char* filename = nullptr;
  char* newFileName = nullptr;
  
  if(!readInput(argc, argv, filename, newFileName, W, H))
    return 0;

  float ratio = (float)W/(float)H;
  float wf = 1.0f/((float)W - 1.0f);
  float hf = 1.0f/((float)H - 1.0f);
	SceneParser sceneParser(filename);
  sceneParser.getGroup()->loadBvh();


  Image image( W , H );
  Ray r = Ray(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 0.0f));

 
  Vector3f ldir;
  Vector3f lcolor;
  Light* L;
  Vector3f p;
  
  float atenuation;
  

  Vector3f ambient = CA * sceneParser.getAmbientLight();
  for(float i = 0; i < W; i++)
  {
    for(float j = 0; j < H; j++)
    {
      Hit h = Hit();
      
      float xr = i * wf;
      float yr = j * hf;
      float x = (xr - (1.0f - xr)) * ratio;
      float y = yr - (1.0f - yr);

      r = sceneParser.getCamera()->generateRay(Vector2f(x, -y));

      bool t = sceneParser.getGroup()->intersect(r, h,sceneParser.getCamera()->getTMin());
      if(t)
      {
        Vector3f finalColor = Vector3f(0.0f, 0.0f, 0.0f);
        float distance = -1;
        for(size_t l = 0; l < sceneParser.getNumLights();l++)
        {
          L = sceneParser.getLight(l);
          p = r.getOrigin() + r.getDirection() * h.getT();

          L->getIllumination(p, ldir, lcolor, distance);
          finalColor += h.getMaterial()->Shade(r, h, ldir, lcolor, distance);
        }

        image.SetPixel( i,j, finalColor + ambient * h.getMaterial()->getDiffuseColor(h));
      }
      else
        image.SetPixel( i,j, sceneParser.getBackgroundColor());
    }
  }
  image.SaveImage(newFileName);


  
  return 0;
}

bool readInput(int &argc, char* argv[], char* &filename, char* &newFileName, int& W, int& H) 
{
  for( int argNum = 1; argNum < argc; ++argNum )
  {
    std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
    if(strcmp("-input", argv[argNum]) == 0)
      filename = argv[argNum+1];
    if(strcmp("-size", argv[argNum]) == 0)
    {
      W = atoi(argv[argNum+1]);
      H = atoi(argv[argNum+2]);
    }
    if(strcmp("-output", argv[argNum]) == 0)
      newFileName = argv[argNum+1];
  }

  if(W == -1 || H == -1 || filename == nullptr || newFileName == nullptr)
  {
    std::cout << "Please execute the program like: ./a4 -input ./someScene.txt -size <width> <height> -output <outputName>.bmp" << std::endl;
    return false;
  }
  return true;
}

