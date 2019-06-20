#pragma once

#include "Analysis/Solvers/Solver.h"

namespace fea
{
	namespace analysis
	{
		namespace solvers
		{
			class Static_Linear : public Solver
			{
			public:
				//friends
				friend class Solver;

			protected:
				//constructors
				Static_Linear(void);

				//destructor
				virtual ~Static_Linear(void) override;

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
				
				virtual void record(void) override;
				virtual void finish(void) const override;
				
				virtual void compute_state(void);
				virtual void compute_increment(void);
				virtual void compute_reactions(void);
			};
		}
	}
}
