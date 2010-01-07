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
	BtButton(QWidget *parent=0);

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
	// Set the button as a 'on-off button'. This means that the button is almost
	// a toggle button, but instead of changes status when the button is pressed
	// down, changes its status manually, calling the 'setStatus' method.
	// In this case, you can retrieve the current status using the 'getStatus'
	// method.
	void setOnOff();

	virtual void enable();
	virtual void disable();

	bool getStatus();

public slots:
	void setStatus(bool st);

protected:
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);

	// The sizeHint method is required to obtain a layout management that work fine.
	virtual QSize sizeHint() const;

	/// The pixmap to show when the button is in normal state
	QPixmap pixmap;

private:
	bool is_enabled;

	/// The pixmap to show when the button is down, cheched or on.
	QPixmap pressed_pixmap;

	bool is_on_off; // a flag that mark if the button is a on-off button
	bool current_status; // the current status of a on off button.

	bool isToggle();
};

#endif // BTBUTTON_H
