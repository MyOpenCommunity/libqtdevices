#include "btbutton.h"
#include "main.h"
#include "generic_functions.h" // beep

#include <QDebug>
#include <QStyleOption>
#include <QStylePainter>


BtButton::BtButton(QWidget *parent) : QPushButton(parent)
{
	setStyleSheet(QString("border:0px;"));
	setFocusPolicy(Qt::NoFocus);
	// ?? TODO: sistemare, ci sono problemi.. verificare a cosa serviva in buttons_bar e sveglia..
	// setCheckable(true);
}

void BtButton::setImage(const QString &icon_path, IconFlag f)
{
	pixmap = *icons_library.getIcon(icon_path);

	if (f == LOAD_PRESSED_ICON)
		setPressedImage(getPressName(icon_path));
}

void BtButton::setPressedImage(const QString &pressed_icon)
{
	pressed_pixmap = *icons_library.getIcon(pressed_icon);
}

void BtButton::setPressedPixmap(const QPixmap &p)
{
	pressed_pixmap = p;
}

void BtButton::setPixmap(const QPixmap &p)
{
	pixmap = p;
}

void BtButton::paintEvent(QPaintEvent *event)
{
	QPixmap *p = &pixmap;

	if (!pressed_pixmap.isNull() && (isDown() || isChecked()))
	{
		//qDebug() << "Is down:" << isDown() << " Is checked:" << isChecked();
		p = &pressed_pixmap;
	}

	setIcon(*p);
	setIconSize(p->size());
	resize(p->size());

	if (isDown())
		beep();

	// The drawing of BtButton is reimplemented here to prevent the "graying"
	// of the image when the button is in the disabled state.
	QStylePainter painter(this);
	QStyleOptionButton option;
	initStyleOption(&option);

	option.state |= QStyle::State_Enabled; // Prevent the "graying"
	painter.drawControl(QStyle::CE_PushButton, option);
}
