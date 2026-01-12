#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>
#include <stdexcept> 

#include <GL/glut.h>
#include <vecmath.h>
#include "camera.h"


///TODO: include more headers if necessary

#include "TimeStepper.hpp"
#include "simpleSystem.h"
#include "pendulumSystem.h"
#include "ClothSystem.h"
using namespace std;

// Globals here.
namespace
{

  ParticleSystem *system;
  TimeStepper * timeStepper;
  Vector4f origin;
  Vector4f direction;
  unsigned int W, H;
  // initialize your particle systems
  ///TODO: read argv here. set timestepper , step size etc

  float STEP = 0.00075f;//Maximo que meu pc aguenta, se necessario aumente use 0.001 ou no maximo 0.0015
  void initSystem(int argc, char * argv[])
  {
    // seed the random number generator with the current time
    srand( time( NULL ) );
    if(argc < 2)
    {
      throw std::runtime_error("Provide an arg use 'e' for euler, 't' for trapzoidal or 'r' for RK4. \n");
    }

    system = new ClothSystem(10,10);
    
    
    switch (argv[1][0])
    {
      case 'r':
        timeStepper = new RK4();
        break;
      case 'e':
        timeStepper = new ForwardEuler();
      case 't':
        timeStepper = new Trapzoidal();
      default:
        break;
    }

    if(argc >= 3)
    {
      STEP = static_cast<float>(std::atof(argv[2]));
    }
  }

  // Take a step forward for the particle shower
  ///TODO: Optional. modify this function to display various particle systems
  ///and switch between different timeSteppers
  void stepSystem()
  {
      ///TODO The stepsize should change according to commandline arguments
    int loopAmount = (int)(0.04f / STEP);
    if(timeStepper!=0){
      while(loopAmount > 0)
      {
        timeStepper->takeStep(system,STEP);
        loopAmount--;
      }
      
    }
  }

  // Draw the current particle positions
  void drawSystem()
  {
    
    // Base material colors (they don't change)
    GLfloat particleColor[] = {0.4f, 0.7f, 1.0f, 1.0f};
    GLfloat floorColor[] = {1.0f, 0.0f, 0.0f, 1.0f};
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, particleColor);
    
    //glutSolidSphere(0.1f,10.0f,10.0f);
    
