#ifndef RAY_H_
#define RAY_H_

#include "scene.h"

/**
* describes the position of the ray origin and its direction
*/
typedef struct
{
	vec_d pos;
	vec_d dir;
} ray_d;

/**
* contains a ray and can hold a shadow ray and a reflection ray. Might add refraction later.
*/
struct ray_node
{
	ray_d ray;
	color c;
	ray_d * shad_ray;
	struct ray_node * refl_ray;
	//struct ray_node * refr_ray;
};

typedef struct ray_node ray_node;

/**
* Creates a raytraced image of the scene, casting one ray per pixel
* Allocates colors for pixels dynamically, but will free them if it fails
*
* @param scene trace_scene the scene to draw
* @param color ** pixels where to insert the result
* @param int res_x the width in pixels of the output
* @param int res_y the height in pixels of the output
* @param int the number of recursions to perform on each ray
*
* @return int 0 if it fails, positive number if it succeeds
*/
int ray_trace(scene * scn, color ** pixels, int res_x, int res_y, int depth, int verbose);

#endif