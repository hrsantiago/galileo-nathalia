#pragma once

#include <cfloat>
#include <climits>

#include "Analysis/Solvers/Nonlinear.h"

namespace fea
{
	namespace analysis
	{
		//declarations
		namespace solvers
		{
			enum class type : unsigned;
		}
		namespace strategies
		{
			class Strategy;
			enum class type : unsigned;
		}
		
		namespace solvers
		{
			//structs
			struct unload
			{
				bool test = false;
				double load = DBL_MAX;
				double state = DBL_MAX;
				unsigned step = UINT_MAX;
			};
			
			//class
			class Static_Nonlinear : public Nonlinear
			{
			public:
				//friends
				friend class Solver;

			protected:
				//constructors
				Static_Nonlinear(void);

				//destructor
				virtual ~Static_Nonlinear(void) override;
				
				//serialization
				virtual void load(FILE*) override;
				virtual void save(FILE*) const override;

			public:
				//data
				bool load_adjust(void) const;
				bool load_adjust(bool);
				bool frequencies(void) const;
				bool frequencies(bool);
				bool branch_switch(void) const;
				bool branch_switch(bool);
				
				double load_min(void) const;
				double load_min(double);
				double load_max(void) const;
				double load_max(double);
				double load_factor(void) const;
				double load_predictor(void) const;
				double load_predictor(double);
				double mode_injection(void) const;
				double mode_injection(double);
				double load_increment_min(void) const;
				double load_increment_min(double);
				double load_increment_max(void) const;
				double load_increment_max(double);
				double state_increment_min(void) const;
				double state_increment_min(double);
				double state_increment_max(void) const;
				double state_increment_max(double);
				
				unsigned bifurcation_count(void) const;
				unsigned bifurcation_track(void) const;
				unsigned bifurcation_track(unsigned);
				
				solvers::unload& unload(void);
				
				strategies::type strategy(void) const;
				strategies::type strategy(strategies::type);

				//type
				virtual solvers::type type(void) const override;

				//sets
				virtual unsigned state_set(void) const override;
				virtual unsigned force_set(void) const override;
				virtual unsigned tangent_set(void) const override;

			protected:
				//analysis
				virtual void run(void) override;
				
				virtual void run_unload(void);
				virtual void run_frequencies(void);
				virtual void run_branch_switch(void);
				
				virtual bool stop(bool) const override;
				
				virtual void record(void) override;
				virtual void finish(void) const override;
				
				virtual void compute_residue(void);
				virtual bool compute_predictor(void);
				virtual bool compute_corrector(void);
				virtual void compute_reactions(void);

				//attributes
				bool m_frequencies;
				bool m_load_adjust;
				bool m_branch_switch;

				double m_load_min;
				double m_load_max;
				double m_load_factor;
				double m_mode_injection;
				double m_load_increment_min;
				double m_load_increment_max;
				double m_state_increment_min;
				double m_state_increment_max;

				unsigned m_bifurcation_count;
				unsigned m_bifurcation_track;

				solvers::unload m_unload;

				strategies::Strategy* m_strategy;
			};
		}
	}
}
