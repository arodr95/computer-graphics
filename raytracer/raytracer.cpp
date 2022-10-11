// raytracer.cpp : Defines the entry point for the console application.
//


#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <GLUT/GLUT.h>




#include "myray.h"
#include "glm.h"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

extern Camera* ray_cam;       // camera info
extern int image_i, image_j;  // current pixel being shaded
extern bool wrote_image;      // has the last pixel been shaded?

							  // reflection/refraction recursion control

extern int maxlevel;          // maximum depth of ray recursion 
extern double minweight;      // minimum fractional contribution to color

							  // these describe the scene

extern vector < GLMmodel* > model_list;
extern vector < Sphere* > sphere_list;
extern vector < Light* > light_list;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

// intersect a ray with the entire scene (.obj models + spheres)

// x, y are in pixel coordinates with (0, 0) the upper-left hand corner of the image.
// color variable is result of this function--it carries back info on how to draw the pixel

void trace_ray(int level, double weight, Ray* ray, Vect color)
{
	Intersection* nearest_inter = NULL;
	Intersection* inter = NULL;
	int i;

	// test for intersection with all .obj models

	for (i = 0; i < model_list.size(); i++) {
		inter = intersect_ray_glm_object(ray, model_list[i]);
		update_nearest_intersection(&inter, &nearest_inter);
	}

	// test for intersection with all spheres

	for (i = 0; i < sphere_list.size(); i++) {
		inter = intersect_ray_sphere(ray, sphere_list[i]);
		update_nearest_intersection(&inter, &nearest_inter);
	}

	// "color" the ray according to intersecting surface properties

	// choose one of the simpler options below to debug or preview your scene more quickly.
	// another way to render faster is to decrease the image size.

	if (nearest_inter) {
		//shade_ray_false_color_normal(nearest_inter, color);
		//shade_ray_intersection_mask(color);
		shade_ray_diffuse(ray, nearest_inter, color);
		shade_ray_recursive(level, weight, ray, nearest_inter, color);
	}

	// color the ray using a default

	else
		shade_ray_background(ray, color);
}

//----------------------------------------------------------------------------

// test for ray-sphere intersection; return details of intersection if true

Intersection* intersect_ray_sphere(Ray* ray, Sphere* S)
{
    Intersection* inter = NULL;
    double t0, t1;
    
    double a = VectDotProd(ray->dir, ray->dir);
    
    Vect oc;
    VectSub(ray->orig, S->P, oc);
    double b = 2.0 * VectDotProd(ray->dir, oc);
    
    double c = VectDotProd(oc, oc) - pow(S->radius, 2);
    
    double discriminant = pow(b,2) - 4 * a * c;
    
    //no intersection
    if (discriminant<0.0)
    {
        return NULL;
    }
    //one intersection point
    else if (discriminant == 0)
    {
        inter->t = -b / (2 * a);
    }
    //two intersection points
    else
    {
        t0 = (-b + sqrt(discriminant)) / (2 * a);
        t1 = (-b - sqrt(discriminant)) / (2 * a);
        
        //both intersections behind camera
        if (t0 < 0 && t1 < 0)
        {
            return NULL;
        }
        //one intersection behind camera
        else if( t0 < 0 || t1 < 0)
        {
            //t0 is in front
            if(t0>t1)
            {
                inter->t = t0;
            }
            //t1 is in front
            else
            {
                inter->t = t1;
            }
        }
        else
        {
            double t = min(t0,t1);
            inter->t = t;
        }
    }
    
    VectAddS(inter->t, ray->dir, ray->orig, inter->P);
    
    VectSub(inter->P, S->P, inter->N);
    VectUnit(inter->N);

    
    inter->surf = S->surf;

    return inter;
}

//----------------------------------------------------------------------------

// only local, ambient + diffuse lighting (no specular, shadows, reflections, or refractions)

void shade_ray_diffuse(Ray* ray, Intersection* inter, Vect color)
{
	Vect L;

	// iterate over lights

	for (int i = 0; i < light_list.size(); i++) {

		// AMBIENT

		color[R] += inter->surf->amb[R] * light_list[i]->amb[R];
		color[G] += inter->surf->amb[G] * light_list[i]->amb[G];
		color[B] += inter->surf->amb[B] * light_list[i]->amb[B];

		// DIFFUSE
        
        VectSub(inter->P, light_list[i]->P, L);
        VectUnit(L);
        
        color[R] += inter->surf->diff[R] * light_list[i]->diff[R] * VectDotProd(inter->N, L);
        color[G] += inter->surf->diff[G] * light_list[i]->diff[G] * VectDotProd(inter->N, L);
        color[B] += inter->surf->diff[B] * light_list[i]->diff[B] * VectDotProd(inter->N, L);

	}

	// clamp color to [0, 1]

	VectClamp(color, 0, 1);
}

