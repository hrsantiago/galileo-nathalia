//std
#include <cstring>
#include <algorithm>

//fea
#include "Mesh/Mesh.h"
#include "Mesh/Materials/Materials.h"

namespace fea
{
	namespace mesh
	{
		namespace materials
		{
			//constructors
			Material::Material(void) : m_label("Material"), m_specific_mass(1e3)
			{
				return;
			}

			//destructor
			Material::~Material(void)
			{
				return;
			}

			//serialization
			void Material::load(FILE* file)
			{
				fscanf(file, "%s", m_label);
			}
			void Material::save(FILE* file) const
			{
				fprintf(file, "%s ", m_label);
			}

			//create
			Material* Material::create(Material*& material, const Material* base)
			{
				return Material::create(material, base->type(), base);
			}
			Material* Material::create(Material*& material, materials::type type, const Material* base)
			{
				//delete
				delete material;
				//create
				switch(type)
				{
					case materials::type::heat:
						return material = base ? new Heat(*(Heat*) base) : new Heat;
					case materials::type::steel:
						return material = base ? new Steel(*(Steel*) base) : new Steel;
					case materials::type::concrete:
						return material = base ? new Concrete(*(Concrete*) base) : new Concrete;
					default:
						return material = nullptr;
				}
			}

			//data
			Mesh* Material::mesh(void)
			{
				return m_mesh;
			}
			
			const char* Material::label(void) const
			{
				return m_label;
			}
			const char* Material::label(const char* label)
			{
				return (const char*) strcpy(m_label, label);
			}
			
			double Material::specific_mass(void) const
			{
				return m_specific_mass;
			}
			double Material::specific_mass(double specific_mass)
			{
				return m_specific_mass = specific_mass;
			}
			
			//name
			const char* Material::name(void) const
			{
				return name(type());
			}
			const char* Material::name(materials::type type)
			{
				switch(type)
				{
					case materials::type::heat:
						return "Heat";
					case materials::type::steel:
						return "Steel";
					case materials::type::concrete:
						return "Concrete";
					default:
						return "";
				}
			}

			//index
			unsigned Material::index(void) const
			{
				return distance(m_mesh->m_materials.begin(), find(m_mesh->m_materials.begin(), m_mesh->m_materials.end(), this));
			}

			//analysis
			bool Material::check(void) const
			{
				if(m_specific_mass < 0)
				{
					printf("\tMaterial %02d must have non negative specific mass!\n", index());
					return false;
				}
				else
				{
					return true;
				}
			}

			//static attributes
			Mesh* Material::m_mesh = nullptr;
		}
	}
}
