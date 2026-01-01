#include "Mesh.h"

using namespace std;

void Mesh::load( const char* filename )
{
	// 2.1.1. load() should populate bindVertices, currentVertices, and faces
  FILE* objFilePtr = fopen(filename, "r");
  char lineBuffer[256];
  if (objFilePtr == NULL)
  {
    cerr << "Could not open file";
    return;
  }

  while (fgets(lineBuffer, sizeof(lineBuffer), objFilePtr) != NULL)
  {
      
    switch (lineBuffer[0])
    {
      case 'v':
        if (lineBuffer[1] == ' ')
        {
          Vector3f vec;
          sscanf(lineBuffer, "v %f %f %f", &vec.x(), &vec.y(), &vec.z());
          
          bindVertices.push_back(vec);
        }
        break;
      case 'f':
        Tuple3u face;
        sscanf(lineBuffer, "f %u %u %u", &face[0], &face[1], &face[2]);
        face[0]--; face[1]--; face[2]--;
        faces.push_back(face);
        break;
    }

  }

  fclose(objFilePtr);
	currentVertices = bindVertices;
}

void Mesh::draw()
{
	// Since these meshes don't have normals
	// be sure to generate a normal per triangle.
	// Notice that since we have per-triangle normals
	// rather than the analytical normals from
	// assignment 1, the appearance is "faceted".
  glBegin(GL_TRIANGLES);
  for(auto face: this->faces)
  {
    
    Vector3f ab = this->currentVertices[face[1]] - this->currentVertices[face[0]];
    Vector3f ac = this->currentVertices[face[2]] - this->currentVertices[face[0]];
    Vector3f normal = Vector3f::cross(ab,ac).normalized();

    glNormal3d(normal.x(), normal.y(), normal.z());

    //
    glVertex3d(this->currentVertices[face[0]].x(), this->currentVertices[face[0]].y(), this->currentVertices[face[0]].z());

    glVertex3d(this->currentVertices[face[1]].x(), this->currentVertices[face[1]].y(), this->currentVertices[face[1]].z());
    
    glVertex3d(this->currentVertices[face[2]].x(), this->currentVertices[face[2]].y(), this->currentVertices[face[2]].z());

    
  }
  glEnd();
}

void Mesh::loadAttachments( const char* filename, int numJoints )
{
	// 2.2. Implement this method to load the per-vertex attachment weights
	// this method should update m_mesh.attachments
  FILE* objFilePtr = fopen(filename, "r");
  char lineBuffer[256];
  if (objFilePtr == NULL)
  {
    cerr << "Could not open file";
    return;
  }
  while (fgets(lineBuffer, sizeof(lineBuffer), objFilePtr) != NULL)
  {
    vector<float> weights(numJoints);
    std::stringstream ss(lineBuffer);
    weights[0] = 0.0f;
    for (int i = 1; i < numJoints; i++)
      ss >> weights[i];

    attachments.push_back(weights);
  }
  fclose(objFilePtr);
}
