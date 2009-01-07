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

protected:
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);

	// The sizeHint method is required to obtain a layout management that work fine.
	virtual QSize sizeHint() const;

private:
	/// The pixmap to show when the button is down
	QPixmap pressed_pixmap;
	/// The pixmap to show when the button is in normal state
	QPixmap pixmap;
private slots:
	void toggled(bool checked);
};

#endif // BTBUTTON_H
