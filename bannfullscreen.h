/*!
 * \bannfullscreen.h
 * <!--
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief A set of full screen banners
 * Right now there is no logic included.
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */
#ifndef SMFULLSCREEN_H
#define SMFULLSCREEN_H

#include "banner.h"
#include <qlabel.h>
#include <qlayout.h>
#include <qbuttongroup.h>

class BannFullScreen : public banner
{
Q_OBJECT
public:
	// mi manca di sicuro la descrizione della zona e il device da cui prendo le modifiche
	// forse posso spostare il set dei vari *ground in alcuni membri?
	BannFullScreen(QWidget *parent, const char *name = 0);
	virtual void Draw();
	// void callmeBack() call this from sottoMenu to setNavBarMode with the correct icon
	virtual void postDisplay();
	void setSecondForeground(QColor fg2);

protected:
	QVBoxLayout main_layout;
	/// Measured temperature label and string
	QLabel *temp_label;
	QString temp;
	/// Zone description label and string
	QLabel *descr_label;
	QString descr;
	QColor second_fg;
};


class FSBann4zProbe : public BannFullScreen
{
Q_OBJECT
public:
	FSBann4zProbe(QWidget *parent, const char *name = 0);
	virtual void Draw();
	virtual void postDisplay();
protected:
	QString setup_temp;
	QLabel  *setup_temp_label;
	QString setpoint;
	QLabel *setpoint_label;
};

// FIXME: Use decorator !!
// I need the interface but how do I store the decorated object?
//
class FSBann4zFancoil : public FSBann4zProbe
{
Q_OBJECT
public:
	FSBann4zFancoil(QWidget *parent, const char *name = 0);
	virtual void Draw();
	virtual void postDisplay();
private:
	/**
	 * Creates fancoil buttons and loads icons
	 */
	void createFancoilButtons();
	QButtonGroup fancoil_buttons;
	int fancoil_status;
};

enum BannID
{
	fs_nc_probe = 1,                      // not controlled probe
	fs_4z,                                // 4 zones thermal regulator device
	fs_4z_probe,                          // 4 zones controlled probe
	fs_4z_fancoil,                        // 4 zones controlled probe with fancoil
	fs_99z,                               // 99 zones thermal regulator device
	fs_99z_probe,                         // 99 zones controlled probe
	fs_99z_fancoil,                       // 99 zones controlled probe with fancoil
};

class FSBannFactory
{
public:
	static FSBannFactory *getInstance();
	BannFullScreen *getBanner(BannID id, QWidget *parent);
private:
	static FSBannFactory *instance;
	FSBannFactory();
	FSBannFactory(const FSBannFactory &);
	FSBannFactory &operator= (const FSBannFactory&);
};
#endif // SMFULLSCREEN_H
