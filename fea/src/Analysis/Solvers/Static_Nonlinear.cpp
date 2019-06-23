//std
#include <cmath>
#include <cfloat>

//mat
#include "misc/util.h"
#include "linear/dense.h"

//fea
#include "Model/Model.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Solvers/Watch_Dof.h"
#include "Analysis/Assembler/Assembler.h"
#include "Analysis/Strategies/Strategies.h"
#include "Analysis/Solvers/Static_Nonlinear.h"

namespace fea
{
	namespace analysis
	{
		namespace solvers
		{
			//static data
			static unsigned n;
			static Assembler* assembler;
			
			//constructors
			Static_Nonlinear::Static_Nonlinear(void) :
				m_frequencies(false), m_load_adjust(false), m_branch_switch(false), 
				m_load_min(-DBL_MAX), m_load_max(+DBL_MAX), m_load_factor(1), m_mode_injection(1e2), 
				m_load_increment_min(0), m_load_increment_max(DBL_MAX),
				m_state_increment_min(0), m_state_increment_max(1e100),
				m_bifurcation_count(0U), m_bifurcation_track(1U),
				m_strategy(new strategies::Arc_Length_Cylindric)
			{
				return;
			}

			//destructor
			Static_Nonlinear::~Static_Nonlinear(void) 
			{
				delete m_strategy;
			}

			//serialization
			void Static_Nonlinear::load(FILE* file)
			{
				//base call
				Solver::load(file);
				Nonlinear::load(file);
				//load
				strategies::type type;
				fscanf(file, "%d %d %d %lf %lf %lf %lf %lf %d",
					&m_frequencies,
					&m_load_adjust,
					&m_branch_switch,
					&m_load_min,
					&m_load_max,
					&m_load_increment_min,
					&m_load_increment_max,
					&m_mode_injection,
					&type);
				strategies::Strategy::create(m_strategy, type);
			}
			void Static_Nonlinear::save(FILE* file) const
			{
				//base call
				Solver::save(file);
				Nonlinear::save(file);
				//save
				fprintf(file, "%01d %01d %01d %+.6e %+.6e %+.6e %+.6e %+.6e %02d\n", 
					m_frequencies, 
					m_load_adjust, 
					m_branch_switch, 
					m_load_min, 
					m_load_max, 
					m_load_increment_min, 
					m_load_increment_max,
					m_mode_injection, 
					m_strategy->type());
			}

			//data
			bool Static_Nonlinear::load_adjust(void) const 
			{
				return m_load_adjust;
			}
			bool Static_Nonlinear::load_adjust(bool load_adjust) 
			{
				return m_load_adjust = load_adjust;
			}
			bool Static_Nonlinear::frequencies(void) const 
			{
				return m_frequencies;
			}
			bool Static_Nonlinear::frequencies(bool frequencies) 
			{
				return m_frequencies = frequencies;
			}
			bool Static_Nonlinear::branch_switch(void) const 
			{
				return m_branch_switch;
			}
			bool Static_Nonlinear::branch_switch(bool branch_switch) 
			{
				return m_branch_switch = branch_switch;
			}
			
