// assignment1.cpp : Defines the entry point for the console application.

#include <iostream>
//#include <GL/glut.h>  //use #include <GLUT/glut.h> if using Xcode in MacOS
#include <GLUT/glut.h>
#include <math.h>
#include <time.h>

int win_H, win_W;
time_t timer;
struct tm curr_time;


void
reshape(int w, int h)
{
    
    glViewport(0, 0, w, h);       /* Establish viewing area to cover entire window. */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, 0, h, -1, 1);
    glScalef(1, -1, 1);
    glTranslatef(0, -h, 0);
}

void
display(void)
{
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    //Insert your own code here
    
    //draw clock face
    glColor3f(1, 1, 1);
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(256, 256, 0);
        float theta;
        float pi = 3.14159;
        for (int i = 0; i < 361; i++)
        {
            theta = i * pi / 180;
            glVertex3f(256 + 150 * cos(theta), 256 + 150 * sin(theta), 0);
        }
    glEnd();

    //draw hours hand
    glPushMatrix();
    
    GLfloat angleH = (float)curr_time.tm_hour * 30.0 + (float)curr_time.tm_min / 2.0 + (float)curr_time.tm_sec / 120.0;
    glTranslatef(256, 256, 0);
    glRotatef(angleH, 0, 0, 1);
    glTranslatef(-256, -256, 0);
    
    glColor3f(0.1, 0.1, 0.1);
    glBegin(GL_TRIANGLE_STRIP);
        glVertex2f(266.0, 256.0);
        glVertex2f(246.0, 256.0);
        glVertex2f(261.0, 156.0);
        glVertex2f(251.0, 156.0);
    glEnd();

    glPopMatrix();
  
    //draw minutes hand
    glPushMatrix();
    
    GLfloat angleM = (float)curr_time.tm_min * 6 + (float)curr_time.tm_sec / 10;
    glTranslatef(256, 256, 0);
    glRotatef(angleM, 0, 0, 1);
    glTranslatef(-256, -256, 0);
    
    glColor3f(0.5, 0.5, 0.5);
    glBegin(GL_TRIANGLES);
        glVertex3f(262, 256, 0);
        glVertex3f(250, 256, 0);
        glVertex3f(256, 156, 0);
    glEnd();

    glPopMatrix();
    
    //draw seconds hand
    glPushMatrix();
    
    GLfloat angleS = (float)curr_time.tm_sec * 6;
    glTranslatef(256, 256, 0);
    glRotatef(angleS, 0, 0, 1);
    glTranslatef(-256, -256, 0);
    
    glColor3f(1, 0, 0);
    glBegin(GL_LINES);
        glVertex3f(256, 256, 0);
        glVertex3f(256, 146, 0);
    glEnd();
    
    glPopMatrix();

    // Your code ends here
    
    glutSwapBuffers(); // swap the back buffer to front
}

void TimeEvent(int time_val)
{
    time(&timer); // get the current date and time from system
    //localtime_s(&curr_time, &timer); // use localtime_r(&timer, &curr_time); if using Xcode in MacOS
    localtime_r(&timer, &curr_time);
    glutPostRedisplay();
    glutTimerFunc(30, TimeEvent, 1);// By using a timed event, your application should run at the same speed on any machine.
}

int
main(int argc, char **argv)
{
    GLenum type;
    
    glutInit(&argc, argv);
    
    type = GLUT_DEPTH;
    type |= GLUT_RGB;
    type |= GLUT_DOUBLE;
    glutInitDisplayMode(type);
    
    time(&timer); // get current date and time
    //localtime_s(&curr_time, &timer); // use localtime_r(&timer, &curr_time); if using Xcode in MacOS
    localtime_r(&timer, &curr_time);
    
    // set window size and create a window for rendering
    win_W = 512;
    win_H = 512;
    glutInitWindowSize(win_H, win_W);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("My clock");
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    
    glutTimerFunc(30, TimeEvent, 1);
    glutMainLoop();
    return 0;
}
