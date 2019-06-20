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
			
				double zoom(void) const;
				double zoom(double, bool = false);
				const double* pan(void) const;
				const double* pan(const double*, bool = false);
				const double* rotation(void) const;
				const double* rotation(const double*, bool = false);
			
				double x_min(void) const;
				double y_min(void) const;
				double z_min(void) const;
				double x_max(void) const;
				double y_max(void) const;
				double z_max(void) const;

				//center
				double* center(double*) const;
				
				//limits
				void apply(void);
				void reset(void);
			
				void update(fea::models::Model*);
				void update(const double**, unsigned);
				void update(const double***, unsigned, unsigned);

				//rotation
				static double* rotation(double*, const double*, double);

			private:
				//attributes			
				double m_zoom;
				double m_pan[3];
				double m_rotation[16];
			
				double m_x_min;
				double m_y_min;
				double m_z_min;
				double m_x_max;
				double m_y_max;
				double m_z_max;
		};
	}
}
