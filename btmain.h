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

#include <QWidget>

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

class BtMain : public QWidget
{
Q_OBJECT
public:
	BtMain(QWidget *parent=0);
	~BtMain();
	Client *client_richieste;
	Client *client_comandi;
	Client *client_monitor;

	void resetTimer();
	/// Freeze or unfreeze the application
	void freeze(bool);

private slots:
	void hom();
	void init();
	void gesScrSav();
	void setPwd(bool,char*);
	void testPwd(char*);
	void testFiles();
	void svegl(bool);
	void waitBeforeInit();
	void monitorReady();

public slots:
	void startCalib();
	void endCalib();

protected:
	virtual bool eventFilter(QObject *obj, QEvent *ev);

private:
	homePage *Home, *specPage;
	sottoMenu *illumino,*scenari,*videocitofonia,*carichi,*imposta,*automazioni,*scenari_evoluti;
	diffmulti *dm;
	ThermalMenu *termo;
	diffSonora *difSon;
	antintrusione *antintr;
	SupervisionMenu *supervisione;
	versio *datiGen;
	QTimer *tempo1;
	QTimer *tempo2;
	QWidget *pagDefault;
	char pwd[10];
	bool pwdOn,svegliaIsOn,alreadyCalibrated;
	tastiera *tasti;
	bool event_unfreeze;
	bool firstTime, bloccato;
	bool pd_shown;
	genPage *screen;
	unsigned char tiposcreen;
	unsigned long tiempo_ev;
	unsigned long tiempo_last_ev;
	bool calibrating;
	Calibrate *calib;
	ScreenSaver *screensaver;

	// A flag that is set when the client monitor socket is ready
	bool monitor_ready;
	// A flag that is set when the configuration has loaded
	bool config_loaded;

	void myMain();
	/// Load the main configuration
	bool loadConfiguration(QString cfg_file);
	/// Load the skin configuration
	bool loadSkin(QString xml_file);

signals:
	void resettimer();
	void freezed(bool);
};

#endif// BTMAIN_H
