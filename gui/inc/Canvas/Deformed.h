#pragma once

#include "Canvas/Model.h"

namespace gui
{
	//forward declarations
	namespace results
	{
		class Deformed;
	}
	
	namespace canvas
	{
		class Deformed : public Model
		{
			Q_OBJECT
		
			//friends
			friend class results::Deformed;
		
			public:
				//construtors
				Deformed(QWidget* = nullptr);
		
				//destructor
				virtual ~Deformed(void);

			protected:
				//draw
				virtual void paintGL(void) override;
				
				//data
				virtual void allocate(void);
				virtual void get_data(void);
				
				//update
				virtual void update_model(fea::models::Model*) override;
				
				//events
				virtual void mousePressEvent(QMouseEvent*) override;
			
			protected slots:
				//slots
				virtual void slot_center(void);
			
			private:
				//attributes
				unsigned m_steps;
				
				double* m_dof;
				double* m_parameter;
				
				double*** m_positions;
				
				std::vector<unsigned> m_paths;
				std::vector<unsigned> m_plots;
		};
	}
}
