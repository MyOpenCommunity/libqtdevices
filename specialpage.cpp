#include "specialpage.h"
#include "timescript.h"
#include "btbutton.h"
#include "main.h" // ICON_FRECCIA_SX
#include "xml_functions.h" // getChildren, getTextChild, getChildWithName
#include "generic_functions.h" // createMsgOpen
#include "fontmanager.h" // bt_global::font
#include "temperatureviewer.h"
#include "skinmanager.h" //skin

#include <openmsg.h>

#include <QDomNode>
#include <QDebug>
#include <QLabel>


#define DIM_BUT_BACK 60


SpecialPage::SpecialPage(const QDomNode &config_node)
{
	temp_viewer = new TemperatureViewer(this);
	loadItems(config_node);
	loadSpecial(config_node);
}

void SpecialPage::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode &item, getChildren(config_node, "item"))
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
			d->setLineWidth(3);
			break;
		}
		case TEMPERATURA:
		case TERMO_HOME_NC_PROBE:
		case TERMO_HOME_NC_EXTPROBE:
		{
			QString ext = (id == TERMO_HOME_NC_EXTPROBE) ? "1" : "0";
			temp_viewer->add(getTextChild(item, "where"), 10, (itemNum-1)*80 + 10, 220, 60,
				getTextChild(item, "descr"), ext);
			break;
		}
		default:
			qFatal("Unknown item type on SpecialPage!");
		}
	}
}

void SpecialPage::loadSpecial(const QDomNode &config_node)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());

	// Load the back button
	BtButton *b = new BtButton(this);
	b->setImage(bt_global::skin->getImage("back"));
	b->setGeometry(0, 260, DIM_BUT_BACK, DIM_BUT_BACK);
	connect(b, SIGNAL(clicked()), SIGNAL(Closed()));

	// Load the special button
	b = new BtButton(this);
	b->setImage(bt_global::skin->getImage("command"));
	b->setGeometry(DIM_BUT_BACK, 260, MAX_WIDTH - DIM_BUT_BACK, DIM_BUT_BACK);

	QDomNode command = getChildWithName(config_node, "command");
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
	QLabel *box_text = new QLabel(this);
	box_text->setFont(bt_global::font->get(FontManager::TEXT));
	box_text->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	box_text->setText(getTextChild(command, "descr"));
	box_text->setGeometry(DIM_BUT_BACK, 240, MAX_WIDTH - DIM_BUT_BACK, 20);
	box_text->setFrameStyle(QFrame::Plain);
	box_text->setLineWidth(3);

	subscribe_monitor(who.toInt());
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

void SpecialPage::manageFrame(OpenMsg &msg)
{
	if (who.toInt() == msg.who() && type == CYCLIC)
		if (where == QString(msg.Extract_dove()))
			what = QString(msg.Extract_cosa());
}

void SpecialPage::inizializza()
{
	temp_viewer->inizializza();
}
