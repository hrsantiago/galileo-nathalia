//fea
#include "Models/Model.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Time.h"

namespace fea
{
	namespace analysis
	{
		namespace solvers
		{
			//newmark
			void newmark::load(FILE* file) 
			{
				fscanf(file, "%lf %lf", &gamma, &beta);
			}
			void newmark::save(FILE* file) const 
			{
				fprintf(file, "%+.6e %+.6e ", gamma, beta);
			}
			
			//rayleigh
			void rayleigh::load(FILE* file) 
			{
				fscanf(file, "%lf %lf", &alpha, &beta);
			}
			void rayleigh::save(FILE* file) const 
			{
				fprintf(file, "%+.6e %+.6e ", alpha, beta);
			}
			
			//constructors
			Time::Time(void) : m_time_adjust(false), m_euller(1), m_damping((unsigned) solvers::damping::formulation)
			{
				return;
			}

			//destructor
			Time::~Time(void)
			{
				return;
			}

			//serialization
			void Time::load(FILE* file)
			{
				fscanf(file, "%d %d %lf", &m_time_adjust, &m_damping, &m_euller);
				m_newmark.load(file);
				m_rayleigh.load(file);
			}
			void Time::save(FILE* file) const
			{
				fprintf(file, "%01d %02d %+.6e ", m_time_adjust, m_damping, m_euller);
				m_newmark.save(file);
				m_rayleigh.save(file);
			}

			//data
			bool Time::time_adjust(void) const
			{
				return m_time_adjust;
			}
			bool Time::time_adjust(bool time_adjust)
			{
				return m_time_adjust = time_adjust;
			}
			
			unsigned Time::damping(void) const
			{
				return m_damping;
			}
			unsigned Time::damping(unsigned damping)
			{
				return m_damping = damping;
			}
			
			solvers::newmark& Time::newmark(void)
			{
				return m_newmark;
			}
			solvers::rayleigh& Time::rayleigh(void)
			{
				return m_rayleigh;
			}

			//analysis
			void Time::record(void)
			{
				char formatter[200];
				sprintf(formatter, "%+.6e\n", m_t);
				m_results[0] += formatter;
			}
			void Time::finish(void) const
			{
				const std::string path = m_analysis->model()->folder() + "/Solver/Time.txt";
				FILE* file = fopen(path.c_str(), "w");
				fprintf(file, m_results[0].c_str());
				fclose(file);
			}
		}
	}
}
