//qt
#include <QStandardItemModel>

//gui
#include "Actions/Results/Paths.h"

//ui
#include "ui_Paths.h"

namespace gui
{
	namespace results
	{
		//constructors
		Paths::Paths(double*** positions, std::vector<unsigned>* paths, unsigned nodes, QWidget* parent) : QDialog(parent), 
			m_ui(new Ui::Paths), m_nodes(nodes), m_paths(paths)
		{
			//set ui
			m_ui->setupUi(this);
			//formatter
			char formatter[200];
			sprintf(formatter, "%05d", m_paths->empty() || m_paths->back() == m_nodes ? 0 : m_paths->back() + 1);
			//set edit
			m_ui->edit->setText(formatter);
			m_ui->edit->setValidator(new QIntValidator(0, m_nodes));
			//set table
			m_ui->table->setColumnCount(1);
			m_ui->table->setHorizontalHeaderItem(0, new QTableWidgetItem("Nodes"));
			m_ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); 
			//update
			update_table();
			m_ui->table->setCurrentCell(m_ui->table->rowCount() - 1, 0);
			//connect
			QObject::connect(m_ui->button_add, SIGNAL(clicked(bool)), this, SLOT(slot_add(void)));
			QObject::connect(m_ui->button_clear, SIGNAL(clicked(bool)), this, SLOT(slot_clear(void)));
			QObject::connect(m_ui->button_remove, SIGNAL(clicked(bool)), this, SLOT(slot_remove(void)));
		}
		
		//destructor
		Paths::~Paths(void)
		{
			delete m_ui;
		}
		
		//slots
		void Paths::slot_add(void)
		{
			const unsigned p = m_ui->edit->text().toUInt();
			if(std::find(m_paths->begin(), m_paths->end(), p) == m_paths->end())
			{
				//plots
				m_paths->push_back(p);
				//edit
				char formatter[200];
				sprintf(formatter, "%05d", p == m_nodes ? 0 : p + 1);
				m_ui->edit->setText(formatter);
				//table
				update_table();
				m_ui->table->selectRow(m_ui->table->rowCount() - 1);
			}
		}
		void Paths::slot_clear(void)
		{
			//plots
			m_paths->clear();
			//table
			update_table();
			//edit
			char formatter[200];
			sprintf(formatter, "%05d", 0);
			m_ui->edit->setText(formatter);
		}
		void Paths::slot_remove(void)
		{
			//list
			QModelIndexList sr = m_ui->table->selectionModel()->selectedRows(0);
			//remove
			for(unsigned i = 0; i < sr.size(); i++)
			{
				const unsigned j = sr.size() - i - 1;
				m_paths->erase(m_paths->begin() + sr[j].row());
			}
			//table
			update_table();
			m_ui->table->setCurrentCell(m_ui->table->rowCount() - 1, 0);
			//formatter
			char formatter[200];
			sprintf(formatter, "%05d", m_paths->empty() || m_paths->back() == m_nodes ? 0 : m_paths->back() + 1);
			//edit
			m_ui->edit->setText(formatter);
		}
		
		//update
		void Paths::update_table(void)
		{
			//set rows
			m_ui->table->setRowCount(m_paths->size());
			//set data
			char formatter[200];
			for(unsigned i = 0; i < m_paths->size(); i++)
			{
				//text
				sprintf(formatter, "%05d", (*m_paths)[i]);
				QTableWidgetItem* item = new QTableWidgetItem(formatter);
				//properties
				item->setTextAlignment(Qt::AlignCenter);
				item->setFlags(item->flags() ^ Qt::ItemIsEditable);
				//add
				m_ui->table->setItem(i, 0, item);
			}
		}
	}
}