			double Static_Nonlinear::load_min(void) const 
			{
				return m_load_min;
			}
			double Static_Nonlinear::load_min(double load_min) 
			{
				return m_load_min = load_min;
			}
			double Static_Nonlinear::load_max(void) const 
			{
				return m_load_max;
			}
			double Static_Nonlinear::load_max(double load_max) 
			{
				return m_load_max = load_max;
			}
			double Static_Nonlinear::load_factor(void) const 
			{
				return m_load_factor;
			}
			double Static_Nonlinear::load_predictor(void) const
			{
				return m_dl;
			}
			double Static_Nonlinear::load_predictor(double load_predictor) 
			{
				return m_dl = m_dl0 = load_predictor;
			}
			double Static_Nonlinear::mode_injection(void) const 
			{
				return m_mode_injection;
			}
			double Static_Nonlinear::mode_injection(double mode_injection) 
			{
				return m_mode_injection = mode_injection;
			}
			double Static_Nonlinear::load_increment_min(void) const 
			{
				return m_load_increment_min;
			}
			double Static_Nonlinear::load_increment_min(double load_increment_min) 
			{
				return m_load_increment_min = load_increment_min;
			}
			double Static_Nonlinear::load_increment_max(void) const 
			{
				return m_load_increment_max;
			}
			double Static_Nonlinear::load_increment_max(double load_increment_max) 
			{
				return m_load_increment_max = load_increment_max;
			}
			double Static_Nonlinear::state_increment_min(void) const 
			{
				return m_state_increment_min;
			}
			double Static_Nonlinear::state_increment_min(double state_increment_min) 
			{
				return m_state_increment_min = state_increment_min;
			}
			double Static_Nonlinear::state_increment_max(void) const 
			{
				return m_state_increment_max;
			}
			double Static_Nonlinear::state_increment_max(double state_increment_max) 
			{
				return m_state_increment_max = state_increment_max;
			}
			
			unsigned Static_Nonlinear::bifurcation_track(void) const 
			{
				return m_bifurcation_track;
			}
			unsigned Static_Nonlinear::bifurcation_track(unsigned bifurcation_track) 
			{
				return m_bifurcation_track = bifurcation_track;
			}
			
			solvers::unload& Static_Nonlinear::unload(void) 
			{
				return m_unload;
			}
			
			strategies::type Static_Nonlinear::strategy(void) const 
			{
				return m_strategy->type();
			}
			strategies::type Static_Nonlinear::strategy(strategies::type strategy_type) 
			{
				if(m_strategy->type() != strategy_type)
				{
					strategies::Strategy::create(m_strategy, strategy_type);
				}
				return strategy_type;
			}

			//type
			solvers::type Static_Nonlinear::type(void) const 
			{
				return solvers::type::static_nonlinear;
			}

			//sets
			unsigned Static_Nonlinear::state_set(void) const 
			{
				return (unsigned) solvers::state::u;
			}
			unsigned Static_Nonlinear::force_set(void) const 
			{
				return
					(unsigned) solvers::force::r |
					(unsigned) solvers::force::R |
					(unsigned) solvers::force::Fi |
					(unsigned) solvers::force::Fr |
					(unsigned) solvers::force::Fd |
					(m_branch_switch || m_frequencies ? (unsigned) solvers::force::k : 0);
			}
			unsigned Static_Nonlinear::tangent_set(void) const 
			{
				return
					(unsigned) solvers::tangent::K |
					(m_frequencies ?   (unsigned) solvers::tangent::M : 0) |
					(m_branch_switch ? (unsigned) solvers::tangent::f : 0);
			}

