#pragma once

#include "Analysis/Solvers/Time.h"

namespace fea
{
	namespace analysis
	{
		namespace solvers
		{
			class Dynamic_Linear : public Time
			{
			public:
				//constructors
				Dynamic_Linear(void);

				//destructor
				virtual ~Dynamic_Linear(void) override;
				
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
				virtual void prepare(void) override;
				
				virtual void run(void) override;
				
				virtual void compute_predictor(void);
				virtual void compute_corrector(void);
				virtual void compute_reactions(void);
				virtual void compute_acceleration(void);
				virtual void compute_system_matrix(void);
			};
		}
	}
}
