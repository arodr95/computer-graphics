//
//  objLoader.cpp
//  trackball
//
//  Created by JINWEI YE on 10/3/17.
//  Copyright © 2017 JINWEI YE. All rights reserved.
//

#include "objLoader.hpp"




Vertex::Vertex(double x, double y, double z)
{
    setCoordinates(x, y, z);
}
void Vertex::setCoordinates(double xval, double yval, double zval)
{
    x = xval; y = yval; z = zval;
}




Normal::Normal(double x, double y, double z)
{
    setCoordinates(x, y, z);
}
void Normal::setCoordinates(double xval, double yval, double zval)
{
    double l = sqrt(xval*xval + yval*yval + zval*zval);
    if (l != 0.0)
        l = 1.0 / l;
    x = l*xval;
    y = l*yval;
    z = l*zval;
}




Texel::Texel(double u, double v)
{
    setCoordinates(u, v);
}
void Texel::setCoordinates(double uval, double vval)
{
    u = uval;
    v = vval;
}




Face::Face() : DEFAULT_SIZE(4)
{
    vIndex = -1;
}

void Face::addVertex(int v)
{
    // make indices zero referenced
    add(v - 1, -1, -1);
}

void Face::addVertexTexel(int v, int t)
{
    add(v - 1, -1, t - 1);
}

void Face::addVertexNormal(int v, int n)
{
    add(v - 1, n - 1, -1);
}

void Face::addVertexNormalTexel(int v, int n, int t)
{
    add(v - 1, n - 1, t - 1);
}

void Face::add(int v, int n, int t)
{
    if (vIndex < 0) {
        vList = new int[DEFAULT_SIZE];
        nList = new int[DEFAULT_SIZE];
        tList = new int[DEFAULT_SIZE];
        current_max_size = DEFAULT_SIZE;
        vIndex = 0;
    }
    vList[vIndex] = v;
    nList[vIndex] = n;
    tList[vIndex] = t;
    vIndex += 1;
    if (vIndex == current_max_size)
    {
        current_max_size = 2 * vIndex;
        int *newV = new int[current_max_size];
        int *newN = new int[current_max_size];
        int *newT = new int[current_max_size];
        for (int i = 0; i < vIndex; i++)
        {
            newV[i] = vList[i];
            newN[i] = nList[i];
            newT[i] = tList[i];
        }
        delete[] vList;
        delete[] nList;
        delete[] tList;
        vList = newV;
        nList = newN;
        tList = newT;
    }
}




WavefrontObj::WavefrontObj(char *filename) : DEFAULT_SIZE(16)
{
    vIndex = -1;
    nIndex = -1;
    tIndex = -1;
    fIndex = -1;
    
    isFlat = false;
    mode = GL_POLYGON;
    
    char *line = new char[200];
    char *line_back = new char[200];
    char wspace[] = { ' ', '\t' };
    char separator[] = { '/' };
    char *token;
    int indices[3];
    double x, y, z;
    double tex_u, tex_v;
    
    ifstream file(filename);
    if (!file)
    {
        cerr << "Cannot open file: " << filename << " exiting." << endl;
        exit(-1);
    }
    while (!file.eof())
    {
        file.getline(line, 199);
        // first, strip off comments
        if (line[0] == '#')
            continue;
        else if (!strcmp(line, ""))
            continue;
        
        else
        {
            char* next_token;
            
            strncpy(line_back, line, 199); // strtok destroys line.
            token = strtok_r(line, " ", &next_token);
            if (!strcmp(token, "v"))
            {
                x = atof(strtok_r(NULL, " ", &next_token));
                y = atof(strtok_r(NULL, " ", &next_token));
                z = atof(strtok_r(NULL, " ", &next_token));
                addVertex(x, y, z);
            }
            else if (!strcmp(token, "vn"))
            {
                x = atof(strtok_r(NULL, " ", &next_token));
                y = atof(strtok_r(NULL, " ", &next_token));
                z = atof(strtok_r(NULL, " ", &next_token));
                addNormal(x, y, z);
            }
            else if (!strcmp(token, "vt"))
            {
                tex_u = atof(strtok_r(NULL, " ", &next_token));
                tex_v = atof(strtok_r(NULL, " ", &next_token));
                addTexel(tex_u, tex_v);
            }
            else if (!strcmp(token, "f"))
            {
                Face *f = addFace();
                for (char *p = strtok_r(NULL, " ", &next_token); p; p = strtok_r(NULL, " ", &next_token))
                {
                    indices[0] = -1;
                    indices[1] = -1;
                    indices[2] = -1;
                    int i = 0;
                    for (int j = 0; j < strlen(p); j++)
                    {
                        if (p[j] != '/')
                        {
                            if (indices[i] == -1)
                            {
                                indices[i] = 0;
                            }
                            
                            indices[i] *= 10;
                            char c[2];
                            c[0] = p[j];
                            c[1] = '\0';
                            indices[i] += atoi(c);
                        }
                        else
                        {
                            i++;
                        }
                    }
                    
                    if ((indices[1] == -1) & (indices[2] == -1))
                    {
                        f->addVertex(indices[0]);                                    // num//
                    }
                    else  if (indices[2] == -1) {
                        f->addVertexTexel(indices[0], indices[1]);                   // num/num/
                    }
                    else if (indices[1] == -1) {
                        f->addVertexNormal(indices[0], indices[2]);                // num//num
                    }
                    else {
                        f->addVertexNormalTexel(indices[0], indices[1], indices[2]); // num/num/num
                    }
                }
            }
            else if (!strcmp(token, "g"))
            {
                // group
            }
            else if (!strcmp(token, "s"))
            {
                // smoothing group
            }
            else if (!strcmp(token, ""))
            {
                // blank line
            }
            else {
                cout << line_back << endl;
            }
        }
    }
}