			//analysis
			void Static_Nonlinear::run(void) 
			{
				//load
				m_l = 0;
				m_step = 0;
				//assembler
				assembler = m_analysis->assembler();
				//assembly dead and reference loads
				assembler->assembly_dead_force();
				assembler->assembly_reference_force();
				//initials
				assembler->apply_initials();
				assembler->apply_supports();
				assembler->apply_dependencies();
				assembler->apply_configurations();
				//frequencies
				run_frequencies();
				//record
				assembler->record();
				//incremental loop
				while(!stop(true))
				{
					//predictor
					if(!compute_predictor())
					{
						break;
					}
					//iterative loop
					while(m_iteration < m_iteration_max)
					{
						//set state
						assembler->increment_state();
						assembler->apply_dependencies();
						assembler->apply_configurations();
						assembler->assembly_internal_force();
						//check solution
						compute_residue();
						if(equilibrium())
						{
							//load
							m_l += m_dl;
							//time
							m_t += m_dt;
							//reaction
							compute_reactions();
							assembler->apply_reactions();
							//frenquencies
							run_frequencies();
							//record and update
							if(!stop(false))
							{
								assembler->record();
								assembler->update();
							}
							//print
							printf("step: %06d attempts: %02d iterations: %02d load: %+4.2e dof: %+4.2e\n", m_step, m_attempt, m_iteration, m_l, dof());
							//break iteration loop
							break;
						}
						//update solution
						if(!compute_corrector())
						{
							break;
						}
						//increment iteration
						m_iteration++;
					}
					//free memory
					if(m_mode == solvers::mode::modified)
					{
						lindel();
					}
					//check solution
					if(!m_equilibrium)
					{
						printf("\tStep %03d failed in attempt %02d!\n", m_step, m_attempt);
						m_dt /= 2;
						m_attempt++;
						m_iteration = 0;
						assembler->restore();
						continue;
					}
					//load adjust factor
					m_load_factor = sqrt((double) (m_iteration_desired + 1) / (m_iteration + 1));
					//increment step
					m_step++;
					m_attempt = 0;
					m_iteration = 0;
				}
			}
			void Static_Nonlinear::run_unload(void) 
			{
				if(!m_unload.test && (fabs(dof()) > m_unload.state || fabs(m_l) > m_unload.load || m_step > m_unload.step))
				{
					m_dl = m_dl0 *= -1;
					m_unload.test = true;
					mat::mul(m_du, -1, n);
					printf("\tUnload condition reached: Reversing load!\n");
				}
			}
			void Static_Nonlinear::run_frequencies(void) 
			{
				if(m_frequencies)
				{
					const Assembler* assembler = m_analysis->assembler();
					assembler->assembly_inertia();
					assembler->assembly_stiffness();
//					const arma::mat M = arma::mat(m_M);
//					const arma::mat K = arma::mat(m_K);
//					const arma::mat S = inv(M) * K;
//					arma::eig_sym(m_k, S);
				}
			}
			void Static_Nonlinear::run_branch_switch(void) 
			{
				if(m_branch_switch && m_bifurcation_count < m_bifurcation_track)
				{
//					arma::eig_sym(m_k, m_f, arma::mat(m_K));
//					if(m_k[0] < 0 && abs(arma::dot(m_Fru, m_f.col(0))) < m_tolerance * arma::norm(m_Fru))
//					{
//						m_bifurcation_count++;
//						m_du += m_mode_injection * arma::norm(m_du) * m_f.col(0);
//						printf("\tBifurcation point founded: Following secondary path!\n");
//					}
				}
			}
			
			bool Static_Nonlinear::stop(bool log) const 
			{
				//base call
				if(Solver::stop(log) || Nonlinear::stop(log))
				{
					return true;
				}
				//dof
				const double d = dof();
				//test
				const bool t[] = {d < m_dof_min, d > m_dof_max, m_l < m_load_min, m_l > m_load_max};
				//message
				const char* m[] = {"\tMin dof reached!\n", "\tMax dof reached!\n", "\tMin load reached!\n", "\tMax load reached!\n"};
				//print
				for(unsigned i = 0; i < 4; i++)
				{
					if(t[i])
					{
						if(log)
						{
							printf(m[i]);
						}
						return true;
					}
				}
				//return
				return false;
			}
			
			void Static_Nonlinear::record(void) 
			{
				//record dof & load
				char formatter[200];
				sprintf(formatter, "%+.6e\n", m_l);
				m_results[0] += formatter;
				//record frequencies
				const unsigned nz = m_analysis->assembler()->dof_nonzero();
				if(m_frequencies)
				{
					for(unsigned i = 0; i < nz; i++)
					{
						sprintf(formatter, "%+.6e ", m_k[i]);
						m_results[1] += formatter;
					}
					m_results[1] += "\n";
				}
			}
			void Static_Nonlinear::finish(void) const
			{
				//save load
				const std::string path = m_analysis->model()->folder() + "/Solver/Load.txt";
				FILE* file = fopen(path.c_str(), "w");
				fprintf(file, m_results[0].c_str());
				fclose(file);
				//save frenquencies
				if(m_frequencies)
				{
					const std::string path = m_analysis->model()->folder() + "/Solver/Frequencies.txt";
					FILE* file = fopen(path.c_str(), "w");
					fprintf(file, m_results[1].c_str());
					fclose(file);
				}
			}
			
