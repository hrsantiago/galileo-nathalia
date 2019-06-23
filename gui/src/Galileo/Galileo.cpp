//qt
#include <QKeyEvent>
#include <QFileDialog>
#include <QMessageBox>

//boost
#include <boost/filesystem.hpp>

//fea
#include "Models/Model.h"

#include "Plot/Plot.h"

#include "Mesh/Mesh.h"
#include "Mesh/Nodes/Node.h"
#include "Mesh/Nodes/Dofs.h"
#include "Mesh/Joints/Joint.h"
#include "Mesh/Joints/States.h"
#include "Mesh/Elements/States.h"
#include "Mesh/Elements/Element.h"

#include "Boundary/Boundary.h"
#include "Boundary/Supports/Support.h"

#include "Analysis/Analysis.h"

//gui
#include "Galileo/Galileo.h"

#include "Actions/Model/Model.h"

#include "Actions/Mesh/Nodes/Nodes.h"
#include "Actions/Mesh/Cells/Cells.h"
#include "Actions/Mesh/Sections/Sections.h"
#include "Actions/Mesh/Materials/Materials.h"

#include "Actions/Results/Nodal.h"
#include "Actions/Results/Elements.h"
#include "Actions/Results/Deformed.h"
#include "Actions/Results/Equilibrium_Path.h"

#include "Actions/Help/About.h"
#include "Actions/Help/Index.h"

//ui
#include "ui_Galileo.h"

namespace gui
{
	//constructors
	Galileo::Galileo(QWidget* parent) : QMainWindow(parent), m_ui(new Ui::Galileo), m_plot(new fea::plot::Plot), m_model(new fea::models::Model)
	{
		//ui
		m_ui->setupUi(this);
		m_ui->canvas->update_model(m_model);
		m_ui->menubar->setNativeMenuBar(false);
		//plot
		plot(true);
		//recent
		recent(nullptr);
		//maximize
		showMaximized();
	}

	//destructor
	Galileo::~Galileo(void)
	{
		//save plot
		plot(false);
		//delete
		delete m_ui;
		delete m_plot;
		delete m_model;
	}
		
	//slots
	bool Galileo::slot_saved(void)
	{
		if(!m_model->saved())
		{
			const QString question = "Would you like to save the current model first?";
			const QMessageBox::StandardButtons buttons = QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel;
			const QMessageBox::StandardButton reply = QMessageBox::warning(nullptr, "Save Model", question, buttons);
			if(reply == QMessageBox::StandardButton::Yes)
			{
				on_action_model_save_triggered();
			}
			else if(reply == QMessageBox::StandardButton::Cancel)
			{
				return false;
			}
		}
		return true;
	}
	void Galileo::slot_recent(void)
	{
		//check
		if(!slot_saved())
		{
			return;
		}
		//load model
		delete m_model;
		m_model = new fea::models::Model();
		const std::string path = ((QAction*) QObject::sender())->text().toStdString();
		if(!m_model->load(path))
		{
			QMessageBox::critical(nullptr, "Error", "Unable to load model!", QMessageBox::Ok);
		}
		//update
		recent(path.c_str());
		m_model->plot(m_plot);
		m_ui->canvas->m_model = m_model;
		m_ui->canvas->m_bound.update(m_model);
		setWindowTitle("Galileo - " + QString(m_model->name().c_str()));
	}
	
