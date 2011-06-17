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


#ifndef BANNER_H
#define BANNER_H


#define MAX_NUM_ICON    46
#define MAX_PRESS_ICON  5

#include "device.h"

#include <QWidget>


class BtButton;
class ClientWriter;
class Page;
class BannerContent;
class ScrollingLabel;


class QPixmap;
class QTimer;
class QLabel;


/*!
	\brief A rectangular widget that can be used to present the status of a \ref device,
	to interact with it and to navigate to other Page%s.
*/
class banner : public QWidget
{
friend class BannerContent;
Q_OBJECT
public:
	banner(QWidget *parent);
	virtual ~banner() {}
	/*!
		\brief Sets the Id of the object controlled by the banner.
		\warning Do not use in new code, this method will be removed.
	*/
	void setId(int);

	/*!
		\brief Retrieves the Id of the object controlled by the banner.
		\warning Do not use in new code, this method will be removed.
	*/
	int getId();

	/*!
		\brief Sets the serial number of the banner.
		\warning Do not use in new code, this method will be removed.
	*/
	virtual void setSerNum(int);

	/*!
		\brief Retrieves the serial number of the banner.
		\warning Do not use in new code, this method will be removed.
	*/
	int getSerNum();

	/*!
		\brief Set the clients used to send frames.
	*/
	static void setClients(ClientWriter *command, ClientWriter *request);

	/*!
		\brief Init the banner.
		\warning The method should not be used, if you use a device, initialize it instead.
	*/
	virtual void inizializza(bool forza=false) {}

	/*!
		\brief Draw the content of the banner.
		\warning Do not use in new code, this method will be removed.
	*/
	virtual void Draw() {}
	/*!
		\brief Set the text of the banner.
		\warning Do not use in new code, this method will be removed.
	*/
	virtual void setText(const QString &) {}

public slots:
	void connectionUp();
	void connectionDown();

signals:
	/*!
		\brief Emitted when any of the linked pages is closed.
	*/
	void pageClosed();

protected slots:
	virtual void valueReceived(const DeviceValues &values_list) {}

protected:
	// Width and height of the banner. Used for the sizeHint method!
	int banner_width;
	int banner_height;

	// The sizeHint method is required to have a layout management that work fine.
	// Note that this sizeHint is ok for banner with a standard dimension, banner
	// bigger or smaller should be re-define this method.
	virtual QSize sizeHint() const;

	// A global way to send frames/init requests. Do not use these directly, prefer using
	// devices specific methods, unless you have to send frames without reading responses.
	void sendFrame(QString frame) const;
	void sendInit(QString frame) const;

	void setOpenserverConnection(device *dev);

private:
	int id;
	int serNum;

	static ClientWriter *client_request;
	static ClientWriter *client_command;
};


class BannerNew : public banner
{
Q_OBJECT
public:
	BannerNew(QWidget *parent) : banner(parent) { }
	virtual void Draw() { }
	virtual void inizializza(bool forza=false);

protected:
	ScrollingLabel *createTextLabel(const QRect &size, Qt::Alignment align, const QFont &font);
	ScrollingLabel *createTextLabel(Qt::Alignment align, const QFont &font);
	void connectButtonToPage(BtButton *b, Page *p);
	virtual void hideEvent(QHideEvent *event);
	bool initButton(BtButton *btn, const QString &icon);
	bool initLabel(ScrollingLabel *lbl, const QString &text, const QFont &font);

private:
	QVector<Page *> linked_pages;
};


#endif // BANNER_H
