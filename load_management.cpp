#include "load_management.h"
#include "xml_functions.h"
#include "bannercontent.h"
#include "main.h"
#include "bann_energy.h"
#include "skinmanager.h" // SkinContext
#include "fontmanager.h" // FontManager
#include "navigation_bar.h" //NavigationBar
#include "bann2_buttons.h" // Bann2Buttons

#include <QLabel>
#include <QDebug>
#include <QVBoxLayout>


namespace
{
	QString getDescriptionWithPriority(const QDomNode &n)
	{
		QString where = getTextChild(n, "where");
		int pos = where.indexOf('#');
		Q_ASSERT_X(pos > -1, "getDescriptionWithPriority", "Device where must have a '#' character");
		QString descr = getTextChild(n, "descr");
		// remove part after '#'
		QString priority = where.left(pos);
		// remove first 7
		priority = priority.mid(1);
		return priority + ". " + descr;
	}

	bool isRateEnabled(const QDomNode &n)
	{
		QDomElement e = getElement(n, "rate/ab");
		bool ok;
		int tmp = e.text().toInt(&ok);
		if (ok)
			return (tmp == 1);
		else
		{
			qDebug() << "isRateEnabled() failed, text was: " << e.text();
			return false;
		}
	}
}
LoadManagement::LoadManagement(const QDomNode &config_node) :
	BannerPage(0)
{
	buildPage(getTextChild(config_node, "descr"));
	loadItems(config_node);
}

void LoadManagement::loadItems(const QDomNode &config_node)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		banner *b = getBanner(item);
		if (b)
		{
			page_content->appendBanner(b);
			connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
		}
	}
}

banner *LoadManagement::getBanner(const QDomNode &item_node)
{
	SkinContext context(getTextChild(item_node, "cid").toInt());
	int id = getTextChild(item_node, "id").toInt();
	banner *b = 0;
	switch (id)
	{
	case LOAD_WITH_CU:
	{
		bool advanced = getTextChild(item_node, "advanced").toInt();
		BannLoadWithCU *bann = new BannLoadWithCU(getDescriptionWithPriority(item_node),
			advanced ? BannLoadWithCU::ADVANCED_MODE : BannLoadWithCU::BASE_MODE);
		if (advanced)
		{
			Page *p = new LoadDataPage(item_node);
			bann->connectRightButton(p);
			connect(p, SIGNAL(Closed()), bann, SIGNAL(pageClosed()));
		}
		Page *d = new DeactivationTimePage(item_node);
		connect(bann, SIGNAL(deactivateDevice()), d, SLOT(showPage()));
		connect(d, SIGNAL(Closed()), bann, SIGNAL(pageClosed()));

		b = bann;
	}
		break;
	case LOAD_WITHOUT_CU:
	{
		BannLoadNoCU *bann = new BannLoadNoCU(getTextChild(item_node, "descr"));
		Page *p = new LoadDataPage(item_node);
		bann->connectRightButton(p);
		connect(p, SIGNAL(Closed()), bann, SIGNAL(pageClosed()));
		b = bann;
	}
		break;
	}

	if (b)
		b->setId(id);
	return b;
}


ConfirmationPage::ConfirmationPage(const QString &text)
{
	NavigationBar *nav_bar = new NavigationBar(bt_global::skin->getImage("ok"));
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(cancel()));
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));
	connect(nav_bar, SIGNAL(forwardClick()), SIGNAL(accept()));
	connect(nav_bar, SIGNAL(forwardClick()), SIGNAL(Closed()));
	nav_bar->displayScrollButtons(false);

	QLabel *content = new QLabel(text);
	content->setFont(bt_global::font->get(FontManager::SUBTITLE));
	content->setWordWrap(true);

	QVBoxLayout *main = new QVBoxLayout(this);
	main->setContentsMargins(0, 5, 0, 10);
	main->setSpacing(0);
	main->addWidget(content, 1);
	main->addWidget(nav_bar);
}


LoadDataContent::LoadDataContent(int _rate_id)
{
	const int FIRST_RESET = 1;
	const int SECOND_RESET = 2;

	current_consumption = new QLabel;
	current_consumption->setText("Current consumption");
	current_consumption->setFont(bt_global::font->get(FontManager::SUBTITLE));

	first_period = new Bann2Buttons;
	first_period->initBanner(QString(), bt_global::skin->getImage("empty_background"), bt_global::skin->getImage("ok"), "data/ora del reset");
	first_period->setCentralText("Total consumption 1");
	connect(first_period, SIGNAL(rightClicked()), &mapper, SLOT(map()));
	mapper.setMapping(first_period, FIRST_RESET);

	second_period = new Bann2Buttons;
	second_period->initBanner(QString(), bt_global::skin->getImage("empty_background"), bt_global::skin->getImage("ok"), "data/ora del reset");
	second_period->setCentralText("Total consumption 2");
	connect(second_period, SIGNAL(rightClicked()), &mapper, SLOT(map()));
	mapper.setMapping(second_period, SECOND_RESET);

	connect(&mapper, SIGNAL(mapped(int)), SIGNAL(resetActuator(int)));

	QVBoxLayout *main = new QVBoxLayout(this);
	main->setContentsMargins(0, 0, 0, 0);
	main->setSpacing(0);
	main->addWidget(current_consumption, 0, Qt::AlignHCenter);
	main->addWidget(first_period);
	main->addWidget(second_period);

	rate_id = _rate_id;
	rate = EnergyRates::energy_rates.getRate(rate_id);
	connect(&EnergyRates::energy_rates, SIGNAL(rateChanged(int)), SLOT(rateChanged(int)));
}

