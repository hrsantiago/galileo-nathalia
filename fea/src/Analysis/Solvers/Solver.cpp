//std
#include <cfloat>

//suitesparse
#include <umfpack.h>

//fea
#include "Model/Model.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Node.h"

#include "Boundary/Boundary.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Solvers/Solvers.h"
#include "Analysis/Solvers/Watch_Dof.h"
#include "Analysis/Assembler/Assembler.h"

namespace fea
{
	namespace analysis
	{
		namespace solvers
		{
			//constructors
			Solver::Solver(void) :
				m_dead(false), m_step(0), m_step_max(100), m_load_case(0), m_dead_case(0), 
				m_dof_min(-DBL_MAX), m_dof_max(DBL_MAX),
				m_time_min(0), m_time_max(1), m_time_increment_max(DBL_MAX),
				m_t(0), m_dt(1e-2),
				m_l(0), m_dl(1e-2), m_dl0(0), m_ddl(0),
				m_f(nullptr), m_K(nullptr), m_C(nullptr), m_M(nullptr),
				m_u(nullptr), m_v(nullptr), m_a(nullptr), m_du(nullptr), 
				m_dv(nullptr), m_da(nullptr), m_dut(nullptr), m_ddu(nullptr), 
				m_ddur(nullptr), m_ddut(nullptr),m_r(nullptr), m_R(nullptr), m_k(nullptr), 
				m_Fiu(nullptr), m_Fik(nullptr), m_Fnu(nullptr), m_Fnk(nullptr), m_Feu(nullptr), 
				m_Fek(nullptr), m_Fru(nullptr), m_Frk(nullptr), m_Fdu(nullptr), m_Fdk(nullptr),
				m_watch_dof(new Watch_Dof)
			{
				return;
			}

			//destructor
			Solver::~Solver(void)
			{
				delete[] m_f; 
				delete[] m_K; 
				delete[] m_C; 
				delete[] m_M;
				delete[] m_u; 
				delete[] m_v; 
				delete[] m_a; 
				delete[] m_r; 
				delete[] m_R; 
				delete[] m_k; 
				delete[] m_du; 
				delete[] m_dv; 
				delete[] m_da; 
				delete[] m_dut; 
				delete[] m_ddu; 
				delete[] m_Fiu; 
				delete[] m_Fik; 
				delete[] m_Fnu; 
				delete[] m_Fnk; 
				delete[] m_Feu; 
				delete[] m_Fek; 
				delete[] m_Fru; 
				delete[] m_Frk; 
				delete[] m_Fdu; 
				delete[] m_Fdk;
				delete[] m_ddur; 
				delete[] m_ddut;
				delete m_watch_dof;
			}

			//create
			void Solver::create(Solver*& solver, solvers::type type)
			{
				//delete solver
				delete solver;
				//create solver
				switch(type)
				{
					case solvers::type::state:
						solver = new State;
						break;
					case solvers::type::modal:
						solver = new Modal;
						break;
					case solvers::type::minimum:
						solver = new Minimum;
						break;
					case solvers::type::buckling:
						solver = new Buckling;
						break;
					case solvers::type::static_linear:
						solver = new Static_Linear;
						break;
					case solvers::type::static_nonlinear:
						solver = new Static_Nonlinear;
						break;
					case solvers::type::transient_linear:
						solver = new Transient_Linear;
						break;
					case solvers::type::transient_nonlinear:
						solver = new Transient_Nonlinear;
						break;
					case solvers::type::dynamic_linear:
						solver = new Dynamic_Linear;
						break;
					case solvers::type::dynamic_nonlinear:
						solver = new Dynamic_Nonlinear;
						break;
					default:
						break;
				}
			}

			//serialization
			void Solver::load(FILE* file)
			{
				m_watch_dof->load(file);
				fscanf(file, "%d %d %d %d %lf %lf %lf %lf %lf", 
					&m_dead, 
					&m_step_max, 
					&m_load_case,
					&m_dead_case,
					&m_dof_min, 
					&m_dof_max, 
					&m_time_min,
					&m_time_max, 
					&m_time_increment_max
				);
			}
			void Solver::save(FILE* file) const
			{
				m_watch_dof->save(file);
				fprintf(file, "%01d %04d %02d %02d %+.6e %+.6e %+.6e %+.6e %+.6e ", 
					m_dead, 
					m_step_max, 
					m_load_case, 
					m_dead_case, 
					m_dof_min, 
					m_dof_max, 
					m_time_max,
					m_time_min, 
					m_time_increment_max
				);
			}

