#pragma once

#include "Mesh/Materials/Material.h"

namespace fea
{
	namespace mesh
	{
		namespace materials
		{
			class Heat : public Material
			{
			public:
				//friends
				friend class Material;

			protected:
				//constructors
				Heat(void);

				//destructor
				virtual ~Heat(void) override;

				//serialization
				virtual void load(FILE*) override;
				virtual void save(FILE*) const override;

			public:
				//type
				virtual materials::type type(void) const override;
				
				//data
				double capacity(void) const;
				double capacity(double);
				double condutivity(void) const;
				double condutivity(double);

			protected:
				//attributes
				double m_capacity;
				double m_condutivity;
			};
		}
	}
}
