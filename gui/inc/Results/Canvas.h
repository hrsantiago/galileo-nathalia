#pragma once

namespace fea
{
	namespace models
	{
		class Model;
	}
}
namespace gui
{
	namespace limit
	{
		class Dof;
		class State;
	}
	namespace results
	{
		class Path;
	}
}

namespace gui
{
	namespace results
	{
		class Canvas
		{
		public:
			//constructors
			Canvas(const fea::models::Model*, const Path*);
			
			//destructor
			~Canvas(void);
			
			//data
			unsigned dofs(void) const;
			unsigned steps(void) const;
			unsigned states(void) const;
			
			const double**** node(void) const;
			const double**** element(void) const;
			const double*** position(void) const;
			
			const limit::State& limit_state(unsigned) const;
			const limit::Dof& limit_dof(unsigned, unsigned) const;
			
			void model(const fea::models::Model*);
			
			//data
			void read(void);
			void clear(void);
			
		protected:
			//setup
			void dofs(void);
			void states(void);
			
			//read
			void read_node(void);
			void read_element(void);
			void read_position(void);
			
			//clear
			void clear_node(void);
			void clear_element(void);
			void clear_position(void);
			
			//scale
			void scale_node(void);
			void scale_element(void);
			
			//allocate
			void allocate_node(void);
			void allocate_element(void);
			void allocate_position(void);
			
		private:
			//attributes
			unsigned m_dofs;
			unsigned m_steps;
			unsigned m_states;
			
			double*** m_node[3];
			double*** m_position;
			double**** m_element;
			
			limit::Dof* m_limit_dof[3];
			limit::State* m_limit_state;
			
			const Path* m_path;
			const fea::models::Model* m_model;
		};
	}
}
