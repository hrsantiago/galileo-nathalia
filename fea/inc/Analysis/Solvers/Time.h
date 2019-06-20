#pragma once

#include "Analysis/Solvers/Solver.h"

namespace fea
{
	namespace analysis
	{
		namespace solvers
		{
			//enumerations
			enum class damping : unsigned
			{
				rayleigh = 1 << 0,
				formulation = 1 << 1
			};

			//structs
			struct newmark
			{
				//serialization
				void load(FILE*);
				void save(FILE*) const;

				//attributes
				double beta = 0.25;
				double gamma = 0.50;
			};
			struct rayleigh
			{
				//serialization
				void load(FILE*);
				void save(FILE*) const;

				//attributes
				double beta = 0;
				double alpha = 0;
			};
			
			//class
			class Time : public virtual Solver
			{
			protected:
				//constructors
				Time(void);

				//destructor
				virtual ~Time(void) override;

				//serialization
				virtual void load(FILE*) override;
				virtual void save(FILE*) const override;

			public:
				//data
				bool time_adjust(void) const;
				bool time_adjust(bool);
				
				unsigned damping(void) const;
				unsigned damping(unsigned);
				
				solvers::newmark& newmark(void);
				solvers::rayleigh& rayleigh(void);

			protected:
				//analysis
				virtual void record(void) override;
				
				virtual void finish(void) const override;

				//attributes
				bool m_time_adjust;

				unsigned m_damping;

				double m_euller;
				solvers::newmark m_newmark;
				solvers::rayleigh m_rayleigh;
			};
		}
	}
}
