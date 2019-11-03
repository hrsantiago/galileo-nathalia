#pragma once

#include <vector>
#include <string>

namespace fea
{
	//declarations
	namespace mesh
	{
		class Mesh;
		namespace nodes
		{
			class Node;
		}
		namespace cells
		{
			class Cell;
		}
		namespace elements
		{
			enum class type : unsigned;
			enum class state : unsigned;
		}
		namespace materials
		{
			class Material;
		}
		namespace points
		{
			class Point;
			enum class type : unsigned;
		}
	}
	namespace boundary
	{
		namespace loads
		{
			class Element;
		}
	}
	namespace analysis
	{
		class Assembler;
	}

	namespace mesh
	{
		namespace elements
		{
			class Element
			{
			public:
				//friends
				friend class mesh::Mesh;
				friend class analysis::Assembler;
				friend class boundary::loads::Element;

			protected:
				//constructors
				Element(void);

				//destructor
				virtual ~Element(void);

				//serialization
				virtual void load(FILE*);
				virtual void save(FILE*) const;

				//create
				static void create(Element*&, const Element*);
				static void create(Element*&, elements::type, const Element* = nullptr);

			public:
				//data
				static Mesh* mesh(void);
				
				cells::Cell* cell(void) const;
				cells::Cell* cell(unsigned);
				
				nodes::Node* node(unsigned) const;
				nodes::Node* node(unsigned, unsigned);
				
				materials::Material* material(void) const;
				materials::Material* material(unsigned);

				//types
				virtual unsigned cells(void) const = 0;
				virtual unsigned loads(void) const = 0;
				virtual unsigned states(void) const = 0;
				virtual unsigned dofs(unsigned) const = 0;
				virtual points::type point(void) const = 0;
				virtual elements::type type(void) const = 0;
				virtual std::vector<unsigned> dofs(void) const;
				
				//name
				virtual const char* name(void) const;
				static const char* name(elements::type);
				
				//state
				static const char* state_name(state);
				
				//sizes
				const std::vector<unsigned>& nodes(void) const;

				//index
				unsigned index(void) const;
				unsigned index_cell(void) const;
				unsigned index_node(unsigned) const;
				unsigned index_material(void) const;

				//add
				void add_node(unsigned);

				//remove
				void remove_node(unsigned);

			protected:
				//analysis
				void add_dof(void) const;

				virtual bool symmetric(void) const;

				virtual void apply(void);

				virtual bool check(void) const;
				virtual void prepare(void);
				
				virtual void update(void);
				virtual void restore(void);
				
				virtual void record(void);
				virtual void finish(void) const;
				
				//data
				double* state(double*, unsigned = 1) const;
				double* velocity(double*, unsigned = 1) const;
				double* acceleration(double*, unsigned = 1) const;

				//formulation
				virtual double* internal_force(double*) const = 0;
				virtual double* inertial_force(double*) const = 0;
				
				virtual double* reference_force(double*, const boundary::loads::Element*) const = 0;

				virtual double* inertia(double*) const = 0;
				virtual double* damping(double*) const = 0;
				virtual double* stiffness(double*) const = 0;

				//attributes
				static Mesh* m_mesh;

				unsigned m_cell;
				unsigned m_material;

				std::vector<unsigned> m_nodes;
				std::vector<points::Point*> m_points;

				std::vector<unsigned> m_dof;

				std::string m_results;
			};
		}
	}
}
