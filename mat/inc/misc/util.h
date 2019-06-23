#pragma once

namespace mat
{
	int sign(int);
	int sign(bool);
	int sign(double);
	int step(double, double);

	void swap(double&, double&);
	void swap(unsigned&, unsigned&);

	void cyclic(unsigned, unsigned&, unsigned&);

	bool bit_set(unsigned, unsigned);
	unsigned char bit_find(unsigned, unsigned);
	unsigned bit_find(unsigned, unsigned, unsigned char);

	int delta(unsigned, unsigned);
	int permutation(unsigned, unsigned, unsigned);

	double randu(double = 0, double = 1);
	double bound(double, double = -1, double = +1);
	
	double neville_polynomial(const double*, const double*, unsigned, double);
	
	double* inv_color(double*);
	double* inv_color(double*, const double*);
	double* map_color(double*, const double**, double);
	
	unsigned log2(unsigned);
	
	unsigned* range(unsigned*, unsigned);
	unsigned* range(unsigned*, unsigned, unsigned);
	unsigned* range(unsigned*, unsigned, unsigned, unsigned);
	
	char* time_break(char*, double);
	
	void gauss_point(double&, double&, unsigned, unsigned);
}
