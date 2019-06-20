//mat
#include "misc/util.h"

//fea
#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Nodes/Node.h"
#include "Mesh/Cells/Cell.h"
#include "Mesh/Points/Point.h"
#include "Mesh/Elements/States.h"
#include "Mesh/Elements/Elements.h"

#include "Models/Model.h"

namespace fea
{
	namespace mesh
	{
		namespace elements
		{
			//constructors
			Element::Element(void) : m_cell(0), m_material(0)
			{
				return;
			}

			//destructor
			Element::~Element(void)
			{
				for(points::Point* point : m_points)
				{
					delete point;
				}
			}

			//serialization
			void Element::load(FILE* file)
			{
				//load cell
				fscanf(file, "%d", &m_cell);
				//load material
				fscanf(file, "%d", &m_material);
				//load nodes
				unsigned nn;
				fscanf(file, "%d", &nn);
				m_nodes.resize(nn);
				for(unsigned i = 0; i < nn; i++)
				{
					fscanf(file, "%d", &m_nodes[i]);
				}
			}
			void Element::save(FILE* file) const
			{
				//save cell
				fprintf(file, "%02d ", m_cell);
				//save material
				fprintf(file, "%02d ", m_material);
				//save nodes
				fprintf(file, "%02d ", (unsigned) m_nodes.size());
				for(const unsigned n : m_nodes)
				{
					fprintf(file, "%04d ", n);
				}
			}

			//create
			void Element::create(Element*& element, const Element* base)
			{
				Element::create(element, base->type(), base);
			}
			void Element::create(Element*& element, elements::type type, const Element* base)
			{
				//delete element
				delete element;
				//create element
				switch(type)
				{
					case elements::type::bar:
						element = base ? new Bar(*(Bar*) base) : new Bar;
						break;
					case elements::type::rope:
						element = base ? new Rope(*(Rope*) base) : new Rope;
						break;
					case elements::type::heat:
						element = base ? new Heat(*(Heat*) base) : new Heat;
						break;
					case elements::type::cable:
						element = base ? new Cable(*(Cable*) base) : new Cable;
						break;
					case elements::type::beam2:
						element = base ? new Beam2(*(Beam2*) base) : new Beam2;
						break;
					case elements::type::beam3:
						element = base ? new Beam3(*(Beam3*) base) : new Beam3;
						break;
					case elements::type::plane:
						element = base ? new Plane(*(Plane*) base) : new Plane;
						break;
					case elements::type::solid:
						element = base ? new Solid(*(Solid*) base) : new Solid;
						break;
				}
			}

			//data
			cells::Cell* Element::cell(void) const
			{
				return m_mesh->cell(m_cell);
			}
			cells::Cell* Element::cell(unsigned cell)
			{
				return m_mesh->cell(m_cell = cell);
			}
			nodes::Node* Element::node(unsigned index) const
			{
				return m_mesh->node(m_nodes[index]);
			}
			nodes::Node* Element::node(unsigned index, unsigned node)
			{
				return m_mesh->node(m_nodes[index] = node);
			}
			materials::Material* Element::material(void) const
			{
				return m_mesh->material(m_material);
			}
			materials::Material* Element::material(unsigned material)
			{
				return m_mesh->material(m_material = material);
			}

			//types
			std::vector<unsigned> Element::dofs(void) const
			{
				const unsigned nn = m_nodes.size();
				std::vector<unsigned> d(nn);
				for(unsigned i = 0; i < nn; i++)
				{
					d[i] = dofs(i);
				}
				return d;
			}
			
			//name
			const char* Element::name(void) const
			{
				return name(type());
			}
			const char* Element::name(elements::type type)
			{
				switch(type)
				{
					case elements::type::bar:
						return "bar";
					case elements::type::rope:
						return "rope";
					case elements::type::heat:
						return "heat";
					case elements::type::cable:
						return "cable";
					case elements::type::beam2:
						return "beam2";
					case elements::type::beam3:
						return "beam3";
					case elements::type::plane:
						return "plane";
					case elements::type::solid:
						return "solid";
				}
			}

			//state
			const char* Element::state_name(elements::state state)
			{
				switch(state)
				{
					case elements::state::axial:
						return "Axial";
					case elements::state::shear_y:
						return "Shear y";
					case elements::state::shear_z:
						return "Shear z";
					case elements::state::torsion:
						return "Torsion";
					case elements::state::bending_y:
						return "Bending y";
					case elements::state::bending_z:
						return "Bending z";
					case elements::state::heat_flux:
						return "Heat Flux";
					default:
						return "error";
				}
			}

			//sizes
			unsigned Element::nodes(void) const
			{
				return (unsigned) m_nodes.size();
			}

