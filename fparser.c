#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fparser.h"
#include "strfuncs.h"

#define LEN_ERROR 256

char * g_parse_err;
int g_err_line_num = 0;
int g_light_count = 0;
int g_sphere_count = 0;
int g_triangle_count = 0;

/**
* Does a preliminary check to calculate the intial memory required for lights, spheres, and triangles
*
* @param char * file_path the location of the file
*
* @return int 0 if it fails, positive number if it succeeds
*/
int get_counts(char * file_path);

/**
* Reads, parses, and handles the data from on single line
*
* @param char * file_path the location of the file
* @param color ** pixels where to insert the result
*
* @return int 0 if it fails, positive number if it succeeds
*/
int parse_line(char * line, int line_num, scene * scn);

/**
* These functions set the values of a scene based on the text read from a rayTracing file
*
* @param char ** strs array of strings that contain all of the parameters from the line in the rayTracing file
* @param int w_count size of strs
* @param scene * scn a scene in which the values will be stored
*
* @return int 0 if it fails, positive number if it succeeds
*/
int parse_fov(char ** strs, int w_count, scene * scn);
int parse_light(char ** strs, int w_count, scene * scn);
int parse_sphere(char ** strs, int w_count, scene * scn);
int parse_triangle(char ** strs, int w_count, scene * scn);


/**
* Similar to the previous functions. The next 2 functions only differ in the name of the 3rd parameter type
*
* @param char ** strs array of strings holding the values to be inserted into the struct
* @param int w_count length of strs
* @param vec_d/color * pointer to the struct of three doubles which hold the parsed line
*
* @return int 0 if it fails, positive number if it succeeds
*/
int parse_vec_d(char ** strs, int w_count, vec_d * vec);
int parse_color(char ** strs, int w_count, color * c);

int parse_3vec(char ** strs, int w_count, double * v1, double * v2, double * v3);
int parse_double(char ** strs, int w_count, double * d);
/**
* Parses properties of an object material.
*
* @param char ** strs array of material properties
* @param int w_count length of strs
* @param material * mat material where the properties will be stored
*
* @return int 0 if it fails, positive number if it succeeds
*/
int parse_material(char ** strs, int w_count, material * mat);

int parse_file(char * file_path, scene * out_scene)
{
	g_parse_err = (char *) malloc(LEN_ERROR);
	if (!get_counts(file_path))
	{
		return 0;
	}
	FILE * f = fopen(file_path, "r");
	if (!f)
	{
		snprintf(g_parse_err, LEN_ERROR, "Could not find ray trace file at '%s'\n", file_path);
		return 0;
	}
	init_scene(out_scene, g_light_count, g_sphere_count, g_triangle_count);
	//now use counts to keep track of how many objects have been intialized
	g_light_count = 0;
	g_sphere_count = 0;
	g_triangle_count = 0;
	char line[128];
	int line_num = 1;
	while (fgets(line, sizeof(line), f))
	{
		if (!parse_line(line, line_num, out_scene))
		{
			g_err_line_num = line_num;
			destroy_scene_counts(out_scene, g_light_count, g_sphere_count, g_triangle_count);
			return 0;
		}
		line_num++;
	}
	fclose(f);
	free(g_parse_err);
	return 1;
}

int get_counts(char * file_path)
{
	FILE * f = fopen(file_path, "r");
	char line[128];
	if (!f)
	{
		snprintf(g_parse_err, LEN_ERROR, "Could not find ray trace file at '%s'\n", file_path);
		return 0;
	}
	while (fgets(line, sizeof(line), f))
	{
		char * word = substr(line, ' ');
		if (!strcmp(word, "DirectionToLight") || !strcmp(word, "LightColor"))
		{
			g_light_count++;
		}
		else if (!strcmp(word, "Sphere"))
		{
			g_sphere_count++;
		}
		else if (!strcmp(word, "Triangle"))
		{
			g_triangle_count++;
		}
		free(word);
	}
	fclose(f);
	return 1;
}

char * get_file_parse_error()
{
	char * error_msg = (char *) malloc(LEN_ERROR);
	memcpy(error_msg, g_parse_err, LEN_ERROR);
	if (g_err_line_num)
	{
		snprintf(g_parse_err, LEN_ERROR, "File parse error (%d): %s", g_err_line_num, error_msg);
	}
	else
	{
		snprintf(g_parse_err, LEN_ERROR, "File parse error: %s", error_msg);
	}
	free(error_msg);
	return g_parse_err;
}

