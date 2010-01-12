#include "enabling_button.h"
#include "generic_functions.h" //getBostikName
#include "icondispatcher.h" // icons_cache


EnablingButton::EnablingButton(QWidget *parent) : BtButton(parent)
{
	setStatus(OFF);
}

void EnablingButton::setDisabledImage(const QString &path)
{
	disabled_pixmap = *bt_global::icons_cache.getIcon(path);
}

void EnablingButton::setStatus(EnablingButton::Status st)
{
	status = st;
	switch (status)
	{
	case DISABLED:
		setIcon(disabled_pixmap);
		disable();
		break;
	case ON:
		setIcon(pressed_pixmap);
		enable();
		break;
	case OFF:
	default:
		setIcon(pixmap);
		enable();
		break;
	}
}


