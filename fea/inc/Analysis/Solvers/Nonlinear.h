#pragma once

#include "Analysis/Solvers/Solver.h"

namespace fea
{
	namespace analysis
	{
		namespace solvers
		{
			//enumerations
			enum class mode : unsigned
			{
				full,
				modified
			};
			enum class convergence
			{
				fixed = 1 << 0,
				force = 1 << 1,
				state = 1 << 2
			};
			
			//class
			class Nonlinear : public virtual Solver
			{
			protected:
				//constructors
				Nonlinear(void);

				//destructor
				virtual ~Nonlinear(void) override;

				//serialization
				virtual void load(FILE*) override;
				virtual void save(FILE*) const override;

			public:
				//data
				double tolerance(void) const;
				double tolerance(double);
				
				unsigned convergence(void) const;
				unsigned convergence(unsigned);
				
				unsigned attempt_max(void) const;
				unsigned attempt_max(unsigned);
				
				unsigned iteration_max(void) const;
				unsigned iteration_max(unsigned);
				
				unsigned iteration_desired(void) const;
				unsigned iteration_desired(unsigned);
				
				solvers::mode mode(void) const;
				solvers::mode mode(solvers::mode);

				//loops
				unsigned attempt(void) const;
				unsigned iteration(void) const;

				//convergence
				bool equilibrium(void);

			protected:
				//analysis
				virtual bool stop(bool) const override;
				
				//attributes
				bool m_equilibrium;

				double m_tolerance;

				unsigned m_attempt;
				unsigned m_attempt_max;
				unsigned m_iteration;
				unsigned m_iteration_max;
				unsigned m_iteration_desired;
				unsigned m_convergence;
				unsigned m_criticals;
				
				solvers::mode m_mode;
			};
		}
	}
}
