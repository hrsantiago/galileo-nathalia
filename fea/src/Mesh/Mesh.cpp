//std
#include <cmath>

//gl
#include <GL/gl.h>

//mat
#include "misc/util.h"

//fea
#include "Models/Model.h"

#include "Plot/Plot.h"
#include "Plot/What.h"
#include "Plot/Sizes.h"
#include "Plot/Colors.h"
#include "Plot/Gradient.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Node.h"
#include "Mesh/Cells/Cell.h"
#include "Mesh/Points/Point.h"
#include "Mesh/Joints/Joint.h"
#include "Mesh/Sections/Section.h"
#include "Mesh/Elements/Element.h"
#include "Mesh/Materials/Material.h"

#include "Analysis/Analysis.h"
#include "Analysis/Assembler/Assembler.h"

namespace fea
{
	namespace mesh
	{
		//constructors
		Mesh::Mesh(void)
		{
			nodes::Node::m_mesh = this;
			cells::Cell::m_mesh = this;
			points::Point::m_mesh = this;
			joints::Joint::m_mesh = this;
			sections::Section::m_mesh = this;
			elements::Element::m_mesh = this;
			materials::Material::m_mesh = this;
		}

		//destructor
		Mesh::~Mesh(void)
		{
			//delete nodes
			for(const nodes::Node* node : m_nodes)
			{
				delete node;
			}
			//delete cells
			for(const cells::Cell* cell : m_cells)
			{
				delete cell;
			}
			//delete joints
			for(const joints::Joint* joint : m_joints)
			{
				delete joint;
			}
			//delete sections
			for(const sections::Section* section : m_sections)
			{
				delete section;
			}
			//delete elements
			for(const elements::Element* element : m_elements)
			{
				delete element;
			}
			//delete materials
			for(const materials::Material* material : m_materials)
			{
				delete material;
			}
		}

		//serialization
		void Mesh::load(FILE* file)
		{
			load_nodes(file);
			load_cells(file);
			load_joints(file);
			load_sections(file);
			load_elements(file);
			load_materials(file);
		}
		void Mesh::load_nodes(FILE* file)
		{
			unsigned size;
			fscanf(file, "%d", &size);
			for(unsigned i = 0; i < size; i++)
			{
				add_node(0, 0, 0);
				m_nodes[i]->load(file);
			}
		}
		void Mesh::load_cells(FILE* file)
		{
			unsigned size;
			mesh::cells::type type;
			fscanf(file, "%d", &size);
			for(unsigned i = 0; i < size; i++)
			{
				fscanf(file, "%d", &type);
				add_cell(type);
				m_cells[i]->load(file);
			}
		}
		void Mesh::load_joints(FILE* file)
		{
			unsigned nc;
			mesh::joints::type type;
			fscanf(file, "%d", &nc);
			for(unsigned i = 0; i < nc; i++)
			{
				fscanf(file, "%d", &type);
				add_joint(type);
				m_joints[i]->load(file);
			}
		}
		void Mesh::load_sections(FILE* file)
		{
			unsigned size;
			mesh::sections::type type;
			fscanf(file, "%d", &size);
			for(unsigned i = 0; i < size; i++)
			{
				fscanf(file, "%d", &type);
				add_section(type);
				m_sections[i]->load(file);
			}
		}
		void Mesh::load_elements(FILE* file)
		{
			unsigned size;
			mesh::elements::type type;
			fscanf(file, "%d", &size);
			for(unsigned i = 0; i < size; i++)
			{
				fscanf(file, "%d", &type);
				add_element(type);
				m_elements[i]->load(file);
			}
		}
		void Mesh::load_materials(FILE* file)
		{
			unsigned size;
			mesh::materials::type type;
			fscanf(file, "%d", &size);
			for(unsigned i = 0; i < size; i++)
			{
				fscanf(file, "%d", &type);
				add_material(type);
				m_materials[i]->load(file);
			}
		}
		
		void Mesh::save(FILE* file) const
		{
			save_nodes(file);
			save_cells(file);
			save_joints(file);
			save_sections(file);
			save_elements(file);
			save_materials(file);
		}
		void Mesh::save_nodes(FILE* file) const
		{
			fprintf(file, "%04d\n", (unsigned) m_nodes.size());
			for(const nodes::Node* node : m_nodes)
			{
				node->save(file);
				fprintf(file, "\n");
			}
		}
		void Mesh::save_cells(FILE* file) const
		{
			fprintf(file, "%02d\n", (unsigned) m_cells.size());
			for(const cells::Cell* cell : m_cells)
			{
				fprintf(file, "%04d ", cell->type());
				cell->save(file);
				fprintf(file, "\n");
			}
		}
		void Mesh::save_joints(FILE* file) const
		{
			fprintf(file, "%04d\n", (unsigned) m_joints.size());
			for(const joints::Joint* joint : m_joints)
			{
				fprintf(file, "%04d ", joint->type());
				joint->save(file);
				fprintf(file, "\n");
			}
		}
		void Mesh::save_sections(FILE* file) const
		{
			fprintf(file, "%02d\n", (unsigned) m_sections.size());
			for(const sections::Section* section : m_sections)
			{
				fprintf(file, "%04d ", section->type());
				section->save(file);
				fprintf(file, "\n");
			}
		}
		void Mesh::save_elements(FILE* file) const
		{
			fprintf(file, "%04d\n", (unsigned) m_elements.size());
			for(const elements::Element* element : m_elements)
			{
				fprintf(file, "%04d ", element->type());
				element->save(file);
				fprintf(file, "\n");
			}
		}
		void Mesh::save_materials(FILE* file) const
		{
			fprintf(file, "%02d\n", (unsigned) m_materials.size());
			for(const materials::Material* material : m_materials)
			{
				fprintf(file, "%04d ", material->type());
				material->save(file);
				fprintf(file, "\n");
			}
		}

