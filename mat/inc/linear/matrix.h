#pragma once

#include <cmath>
#include <cstdio>
#include <cstring>

#define MAT_STATIC_SIZE 36

namespace mat
{
	class vector;
	
	class matrix
	{
	public:
		//constructors
		matrix(const matrix&);
		matrix(unsigned, unsigned);
		matrix(double*, unsigned, unsigned);
		matrix(const double*, unsigned, unsigned);

		//destructor
		virtual ~matrix(void);

		//operators
		matrix operator+(void) const;
		matrix operator-(void) const;
		matrix operator/(double) const;
		
		matrix operator+(const matrix&) const;
		matrix operator-(const matrix&) const;
		vector operator*(const vector&) const;
		matrix operator*(const matrix&) const;
		
		matrix& operator=(double);
		matrix& operator=(const double*);
		matrix& operator=(const matrix&);
		
		matrix& operator+=(double);
		matrix& operator-=(double);
		matrix& operator*=(double);
		matrix& operator/=(double);
		
		matrix& operator+=(const double*);
		matrix& operator-=(const double*);
		matrix& operator+=(const matrix&);
		matrix& operator-=(const matrix&);
		
		double& operator[](unsigned);
		double& operator()(unsigned);
		double& operator()(unsigned, unsigned);
		
		const double& operator[](unsigned) const;
		const double& operator()(unsigned) const;
		const double& operator()(unsigned, unsigned) const;
		
		//data
		unsigned rows(void) const;
		unsigned cols(void) const;
		
		double* mem(void);
		const double* mem(void) const;
		
		//info
		void print(const char* = "") const;
		
		//linear
		double norm(void) const;
		double trace(void) const;
		double determinant(void) const;
		
		matrix inverse(void) const;
		matrix transpose(void) const;

		//static
		static matrix eye(unsigned, unsigned);
		static matrix zeros(unsigned, unsigned);
		
		//friends
		friend matrix operator*(double, const matrix&);
	
	private:
		//determinant
		double det1(void) const;
		double det2(void) const;
		double det3(void) const;
		double det4(void) const;
		
	protected:
		//attributes
		bool m_own;
		double* m_mem;
		unsigned m_rows;
		unsigned m_cols;
		const double* m_ref;
		double m_local[MAT_STATIC_SIZE];
	};
}
