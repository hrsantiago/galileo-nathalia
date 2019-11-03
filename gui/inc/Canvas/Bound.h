namespace fea
{
	namespace models
	{
		class Model;
	}
}

namespace gui
{
	namespace canvas
	{
		class Bound
		{
			public:
				//constructors
				Bound(void);
		
				//destructor
				virtual ~Bound(void);

				//data
				double size(void) const;
				double x_min(void) const;
				double y_min(void) const;
				double z_min(void) const;
				double x_max(void) const;
				double y_max(void) const;
				double z_max(void) const;
				
				//transform
				void apply(double) const;

				//update
				void update(const fea::models::Model*);
				void update(const double***, unsigned, unsigned);

			private:
				//attributes			
				double m_x_min;
				double m_y_min;
				double m_z_min;
				double m_x_max;
				double m_y_max;
				double m_z_max;
		};
	}
}
