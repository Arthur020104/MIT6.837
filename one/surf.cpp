#include "surf.h"
#include "extra.h"
using namespace std;

namespace
{

    // We're only implenting swept surfaces where the profile curve is
    // flat on the xy-plane.  This is a check function.
    static bool checkFlat(const Curve& profile)
    {
        for (unsigned i = 0; i < profile.size(); i++)
            if (profile[i].V[2] != 0.0 ||
                profile[i].T[2] != 0.0 ||
                profile[i].N[2] != 0.0)
                return false;

        return true;
    }
}

Surface makeSurfRev(const Curve& profile, unsigned steps)
{
    Surface surface;

    if (!checkFlat(profile))
    {
        cerr << "surfRev profile curve must be flat on xy plane." << endl;
        exit(0);
    }
    unsigned profileSize = profile.size();
    float rate = (2.0f * M_PI) / steps;
    vector<CurvePoint> currentCurve; currentCurve.insert(currentCurve.end(), profile.begin(), profile.end());

    vector< Tup3u> faces;
    vector<Vector3f> vertices;
    vector<Vector3f> normals;
    for (int i = 0; i <= steps; i++)
    {
        vector<CurvePoint> rotCurve; rotCurve.resize(profileSize);
        for (int j = 0; j < profileSize; j++)//criando proxima curva
        {
            rotCurve[j] = currentCurve[j];
            Vector4f rotatedV = Matrix4f::rotateY(rate) * Vector4f(rotCurve[j].V, 1);
            rotCurve[j].V = Vector3f(rotatedV.x(), rotatedV.y(), rotatedV.z());

            //Inicialmente tentei fazer como pedia no projeto, mas as normais ficaram erradas
            /*Well, we already have normal vectors N from the evaluation of the curve. So, we can just rotate these normal
vectors using the same transformation as we used for the vertices, right? Yes, and no. It turns out that, if
we transform a vertex by a homogeneous transformation matrix M, its normal should be transformed by
the inverse transpose of the top-left 3 × 3 submatrix of M. A discussion of why this is the case appears
in the Red Book. You can take comfort in the fact that the inverse transpose of a rotation matrix is itself
(since rotation is a rigid transformation).
*/
//Matrix3f rotMatrixNormal = Matrix4f::rotateY(rate).getSubmatrix3x3(0, 0).inverse().transposed();
//rotCurve[j].N = (rotMatrixNormal * rotCurve[j].N).normalized();
        }

        for (int j = 0; j < profileSize - 1; j++)
        {
            unsigned size = vertices.size();

            Tup3u faceT1 = Tup3u(size, size + 1, size + 2);
            vertices.push_back(currentCurve[j].V);
            vertices.push_back(currentCurve[j + 1].V);
            vertices.push_back(rotCurve[j].V);
            faces.push_back(faceT1);




            size += 3;

            Tup3u faceT2 = Tup3u(size, size + 1, size + 2);
            vertices.push_back(currentCurve[j + 1].V);
            vertices.push_back(rotCurve[j + 1].V);
            vertices.push_back(rotCurve[j].V);
            faces.push_back(faceT2);


            //Alternativa que foi vista no CMU
           //Media da normal dos vertices que pertencem a mais de um triangulo
            //Triangulo 1
            Vector3f normalT1 = Vector3f::cross(currentCurve[j + 1].V - currentCurve[j].V, rotCurve[j].V - currentCurve[j].V).normalized();
            Vector3f normalT2 = Vector3f::cross(rotCurve[j + 1].V - currentCurve[j + 1].V, rotCurve[j].V - currentCurve[j + 1].V).normalized();
            Vector3f avg = (normalT1 + normalT2).normalized();

            normals.push_back(normalT1); normals.push_back(avg); normals.push_back(avg);

            //Triangulo 2

            normals.push_back(avg); normals.push_back(avg); normals.push_back(normalT2);

            /*
            * tentativa com o pedido do projeto
            normals.push_back(currentCurve[j].N);
             normals.push_back(currentCurve[j + 1].N);
             normals.push_back(rotCurve[j].N);

             normals.push_back(currentCurve[j + 1].N);
             normals.push_back(rotCurve[j + 1].N);
             normals.push_back(rotCurve[j].N);*/
        }

        currentCurve = rotCurve;

    }
    surface.VV = vertices;
    surface.VF = faces;
    surface.VN = normals;
    // TODO: Here you should build the surface.  See surf.h for details.

   //..// cerr << "\t>>> makeSurfRev called (but not implemented).\n\t>>> Returning empty surface." << endl;

    return surface;
}
/*
* Tive duas tentativas para implementar a funcao makeGenCyl. No entanto, ambas foram incompletas ou insatisfatorias.
* Abaixo dessa solucao de terceiro existe minhas tentativas anteriores comentadas.
O codigo abaixo eh de autoria de Andrew Moran, disponivel em
https://github.com/andrewmo2014/
e pode ser encontrado em https://github.com/andrewmo2014/Curves-and-Surfaces/blob/master/surf.cpp

*/
Surface makeGenCyl(const Curve& profile, const Curve& sweep)
{
    Surface surface;

    if (!checkFlat(profile))
    {
        cerr << "genCyl profile curve must be flat on xy plane." << endl;
        exit(0);
    }

    // TODO: Here you should build the surface.  See surf.h for details.

    //cerr << "\t>>> makeGenCyl called (but not implemented).\n\t>>> Returning empty surface." <<endl;

    for (unsigned i = 0; i < profile.size(); i++) {

        for (unsigned j = 0; j < sweep.size(); j++) {

            //Matrix from sweep
            Matrix4f coordM(sweep[j].N[0], sweep[j].B[0], sweep[j].T[0], sweep[j].V[0],
                sweep[j].N[1], sweep[j].B[1], sweep[j].T[1], sweep[j].V[1],
                sweep[j].N[2], sweep[j].B[2], sweep[j].T[2], sweep[j].V[2],
                0.f, 0.f, 0.f, 1.f);

            //Matrix operations to get normal
            Matrix3f rotatMsub = coordM.getSubmatrix3x3(0, 0);
            Matrix3f rotatMtrans = rotatMsub.transposed();
            Matrix3f rotatN = rotatMtrans.inverse();

            //Calculate surface vertex
            Vector4f surfaceCalc = Vector4f(profile[i].V[0], profile[i].V[1], profile[i].V[2], 1.f);
            Vector4f surfaceVecInit = coordM * surfaceCalc;
            Vector3f surfaceVec = Vector3f(surfaceVecInit[0], surfaceVecInit[1], surfaceVecInit[2]);

            //Calculate surface normal
            Vector3f surfaceVNInit = rotatN * profile[i].N;

            //Push vectors into surface data
            surface.VV.push_back(surfaceVec);
            surface.VN.push_back(-1 * surfaceVNInit);
        }
    }

    //Calculate faces once all the vertices are added
    for (unsigned k = 0; k < surface.VV.size() - (sweep.size()); k++) {

        Tup3u firstTri;		//faces uses a series of connected triangles
        Tup3u secondTri;

        if ((k + 1) % (sweep.size()) != 0)	//Create triangles (considering edge conditions)
        {
            //Triangles in counter-clockwise manner
            firstTri = Tup3u(k + 1, k, k + sweep.size());
            secondTri = Tup3u(k + 1, k + sweep.size(), k + 1 + sweep.size());
        }

        surface.VF.push_back(firstTri);
        surface.VF.push_back(secondTri);

    }

    return surface;
}


