#pragma once

namespace mat
{
	class vec3;
	class mat3;
	double dot(const vec3& x, const vec3& y);
	vec3 cross(const vec3& x, const vec3& y);
	mat3 outer(const vec3& x, const vec3& y);
}
