//
//  main.cpp
//  trackball
//
//  Created by JINWEI YE on 10/3/17.
//  Copyright © 2017 JINWEI YE. All rights reserved.
//

#include <iostream>
#include <fstream>
using namespace std;

#include <stdlib.h>
#include <math.h>

#include <sys/types.h>
#include <GLUT/glut.h>


#include "objLoader.hpp"


#define KEY_LEFT 100
#define KEY_UP 101
#define KEY_RIGHT 102
#define KEY_DOWN 103

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

int winWidth = 1024;
int winHeight = 1024;
bool firstTime = true;

WavefrontObj *obj_data;

// Trackball parameters initialization
float angle = 0.0, axis[3], trans[3];

bool trackingMouse = false;
bool redrawContinue = false;
bool trackballMove = false;

float lastPos[3] = { 0.0, 0.0, 0.0 };
int curx, cury;
int startX, startY;


// Translation & Rotation
float x_trans = 0.0f; // translate object in x direction
float y_trans = 0.0f; // translate object in y direction
float zoom = 1.0f; // zoom for scaling

float x_TotalTrans = 0.0f; //keep track of total translation
float y_TotalTrans = 0.0f;



void Init(int w, int h)
{
    glViewport(0, 0, w, h);
    glShadeModel(GL_SMOOTH);                                // Set Smooth Shading
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);                    // Background Color
    glClearDepth(1.0f);                                        // Depth buffer setup
    glEnable(GL_DEPTH_TEST);                                // Enables Depth Testing
    glDepthFunc(GL_LEQUAL);                                    // The Type Of Depth Test To Do
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);        // Use perspective correct interpolation if available
    
    glMatrixMode(GL_PROJECTION);                            // Select The Projection Matrix
    glLoadIdentity();                                        // Reset The Projection Matrix
    double aspect = (double)h / w;
    glFrustum(-5, 5, -5 * aspect, 5 * aspect, 10, 500);          // Define perspective projection frustum
    //gluPerspective(30, w/h, 10, 74);
    glTranslated(0.0, 0.0, -24);                          // Viewing transformation
    
    glMatrixMode(GL_MODELVIEW);                                // Select The Modelview Matrix
    glLoadIdentity();                                        // Reset The Modelview Matrix
    
    if (firstTime)
    {
        glEnable(GL_LIGHTING);
        float frontColor[] = { 0.2f, 0.7f, 0.7f, 1.0f };
        
        glMaterialfv(GL_FRONT, GL_AMBIENT, frontColor);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, frontColor);
        glMaterialfv(GL_FRONT, GL_SPECULAR, frontColor);
        glMaterialf(GL_FRONT, GL_SHININESS, 100);
        
        float lightDirection[] = { 2.0f, 0.0f, 1.0f, 0 };
        float ambientIntensity[] = { 0.1f, 0.1f, 0.1f, 1.0f };
        float lightIntensity[] = { 0.9f, 0.9f, 0.9f, 1.0f };
        float lightSpec[] = { 1.0f, 1.0f, 1.0f, 1 };
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambientIntensity);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity);
        glLightfv(GL_LIGHT0, GL_POSITION, lightDirection);
        glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpec);
        glEnable(GL_LIGHT0);
        firstTime = false;
    }
}


void Draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);        // Clear The Screen And The Depth Buffer
    
    // Insert your own code here (Hint:Translation Rotation & Scaling)
    
    GLfloat objectXform[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, objectXform);
    glLoadIdentity();
    glTranslatef(x_TotalTrans + x_trans, y_TotalTrans + y_trans, 0);
    glScalef(zoom, zoom, zoom);
    glRotatef(angle, axis[0], axis[1], axis[2]);
    glTranslatef(-x_TotalTrans, -y_TotalTrans, 0);
    glMultMatrixf(objectXform);
    
    // end of code
    if (obj_data != NULL)
        obj_data->Draw();
    else
        glutSolidTeapot(1.0);    //draw a teapot when no argument is provided
    
    glFlush();
    glutSwapBuffers();
    
    x_TotalTrans += x_trans; //Add current translation to total translation
    y_TotalTrans += y_trans;
    x_trans = 0.0f; //reset
    y_trans = 0.0f;
    zoom = 1.0f;
    angle = 0.0f;
    
}

//
// GLUT keypress function
//

