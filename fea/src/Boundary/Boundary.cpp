//std
#include <cmath>
#include <cstring>
#include <algorithm>

//mat
#include "misc/util.h"

//fea
#include "Model/Model.h"

#include "Plot/Plot.h"
#include "Plot/What.h"
#include "Plot/Sizes.h"
#include "Plot/Colors.h"

#include "Mesh/Mesh.h"
#include "Mesh/Cells/Cell.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Nodes/Node.h"
#include "Mesh/Joints/Joint.h"
#include "Mesh/Elements/Element.h"
#include "Mesh/Materials/Material.h"

#include "Boundary/Boundary.h"
#include "Boundary/Cases/Load_Case.h"
#include "Boundary/Initials/Initial.h"
#include "Boundary/Supports/Support.h"
#include "Boundary/Loads/Nodes/Node.h"
#include "Boundary/Loads/Elements/Element.h"
#include "Boundary/Dependencies/Dependency.h"

#include "Analysis/Analysis.h"
#include "Analysis/Solvers/Solver.h"

namespace fea
{
	namespace boundary
	{
		//constructors
		Boundary::Boundary(void)
		{
			Initial::m_boundary = this;
			Support::m_boundary = this;
			Load_Case::m_boundary = this;
			Dependency::m_boundary = this;
		}

		//destructor
		Boundary::~Boundary(void)
		{
			//delete nodes
			for(const Initial* initial : m_initials)
			{
				delete initial;
			}
			//delete supports
			for(const Support* support : m_supports)
			{
				delete support;
			}
			//delete joints
			for(const Load_Case* load_case : m_load_cases)
			{
				delete load_case;
			}
			//delete sections
			for(const Dependency* dependency : m_dependencies)
			{
				delete dependency;
			}
		}

		//serialization
		void Boundary::load(FILE* file)
		{
			load_initials(file);
			load_supports(file);
			load_load_cases(file);
			load_dependencies(file);
		}
		void Boundary::load_initials(FILE* file)
		{
			unsigned ni;
			fscanf(file, "%d", &ni);
			m_initials.resize(ni);
			for(Initial*& initial : m_initials)
			{
				initial = new Initial;
				initial->load(file);
			}
		}
		void Boundary::load_supports(FILE* file)
		{
			unsigned ns;
			fscanf(file, "%d", &ns);
			m_supports.resize(ns);
			for(Support*& support : m_supports)
			{
				support = new Support;
				support->load(file);
			}
		}
		void Boundary::load_load_cases(FILE* file)
		{
			unsigned nl;
			fscanf(file, "%d", &nl);
			m_load_cases.resize(nl);
			for(Load_Case*& load_case : m_load_cases)
			{
				load_case = new Load_Case;
				load_case->load(file);
			}
		}
		void Boundary::load_dependencies(FILE* file)
		{
			unsigned nd;
			fscanf(file, "%d", &nd);
			m_dependencies.resize(nd);
			for(Dependency*& dependency : m_dependencies)
			{
				dependency = new Dependency;
				dependency->load(file);
			}
		}
		
		void Boundary::save(FILE* file) const
		{
			save_initials(file);
			save_supports(file);
			save_load_cases(file);
			save_dependencies(file);
		}
		void Boundary::save_initials(FILE* file) const
		{
			fprintf(file, "%04d\n", (unsigned) m_initials.size());
			for(const Initial* initial : m_initials)
			{
				initial->save(file);
				fprintf(file, "\n");
			}
		}
		void Boundary::save_supports(FILE* file) const
		{
			fprintf(file, "%04d\n", (unsigned) m_supports.size());
			for(const Support* support : m_supports)
			{
				support->save(file);
				fprintf(file, "\n");
			}
		}
		void Boundary::save_load_cases(FILE* file) const
		{
			fprintf(file, "%04d\n", (unsigned) m_load_cases.size());
			for(const Load_Case* load_case : m_load_cases)
			{
				load_case->save(file);
			}
		}
		void Boundary::save_dependencies(FILE* file) const
		{
			fprintf(file, "%04d\n", (unsigned) m_dependencies.size());
			for(const Dependency* dependency : m_dependencies)
			{
				dependency->save(file);
				fprintf(file, "\n");
			}
		}

		//model
		models::Model* Boundary::model(void) const
		{
			return m_model;
		}

		//data
		Initial* Boundary::initial(unsigned index) const
		{
			return index < m_initials.size() ? m_initials[index] : nullptr;
		}
		Support* Boundary::support(unsigned index) const
		{
			return index < m_supports.size() ? m_supports[index] : nullptr;
		}
		Load_Case* Boundary::load_case(unsigned index) const
		{
			return index < m_load_cases.size() ? m_load_cases[index] : nullptr;
		}
		Dependency* Boundary::dependency(unsigned index) const
		{
			return index < m_dependencies.size() ? m_dependencies[index] : nullptr;
		}

