#pragma once

#include "Boundary/Loads/Load.h"

namespace fea
{
	//declarations
	namespace mesh
	{
		namespace nodes
		{
			class Node;
			enum class dof : unsigned;
		}
	}
	namespace boundary
	{
		class Boundary;
		class Load_Case;
	}
	namespace analysis
	{
		class Assembler;
	}

	namespace boundary
	{
		namespace loads
		{
			class Node : public Load
			{
			public:
				//friends
				friend class boundary::Boundary;
				friend class boundary::Load_Case;
				friend class analysis::Assembler;

			protected:
				//constructors
				Node(void);

				//destructor
				virtual ~Node(void) override;

				//serialization
				virtual void load(FILE*) override;
				virtual void save(FILE*) const override;

			public:
				//data
				mesh::nodes::dof dof(void) const;
				mesh::nodes::dof dof(mesh::nodes::dof);
				mesh::nodes::Node* node(void) const;
				mesh::nodes::Node* node(unsigned);

				//index
				unsigned index(void) const;

			protected:
				//analysis
				bool check(void) const override;
				void prepare(void);
				void add_dof(void) const;
				
				//results
				virtual void plot(double, const double*, const double** = nullptr) const override;
				
				virtual void plot_rotation_x(double, const double*, const double*) const;
				virtual void plot_rotation_y(double, const double*, const double*) const;
				virtual void plot_rotation_z(double, const double*, const double*) const;
				virtual void plot_translation_x(double, const double*, const double*) const;
				virtual void plot_translation_y(double, const double*, const double*) const;
				virtual void plot_translation_z(double, const double*, const double*) const;
				
				//attributes
				unsigned m_dof;
				unsigned m_node;
				mesh::nodes::dof m_dof_type;
			};
		}
	}
}
