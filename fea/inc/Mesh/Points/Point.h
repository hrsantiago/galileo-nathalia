#pragma once

namespace fea
{
	namespace mesh
	{
		//declarations
		class Mesh;
		namespace points
		{
			enum class type : unsigned;
		}
		namespace elements
		{
			class Element;
		}
		
		//class
		namespace points
		{
			class Point
			{
			public:
				//friends
				friend class Mesh;
				friend class elements::Element;

				//constructors
				Point(void);

				//destructor
				virtual ~Point(void);

				//create
				static void create(Point*&, const Point*);
				static void create(Point*&, points::type, const Point* = nullptr);

				//type
				virtual points::type type(void) const = 0;

				//analysis
				virtual void prepare(const elements::Element*) = 0;
				
				virtual void update(void) = 0;
				virtual void restore(void) = 0;

				//attributes
				static Mesh* m_mesh;
			};
		}
	}
}
