#include "SkeletalModel.h"

#include <FL/Fl.H>

using namespace std;

void SkeletalModel::load(const char *skeletonFile, const char *meshFile, const char *attachmentsFile)
{
	loadSkeleton(skeletonFile);

	m_mesh.load(meshFile);
	m_mesh.loadAttachments(attachmentsFile, m_joints.size());

	computeBindWorldToJointTransforms();
	updateCurrentJointToWorldTransforms();
}

void SkeletalModel::draw(Matrix4f cameraMatrix, bool skeletonVisible)
{
	// draw() gets called whenever a redraw is required
	// (after an update() occurs, when the camera moves, the window is resized, etc)

	m_matrixStack.clear();
	m_matrixStack.push(cameraMatrix);

	if( skeletonVisible )
	{
		drawJoints();

		drawSkeleton();
	}
	else
	{
		// Clear out any weird matrix we may have been using for drawing the bones and revert to the camera matrix.
		glLoadMatrixf(m_matrixStack.top());

		// Tell the mesh to draw itself.
		m_mesh.draw();
	}
}

void SkeletalModel::loadSkeleton( const char* filename )
{
	// Load the skeleton from file here.

  FILE* objFilePtr = fopen(filename, "r");
  char lineBuffer[256];
  if (objFilePtr == NULL)
  {
    cerr << "Could not open file";
    return;
  }
  

  while (fgets(lineBuffer, sizeof(lineBuffer), objFilePtr) != NULL)
  {
    float x, y, z;
    int parentIndex;
    Vector4f column3(0, 0, 0, 1);
    sscanf(lineBuffer, "%f %f %f %d", &column3.x(), &column3.y(), &column3.z(), &parentIndex);
    
    Joint* j = new Joint;

    if(parentIndex != -1)
    {
      m_joints[parentIndex]->children.push_back(j);
    }
    else
    {
      m_rootJoint = j;
    }
    m_joints.push_back(j);

    Matrix4f transform = Matrix4f::identity();
    transform.setCol(3, column3);

    j->transform = transform;
  }

  fclose(objFilePtr);
}
void SkeletalModel::drawJointHierarchy(Joint* j, Matrix4f transform)
{
  this->m_matrixStack.push(j->transform);

  Matrix4f result = transform * j->transform;

  for(Joint* child : j->children)
  {
    drawJointHierarchy(child, result);
  }
  
  glLoadMatrixf(result);
  glutSolidSphere(0.025f, 12, 12 );

  this->m_matrixStack.pop();
}
void SkeletalModel::drawJoints( )
{
	// Draw a sphere at each joint. You will need to add a recursive helper function to traverse the joint hierarchy.
	//
	// We recommend using glutSolidSphere( 0.025f, 12, 12 )
	// to draw a sphere of reasonable size.
	//
  /*int i = 3;
  while(i>0)
  {
    std::cout<<"i: "<<i<<std::endl;
    m_matrixStack.top().print();
    m_matrixStack.pop();
    i--;
  }
  return;*/
  this->drawJointHierarchy(m_rootJoint, m_matrixStack.top());
	// You are *not* permitted to use the OpenGL matrix stack commands
	// (glPushMatrix, glPopMatrix, glMultMatrix).
	// You should use your MatrixStack class
	// and use glLoadMatrix() before your drawing call.
}
void SkeletalModel::drawSkeletonHierarchy(Joint* j, Matrix4f transform)
{
  this->m_matrixStack.push(j->transform);

  Matrix4f result = transform * j->transform;

  for(Joint* child : j->children)
  {
    drawSkeletonHierarchy(child, result);
  }
  if(j == this->m_rootJoint)
    return;
  
  Matrix4f translation = Matrix4f::translation(0.0f, 0.0f, 0.5f);
  Vector4f v = j->transform.getCol(3);
  float l = sqrt(v.x() * v.x() + v.y() * v.y() + v.z() * v.z());

  Matrix4f scale = Matrix4f::scaling(0.025f, 0.025f, l);

  Vector3f z(v.x(),v.y(),v.z()); z.normalize();
  
  Vector3f rnd(0.0f, 0.0f, 1.0f);
  Vector3f y = Vector3f::cross(z, rnd).normalized();
  Vector3f x = Vector3f::cross(y, z).normalized();

  Matrix4f rotation(Vector4f(x, 0.0f), Vector4f(y, 0.0f), Vector4f(z, 0.0f), Vector4f(0.0f, 0.0f, 0.0f, 1.0f));

  Matrix4f r = transform * rotation * scale * translation;
  glLoadMatrixf(r);
  glutSolidCube( 1.0f );

  this->m_matrixStack.pop();
}
void SkeletalModel::drawSkeleton( )
{
	// Draw boxes between the joints. You will need to add a recursive helper function to traverse the joint hierarchy.
  this->drawSkeletonHierarchy(m_rootJoint, m_matrixStack.top());
}

