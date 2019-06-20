#pragma once

#include <cstdio>

namespace fea
{
	//declarations
	namespace mesh
	{
		class Mesh;
		namespace materials
		{
			enum class type : unsigned;
		}
	}

	namespace mesh
	{
		namespace materials
		{
			class Material
			{
			public:
				//friends
				friend class mesh::Mesh;

			protected:
				//constructors
				Material(void);

				//destructor
				virtual ~Material(void);

				//serialization
				virtual void load(FILE*);
				virtual void save(FILE*) const;

				//create
				static Material* create(Material*&, const Material*);
				static Material* create(Material*&, materials::type, const Material* = nullptr);

			public:
				//data
				virtual const char* label(void) const;
				virtual const char* label(const char*);
				
				virtual double specific_mass(void) const;
				virtual double specific_mass(double);

				//type
				virtual materials::type type(void) const = 0;
				
				//type
				virtual const char* name(void) const;
				static const char* name(materials::type);

				//index
				unsigned index(void) const;

				//analysis
				virtual bool check(void) const;

			protected:
				//attributes
				static Mesh* m_mesh;

				char m_label[200];

				double m_specific_mass;
			};
		}
	}
}
