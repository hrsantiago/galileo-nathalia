#include <cstring>

#include "Plot/Gradient.h"

namespace fea
{
	namespace plot
	{
		//constructors
		Gradient::Gradient(void)
		{
			return;
		}
		
		//destructor
		Gradient::~Gradient(void)
		{
			return;
		}
		
		//serialization
		void Gradient::load(FILE* file)
		{
			double* data[] = {
				m_color_min,
				m_color_max,
				m_color_mean
			};
			for(unsigned i = 0; i < sizeof(data) / sizeof(double*); i++)
			{
				for(unsigned j = 0; j < 4; j++)
				{
					fscanf(file, "%lf", &data[i][j]);
				}
			}
		}
		void Gradient::save(FILE* file) const
		{
			const double* data[] = {
				m_color_min,
				m_color_max,
				m_color_mean
			};
			for(unsigned i = 0; i < sizeof(data) / sizeof(double*); i++)
			{
				for(unsigned j = 0; j < 4; j++)
				{
					fprintf(file, "%.2f ", data[i][j]);
				}
				fprintf(file, "\n");
			}
		}
		
		//data
		double* Gradient::color_min(double* color_min)
		{
			return (double*) memcpy(m_color_min, color_min, 4 * sizeof(double));
		}
		double* Gradient::color_max(double* color_max)
		{
			return (double*) memcpy(m_color_max, color_max, 4 * sizeof(double));
		}
		double* Gradient::color_mean(double* color_mean)
		{
			return (double*) memcpy(m_color_mean, color_mean, 4 * sizeof(double));
		}
		
		const double* Gradient::color_min(void) const
		{
			return m_color_min;
		}
		const double* Gradient::color_max(void) const
		{
			return m_color_max;
		}
		const double* Gradient::color_mean(void) const
		{
			return m_color_mean;
		}
		
		//info
		void Gradient::info(void) const
		{
			printf("Gradient\n");
			printf("\tmin:\t\t%.2f %.2f %.2f %.2f\n", m_color_min[0], m_color_min[1], m_color_min[2], m_color_min[3]);
			printf("\tmax:\t\t%.2f %.2f %.2f %.2f\n", m_color_max[0], m_color_max[1], m_color_max[2], m_color_max[3]);
			printf("\tmean:\t\t%.2f %.2f %.2f %.2f\n", m_color_mean[0], m_color_mean[1], m_color_mean[2], m_color_mean[3]);
		}
		
		//config
		void Gradient::reset(void)
		{
			double* data[] = {
				m_color_min,
				m_color_max,
				m_color_mean
			};
			const double colors[][4] = {
				{1.00, 0.00, 0.00, 0.50},
				{0.00, 0.00, 1.00, 0.50},
				{1.00, 1.00, 1.00, 0.50}
			};
			for(unsigned i = 0; i < sizeof(data) / sizeof(double*); i++)
			{
				memcpy(data[i], colors[i], 4 * sizeof(double));
			}
		}
	}
}
