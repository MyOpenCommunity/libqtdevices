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

#include <qlayout.h>


// Interface icon paths.
static const char *IMG_SELECT = IMG_PATH "arrrg.png";
static const char *IMG_SELECT_P = IMG_PATH "arrrgp.png";


ListBrowser::ListBrowser(QWidget *parent, unsigned _rows_per_page, const char *name, WFlags f) :
	QWidget(parent, name, f)
{
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

	// Create main Layout
	QHBoxLayout *main_layout = new QHBoxLayout(this);
	main_layout->setMargin(0);
	main_layout->setSpacing(0);

	// Set the number of elements shown
	rows_per_page = _rows_per_page;

	// Create labels_layout
	QVBoxLayout *labels_layout = new QVBoxLayout(main_layout);
	labels_layout->setMargin(0);
	labels_layout->setSpacing(0);

	// Create labels and add them to label_layout
	// WARNING Quick and Dirty alignment using offsets of TitleLabel
	QValueVector<int> h_offsets;
	h_offsets.append(-4);
	h_offsets.append(-2);
	h_offsets.append(-1);
	h_offsets.append(0);
	labels_list.resize(rows_per_page);
	labels_list.setAutoDelete(true);
	for (unsigned i = 0; i < rows_per_page; ++i)
	{
		// Create label and add it to labels_layout
		labels_list.insert(i, new TitleLabel(this, MAX_WIDTH - 60, 50, 9, h_offsets[i], true));
		labels_layout->addWidget(labels_list[i]);
	}

	// Create buttons_bar
	buttons_bar = new ButtonsBar(this, rows_per_page, Qt::Vertical);

	// Set Icons for buttons_bar (using icons_library cache)
	QPixmap *icon         = icons_library.getIcon(IMG_SELECT);
	QPixmap *pressed_icon = icons_library.getIcon(IMG_SELECT_P);
	for (unsigned i = 0; i < rows_per_page; ++i)
		buttons_bar->setButtonIcons(i, *icon, *pressed_icon);

	// Add buttons_bar to main_layout
	main_layout->addWidget(buttons_bar);
	connect(buttons_bar, SIGNAL(clicked(int)), SLOT(itemIsClicked(int)));
}

void ListBrowser::setBGColor(QColor c)
{
	setPaletteBackgroundColor(c);
	buttons_bar->setBGColor(c);
}

void ListBrowser::setFGColor(QColor c)
{
	setPaletteForegroundColor(c);
	buttons_bar->setFGColor(c);
}

void ListBrowser::setList(QValueVector<QString> _item_list, unsigned _current_page)
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
	unsigned start = current_page * rows_per_page;
	unsigned end   = std::min(start + rows_per_page, item_list.count());

	for (unsigned i = 0; i < labels_list.size(); ++i)
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

	if (start==end)
	{
		buttons_bar->showButton(0);
		buttons_bar->setEnabled(false);
	}
	else
	{
		buttons_bar->setEnabled(true);
	}
}

void ListBrowser::nextItem()
{
	if ((current_page + 1) * rows_per_page <= item_list.count())
		++current_page;
	showList();
}

void ListBrowser::prevItem()
{
	if (current_page)
		--current_page;
	showList();
}

void ListBrowser::itemIsClicked(int item)
{
	QString clicked_element = item_list[current_page * rows_per_page + item];
	emit itemIsClicked(clicked_element);
}