/*1. Funcional, porem altamente ineficiente 
Segunda tentativa de solucao. Codigo compila e gera a superficie, porem eh muito ineficiente a solucao geometrica eh a esperada, mas a superficie nao eh "suave" como o esperado.

Resolvi o problema da primeira tentativa girando a curva de perfil em cada ponto, no caso de teste com arquivo fircle.swp, funciona corretamente. Mas em outros casos 
espero encontrar problemas. 
Surface makeGenCyl(const Curve& profile, const Curve& sweep)
{
    Surface surface;

    if (!checkFlat(profile))
    {
        cerr << "genCyl profile curve must be flat on xy plane." << endl;
        exit(0);
    }

    // TODO: Here you should build the surface.  See surf.h for details.

    cerr << "\t>>> makeGenCyl called (but not implemented).\n\t>>> Returning empty surface." << endl;
    unsigned profileSize = profile.size();
    unsigned steps = 20;
    float rate = (2.0f * M_PI) / steps;


    vector< Tup3u> faces;
    vector<Vector3f> vertices;
    vector<Vector3f> normals;

    vector<CurvePoint> nextCurve;
    for (int h = 0; h < sweep.size(); h++)
    {

        if (h == 0)
        {
            for (auto pt : profile)
            {
                pt.V += sweep[h].V;

                nextCurve.push_back(pt);
            }
            continue;
        }
        for (auto& pt : nextCurve)
        {
            pt.V += sweep[h].V - sweep[h - 1].V;
        }
        vector<CurvePoint> currentCurve = nextCurve;
        for (int i = 0; i <= steps; i++)
        {
            vector<CurvePoint> rotCurve; rotCurve.resize(profileSize);
            for (int j = 0; j < profileSize; j++)//criando proxima curva
            {
                rotCurve[j] = currentCurve[j];
                Vector3f pivot = sweep[h].V;

                Vector3f v = rotCurve[j].V - pivot;

                float c = cosf(rate);
                float s = sinf(rate);

                Vector3f vrot;
                vrot.x() = v.x();
                vrot.y() = c * v.y() - s * v.z();
                vrot.z() = s * v.y() + c * v.z();

                rotCurve[j].V = vrot + pivot;

            }
            for (int j = 0; j < profileSize - 1; j++)
            {
                unsigned size = vertices.size();

                Tup3u faceT1 = Tup3u(size, size + 1, size + 2);
                vertices.push_back(currentCurve[j].V);
                vertices.push_back(currentCurve[j + 1].V);
                vertices.push_back(rotCurve[j].V);
                faces.push_back(faceT1);




                size += 3;

                Tup3u faceT2 = Tup3u(size, size + 1, size + 2);
                vertices.push_back(currentCurve[j + 1].V);
                vertices.push_back(rotCurve[j + 1].V);
                vertices.push_back(rotCurve[j].V);
                faces.push_back(faceT2);


                //Alternativa que foi vista no CMU
                //Media da normal dos vertices que pertencem a mais de um triangulo
                //Triangulo 1
                Vector3f normalT1 = Vector3f::cross(currentCurve[j + 1].V - currentCurve[j].V, rotCurve[j].V - currentCurve[j].V).normalized();
                Vector3f normalT2 = Vector3f::cross(rotCurve[j + 1].V - currentCurve[j + 1].V, rotCurve[j].V - currentCurve[j + 1].V).normalized();
                Vector3f avg = (normalT1 + normalT2).normalized();

                normals.push_back(normalT1); normals.push_back(avg); normals.push_back(avg);

                //Triangulo 2

                normals.push_back(avg); normals.push_back(avg); normals.push_back(normalT2);

                
                //* tentativa com o pedido do projeto
               // normals.push_back(currentCurve[j].N);
                //    normals.push_back(currentCurve[j + 1].N);
                //    normals.push_back(rotCurve[j].N);
                //
                //   normals.push_back(currentCurve[j + 1].N);
                 //   normals.push_back(rotCurve[j + 1].N);
                 //   normals.push_back(rotCurve[j].N);
            }

            currentCurve = rotCurve;


        }
    }
    surface.VV = vertices;
    surface.VF = faces;
    surface.VN = normals;
    // TODO: Here you should build the surface.  See surf.h for details.

   //..// cerr << "\t>>> makeSurfRev called (but not implemented).\n\t>>> Returning empty surface." << endl;

    return surface;


    //return surface;
}*/

