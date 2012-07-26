#include "scenevodevicesconddisplay.h"
#include "fontmanager.h" // bt_global::font
#include "icondispatcher.h" // bt_global::icons_cache
#include "skinmanager.h" // bt_global::skin
#include "xml_functions.h" // getTextChild
#include "btbutton.h"

#include <QVBoxLayout>
#include <QLabel>

// The language used for the floating point number
static QLocale loc(QLocale::Italian);


DeviceConditionDisplay::DeviceConditionDisplay(QWidget *parent, QString descr, QString top_icon) : QWidget(parent)
{
	up_button = new BtButton(bt_global::skin->getImage("plus"));
	connect(up_button, SIGNAL(clicked()), this, SIGNAL(upClicked()));

	condition = new QLabel;
	condition->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	condition->setFont(bt_global::font->get(FontManager::TEXT));

	down_button = new BtButton(bt_global::skin->getImage("minus"));
	connect(down_button, SIGNAL(clicked()), this, SIGNAL(downClicked()));

	QGridLayout *main_layout = new QGridLayout(this);
	main_layout->setContentsMargins(0, 0, 0, 0);

#ifdef LAYOUT_TS_3_5
	main_layout->setSpacing(0);

	QHBoxLayout *top_layout = new QHBoxLayout;
	top_layout->setContentsMargins(0, 0, 0, 0);
	top_layout->setSpacing(0);

	QLabel *top_image = new QLabel;
	top_image->setPixmap(*bt_global::icons_cache.getIcon(top_icon));
	top_layout->addWidget(top_image);

	QLabel *description = new QLabel;
	description->setFont(bt_global::font->get(FontManager::TEXT));
	description->setText(descr);
	top_layout->addWidget(description, 1, Qt::AlignHCenter);

	main_layout->addLayout(top_layout, 0, 0, Qt::AlignTop);

	QBoxLayout *central_layout = new QVBoxLayout;
	central_layout->addWidget(up_button, 0, Qt::AlignHCenter);
	central_layout->addWidget(condition);
	central_layout->addWidget(down_button, 0, Qt::AlignHCenter);
	central_layout->setContentsMargins(0, 10, 0, 10);
	central_layout->setSpacing(20);

	main_layout->addLayout(central_layout, 1, 0, Qt::AlignHCenter);
#else
	Q_UNUSED(descr)
	Q_UNUSED(top_icon)
	main_layout->addWidget(down_button, 0, 0);
	main_layout->addWidget(condition, 0, 1);
	main_layout->addWidget(up_button, 0, 2);
	main_layout->setColumnMinimumWidth(1, 80);
	main_layout->setSpacing(10);
#endif
}


void DeviceConditionDisplayOnOff::updateText(int min_condition_value, int max_condition_value)
{
	Q_UNUSED(max_condition_value)
	condition->setText(min_condition_value ? tr("ON") : tr("OFF"));
}


void DeviceConditionDisplayDimming::updateText(int min_condition_value, int max_condition_value)
{
	if (min_condition_value == 0)
		condition->setText(tr("OFF"));
	else
		condition->setText(QString("%1% - %2%").arg(min_condition_value).arg(max_condition_value));
}


void DeviceConditionDisplayVolume::updateText(int min_condition_value, int max_condition_value)
{
	if (min_condition_value == -1)
		condition->setText(tr("OFF"));
	else if (min_condition_value == 0 && max_condition_value == 31)
		condition->setText(tr("ON"));
	else
	{
		int val_min = min_condition_value;
		int val_max = max_condition_value;
		int vmin = (val_min == 0 ? 0 : (10 * (val_min <= 15 ? val_min/3 : (val_min-1)/3) + 1));
		int vmax = 10 * (val_max <= 15 ? val_max/3 : (val_max-1)/3);
		condition->setText(QString("%1% - %2%").arg(vmin).arg(vmax));
	}
}


void DeviceConditionDisplayTemperature::updateText(int min_condition_value, int max_condition_value)
{
	up_button->setAutoRepeat(true);
	down_button->setAutoRepeat(true);
	Q_UNUSED(max_condition_value)
	QString tmp = loc.toString(min_condition_value / 10.0, 'f', 1);
	TemperatureScale temp_scale = static_cast<TemperatureScale>((*bt_global::config)[TEMPERATURE_SCALE].toInt());

	switch (temp_scale)
	{
	case CELSIUS:
		tmp += TEMP_DEGREES"C \2611"TEMP_DEGREES"C";
		break;
	case FAHRENHEIT:
		tmp += TEMP_DEGREES"F \2611"TEMP_DEGREES"F";
		break;
	default:
		qWarning("Wrong temperature scale, defaulting to celsius");
		tmp += TEMP_DEGREES"C \2611"TEMP_DEGREES"C";
	}
	condition->setText(tmp);
}
