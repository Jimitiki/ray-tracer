#include <math.h>
#include "vec.h"

vec_d vec_mult(vec_d * v, double m)
{
	vec_d res = *v;
	res.x *= m;
	res.y *= m;
	res.z *= m;
	return res;
}

double vec_distance(vec_d * v1, vec_d * v2)
{
	return sqrt(pow(v1->x - v2->x, 2) + pow(v1->y - v2->y, 2) + pow(v1->z - v2->z, 2));
}

double vec_magnitude(vec_d * vec)
{
	return sqrt(pow(vec->x, 2) + pow(vec->y, 2) + pow(vec->z, 2));
}

void vec_normalize(vec_d * vec)
{
	double magnitude = vec_magnitude(vec);
	if (magnitude)
	{
		vec->x /= magnitude;
		vec->y /= magnitude;
		vec->z /= magnitude;
	}
}

vec_d sub_vecs(vec_d * v1, vec_d * v2)
{
	vec_d diff;
	diff.x = v1->x - v2->x;
	diff.y = v1->y - v2->y;
	diff.z = v1->z - v2->z;
	return diff;
}

vec_d sum_vecs(vec_d * v1, vec_d * v2)
{
	vec_d sum;
	sum.x = v1->x + v2->x;
	sum.y = v1->y + v2->y;
	sum.z = v1->z + v2->z;
	return sum;
}

double dot(vec_d * v1, vec_d * v2)
{
	return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
}

vec_d vec_neg(vec_d * vec)
{
	vec_d vec_n;
	vec_n.x = vec->x * -1;
	vec_n.y = vec->y * -1;
	vec_n.z = vec->z * -1;
	return vec_n;
}

vec_d vec_cross(vec_d * v1, vec_d * v2)
{
	vec_d cross;
	cross.x = v1->y * v2->z - v1->z * v2->y;
	cross.y = v1->z * v2->x - v1->x * v2->z;
	cross.z = v1->x * v2->y - v1->y * v2->x;
	return cross;
}

vec_d vec_reflect(vec_d * vec, vec_d * normal)
{
	vec_d v = vec_mult(normal, 2 * dot(normal, vec));
	vec_d reflection = sub_vecs(&v, vec);
	return reflection;
}