int parse_line(char * line, int line_num, scene * scn)
{
	int w_count;
	char ** split_line = strsplit(line, ' ', &w_count);
	if (!strcmp(split_line[0], "CameraLookAt"))
	{
		if (!parse_vec_d(split_line, w_count, &scn->cam->at))
		{
			return 0;
		}
	}
	else if (!strcmp(split_line[0], "CameraLookFrom"))
	{
		if (!parse_vec_d(split_line, w_count, &scn->cam->from))
		{
			return 0;
		}
	}
	else if (!strcmp(split_line[0], "CameraLookUp"))
	{
		if (!parse_vec_d(split_line, w_count, &scn->cam->up))
		{
			return 0;
		}
	}
	else if (!strcmp(split_line[0], "FieldOfView"))
	{
		if (!parse_fov(split_line, w_count, scn))
		{
			return 0;
		}
	}
	else if (!strcmp(split_line[0], "AmbientLight"))
	{
		if (!parse_color(split_line, w_count, scn->amb_light))
		{
			return 0;
		}
	}
	else if (!strcmp(split_line[0], "BackgroundColor"))
	{
		if (!parse_color(split_line, w_count, scn->bg_color))
		{
			return 0;
		}
	}
	else if (!strcmp(split_line[0], "DirectionToLight") || !strcmp(split_line[0], "LightColor"))
	{
		if (!parse_light(split_line, w_count, scn))
		{
			return 0;
		}
	}
	else if (!strcmp(split_line[0], "Sphere"))
	{
		if (!parse_sphere(split_line, w_count, scn))
		{
			return 0;
		}
	}
	else if (!strcmp(split_line[0], "Triangle"))
	{
		if (!parse_triangle(split_line, w_count, scn))
		{
			return 0;
		}
	}
	int i;
	for (i = 0; i < w_count; i++)
	{
		free(split_line[i]);
	}
	free(split_line);
	return 1;
}

int parse_fov(char ** strs, int w_count, scene * scn)
{
	return parse_double(strs, w_count, &scn->fov);
}

int parse_light(char ** strs, int w_count, scene * scn)
{
	if (w_count != 8)
	{
		snprintf(g_parse_err, LEN_ERROR, "Invalid number of parameters given for directional light.");
		return 0;
	}
	errno = 0;
	char * e = NULL;
	int i = 0;
	light * l = (light *) malloc(sizeof(light));
	while (i < 8)
	{
		if (!strcmp(strs[i], "DirectionToLight"))
		{
			if (!parse_vec_d(strs + i, 4, &l->to_dir))
			{
				free(l);
				return 0;
			}
		}
		else if (!strcmp(strs[i], "LightColor"))
		{
			if (!parse_color(strs + i, 4, &l->l_color))
			{
				free(l);
				return 0;
			}
		}
		else
		{
			snprintf(g_parse_err, LEN_ERROR, "Invalid parameter given for directional light: %s", strs[i]);
			free(l);
			return 0;
		}
		i += 4;
	}
	vec_normalize(&l->to_dir);
	scn->lights[g_light_count] = l;
	g_light_count++;
	return 1;
}
int parse_sphere(char ** strs, int w_count, scene * scn)
{
	if (w_count < 7 || w_count > 7 && strcmp(strs[7], "Material"))
	{
		snprintf(g_parse_err, LEN_ERROR, "Invalid number of parameters given for Sphere");
		return 0;
	}
	int i = 1;
	sphere * s = (sphere *) malloc(sizeof(sphere));
	char * e = NULL;
	errno = 0;
	while (i < 7)
	{
		if (!strcmp(strs[i], "Center"))
		{
			if (!parse_vec_d(strs + i, 4, &s->center))
			{
				free(s);
				return 0;
			}
			i += 4;
		}
		else if (!strcmp(strs[i], "Radius"))
		{
			if (!parse_double(strs + i, 2, &s->radius))
			{
				free(s);
				return 0;
			}
			i += 2;
		}
		else
		{
			snprintf(g_parse_err, LEN_ERROR, "Invalid parameter given for Sphere: %s", strs[i]);
			free(s);
			return 0;
		}
	}
	s->mat = (material *) malloc(sizeof(material));
	if (w_count == 7)
	{
		if (!parse_material(NULL, 0, s->mat))
		{
			free(s->mat);
			free(s);
			return 0;
		}
	}
	else if (!parse_material(strs + 7, w_count - 7, s->mat))
	{
		free(s->mat);
		free(s);
		return 0;
	}
	scn->spheres[g_sphere_count] = s;
	g_sphere_count++;
	return 1;
}

