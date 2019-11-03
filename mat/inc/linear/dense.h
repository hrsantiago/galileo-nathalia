#pragma once

namespace mat
{
	void print(const double*, unsigned, const char* = "");
	void print(const double*, unsigned, unsigned, const char* = "");
	
	void triad_form(const double*, double*, double*, double = 0);
	bool triad_test(const double*, const double*, const double*);
	
	double min(const double*, unsigned, unsigned* = nullptr, bool = false);
	double max(const double*, unsigned, unsigned* = nullptr, bool = false);
	
	double dot(const double*, unsigned);
	double dot(const double*, const double*, unsigned);
	double dot(const double*, const double*, const double*, unsigned);
	
	double norm(const double*, unsigned);
	double norm(const double*, const double*, unsigned);
	
	double trace(const double*, unsigned);
	double value(const double*, unsigned, unsigned, unsigned);
	
	double* set(double*, const double*, unsigned, double = 1);
	
	double* eye(double*, unsigned);
	
	double* clean(double*, unsigned);

	double* swap(double*, double*, unsigned);
	
	double& index(double*, unsigned, unsigned, unsigned);
	
	double* add(double*, double, unsigned);
	double* sub(double*, double, unsigned);
	double* mul(double*, double, unsigned);
	double* div(double*, double, unsigned);
	
	double* add(double*, const double*, unsigned, double = 1);
	double* sub(double*, const double*, unsigned, double = 1);
	double* mul(double*, const double*, unsigned, double = 1);
	double* div(double*, const double*, unsigned, double = 1);
	
	double* add(double*, const double*, const double*, unsigned, double = 1);
	double* sub(double*, const double*, const double*, unsigned, double = 1);
	double* mul(double*, const double*, const double*, unsigned, double = 1);
	double* div(double*, const double*, const double*, unsigned, double = 1);
	
	double* abs(double*, unsigned);
	double* abs(double*, const double*, unsigned);
	
	double* normal(const double*, double*, double* = nullptr);
	
	double* randu(double*, unsigned, unsigned = 1, double = 0, double = 1, bool = false);
	
	double cross(const double*, const double*, unsigned);
	double cross(const double*, const double*, const double*, unsigned);
	
	double* cross(double*, const double*, const double*);
	double* cross(double*, const double*, const double*, const double*);
	
	double* outer(double*, const double*, unsigned, double = 1);
	double* outer(double*, const double*, const double*, unsigned, double = 1);

	double* restore(double*, unsigned, bool);

	double* normalize(double*, unsigned);
	double* normalize(double*, const double*, unsigned);
	double* normalize(double*, const double*, const double*, unsigned);

	double* transpose(double*, unsigned);
	double* transpose(double*, unsigned, unsigned);
	double* transpose(double*, const double*, unsigned, unsigned);

	double* multiadd(double*, const double*, const double*, unsigned, unsigned, unsigned = 1, double = 1);
	double* multisub(double*, const double*, const double*, unsigned, unsigned, unsigned = 1, double = 1);
	double* multisym(double*, const double*, const double*, unsigned, unsigned, bool = false, double = 1);
	double* multiply(double*, const double*, const double*, unsigned, unsigned, unsigned = 1, bool = false, double = 1);
}