void WavefrontObj::addVertex(double x, double y, double z)
{
    Vertex *vert = new Vertex(x, y, z);
    addVertex(vert);
}

void WavefrontObj::addVertex(Vertex *vert)
{
    if (vIndex < 0) {
        v = new Vertex*[DEFAULT_SIZE];
        vIndex = 0;
        current_max_verticies = DEFAULT_SIZE;
    }
    v[vIndex] = vert;
    vIndex += 1;
    if (vIndex == current_max_verticies)
    {
        current_max_verticies = 2 * vIndex;
        Vertex **newV = new Vertex*[current_max_verticies];
        for (int i = 0; i < vIndex; i++)
            newV[i] = v[i];
        delete[] v;
        v = newV;
    }
}

void WavefrontObj::addNormal(double x, double y, double z)
{
    Normal *norm = new Normal(x, y, z);
    addNormal(norm);
}

void WavefrontObj::addNormal(Normal *norm)
{
    if (nIndex < 0) {
        n = new Normal*[DEFAULT_SIZE];
        nIndex = 0;
        current_max_normals = DEFAULT_SIZE;
    }
    n[nIndex] = norm;
    nIndex += 1;
    if (nIndex == current_max_normals)
    {
        current_max_normals = 2 * nIndex;
        Normal **newN = new Normal*[current_max_normals];
        for (int i = 0; i < nIndex; i++)
            newN[i] = n[i];
        delete[] n;
        n = newN;
    }
}

void WavefrontObj::addTexel(double u, double v)
{
    Texel *texl = new Texel(u, v);
    addTexel(texl);
}

void WavefrontObj::addTexel(Texel *texl)
{
    if (tIndex < 0)
    {
        t = new Texel*[DEFAULT_SIZE];
        tIndex = 0;
        current_max_texels = DEFAULT_SIZE;
    }
    t[tIndex] = texl;
    tIndex += 1;
    if (tIndex == current_max_texels)
    {
        current_max_texels = 2 * tIndex;
        Texel **newT = new Texel*[current_max_texels];
        for (int i = 0; i < tIndex; i++)
            newT[i] = t[i];
        delete[] t;
        t = newT;
    }
}

Face *WavefrontObj::addFace()
{
    if (fIndex < 0)
    {
        f = new Face*[DEFAULT_SIZE];
        fIndex = 0;
        current_max_faces = DEFAULT_SIZE;
    }
    f[fIndex] = new Face();
    fIndex += 1;
    if (fIndex == current_max_faces)
    {
        current_max_faces = 2 * fIndex;
        Face **newF = new Face*[current_max_faces];
        for (int i = 0; i < fIndex; i++)
            newF[i] = f[i];
        delete[] f;
        f = newF;
    }
    return f[fIndex - 1];
}

Normal *WavefrontObj::faceNormal(Vertex *v0, Vertex *v1, Vertex *v2)
{
    double ax, ay, az;
    double bx, by, bz;
    double nx, ny, nz;
    
    ax = v1->x - v0->x;
    ay = v1->y - v0->y;
    az = v1->z - v0->z;
    
    bx = v2->x - v0->x;
    by = v2->y - v0->y;
    bz = v2->z - v0->z;
    
    nx = ay*bz - az*by;
    ny = az*bx - ax*bz;
    nz = ax*by - ay*bx;
    
    return new Normal(nx, ny, nz);
}

void WavefrontObj::Draw()
{
    int face, vertex, i;
    for (face = 0; face < fIndex; face++)
    {
        Face *currentFace = f[face];
        glBegin(mode);
        for (vertex = 0; vertex < currentFace->vIndex; vertex++)
        {
            if (isFlat) {
                if (vertex == 0)
                {
                    Normal *norm = faceNormal(v[currentFace->vList[0]], v[currentFace->vList[1]], v[currentFace->vList[2]]);
                    glNormal3d(norm->x, norm->y, norm->z);
                }
            }
            else if ((i = currentFace->nList[vertex]) >= 0)
            {
                if (i < nIndex)
                    glNormal3d(n[i]->x, n[i]->y, n[i]->z);
                else {
                    //                      cerr <<"Error i = " <<i <<" nIndex = " <<nIndex <<endl;
                }
            }
            else if (vertex == 0)
            {
                Normal *norm = faceNormal(v[currentFace->vList[0]], v[currentFace->vList[1]], v[currentFace->vList[2]]);
                currentFace->nList[0] = nIndex;
                addNormal(norm);
                glNormal3d(norm->x, norm->y, norm->z);
            }
            if ((i = currentFace->tList[vertex]) >= 0)
            {
                glTexCoord2d(t[i]->u, t[i]->v);
            }
            i = currentFace->vList[vertex];
            if (i < vIndex)
                glVertex3d(v[i]->x, v[i]->y, v[i]->z);
            else {
                //                  cerr <<"Error i = " <<i <<" vIndex = " <<vIndex <<endl;
            }
        }
        glEnd();
    }
}
