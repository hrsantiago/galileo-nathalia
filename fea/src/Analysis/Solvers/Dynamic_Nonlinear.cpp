//std
#include <cmath>

//mat
#include "linear/dense.h"

//fea
#include "Models/Model.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Assembler/Assembler.h"
#include "Analysis/Solvers/Dynamic_Nonlinear.h"

namespace fea
{
	namespace analysis
	{
		namespace solvers
		{
			//static data
			static int n;
			static double b, g;
			static double *un, *vn;
			static Assembler* assembler;
			
			//constructors
			Dynamic_Nonlinear::Dynamic_Nonlinear(void) : m_frequencies(false), m_integration(solvers::integration::newmark)
			{
				return;
			}

			//destructor
			Dynamic_Nonlinear::~Dynamic_Nonlinear(void)
			{
				return;
			}

			//serialization
			void Dynamic_Nonlinear::load(FILE* file)
			{
				//base calls
				Solver::load(file);
				Time::load(file);
				Nonlinear::load(file);
				//save
				fscanf(file, "%d", &m_integration);
			}
			void Dynamic_Nonlinear::save(FILE* file) const
			{
				//base calls
				Solver::save(file);
				Time::save(file);
				Nonlinear::save(file);
				//save
				fprintf(file, "%01d ", m_integration);
			}

			//data
			bool Dynamic_Nonlinear::frequencies(void) const
			{
				return m_frequencies;
			}
			bool Dynamic_Nonlinear::frequencies(bool frequencies)
			{
				m_frequencies = frequencies;
			}
			solvers::integration Dynamic_Nonlinear::integration(void) const
			{
				return m_integration;
			}
			solvers::integration Dynamic_Nonlinear::integration(solvers::integration integration)
			{
				m_integration = integration;
			}
			
			//type
			solvers::type Dynamic_Nonlinear::type(void) const
			{
				return solvers::type::dynamic_nonlinear;
			}

			//sets
			unsigned Dynamic_Nonlinear::state_set(void) const
			{
				return 
					(unsigned) solvers::state::u | 
					(unsigned) solvers::state::v | 
					(unsigned) solvers::state::a;
			}
			unsigned Dynamic_Nonlinear::force_set(void) const
			{
				return 
					(unsigned) solvers::force::r | 
					(unsigned) solvers::force::R | 
					(unsigned) solvers::force::Fn | 
					(unsigned) solvers::force::Fi | 
					(unsigned) solvers::force::Fe;
			}
			unsigned Dynamic_Nonlinear::tangent_set(void) const
			{
				return 
					(unsigned) solvers::tangent::M | 
					(unsigned) solvers::tangent::C | 
					(unsigned) solvers::tangent::K;
			}

			//analysis
			void Dynamic_Nonlinear::prepare(void)
			{
				Time::prepare();
			}
			
