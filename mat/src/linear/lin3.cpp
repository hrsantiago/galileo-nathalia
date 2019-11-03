//mat
#include "linear/vec3.h"
#include "linear/mat3.h"
#include "linear/lin3.h"

namespace mat
{
	double dot(const vec3& x, const vec3& y)
	{
		return x.dot(y);
	}
	vec3 cross(const vec3& x, const vec3& y)
	{
		return x.cross(y);
	}
	mat3 outer(const vec3& x, const vec3& y)
	{
		return x.outer(y);
	}
}