int parse_triangle(char ** strs, int w_count, scene * scn)
{
	if (w_count < 10 || w_count > 10 && strcmp(strs[10], "Material"))
	{
		snprintf(g_parse_err, LEN_ERROR, "Invalid number of parameters given for Triangle");
		return 0;
	}
	int i = 0;
	char * e = NULL;
	triangle * t = (triangle *) malloc(sizeof(triangle));
	if (!strtodouble(strs[++i], &(t->p1.x), e) ||
		!strtodouble(strs[++i], &(t->p1.y), e) ||
		!strtodouble(strs[++i], &(t->p1.z), e) ||
		!strtodouble(strs[++i], &(t->p2.x), e) ||
		!strtodouble(strs[++i], &(t->p2.y), e) ||
		!strtodouble(strs[++i], &(t->p2.z), e) ||
		!strtodouble(strs[++i], &(t->p3.x), e) ||
		!strtodouble(strs[++i], &(t->p3.y), e) ||
		!strtodouble(strs[++i], &(t->p3.z), e))
	{
		free(t);
		snprintf(g_parse_err, LEN_ERROR, "Invalid parameter given for Triangle: %s", strs[i]);
		return 0;
	}
	t->mat = (material *) malloc(sizeof(material));
	if (w_count == 10)
	{
		if (!parse_material(NULL, 0, t->mat))
		{
			free(t->mat);
			free(t);
			return 0;
		}
	}
	else if (!parse_material(strs + 10, w_count - 10, t->mat))
	{
		free(t->mat);
		free(t);
		return 0;
	}
	scn->triangles[g_triangle_count] = t;
	calculate_triangle_normal(t);
	g_triangle_count++;
	return 1;
}

int parse_vec_d(char ** strs, int w_count, vec_d * vec)
{
	return parse_3vec(strs, w_count, &vec->x, &vec->y, &vec->z);
}

int parse_color(char ** strs, int w_count, color * c)
{
	return parse_3vec(strs, w_count, &c->r, &c->g, &c->b);
}

int parse_3vec(char ** strs, int w_count, double * v1, double * v2, double * v3)
{
	if (w_count != 4)
	{
		snprintf(g_parse_err, LEN_ERROR, "Invalid number of parameters given for %s", strs[0]);
		return 0;
	}
	int i = 0;
	char * e = NULL;
	if (!strtodouble(strs[++i], v1, e) ||
		!strtodouble(strs[++i], v2, e) ||
		!strtodouble(strs[++i], v3, e))
	{
		snprintf(g_parse_err, LEN_ERROR, "Invalid parameter given for %s: %s", strs[0], strs[i]);
		return 0;
	}
	return 1;
}

int parse_double(char ** strs, int w_count, double * d)
{
	if (w_count != 2)
	{
		snprintf(g_parse_err, LEN_ERROR, "Invalid number of parameters given for %s", strs[0]);
		return 0;
	}
	char * e = NULL;
	if (!strtodouble(strs[1], d, e))
	{
		snprintf(g_parse_err, LEN_ERROR, "Invalid parameter given for %s: %s", strs[0], strs[1]);
		return 0;
	}
	return 1;
}

int parse_material(char ** strs, int w_count, material * mat)
{
	init_material(mat);
	if (w_count == 0)
	{
		return 1;
	}
	if (w_count < 4 || w_count > 15 || w_count == 4 && strcmp(strs[1], "Diffuse") && strcmp(strs[1], "Reflective"))
	{
		snprintf(g_parse_err, LEN_ERROR, "Invalid number of parameters given for Material");
		return 0;
	}
	if (strcmp(strs[0], "Material"))
	{
		snprintf(g_parse_err, LEN_ERROR, "Argument after Sphere must be a Material, was '%s' instead", strs[0]);
		return 0;
	}
	int i = 1;
	char * e = "\0";
	while (i < w_count)
	{
		if (!strcmp(strs[i], "Diffuse"))
		{
			if (!parse_color(strs + i, 4, &mat->diff))
			{
				return 0;
			}
			i += 4;
		}
		else if (!strcmp(strs[i], "SpecularHighlight"))
		{
			if (!parse_color(strs + i, 4, &mat->spec))
			{
				return 0;
			}
			i += 4;
		}
		else if (!strcmp(strs[i], "PhongConstant"))
		{
			if (!parse_double(strs + i, 2, &mat->p_const))
			{
				return 0;
			}
			i += 2;
		}
		else if (!strcmp(strs[i], "Reflective"))
		{
			if (!parse_color(strs + i, 4, &mat->refl))
			{
				return 0;
			}
			i += 4;
		}
		else
		{
			snprintf(g_parse_err, LEN_ERROR, "Invalid parameter given for Material: %s", strs[i]);
			return 0;
		}
	}
	return 1;
}