/*2.Nao chega a solucao esperada.
Tentativa de solucao 1. Codigo funciona e compila, mas tive um problema para girar o de acordo com a direcao da curva
* 
* 
* 
Surface makeGenCyl(const Curve& profile, const Curve& sweep)
{
    Surface surface;

    if (!checkFlat(profile))
    {
        cerr << "genCyl profile curve must be flat on xy plane." << endl;
        exit(0);
    }

    // TODO: Here you should build the surface.  See surf.h for details.

    cerr << "\t>>> makeGenCyl called (but not implemented).\n\t>>> Returning empty surface." << endl;
    unsigned profileSize = profile.size();



    vector< Tup3u> faces;
    vector<Vector3f> vertices;
    vector<Vector3f> normals;


    vector<CurvePoint> currentCurve;
    for (int i = 0; i < sweep.size(); i++)
    {
        if (i == 0)
        {
            currentCurve.resize(0);
            for (auto pt : profile)
            {
                Vector4f p = Matrix4f::rotateY(M_PI/2) * Vector4f(pt.V, 1);
                pt.V = Vector3f(p.x(), p.y(), p.z());
                pt.V += sweep[i].V;
               
                currentCurve.push_back(pt);
            }
            continue;
        }
        vector<CurvePoint> rotCurve; rotCurve.resize(profileSize);
        for (int j = 0; j < profileSize; j++)//criando proxima curva
        {
            rotCurve[j] = currentCurve[j];
            rotCurve[j].V += sweep[i].V - sweep[i - 1].V;


        }
        for (int j = 0; j < profileSize - 1; j++)
        {
            unsigned size = vertices.size();

            Tup3u faceT1 = Tup3u(size, size + 1, size + 2);
            vertices.push_back(currentCurve[j].V);
            vertices.push_back(currentCurve[j + 1].V);
            vertices.push_back(rotCurve[j].V);
            faces.push_back(faceT1);




            size += 3;

            Tup3u faceT2 = Tup3u(size, size + 1, size + 2);
            vertices.push_back(currentCurve[j + 1].V);
            vertices.push_back(rotCurve[j + 1].V);
            vertices.push_back(rotCurve[j].V);
            faces.push_back(faceT2);


            //Alternativa que foi vista no CMU
            //Media da normal dos vertices que pertencem a mais de um triangulo
            //Triangulo 1
            Vector3f normalT1 = Vector3f::cross(currentCurve[j + 1].V - currentCurve[j].V, rotCurve[j].V - currentCurve[j].V).normalized();
            Vector3f normalT2 = Vector3f::cross(rotCurve[j + 1].V - currentCurve[j + 1].V, rotCurve[j].V - currentCurve[j + 1].V).normalized();
            Vector3f avg = (normalT1 + normalT2).normalized();

            normals.push_back(normalT1); normals.push_back(avg); normals.push_back(avg);

            //Triangulo 2

            normals.push_back(avg); normals.push_back(avg); normals.push_back(normalT2);

            //
            //* tentativa com o pedido do projeto
            //normals.push_back(currentCurve[j].N);
               // normals.push_back(currentCurve[j + 1].N);
              //  normals.push_back(rotCurve[j].N);

               // normals.push_back(currentCurve[j + 1].N);
               //normals.push_back(rotCurve[j + 1].N);
                //normals.push_back(rotCurve[j].N);//
        }

        currentCurve = rotCurve;


    }
    
    surface.VV = vertices;
    surface.VF = faces;
    surface.VN = normals;
    // TODO: Here you should build the surface.  See surf.h for details.

   //..// cerr << "\t>>> makeSurfRev called (but not implemented).\n\t>>> Returning empty surface." << endl;

    return surface;
}*/

