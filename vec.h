#ifndef VEC_H_
#define VEC_H_

/**
* a 3-vector of doubles
*/
typedef struct
{
	double x;
	double y;
	double z;
} vec_d;

vec_d vec_mult(vec_d * v, double m);

/**
* Subtracts two vectors and calculates the magnitude of the result.
*
* @param vec_d * v1 The first vector
* @param vec_d * v2 The second vector
*
* @return double magnitude of the difference of the two vectors
*/
double vec_distance(vec_d * v1, vec_d * v2);

/**
* Calculates the magnitude of a vector
* 
* @param vec_d * vec a 3D vector of doubles
*
* @return double the magnitude 
*/
double vec_magnitude(vec_d * vec);

/**
* Normalizes a vector
*
* @param vec_d * vec a 3D vector of doubles
*/
void vec_normalize(vec_d * vec);

/**
* subtracts two vectors
*
* @param vec_d * v1 The first vector
* @param vec_d * v2 The second vector
*
* @return double the difference between the two vectors
*/
vec_d sub_vecs(vec_d * v1, vec_d * v2);

/**
* Subs two vectors together
*
* @param vec_d * v1
* @param vec_d * v2
*
* @return vec_d * The sum of v1 and v2
*/
vec_d sum_vecs(vec_d * v1, vec_d * v2);

/**
* Calculates the cross product of two vectors
*
* @param vec_d * v1
* @param vec_d * v2
*
* @return vec_d the cross product of v1 and v2
*/
vec_d vec_cross(vec_d * v1, vec_d * v2);

/**
* Calculates the dot product of two vectors
*
* @param vec_d p1 The first vector
* @param vec_d p2 The second vector
*
* @return double the dot product of both vectors
*/
double dot(vec_d * v1, vec_d * v2);

/**
* negates a vector
*
* @param vec_d * vec the vector
*
* @return vec_d negated vector
*/
vec_d vec_neg(vec_d * vec);

/**
* Reflects a vector about a normal
*
* @param vec_d * vec a vector that points towards the reflective surface
* @param vec_d * normal
*
* @return vec_d the reflected vector
*/
vec_d vec_reflect(vec_d * vec, vec_d * normal);

#endif
