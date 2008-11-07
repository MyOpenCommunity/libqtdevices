/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** listbrowser.cpp
 **
 ** browser of a list of items.
 **
 ****************************************************************/

#include "listbrowser.h"
#include "buttons_bar.h"
#include "fontmanager.h"
#include "main.h"

#include <QLayout>
#include <QFont>

#include <algorithm>

// Interface icon paths.
static const char *IMG_SELECT = IMG_PATH "arrrg.png";

ListBrowser::ListBrowser(QWidget *parent, unsigned _rows_per_page, const char *name, Qt::WindowFlags f) :
	QWidget(parent, f)
{
	QFont aFont;
	FontManager::instance()->getFont(font_listbrowser, aFont);
	setFont(aFont);

	// TODO: sistemare lo stile!!
	/*
	// Set Style
	// Look QColorGroup Class Reference
	QPalette current_color_palette = palette();
	current_color_palette.setColor(QColorGroup::Text, Qt::white);
	current_color_palette.setColor(QColorGroup::Base, Qt::black);
	current_color_palette.setColor(QColorGroup::Background, Qt::black);
	current_color_palette.setColor(QColorGroup::Foreground, Qt::white);
	// 3D Effect
	current_color_palette.setColor(QColorGroup::Shadow, Qt::black);
	current_color_palette.setColor(QColorGroup::Midlight, Qt::black);
	current_color_palette.setColor(QColorGroup::Dark, Qt::black);
	setPalette(current_color_palette);
	*/

	// Create main Layout
	QHBoxLayout *main_layout = new QHBoxLayout(this);
	main_layout->setMargin(0);
	main_layout->setSpacing(0);

	// Set the number of elements shown
	rows_per_page = _rows_per_page;
	current_page = 0;

	// Create labels_layout
	QVBoxLayout *labels_layout = new QVBoxLayout();
	labels_layout->setMargin(0);
	labels_layout->setSpacing(0);

	// Create labels and add them to label_layout
	// WARNING Quick and Dirty alignment using offsets of TitleLabel
	QVector<int> h_offsets;
	h_offsets.append(-4);
	h_offsets.append(-2);
	h_offsets.append(-1);
	h_offsets.append(0);
	labels_list.reserve(rows_per_page);
	for (unsigned i = 0; i < rows_per_page; ++i)
	{
		// Create label and add it to labels_layout
		labels_list.append(new TitleLabel(this, MAX_WIDTH - 60, 50, 9, h_offsets[i], true));
		labels_layout->addWidget(labels_list[i]);
	}
	main_layout->addLayout(labels_layout);

	// Create buttons_bar
	buttons_bar = new ButtonsBar(this, rows_per_page, Qt::Vertical);

	// Set Icons for buttons_bar (using icons_library cache)
	for (unsigned i = 0; i < rows_per_page; ++i)
		buttons_bar->setButtonIcon(i, IMG_SELECT);

	// Add buttons_bar to main_layout
	main_layout->addWidget(buttons_bar);
	connect(buttons_bar, SIGNAL(clicked(int)), SLOT(clicked(int)));
}

ListBrowser::~ListBrowser()
{
	for (int i = 0; i < labels_list.size(); ++i)
		delete labels_list[i];
}

void ListBrowser::setList(QVector<QString> _item_list, unsigned _current_page)
{
	item_list = _item_list;
	current_page = _current_page;
}

void ListBrowser::showEvent(QShowEvent *event)
{
	for (unsigned i = 0; i < rows_per_page; ++i)
		labels_list[i]->resetTextPosition();
}

unsigned ListBrowser::getCurrentPage()
{
	return current_page;
}

void ListBrowser::showList()
{
	int start = current_page * rows_per_page;
	int end   = std::min(static_cast<int>(start + rows_per_page), item_list.count());

	for (int i = 0; i < labels_list.size(); ++i)
	{
		if (i < end-start)
		{
			labels_list[i]->setText(item_list[start+i]);
			buttons_bar->showButton(i);
		}
		else
		{
			labels_list[i]->setText("");
			buttons_bar->hideButton(i);
		}
	}

	if (start!=end)
		buttons_bar->setEnabled(true);
}

void ListBrowser::nextItem()
{
	if ((current_page + 1) * rows_per_page < item_list.count())
		++current_page;
	showList();
}

void ListBrowser::prevItem()
{
	if (current_page)
		--current_page;
	showList();
}

void ListBrowser::clicked(int item)
{
	emit itemIsClicked(current_page * rows_per_page + item);
}
