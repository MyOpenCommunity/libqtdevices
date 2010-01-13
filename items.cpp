#include "items.h"

#include "btbutton.h"
#include "generic_functions.h" // getBostikName

#include <QLabel>
#include <QBoxLayout>


ItemTuning::ItemTuning(QString text, QString icon, QWidget *parent) : QWidget(parent)
{
	QVBoxLayout *main_layout = new QVBoxLayout(this);
	main_layout->setContentsMargins(0, 0, 0, 0);
	main_layout->setSpacing(0);

	main_layout->setSizeConstraint(QLayout::SetFixedSize);

	QHBoxLayout *hlayout = new QHBoxLayout;
	hlayout->setContentsMargins(0, 0, 0, 0);
	hlayout->setSpacing(0);
	hlayout->setSizeConstraint(QLayout::SetFixedSize);

	left = new BtButton;
	connect(left, SIGNAL(clicked()), SLOT(decreaseLevel()));
	hlayout->addWidget(left);

	right = new BtButton;
	connect(right, SIGNAL(clicked()), SLOT(increaseLevel()));
	hlayout->addWidget(right);

	main_layout->addLayout(hlayout);

	if (!text.isEmpty())
	{
		QLabel *label = new QLabel;
		label->setText(text);
		main_layout->addWidget(label, 1, Qt::AlignHCenter);
	}

	// levels go from 0 to 8 inclusive
	current_level = 4;
	icon_name = icon;

	left->setImage(getBostikName(icon_name, QString("sxl") + QString::number(current_level)));
	right->setImage(getBostikName(icon_name, QString("dxl") + QString::number(current_level)));
	connect(this, SIGNAL(valueChanged(int)), this, SLOT(changeIcons()));
}

ItemTuningStatus ItemTuning::getStatus()
{
	ItemTuningStatus st;
	st.current_level = current_level;
	return st;
}

void ItemTuning::setStatus(const ItemTuningStatus &st)
{
	current_level = st.current_level;
	changeIcons();
}

void ItemTuning::decreaseLevel()
{
	if (current_level > 0)
	{
		--current_level;
		emit valueChanged(current_level);
	}
}

void ItemTuning::increaseLevel()
{
	if (current_level < 8)
	{
		++current_level;
		emit valueChanged(current_level);
	}
}

void ItemTuning::changeIcons()
{
	left->setImage(getBostikName(icon_name, QString("sxl") + QString::number(current_level)));
	right->setImage(getBostikName(icon_name, QString("dxl") + QString::number(current_level)));
}

