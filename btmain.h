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
#include <QHash>

class sottoMenu;
class SoundDiffusion;
class MultiSoundDiff;
class Antintrusion;
class ThermalMenu;
class Version;
class homePage;
class Calibrate;
class genPage;
class Client;
class Keypad;
class ScreenSaver;
class SupervisionMenu;
class SpecialPage;
class Page;

class QPixmap;
class QString;

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
friend class PageContainer;
public:
	BtMain(QWidget *parent=0);
	~BtMain();
	Client *client_richieste;
	Client *client_comandi;
	Client *client_monitor;

	void resetTimer();
	/// Freeze or unfreeze the application
	void freeze(bool);
	/// Set on/off the sveglia status
	void svegl(bool);

	void setPwd(bool, QString);

private slots:
	void hom();
	void init();
	void gesScrSav();
	void testPwd();
	void testFiles();
	void waitBeforeInit();
	void monitorReady();

public slots:
	void startCalib();
	void endCalib();

protected:
	virtual bool eventFilter(QObject *obj, QEvent *ev);

public:
	// TODO: vedere se ci puo' evitare di rendere questi membri pubblici!
	SoundDiffusion *difSon;
	MultiSoundDiff *dm;
	Version *version;

private:
	QHash<int, Page*> page_list;

	/// build the page related to id if exists and return it, otherwise return 0
	Page *getPage(int id);

	homePage *Home;
	Page *pagDefault;

	QTimer *tempo1;
	QTimer *tempo2;
	QString pwd;
	bool pwdOn,svegliaIsOn,alreadyCalibrated;
	Keypad *tasti;
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
	bool loadStyleSheet(QString filename);

signals:
	void resettimer();
	void freezed(bool);
};

namespace bt_global { extern BtMain *btmain; }

#endif// BTMAIN_H
