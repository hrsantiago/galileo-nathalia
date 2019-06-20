#pragma once

#include "Analysis/Solvers/Nonlinear.h"

namespace fea
{
	namespace analysis
	{
		namespace solvers
		{
			class Minimum : public Nonlinear
			{
			public:
				//constructors
				Minimum(void);

				//destructor
				virtual ~Minimum(void) override;
				
				//serialization
				virtual void load(FILE*) override;
				virtual void save(FILE*) const override;

			public:
				//type
				virtual solvers::type type(void) const override;

				//sets
				virtual unsigned state_set(void) const override;
				virtual unsigned force_set(void) const override;
				virtual unsigned tangent_set(void) const override;

			protected:
				//analysis
				virtual void run(void) override;
				
				virtual void record(void) override;
				virtual void finish(void) const override;
				
				virtual void compute_residue(void);
				virtual void compute_corrector(void);
				
				//attributes
				double m_penalty;
				double m_increment;
			};
		}
	}
}
