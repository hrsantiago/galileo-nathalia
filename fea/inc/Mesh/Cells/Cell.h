#pragma once

#include <vector>
#include <string>

namespace fea
{
	//declarations
	namespace mesh
	{
		class Mesh;
		namespace cells
		{
			enum class type : unsigned;
		}
		namespace elements
		{
			class Element;
		}
	}

	namespace mesh
	{
		namespace cells
		{
			class Cell
			{
			public:
				//friends
				friend class mesh::Mesh;

			protected:
				//constructors
				Cell(void);

				//destructor
				virtual ~Cell(void);

				//serialization
				virtual void load(FILE*) = 0;
				virtual void save(FILE*) const = 0;

				//create
				static void create(Cell*&, const Cell*);
				static void create(Cell*&, cells::type, const Cell* = nullptr);

			public:
				//type
				virtual cells::type type(void) const = 0;

				//name
				static const char* name(cells::type);
				virtual const char* name(void) const;

				//data
				unsigned rule(void) const;
				unsigned rule(unsigned);
				
				static unsigned max_rule(void);
				static unsigned max_nodes(void);
				
				virtual const char* label(void) const;
				virtual const char* label(const char*);

				//topology
				virtual unsigned faces(void) const = 0;
				virtual unsigned edges(void) const = 0;
				virtual unsigned vertices(void) const = 0;
				virtual unsigned dimension(void) const = 0;

				virtual std::vector<unsigned> edge(unsigned) const = 0;
				virtual std::vector<unsigned> face(unsigned) const = 0;

				//parametrization
				virtual void edge(double*, unsigned, double) const = 0;
				virtual void face(double*, unsigned, double, double) const = 0;
				
				virtual void gradient(double*, unsigned, double) const = 0;
				virtual void gradient(double*, unsigned, double, double) const = 0;
				
				//geometry
				virtual double mass(const elements::Element*, double) const;
				virtual double edge(const elements::Element*, unsigned) const;
				virtual double face(const elements::Element*, unsigned) const;
				virtual double jacobian(double*, const elements::Element*, const double*) const;

				//interpolation
				virtual double* function(double*, const double*) const = 0;
				virtual double* gradient(double*, const double*) const = 0;

				//integration
				virtual unsigned points(void) const = 0;
				virtual double point(double*, unsigned) const = 0;

			protected:
				//analysis
				virtual bool check(void) const;
				
				//plot
				virtual void plot(const elements::Element*, const double*, const double** = nullptr, const double* = nullptr, bool = false) const = 0;
				
				//attributes
				unsigned m_rule;
				char m_label[200];
				static Mesh* m_mesh;
				static const unsigned m_max_rule = 9;
				static const unsigned m_max_nodes = 27;
			};
		}
	}
}
