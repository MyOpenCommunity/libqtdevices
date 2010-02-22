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


#ifndef RADIO_H
#define RADIO_H

#include "page.h"

#include <QButtonGroup>

class BtButton;
class bannFrecce;
class QLCDNumber;
class QLabel;


/*!
 * \class radio
 * \brief This class implements the management of the FM tuner specific page.
 *
 */
class  radio : public Page
{
Q_OBJECT
public:
	radio(const QString &amb="");
	/*!
	 * \brief Sets the name of the tuner found in user configuration file
	 */
	void setName(const QString &);
	/*!
	 * \brief Sets the station number of the syntonized station
	 */
	void setStaz(uchar);
	/*!
	 * \brief Draws the page
	 */
	void draw();
	/*!
	 * \brief Sets the frequency of the syntonized station
	 */
	void setFreq(float);
	/*!
	 * \brief Gets the frequency of the syntonized station
	 */
	float getFreq();
	/*!
	 * \brief Sets the RDS message of the syntonized station
	 */
	void setRDS(const QString &);
	/*!
	 * \brief Sets amb. description
	 */
	void setAmbDescr(const QString &);
	/*!
	 * \brief Gets the station number of the syntonized station
	 */
	uchar getStaz();

signals:
	/*!
	 * \brief Emitted when the page is going to be closed
	 */
	void Closed();
	/*!
	 * \brief Emitted when there's a request of an automatic search towards greater frequencies
	 */
	void aumFreqAuto();
	/*!
	 * \brief Emitted when there's a request of an automatic search towards smaller frequencies
	 */
	void decFreqAuto();
	/*!
	 * \brief Emitted when there's a request of a manual search towards greater frequencies 
	 */
	void aumFreqMan();
	/*!
	 *\brief Emitted when there's a request of an manual search towards lesser frequencies
	 */
	void decFreqMan();
	/*!
	 * \brief Emitted to save the station actually tuned to one of the memorized stations
	 */
	void memoFreq(uchar);
	/*!
	 * \brief Emitted to switch to the next station
	 */
	void changeStaz();
	/*!
	 *\brief Emitted to ask the frequency actually tuned
	 */
	void richFreq();
	/*!
	 * \brief Emitted to enter the  tuner details page
	 */
	void entroParam();
	/*!
	 * \brief Emitted to exit the tuner details page
	 */
	void escoParam();

public slots:
	/*!
	 * \brief Shows the tuner details page 
	 */
	virtual void showPage();
	/*!
	 * \brief Changes the state to automatic search
	 */
	void setAuto();
	/*!
	 * \brief Changes the state to manual search
	 */
	void setMan();
	/*!
	 * \brief Save currently tuned station to memory location given by parameter
	 */
	void memo(int memory);

	/*!
	 * \brief Shows the memorization page
	 */
	void cambiaContesto();
	/*!
	 * \brief Hides -memorization page ans show back tuner details page
	 */
	void ripristinaContesto();
	/*!
	 * \brief At the end of a manual search ask the frequency tuned to the tuner to align to the visualized frequency
	 */
	void verTas();

private:
	QWidget *createContent(const QString &amb);
	float frequenza;
	uchar stazione;
	QString qrds;
	char amb[80];
	QString qnome;
	bool manual, wasManual;
	bannFrecce *bannNavigazione;
	BtButton *memoBut,*decBut,*aumBut,*autoBut,*manBut,*cicBut;
	BtButton *unoBut,*dueBut,*treBut,*quatBut,*cinBut,*cancBut;
	QButtonGroup button_group;
	QLabel *rdsLabel, *radioName, *progrText, *ambDescr;
	QLCDNumber *freq;
	QString manual_off, manual_on, auto_off, auto_on;

	/*!
	 * \brief Retrieves if the selected search method is automatic or manual
	 */
	bool isManual();
	/*!
	 * \brief Gets the RDS message of the syntonized station
	 */
	QString * getRDS();

};


#endif // RADIO_H