			//index
			unsigned Element::index(void) const
			{
				for(unsigned i = 0; i < m_mesh->elements(); i++)
				{
					if(m_mesh->element(i) == this)
					{
						return i;
					}
				}
				return 0;
			}
			unsigned Element::index_cell(void) const
			{
				return m_cell;
			}
			unsigned Element::index_node(unsigned index) const
			{
				return m_nodes[index];
			}
			unsigned Element::index_material(void) const
			{
				return m_material;
			}

			//add
			void Element::add_node(unsigned index)
			{
				m_nodes.push_back(index);
			}

			//remove
			void Element::remove_node(unsigned index)
			{
				m_nodes.erase(m_nodes.begin() + index);
			}

			//analysis
			void Element::add_dof(void) const
			{
				const std::vector<unsigned> dt = dofs();
				for(unsigned i = 0; i < m_nodes.size(); i++)
				{
					node(i)->add_dof_type(dt[i]);
				}
			}
			
			bool Element::symmetric(void) const
			{
				return true;
			}
			
			void Element::apply(void)
			{
				return;
			}
			
			bool Element::check(void) const
			{
				//check nodes
				if(m_nodes.empty())
				{
					printf("\tElement %04d has no nodes!\n", index());
					return false;
				}
				else
				{
					const unsigned nn = m_mesh->nodes();
					for(unsigned i = 0; i < m_nodes.size(); i++)
					{
						if(m_nodes[i] >= nn)
						{
							printf("\tElement %04d has out of range nodes!\n", index());
							return false;
						}
					}
				}
				//check cell
				if(m_cell >= m_mesh->cells())
				{
					printf("\tElement %04d has out of range cell!\n", index());
					return false;
				}
				if(!((unsigned) m_mesh->cell(m_cell)->type() & cells()))
				{
					printf("\tElement %04d has no correct cell avaliable!\n", index());
					return false;
				}
				//check material
				if(m_material >= m_mesh->materials())
				{
					printf("\tElement %04d has out of range material!\n", index());
					return false;
				}
				//return
				return true;
			}
			void Element::prepare(void)
			{
				//set dof
				std::vector<unsigned> dt = dofs();
				for(unsigned i = 0; i < m_nodes.size(); i++)
				{
					for(unsigned j = 1; j < (unsigned) nodes::dof::last; j <<= 1)
					{
						if(dt[i] & j)
						{
							const char p = mat::bit_find(node(i)->m_dof_types, j);
							m_dof.push_back(node(i)->m_dof[p]);
						}
					}
				}
				//points
				m_points.resize(cell()->points());
				for(unsigned i = 0; i < m_points.size(); i++)
				{
					points::Point::create(m_points[i], point());
					m_points[i]->prepare(this);
				}
			}
			
			void Element::update(void)
			{
				for(points::Point* point : m_points)
				{
					point->update();
				}
			}
			void Element::restore(void)
			{
				for(points::Point* point : m_points)
				{
					point->restore();
				}
			}
			
			void Element::record(void)
			{
				return;
			}
			void Element::finish(void) const
			{
				char file_path[2000];
				const std::string folder_path = m_mesh->model()->path() + "/" + m_mesh->model()->name();
				sprintf(file_path, "%s/Elements/E%04d.txt", folder_path.c_str(), index());
				FILE* file = fopen(file_path, "w");
				fprintf(file, m_results.c_str());
				fclose(file);
			}

			//data
			double* Element::state(double* u, unsigned c) const
			{
				unsigned p = 0;
				const std::vector<unsigned> dt = dofs();
				for(unsigned i = 0; i < dt.size(); i++)
				{
					for(unsigned j = 1; j < (unsigned) nodes::dof::last; j <<= 1)
					{
						if(dt[i] & j)
						{
							u[p++] = node(i)->state((nodes::dof) j, c);
						}
					}
				}
				return u;
			}
			double* Element::velocity(double* v, unsigned c) const
			{
				unsigned p = 0;
				const std::vector<unsigned> dt = dofs();
				for(unsigned i = 0; i < dt.size(); i++)
				{
					for(unsigned j = 1; j < (unsigned) nodes::dof::last; j <<= 1)
					{
						if(dt[i] & j)
						{
							v[p++] = node(i)->velocity((nodes::dof) j, c);
						}
					}
				}
				return v;
			}
			double* Element::acceleration(double* a, unsigned c) const
			{
				unsigned p = 0;
				const std::vector<unsigned> dt = dofs();
				for(unsigned i = 0; i < dt.size(); i++)
				{
					for(unsigned j = 1; j < (unsigned) nodes::dof::last; j <<= 1)
					{
						if(dt[i] & j)
						{
							a[p++] = node(i)->acceleration((nodes::dof) j, 0);
						}
					}
				}
				return a;
			}
			
			//static attributes
			Mesh* Element::m_mesh = nullptr;
		}
	}
}
