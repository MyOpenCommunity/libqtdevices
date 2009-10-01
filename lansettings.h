/**
 * \file
 * <!--
 * Copyright 2009 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief This file contain the page to show and control network settings.
 *
 * \author Gianni Valdambrini <aleister@develer.com>
 */

#ifndef LAN_SETTINGS_H
#define LAN_SETTINGS_H

#include "page.h"

#include <QVariant>
#include <QFrame>
#include <QHash>

class LanDevice;
class BtButton;
class QGridLayout;
class QDomNode;


/**
 * \class Fram2Column
 *
 * An utility class to show text in 2 column like a table.
 */
class Text2Column : public QFrame
{
Q_OBJECT
public:
	Text2Column();
	void addRow(QString text, Qt::Alignment align=Qt::AlignLeft);
	void addRow(QString label, QString text);
	void setText(int row, QString text);

private:
	QGridLayout *main_layout;
};


/**
 * \class LanSettings
 *
 * This class is a page used to show lan settings and enable activation
 * (disactivation) of lan interface.
 */
class LanSettings : public PageLayout
{
Q_OBJECT
public:
	LanSettings(const QDomNode &config_node);
	virtual void inizializza();

public slots:
	virtual void showPage();

private:
	BtButton *toggle_btn;
	Text2Column *box_text;
	LanDevice *dev;
	// The real status of the lan
	bool lan_status;
	// The status of the lan as stored in the configuration file
	bool saved_status;

private slots:
	void status_changed(const StatusList &status_list);
	void toggleLan();
};

#endif // LAN_SETTINGS_H
