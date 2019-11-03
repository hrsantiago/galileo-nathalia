#pragma once

namespace mat
{
	class quat;
	class mat3;
	
	class vec3
	{
	public:
		//constructors
		vec3(void);
		vec3(double*);
		vec3(const vec3&);
		vec3(const double*);
		vec3(double, double, double);

		//destructor
		virtual ~vec3(void);
		
		//data
		double* mem(void);
		const double* mem(void) const;
		
		//operators
		vec3 operator+(void) const;
		vec3 operator-(void) const;
		
		vec3 operator/(double) const;
		vec3 operator+(const vec3&) const;
		vec3 operator-(const vec3&) const;
		
		vec3& operator+=(double);
		vec3& operator-=(double);
		vec3& operator*=(double);
		vec3& operator/=(double);
		
		vec3& operator=(const vec3&);
		vec3& operator+=(const vec3&);
		vec3& operator-=(const vec3&);
		
		double& operator[](unsigned);
		double& operator()(unsigned);
		
		const double& operator[](unsigned) const;
		const double& operator()(unsigned) const;
		
		//linear
		vec3& clear(void);
		vec3& normalize(void);
		vec3& project(const vec3&);
		
		mat3 spin(void) const;
		quat rotation(void) const;
		mat3 projection(void) const;
		
		double norm(void) const;
		double dot(const vec3&) const;
		mat3 outer(const vec3&) const;
		vec3 cross(const vec3&) const;
		
		//friends
		friend vec3 operator*(double, const vec3&);
		
	private:
		//attributes
		double* m_ptr;
		double m_mem[3];
		const double* m_ref;
	};
}
