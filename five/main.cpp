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
#include <random>
#include "bitmap_image.hpp"
#include "RayTracer.h"
using namespace std;

float clampedDepth ( float depthInput, float depthMin , float depthMax);
bool readInput(int &argc, char* argv[], char* &filename, char* &newFileName, int& W, int& H) ;


const float CA = 1.1f;

void getXy(const float &i, const float &j, const float &wf, const float &hf, const float &ratio, float &x, float &y)
{
  float xr = i * wf;
  float yr = j * hf;
  x = (xr - (1.0f - xr)) * ratio;
  y = yr - (1.0f - yr);
}
Vector3f& safeImgAcess(Vector3f* imageBuffer, const int &W, const int &H, const int &i, const int &j)
{
  int iIdx = i;
  int jIdx = j;
  if(i >= W)
    iIdx = W-1;
  else if(i < 0)
    iIdx = 0;
  
  if(j >= H)
    jIdx = H-1;
  else if(j < 0)
    jIdx = 0;

  return imageBuffer[iIdx + jIdx * W];
}
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

  bool jitter = true;
  int FW;
  int FH;
  if(jitter)
  {
    FW = W;
    FH = H;//chagnge afther just for jitter
    W *= 3;
    H *= 3;
  }
  
  float ratio = (float)W/(float)H;
  float wf = 1.0f/((float)W - 1.0f);
  float hf = 1.0f/((float)H - 1.0f);


	SceneParser sceneParser(filename);
  sceneParser.getGroup()->loadBvh();


  Image image( FW , FH );
  Ray r = Ray(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 0.0f));

 
  // Vector3f ldir;
  // Vector3f lcolor;
  // Light* L;
  // Vector3f p;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> rndDist(-0.5, 0.5);

  float atenuation;
  Vector3f* imageBuffer = new Vector3f[W * H];

  Vector3f ambient = CA * sceneParser.getAmbientLight();
  for(float i = 0; i < W; i++)
  {
    for(float j = 0; j < H; j++)
    {
      Hit h = Hit();
      float x, y;

      getXy(i + rndDist(gen), j + rndDist(gen), wf, hf, ratio, x, y);    
      r = sceneParser.getCamera()->generateRay(Vector2f(x, -y));
      Vector3f finalColor = RayTracer::traceRay(r, sceneParser,  5 );//1 + bounces

      imageBuffer[(int)(i + j * W)] = finalColor;
    }
  }
  bool gaussian = true;

  float K[5] = {0.1201f, 0.2339f, 0.2931f, 0.2339f, 0.1201f};
  for(int i = 0; i < W; i++)
  {
    for(int j = 0; j < H; j++)
    { 
      
      if(gaussian)
      { 
        imageBuffer[(int)(i + j * W)] = safeImgAcess(imageBuffer, W, H, i, j - 2) * K[0]
        + safeImgAcess(imageBuffer, W, H, i, j - 1) * K[1]
        + safeImgAcess(imageBuffer, W, H, i, j) * K[2]
        + safeImgAcess(imageBuffer, W, H, i, j + 1) * K[3]
        + safeImgAcess(imageBuffer, W, H, i, j + 2) * K[4];
      }
      else
      {
        Vector3f finalColor = safeImgAcess(imageBuffer, W, H, i, j);
        image.SetPixel(i, j, finalColor);
      }
      
    }
  }
  gaussian = true;
  if(gaussian)
  {
    //downsample
    Vector3f* img = new Vector3f[FW * FH];
    for(int i = 0; i < FW; i++)
    {
      for(int j = 0; j < FH; j++)
      {
        Vector3f finalColor;
        finalColor += safeImgAcess(imageBuffer, W, H, i * 3, j * 3);
        finalColor += safeImgAcess(imageBuffer, W, H, i * 3 + 1, j * 3);
        finalColor += safeImgAcess(imageBuffer, W, H, i * 3, j * 3 + 1);
        finalColor += safeImgAcess(imageBuffer, W, H, i * 3 - 1, j * 3);
        finalColor += safeImgAcess(imageBuffer, W, H, i * 3, j * 3 - 1);

        finalColor = finalColor / 5.0f;
        image.SetPixel(i, j, finalColor);
      }
    }
    delete[] img;
  }

  image.SaveImage(newFileName);


  delete[] imageBuffer;
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

