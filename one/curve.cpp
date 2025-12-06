#include "curve.h"
#include "extra.h"
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
using namespace std;

namespace
{
    // Approximately equal to.  We don't want to use == because of
    // precision issues with floating point.
    inline bool approx(const Vector3f& lhs, const Vector3f& rhs)
    {
        const float eps = 1e-8f;
        return (lhs - rhs).absSquared() < eps;
    }

}

CurvePoint simpleDeCasteljau(float t, const Vector3f* controlPoints)
{ 
    
    Vector3f p01 = controlPoints[0] * (1 - t) + controlPoints[1] * t;
    Vector3f p02 = controlPoints[1] * (1 - t) + controlPoints[2] * t;
    Vector3f p03 = controlPoints[2] * (1 - t) + controlPoints[3] * t;

    Vector3f p11 = p01 * (1 - t) + p02 * t;
    Vector3f p12 = p02 * (1 - t) + p03 * t;


    Vector3f p21 = p11 * (1 - t) + p12 * t;


    CurvePoint finalPoint;
    finalPoint.V = p21;
    Vector3f Tv = 3 * (p12 - p11);
    finalPoint.T = Tv.normalized();

    return finalPoint;

}
CurvePoint bSplineEval(float t, const Vector3f* controlPoints)//fiz somente para aprendizado
{
    const Matrix4f mBSpline(
        -1.0f/6.0f, 3.0f/6.0f, -3.0f/6.0f, 1.0f/6.0f,
        3.0f/6.0f, -6.0f/6.0f, 3.0f/6.0f, 0,
        -3.0f/6.0f, 0, 3.0f/6.0f, 0,
        1.0f/6.0f, 4.0f/6.0f,1.0f/6.0f, 0
    );
    //4x3 matrix
    const Matrix4f geometricM(
		controlPoints[0].x(), controlPoints[0].y(), controlPoints[0].z(), 0,
		controlPoints[1].x(), controlPoints[1].y(), controlPoints[1].z(), 0,
		controlPoints[2].x(), controlPoints[2].y(), controlPoints[2].z(), 0,
		controlPoints[3].x(), controlPoints[3].y(), controlPoints[3].z(), 0
    );

	const Vector4f monomialBasis(t * t * t, t * t, t, 1);

    Matrix4f interm;
    //r-> row, c-> column, e-> element
    for (int r = 0; r < 4; r++)
    {
        for (int c = 0; c < 3; c++)
        {
            for (int e = 0; e < 4; e++)
            {
                interm(r, c) += geometricM(e,c) * mBSpline(r, e);
            }
        }
    }

    Vector4f result(0, 0, 0, 0);
    for (int c = 0; c < 4; c++)
    {
        for (int e = 0; e < 4; e++)
        {
            result[c] += monomialBasis[e] * interm(e, c);
        }
    }

    CurvePoint resultF;
    resultF.V = Vector3f(result.x(), result.y(), result.z());
    
    return resultF;
}
//Poderia tentar fazer tudo em um funcao para melhor performance, mas como um conceito que ainda estou tentando entender melhor, acho que por clareza vale a pena deixar separado
void computeTangents(vector<CurvePoint>& points, float dt)
{
    for (int j = 0; j < points.size(); j++)//calculo das tangentes
    {
        if (j == 0 && j + 1 < points.size())
        {
            Vector3f tan = (points[j + 1].V - points[j].V) / dt;
            points[j].T = tan.normalized();
            continue;
        }
        else if (j + 1 == points.size() && points.size() > 1)
        {
            Vector3f tan = (points[j].V - points[j - 1].V) / dt;
            points[j].T = tan.normalized();
            continue;
        }

        Vector3f tan = (points[j + 1].V - points[j - 1].V) / (2 * dt);
        points[j].T = tan.normalized();

    }
}
void computeNormalsAndBinormals(vector<CurvePoint>& points)
{
    //Asumindo aqui que as tangentes ja estao calculadas
    for (int i = 0; i < points.size(); i++)
    {
        if (i == 0)
        {
			//binormal tem valor inicial arbitrario, desde que nao seja paralelo a tangente
            Vector3f binormalInicial(0.0f, 0.0f, 1.0f);
            Vector3f product = Vector3f::cross(points[i].T, binormalInicial);
            if (product.x() == 0.0f && product.y() == 0.0f && product.z() == 0.0f)
                binormalInicial = Vector3f(0.0f, 1.0f, 0.0f);
                
            points[i].B = binormalInicial;
            points[i].N = Vector3f::cross(points[i].B, points[i].T).normalized();

            continue;
        }

        
        points[i].N = Vector3f::cross(points[i-1].B, points[i].T).normalized();
        points[i].B = Vector3f::cross(points[i].T, points[i].N).normalized();
    }

}
Curve evalBezier( const vector< Vector3f >& P, unsigned steps )
{
    // Check
    if( P.size() < 4 || P.size() % 3 != 1 )
    {
        cerr << "evalBezier must be called with 3n+1 control points." << endl;
        exit( 0 );
    }

    // TODO:
    // You should implement this function so that it returns a Curve
    // (e.g., a vector< CurvePoint >).  The variable "steps" tells you
    // the number of points to generate on each piece of the spline.
    // At least, that's how the sample solution is implemented and how
    // the SWP files are written.  But you are free to interpret this
    // variable however you want, so long as you can control the
    // "resolution" of the discretized spline curve with it.

    // Make sure that this function computes all the appropriate
    // Vector3fs for each CurvePoint: V,T,N,B.
    // [NBT] should be unit and orthogonal.

    // Also note that you may assume that all Bezier curves that you
    // receive have G1 continuity.  Otherwise, the TNB will not be
    // be defined at points where this does not hold.
	vector<CurvePoint> results;
    
    for (int i = 0; i+3 < P.size(); i+= 3)
    {
		vector<CurvePoint> pieceResults;
		Vector3f controlPoints[4] = { P[i], P[i + 1], P[i + 2], P[i + 3] };
		float amount = 1.0f / steps;
        for (int j = 0; j <= steps; j++)
        {
            float t = j * amount;
            CurvePoint pt = simpleDeCasteljau(t, controlPoints);
            pieceResults.push_back(pt);
		}
        computeNormalsAndBinormals(pieceResults);
		results.insert(results.end(), pieceResults.begin(), pieceResults.end());
    }

    cerr << "\t>>> evalBezier has been called with the following input:" << endl;

    cerr << "\t>>> Control points (type vector< Vector3f >): "<< endl;
    for( unsigned i = 0; i < P.size(); ++i )
    {
        cerr << "\t>>> " << P[i] << endl;
    }

    cerr << "\t>>> Steps (type steps): " << steps << endl;
    cerr << "\t>>> Returning empty curve." << endl;
    // Right now this will just return this empty curve.
	//debug printing of results
	
    return Curve(results);
}

