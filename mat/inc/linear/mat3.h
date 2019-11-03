#pragma once

namespace mat
{
	class vec3;
	
	class mat3
	{
	public:
		//constructors
		mat3(void);
		mat3(double*);
		mat3(const mat3&);
		mat3(const double*);

		//destructor
		virtual ~mat3(void);
		
		//data
		double* mem(void);
		const double* mem(void) const;
		
		//operators
		mat3 operator+(void) const;
		mat3 operator-(void) const;
		
		mat3 operator/(double) const;
		mat3 operator+(const mat3&) const;
		mat3 operator-(const mat3&) const;
		mat3 operator*(const mat3&) const;
		vec3 operator*(const vec3&) const;
		
		mat3& operator+=(double);
		mat3& operator-=(double);
		mat3& operator*=(double);
		mat3& operator/=(double);
		
		mat3& operator+=(const mat3&);
		mat3& operator-=(const mat3&);
		
		double& operator[](unsigned);
		double& operator()(unsigned);
		double& operator()(unsigned, unsigned);
		
		const double& operator[](unsigned) const;
		const double& operator()(unsigned) const;
		const double& operator()(unsigned, unsigned) const;
		
		//linear
		vec3 eigen(void) const;
		mat3 inverse(void) const;
		mat3 transpose(void) const;
		
		double trace(void) const;
		double lode_angle(void) const;
		double determinant(void) const;
		double invariant_I1(void) const;
		double invariant_I2(void) const;
		double invariant_I3(void) const;
		double deviatoric_J2(void) const;
		double deviatoric_J3(void) const;
		
		//friends
		friend mat3 operator*(double, const mat3&);
		
	private:
		//attributes
		double* m_ptr;
		double m_mem[9];
		const double* m_ref;
	};
}
