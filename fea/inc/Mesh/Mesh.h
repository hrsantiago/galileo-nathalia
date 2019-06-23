#pragma once

#include <cstdio>
#include <vector>

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
			class Node;
		}
		namespace cells
		{
			class Cell;
			enum class type : unsigned;
		}
		namespace joints
		{
			class Joint;
			enum class type : unsigned;
		}
		namespace materials
		{
			class Material;
			enum class type : unsigned;
		}
		namespace sections
		{
			class Section;
			enum class type : unsigned;
		}
		namespace elements
		{
			class Element;
			enum class type : unsigned;
		}
	}
	namespace boundary
	{
		class Boundary;
	}
	namespace analysis
	{
		class Assembler;
	}

	namespace mesh
	{
		class Mesh
		{
		public:
			//friends
			friend class nodes::Node;
			friend class cells::Cell;
			friend class joints::Joint;
			friend class sections::Section;
			friend class elements::Element;
			friend class materials::Material;
			
			friend class models::Model;
			friend class boundary::Boundary;
			friend class analysis::Assembler;

		protected:
			//constructors
			Mesh(void);

			//destructor
			~Mesh(void);

			//serialization
			void load(FILE*);
			void load_nodes(FILE*);
			void load_cells(FILE*);
			void load_joints(FILE*);
			void load_sections(FILE*);
			void load_elements(FILE*);
			void load_materials(FILE*);
			
			void save(FILE*) const;
			void save_nodes(FILE*) const;
			void save_cells(FILE*) const;
			void save_joints(FILE*) const;
			void save_sections(FILE*) const;
			void save_elements(FILE*) const;
			void save_materials(FILE*) const;

		public:
			//model
			models::Model* model(void) const;

			//data
			nodes::Node* node(unsigned) const;
			cells::Cell* cell(unsigned) const;
			joints::Joint* joint(unsigned) const;
			sections::Section* section(unsigned) const;
			elements::Element* element(unsigned) const;
			materials::Material* material(unsigned) const;
			
			//bound
			double size(void) const;
			double limit(unsigned, bool) const;

			//sizes
			unsigned nodes(void) const;
			unsigned cells(void) const;
			unsigned joints(void) const;
			unsigned sections(void) const;
			unsigned elements(void) const;
			unsigned materials(void) const;

			//add
			nodes::Node* add_node(const double*);
			nodes::Node* add_node(double, double, double);
			
			cells::Cell* add_cell(cells::type);
			
			joints::Joint* add_joint(joints::type, std::vector<unsigned> = {});
			
			sections::Section* add_section(sections::type);
			
			elements::Element* add_element(const elements::Element*);
			elements::Element* add_element(elements::type, std::vector<unsigned> = {}, unsigned = 0, unsigned = 0);

			materials::Material* add_material(materials::type);

			//remove
			void remove_node(unsigned);
			void remove_cell(unsigned);
			void remove_joint(unsigned);
			void remove_section(unsigned);
			void remove_element(unsigned);
			void remove_material(unsigned);

		protected:
			//analysis
			bool check(void) const;
			void prepare(void) const;
			void finish(void) const;
			void record(void) const;
			void update(void) const;
			void restore(void) const;
			void apply_dof(void) const;
			
			//results
			void plot(double, const double** = nullptr, const double* = nullptr, const double** = nullptr) const;

			//attributes
			static models::Model* m_model;

			std::vector<nodes::Node*> m_nodes;
			std::vector<cells::Cell*> m_cells;
			std::vector<joints::Joint*> m_joints;
			std::vector<sections::Section*> m_sections;
			std::vector<elements::Element*> m_elements;
			std::vector<materials::Material*> m_materials;
		};
	}
}
