#include <stdlib.h>
#include <time.h>
#include <float.h>
#include <math.h>
#include "ray.h"

#define max(a, b) (a > b ? a : b)
#define min(a, b) (a < b ? a : b)

//These two structs are only used to check intersections of rays with triangles
typedef struct
{
	double u;
	double v;
}vec_d_2D;

typedef struct
{
	vec_d_2D p1;
	vec_d_2D p2;
	vec_d_2D p3;
} triangle_2D;

/**
* Recursively traces a single ray
*
* @param ray_node * A node on a ray tree
* @param scene trace_scene the scene to draw
* @param int max_depth recursion depth
* @param int depth current recursion level
*
* @return int 0 if it fails, positive number if it succeeds
*/
int trace_ray(ray_node * ray, scene * scn, int max_depth, int depth);

/**
* Checks for ray intersections with all objects in the scene.
*
* @param ray_d * A ray
* @param scene * scn The scene to compare the ray against
* @param vec_d ** position The closest intersection with an object. Set by this function
* @param vec_d ** normal The normal vector of the intersected object at *position. Set by this function
* @param material ** mat The material of the intersected object. Set by this function
*
* @return int 0 if no collisions are detected, positive number if ray intersects an object
*/
int check_collide(ray_d * ray, scene * scn, vec_d * position, vec_d * normal, material ** mat);

int check_shadow_collide(ray_d * s_ray, scene * scn);

/**
* Two similar functions, check if a ray intersects with a sphere/triangle
*
* @param ray_d * ray The ray
* @param sphere/triangle * The object to compare the ray against
* @param vec_d * position Where, if the ray intersects, the intersection happens
*
* @return int 0 if the ray doesn't intersects the object, positive number if it does
*/
int sphere_collide(ray_d * ray, sphere * sph, vec_d * position);
int triangle_collide(ray_d * ray, triangle * tri, vec_d * position);

/**
* Calculates if a ray intersects with a plane defined by a point and a normal
* 
* @param ray_d * ray
* @param vec_d * p_point an arbitrary point on a plane
* @param vec_d * normal the normal vector of that plane
* @param vec_d * position where the ray intersects the plane
*
* @return int 0 if the ray doesn't intersect, positive number if it does
*/
int plane_collide(ray_d * ray, vec_d * p_point, vec_d * p_normal, vec_d * position);

/***/
void check_cross(vec_d_2D * p1, vec_d_2D * p2, int * cross_count, int * sign);

/**
* Recursively frees a ray tree
*
* @param ray_node * A node on a ray tree
*
* @return int 0 if it fails, positive number if it succeeds
*/
void destroy_node(ray_node * node);

/**
* Projects a 3D triangle and a 3D vector to 2D.
* 
* @param triangle * tri a 3 dimensional triangle
* @param vec_d * vec a 3 dimensional vector
* @param triangle_2D * tri_proj the projected triangle. Updated by this function
* @param vec_d_2D * vec_proj the projected vector. Updated by this function
*/
void project_2D(triangle * tri, vec_d * vec, triangle_2D * tri_proj, vec_d_2D * vec_proj);


void calculateAmbient(color * c, material * mat, color * amb_light);
void calculateDiffuse(color * c, material * mat, vec_d * normal, light * lights);
void calculateSpecular(vec_d * position, color * c, material * mat, vec_d * normal, light * lgt, camera * cam);
void clamp_color(color * c);