void drawSurface(const Surface& surface, bool shaded)
{
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    if (shaded)
    {
        // This will use the current material color and light
        // positions.  Just set these in drawScene();
        glEnable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // This tells openGL to *not* draw backwards-facing triangles.
        // This is more efficient, and in addition it will help you
        // make sure that your triangles are drawn in the right order.
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }
    else
    {
        glDisable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glColor4f(0.4f, 0.4f, 0.4f, 1.f);
        glLineWidth(1);
    }

    glBegin(GL_TRIANGLES);
    for (unsigned i = 0; i < surface.VF.size(); i++)
    {
        glNormal(surface.VN[surface.VF[i][0]]);
        glVertex(surface.VV[surface.VF[i][0]]);
        glNormal(surface.VN[surface.VF[i][1]]);
        glVertex(surface.VV[surface.VF[i][1]]);
        glNormal(surface.VN[surface.VF[i][2]]);
        glVertex(surface.VV[surface.VF[i][2]]);
    }
    glEnd();

    glPopAttrib();
}

void drawNormals(const Surface& surface, float len)
{
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glDisable(GL_LIGHTING);
    glColor4f(0, 1, 1, 1);
    glLineWidth(1);

    glBegin(GL_LINES);
    for (unsigned i = 0; i < surface.VV.size(); i++)
    {
        glVertex(surface.VV[i]);
        glVertex(surface.VV[i] + surface.VN[i] * len);
    }
    glEnd();

    glPopAttrib();
}

void outputObjFile(ostream& out, const Surface& surface)
{

    for (unsigned i = 0; i < surface.VV.size(); i++)
        out << "v  "
        << surface.VV[i][0] << " "
        << surface.VV[i][1] << " "
        << surface.VV[i][2] << endl;

    for (unsigned i = 0; i < surface.VN.size(); i++)
        out << "vn "
        << surface.VN[i][0] << " "
        << surface.VN[i][1] << " "
        << surface.VN[i][2] << endl;

    out << "vt  0 0 0" << endl;

    for (unsigned i = 0; i < surface.VF.size(); i++)
    {
        out << "f  ";
        for (unsigned j = 0; j < 3; j++)
        {
            unsigned a = surface.VF[i][j] + 1;
            out << a << "/" << "1" << "/" << a << " ";
        }
        out << endl;
    }
}