void LoadDataContent::updatePeriodDate(int period, QDate date, BtTime time)
{
	QString text = date.toString() + " " + time.toString();
	if (period == 1)
	{
		first_period->setDescriptionText(text);
	}
	else if (period == 2)
	{
		second_period->setDescriptionText(text);
	}
	else
		qDebug() << "LoadDataContent::updatePeriodDate: Invalid period";
}

void LoadDataContent::updatePeriodValue(int period, int new_value)
{
	if (period == 1)
	{
		first_period_value = new_value;
	}
	else if (period == 2)
	{
		second_period_value = new_value;
	}
	else
		qDebug() << "LoadDataContent::updatePeriodValue: Invalid period";
	updateValues();
}

void LoadDataContent::setConsumptionValue(int new_value)
{
	current_value = new_value;
	updateValues();
}

void LoadDataContent::rateChanged(int id)
{
	if (id == rate_id)
	{
		rate = EnergyRates::energy_rates.getRate(id);
		updateValues();
	}
}

void LoadDataContent::updateValues()
{
	// TODO: update all values
	// TODO: do conversions
}

// TODO: update banner values. These are the steps
// - current and total consumption: convert using convertToRawData(), with ELECTRICITY_CURRENT as parameter
// - create the text for the label, use the precision specified in conf file


LoadDataPage::LoadDataPage(const QDomNode &config_node)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());
	content = new LoadDataContent;

	QLabel *page_title = new QLabel(getDescriptionWithPriority(config_node));
	page_title->setFont(bt_global::font->get(FontManager::TEXT));

	ConfirmationPage *confirm = new ConfirmationPage(tr("Are you sure to delete all consumption data?"));
	// show pages correctly
	connect(content, SIGNAL(resetActuator(int)), confirm, SLOT(showPage()));
	connect(confirm, SIGNAL(Closed()), SLOT(showPage()));
	// these connects handle the logic
	connect(content, SIGNAL(resetActuator(int)), SLOT(resetRequested(int)));
	connect(confirm, SIGNAL(accept()), SLOT(reset()));
	reset_number = 0;

	// read the correct measure unit
	QString forward_button;
	if (isRateEnabled(config_node))
		forward_button = bt_global::skin->getImage("currency_exchange");
	// measure unit here will always be kWh

	NavigationBar *nav_bar = new NavigationBar(forward_button);
	nav_bar->displayScrollButtons(false);
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));
	QVBoxLayout *main = new QVBoxLayout(this);
	main->setContentsMargins(0, 5, 0, 10);
	main->setSpacing(0);
	main->addWidget(page_title, 0, Qt::AlignHCenter);
	main->addWidget(content, 1);
	main->addWidget(nav_bar);
}

void LoadDataPage::resetRequested(int which)
{
	reset_number = which;
}

void LoadDataPage::reset()
{
	// TODO: send reset frame with correct reset_number
	qDebug() << "Sending reset number: " << reset_number;
}



DeactivationTimePage::DeactivationTimePage(const QDomNode &config_node)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());

	QLabel *page_title = new QLabel(getDescriptionWithPriority(config_node));
	page_title->setFont(bt_global::font->get(FontManager::TEXT));

	NavigationBar *nav_bar = new NavigationBar(bt_global::skin->getImage("ok"));
	nav_bar->displayScrollButtons(false);
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));
	// TODO: cancel user selection?
	connect(nav_bar, SIGNAL(forwardClick()), SLOT(sendDeactivateDevice()));
	connect(nav_bar, SIGNAL(forwardClick()), SIGNAL(Closed()));

	content = new DeactivationTime(BtTime(2, 30, 0));
	QVBoxLayout *main = new QVBoxLayout(this);
	main->setContentsMargins(0, 5, 0, 10);
	main->setSpacing(0);
	main->addWidget(page_title, 0, Qt::AlignHCenter);
	main->addWidget(content, 1);
	main->addWidget(nav_bar);
}

void DeactivationTimePage::sendDeactivateDevice()
{
	// TODO: send a deactivation frame
	qDebug() << "Deactivating device for time: " << content->currentTime().toString();
}
