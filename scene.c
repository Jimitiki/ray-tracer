#include <stdlib.h>
#include "scene.h"

void init_scene(scene * scn, int light_count, int sphere_count, int triangle_count)
{
	scn->cam = (camera *) malloc(sizeof(camera));
	scn->amb_light = (color *) malloc(sizeof(vec_d));
	scn->bg_color = (color *) malloc(sizeof(vec_d));
	scn->lights = (light **) malloc(sizeof(light *) * light_count);
	scn->spheres = (sphere **) malloc(sizeof(sphere *) * sphere_count);
	scn->triangles = (triangle **) malloc(sizeof(triangle *) * triangle_count);
	scn->light_count = light_count;
	scn->sphere_count = sphere_count;
	scn->triangle_count = triangle_count;
}

void destroy_scene_counts(scene * scn, int light_count, int sphere_count, int triangle_count)
{
	free(scn->cam);
	free(scn->amb_light);
	free(scn->bg_color);
	int i;
	for (i = 0; i < light_count; i++)
	{
		free(scn->lights[i]);
	}
	free(scn->lights);
	for (i = 0; i < sphere_count; i++)
	{
		free(scn->spheres[i]->mat);
		free(scn->spheres[i]);
	}
	free(scn->spheres);
	for (i = 0; i < triangle_count; i++)
	{
		free(scn->triangles[i]->mat);
		free(scn->triangles[i]);
	}
	free(scn->triangles);
}

void destroy_scene(scene * scn)
{
	free(scn->cam);
	free(scn->amb_light);
	free(scn->bg_color);
	int i;
	for (i = 0; i < scn->light_count; i++)
	{
		free(scn->lights[i]);
	}
	free(scn->lights);
	for (i = 0; i < scn->sphere_count; i++)
	{
		free(scn->spheres[i]->mat);
		free(scn->spheres[i]);
	}
	free(scn->spheres);
	for (i = 0; i < scn->triangle_count; i++)
	{
		free(scn->triangles[i]->mat);
		free(scn->triangles[i]);
	}
	free(scn->triangles);
}

void init_material(material * mat)
{
	mat->refl.r = 0;
	mat->refl.g = 0;
	mat->refl.b = 0;
	mat->diff.r = 0;
	mat->diff.g = 0;
	mat->diff.b = 0;
	mat->spec.r = 0;
	mat->spec.g = 0;
	mat->spec.b = 0;
	mat->p_const = 0;
}

vec_d get_sphere_normal(sphere * sph, vec_d * position)
{
	vec_d normal = sub_vecs(position, &sph->center);
	normal.x /= sph->radius;
	normal.y /= sph->radius;
	normal.z /= sph->radius;
	return normal;
}

vec_d get_triangle_normal(triangle * tri, vec_d * position, vec_d * origin)
{
	vec_d intersect = sub_vecs(position, origin);
	if (dot(&tri->normal, &intersect) < 0)
	{
		return tri->normal;
	}
	return vec_neg(&tri->normal);
}

void calculate_triangle_normal(triangle * tri)
{
	vec_d v1 = sub_vecs(&tri->p1, &tri->p2);
	vec_d v2 = sub_vecs(&tri->p3, &tri->p2);
	tri->normal = vec_cross(&v1, &v2);
	vec_normalize(&tri->normal);
}