Curve evalBspline( const vector< Vector3f >& P, unsigned steps )
{
    // Check
    if( P.size() < 4 )
    {
        cerr << "evalBspline must be called with 4 or more control points." << endl;
        exit( 0 );
    }

    // TODO:
    // It is suggested that you implement this function by changing
    // basis from B-spline to Bezier.  That way, you can just call
    // your evalBezier function.
    vector<CurvePoint> results;
    for (int i = 0; i + 3 < P.size(); i ++)
    {
        vector<CurvePoint> pieceResults;
        Vector3f controlPoints[4] = { P[i], P[i + 1], P[i + 2], P[i + 3] };
        float amount = 1.0f / steps;

        for (int j = 0; j <= steps; j++)
        {
            float t = j * amount;
            CurvePoint pt = bSplineEval(t, controlPoints);
            pieceResults.push_back(pt);
        }

		computeTangents(pieceResults, amount);
        computeNormalsAndBinormals(pieceResults);
        results.insert(results.end(), pieceResults.begin(), pieceResults.end());
    }

	cerr << "\t>>> debug size of results in evalBspline: " << results.size() << endl;

    // Return an empty curve right now.
    //results
    return Curve(results);
}

Curve evalCircle( float radius, unsigned steps )
{
    // This is a sample function on how to properly initialize a Curve
    // (which is a vector< CurvePoint >).
    
    // Preallocate a curve with steps+1 CurvePoints
    Curve R( steps+1 );

    // Fill it in counterclockwise
    for( unsigned i = 0; i <= steps; ++i )
    {
        // step from 0 to 2pi
        float t = 2.0f * M_PI * float( i ) / steps;

        // Initialize position
        // We're pivoting counterclockwise around the y-axis
        R[i].V = radius * Vector3f( cos(t), sin(t), 0 );
        
        // Tangent vector is first derivative
        R[i].T = Vector3f( -sin(t), cos(t), 0 );
        
        // Normal vector is second derivative
        R[i].N = Vector3f( -cos(t), -sin(t), 0 );

        // Finally, binormal is facing up.
        R[i].B = Vector3f( 0, 0, 1 );
    }

    return R;
}

void drawCurve( const Curve& curve, float framesize )
{
    // Save current state of OpenGL
    glPushAttrib( GL_ALL_ATTRIB_BITS );

    // Setup for line drawing
    glDisable( GL_LIGHTING ); 
    glColor4f( 1, 1, 1, 1 );
    glLineWidth( 1 );
    
    // Draw curve
    glBegin( GL_LINE_STRIP );
    for( unsigned i = 0; i < curve.size(); ++i )
    {
        glVertex( curve[ i ].V );
    }
    glEnd();

    glLineWidth( 1 );

    // Draw coordinate frames if framesize nonzero
    if( framesize != 0.0f )
    {
        Matrix4f M;

        for( unsigned i = 0; i < curve.size(); ++i )
        {
            M.setCol( 0, Vector4f( curve[i].N, 0 ) );
            M.setCol( 1, Vector4f( curve[i].B, 0 ) );
            M.setCol( 2, Vector4f( curve[i].T, 0 ) );
            M.setCol( 3, Vector4f( curve[i].V, 1 ) );

            glPushMatrix();
            glMultMatrixf( M );
            glScaled( framesize, framesize, framesize );
            glBegin( GL_LINES );
            glColor3f( 1, 0, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 1, 0, 0 );
            glColor3f( 0, 1, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 1, 0 );
            glColor3f( 0, 0, 1 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 0, 1 );
            glEnd();
            glPopMatrix();
        }
    }
    
    // Pop state
    glPopAttrib();
}

