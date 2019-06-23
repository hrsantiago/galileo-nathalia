#pragma once

#include <functional>

namespace fea
{
	//declarations
	namespace models
	{
		class Model;
	}
	namespace mesh
	{
		namespace nodes
		{
			enum class dof : unsigned;
		}
	}
	namespace analysis
	{
		class Analysis;
		class Assembler;
	}
	namespace boundary
	{
		class Initial;
		class Support;
		class Load_Case;
		class Dependency;
		namespace loads
		{
			enum class type : unsigned;
		}
	}

	namespace boundary
	{
		class Boundary
		{
		public:
			//friends
			friend class models::Model;
			friend class analysis::Analysis;
			friend class analysis::Assembler;

		protected:
			//constructors
			Boundary(void);

			//destructor
			virtual ~Boundary(void);

			//serialization
			void load(FILE*);
			void load_initials(FILE*);
			void load_supports(FILE*);
			void load_load_cases(FILE*);
			void load_dependencies(FILE*);
			
			void save(FILE*) const;
			void save_initials(FILE*) const;
			void save_supports(FILE*) const;
			void save_load_cases(FILE*) const;
			void save_dependencies(FILE*) const;

		public:
			//model
			models::Model* model(void) const;

			//data
			Initial* initial(unsigned) const;
			Support* support(unsigned) const;
			Load_Case* load_case(unsigned) const;
			Dependency* dependency(unsigned) const;
			
			//sizes
			unsigned initials(void) const;
			unsigned supports(void) const;
			unsigned load_cases(void) const;
			unsigned dependencies(void) const;

			//add
			Initial* add_initial(unsigned, mesh::nodes::dof, double, double);
			
			Support* add_support(unsigned, mesh::nodes::dof);
			Support* add_support(unsigned, mesh::nodes::dof, std::function<double(double)>);
			Support* add_support(unsigned, mesh::nodes::dof, double, double = 0, double = 0);
			
			Load_Case* add_load_case(const char* = "");
			Load_Case* add_load_case(unsigned, mesh::nodes::dof, double = 1, std::function<double(double)> = [] (double) { return 1; });
			Load_Case* add_load_case(loads::type, std::vector<unsigned> = {}, double = 1, std::function<double(double)> = [] (double) { return 1; });

			Load_Case* add_self_weight(const char*, const double*, double = 9.81);
			Load_Case* add_self_weight(const char*, mesh::nodes::dof, double = 9.81);
			
			Dependency* add_dependency(unsigned, mesh::nodes::dof, unsigned, mesh::nodes::dof, double = 1, double = 0);
			Dependency* add_dependency(unsigned, mesh::nodes::dof, std::vector<unsigned>, std::vector<mesh::nodes::dof>);

			//remove
			void remove_initial(unsigned);
			void remove_support(unsigned);
			void remove_load_case(unsigned);
			void remove_dependency(unsigned);

		private:
			//analysis
			bool check(void) const;
			void prepare(void) const;
			void apply_dof(void) const;
			
			void mesh_union(void);
			void mesh_split(void);
			
			void record(void) const;
			void finish(void) const;

			//results
			void plot(double, const double** = nullptr) const;

		protected:
			//attributes
			static models::Model* m_model;

			std::vector<Initial*> m_initials;
			std::vector<Support*> m_supports;
			std::vector<Load_Case*> m_load_cases;
			std::vector<Dependency*> m_dependencies;
		};
	}
}