			//data
			bool Solver::dead(void) const
			{
				return m_dead;
			}
			bool Solver::dead(bool dead)
			{
				return m_dead = dead;
			}
			
			double Solver::dof_min(void) const
			{
				return m_dof_min;
			}
			double Solver::dof_min(double dof_min)
			{
				return m_dof_min = dof_min;
			}
			double Solver::dof_max(void) const
			{
				return m_dof_max;
			}
			double Solver::dof_max(double dof_max)
			{
				return m_dof_max = dof_max;
			}
			
			unsigned Solver::step_max(void) const
			{
				return m_step_max;
			}
			unsigned Solver::step_max(unsigned step_max)
			{
				return m_step_max = step_max;
			}
			unsigned Solver::load_case(void) const
			{
				return m_load_case;
			}
			unsigned Solver::load_case(unsigned load_case)
			{
				return m_load_case = load_case;
			}
			unsigned Solver::dead_case(void) const
			{
				return m_dead_case;
			}
			unsigned Solver::dead_case(unsigned dead_case)
			{
				return m_dead_case = dead_case;
			}
			
			const Watch_Dof* Solver::watch_dof(void)
			{
				return m_watch_dof;
			}
			const Watch_Dof* Solver::watch_dof(unsigned node, mesh::nodes::dof dof)
			{
				m_watch_dof->m_dof = dof;
				m_watch_dof->m_node = node;
				return m_watch_dof;
			}

			//name
			const char* Solver::name(void) const
			{
				switch(type())
				{
					case solvers::type::state:
						return "State";
					case solvers::type::modal:
						return "Modal";
					case solvers::type::minimum:
						return "Minimum";
					case solvers::type::buckling:
						return "Buckling";
					case solvers::type::static_linear:
						return "Static Linear";
					case solvers::type::static_nonlinear:
						return "Static Nonlinear";
					case solvers::type::transient_linear:
						return "Transient Linear";
					case solvers::type::transient_nonlinear:
						return "Transient Nonlinear";
					case solvers::type::dynamic_linear:
						return "Dynamic Linear";
					case solvers::type::dynamic_nonlinear:
						return "Dynamic Nonlinear";
					default:
						return "";
				}
			}
			const char* Solver::parameter(void) const
			{
				switch(type())
				{
					case solvers::type::state:
						return "Time";
					case solvers::type::modal:
						return "Frequency";
					case solvers::type::minimum:
						return "Step";
					case solvers::type::buckling:
						return "Stiffness";
					case solvers::type::static_linear:
						return "Load";
					case solvers::type::static_nonlinear:
						return "Load";
					case solvers::type::transient_linear:
						return "Time";
					case solvers::type::transient_nonlinear:
						return "Time";
					case solvers::type::dynamic_linear:
						return "Time";
					case solvers::type::dynamic_nonlinear:
						return "Time";
					default:
						return "";
				}
			}

			//step
			unsigned Solver::step(void) const
			{
				return m_step;
			}

			//dof
			double Solver::dof(void) const
			{
				return m_analysis->model()->mesh()->node(m_watch_dof->m_node)->state(m_watch_dof->m_dof);
			}

			//data
			double Solver::time(void) const
			{
				return m_t;
			}
			double Solver::time_min(void) const
			{
				return m_time_min;
			}
			double Solver::time_min(double time_min)
			{
				return m_time_min = time_min;
			}
			double Solver::time_max(void) const
			{
				return m_time_max;
			}
			double Solver::time_max(double time_max)
			{
				return m_time_max = time_max;
			}
			double Solver::time_increment(void) const
			{
				return m_dt;
			}
			double Solver::time_increment_max(void) const
			{
				return m_time_increment_max;
			}
			double Solver::time_increment_max(double time_increment_max)
			{
				return m_time_increment_max = time_increment_max;
			}
			
			double Solver::load(void) const
			{
				return m_l;
			}
			double Solver::load_increment(void) const
			{
				return m_dl;
			}
			double Solver::load_predictor(void) const
			{
				return m_dl0;
			}
			double Solver::load_corrector(void) const
			{
				return m_ddl;
			}
			
			const double* Solver::state(void) const
			{
				return m_u;
			}
			const double* Solver::state_increment(void) const
			{
				return m_du;
			}
			const double* Solver::state_predictor(void) const
			{
				return m_dut;
			}
			const double* Solver::state_corrector_residue(void) const
			{
				return m_ddur;
			}
			const double* Solver::state_corrector_tangent(void) const
			{
				return m_ddut;
			}

			const double* Solver::velocity(void) const
			{
				return m_v;
			}
			const double* Solver::velocity_increment(void) const
			{
				return m_dv;
			}