		//model
		models::Model* Mesh::model(void) const
		{
			return m_model;
		}

		//data
		nodes::Node* Mesh::node(unsigned index) const
		{
			return index < m_nodes.size() ? m_nodes[index] : nullptr;
		}
		cells::Cell* Mesh::cell(unsigned index) const
		{
			return index < m_cells.size() ? m_cells[index] : nullptr;
		}
		joints::Joint* Mesh::joint(unsigned index) const
		{
			return index < m_joints.size() ? m_joints[index] : nullptr;
		}
		sections::Section* Mesh::section(unsigned index) const
		{
			return index < m_sections.size() ? m_sections[index] : nullptr;
		}
		elements::Element* Mesh::element(unsigned index) const
		{
			return index < m_elements.size() ? m_elements[index] : nullptr;
		}
		materials::Material* Mesh::material(unsigned index) const
		{
			return index < m_materials.size() ? m_materials[index] : nullptr;
		}
		
		//bound
		double Mesh::size(void) const
		{
			const double xj = limit(0, true);
			const double yj = limit(1, true);
			const double zj = limit(2, true);
			const double xi = limit(0, false);
			const double yi = limit(1, false);
			const double zi = limit(2, false);
			return sqrt(pow(xj - xi, 2) + pow(yj - yi, 2) + pow(yj - yi, 2));
		}
		double Mesh::limit(unsigned d, bool t) const
		{
			if(m_nodes.empty())
			{
				return t ? +1 : -1;
			}
			else
			{
				double x, s = m_nodes[0]->m_coordinates[d];
				for(unsigned i = 1; i < m_nodes.size(); i++)
				{
					x = m_nodes[i]->m_coordinates[d];
					s = t ? std::max(s, x) : std::min(s, x);
				}
				return s;
			}
		}

		//sizes
		unsigned Mesh::nodes(void) const
		{
			return (unsigned) m_nodes.size();
		}
		unsigned Mesh::cells(void) const
		{
			return (unsigned) m_cells.size();
		}
		unsigned Mesh::joints(void) const
		{
			return (unsigned) m_joints.size();
		}
		unsigned Mesh::materials(void) const
		{
			return (unsigned) m_materials.size();
		}
		unsigned Mesh::sections(void) const
		{
			return (unsigned) m_sections.size();
		}
		unsigned Mesh::elements(void) const
		{
			return (unsigned) m_elements.size();
		}

		//add
		nodes::Node* Mesh::add_node(const double* p)
		{
			nodes::Node* node = new nodes::Node;
			node->coordinates(p);
			m_nodes.push_back(node);
			return node;
		}
		nodes::Node* Mesh::add_node(double x, double y, double z)
		{
			nodes::Node* node = new nodes::Node;
			node->coordinates(x, y, z);
			m_nodes.push_back(node);
			return node;
		}
		
		cells::Cell* Mesh::add_cell(cells::type type)
		{
			cells::Cell* cell = nullptr;
			cells::Cell::create(cell, type);
			m_cells.push_back(cell);
			return cell;
		}
		
		joints::Joint* Mesh::add_joint(joints::type type, std::vector<unsigned> nodes)
		{
			joints::Joint* joint = nullptr;
			joints::Joint::create(joint, type);
			joint->m_nodes = nodes;
			m_joints.push_back(joint);
			return joint;
		}
		
		sections::Section* Mesh::add_section(sections::type type)
		{
			sections::Section* section = nullptr;
			sections::Section::create(section, type);
			m_sections.push_back(section);
			return section;
		}
		
		elements::Element* Mesh::add_element(const elements::Element* base)
		{
			elements::Element* element = nullptr;
			elements::Element::create(element, base);
			m_elements.push_back(element);
			return element;
		}
		
		elements::Element* Mesh::add_element(elements::type type, std::vector<unsigned> nodes, unsigned material, unsigned cell)
		{
			elements::Element* element = nullptr;
			elements::Element::create(element, type);
			element->m_cell = cell;
			element->m_nodes = nodes;
			element->m_material = material;
			m_elements.push_back(element);
			return element;
		}
		
		materials::Material* Mesh::add_material(materials::type type)
		{
			materials::Material* material = nullptr;
			materials::Material::create(material, type);
			m_materials.push_back(material);
			return material;
		}