	void Galileo::on_action_model_new_triggered(void)
	{
		//check
		if(!slot_saved())
		{
			return;
		}
		//create
		delete m_model;
		m_model = new fea::models::Model();
		//update
		m_model->plot(m_plot);
		m_ui->canvas->m_model = m_model;
		m_ui->canvas->m_bound.update(m_model);
	}
	void Galileo::on_action_model_save_triggered(void)
	{
		//check saved
		if(m_model->saved())
		{
			return;
		}
		//create dialog
		std::string folder_path = QFileDialog::getExistingDirectory(nullptr, "Save Model", QDir::currentPath()).toStdString();
		if(folder_path.empty())
		{
			return;
		}
		//save dialog
		m_model->path(folder_path);
		if(!m_model->save())
		{
			QMessageBox::critical(nullptr, "Error", "Unable to save model!", QMessageBox::Ok);
		}
	}
	void Galileo::on_action_model_open_triggered(void)
	{
		//check
		if(!slot_saved())
		{
			return;
		}
		//get model
		char path[200];
		current(path, true);
		const unsigned options = 
			(unsigned) QFileDialog::ReadOnly |
			(unsigned) QFileDialog::ShowDirsOnly |
			(unsigned) QFileDialog::DontUseNativeDialog | 
			(unsigned) QFileDialog::DontResolveSymlinks;
		strcpy(path, QFileDialog::getExistingDirectory(nullptr, "Open model", path, QFileDialog::Options(options)).toLocal8Bit().data());
		if(path[0] == '\0')
		{
			return;
		}
		//load model
		delete m_model;
		m_model = new fea::models::Model();
		m_model->plot(m_plot);
		if(!m_model->load(path))
		{
			QMessageBox::critical(nullptr, "Error", "Unable to load model!", QMessageBox::Ok);
		}
		//update
		recent(path);
		current(path, false);
		m_ui->canvas->m_model = m_model;
		m_ui->canvas->m_bound.update(m_model);
		setWindowTitle("Galileo - " + QString(m_model->name().c_str()));
	}
	void Galileo::on_action_model_plot_triggered(void)
	{
		//create dialog
		model::Model* dia = new model::Model(m_model, nullptr);
        dia->exec();
        //update title
		setWindowTitle("Galileo - " + QString(m_model->name().c_str()));
		//delete dialog
        delete dia;
	}
	void Galileo::on_action_model_print_triggered(void)
	{
		//data
		const QString path = m_model->path().c_str();
		const QString name = m_model->name().c_str();
		const QString file = path + "/" + name + "/Model.png";
		//print
		m_ui->canvas->grabFramebuffer().save(file);
		QMessageBox::information(nullptr, "Model", "Model image saved!", QMessageBox::Ok);
	}
	void Galileo::on_action_model_close_triggered(void)
	{
		//check
		if(!slot_saved())
		{
			return;
		}
		//close
		QApplication::quit();
	}
	
	void Galileo::on_action_mesh_nodes_triggered(void)
	{
		mesh::nodes::Nodes(m_model->mesh(), m_ui->canvas, nullptr).exec();
	}
	void Galileo::on_action_mesh_cells_triggered(void)
	{
		mesh::cells::Cells(m_model->mesh(), nullptr).exec();
	}
	void Galileo::on_action_mesh_sections_triggered(void)
	{
		mesh::sections::Sections(m_model->mesh(), nullptr).exec();
	}
	void Galileo::on_action_mesh_materials_triggered(void)
	{
		mesh::materials::Materials(m_model->mesh(), nullptr).exec();
	}
	
	void Galileo::on_action_results_nodes_triggered(void)
	{
		if(!m_model->analysis()->analysed())
		{
			QMessageBox::warning(nullptr, "Analysis", "The model must be analysed first!", QMessageBox::Ok);
			return;
		}
		results::Nodal* dia = new results::Nodal(m_model, this);
		dia->exec();
		delete dia;
	}
	void Galileo::on_action_results_elements_triggered(void)
	{
		if(!m_model->analysis()->analysed())
		{
			QMessageBox::warning(nullptr, "Analysis", "The model must be analysed first!", QMessageBox::Ok);
			return;
		}
		results::Elements* dia = new results::Elements(m_model, this);
		dia->exec();
		delete dia;
	}
	void Galileo::on_action_results_deformed_triggered(void)
	{
		if(!m_model->analysis()->analysed())
		{
			QMessageBox::warning(nullptr, "Analysis", "The model must be analysed first!", QMessageBox::Ok);
			return;
		}
		results::Deformed* dia = new results::Deformed(m_model, this);
		dia->exec();
		delete dia;
	}
	void Galileo::on_action_results_equilibrium_path_triggered(void)
	{
		if(!m_model->analysis()->analysed())
		{
			QMessageBox::warning(nullptr, "Analysis", "The model must be analysed first!", QMessageBox::Ok);
			return;
		}
		const std::string path = m_model->path() + "/" + m_model->name() + "/Solver/";
		if(!boost::filesystem::exists(path + "Load.txt") && !boost::filesystem::exists(path + "Time.txt"))
		{
			QMessageBox::warning(nullptr, "Analysis", "Solver type does not allow equilibrium path!", QMessageBox::Ok);
			return;
		}
		results::Equilibrium_Path* dia = new results::Equilibrium_Path(m_model, this);
		dia->exec();
		delete dia;
	}
	