int ray_trace(scene * scn, color ** pixels, int res_x, int res_y, int depth, int verbose)
{
	double view_w = tan(scn->fov * (atan(1) * 4 / 180.0)) * 2;
	double i, j;
	int pixel_count = 0;
	double x_step = view_w / res_x, y_step = view_w / res_y;
	srand(time(NULL));
	for (i = res_y * 0.5; i > res_y * -0.5; i--)
	{
		for (j = res_x * -0.5; j < res_x * 0.5; j++)
		{
			if (i == 0 && j == 0)
			{
				abs(1);
			}
			ray_node * node = (ray_node *) malloc(sizeof(ray_node));

			node->ray.pos.x = scn->cam->from.x;
			node->ray.pos.y = scn->cam->from.y;
			node->ray.pos.z = scn->cam->from.z;
			vec_d ray_to;
			ray_to.x = j * x_step + x_step / 2;
			ray_to.y = i * y_step - y_step / 2;
			ray_to.z = -0.0f;
			node->ray.dir = sub_vecs(&ray_to, &node->ray.pos);
			vec_normalize(&node->ray.dir);

			trace_ray(node, scn, depth, 0);

			color * pixel = (color *) malloc(sizeof(color));
			*pixel = node->c;
			pixels[pixel_count] = pixel;
			pixel_count++;
			destroy_node(node);
		}
	}
	return 1;
}

int trace_ray(ray_node * ray, scene * scn, int max_depth, int depth)
{
	ray->c.r = 0;
	ray->c.g = 0;
	ray->c.b = 0;
	ray->refl_ray = NULL;
	ray->shad_ray = NULL;
	material * mat;
	vec_d * normal = (vec_d *) malloc(sizeof(vec_d));
	vec_d * position = (vec_d *) malloc(sizeof(vec_d));
	if (!check_collide(&ray->ray, scn, position, normal, &mat))
	{
		ray->c = *scn->bg_color;
		free(normal);
		free(position);
		return 1;
	}
	int i;
	if (scn->amb_light->r || scn->amb_light->g || scn->amb_light->b)
	{
		calculateAmbient(&ray->c, mat, scn->amb_light);
	}
	vec_d offset = vec_mult(normal, .001);
	vec_d origin = sum_vecs(position, &offset);
	for (i = 0; i < scn->light_count; i++)
	{
		light * lgt = scn->lights[i];
		ray->shad_ray = (ray_d *) malloc(sizeof(ray_d));
		ray->shad_ray->pos = origin;
		ray->shad_ray->dir = lgt->to_dir;
		if (!check_shadow_collide(ray->shad_ray, scn))
		{
			if (mat->diff.r || mat->diff.g || mat->diff.b)
			{
				calculateDiffuse(&ray->c, mat, normal, lgt);
			}
			if (mat->spec.r || mat->spec.g || mat->spec.b)
			{
				calculateSpecular(position, &ray->c, mat, normal, lgt, scn->cam);
			}
		}
		if ((mat->refl.r || mat->refl.g || mat->refl.b) && depth < max_depth)
		{
			ray->refl_ray = (ray_node *) malloc(sizeof(ray_node));
			ray->refl_ray->ray.pos = origin;
			vec_d v = vec_neg(&ray->ray.dir);
			ray->refl_ray->ray.dir = vec_reflect(&v, normal);
			trace_ray(ray->refl_ray, scn, max_depth, depth + 1);
			ray->c.r += mat->refl.r * ray->refl_ray->c.r;
			ray->c.g += mat->refl.g * ray->refl_ray->c.g;
			ray->c.b += mat->refl.b * ray->refl_ray->c.b;
		}
	}
	free(position);
	free(normal);
	clamp_color(&ray->c);
	
	return 1;
}

void destroy_node(ray_node * node)
{
	if (node->refl_ray)
	{
		destroy_node(node->refl_ray);
	}
	else if (node->shad_ray)
	{
		free(node->shad_ray);
	}
	free(node);
}

