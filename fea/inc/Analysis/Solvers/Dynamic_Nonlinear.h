#pragma once

#include "Analysis/Solvers/Time.h"
#include "Analysis/Solvers/Nonlinear.h"

namespace fea
{
	namespace analysis
	{
		namespace solvers
		{
			//enumerations
			enum class integration : unsigned
			{
				newmark,
				runge_kutta
			};
			
			//class
			class Dynamic_Nonlinear : public virtual Time, public virtual Nonlinear
			{
			public:
				//friends
				friend class Solver;

			protected:
				//constructors
				Dynamic_Nonlinear(void);

				//destructor
				virtual ~Dynamic_Nonlinear(void) override;

				//serialization
				virtual void load(FILE*) override;
				virtual void save(FILE*) const override;

			public:
				//data
				bool frequencies(void) const;
				bool frequencies(bool);
				solvers::integration integration(void) const;
				solvers::integration integration(solvers::integration);

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
				virtual void run_newmark(void);
				virtual void run_runge_kutta(void);
				virtual void run_frequencies(void);
				
				virtual void record(void) override;
				virtual void finish(void) const override;
				
				virtual void compute_initial(void);
				virtual void compute_residue(void);
				virtual void compute_tangent_1(void);
				virtual void compute_tangent_2(void);
				virtual void compute_tangent_3(void);
				virtual void compute_tangent_4(void);
				virtual void compute_predictor(void);
				virtual void compute_corrector(void);
				virtual void compute_reactions(void);
				virtual void compute_acceleration(void);
				virtual void compute_system_matrix(void);

				//attributes
				bool m_frequencies;
				solvers::integration m_integration;
			};
		}
	}
}