void onKeyPress(unsigned char key, int x, int y)
{
    if (key == 'w')
    {
        obj_data->mode = GL_LINE_LOOP;
    }
    else if (key == 'p')
    {
        obj_data->mode = GL_POINTS;
    }
    else if (key == 's')
    {
        glShadeModel(GL_SMOOTH);                                // Set Smooth Shading
        obj_data->mode = GL_POLYGON;
    }
    else if (key == 'f')
    {
        glShadeModel(GL_FLAT);                                // Set Smooth Shading
        obj_data->mode = GL_POLYGON;
    }
    else if (key == 'q')
    {
        delete obj_data;
        exit(0);
    }
    // Insert your own code here (Hint: Use keyboard to control translations and scaling)
    //'i' key used to zoom in
    else if (key == 'i')
    {
        zoom += 0.05;
    }
    //'o' key used to zoom out
    else if (key == 'o')
    {
        zoom -= 0.05;
    }
    else if(key == 'h')
    {
        y_trans += 0.5;
    }
    glutPostRedisplay();
}

void Specialkey(int key, int x, int y)
 {
     //'up arrow' used to translate up
     if (key == KEY_UP)
     {
         y_trans += 0.5;
     }
     //'down arrow' used to translate down
     else if (key == KEY_DOWN)
     {
         y_trans -= 0.5;
     }
     //'right arrow' used to translate right
     else if (key == KEY_RIGHT)
     {
         x_trans += 0.5;
     }
     //'left arrow' used to translate left
     else if (key == KEY_LEFT)
     {
         x_trans -= 0.5;
     }
     glutPostRedisplay();
 }

void mouseMotion(int x, int y)
{
    // Insert your own code here (Hint: track the motion of mouse point)
    if (trackingMouse) //check if mouse is being tracked
    {
        float curPos[3], dx, dy, dz;
        float d, norm;
        curPos[0] = (2.0f * x - winWidth) / winWidth; //x of current position
        curPos[1] = (winHeight - 2.0f * y) / winHeight; //y of current position
        d = sqrtf(curPos[0] * curPos[0] + curPos[1] * curPos[1]);
        d = (d < 1.0f) ? d : 1.0f; //this line of code from the ppt
        curPos[2] = sqrtf(1.001f - d * d); //z component of current position
        norm = 1.0f / sqrt(curPos[0] * curPos[0] + curPos[1] * curPos[1] + curPos[2] * curPos[2]);
        curPos[0] *= norm; //normalize each value
        curPos[1] *= norm;
        curPos[2] *= norm;
        dx = curPos[0] - lastPos[0]; //calculate change in position
        dy = curPos[1] - lastPos[1];
        dz = curPos[2] - lastPos[2];
        if (dx || dy || dz) //if the mouse has changed position
        {
            angle = 90.0f * sqrt(dx * dx + dy * dy + dz * dz); //calculate rotation angle
            axis[0] = lastPos[1] * curPos[2] - lastPos[2] * curPos[1]; //calculate rotation axis with cross product
            axis[1] = lastPos[2] * curPos[0] - lastPos[0] * curPos[2];
            axis[2] = lastPos[0] * curPos[1] - lastPos[1] * curPos[0];
            lastPos[0] = curPos[0]; //update position
            lastPos[1] = curPos[1];
            lastPos[2] = curPos[2];
        }
    }

    glutPostRedisplay();
}

void mouseButton(int button, int state, int x, int y)
{
    // Insert your own code here (Hint:click down the left button to allow rotation)
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) //check if left button is being held down
    {
            trackingMouse = true; //start tracking mouse movement
            float d, norm;
            lastPos[0] = (2.0f * x - winWidth) / winWidth; //x of postion
            lastPos[1] = (winHeight - 2.0f * y) / winHeight; //y of position
            d = sqrtf(lastPos[0] * lastPos[0] + lastPos[1] * lastPos[1]);
            d = (d < 1.0f) ? d : 1.0f; //code from ppt
            lastPos[2] = sqrtf(1.001f - d * d); //z of position
            norm = 1.0f / sqrt(lastPos[0] * lastPos[0] + lastPos[1] * lastPos[1] + lastPos[2] * lastPos[2]);
            lastPos[0] *= norm; //normalize each value
            lastPos[1] *= norm;
            lastPos[2] *= norm;
    }
    else
        trackingMouse = false; //stop tracking mouse movement
}

int main(int argc, char *argv[])
{
    // glut initialization functions:
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(winWidth, winHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("ImageViewer");
    
    Init(winWidth, winHeight);
    
    // display, onMouseButton, mouse_motion, onKeyPress, and resize are functions defined above
    glutDisplayFunc(Draw);
    glutKeyboardFunc(onKeyPress);
    glutSpecialFunc(Specialkey);  //use this function if you would like to use arrow buttons on your keyboard to control translation
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);
    glutReshapeFunc(Init);
    
    if (argc >= 2)
        obj_data = new WavefrontObj(argv[1]);
    else
        obj_data = NULL;
    
    // start glutMainLoop -- infinite loop
    glutMainLoop();
    
    return 0;
}