	void Galileo::on_action_help_index_triggered(void)
	{
		help::Index* dia = new help::Index(nullptr);
		dia->exec();
		delete dia;
	}
	void Galileo::on_action_help_about_triggered(void)
	{
        help::About* dia = new help::About(nullptr);
		dia->exec();
		delete dia;
	}
	
	//events
	void Galileo::keyPressEvent(QKeyEvent* event) 
	{
		//key
		const unsigned key = event->key();
		//canvas
		const unsigned ck[] = {
			Qt::Key_X, Qt::Key_Y, Qt::Key_Z, Qt::Key_I, Qt::Key_F, Qt::Key_C, 
			Qt::Key_Right, Qt::Key_Left, Qt::Key_Up, Qt::Key_Down, Qt::Key_Plus, Qt::Key_Minus
		};
		if(std::find(ck, ck + 11, key) != ck + 11)
		{
			m_ui->canvas->keyPressEvent(event);
		}
		//new
		if(key == Qt::Key_N)
		{
			on_action_model_new_triggered();
		}
		//save
		if(key == Qt::Key_S)
		{
			on_action_model_save_triggered();
		}
		//load
		if(key == Qt::Key_O)
		{
			on_action_model_open_triggered();
		}
		//plot
		if(key == Qt::Key_W)
		{
			on_action_model_plot_triggered();
		}
		//print
		if(key == Qt::Key_P)
		{
			on_action_model_print_triggered();
		}
		//close
		if(key == Qt::Key_Q || key == Qt::Key_Escape)
		{
			on_action_model_close_triggered();
		}
		//deformed
		if(key == Qt::Key_D)
		{
			on_action_results_deformed_triggered();
		}
		//equilibrium path
		if(key == Qt::Key_E)
		{
			on_action_results_equilibrium_path_triggered();
		}
	}
	
	//saved
	void Galileo::plot(bool flag)
	{
		if(flag)
		{
			m_model->plot(m_plot);
			FILE* file = fopen(".plot", "r");
			if(file)
			{
				m_plot->load(file);
				fclose(file);
			}
		}
		else
		{
			FILE* file = fopen(".plot", "w");
			m_plot->save(file);
			fclose(file);
		}
	}
	void Galileo::recent(const char* path)
	{
		unsigned n = 0;
		unsigned m = 10;
		unsigned l = 200;
		char paths[m][l];
		m_ui->menu_model_recent->clear();
		FILE* file = fopen(".recent", "r");
		if(file)
		{
			while(fgets(paths[n], l, file))
			{
				*strrchr(paths[n++], '\n') = '\0';
			}
			fclose(file);
		}
		if(path)
		{
			for(unsigned i = 0; i < n; i++)
			{
				if(!strcmp(paths[i], path))
				{
					for(unsigned j = i; j + 1 < n; j++)
					{
						strcpy(paths[j], paths[j + 1]);
					}
					n--;
					break;
				}
			}
			n += n < m;
			for(unsigned i = 1; i < n; i++)
			{
				strcpy(paths[n - i], paths[n - i - 1]);
			}
			strcpy(paths[0], path);
			file = fopen(".recent", "w");
			for(unsigned i = 0; i < n; i++)
			{
				fprintf(file, "%s\n", paths[i]);
			}
			fclose(file);
		}
		if(n == 0)
		{
			m_ui->menu_model_recent->addAction("(none)")->setEnabled(false);
		}
		else
		{
			for(unsigned i = 0; i < n; i++)
			{
				QObject::connect(m_ui->menu_model_recent->addAction(paths[i]), SIGNAL(triggered(bool)), this, SLOT(slot_recent(void)));
			}
		}
	}
	void Galileo::current(char* path, bool flag)
	{
		if(flag)
		{
			FILE* file = fopen(".current", "r");
			strcpy(path, QDir::currentPath().toLocal8Bit().data());
			if(file)
			{
				fgets(path, 200, file);
				fclose(file);
			}
		}
		else
		{
			*strrchr(path, '/') = '\0';
			FILE* file = fopen(".current", "w");
			fprintf(file, "%s", path);
			fclose(file);
		}
	}
}
