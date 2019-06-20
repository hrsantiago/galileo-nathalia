//mat
#include "misc/util.h"

//fea
#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Nodes/Node.h"
#include "Mesh/Joints/States.h"
#include "Mesh/Joints/Joints.h"

#include "Models/Model.h"

#include "Boundary/Boundary.h"
#include "Boundary/Dependencies/Dependency.h"

namespace fea
{
	namespace mesh
	{
		namespace joints
		{
			//constructors
			Joint::Joint(void) : m_mass(0)
			{
				return;
			}

			//destructor
			Joint::~Joint(void)
			{
				for(boundary::Dependency* dependency : m_dependencies)
				{
					delete dependency;
				}
			}

			//serialization
			void Joint::load(FILE* file)
			{
				unsigned nn;
				fscanf(file, "%d", &nn);
				m_nodes.resize(nn);
				for(unsigned i = 0; i < nn; i++)
				{
					fscanf(file, "%d", &m_nodes[i]);
				}
			}
			void Joint::save(FILE* file) const
			{
				fprintf(file, "%02d ", (unsigned) m_nodes.size());
				for(const unsigned n : m_nodes)
				{
					fprintf(file, "%04d ", n);
				}
			}

			//create
			void Joint::create(Joint*& joint, const Joint* base)
			{
				Joint::create(joint, base->type(), base);
			}
			void Joint::create(Joint*& joint, joints::type type, const Joint* base)
			{
				//delete
				delete joint;
				//create
				switch(type)
				{
					case joints::type::hinge:
						joint = base ? new Hinge(*(Hinge*) base) : new Hinge;
						break;
					case joints::type::rigid:
						joint = base ? new Rigid(*(Rigid*) base) : new Rigid;
						break;
					case joints::type::spring:
						joint = base ? new Spring(*(Spring*) base) : new Spring;
						break;
					case joints::type::pinned:
						joint = base ? new Pinned(*(Pinned*) base) : new Pinned;
						break;
					case joints::type::spherical:
						joint = base ? new Spherical(*(Spherical*) base) : new Spherical;
						break;
				}
			}

			//data
			double Joint::mass(void) const
			{
				return m_mass;
			}
			double Joint::mass(double mass)
			{
				return m_mass = mass;
			}
			nodes::Node* Joint::node(unsigned index) const
			{
				return m_mesh->node(m_nodes[index]);
			}
			nodes::Node* Joint::node(unsigned index, unsigned node)
			{
				return m_mesh->node(m_nodes[index] = node);
			}
			
			//state
			const char* Joint::state_name(joints::state state)
			{
				switch(state)
				{
					case joints::state::axial:
						return "Axial";
					case joints::state::shear_y:
						return "Shear y";
					case joints::state::shear_z:
						return "Shear z";
					case joints::state::torsion:
						return "Torsion";
					case joints::state::bending_y:
						return "Bending y";
					case joints::state::bending_z:
						return "Bending z";
					default:
						return "error";
				}
			}
			
			//sizes
			unsigned Joint::nodes(void) const
			{
				return (unsigned) m_nodes.size();
			}
			unsigned Joint::dependencies(void) const
			{
				return (unsigned) m_dependencies.size();
			}
			
			//index
			unsigned Joint::index(void) const
			{
				for(unsigned i = 0; i < m_mesh->joints(); i++)
				{
					if(m_mesh->joint(i) == this)
					{
						return i;
					}
				}
				return 0;
			}
			unsigned Joint::index_node(unsigned index) const
			{
				return m_nodes[index];
			}
			
			//add
			void Joint::add_node(unsigned index)
			{
				m_nodes.push_back(index);
			}
			
			//remove
			void Joint::remove_node(unsigned index)
			{
				m_nodes.erase(m_nodes.begin() + index);
			}

			//analysis
			void Joint::add_dof(void) const
			{
				const std::vector<unsigned> dt = dofs();
				for(unsigned i = 0; i < m_nodes.size(); i++)
				{
					node(i)->add_dof_type(dt[i]);
				}
			}
			
			bool Joint::check(void) const
			{
				for(unsigned i : m_nodes)
				{
					if(i >= m_mesh->nodes())
					{
						printf("\tJoint %04d has out of range nodes!\n", index());
						return false;
					}
				}
				return true;
			}
			void Joint::prepare(void)
			{
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
			}
			void Joint::configure(void)
			{
				return;
			}
			
			void Joint::update(void)
			{
				return;
			}
			void Joint::restore(void)
			{
				return;
			}
			
			void Joint::record(void)
			{
				return;
			}
			void Joint::finish(void) const
			{
				//save Joint data
				char path[1000];
				sprintf(path, "%s/Joints/J%04d.txt", m_mesh->model()->folder().c_str(), index());
				FILE* file = fopen(path, "w");
				fprintf(file, m_results.c_str());
				fclose(file);
			}
			
			boundary::Dependency* Joint::create(void)
			{
				m_dependencies.push_back(new boundary::Dependency);
				return m_dependencies.back();
			}
			
			//static attributes
			Mesh* Joint::m_mesh = nullptr;
		}
	}
}
