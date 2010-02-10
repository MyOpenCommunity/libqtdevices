#include "scenevomanager.h"
#include "scenevocond.h"
#include "btbutton.h"
#include "skinmanager.h" // bt_global::skin

#include <QVBoxLayout>
#include <QStackedLayout>
#include <QDebug>



ScenEvoManager::ScenEvoManager(ScenEvoTimeCondition *time_cond, ScenEvoDeviceCondition *device_cond)
{
	QVBoxLayout *main_layout = new QVBoxLayout(this);
	main_layout->setContentsMargins(0, 5, 0, 10);
	main_layout->setSpacing(0);

	conditions_stack = new QStackedLayout;
	conditions_stack->setContentsMargins(0, 0, 0, 0);
	conditions_stack->setSpacing(0);

	if (time_cond)
		conditions_stack->addWidget(time_cond);

	if (device_cond)
		conditions_stack->addWidget(device_cond);

	main_layout->addLayout(conditions_stack);

	QHBoxLayout *bottom_layout = new QHBoxLayout;
	bottom_layout->setContentsMargins(0, 0, 0, 0);
	bottom_layout->setSpacing(0);

	BtButton *prev_button = new BtButton;
	prev_button->setImage(bt_global::skin->getImage("back"));
	connect(prev_button, SIGNAL(clicked()), SLOT(prev()));
	bottom_layout->addWidget(prev_button);
	bottom_layout->addStretch(1);

	next_button = new BtButton;
	next_button->setImage(bt_global::skin->getImage("forward"));
	connect(next_button, SIGNAL(clicked()), SLOT(next()));
	bottom_layout->addWidget(next_button);
	bottom_layout->addStretch(1);

	BtButton *ok = new BtButton;
	ok->setImage(bt_global::skin->getImage("ok"));
	connect(ok, SIGNAL(clicked()), SLOT(ok()));
	bottom_layout->addWidget(ok);

	main_layout->addLayout(bottom_layout);
	manageNextButton();
}

void ScenEvoManager::manageNextButton()
{
	if (conditions_stack->currentIndex() == conditions_stack->count() - 1)
		next_button->hide();
	else
		next_button->show();
}

void ScenEvoManager::prev()
{
	emit Closed();
	conditions_stack->setCurrentIndex(0);
	manageNextButton();
	emit reset();
}

void ScenEvoManager::next()
{
	if (conditions_stack->currentIndex() < conditions_stack->count() - 1)
		conditions_stack->setCurrentIndex(conditions_stack->currentIndex() + 1);

	manageNextButton();
}

void ScenEvoManager::ok()
{
	emit Closed();
	conditions_stack->setCurrentIndex(0);
	manageNextButton();
	emit save();
}

