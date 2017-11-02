#ifndef SCENE_H_
#define SCENE_H_

#include "vec.h"

/**
* a 3-vector of doubles, but with semantically accurate names for each element
*/
typedef struct
{
	double r;
	double g;
	double b;
} color;

/**
* a 3 vectors describing the camera position and orientation
*/
typedef struct
{
	vec_d at;
	vec_d up;
	vec_d from;
} camera;

/**
* orientaion of the directional light and its color
*/
typedef struct
{
	vec_d to_dir;
	color l_color;
} light;

/**
* describes reflectivity, specularity and diffuse properties of an object
*/
typedef struct
{
	color refl;
	color diff;
	color spec;
	double p_const;
} material;


/**
* position and size of the sphere and a pointer to its material
*/
typedef struct
{
	vec_d center;
	double radius;
	material * mat;
} sphere;

/**
* Position of each vertex of the triangle and a pointer to its material
*/
typedef struct
{
	vec_d p1;
	vec_d p2;
	vec_d p3;
	vec_d normal;
	material * mat;
} triangle;

/**
* all of the data needed to render the scene in the raytracer
*/
typedef struct
{
	double fov;
	color * amb_light;
	color * bg_color;
	camera * cam;
	light ** lights;
	sphere ** spheres;
	triangle ** triangles;
	int light_count;
	int sphere_count;
	int triangle_count;
} scene;

/**
* Alocates memory for a new scene and for each object in the scene.
*
* @param int light_count the number of lights in the scene
* @param int sphere_count the number of spheres in the scene
* @param int triangle_count the number of triangles in the scene
*
* @return scene * a blank scene with the correct number of objects to accomadate the counts supplied. NULL if it fails (it shouldn't, thought)
*/
void init_scene(scene * scn, int light_count, int sphere_count, int triangle_count);

/**
* frees the memory allocated for the scene. Only used if not all of the scene objects have been intialized yet, so that the correct number will be freed
*
* @param scene * scn the scene to deallocate
* @param int light_count the number of lights to free
* @param int sphere_count the number of spheres to free
* @param int triangle_count the number of triangles to free
*/
void destroy_scene_counts(scene * scn, int light_count, int sphere_count, int triangle_count);

/**
* frees the memory allocated for the scene. This should be used if all objects have been allocated
*
* @param scene * scn the scene to deallocate
*/
void destroy_scene(scene * scn);

/**
*Creates a new material and initializes all values to 0
*
* @param material * mat location of the material to be intiialized
*/
void init_material(material * mat);


/**
* Calculates the normal of the sphere surface at a given position
*
* @param sphere * sph the sphere
* @param vec_d * position the position on the sphere surface
*
* @return vec_d the normal vector
*/
vec_d get_sphere_normal(sphere * sph, vec_d * position);

/**
* Retrieves the normal of the triangle from the direction of origin
*
* @param triangle * tri the triangle
* @param vec_d * origin used to calculate which side's normal to return
*
* @return vec_d the normal vector
*/
vec_d get_triangle_normal(triangle * tri, vec_d * position, vec_d * origin);

/**
* Calculates one of the triangle normal vectors
*
*@param triangle * tri the triangle
*/
void calculate_triangle_normal(triangle * tri);

#endif