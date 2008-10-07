/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
** BtMain.cpp
**
**Apertura pagina iniziale e associazione tasti-sottomenù
**
****************************************************************/

#ifndef BTMAIN_H
#define BTMAIN_H

#include "btlabel.h"

#include <common_functions.h>

#include <qwidget.h>

class sottoMenu;
class diffSonora;
class diffmulti;
class antintrusione;
class ThermalMenu;
class versio;
class homePage;
class Calibrate;
class genPage;
class Client;
class tastiera;
class ScreenSaver;
class SupervisionMenu;

/*!
  \class BtMain
  \brief This is a class used to manage the starting sequence, call the xml handler that builds the objects configured, shunt signals and controlling screen-saver and special page displaying (such as configuration page, colored pages and so on).

  This is the widget used for screen saver but has the main aim to connect the various class passing signal from one to the others. This class supervise the freezing (baclight off and uman inteface disabled) and the starting sequence also deciding if there's need of a new calibration.
  \author Davide
  \date lug 2005
*/

class  BtMain : public QWidget
{
Q_OBJECT
public:
	BtMain(QWidget *parent=0, const char *name=0,QApplication*a=NULL);
	~BtMain();
	homePage * Home, *specPage;
	sottoMenu *illumino,*scenari,*videocitofonia,*carichi,*imposta,*automazioni,*scenari_evoluti;
	diffmulti *dm;
	ThermalMenu *termo;
	diffSonora *difSon;
	antintrusione* antintr;
	SupervisionMenu *supervisione;
	Client * client_richieste;
	Client * client_comandi;
	Client *  client_monitor;
	versio* datiGen;
	void ResetTimer();
signals:
/*!
  \brief Emitted to (de)freeze the console
*/
	void freeze(bool);
	void resettimer();

private slots:
	void hom();
	void myMain();
	void init();
	void gesScrSav();
	void freezed(bool);
	void setPwd(bool,char*);
	void testPwd(char*);
	void testFiles();
	void svegl(bool);
	void waitBeforeInit();

public slots:
	void startCalib();
	void endCalib();

private:
	QTimer *tempo1;
	QTimer *tempo2;
	QWidget *pagDefault;
	char pwd[10];
	bool pwdOn,svegliaIsOn,alreadyCalibrated;
	tastiera *tasti;
	bool event_unfreeze;
	unsigned char firstTime,bloccato;
	bool pd_shown;
	genPage *screen;
	unsigned char tiposcreen;
	unsigned long tiempo_ev;
	unsigned long tiempo_last_ev;
	bool calibrating;
	Calibrate* calib;
	ScreenSaver *screensaver;

	// TODO: rimuovere questi metodi qt3!
	void setPaletteBackgroundPixmap(const QPixmap &pixmap);
	void setBackgroundColor(const QColor &c);
};

#endif// BTMAIN_H
