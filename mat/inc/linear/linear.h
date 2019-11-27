#pragma once

namespace mat
{
	//dense
	double determinant(const double*, unsigned);
	
	double* inverse(double*, unsigned);
	double* inverse(double*, const double*, unsigned);
	
	double* inverse_lower(double*, unsigned);
	double* inverse_upper(double*, unsigned);
	double* inverse_lower(double*, const double*, unsigned);
	double* inverse_upper(double*, const double*, unsigned);
	
	double* cholesky(double*, unsigned);
	
	double* eigen(double*, double*, double*, unsigned, double);
	double* eigen(double*, double*, double*, double*, unsigned, double);
	
	double* solve(double*, const double*, const double*, unsigned, unsigned = 1);
	double* solve_lower(double*, const double*, const double*, unsigned, unsigned = 1);
	double* solve_upper(double*, const double*, const double*, unsigned, unsigned = 1);
	
	double* givens(double*, unsigned, unsigned, unsigned, double, unsigned);

	void gauss(double*, double*, unsigned, unsigned, unsigned);
	bool pivot(double*, double*, double, unsigned, unsigned, unsigned);

	void offdiag(const double*, double*, unsigned*, unsigned);
	void offdiag_row(const double*, double*, unsigned*, unsigned, unsigned);
	void offdiag_col(const double*, double*, unsigned*, unsigned, unsigned);
	void offdiag_find(double&, unsigned&, unsigned&, const double*, const unsigned*, unsigned);
}
