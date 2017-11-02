#define _USE_MATH_DEFINES

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include "scene.h"
#include "fparser.h"
#include "ray.h"

int g_res = 1080;
char * g_file_path;
char * g_a_parse_err;
scene * scn;
int g_verbose = 0;
int view_dim;

int parse_args(int argc, char * argv[]);
int write_file(color ** pixels, int res_x, int res_y);

int main(int argc, char * argv[])
{
	if (!parse_args(argc, argv))
	{
		printf("Argument error: %s\tuse '<source name> help' to see usage\n", g_a_parse_err);
		return -1;
	}
	scene * scn = (scene *) malloc(sizeof(scene));
	if (!parse_file(g_file_path, scn))
	{
		char * error_msg = get_file_parse_error();
		printf("%s", error_msg);
		//this sucks
		free(error_msg);
		return -1;
	}

	color ** pixels = (color **) malloc(g_res * g_res * sizeof(color *));
	if (!ray_trace(scn, pixels, g_res, g_res, 2, g_verbose))
	{
		free(pixels);
		destroy_scene(scn);
		return -1;
	}
	write_file(pixels, g_res, g_res);
	for (int i = 0; i < g_res * g_res; i++)
	{
		free(pixels[i]);
	}
	free(pixels);
	destroy_scene(scn);
	return 0;
}

int parse_args(int argc, char * argv[])
{
	int i;
	if (argc < 2)
	{
		g_a_parse_err = "No file path provided";
	}
	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			if (!strcmp(argv[i], "--dimension"))
			{
				i++;
				if (i >= argc)
				{
					g_a_parse_err = "No parameter given for argument --dimension\n";
					return 0;
				}
				if (!(g_res = atoi(argv[i])))
				{
					g_a_parse_err = "Invalid parameter given for argument --dimension\n";
					return 0;
				}
			}
			else if (strcmp(argv[i], "--verbose") || strcmp(argv[i], "-v"))
			{
				g_verbose = 1;
			}
		}
		else
		{
			g_file_path = argv[i];
		}
	}
	return 1;
}

int write_file(color ** pixels, int res_x, int res_y)
{
	FILE * f = fopen("raytrace.ppm", "w");
	fprintf(f, "P3\n%d %d\n65535\n", res_x, res_y);
	int i, j, p_count = 0;
	for (i = 0; i < res_y; i++)
	{
		for (j = 0; j < res_x; j++)
		{
			fprintf(f, "%d %d %d  ", (int) (pixels[p_count]->r * 65535), (int) (pixels[p_count]->g * 65535), (int) (pixels[p_count]->b * 65535));
			p_count++;
		}
		fprintf(f, "\n");
	}
	fclose(f);
	return 1;
}