		//sizes
		unsigned Boundary::initials(void) const
		{
			return (unsigned) m_initials.size();
		}
		unsigned Boundary::supports(void) const
		{
			return (unsigned) m_supports.size();
		}
		unsigned Boundary::load_cases(void) const
		{
			return (unsigned) m_load_cases.size();
		}
		unsigned Boundary::dependencies(void) const
		{
			return (unsigned) m_dependencies.size();
		}

		//add
		Initial* Boundary::add_initial(unsigned node, mesh::nodes::dof dof, double u0, double v0)
		{
			Initial* initial = new Initial;
			initial->m_node = node;
			initial->m_state = u0;
			initial->m_velocity = v0;
			initial->m_dof_type = dof;
			m_initials.push_back(initial);
			return initial;
		}
		
		Support* Boundary::add_support(unsigned node, mesh::nodes::dof dof)
		{
			Support* support = new Support;
			support->m_fix = true;
			support->m_node = node;
			support->m_dof_type = dof;
			m_supports.push_back(support);
			return support;
		}
		Support* Boundary::add_support(unsigned node, mesh::nodes::dof dof, std::function<double(double)> state)
		{
			Support* support = new Support;
			support->m_fix = true;
			support->m_node = node;
			support->m_state = state;
			support->m_dof_type = dof;
			m_supports.push_back(support);
			return support;
		}
		Support* Boundary::add_support(unsigned node, mesh::nodes::dof dof, double k, double c, double m)
		{
			Support* support = new Support;
			support->m_fix = false;
			support->m_node = node;
			support->m_inertia = m;
			support->m_damping = c;
			support->m_stiffness = k;
			support->m_dof_type = dof;
			m_supports.push_back(support);
			return support;
		}
		
		Load_Case* Boundary::add_load_case(const char* label)
		{
			Load_Case* load_case = new Load_Case;
			if(label[0] != '\0')
			{
				strcpy(load_case->m_label, label);
			}
			m_load_cases.push_back(load_case);
			return load_case;
		}
		Load_Case* Boundary::add_load_case(unsigned node, mesh::nodes::dof dof, double value, std::function<double(double)> function)
		{
			Load_Case* load_case = new Load_Case;
			m_load_cases.push_back(load_case);
			load_case->add_load_node(node, dof, value, function);
			return load_case;
		}
		Load_Case* Boundary::add_load_case(boundary::loads::type type, std::vector<unsigned> elements, double value, std::function<double(double)> function)
		{
			Load_Case* load_case = new Load_Case;
			m_load_cases.push_back(load_case);
			load_case->add_load_element(type, elements, value, function);
			return load_case;
		}
		
		Load_Case* Boundary::add_self_weight(const char* label, const double* p, double g)
		{
			const mesh::nodes::dof dof[] = {
				mesh::nodes::dof::translation_x,
				mesh::nodes::dof::translation_y, 
				mesh::nodes::dof::translation_z
			};
			const mesh::Mesh* mesh = m_model->mesh();
			Load_Case* load_case = add_load_case(label);
			for(const mesh::joints::Joint* joint : mesh->m_joints)
			{
				const double m = joint->mass();
				const unsigned n = joint->nodes();
				if(m != 0)
				{
					for(unsigned j = 0; j < n; j++)
					{
						for(unsigned k = 0; k < 3; k++)
						{
							if(p[k] != 0)
							{
								load_case->add_load_node(joint->index_node(j), dof[k], -m * g / n * p[k]);
							}
						}
					}
				}
			}
			for(const mesh::elements::Element* element : mesh->m_elements)
			{
				const unsigned n = element->nodes();
				const double r = element->material()->specific_mass();
				const double m = element->cell()->mass(element, r);
				if(m != 0)
				{
					for(unsigned j = 0; j < n; j++)
					{
						for(unsigned k = 0; k < 3; k++)
						{
							if(p[k] != 0)
							{
								load_case->add_load_node(element->index_node(j), dof[k], -m * g / n * p[k]);
							}
						}
					}
				}
			}
			return load_case;
		}
		Load_Case* Boundary::add_self_weight(const char* label, mesh::nodes::dof dof, double g)
		{
			const double s[3][3] = {
				{1, 0, 0},
				{0, 1, 0},
				{0, 0, 1}
			};
			const unsigned p = log2(unsigned(dof));
			return add_self_weight(label, s[p < 3 ? p : 2], g);
		}
		
		Dependency* Boundary::add_dependency(unsigned ns, mesh::nodes::dof ds, unsigned nm, mesh::nodes::dof dm, double a, double b)
		{
			Dependency* dependency = new Dependency;
			dependency->m_slave_dof = ds;
			dependency->m_slave_node = ns;
			dependency->m_masters_dof = { dm };
			dependency->m_masters_node = { nm };
			dependency->m_state = [a, b] (double* d) { return a * d[0] + b; };
			dependency->m_gradient = [a] (double*, unsigned) { return a; };
			dependency->m_hessian = [] (double*, unsigned, unsigned) { return 0; };
			m_dependencies.push_back(dependency);
			return dependency;
		}
		Dependency* Boundary::add_dependency(unsigned ns, mesh::nodes::dof ds, std::vector<unsigned> nm, std::vector<mesh::nodes::dof> dm)
		{
			Dependency* dependency = new Dependency;
			m_dependencies.push_back(dependency);
			dependency->m_slave_dof = ds;
			dependency->m_slave_node = ns;
			dependency->m_masters_dof = dm;
			dependency->m_masters_node = nm;
			return dependency;
		}