			void Static_Nonlinear::compute_residue(void)
			{
				n = assembler->dof_unknow();
				for(unsigned i = 0; i < n; i++)
				{
					m_r[i] = (m_l + m_dl) * m_Fru[i] + m_Fdu[i] - m_Fiu[i];
				}
			}
			bool Static_Nonlinear::compute_predictor(void)
			{
				//apply state
				assembler->apply_supports();
				assembler->apply_configurations();
				//assembly stiffness
				assembler->assembly_stiffness();
				//state increment tangent
				if(!lindec(m_K) || !linsub(m_dut, m_K, m_Fru))
				{
					lindel();
					printf("\tUnable to compute tangent displacement increment in step %04d\n", m_step);
					return false;
				}
				//unload
				run_unload();
				//load increment
				if(m_step != 0)
				{
					m_dl = m_dl0 = m_strategy->predictor() / (1 << m_attempt);
				}
				if(std::isnan(m_dl))
				{
					printf("\tUnable to compute load increment in step %04d\n", m_step);
					return false;
				}
				if(m_load_adjust && m_step != 0)
				{
					m_dl = m_dl0 *= m_load_factor;
				}
				//adjust increment
				const unsigned d = m_watch_dof->m_dof_index;
				m_dl = m_dl0 = mat::sign(m_dl) * std::max(fabs(m_dl), m_load_increment_min);
				m_dl = m_dl0 = mat::sign(m_dl) * std::min(fabs(m_dl), m_load_increment_max);
				m_dl = m_dl0 = mat::sign(m_dl) * std::max(fabs(m_dl), m_step ? m_state_increment_min / fabs(m_dut[d]) : fabs(m_dl));
				m_dl = m_dl0 = mat::sign(m_dl) * std::min(fabs(m_dl), m_step ? m_state_increment_max / fabs(m_dut[d]) : fabs(m_dl));
				//state increment
				n = assembler->dof_unknow();
				mat::set(m_du, m_dut, n, m_dl);
				if(m_mode == solvers::mode::full)
				{
					lindel();
				}
				else
				{
					mat::set(m_ddut, m_dut, n);
				}
				//branch switch
				run_branch_switch();
				//return
				return true;
			}
			bool Static_Nonlinear::compute_corrector(void)
			{
				//assembly
				if(m_mode == solvers::mode::full)
				{
					m_analysis->assembler()->assembly_stiffness();
					if(!lindec(m_K))
					{
						lindel();
						return false;
					}
				}
				//residue corrector
				if(!linsub(m_ddur, m_K, m_r))
				{
					return false;
				}
				//tangent corrector
				if(m_mode == solvers::mode::full)
				{
					if(!linsub(m_ddut, m_K, m_Fru))
					{
						return false;
					}
				}
				//free memory
				if(m_mode == solvers::mode::full)
				{
					lindel();
				}
				//load corrector
				m_ddl = m_strategy->corrector();
				if(std::isnan(m_ddl))
				{
					printf("\tUnable to compute load corrector increment in step %d\n", m_step);
					return false;
				}
				//increment
				m_dl += m_ddl;
				n = assembler->dof_unknow();
				for(unsigned i = 0; i < n; i++)
				{
					m_du[i] += m_ddu[i] = m_ddur[i] + m_ddl * m_ddut[i];
				}
				//return
				return true;
			}
			void Static_Nonlinear::compute_reactions(void)
			{
				n = assembler->dof_know();
				for(unsigned i = 0; i < n; i++)
				{
					m_R[i] = m_Fik[i] - m_l * m_Frk[i] - m_Fdk[i];
				}
			}
		}
	}
}
