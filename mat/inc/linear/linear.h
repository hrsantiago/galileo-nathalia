#pragma once

namespace mat
{
	double det(double*, unsigned);
	
	double* inv(double*, unsigned);
	double* inv(double*, double*, unsigned);
	
	double* eigen(double*, double*, unsigned, double* = nullptr);
	
	double* solve(double*, const double*, const double*, unsigned, unsigned = 1);
	
	double* givens(double*, unsigned, unsigned, unsigned, double, unsigned);

	bool positive_definite(const double*, unsigned);
	bool positive_definite(const double*, const int*, const int*, unsigned);
	
	bool diagonally_dominant(const double*, unsigned);
	bool diagonally_dominant(const double*, const int*, const int*, unsigned);
}