int check_collide(ray_d * ray, scene * scn, vec_d * position, vec_d * normal, material ** mat)
{
	int i;
	double min_dist = DBL_MAX;
	vec_d * intersection = (vec_d *) malloc(sizeof(vec_d));
	double dist_to_intersection;
	for (i = 0; i < scn->sphere_count; i++)
	{
		sphere * sph = scn->spheres[i];
		if (sphere_collide(ray, scn->spheres[i], intersection) && 
			(dist_to_intersection = vec_distance(&ray->pos, intersection)) < min_dist)
		{
			*position = *intersection;
			*normal = get_sphere_normal(sph, intersection);
			min_dist = dist_to_intersection;
			*mat = sph->mat;
		}
	}
	for (i = 0; i < scn->triangle_count; i++)
	{
		triangle * tri = scn->triangles[i];
		if (triangle_collide(ray, scn->triangles[i], intersection) && 
			(dist_to_intersection = vec_distance(&ray->pos, intersection)) < min_dist)
		{
			*position = *intersection;
			*normal = get_triangle_normal(tri, intersection, &ray->pos);
			min_dist = dist_to_intersection;
			*mat = tri->mat;
		}
	}
	free(intersection);
	if (min_dist == DBL_MAX)
	{
		return 0;
	}
	return 1;
}

int check_shadow_collide(ray_d * s_ray, scene * scn)
{
	vec_d position;
	int i;
	for (i = 0; i < scn->sphere_count; i++)
	{
		sphere * sph = scn->spheres[i];
		if (sphere_collide(s_ray, sph, &position))
		{
			return 1;
		}
	}
	for (i = 0; i < scn->triangle_count; i++)
	{
		triangle * tri = scn->triangles[i];
		if (triangle_collide(s_ray, tri, &position))
		{
			return 1;
		}
	}
	return 0;
}

int sphere_collide(ray_d * ray, sphere * sph, vec_d * position)
{
	vec_d oc = sub_vecs(&sph->center, &ray->pos);
	double oc_mag = vec_magnitude(&oc);
	int inside_sphere = oc_mag < sph->radius;
	double closest_dist = dot(&ray->dir, &oc);
	if (closest_dist < 0 && !inside_sphere)
	{
		return 0;
	}
	double dist_to_sphere_sq = sph->radius * sph->radius - oc_mag * oc_mag + closest_dist * closest_dist;
	if (dist_to_sphere_sq < 0)
	{
		return 0;
	}
	double intersect_dist;
	if (inside_sphere)
	{
		intersect_dist = closest_dist + sqrt(dist_to_sphere_sq);
	}
	else
	{
		intersect_dist = closest_dist - sqrt(dist_to_sphere_sq);
	}

	vec_d intersect_vec = vec_mult(&ray->dir, intersect_dist);
	*position = sum_vecs(&ray->pos, &intersect_vec);
	return 1;
}

int triangle_collide(ray_d * ray, triangle * tri, vec_d * position)
{
	if (!plane_collide(ray, &tri->p1, &tri->normal, position))
	{
		return 0;
	}
	triangle_2D * tri_2D = (triangle_2D *) malloc(sizeof(triangle_2D));
	vec_d_2D * vec_2D = (vec_d_2D *) malloc(sizeof(vec_d_2D));
	project_2D(tri, position, tri_2D, vec_2D);

	tri_2D->p1.u -= vec_2D->u;
	tri_2D->p1.v -= vec_2D->v;
	tri_2D->p2.u -= vec_2D->u;
	tri_2D->p2.v -= vec_2D->v;
	tri_2D->p3.u -= vec_2D->u;
	tri_2D->p3.v -= vec_2D->v;

	int cross_count = 0;
	int sign = tri_2D->p1.v < 0 ? -1 : 1;

	check_cross(&tri_2D->p1, &tri_2D->p2, &cross_count, &sign);
	check_cross(&tri_2D->p2, &tri_2D->p3, &cross_count, &sign);
	check_cross(&tri_2D->p3, &tri_2D->p1, &cross_count, &sign);

	free(tri_2D);
	free(vec_2D);

	return cross_count % 2 ? 1 : 0;
}

int plane_collide(ray_d * ray, vec_d * p_point, vec_d * p_normal, vec_d * position)
{
	vec_d p_ray_vec = sub_vecs(p_point, &ray->pos);
	double denominator = dot(p_normal, &ray->dir);
	if (denominator == 0)
	{
		return 0;
	}
	double vec_param = dot(p_normal, &p_ray_vec) / denominator;
	if (vec_param < 0)
	{
		return 0;
	}
	position->x = vec_param * ray->dir.x + ray->pos.x;
	position->y = vec_param * ray->dir.y + ray->pos.y;
	position->z = vec_param * ray->dir.z + ray->pos.z;
	return 1;
}

