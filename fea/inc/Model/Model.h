#pragma once

#include <string>

namespace fea
{
	namespace plot
	{
		class Plot;
	}
	namespace mesh
	{
		class Mesh;
	}
	namespace boundary
	{
		class Boundary;
	}
	namespace topology
	{
		class Topology;
	}
	namespace analysis
	{
		class Analysis;
		class Assembler;
	}
	
	namespace models
	{
		class Model
		{
		public:
			//friends
			friend class analysis::Analysis;
			friend class analysis::Assembler;

			//constructors
			Model(std::string = "Model", std::string = "", plot::Plot* = nullptr);

			//destructor
			virtual ~Model(void);

			//serialization
			bool load(std::string);
			bool save(std::string = "");

			//data
			void mark(void);
			bool saved(void) const;
			
			std::string path(void) const;
			std::string name(void) const;
			std::string path(std::string);
			std::string name(std::string);
			std::string folder(void) const;
			
			plot::Plot* plot(void) const;
			mesh::Mesh* mesh(void) const;
			plot::Plot* plot(plot::Plot*);
			boundary::Boundary* boundary(void) const;
			topology::Topology* topology(void) const;
			analysis::Analysis* analysis(void) const;

			//analysis
			void modified(void);

			//results
			void plot(double, const double** = nullptr, const double* = nullptr, const double** = nullptr) const;

		protected:
			//header
			void header(void) const;
			void quiter(void) const;
			
			//attributes
			bool m_saved;
			
			std::string m_path;
			std::string m_name;

			plot::Plot* m_plot;
			mesh::Mesh* m_mesh;
			boundary::Boundary* m_boundary;
			topology::Topology* m_topology;
			analysis::Analysis* m_analysis;
		};
	}
}
