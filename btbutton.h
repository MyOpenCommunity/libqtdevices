/* 
 * BTouch - Graphical User Interface to control MyHome System
 *
 * Copyright (C) 2010 BTicino S.p.A.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#ifndef BTBUTTON_H
#define BTBUTTON_H

#include <QPushButton>


class BtButton : public QPushButton
{
Q_OBJECT
public:
	enum IconFlag
	{
		NO_FLAG,                    // empty flag
		LOAD_PRESSED_ICON,          // load pressed icon in setIcon
	};
	BtButton(QWidget *parent = 0);
	BtButton(QString icon_path, QWidget *parent = 0);

	/**
	 * Loads an icon into the button.
	 * \param icon_path The full path of the icon to be loaded
	 * \param f Flags defined in IconFlag. If LOAD_PRESSED_ICON, call setPressedIcon
	 * with parameter icon_path + "p".
	 */
	void setImage(const QString &icon_path, IconFlag f = LOAD_PRESSED_ICON);
	void setPressedImage(const QString &pressed_icon);

	void setPressedPixmap(const QPixmap &p);
	void setPixmap(const QPixmap &p);

	virtual void enable();
	virtual void disable();

protected:
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual bool event(QEvent *e);

	// The sizeHint method is required to obtain a layout management that work fine.
	virtual QSize sizeHint() const;

	// Helper function to load the pressed image for an icon; if not present,
	// returns a null QPixmap
	QPixmap loadPressedImage(const QString &icon_path);

	/// The pixmap to show when the button is in normal state
	QPixmap pixmap;

	/// The pixmap to show when the button is down
	QPixmap pressed_pixmap;

private:
	bool is_enabled;

	void initButton();
};

#endif // BTBUTTON_H