    system->draw();
    
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, floorColor);
    glPushMatrix();
    glTranslatef(0.0f,-5.0f,0.0f);
    glScaled(50.0f,0.01f,50.0f);
    glutSolidCube(1);
    glPopMatrix();
    
  }
        

    //-------------------------------------------------------------------
    
        
    // This is the camera
    Camera camera;

    // These are state variables for the UI
    bool g_mousePressed = false;

    // Declarations of functions whose implementations occur later.
    void arcballRotation(int endX, int endY);
    void keyboardFunc( unsigned char key, int x, int y);
    void specialFunc( int key, int x, int y );
    void mouseFunc(int button, int state, int x, int y);
    void motionFunc(int x, int y);
    void reshapeFunc(int w, int h);
    void drawScene(void);
    void initRendering();

    // This function is called whenever a "Normal" key press is
    // received.
    void keyboardFunc( unsigned char key, int x, int y )
    {
        switch ( key )
        {
        case 27: // Escape key
            exit(0);
            break;
        case ' ':
        {
            Matrix4f eye = Matrix4f::identity();
            camera.SetRotation( eye );
            camera.SetCenter( Vector3f::ZERO );
            break;
        }
        case 'a':
        {
          PendulumSystem* ps = dynamic_cast<PendulumSystem*>(system);
          if(ps)
          {
            vector<Vector3f> state = ps->getState();
            state[state.size()-1] += Vector3f(-30.0f,0.0f,0.0f);
            ps->setState(state);
          }
          break;
        }
        case 'd':
        {
          PendulumSystem* ps = dynamic_cast<PendulumSystem*>(system);
          if(ps)
          {
            vector<Vector3f> state = ps->getState();
            state[state.size()-1] += Vector3f(  30.0f,0.0f,0.0f);
            ps->setState(state);
          }
          break;
        }
        case 'r':
        {
          ClothSystem* ch = dynamic_cast<ClothSystem*>(system);
          if(ch)
          {
            delete(system);
            system = new ClothSystem(10,10);
          }
          break;
        }
        case 's':
        {
          ClothSystem* ch = dynamic_cast<ClothSystem*>(system);
          if(ch)
          {
            ch->drawState = (ch->drawState + 1) % 3;
          }
          break;
        }
        case 'h':
        {
          ClothSystem* ch = dynamic_cast<ClothSystem*>(system);
          if(ch)
          {
            system = new PendulumSystem(30);
          }
          else
          {
            system = new ClothSystem(10,10);
          }
          break;
        }
        default:
            cout << "Unhandled key press " << key << "." << endl;        
        }

        glutPostRedisplay();
    }

    // This function is called whenever a "Special" key press is
    // received.  Right now, it's handling the arrow keys.
    void specialFunc( int key, int x, int y )
    {
        switch ( key )
        {

        }
        //glutPostRedisplay();
    }

    //  Called when mouse button is pressed.
    void mouseFunc(int button, int state, int x, int y)
    {
        if (state == GLUT_DOWN)
        {
            g_mousePressed = true;

            float ndcX = (float)x /(float)W;
            float normalizedX = -1 * (1 - ndcX) + 1 * ndcX;

            float ndcY = (float)y/(float)H;
            float normalizedY = -1*(1-ndcY) + 1 * ndcY;

            GLfloat modelview[16];
            glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
            Matrix4f m = Matrix4f(modelview[0], modelview[1], modelview[2], modelview[3],
                      modelview[4], modelview[5], modelview[6], modelview[7],
                      modelview[8], modelview[9], modelview[10], modelview[11],
                      modelview[12], modelview[13], modelview[14], modelview[15]);
            Vector4f v = (m* Vector4f(0.0f, 0.0f, camera.GetDistance()/2.0f, 1.0f));
            
            origin = v;
          
            Vector4f t = Vector4f(Vector3f(normalizedX, -normalizedY, -1.0f).normalized(), 1.0f);
            direction = (m * t);
            
            ClothSystem* ch = dynamic_cast<ClothSystem*>(system);
            if(ch)
              ch->addForceToClosestVertex(origin, direction, direction.xyz() * 20.0f);
            
            switch (button)
            {
            case GLUT_LEFT_BUTTON:
                camera.MouseClick(Camera::LEFT, x, y);
                break;
            case GLUT_MIDDLE_BUTTON:
                camera.MouseClick(Camera::MIDDLE, x, y);
                break;
            case GLUT_RIGHT_BUTTON:
                camera.MouseClick(Camera::RIGHT, x,y);
            default:
                break;
            }                       
        }
        else
        {
            camera.MouseRelease(x,y);
            g_mousePressed = false;
        }
        glutPostRedisplay();
    }

    // Called when mouse is moved while button pressed.
    void motionFunc(int x, int y)
    {
        camera.MouseDrag(x,y);        
    
        glutPostRedisplay();
    }

    // Called when the window is resized
    // w, h - width and height of the window in pixels.
    void reshapeFunc(int w, int h)
    {
        W = w;
        H = h;
        camera.SetDimensions(w,h);

        camera.SetViewport(0,0,w,h);
        camera.ApplyViewport();

        // Set up a perspective view, with square aspect ratio
        glMatrixMode(GL_PROJECTION);

        camera.SetPerspective(50);
        glLoadMatrixf( camera.projectionMatrix() );
    }

    // Initialize OpenGL's rendering modes
    void initRendering()
    {
        glEnable(GL_DEPTH_TEST);   // Depth testing must be turned on
        glEnable(GL_LIGHTING);     // Enable lighting calculations
        glEnable(GL_LIGHT0);       // Turn on light #0.

        glEnable(GL_NORMALIZE);

        // Setup polygon drawing
        glShadeModel(GL_SMOOTH);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        // Clear to black
        glClearColor(0,0,0,1);
    }

    // This function is responsible for displaying the object.
    void drawScene(void)
    {
        // Clear the rendering window
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode( GL_MODELVIEW );  
        glLoadIdentity();              

        // Light color (RGBA)
        GLfloat Lt0diff[] = {1.0,1.0,1.0,1.0};
        GLfloat Lt0pos[] = {3.0,3.0,5.0,1.0};
        glLightfv(GL_LIGHT0, GL_DIFFUSE, Lt0diff);
        glLightfv(GL_LIGHT0, GL_POSITION, Lt0pos);

        glLoadMatrixf( camera.viewMatrix() );

        // THIS IS WHERE THE DRAW CODE GOES.

        drawSystem();

        // This draws the coordinate axes when you're rotating, to
        // keep yourself oriented.
        if( g_mousePressed )
        {
            glPushMatrix();
            Vector3f eye = camera.GetCenter();
            glTranslatef( eye[0], eye[1], eye[2] );

            // Save current state of OpenGL
            glPushAttrib(GL_ALL_ATTRIB_BITS);

            // This is to draw the axes when the mouse button is down
            glDisable(GL_LIGHTING);
            glLineWidth(3);
            glPushMatrix();
            glScaled(5.0,5.0,5.0);
            glBegin(GL_LINES);
            glColor4f(1,0.5,0.5,1); glVertex3f(0,0,0); glVertex3f(1,0,0);
            glColor4f(0.5,1,0.5,1); glVertex3f(0,0,0); glVertex3f(0,1,0);
            glColor4f(0.5,0.5,1,1); glVertex3f(0,0,0); glVertex3f(0,0,1);

            glColor4f(0.5,0.5,0.5,1);
            glVertex3f(0,0,0); glVertex3f(-1,0,0);
            glVertex3f(0,0,0); glVertex3f(0,-1,0);
            glVertex3f(0,0,0); glVertex3f(0,0,-1);

            glEnd();
            glPopMatrix();

            glPopAttrib();
            glPopMatrix();
        }
        // glLineWidth(10);
        // glBegin(GL_LINES);
        // glPushMatrix();
        // glVertex3fv(origin.xyz()); 
        
        // glVertex3fv(direction.xyz() * 100);
        // glPopMatrix();
        // glEnd();
        // Dump the image to the screen.
        glutSwapBuffers();
    }

    void timerFunc(int t)
    {
        stepSystem();

        glutPostRedisplay();

        glutTimerFunc(t, &timerFunc, t);
    }

    

    
    
}

