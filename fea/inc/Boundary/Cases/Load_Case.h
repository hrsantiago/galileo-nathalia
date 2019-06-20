#pragma once

#include <vector>

namespace fea
{
	//declarations
	namespace mesh
	{
		namespace nodes
		{
			enum class dof : unsigned;
		}
	}
	namespace boundary
	{
		class Boundary;
		namespace loads
		{
			class Node;
			class Element;
			enum class type : unsigned;
		}
	}
	namespace analysis
	{
		class Assembler;
	}

	namespace boundary
	{
		class Load_Case
		{
		public:
			//friends
			friend class boundary::Boundary;
			friend class analysis::Assembler;

		protected:
			//constructors
			Load_Case(void);

			//destructor
			virtual ~Load_Case(void);

			//serialization
			void load(FILE*);
			void save(FILE*) const;

		public:
			//data
			const char* label(void) const;
			const char* label(const char*);
			loads::Node* load_node(unsigned) const;
			loads::Element* load_element(unsigned) const;
			
			//sizes
			unsigned loads_nodes(void) const;
			unsigned loads_elements(void) const;

			//index
			unsigned index(void) const;

			//add
			void add_load_node(unsigned, mesh::nodes::dof, double = 1, std::function<double(double)> = [] (double) { return 1; });
			void add_load_element(loads::type, std::vector<unsigned> = {}, double = 1, std::function<double(double)> = [] (double) { return 1; });

			//remove
			void remove_load_node(unsigned);
			void remove_load_element(unsigned);

		protected:
			//analysis
			bool check(void) const;
			void prepare(void);

			//results
			void plot(double, const double*, const double** = nullptr) const;

			//attributes
			static Boundary* m_boundary;

			char m_label[200];

			std::vector<loads::Node*> m_loads_nodes;
			std::vector<loads::Element*> m_loads_elements;
		};
	}
}