//----------------------------------------------------------------------------

// same as shade_ray_diffuse(), but add specular lighting + shadow rays (i.e., full Phong illumination model)

void shade_ray_local(Ray* ray, Intersection* inter, Vect color)
{
    Vect L;
    Vect V;
    Vect Re;
    
    // iterate over lights

    for (int i = 0; i < light_list.size(); i++) {

        // AMBIENT

        color[R] += inter->surf->amb[R] * light_list[i]->amb[R];
        color[G] += inter->surf->amb[G] * light_list[i]->amb[G];
        color[B] += inter->surf->amb[B] * light_list[i]->amb[B];

        // DIFFUSE
        
        VectSub(inter->P, light_list[i]->P, L);
        VectMag(L);
        
        color[R] += inter->surf->diff[R] * light_list[i]->diff[R] * VectDotProd(inter->N, L);
        color[G] += inter->surf->diff[G] * light_list[i]->diff[G] * VectDotProd(inter->N, L);
        color[B] += inter->surf->diff[B] * light_list[i]->diff[B] * VectDotProd(inter->N, L);
        
        //SPECULAR
        
        VectNegate(ray_cam->eye, V);
        VectUnit(V);
        
        color[R] += inter->surf->spec[R] * light_list[i]->spec[R] * pow(VectDotProd(V, Re), light_list[i]->spec_exp);
        color[G] += inter->surf->spec[G] * light_list[i]->spec[G] * pow(VectDotProd(V, Re), light_list[i]->spec_exp);
        color[B] += inter->surf->spec[B] * light_list[i]->spec[B] * pow(VectDotProd(V, Re), light_list[i]->spec_exp);

    }

    // clamp color to [0, 1]

    VectClamp(color, 0, 1);
}

//----------------------------------------------------------------------------

// full shading model: ambient/diffuse/specular lighting, shadow rays, recursion for reflection, refraction

// level = recursion level (only used for reflection/refraction)

void shade_ray_recursive(int level, double weight, Ray* ray, Intersection* inter, Vect color)
{
	Surface* surf = (Surface*)malloc(sizeof(Surface));
	int i;

	// initialize color to Phong reflectance model

	shade_ray_local(ray, inter, color);

	// if not too deep, recurse

	if (level + 1 < maxlevel) {

		// add reflection component to color

		if (surf->reflectivity * weight > minweight) {

			// INSERT YOUR CODE HERE
            
            //r = l - 2(n . l)n - l
            Vect l, r;
            VectSub(ray->orig, inter->P, l);
            VectNegate(l, l);
            VectAddS(2*VectDotProd(inter->N, l), inter->N, l, r);
            VectNegate(r, ray->dir);
        
            shade_ray_recursive(level + 1, weight, ray, inter, color);
		}

		// add refraction component to color

		if (surf->transparency * weight > minweight) {

			// INSERT YOUR CODE HERE (EXTRA CREDITS FOR REFRACTION)

		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

// ray trace another pixel if the image isn't finished yet

void idle()
{
	if (image_j < ray_cam->im->h) {

		raytrace_one_pixel(image_i, image_j);

		image_i++;

		if (image_i == ray_cam->im->w) {
			image_i = 0;
			image_j++;
		}
	}

	// write rendered image to file when done

	else if (!wrote_image) {

		write_PPM("output.ppm", ray_cam->im);

		wrote_image = true;
	}

	glutPostRedisplay();
}

//----------------------------------------------------------------------------

// show the image so far

void display(void)
{
	// draw it!

	glPixelZoom(1, -1);
	glRasterPos2i(0, ray_cam->im->h);

	glDrawPixels(ray_cam->im->w, ray_cam->im->h, GL_RGBA, GL_FLOAT, ray_cam->im->data);

	glFlush();
}

//----------------------------------------------------------------------------

void init()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, ray_cam->im->w, 0.0, ray_cam->im->h);
}

//----------------------------------------------------------------------------

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	// initialize scene (must be done before scene file is parsed)
	Camera* new_cam = (Camera*)malloc(sizeof(Camera));
	ray_cam = (Camera*)malloc(sizeof(Camera));
	init_raytracing(ray_cam);



	if (argc == 2)
		parse_scene_file(argv[1], ray_cam);
	else {
		printf("missing .scene file\n");
		exit(1);
	}

	// opengl business

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(ray_cam->im->w, ray_cam->im->h);
	glutInitWindowPosition(500, 300);
	glutCreateWindow("hw4_raytracing");
	init();

	glutDisplayFunc(display);
	glutIdleFunc(idle);

	glutMainLoop();

	return 0;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
