#pragma once

namespace mat
{
	//sparse
	void print(const double*, const int*, const int*, unsigned, const char* = "", bool = false);
	
	double trace(const double*, const int*, const int*, unsigned);
	double value(const double*, const int*, const int*, unsigned, unsigned);
	
	double& index(double*, const int*, const int*, unsigned, unsigned);
	
	double* mulvec(double*, const double*, const int*, const int*, const double*, unsigned);
	
	bool eigen(double&, double*, const double*, const int*, const int*, unsigned, bool = true);
	bool eigen(double&, double*, const double*, const double*, const int*, const int*, unsigned, bool = true);
	
	double* eigen(double*, double*, const double*, const int*, const int*, unsigned, unsigned);
	double* eigen(double*, double*, const double*, const double*, const int*, const int*, unsigned, unsigned);
	
	bool power_iteration(double&, double*, double*, double* e, const double*, const int*, const int*, unsigned, double, double);
	
	double* convert(double*, const double*, const int*, const int*, unsigned);
	double* convert(double*, const int*, const int*, const double*, unsigned);
}
