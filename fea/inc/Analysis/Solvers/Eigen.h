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
				
				solvers::spectre spectre(void) const;
				solvers::spectre spectre(solvers::spectre);

				unsigned modes(void) const;
				
			protected:
				//solve
				bool eigen_std(void) const;
				bool eigen_gen(void) const;
				
				//attributes
				double m_scale;
				solvers::spectre m_spectre;
			};
		}
	}
}
