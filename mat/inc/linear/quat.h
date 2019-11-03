#pragma once

namespace mat
{
	class vec3;
	
	class quat
	{
	public:
		//constructors
		quat(void);
		quat(double*);
		quat(const quat&);
		quat(const double*);
		quat(double, unsigned);
		quat(double, const vec3&);
		quat(double, double, double, double);

		//destructor
		virtual ~quat(void);
		
		//operators
		quat& operator=(const quat&);
		quat& operator*=(const quat&);
		
		quat operator*(const quat&) const;
		
		double& operator[](unsigned);
		double& operator()(unsigned);
		
		const double& operator[](unsigned) const;
		const double& operator()(unsigned) const;
		
		//clear
		quat& clear(void);
		
		//views
		quat& view_x(void);
		quat& view_y(void);
		quat& view_z(void);
		quat& isometric(unsigned);
		
		//linear
		vec3 axial(void) const;
		double angle(void) const;
		
		quat conjugate(void) const;
		vec3 rotate(const vec3&) const;
		double* rotation(double*) const;
		
	private:
		//attributes
		double* m_ptr;
		double m_mem[4];
		const double* m_ref;
	};
}
