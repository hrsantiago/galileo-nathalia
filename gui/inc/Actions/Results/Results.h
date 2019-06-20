#pragma once

namespace gui
{
	class Galileo;
}
namespace fea
{
	namespace models
	{
		class Model;
	}
}

namespace gui
{
	class Results
	{
	public:
		//constructors
		Results(fea::models::Model*);
	
		//destructor
		~Results(void);
		
		//data
		unsigned steps(void) const;
			
		const double* step(void) const;
		const double* time(void) const;
		const double* load(void) const;
		
		const double*** state(void) const;
		const double*** position(void) const;
		const double*** velocity(void) const;
		const double*** acceleration(void) const;
		
		const double**** joint(void) const;
		const double**** element(void) const;
		
		const double** support(void) const;
		
		//friends
		friend class Galileo;

	protected:
		//read
		virtual void read_step(void);
		
		virtual void read_state(void);
		virtual void read_velocity(void);
		virtual void read_acceleration(void);
		
		virtual void read_joint(void);
		virtual void read_element(void);
		virtual void read_support(void);
		
		//delete
		virtual void delete_step(void);
		virtual void delete_time(void);
		virtual void delete_load(void);
		
		virtual void delete_state(void);
		virtual void delete_position(void);
		virtual void delete_velocity(void);
		virtual void delete_acceleration(void);
		
		virtual void delete_joint(void);
		virtual void delete_element(void);
		virtual void delete_support(void);

	private:
		//attributes
		unsigned m_steps;
			
		double* m_step;
		double* m_time;
		double* m_load;
		
		double*** m_state;
		double*** m_position;
		double*** m_velocity;
		double*** m_acceleration;
		
		double**** m_joint;
		double**** m_element;
		
		double** m_support;
		
		fea::models::Model* m_model;
	};
}