			void Dynamic_Nonlinear::run(void)
			{
				// print head
				printf("-----------------Running a dynamic nonlinear analysis:----------------\n");
				//select solver
				switch(m_integration)
				{
					case solvers::integration::newmark:
						run_newmark();
						break;
					case solvers::integration::runge_kutta:
						run_runge_kutta();
						break;
					default:
						break;
				}
			}
			void Dynamic_Nonlinear::run_newmark(void)
			{
				//initial state
				compute_initial();
				//incremental loop
				while (m_step < m_step_max)
				{
					//update time
					m_t += m_dt;
					//check time limit
					if(m_t > m_time_max)
					{
						printf("\tMax time reached!\n");
						break;
					}
					//supports
					assembler->apply_supports();
					//external load
					assembler->assembly_external_force();
					//predictor
					compute_predictor();
					//apply state
					assembler->apply_state();
					assembler->apply_velocity();
					assembler->apply_dependencies();
					assembler->apply_configurations();
					//system matrix
					if(m_mode == solvers::mode::modified)
					{
						compute_system_matrix();
					}
					//corector
					while (m_iteration < m_iteration_max)
					{
						//check equilibrium
						compute_residue();
						if(equilibrium())
						{
							//reactions
							compute_reactions();
							assembler->apply_reactions();
							//frenquencies
							frequencies();
							//record and update
							assembler->record();
							assembler->update();
							//print
							printf("step: %06d attempt: %02d iterations: %02d time: %+.2e dof: %+.2e\n", m_step, m_attempt, m_iteration, m_t, dof());
							//break
							break;
						}
						//update solution
						if(m_mode == solvers::mode::full)
						{
							compute_system_matrix();
						}
						compute_corrector();
						assembler->apply_state();
						assembler->apply_velocity();
						assembler->apply_acceleration();
						assembler->apply_dependencies();
						assembler->apply_configurations();
						//increment iteration
						m_iteration++;
					}
					//check solution
					if(!m_equilibrium)
					{
						if(m_attempt < m_attempt_max)
						{
							m_dt /= 2;
							m_attempt++;
							m_iteration = 0;
							assembler->restore();
							printf("\tStep %i failed in attempt %i!\n", m_step, m_attempt);
							continue;
						}
						else
						{
							printf("\tEquilibrium not found in max attempts!\n");
							break;
						}
					}
					//set time step
					if(m_time_adjust)
					{
						m_dt *= sqrt((double) (m_iteration_desired + 1) / (m_iteration + 1));
					}
					//increment step
					m_step++;
					m_attempt = 0;
					m_iteration = 0;
				}
			}
			void Dynamic_Nonlinear::run_runge_kutta(void)
			{
				//initial state
				compute_initial();
				//time loop
				while (m_step < m_step_max)
				{
					//update solution
					compute_tangent_1();
					compute_tangent_2();
					compute_tangent_3();
					compute_tangent_4();
					compute_corrector();
					//assembly state
					assembler->apply_state();
					assembler->apply_velocity();
					assembler->apply_dependencies();
					assembler->apply_configurations();
					//assembly forces
					assembler->assembly_inertia();
					assembler->assembly_internal_force();
					assembler->assembly_external_force();
					//compute acceleration
					compute_acceleration();
					assembler->apply_acceleration();
					assembler->assembly_inertial_force();
					//reaction
					compute_reactions();
					assembler->apply_reactions();
					//frequencies
					run_frequencies();
					//update and record
					assembler->record();
					assembler->update();
					//print
					printf("step: %06d time: %.2e dof: %+.2e\n", m_step, m_t, dof());
					//increment step
					m_step++;
				}
				//free data
				delete[] un;
				delete[] vn;
			}
			void Dynamic_Nonlinear::run_frequencies(void)
			{
				if(m_frequencies)
				{
					const Assembler* assembler = m_analysis->assembler();
					if(m_integration == solvers::integration::runge_kutta)
					{
						assembler->assembly_stiffness();
					}
//					const arma::mat M = arma::mat(m_M);
//					const arma::mat K = arma::mat(m_K);
//					const arma::mat S = inv(M) * K;
//					arma::eig_sym(m_k, S);
				}
			}
			
