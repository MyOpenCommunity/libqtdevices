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

#include <QObject>
#include <QHash>


class SoundDiffusion;
class MultiSoundDiffAlarm;
class Version;
class HomePage;
class Calibrate;
class genPage;
class Client;
class KeypadWindow;
class ScreenSaver;
class Page;
class PageContainer;
class WindowContainer;
class Window;
class TrayBar;

class QPixmap;
class QString;
class QTimer;
class QTime;

/*!
  \class BtMain
  \brief This is a class used to manage the starting sequence, call the xml handler that builds the objects configured, shunt signals and controlling screen-saver and special page displaying (such as configuration page, colored pages and so on).

  This is the widget used for screen saver but has the main aim to connect the various class passing signal from one to the others. This class supervise the freezing (baclight off and uman inteface disabled) and the starting sequence also deciding if there's need of a new calibration.
  \author Davide
  \date lug 2005
*/

class BtMain : public QObject
{
Q_OBJECT
friend Page *getPage(int id);
friend class HomePage;

public:
	BtMain();
	~BtMain();
	Client *client_richieste;
	Client *client_comandi;
	SoundDiffusion *difSon;
	MultiSoundDiffAlarm *dm;
	Version *version;

	void resetTimer();
	/// Freeze or unfreeze the application
	void freeze(bool);
	/// Set on/off the sveglia status
	void svegl(bool);

	void setPwd(bool, QString);

	Window *homeWindow();
	TrayBar *trayBar();
	void showHomePage();

	// stop the screen saver and hide the password keypad but keep the
	// screen frozen if password protection is active
	void makeActiveAndFreeze();

public slots:
	void startCalib();
	void endCalib();

signals:
	void resettimer();
	void freezed(bool);
	void startscreensaver(Page*);

protected:
	virtual bool eventFilter(QObject *obj, QEvent *ev);


private slots:
	void init();
	void gesScrSav();
	void testPwd();
	void testFiles();
	void waitBeforeInit();
	void monitorReady();

private:
	Client *client_monitor;
#if DEBUG
	Client *client_supervisor;
#endif
	QHash<int, Page*> page_list;
	QTime *boot_time;
	HomePage *Home;
	Page *pagDefault;

	QTimer *tempo1;
	QTimer *tempo2;
	QString pwd;
	bool pwdOn,svegliaIsOn,alreadyCalibrated;
	KeypadWindow *tasti;
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
	PageContainer *page_container;
	WindowContainer *window_container;

	// A flag that is set when the client monitor socket is ready
	bool monitor_ready;
	// A flag that is set when the configuration has loaded
	bool config_loaded;

	void myMain();
	/// Load the main configuration
	void loadConfiguration();

	// Load the global configuration (the item in "generale" section of xml config file)
	void loadGlobalConfig();
	// Unroll all the pages until homepage
	void unrollPages();
};

namespace bt_global { extern BtMain *btmain; }

#endif// BTMAIN_H
