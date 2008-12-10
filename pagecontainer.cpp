#include "pagecontainer.h"
#include "main.h"
#include "btbutton.h"

#define DIM_BUT 80


PageContainer::PageContainer() : buttons_group(this)
{
	setGeometry(0, 0, MAX_WIDTH, MAX_HEIGHT);
	setFixedSize(MAX_WIDTH, MAX_HEIGHT);
	connect(&buttons_group, SIGNAL(buttonClicked(int)), SLOT(clicked(int)));
}

void PageContainer::addPage(Page *page, int id, QString iconName, int x, int y)
{
	BtButton *b = new BtButton(this);
	b->setGeometry(x, y, DIM_BUT, DIM_BUT);
	b->setImage(iconName);

	buttons_group.addButton(b, id);
	page_list[id] = page;
	connect(page, SIGNAL(Closed()), this, SLOT(showPage()));
}

void PageContainer::clicked(int id)
{
	page_list[id]->showPage();
	hide();
}