		//remove
		void Boundary::remove_initial(unsigned index)
		{
			delete m_initials[index];
			m_initials.erase(m_initials.begin() + index);
		}
		void Boundary::remove_support(unsigned index)
		{
			delete m_supports[index];
			m_supports.erase(m_supports.begin() + index);
		}
		void Boundary::remove_load_case(unsigned index)
		{
			delete m_load_cases[index];
			m_load_cases.erase(m_load_cases.begin() + index);
		}
		void Boundary::remove_dependency(unsigned index)
		{
			delete m_dependencies[index];
			m_dependencies.erase(m_dependencies.begin() + index);
		}

		//analysis
		bool Boundary::check(void) const
		{
			//test
			bool test = true;
			//check initials
			for(const Initial* initial : m_initials)
			{
				test = test && initial->check();
			}
			//check supports
			for(const Support* support : m_supports)
			{
				test = test && support->check();
			}
			//check load cases
			for(const Load_Case* load_case : m_load_cases)
			{
				test = test && load_case->check();
			}
			//check dependencies
			for(const Dependency* dependency : m_dependencies)
			{
				test = test && dependency->check();
			}
			//return
			return test;
		}
		void Boundary::prepare(void) const
		{
			//rotation
			const unsigned r = 
				(unsigned) mesh::nodes::dof::rotation_x |
				(unsigned) mesh::nodes::dof::rotation_y |
				(unsigned) mesh::nodes::dof::rotation_z;
			//initials
			for(Initial* initial : m_initials)
			{
				initial->prepare();
			}
			//supports
			for(Support* support : m_supports)
			{
				support->prepare();
				const unsigned n = support->m_node;
				const unsigned d = (unsigned) support->m_dof;
				const unsigned t = support->node()->dof_types();
				if(d & r && mat::bit_find(t & r, (unsigned) mesh::nodes::dof::last) == 3)
				{
					Support::m_update_nodes.push_back(n);
				}
			}
			Support::m_update_nodes.unique();
			//load cases
			for(Load_Case* load_case : m_load_cases)
			{
				load_case->prepare();
			}
			//dependencies
			for(Dependency* dependency : m_dependencies)
			{
				dependency->prepare();
				const unsigned n = dependency->m_slave_node;
				const unsigned d = (unsigned) dependency->m_slave_dof;
				const unsigned t = dependency->slave_node()->dof_types();
				if(d & r && mat::bit_find(t & r, (unsigned) mesh::nodes::dof::last) == 3)
				{
					Dependency::m_update_nodes.push_back(n);
				}
			}
			Dependency::m_update_nodes.unique();
		}
		void Boundary::apply_dof(void) const
		{
			//initials
			for(const Initial* initial : m_initials)
			{
				initial->add_dof();
			}
			//supports
			for(const Support* support : m_supports)
			{
				support->add_dof();
			}
			//load cases
			for(Load_Case* load_case : m_load_cases)
			{
				for(loads::Node* load : load_case->m_loads_nodes)
				{
					load->add_dof();
				}
			}
			//dependencies
			for(const Dependency* dependency : m_dependencies)
			{
				dependency->add_dof();
			}
		}
		
		void Boundary::mesh_union(void)
		{
			for(mesh::joints::Joint* joint : m_model->mesh()->m_joints)
			{
				joint->configure();
				m_dependencies.insert(m_dependencies.end(), joint->m_dependencies.begin(), joint->m_dependencies.end());
			}
		}
		void Boundary::mesh_split(void)
		{
			for(mesh::joints::Joint* joint : m_model->mesh()->m_joints)
			{
				for(Dependency* dependency : joint->m_dependencies)
				{
					m_dependencies.erase(std::find(m_dependencies.begin(), m_dependencies.end(), dependency));
				}
			}
		}
		
		void Boundary::record(void) const
		{
			for(Support* support : m_supports)
			{
				support->record();
			}
		}
		void Boundary::finish(void) const
		{
			for(const Support* support : m_supports)
			{
				support->finish();
			}
		}

		//results
		void Boundary::plot(double s, const double** p) const
		{
			//supports
			if(m_model->plot()->what()->supports())
			{
				for(const Support* support : m_supports)
				{
					support->plot(m_model->plot()->sizes()->supports() * s, m_model->plot()->colors()->supports(), p);
				}		
			}
			//loads
			if(m_model->plot()->what()->loads() && !m_load_cases.empty())
			{
				m_load_cases[m_model->analysis()->solver()->load_case()]->plot(m_model->plot()->sizes()->loads() * s, m_model->plot()->colors()->loads(), p);
			}
		}

		//static attributes
		models::Model* Boundary::m_model = nullptr;
	}
}
