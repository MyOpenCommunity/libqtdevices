#include "btbutton.h"
#include "icondispatcher.h" // bt_global::icons_cache
#include "generic_functions.h" // beep

#include <QDebug>
#include <QFile>


BtButton::BtButton(QWidget *parent) : QPushButton(parent)
{
	setStyleSheet("border:0px;");
	setFocusPolicy(Qt::NoFocus);
	is_on_off = false;
	connect(this, SIGNAL(toggled(bool)), SLOT(setStatus(bool)));
}

QSize BtButton::sizeHint() const
{
	if (!pixmap.isNull())
		return pixmap.size();
	return QSize();
}

void BtButton::setOnOff()
{
	is_on_off = true;
}

bool BtButton::isToggle()
{
	return isCheckable() || is_on_off;
}

void BtButton::setImage(const QString &icon_path, IconFlag f)
{
	setPixmap(*bt_global::icons_cache.getIcon(icon_path));

	if (f == LOAD_PRESSED_ICON)
	{
		QString pressed_name = getPressName(icon_path);
		if (QFile::exists(pressed_name))
			setPressedImage(pressed_name);
	}
}

void BtButton::setPressedImage(const QString &pressed_icon)
{
	pressed_pixmap = *bt_global::icons_cache.getIcon(pressed_icon);
}

void BtButton::setPressedPixmap(const QPixmap &p)
{
	pressed_pixmap = p;
}

void BtButton::setPixmap(const QPixmap &p)
{
	// Pixmaps are saved into a cache, so if two pixmap are the same also the addresses
	// are equal.
	bool need_update = (&pixmap != &p);
	pixmap = p;
	// Icon is set only the first time; in other cases the pixmap (pressed
	// or normal) is set when there is a status change.
	if (icon().isNull() || need_update)
	{
		setIcon(pixmap);
		setIconSize(pixmap.size());
	}
}

void BtButton::mousePressEvent(QMouseEvent *event)
{
	// Toggle buttons are managed by toggled slot, that is always called when
	// the button changes its state.
	if (!isToggle() && !pressed_pixmap.isNull())
		setIcon(pressed_pixmap);

	beep();
	QPushButton::mousePressEvent(event);
}

void BtButton::mouseReleaseEvent(QMouseEvent *event)
{
	// Manages only normal buttons. Toggle buttons are managed by toggled slot.
	if (!isToggle() && !pressed_pixmap.isNull())
		setIcon(pixmap);

	QPushButton::mouseReleaseEvent(event);
}

void BtButton::setStatus(bool on)
{
	// If the pressed_pixmap doesn't exists, there is nothing to do (because the
	// pixmap is already the normal one)
	if (!pressed_pixmap.isNull())
		setIcon(!on ? pixmap : pressed_pixmap);
}