void check_cross(vec_d_2D * p1, vec_d_2D * p2, int * cross_count, int * sign)
{
	int next_sign = p2->v < 0 ? -1 : 1;
	if (next_sign != *sign)
	{
		if (p1->u > 0 && p2->u > 0)
		{
			(*cross_count)++;
		}
		else if (p1->u > 0 || p2->u > 0)
		{
			if (p1->u - p1->v * (p2->u - p1->u) / (p2->v - p1->v) > 0)
			{
				(*cross_count)++;
			}
		}
	}
	*sign = next_sign;
}

void project_2D(triangle * tri, vec_d * vec, triangle_2D * tri_proj, vec_d_2D * vec_proj)
{
	double x_mag = fabs(tri->normal.x);
	double y_mag = fabs(tri->normal.y);
	double z_mag = fabs(tri->normal.z);
	if (x_mag > y_mag && x_mag > z_mag)
	{
		tri_proj->p1.u = tri->p1.y;
		tri_proj->p1.v = tri->p1.z;
		tri_proj->p2.u = tri->p2.y;
		tri_proj->p2.v = tri->p2.z;
		tri_proj->p3.u = tri->p3.y;
		tri_proj->p3.v = tri->p3.z;
		vec_proj->u = vec->y;
		vec_proj->v = vec->z;
	}
	else if (y_mag > z_mag)
	{
		tri_proj->p1.u = tri->p1.x;
		tri_proj->p1.v = tri->p1.z;
		tri_proj->p2.u = tri->p2.x;
		tri_proj->p2.v = tri->p2.z;
		tri_proj->p3.u = tri->p3.x;
		tri_proj->p3.v = tri->p3.z;
		vec_proj->u = vec->x;
		vec_proj->v = vec->z;
	}
	else
	{
		tri_proj->p1.u = tri->p1.x;
		tri_proj->p1.v = tri->p1.y;
		tri_proj->p2.u = tri->p2.x;
		tri_proj->p2.v = tri->p2.y;
		tri_proj->p3.u = tri->p3.x;
		tri_proj->p3.v = tri->p3.y;
		vec_proj->u = vec->x;
		vec_proj->v = vec->y;
	}
}

void calculateAmbient(color * c, material * mat, color * amb_light)
{
	c->r += mat->diff.r * amb_light->r;
	c->g += mat->diff.g * amb_light->g;
	c->b += mat->diff.b * amb_light->b;
}

void calculateDiffuse(color * c, material * mat, vec_d * normal, light * lgt)
{
	double diff_strength = max(0, dot(normal, &lgt->to_dir));
	if (diff_strength)
	{
		c->r += mat->diff.r * diff_strength * lgt->l_color.r;
		c->g += mat->diff.g * diff_strength * lgt->l_color.g;
		c->b += mat->diff.b * diff_strength * lgt->l_color.b;
	}
}

void calculateSpecular(vec_d * position, color * c, material * mat, vec_d * normal, light * lgt, camera * cam)
{
	vec_d reflection = vec_reflect(&lgt->to_dir, normal);
	vec_d pos_to_camera = sub_vecs(&cam->from, position);
	vec_normalize(&pos_to_camera);
	double spec_strength = pow(max(0, dot(&pos_to_camera, &reflection)), mat->p_const);
	if (spec_strength)
	{
		c->r += mat->spec.r * spec_strength * lgt->l_color.r;
		c->g += mat->spec.g * spec_strength * lgt->l_color.g;
		c->b += mat->spec.b * spec_strength * lgt->l_color.b;
	}
}

void clamp_color(color * c)
{
	c->r = min(1, max(0, c->r));
	c->g = min(1, max(0, c->g));
	c->b = min(1, max(0, c->b));
}
