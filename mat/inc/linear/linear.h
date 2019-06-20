#pragma once

namespace mat
{
	double det(double*, unsigned);
	
	double* inv(double*, unsigned);
	double* inv(double*, double*, unsigned);
	
	double* eigen(double*, double*, unsigned, double* = nullptr);
	
	double* solve(double*, double*, double*, unsigned, unsigned = 1);
	
	double* givens(double*, unsigned, unsigned, unsigned, double, unsigned);
	
	double* decompose_ll(double*, unsigned);
	double* decompose_lu(double*, unsigned);
	double* decompose_qr(double*, unsigned);
	double* decompose_lup(double*, unsigned, unsigned*);
	double* decompose_ldl(double*, unsigned, unsigned*);
	
	double* substitute_ll(double*, const double*, const double*, unsigned);
	double* substitute_lu(double*, const double*, const double*, unsigned);
	double* substitute_qr(double*, const double*, const double*, unsigned);
	double* substitute_lup(double*, const double*, const double*, unsigned*, unsigned);
	double* substitute_ldl(double*, const double*, const double*, unsigned*, unsigned);

	bool positive_definite(const double*, unsigned);
	bool positive_definite(const double*, const int*, const int*, unsigned);
	
	bool diagonally_dominant(const double*, unsigned);
	bool diagonally_dominant(const double*, const int*, const int*, unsigned);
}
