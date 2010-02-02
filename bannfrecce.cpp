/*
 * BTicino Touch scren Colori art. H4686
 *
 * Riga con tasto ON OFF, icona centrale e scritta sotto
 */

#include "bannfrecce.h"
#include "btbutton.h"

bannFrecce::bannFrecce(QWidget *parent, uchar num, QString IconBut4) : BannerOld(parent)
{
	qDebug("bannFrecce::bannFrecce(%d)", num);
	addItem(BUT1,(banner_width/4-BUTFRECCE_DIM_X)/2,
		(banner_height-BUTFRECCE_DIM_Y)/2, BUTFRECCE_DIM_X,
		BUTFRECCE_DIM_Y);

	if (num > 1 && num < 8)
	{
		addItem(BUT3 , banner_width/4+(banner_width/4-BUTFRECCE_DIM_X)/2,
			(banner_height-BUTFRECCE_DIM_Y)/2,
			BUTFRECCE_DIM_X, BUTFRECCE_DIM_Y);

		if (num > 2 && num < 7)
			addItem(BUT4 , banner_width/2 + (banner_width/4 - BUTFRECCE_DIM_X)/2,
				(banner_height-BUTFRECCE_DIM_Y)/2,
				BUTFRECCE_DIM_X, BUTFRECCE_DIM_Y);
	}

	dx_button_coord = QRect(banner_width*3/4+(banner_width/4-BUTFRECCE_DIM_X)/2,
		(banner_height-BUTFRECCE_DIM_Y)/2, BUTFRECCE_DIM_X, BUTFRECCE_DIM_Y);

	if ((num > 3 && num < 6) || num == 9 || num == 10)
	{
		addItem(BUT2, dx_button_coord.left(), dx_button_coord.top(),
			dx_button_coord.width(), dx_button_coord.height());
	}

	if (num < 5 || num == 10)
		SetIcons(ICON_FRECCIA_SX, IconBut4, ICON_FRECCIA_SU, ICON_FRECCIA_GIU);
	else
		SetIcons(ICON_OK, IconBut4, ICON_FRECCIA_SU, ICON_FRECCIA_GIU);

	connect(this, SIGNAL(sxClick()), this, SIGNAL(backClick()));
	connect(this, SIGNAL(dxClick()), this, SIGNAL(forwardClick()));
	connect(this, SIGNAL(csxClick()), this, SIGNAL(upClick()));
	connect(this, SIGNAL(cdxClick()), this, SIGNAL(downClick()));

	connect(this, SIGNAL(sxPressed()), this, SIGNAL(backPress()));
	connect(this, SIGNAL(dxPressed()), this, SIGNAL(forwardPress()));
	connect(this, SIGNAL(csxPressed()), this, SIGNAL(upPress()));
	connect(this, SIGNAL(cdxPressed()), this, SIGNAL(downPress()));

	connect(this, SIGNAL(sxReleased()), this, SIGNAL(backRelease()));
	connect(this, SIGNAL(dxReleased()), this, SIGNAL(forwardRelease()));
	connect(this, SIGNAL(csxReleased()), this, SIGNAL(upRelease()));
	connect(this, SIGNAL(cdxReleased()), this, SIGNAL(downRelease()));

	connect(this, SIGNAL(sxPressed()), this, SLOT(handleBackPress()));
	connect(this, SIGNAL(sxReleased()), this, SLOT(handleBackRelease()));
	connect(&press_timer, SIGNAL(timeout()), SIGNAL(longBackPress()));

	original_dx_button = dxButton;
	Draw();
}

void bannFrecce::handleBackPress()
{
	press_timer.setSingleShot(true);
	press_timer.start(3000);
}

void bannFrecce::handleBackRelease()
{
	press_timer.stop();
}

void bannFrecce::addCdxButton()
{
	addItem(BUT4, banner_width/2+(banner_width/4-BUTFRECCE_DIM_X)/2,
		(banner_height-BUTFRECCE_DIM_Y)/2, BUTFRECCE_DIM_X, BUTFRECCE_DIM_Y);
}
void bannFrecce::setCdxIcon(const QString &image)
{
	// if I had to explain why this work, you wouldn't believe it...
	// ...
	// Ok, I'll explain, as I already know that the next time it will be MY duty to
	// fix this if something goes wrong (as Murphy already demonstrated).
	// It is a combination of banner::attivo (default = 0), addItem(BUT4) and
	// banner::Draw(), which looks for cdxButton (BUT4) AND Icon[3]
	SetIcons(3, image);
}

void bannFrecce::showCdxButton()
{
	cdxButton->show();
}

void bannFrecce::hideCdxButton()
{
	cdxButton->hide();
}

void bannFrecce::Draw()
{
	if (dxButton != original_dx_button)
		drawAllButRightButton();
	else
		BannerOld::Draw();
}

void bannFrecce::setCustomButton(BtButton *btn)
{
	if (dxButton != original_dx_button)
	{
		dxButton->setParent(dx_button_parent);
		dxButton->move(dx_button_coord.topLeft());
	}

	if (btn)
	{
		dx_button_parent = btn->parentWidget();
		// Custom buttons may not have the correct size expected by bannFrecce, so we need to resize it
		btn->setGeometry(dx_button_coord.left(), dx_button_coord.top(), dx_button_coord.width(), dx_button_coord.height());
		// The custom button is created with no parent, so we need to set bannFrecce as the new parent
		btn->setParent(this);
		btn->move(dx_button_coord.topLeft());
		btn->show();
		dxButton = btn;
	}
	else
		dxButton = original_dx_button;
}

bannFrecce::~bannFrecce()
{
	// This istruction is required to avoid destroying of dxButton when the
	// navbar is destroyed.
	if (dxButton != original_dx_button)
	{
		dxButton->setParent(dx_button_parent);
		dxButton->move(dx_button_coord.topLeft());
	}
}