		//remove
		void Mesh::remove_node(unsigned index)
		{
			delete m_nodes[index];
			m_nodes.erase(m_nodes.begin() + index);
		}
		void Mesh::remove_cell(unsigned index)
		{
			delete m_cells[index];
			m_cells.erase(m_cells.begin() + index);
		}
		void Mesh::remove_joint(unsigned index)
		{
			delete m_joints[index];
			m_joints.erase(m_joints.begin() + index);
		}
		void Mesh::remove_section(unsigned index)
		{
			delete m_sections[index];
			m_sections.erase(m_sections.begin() + index);
		}
		void Mesh::remove_element(unsigned index)
		{
			delete m_elements[index];
			m_elements.erase(m_elements.begin() + index);
		}
		void Mesh::remove_material(unsigned index)
		{
			delete m_materials[index];
			m_materials.erase(m_materials.begin() + index);
		}

		//analysis
		bool Mesh::check(void) const
		{
			//test
			bool test = true;
			//nodes
			for(const nodes::Node* node : m_nodes)
			{
				test = test && node->check();
			}
			//cells
			for(const cells::Cell* cell : m_cells)
			{
				test = test && cell->check();
			}
			//joints
			for(joints::Joint* joint : m_joints)
			{
				test = test && joint->check();
			}
			//materials
			for(const materials::Material* material : m_materials)
			{
				test = test && material->check();
			}
			//sections
			for(const sections::Section* section : m_sections)
			{
				test = test && section->check();
			}
			//elements
			for(const elements::Element* element : m_elements)
			{
				test = test && element->check();
			}
			//return
			return test;
		}
		void Mesh::prepare(void) const
		{
			//assembler
			analysis::Assembler* assembler = m_model->analysis()->assembler();
			unsigned& dt = assembler->m_dof_total = 0;
			//nodes
			assembler->apply_dof();
			for(nodes::Node* node : m_nodes)
			{
				node->prepare(dt);
			}
			assembler->sort_dof();
			assembler->save_dof();
			//joints
			for(joints::Joint* joint : m_joints)
			{
				joint->prepare();
			}
			//elements
			for(elements::Element* element : m_elements)
			{
				element->prepare();
			}
		}
		void Mesh::finish(void) const
		{
			//nodes
			for(const nodes::Node* node : m_nodes)
			{
				node->finish();
			}
			//joints
			for(const joints::Joint* joint : m_joints)
			{
				joint->finish();
			}
			//elements
			for(const elements::Element* element : m_elements)
			{
				element->finish();
			}
		}
		void Mesh::record(void) const
		{
			//nodes
			for(nodes::Node* node : m_nodes)
			{
				node->record();
			}
			//joints
			for(joints::Joint* joint : m_joints)
			{
				joint->record();
			}
			//elements
			for(elements::Element* element : m_elements)
			{
				element->record();
			}
		}
		void Mesh::update(void) const
		{
			//nodes
			for(nodes::Node* node : m_nodes)
			{
				node->update();
			}
			//joints
			for(joints::Joint* joint : m_joints)
			{
				joint->update();
			}
			//elements
			for(elements::Element* element : m_elements)
			{
				element->update();
			}
		}
		void Mesh::restore(void) const
		{
			//nodes
			for(nodes::Node* node : m_nodes)
			{
				node->restore();
			}
			//joints
			for(joints::Joint* joint : m_joints)
			{
				joint->restore();
			}
			//elements
			for(elements::Element* element : m_elements)
			{
				element->restore();
			}
		}
		void Mesh::apply_dof(void) const
		{
			//joints
			for(const joints::Joint* joint : m_joints)
			{
				joint->add_dof();
			}
			//elements
			for(const elements::Element* element : m_elements)
			{
				element->add_dof();
			}
		}

		//results
		void Mesh::plot(double s, const double** p, const double* vn, const double** ve) const
		{
			//colors
			const double* cn = m_model->plot()->colors()->nodes();
			const double* cj = m_model->plot()->colors()->joints();
			const double* ce = m_model->plot()->colors()->elements();
			//gradient
			const double* g[] = {
				m_model->plot()->gradient()->color_min(),
				m_model->plot()->gradient()->color_mean(),
				m_model->plot()->gradient()->color_max()
			};
			//elements
			if(m_model->plot()->what()->elements())
			{
				for(unsigned i = 0; i < m_elements.size(); i++)
				{
					m_cells[m_elements[i]->m_cell]->plot(m_elements[i], ce, p, vn ? vn : ve ? ve[i] : nullptr, !vn);
				}
			}
			//joint
			if(m_model->plot()->what()->joints())
			{
				for(const joints::Joint* joint : m_joints)
				{
					joint->plot(m_model->plot()->sizes()->joints() * s, cj, p);
				}
			}
			//nodes
			if(m_model->plot()->what()->nodes())
			{
				double c[4];
				for(unsigned i = 0; i < m_nodes.size(); i++)
				{
					m_nodes[i]->plot(vn ? mat::map_color(c, g, vn[i]) : cn, p ? p[i] : nullptr);
				}
			}
		}

		//static attributes
		models::Model* Mesh::m_model = nullptr;
	}
}
