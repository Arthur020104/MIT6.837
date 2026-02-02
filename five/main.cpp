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
#include <thread>
#include <functional> 
#include <atomic>     
#include "bitmap_image.hpp"
#include "RayTracer.h"
using namespace std;

const unsigned int TOTAL_THREADS = std::thread::hardware_concurrency();
std::atomic<int> IN_USE_THREADS(0);

bool JITTER = false;
bool GAUSSIAN = false;
bool SHADOWS = false;
int BOUNCES = 0;

float clampedDepth ( float depthInput, float depthMin , float depthMax);

bool readInput(int &argc, char* argv[], char* &filename, char* &newFileName, int& W, int& H) ;

void getXy(const float &i, const float &j, const float &wf, const float &hf, const float &ratio, float &x, float &y);

Vector3f& safeImgAcess(Vector3f* imageBuffer, const int &W, const int &H, const int &i, const int &j);

void threadTraceRay(float i, int H, int W, float wf, float hf, float ratio, Vector3f* imageBuffer, SceneParser& sceneParser);

int main( int argc, char* argv[] )
{
  
  if(TOTAL_THREADS <= 0)//por agora so
  {
    std::cerr<<"Must support mutiple threads"<<std::endl;
    return -1;
  }
  
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

  
  int FW;
  int FH;
  if(JITTER)
  {
    W *= 3;
    H *= 3;
  }
  if(GAUSSIAN)
  {
    FW = W / 3.0f;
    FH = H / 3.0f;
  }
  else
  {
    FW = W;
    FH = H;
  }
  
  float ratio = (float)W/(float)H;
  float wf = 1.0f/((float)W - 1.0f);
  float hf = 1.0f/((float)H - 1.0f);


	SceneParser sceneParser(filename);
  sceneParser.getGroup()->loadBvh();


  Image image( FW , FH );
  Ray r = Ray(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 0.0f));

 
  Vector3f* imageBuffer = new Vector3f[W * H];

  for(float i = 0; i < W; i++)
  {
    while(IN_USE_THREADS >= TOTAL_THREADS) 
    {
      std::this_thread::yield();
    }

    IN_USE_THREADS++;
    std::thread t1(threadTraceRay, i, H, W, wf, hf, ratio, imageBuffer, std::ref(sceneParser));
    t1.detach();
  }
  while(IN_USE_THREADS > 0) 
  {
    std::this_thread::yield();
  }
  

  float K[5] = {0.1201f, 0.2339f, 0.2931f, 0.2339f, 0.1201f};
  for(int i = 0; i < W; i++)
  {
    for(int j = 0; j < H; j++)
    { 
      
      if(GAUSSIAN)
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
  if(GAUSSIAN)
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
    if(strcmp("-jitter", argv[argNum]) == 0)
      JITTER = true;
    if(strcmp("-filter", argv[argNum]) == 0)
      GAUSSIAN = true;
    if(strcmp("-bounces", argv[argNum]) == 0)
      BOUNCES = atoi(argv[argNum+1]);
    if(strcmp("-shadows", argv[argNum]) == 0)
      SHADOWS = true;
  }

  if(W == -1 || H == -1 || filename == nullptr || newFileName == nullptr)
  {
    std::cout << "Please execute the program like: ./a4 -input ./someScene.txt -size <width> <height> -output <outputName>.bmp" << std::endl;
    return false;
  }
  return true;
}

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
void threadTraceRay(float i, int H, int W, float wf, float hf, float ratio, Vector3f* imageBuffer, SceneParser& sceneParser)
{
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> rndDist(-0.5, 0.5);
  for(float j = 0; j < H; j++)
  {
    Hit h = Hit();
    float x, y;
    
    if(JITTER)
      getXy(i + rndDist(gen), j + rndDist(gen), wf, hf, ratio, x, y);
    else
      getXy(i, j, wf, hf, ratio, x, y);
    Ray r = sceneParser.getCamera()->generateRay(Vector2f(x, -y));

    Vector3f finalColor = RayTracer::traceRay(r, sceneParser,  1 + BOUNCES, SHADOWS );

    imageBuffer[(int)(i + j * W)] = finalColor;
  }
  IN_USE_THREADS --;

}