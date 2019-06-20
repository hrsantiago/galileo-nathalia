#pragma once

namespace mat
{
	double* spin_vector(double*, const double*);
	double* spin_matrix(double*, const double*);
	
	double* projection(double*, const double*, unsigned);
	double* projection(double*, const double*, const double*, unsigned);
	
	double* rotation_vector(double*, const double*);
	double* rotation_vector(double*, const double*, double&);
	
	double* rotation_matrix(double*, const double*);
	double* rotation_matrix(double*, const double*, double);
	double* rotation_matrix(double*, const double*, const double*, const double*);
	
	double* rotation_increment(double*, const double*);
	
	double* rotation_split(double*, double*, const double*, const double*);
}
