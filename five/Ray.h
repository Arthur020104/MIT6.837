#ifndef RAY_H
#define RAY_H

#include <cassert>
#include <iostream>
#include <Vector3f.h>

using namespace std;

// Ray class mostly copied from Peter Shirley and Keith Morley
class Ray
{
public:
    
    Ray( const Vector3f& orig, const Vector3f& dir )
    {
        origin = orig; 
        direction = dir;
        invDrx = 1.0f/dir.x();
        invDry = 1.0f/dir.y();
        invDrz = 1.0f/dir.z();
        refraction = 1.0f;
    }
    Ray( const Vector3f& orig, const Vector3f& dir, float refraction )
    {
        origin = orig; 
        direction = dir;
        invDrx = 1.0f/dir.x();
        invDry = 1.0f/dir.y();
        invDrz = 1.0f/dir.z();
        refraction = refraction;
    }
    Ray( const Ray& r )
    { 
        origin = r.origin;
        direction = r.direction;
        invDrx = r.invDrx;
        invDry = r.invDry;
        invDrz = r.invDrz;
        refraction = r.refraction;
    }

    const Vector3f& getOrigin() const
    {
        return origin;
    }

    const Vector3f& getDirection() const
    {
        return direction;
    }
    
    Vector3f pointAtParameter( float t ) const
    {
        return origin + direction * t;
    }
  
  float invDrx;
  float invDry;
  float invDrz;
  float refraction;
private:

    // don't use this constructor
    Ray()
    {
        assert( false );
    }

    Vector3f origin;
    Vector3f direction;
    
    

};

inline ostream& operator << ( ostream& os, const Ray& r)
{
    os << "Ray <" << r.getOrigin() << ", " << r.getDirection() << ">";
    return os;
}

#endif // RAY_H
