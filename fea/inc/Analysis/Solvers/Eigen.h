#pragma once

#include "Analysis/Solvers/Solver.h"
#include "Analysis/Solvers/Spectre.h"

namespace fea
{
	namespace analysis
	{
		namespace solvers
		{
			class Eigen : public virtual Solver
			{
			protected:
				//constructors
				Eigen(void);

				//destructor
				virtual ~Eigen(void) override;
				
				//serialization
				virtual void load(FILE*) override;
				virtual void save(FILE*) const override;

			public:
				//data
				double scale(void) const;
				double scale(double);
				
				double tolerance(void) const;
				double tolerance(double);

				unsigned modes(void) const;
				unsigned modes(unsigned);
				
				solvers::spectre spectre(void) const;
				solvers::spectre spectre(solvers::spectre);
				
			protected:
				//solve
				void sort(void) const;
				bool eigen_std(void) const;
				bool eigen_gen(void) const;
				
				//attributes
				double m_scale;
				unsigned m_modes;
				solvers::spectre m_spectre;
				
				double m_tolerance;
			};
		}
	}
}
