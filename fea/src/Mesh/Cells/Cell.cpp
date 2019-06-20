//std
#include <cstring>

//mat
#include "linear/dense.h"
#include "linear/linear.h"

//fea
#include "Mesh/Nodes/Node.h"
#include "Mesh/Cells/Cells.h"
#include "Mesh/Sections/Section.h"
#include "Mesh/Elements/Element.h"

namespace fea
{
	namespace mesh
	{
		namespace cells
		{
			//constructors
			Cell::Cell(void) : m_rule(2), m_label("Cell")
			{
				return;
			}

			//destructor
			Cell::~Cell(void)
			{
				return;
			}

			//create
			void Cell::create(Cell*& cell, const Cell* base)
			{
				Cell::create(cell, base->type(), base);
			}
			void Cell::create(Cell*& cell, cells::type type, const Cell* base)
			{
				//delete cell
				delete cell;
				//create cell
				switch(type)
				{
					case cells::type::bar:
						cell = base ? new Bar(*(Bar*) base) : new Bar;
						break;
					case cells::type::beam:
						cell = base ? new Beam(*(Beam*) base) : new Beam;
						break;
					case cells::type::tri3:
						cell = base ? new Tri3(*(Tri3*) base) : new Tri3;
						break;
					case cells::type::tri6:
						cell = base ? new Tri6(*(Tri6*) base) : new Tri6;
						break;
					case cells::type::quad4:
						cell = base ? new Quad4(*(Quad4*) base) : new Quad4;
						break;
					case cells::type::quad8:
						cell = base ? new Quad8(*(Quad8*) base) : new Quad8;
						break;
					case cells::type::quad9:
						cell = base ? new Quad9(*(Quad9*) base) : new Quad9;
						break;
					case cells::type::tetra4:
						cell = base ? new Tetra4(*(Tetra4*) base) : new Tetra4;
						break;
					case cells::type::tetra10:
						cell = base ? new Tetra10(*(Tetra10*) base) : new Tetra10;
						break;
					case cells::type::brick8:
						cell = base ? new Brick8(*(Brick8*) base) : new Brick8;
						break;
					case cells::type::brick20:
						cell = base ? new Brick20(*(Brick20*) base) : new Brick20;
						break;
					case cells::type::brick27:
						cell = base ? new Brick27(*(Brick27*) base) : new Brick27;
						break;
				}
			}
			
			//name
			const char* Cell::name(cells::type type)
			{
				switch(type)
				{
					case cells::type::bar:
						return "bar";
					case cells::type::beam:
						return "beam";
					case cells::type::tri3:
						return "tri3";
					case cells::type::tri6:
						return "tri6";
					case cells::type::quad4:
						return "quad4";
					case cells::type::quad8:
						return "quad8";
					case cells::type::quad9:
						return "quad9";
					case cells::type::tetra4:
						return "tetra4";
					case cells::type::brick8:
						return "brick8";
					case cells::type::tetra10:
						return "tetra10";
					case cells::type::brick20:
						return "brick20";
					case cells::type::brick27:
						return "brick27";
				}
			}
			const char* Cell::name(void) const
			{
				return name(type());
			}
			
			//data
			unsigned Cell::rule(void) const
			{
				return m_rule;
			}
			unsigned Cell::rule(unsigned rule)
			{
				return m_rule = rule;
			}
			
			const char* Cell::label(void) const
			{
				return m_label;
			}
			const char* Cell::label(const char* label)
			{
				return (const char*) strcpy(m_label, label);
			}
			
			unsigned Cell::max_rule(void)
			{
				return m_max_rule;
			}
			unsigned Cell::max_nodes(void)
			{
				return m_max_nodes;
			}

			//geometry
			double Cell::mass(const elements::Element* element, double r) const
			{
				switch(dimension())
				{
					case 1:
					{
						const double L = edge(element, 0);
						const double A = ((const Line*) this)->section()->area();
						return r * L * A;
					}
					case 2:
					{
						const double A = face(element, 0);
						const double t = ((const Plane*) this)->thickness();
						return r * t * A;
					}
					case 3:
					{
						double J[9], p[3];
						memset(p, 0, 3 * sizeof(double));
						const double j = jacobian(J, element, p);
						return 8 * r * j;
					}
				}
			}
			double Cell::edge(const elements::Element* element, unsigned e) const
			{
				double l = 0;
				const std::vector<unsigned> v = edge(e);
				for(unsigned i = 0; i + 1 < v.size(); i++)
				{
					const double* xi = element->node(v[i + 0])->coordinates();
					const double* xj = element->node(v[i + 1])->coordinates();
					l += mat::norm(xj, xi, 3);
				}
				return l;
			}
			double Cell::face(const elements::Element* element, unsigned f) const
			{
				double A = 0, pi[3], pj[3], pc[3];
				const std::vector<unsigned> v = face(f);
				const double* x0 = element->node(v[0])->coordinates();
				for(unsigned i = 0; i + 2 < v.size(); i++)
				{
					const double* xi = element->node(v[i + 1])->coordinates();
					const double* xj = element->node(v[i + 2])->coordinates();
					A += mat::norm(mat::cross(pc, mat::sub(pi, xi, x0, 3), mat::sub(pj, xj, x0, 3)), 3) / 2;
				}
				return A;
			}
			double Cell::jacobian(double* J, const elements::Element* element, const double* p) const
			{
				//sizes
				const unsigned nv = vertices();
				const unsigned nd = dimension();
				//points
				double Pg[3 * m_max_nodes];
				double dN[3 * m_max_nodes];
				for(unsigned i = 0; i < nv; i++)
				{
					const double* x = element->node(i)->coordinates();
					for(unsigned j = 0; j < nd; j++)
					{
						Pg[j + nd * i] = x[j];
					}
				}
				//return
				return mat::det(mat::multiply(J, Pg, gradient(dN, p), nd, nv, nd), nd);
			}
			
			//analysis
			bool Cell::check(void) const
			{
				return true;
			}

			//static attributes
			Mesh* Cell::m_mesh = nullptr;
		}
	}
}
