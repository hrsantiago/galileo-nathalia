#pragma once

#include "Analysis/Solvers/Solver.h"

namespace fea
{
	namespace analysis
	{
		namespace solvers
		{
			class Buckling : public Solver
			{
			public:
				//friends
				friend class Solver;

			protected:
				//constructors
				Buckling(void);

				//destructor
				virtual ~Buckling(void) override;
				
				//serialization
				virtual void load(FILE*) override;
				virtual void save(FILE*) const override;

			public:
				//type
				virtual solvers::type type(void) const override;

				//data
				double scale(void) const;
				double scale(double);

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
				
				//attributes
				double m_scale;
			};
		}
	}
}
