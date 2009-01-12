#include "specialpage.h"
#include "timescript.h"
#include "btbutton.h"
#include "main.h" // ICON_FRECCIA_SX
#include "xml_functions.h" // getChildren, getTextChild, getChildWithName
#include "generic_functions.h" // createMsgOpen
#include "fontmanager.h"
#include "temperatureviewer.h"

#include <openwebnet.h>

#include <QDomNode>
#include <QDebug>
#include <QLabel>

#include <assert.h>


#define DIM_BUT_BACK 60


SpecialPage::SpecialPage(QDomNode config_node)
{
	temp_viewer = new TemperatureViewer(this);
	loadItems(config_node);
	loadSpecial(config_node);
}

void SpecialPage::loadItems(QDomNode config_node)
{
	QDomNode item;
	foreach (item, getChildren(config_node, "item"))
	{
		int id = getTextChild(item, "id").toInt();
		// We get the number after "item"
		int itemNum = item.nodeName().mid(QString("item").length()).toInt();
		switch (id)
		{
		case DATA:
		case OROLOGIO:
		{
			timeScript *d = new timeScript(this, id == DATA ? 25 : 1);
			d->setGeometry(10, (itemNum-1)*80 + 10, 220, 60);
			d->setFrameStyle(QFrame::Plain);
			d->setLineWidth(3);
			break;
		}
		case TEMPERATURA:
		case TERMO_HOME_NC_PROBE:
		case TERMO_HOME_NC_EXTPROBE:
		{
			QString ext = (id == TERMO_HOME_NC_EXTPROBE) ? "1" : "0";
			temp_viewer->add(getTextChild(item, "where"), 10, (itemNum-1)*80 + 10, 220, 60, QFrame::Plain,
				3, getTextChild(item, "descr"), ext);
			break;
		}
		default:
			assert(!"Unknown item type on SpecialPage!");
		}
	}
}

void SpecialPage::loadSpecial(QDomNode config_node)
{
	// Load the back button
	BtButton *b = new BtButton(this);
	b->setImage(ICON_FRECCIA_SX);
	b->setGeometry(0, 260, DIM_BUT_BACK, DIM_BUT_BACK);
	connect(b, SIGNAL(clicked()), this, SIGNAL(Closed()));
	QDomNode command = getChildWithName(config_node, "command");

	// Load the special button
	b = new BtButton(this);
	b->setImage(IMG_PATH + getTextChild(command, "cimg1"));
	b->setGeometry(DIM_BUT_BACK, 260, 260 - DIM_BUT_BACK, DIM_BUT_BACK);

	type = static_cast<specialType>(getTextChild(command, "type").toInt());
	who = getTextChild(command, "who");
	what = getTextChild(command, "what");
	where = getTextChild(command, "where");

	if (type == BUTTON)
	{
		connect(b, SIGNAL(pressed()), SLOT(pressedButton()));
		connect(b, SIGNAL(released()), SLOT(releasedButton()));
	}
	else
		connect(b, SIGNAL(clicked()), SLOT(clickedButton()));

	// Load the description of special button
	QFont aFont;
	FontManager::instance()->getFont(font_homepage_bottoni_descrizione, aFont);
	QLabel *box_text = new QLabel(this);
	box_text->setFont(aFont);
	box_text->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	box_text->setText(getTextChild(command, "descr"));
	box_text->setGeometry(DIM_BUT_BACK, 240, 260 - DIM_BUT_BACK, 20);
	box_text->setFrameStyle(QFrame::Plain);
	box_text->setLineWidth(3);
}

void SpecialPage::clickedButton()
{
	if (type == CYCLIC)
		what = what == "0" ? "1" : "0";

	sendFrame(createMsgOpen(who, what, where));
}

void SpecialPage::pressedButton()
{
	sendFrame(createMsgOpen(who, "1", where));
}

void SpecialPage::releasedButton()
{
	sendFrame(createMsgOpen(who, "0", where));
}

void SpecialPage::gestFrame(char *frame)
{
	temp_viewer->gestFrame(frame);
	openwebnet msg_open;
	msg_open.CreateMsgOpen(frame, strstr(frame,"##") - frame + 2);

	if (who == QString(msg_open.Extract_chi()) && type == CYCLIC)
		if (where == QString(msg_open.Extract_dove()))
			what = QString(msg_open.Extract_cosa());
}

void SpecialPage::inizializza()
{
	temp_viewer->inizializza();
}