// Main routine.
// Set up OpenGL, define the callbacks and start the main loop
int main( int argc, char* argv[] )
{
    glutInit( &argc, argv );

    // We're going to animate it, so double buffer 
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

    // Initial parameters for window position and size
    glutInitWindowPosition( 0, 0 );
    glutInitWindowSize( 1000, 1000 );
    
    camera.SetDimensions( 1000, 1000 );

    camera.SetDistance( 10 );
    camera.SetCenter( Vector3f::ZERO );
    
    glutCreateWindow("Assignment 4");

    // Initialize OpenGL parameters.
    initRendering();

    // Setup particle system
    initSystem(argc,argv);

    // Set up callback functions for key presses
    glutKeyboardFunc(keyboardFunc); // Handles "normal" ascii symbols
    glutSpecialFunc(specialFunc);   // Handles "special" keyboard keys

    // Set up callback functions for mouse
    glutMouseFunc(mouseFunc);
    glutMotionFunc(motionFunc);

    // Set up the callback function for resizing windows
    glutReshapeFunc( reshapeFunc );

    // Call this whenever window needs redrawing
    glutDisplayFunc( drawScene );

    // Trigger timerFunc every 20 msec
    glutTimerFunc(2000, timerFunc, 20);

        
    // Start the main loop.  glutMainLoop never returns.
    glutMainLoop();

    return 0;	// This line is never reached.
}
