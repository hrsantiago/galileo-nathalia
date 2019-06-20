#pragma once

#include <cstdio>

namespace fea
{
	//forward declarations
	namespace models
	{
		class Model;
	}
	namespace analysis
	{
		class Assembler;
		namespace solvers
		{
			class Solver;
			enum class type : unsigned;
		}
	}

	namespace analysis
	{
		class Analysis
		{
		public:
			//friends
			friend class models::Model;
			friend class analysis::Assembler;

		protected:
			//constructors
			Analysis(void);

			//destructor
			virtual ~Analysis(void);

			//serialization
			void load(FILE*);
			void save(FILE*) const;

		public:
			//model
			models::Model* model(void) const;

			//data
			bool analysed(void) const;
			
			Assembler* assembler(void) const;
			solvers::Solver* solver(void) const;
			solvers::Solver* solver(solvers::type);
			
			//solve
			bool solve(bool = false);

		protected:
			//analysis
			bool check(void) const;
			void prepare(void) const;
			void finish(void) const;
			
			//attributes
			static models::Model* m_model;

			bool m_analysed;

			Assembler* m_assembler;
			solvers::Solver* m_solver;
		};
	}
}