void SkeletalModel::setJointTransform(int jointIndex, float rX, float rY, float rZ)
{
	// Set the rotation part of the joint's transformation matrix based on the passed in Euler angles.
  m_joints[jointIndex]->transform.setSubmatrix3x3(0, 0, (Matrix4f::rotateZ(rZ) * Matrix4f::rotateY(rY) * Matrix4f::rotateX(rX)).getSubmatrix3x3(0,0));
}

void SkeletalModel::computeBindWorldToJointTransformsHelper(Joint* j, Matrix4f transform)
{
  Matrix4f result = transform * j->transform;

  for(Joint* child : j->children)
  {
    computeBindWorldToJointTransformsHelper(child, result);
  }
  
  j->bindWorldToJointTransform = result.inverse();
}
void SkeletalModel::computeBindWorldToJointTransforms()
{
	// 2.3.1. Implement this method to compute a per-joint transform from
	// world-space to joint space in the BIND POSE.
	//
	// Note that this needs to be computed only once since there is only
	// a single bind pose.
  this->computeBindWorldToJointTransformsHelper(m_rootJoint, Matrix4f::identity());
	// This method should update each joint's bindWorldToJointTransform.
	// You will need to add a recursive helper function to traverse the joint hierarchy.
}
void SkeletalModel::updateCurrentJointToWorldTransformsHelper(Joint* j, Matrix4f transform)
{

  Matrix4f result = transform * j->transform;

  for(Joint* child : j->children)
  {
    updateCurrentJointToWorldTransformsHelper(child, result);
  }
  
  j->currentJointToWorldTransform = result;
}
void SkeletalModel::updateCurrentJointToWorldTransforms()
{
	// 2.3.2. Implement this method to compute a per-joint transform from
	// joint space to world space in the CURRENT POSE.
	//
	// The current pose is defined by the rotations you've applied to the
	// joints and hence needs to be *updated* every time the joint angles change.
  this->updateCurrentJointToWorldTransformsHelper(m_rootJoint, Matrix4f::identity());
	//
	// This method should update each joint's bindWorldToJointTransform.
	// You will need to add a recursive helper function to traverse the joint hierarchy.
}
Vector3f SkeletalModel::interpolateHelper(Vector3f& v, std::vector<float>& w)
{
  const Vector4f vInstance(v.x(), v.y(), v.z(), 1.0f);
  Vector4f r(0,0,0,0);
  

  for(size_t i = 0; i < this->m_joints.size(); i++)
  {
    if(w[i] < 0.001f)
    {
      continue;
    }
    //Sum w * t_j_current * t_j_bind_inverse * v
    Vector4f instance = m_joints[i]->currentJointToWorldTransform * (m_joints[i]->bindWorldToJointTransform * vInstance);

    r = r + (instance * w[i]);
  }

  return Vector3f(r.x(), r.y(), r.z());
}
void SkeletalModel::updateMesh()
{
	// 2.3.2. This is the core of SSD.
	// Implement this method to update the vertices of the mesh
	// given the current state of the skeleton
  m_mesh.currentVertices.clear();
  for(size_t i = 0;i < m_mesh.bindVertices.size(); i++)
  {
    m_mesh.currentVertices.push_back(interpolateHelper(m_mesh.bindVertices[i], m_mesh.attachments[i]));
  }
	// You will need both the bind pose world --> joint transforms.
	// and the current joint --> world transforms.
}

