#include "vctcallpage.h"
#include "btbutton.h"
#include "skinmanager.h"
#include "generic_functions.h" //getBostikName

#include <QDomNode>
#include <QHBoxLayout>
#include <QDebug>

BannTuning::BannTuning(const QString &banner_text, const QString &icon_name, QWidget *parent) :
	Bann2CentralButtons(parent)
{
	// levels go from 0 to 8 inclusive
	current_level = 4;
	center_icon = icon_name;

	initBanner(getBostikName(center_icon, QString("sxl") + QString::number(current_level)),
		getBostikName(center_icon, QString("dxl") + QString::number(current_level)),
		banner_text);
	connect(center_left, SIGNAL(clicked()), SLOT(decreaseLevel()));
	connect(center_right, SIGNAL(clicked()), SLOT(increaseLevel()));
}

void BannTuning::decreaseLevel()
{
	if (current_level > 0)
	{
		--current_level;
		changeIcons();
		emit valueChanged(current_level);
	}
}

void BannTuning::increaseLevel()
{
	if (current_level < 8)
	{
		++current_level;
		changeIcons();
		emit valueChanged(current_level);
	}
}

void BannTuning::changeIcons()
{
	center_left->setImage(getBostikName(center_icon, QString("sxl") + QString::number(current_level)));
	center_right->setImage(getBostikName(center_icon, QString("dxl") + QString::number(current_level)));
}



VCTCallPage::VCTCallPage()
{
	SkinContext ctx(666);

	// sidebar
	QVBoxLayout *sidebar = new QVBoxLayout;

	BannTuning *bright = new BannTuning(tr("Brightness"), bt_global::skin->getImage("brightness"));
	sidebar->addWidget(bright);
	BannTuning *contrast = new BannTuning(tr("Contrast"), bt_global::skin->getImage("contrast"));
	sidebar->addWidget(contrast);
	BannTuning *color = new BannTuning(tr("Color"), bt_global::skin->getImage("color"));
	sidebar->addWidget(color);

	BtButton *setup_vct = new BtButton;
	setup_vct->setImage(bt_global::skin->getImage("setup_vct"));
	sidebar->addWidget(setup_vct);
	//connect(bright, SIGNAL(center_left_clicked()), SLOT(decreaseBrightness()));

	QHBoxLayout *hbox = new QHBoxLayout;

	// widget where video will be displayed
	QWidget *video = new QWidget;
	video->setFixedSize(352, 240);
	video->setStyleSheet("background-color: black");

	hbox->addWidget(video);
	hbox->addLayout(sidebar);

	BtButton *back = new BtButton;
	back->setImage(bt_global::skin->getImage("back"));
	connect(back, SIGNAL(clicked()), SIGNAL(Closed()));

	call_accept = new BtButton;
	call_icon = bt_global::skin->getImage("call");
	call_accept->setImage(getBostikName(call_icon, "off"));

	BannTuning *volume = new BannTuning("", bt_global::skin->getImage("volume"));

	mute_button = new BtButton;
	mute_icon = bt_global::skin->getImage("mute");
	mute_button->setImage(getBostikName(mute_icon, "offdis"));

	stairlight = new BtButton;
	stairlight_icon = bt_global::skin->getImage("stairlight");
	stairlight->setImage(stairlight_icon);

	unlock_door = new BtButton;
	unlock_icon = bt_global::skin->getImage("unlock_door");
	unlock_door->setImage(unlock_icon);

	cycle = new BtButton;
	cycle->setImage(bt_global::skin->getImage("cycle"));

	QHBoxLayout *bottom = new QHBoxLayout;
	bottom->setContentsMargins(0, 0, 30, 0);
	bottom->setSpacing(15);
	bottom->addWidget(back);
	bottom->addWidget(call_accept);
	bottom->addWidget(volume);
	bottom->addWidget(mute_button);
	bottom->addWidget(stairlight);
	bottom->addWidget(unlock_door);
	bottom->addWidget(cycle);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addLayout(hbox);
	layout->addLayout(bottom);
}

void VCTCallPage::addExternalPlace(const QString &where)
{
	places << where;
}
