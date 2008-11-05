/*!
 * \brightnesspage.cpp
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */

#include "brightnesspage.h"
#include "main.h" // for ICON_{OK,VUOTO}
#include "btbutton.h"

static brightness_policy_t getBrightnessPolicy(brightness_state_t on_state,
		brightness_state_t off_state, brightness_state_t screensaver_state)
{
	// set the size, we don't need more states
	brightness_policy_t policy(BrightnessControl::NUMBER_OF_STATES);
	policy.push_back(on_state);
	policy.push_back(off_state);
	policy.push_back(screensaver_state);
	return policy;
}

BrightnessPage::BrightnessPage(QWidget *parent) : sottoMenu(parent)
{
	setNavBarMode(10, ICON_OK);
	QColor *bg, *fg, *fg2;
	readExtraConf(&bg, &fg, &fg2);
	setBGColor(*bg);
	setFGColor(*fg);

	BannToggle *off = getBanner(tr("Off"));
	const int off_id = 0;
	buttons.insert(off->getButton(), off_id);
	button_to_policy.insert(off_id, getBrightnessPolicy(
			brightness_state_t(BACKLIGHT_ON, 10),
			brightness_state_t(BACKLIGHT_OFF, 10),
			brightness_state_t(BACKLIGHT_OFF, 10)
			));

	BannToggle *low = getBanner(tr("Low brightness"));
	const int low_id = 1;
	buttons.insert(low->getButton(), low_id);
	button_to_policy.insert(low_id, getBrightnessPolicy(
			brightness_state_t(BACKLIGHT_ON, 10),
			brightness_state_t(BACKLIGHT_ON, 255),
			brightness_state_t(BACKLIGHT_ON, 255)
			));

	BannToggle *high = getBanner(tr("High brightness"));
	const int high_id = 2;
	buttons.insert(high->getButton(), high_id);
	button_to_policy.insert(high_id, getBrightnessPolicy(
			brightness_state_t(BACKLIGHT_ON, 10),
			brightness_state_t(BACKLIGHT_ON, 210),
			brightness_state_t(BACKLIGHT_ON, 210)
			));

	buttons.setExclusive(true);
	connect (this, SIGNAL(goDx()), SLOT(brightnessSelected()));
}

BannToggle *BrightnessPage::getBanner(const QString &banner_text)
{
	BannToggle *bann = new BannToggle(this);
	bann->SetTextU(banner_text);
	elencoBanner.append(bann);
	bann->setFGColor(paletteForegroundColor());
	bann->setBGColor(paletteBackgroundColor());
	return bann;
}

void BrightnessPage::brightnessSelected()
{
	BrightnessControl::setBrightnessPolicy(button_to_policy[buttons.selectedId()]);
	// this will close the window
	emit Closed();
}

BannToggle::BannToggle(sottoMenu *parent, const char *name) : bannOnSx(parent, name)
{
	SetIcons(ICON_VUOTO, 1);
	sxButton->setToggleButton(true);
}

BtButton *BannToggle::getButton()
{
	return sxButton;
}
