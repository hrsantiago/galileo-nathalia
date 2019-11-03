#pragma once

namespace gui
{
	namespace results
	{
		class Path;
		class Canvas;
	}
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
	namespace results
	{
		class Results
		{
		public:
			//constructors
			Results(const fea::models::Model*);
		
			//destructor
			~Results(void);
			
			//data
			bool status(void) const;
			const Path* path(void) const;
			const Canvas* canvas(void) const;
			
			//model
			void model(const fea::models::Model*);
			
			//data
			bool read(void);
			void clear(void);

		private:
			//attributes
			Path* m_path;
			Canvas* m_canvas;
			
			bool m_status;
		};
	}
}
