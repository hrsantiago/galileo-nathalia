#include "Plot/Sizes.h"

namespace fea
{
	namespace plot
	{
		//constructors
		Sizes::Sizes(void)
		{
			return;
		}
		
		//destructor
		Sizes::~Sizes(void)
		{
			return;
		}
		
		//serialization
		void Sizes::load(FILE* file)
		{
			fscanf(file, "%lf", &m_loads);
			fscanf(file, "%lf", &m_joints);
			fscanf(file, "%lf", &m_supports);
		}
		void Sizes::save(FILE* file) const
		{
			fprintf(file, "%.2f ", m_loads);
			fprintf(file, "%.2f ", m_joints);
			fprintf(file, "%.2f ", m_supports);
			fprintf(file, "\n");
		}
		
		//data
		double Sizes::loads(double loads)
		{
			return m_loads = loads;
		}
		double Sizes::joints(double joints)
		{
			return m_joints = joints;
		}
		double Sizes::supports(double supports)
		{
			return m_supports = supports;
		}
		
		double Sizes::loads(void) const
		{
			return m_loads;
		}
		double Sizes::joints(void) const
		{
			return m_joints;
		}
		double Sizes::supports(void) const
		{
			return m_supports;
		}
		
		//info
		void Sizes::info(void) const
		{
			printf("Sizes\n");
			printf("\tloads:\t\t%.2f\n", m_loads);
			printf("\tjoints:\t\t%.2f\n", m_joints);
			printf("\tsupports:\t%.2f\n", m_supports);
		}
		
		//config
		void Sizes::reset(void)
		{
			m_loads = 0.05;
			m_joints = 0.03;
			m_supports = 0.03;
		}
	}
}
