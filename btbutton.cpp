#include "btbutton.h"
#include "icondispatcher.h" // bt_global::icons_cache
#include "generic_functions.h" // beep

#include <QDebug>
#include <QFile>


BtButton::BtButton(QWidget *parent) : QPushButton(parent)
{
	setStyleSheet("border:0px;");
	setFocusPolicy(Qt::NoFocus);
	connect(this, SIGNAL(toggled(bool)), SLOT(toggled(bool)));
}

QSize BtButton::sizeHint() const
{
	if (!pixmap.isNull())
		return pixmap.size();
	return QSize();
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
	pixmap = p;
	// Icon is set only the first time; in other cases the pixmap (pressed
	// or normal) is set when there is a status change.
	if (icon().isNull())
	{
		setIcon(pixmap);
		setIconSize(pixmap.size());
	}
}

void BtButton::mousePressEvent(QMouseEvent *event)
{
	// Toggle buttons are managed by toggled slot, that is always called when
	// the button changes its state.
	if (!isCheckable() && !pressed_pixmap.isNull())
		setIcon(pressed_pixmap);

	beep();
	QPushButton::mousePressEvent(event);
}

void BtButton::mouseReleaseEvent(QMouseEvent *event)
{
	// Manages only normal buttons. Toggle buttons are managed by toggled slot.
	if (!isCheckable() && !pressed_pixmap.isNull())
		setIcon(pixmap);

	QPushButton::mouseReleaseEvent(event);
}

void BtButton::toggled(bool checked)
{
	// If the pressed_pixmap doesn't exists, there is nothing to do (because the
	// pixmap is already the normal one)
	if (!pressed_pixmap.isNull())
		setIcon(!checked ? pixmap : pressed_pixmap);
}