			const double* Solver::acceleration(void) const
			{
				return m_a;
			}
			const double* Solver::acceleration_increment(void) const
			{
				return m_da;
			}
			
			const double* Solver::residue(void) const
			{
				return m_r;
			}
			const double* Solver::reaction(void) const
			{
				return m_R;
			}
			
			const double* Solver::inertial_force(bool flag) const
			{
				return flag ? m_Fnu : m_Fnk;
			}
			const double* Solver::internal_force(bool flag) const
			{
				return flag ? m_Fiu : m_Fik;
			}
			const double* Solver::external_force(bool flag) const
			{
				return flag ? m_Feu : m_Fek;
			}
			const double* Solver::reference_force(bool flag) const
			{
				return flag ? m_Fru : m_Frk;
			}

			const double* Solver::inertia(void) const
			{
				return m_M;
			}
			const double* Solver::damping(void) const
			{
				return m_C;
			}
			const double* Solver::stiffness(void) const
			{
				return m_K;
			}

			//analysis
			bool Solver::check(void) const
			{
				//sizes
				const unsigned nn = m_analysis->model()->mesh()->nodes();
				const unsigned nl = m_analysis->model()->boundary()->load_cases();
				//check watch dof
				if(m_watch_dof->m_node >= nn)
				{
					printf("Solver watch dof has out of range node!\n");
					return false;
				}
				//check load cases
				if(m_load_case >= nl && nl != 0)
				{
					printf("Solver selected load case is out of range!\n");
					return false;
				}
				//check dof limits
				if(m_dof_min > m_dof_max)
				{
					printf("Solver dof limits are inconsistent!\n");
					return false;
				}
				//check time limits
				if(m_time_max < m_time_min || m_time_increment_max < 0)
				{
					printf("Solver time limits are inconsistent!\n");
					return false;
				}
				//return
				return true;
			}
			void Solver::prepare(void)
			{
				m_t = m_time_min;
				m_dt = std::min((m_time_max - m_time_min) / m_step_max, m_time_increment_max);
				m_watch_dof->m_dof_index = m_analysis->model()->mesh()->node(m_watch_dof->m_node)->dof_index(m_watch_dof->m_dof);
			}
			bool Solver::stop(bool log) const
			{
				if(m_step > m_step_max)
				{
					if(log)
					{
						printf("\tMax steps reached!\n");
					}
					return true;
				}
				return false;
			}
			
			//linear solver
			void Solver::lindel(void)
			{
				umfpack_di_free_numeric(&m_num);
				umfpack_di_free_symbolic(&m_sym);
			}
			bool Solver::lindec(const double* K)
			{
				//data
				const int* c = m_analysis->assembler()->m_col_map;
				const int* r = m_analysis->assembler()->m_row_map;
				const unsigned n = m_analysis->assembler()->m_dof_unknow;
				//decompose
				if(umfpack_di_symbolic(n, n, c, r, K, &m_sym, nullptr, nullptr) == UMFPACK_OK)
				{
					return umfpack_di_numeric(c, r, K, m_sym, &m_num, nullptr, nullptr) == UMFPACK_OK;
				}
				else
				{
					return false;
				}
			}
			bool Solver::linsub(double* x, const double* K, const double* f)
			{
				//data
				const int* c = m_analysis->assembler()->m_col_map;
				const int* r = m_analysis->assembler()->m_row_map;
				//substitution
				return umfpack_di_solve(UMFPACK_A, c, r, K, x, f, m_num, nullptr, nullptr) == UMFPACK_OK;
			}
			double* Solver::linsolve(double* x, const double* K, const double* f)
			{
				//data
				const int* c = m_analysis->assembler()->m_col_map;
				const int* r = m_analysis->assembler()->m_row_map;
				const unsigned n = m_analysis->assembler()->m_dof_unknow;
				//solve
				bool t = false;
				if(umfpack_di_symbolic(n, n, c, r, K, &m_sym, nullptr, nullptr) == UMFPACK_OK)
				{
					if(umfpack_di_numeric(c, r, K, m_sym, &m_num, nullptr, nullptr) == UMFPACK_OK)
					{
						t = umfpack_di_solve(UMFPACK_A, c, r, K, x, f, m_num, nullptr, nullptr) == UMFPACK_OK;
					}
				}
				//free memory
				umfpack_di_free_numeric(&m_num);
				umfpack_di_free_symbolic(&m_sym);
				//return
				return t ? x : nullptr;
			}
			
			//static attributes
			Analysis* Solver::m_analysis = nullptr;
		}
	}
}
