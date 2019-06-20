//std
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

//mat
#include "misc/util.h"
#include "linear/matrix.h"

namespace mat
{
	void plot(const double* k, const int* r, const int* c, unsigned n, const char* s)
	{
		//open file
		FILE* file = fopen("temp.gnu", "w");
		//configuration
		fprintf(file, "unset key\n");
		fprintf(file, "set view map\n");
		fprintf(file, "set title '%s'\n", s);
		fprintf(file, "set ylabel 'rows'\n");
		fprintf(file, "set xlabel 'columns'\n");
		fprintf(file, "set xrange [0 : %d]\n", n - 1);
		fprintf(file, "set yrange [%d : 0]\n", n - 1);
		fprintf(file, "set palette rgbformula 33, 13, 10\n");
		fprintf(file, "set style fill transparent solid noborder\n");
		//plot
		fprintf(file, "splot '-' using 2 : 1 : 3 with circles lc palette\n");
		for(unsigned i = 0; i < n; i++)
		{
			for(int j = c[i]; j < c[i + 1]; j++)
			{
				fprintf(file, "%04d %04d %+.2e\n", r[j], i, k[j]);
			}
		}
		fprintf(file, "e");
		//close file
		fclose(file);
		//call gnuplot
		system("gnuplot -p 'temp.gnu'");
		//remove file
		remove("temp.gnu");
	}
	
	void print(const double* k, const int* r, const int* c, unsigned n, const char* s)
	{
		if(strlen(s) != 0)
		{
			printf("%s\n", s);
		}
		for(unsigned i = 0; i < n; i++)
		{
			for(int j = c[i]; j < c[i + 1]; j++)
			{
				printf("(%04d, %04d): %+.2e\n", r[j], i, k[j]);
			}
		}
	}
	
	double trace(const double* k, const int* r, const int* c, unsigned n)
	{
		double s = 0;
		for(int i = 0; i < int(n); i++)
		{
			for(int j = c[i]; j < c[i + 1]; j++)
			{
				if(r[j] == i)
				{
					s += k[j];
				}
			}
		}
		return s;
	}
	
	double value(const double* k, const int* r, const int* c, unsigned i, unsigned j)
	{
		for(int p = c[j]; p < c[j + 1]; p++)
		{
			if(r[p] == int(i))
			{
				return k[p];
			}
		}
		return 0;
	}

	double& index(double* k, const int* r, const int* c, unsigned i, unsigned j)
	{
		for(int p = c[j]; p < c[j + 1]; p++)
		{
			if(r[p] == int(i))
			{
				return k[p];
			}
		}
	}

	double* convert(double* m, const double* k, const int* r, const int* c, unsigned n)
	{
		memset(m, 0, n * n * sizeof(double));
		for(int i = 0; i < int(n); i++)
		{
			for(int j = c[i]; j < c[i + 1]; j++)
			{
				m[r[j] + n * i] = k[j];
			}
		}
	}
	double* convert(double* k, const int* r, const int* c, const double* m, unsigned n)
	{
		for(int i = 0; i < int(n); i++)
		{
			for(int j = c[i]; j < c[i + 1]; j++)
			{
				k[j] = m[r[j] + n * i];
			}
		}
	}
}
