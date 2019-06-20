//fea
#include "Models/Model.h"

#include "Mesh/Nodes/Node.h"

#include "Boundary/Boundary.h"
#include "Boundary/Cases/Load_Case.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Types.h"
#include "Analysis/Solvers/Solver.h"
#include "Analysis/Solvers/Watch_Dof.h"
#include "Analysis/Assembler/Assembler.h"
#include "Analysis/Strategies/Strategy.h"

namespace fea
{
	namespace analysis
	{
		//constructors
		Analysis::Analysis(void) : m_analysed(false), m_assembler(new Assembler), m_solver(nullptr)
		{
			//solver
			solvers::Solver::create(m_solver, solvers::type::static_linear);
			//analysis
			Assembler::m_analysis = this;
			solvers::Solver::m_analysis = this;
			strategies::Strategy::m_analysis = this;
		}
	
		//destructor
		Analysis::~Analysis(void)
		{
			delete m_solver;
			delete m_assembler;
		}
	
		//serialization
		void Analysis::load(FILE* file)
		{
			//create solver
			solvers::type type;
			fscanf(file, "%d %d", &m_analysed, &type);
			solvers::Solver::create(m_solver, type);
			//load solver
			m_solver->load(file);
		}
		void Analysis::save(FILE* file) const
		{
			//solver type
			fprintf(file, "%01d %02d\n", m_analysed, (unsigned) m_solver->type());
			//solver data
			m_solver->save(file);
		}
	
		//model
		models::Model* Analysis::model(void) const
		{
			return m_model;
		}

		//data
		bool Analysis::analysed(void) const
		{
			return m_analysed;
		}
		
		Assembler* Analysis::assembler(void) const
		{
			return m_assembler;
		}
		solvers::Solver* Analysis::solver(void) const
		{
			return m_solver;
		}
		solvers::Solver* Analysis::solver(solvers::type type)
		{
			if(m_solver == nullptr || m_solver->type() != type)
			{
				solvers::Solver::create(m_solver, type);
			}
			return m_solver;
		}

		//solve
		bool Analysis::solve(bool advance)
		{
			//header
			printf("************************   Solution process started   *************************\n");
			//union
			m_model->boundary()->mesh_union();
			//check
			if(!m_analysed)
			{
				m_analysed = m_assembler->check();
			}
			if(!m_analysed)
			{
				return false;
			}
			//prepare
			if(!advance)
			{
				m_assembler->prepare();
			}
			//header
			printf("-------------------------------------------------------------------------------\n");
			printf("Analysis:\n");
			printf("\tMax Steps: \t%d\n", m_solver->m_step_max);
			printf("\tWatch dof: \t%d %s\n", m_solver->m_watch_dof->m_node, mesh::nodes::Node::dof_name(m_solver->m_watch_dof->m_dof));
			if(m_solver->m_dead)
			{
				printf("\tDead Case: \t%s\n", m_model->boundary()->load_case(m_solver->m_dead_case)->label());
			}
			if(m_model->boundary()->load_cases() != 0)
			{
				printf("\tLoad Case: \t%s\n", m_model->boundary()->load_case(m_solver->m_load_case)->label());
			}
			printf("\tSolver Type: \t%s\n", m_solver->name());
			printf("-------------------------------------------------------------------------------\n");
			//solve
			m_solver->run();
			//split
			m_model->boundary()->mesh_split();
			//finish
			m_assembler->finish();
			//print
			printf("\tDone: Solution process finished!\n\n");
			//return
			return m_analysed;
		}

		//analysis
		bool Analysis::check(void) const
		{
			return m_solver->check();
		}
		void Analysis::prepare(void) const
		{
			m_solver->prepare();
		}
		void Analysis::finish(void) const
		{
			m_solver->finish();
		}
		
		//static attributes
		models::Model* Analysis::m_model = nullptr;
	}
}