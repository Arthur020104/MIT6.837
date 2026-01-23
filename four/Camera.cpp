#include "Ray.h"
#include <vecmath.h>
#include <float.h>
#include <cmath>
#include "Camera.h"
Matrix4f PerspectiveCamera::generateLookAt(const Vector3f& center, const Vector3f& direction,const Vector3f& up)
{
  Vector3f w = -direction.normalized();//talvez negativo
  Vector3f u = Vector3f::cross(up, w).normalized();
  Vector3f v = Vector3f::cross(u, w).normalized();

  Matrix4f rot;
  rot.setRow(0, Vector4f(u, 0.0f));//transposta da rot, msm que inversa pq eh ortogonald
  rot.setRow(1, Vector4f(v, 0.0f));
  rot.setRow(2, Vector4f(w, 0.0f));
  rot.setRow(3, Vector4f(0.0f, 0.0f, 0.0f, 1.0f));

  Matrix4f transl = Matrix4f::identity();// A part -center corresponda a inversa da matrix de translacao
  transl.setCol(3, Vector4f(center, 1.0f));

  return rot * transl;
  
}

PerspectiveCamera::PerspectiveCamera(const Vector3f& center, const Vector3f& direction,const Vector3f& up , float angle){

  this->center = center;
  this->direction = direction;
  this->up = up;
  
  lookAtMatrix = generateLookAt(center, direction, up);

  lookAtMatrixInv = lookAtMatrix.inverse();
  

  float scale = 1.0f / tan(0.5f * angle);
  Vector3f scaleV = Vector3f(scale, scale, 1.0f).normalized();
  perspective(0, 0) = scaleV.x();
  perspective(1, 1) = scaleV.y();
  perspective(2, 2) = scaleV.z();
  perspective(3, 2) = 0;
  perspective(2, 3) = 0;
  perspective(3, 3) = 1;
  
  final = perspective * lookAtMatrix;
  final = final.inverse();
}

Ray PerspectiveCamera::generateRay( const Vector2f& point){

  Vector4f p = (final * Vector4f(point, -1.0f, 0.0f)).normalized();

  return Ray(center, p.xyz());

}

// float PerspectiveCamera::getTMin() const { 
//   return 0.0f;
// }