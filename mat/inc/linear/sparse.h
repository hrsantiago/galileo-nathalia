#pragma once

namespace mat
{
	void plot(const double*, const int*, const int*, unsigned, const char* = "");	
	void print(const double*, const int*, const int*, unsigned, const char* = "");
	
	double trace(const double*, const int*, const int*, unsigned);
	double value(const double*, const int*, const int*, unsigned, unsigned);
	
	double& index(double*, const int*, const int*, unsigned, unsigned);
	
	double* convert(double*, const double*, const int*, const int*, unsigned);
	double* convert(double*, const int*, const int*, const double*, unsigned);
}