			void Dynamic_Nonlinear::record(void)
			{
				//record time
				char formatter[200];
				sprintf(formatter, "%+.6e\n", m_t);
				m_results[0] += formatter;
				//record frenquencies
				if(m_frequencies)
				{
					const unsigned nz = m_analysis->assembler()->dof_nonzero();
					for(unsigned i = 0; i < nz; i++)
					{
						sprintf(formatter, "%+.6e ", m_k[i]);
						m_results[1] += formatter;
					}
					m_results[1] += "\n";
				}
			}
			void Dynamic_Nonlinear::finish(void) const
			{
				//save load
				const std::string path = m_analysis->model()->folder() + "/Solver/Time.txt";
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
			
			void Dynamic_Nonlinear::compute_initial(void)
			{
				//assembler
				assembler = m_analysis->assembler();
				//apply state
				assembler->apply_initials();
				assembler->apply_supports();
				assembler->apply_dependencies();
				assembler->apply_configurations();
				//assembly forces
				assembler->assembly_inertia();
				assembler->assembly_internal_force();
				assembler->assembly_external_force();
				//compute acceleration
				compute_acceleration();
				assembler->apply_acceleration();
				assembler->assembly_inertial_force();
				//compute reaction
				compute_reactions();
				assembler->apply_reactions();
				//record
				assembler->record();
				//runge kutta
				n = assembler->dof_unknow();
				if(m_integration == solvers::integration::runge_kutta)
				{
					//allocate
					un = new double[n];
					vn = new double[n];
					//set old state
					mat::set(un, m_u, n);
					mat::set(vn, m_v, n);
				}
			}
			void Dynamic_Nonlinear::compute_residue(void)
			{
				n = assembler->dof_unknow();
				assembler->assembly_inertial_force();
				assembler->assembly_internal_force();
				for(unsigned i = 0; i < n; i++)
				{
					m_r[i] = m_Feu[i] - m_Fnu[i] - m_Fiu[i];					
				}
			}
			void Dynamic_Nonlinear::compute_tangent_1(void)
			{
				n = assembler->dof_unknow();
				for(unsigned i = 0; i < n; i++)
				{
					m_du[i] = m_dt / 6 * m_v[i];
					m_dv[i] = m_dt / 6 * m_a[i];
				}
			}
			void Dynamic_Nonlinear::compute_tangent_2(void)
			{
				//update state
				m_t += m_dt / 2;
				for(unsigned i = 0; i < n; i++)
				{
					m_u[i] += m_dt / 2 * m_v[i];
					m_v[i] += m_dt / 2 * m_a[i];
				}
				//apply state
				assembler->apply_state();
				assembler->apply_velocity();
				assembler->apply_supports();
				assembler->apply_dependencies();
				assembler->apply_configurations();
				//assembly forces
				assembler->assembly_inertia();
				assembler->assembly_internal_force();
				assembler->assembly_external_force();
				linsolve(m_a, m_M, mat::sub(m_Feu, m_Fiu, n));
				//update state
				for(unsigned i = 0; i < n; i++)
				{
					m_du[i] += m_dt / 3 * m_v[i];
					m_dv[i] += m_dt / 3 * m_a[i];
				}
			}
			void Dynamic_Nonlinear::compute_tangent_3(void)
			{
				//update state
				for(unsigned i = 0; i < n; i++)
				{
					m_u[i] = un[i] + m_dt / 2 * m_v[i];
					m_v[i] = vn[i] + m_dt / 2 * m_a[i];
				}
				//apply state
				assembler->apply_state();
				assembler->apply_velocity();
				assembler->apply_dependencies();
				assembler->apply_configurations();
				//assembly forces
				assembler->assembly_inertia();
				assembler->assembly_internal_force();
				assembler->assembly_external_force();
				linsolve(m_a, m_M, mat::sub(m_Feu, m_Fiu, n));
				//update state
				for(unsigned i = 0; i < n; i++)
				{
					m_du[i] += m_dt / 3 * m_v[i];
					m_dv[i] += m_dt / 3 * m_a[i];
				}
			}
			void Dynamic_Nonlinear::compute_tangent_4(void)
			{
				//update state
				m_t += m_dt / 2;
				for(unsigned i = 0; i < n; i++)
				{
					m_u[i] = un[i] + m_dt * m_v[i];
					m_v[i] = vn[i] + m_dt * m_a[i];
				}
				//apply state
				assembler->apply_state();
				assembler->apply_velocity();
				assembler->apply_supports();
				assembler->apply_dependencies();
				assembler->apply_configurations();
				//assembly forces
				assembler->assembly_inertia();
				assembler->assembly_internal_force();
				assembler->assembly_external_force();
				linsolve(m_a, m_M, mat::sub(m_Feu, m_Fiu, n));
				//update state
				for(unsigned i = 0; i < n; i++)
				{
					m_du[i] += m_dt / 6 * m_v[i];
					m_dv[i] += m_dt / 6 * m_a[i];
				}
			}
			void Dynamic_Nonlinear::compute_predictor(void)
			{
				n = assembler->dof_unknow();
				for(unsigned i = 0; i < n; i++)
				{
					m_v[i] += m_dt * m_a[i];
					m_u[i] += m_dt * m_v[i] - m_dt * m_dt * m_a[i] / 2;
				}
			}
			void Dynamic_Nonlinear::compute_corrector(void)
			{
				n = assembler->dof_unknow();
				if(m_integration == solvers::integration::newmark)
				{
					linsolve(m_du, m_K, m_r);
					for(unsigned i = 0; i < n; i++)
					{
						m_u[i] += m_du[i];
						m_v[i] += g / (b * m_dt) * m_du[i];
						m_a[i] += 1 / (b * m_dt * m_dt) * m_du[i];
					}
				}
				else
				{
					for(unsigned i = 0; i < n; i++)
					{
						m_u[i] = un[i] += m_du[i];
						m_v[i] = vn[i] += m_dv[i];
					}
				}
			}
			void Dynamic_Nonlinear::compute_reactions(void)
			{
				n = assembler->dof_know();
				for(unsigned i = 0; i < n; i++)
				{
					m_R[i] = m_Fnk[i] + m_Fik[i] - m_Fek[i];
				}
			}
			void Dynamic_Nonlinear::compute_acceleration(void)
			{
				n = assembler->dof_unknow();
				for(unsigned i = 0; i < n; i++)
				{
					m_r[i] = m_Feu[i] - m_Fiu[i];
				}
				linsolve(m_a, m_M, m_r);
			}
			void Dynamic_Nonlinear::compute_system_matrix(void)
			{
				b = m_newmark.beta;
				g = m_newmark.gamma;
				n = assembler->dof_nonzero();
				assembler->assembly_inertia();
				assembler->assembly_damping();
				assembler->assembly_stiffness();
				for(unsigned i = 0; i < n; i++)
				{
					m_K[i] += (m_M[i] + g * m_dt * m_C[i]) / (b * m_dt * m_dt);
				}
			}
		}
	}
}
