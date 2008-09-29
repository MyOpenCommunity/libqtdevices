/*!
 * \file
 * <!--
 * Copyright 2008 MAC S.r.l. (http://www.mac-italia.com/)
 * All rights reserved.
 * -->
 *
 * \brief  Stop & go related classes
 *
 *  TODO: detailed description (optional) 
 *
 * \author Lucio Macellari
 */


#include "stopngo.h"
#include "genericfunz.h"
#include "fontmanager.h"
#include "main.h"

#include <qlcdnumber.h>
#include <qcursor.h>
#include <qtimer.h>


#define TITLE_BAR_HEIGHT 30
#define TITLE_BAR_INDENT 5
#define BUT_DIM	60
#define BUT_POS_X 0
#define BUT_POS_Y (MAX_HEIGHT-BUT_DIM-10)
#define STATUS_ICON_WIDTH	120
#define STATUS_ICON_HEIGHT 60
#define FIRST_ROW_Y TITLE_BAR_HEIGHT
#define SECOND_ROW_Y (TITLE_BAR_HEIGHT+STATUS_ICON_HEIGHT+8)
#define THIRD_ROW_Y (TITLE_BAR_HEIGHT+STATUS_ICON_HEIGHT+10+BUT_DIM+20)
#define LABELS_HEIGHT 20
#define LABELS_WIDTH (MAX_WIDTH/2)

#define STATUS_BIT_OPEN_CLOSE         0x0001
#define STATUS_BIT_DIFF_FAILURE       0x0002
#define STATUS_BIT_LOCKED             0x0004
#define STATUS_BIT_CC                 0x0008
#define STATUS_BIT_EARTH_FAIL         0x0010
#define STATUS_BIT_VMAX               0x0020
#define STATUS_BIT_AUTOTEST_DISABLED  0x0040
#define STATUS_BIT_AUTOREARM_DISABLED 0x0080
#define STATUS_BIT_VERIFY_DISABLED    0x0100

#define STOPNGO_WARNING_MASK 0x003C

#define OPEN_WHO "18"
#define OPEN_WHAT_CLOSE         "21"
#define OPEN_WHAT_OPEN          "22"
#define OPEN_WHAT_AUTOTEST_ON   "23"
#define OPEN_WHAT_AUTOTEST_OFF  "24"
#define OPEN_WHAT_EXEC_AUTOTEST "25"
#define OPEN_WHAT_AUTOARM_ON    "26"
#define OPEN_WHAT_AUTOARM_OFF   "27"
#define OPEN_WHAT_VERIFY_ON     "28"
#define OPEN_WHAT_VERIFY_OFF    "29"

#define OPEN_GRANDEZZA_VV                "21"
#define OPEN_GRANDEZZA_RC                "22"
#define OPEN_GRANDEZZA_RI                "23"
#define OPEN_GRANDEZZA_CONT_RIARMI       "24"
#define OPEN_GRANDEZZA_CONT_APERT        "25"
#define OPEN_GRANDEZZA_CONT_G_CC         "26"
#define OPEN_GRANDEZZA_CONT_G_VT         "27"
#define OPEN_GRANDEZZA_CONT_G_VM         "28"
#define OPEN_GRANDEZZA_CONT_G_BL         "29"
#define OPEN_GRANDEZZA_CONT_AUTOTEST     "210"
#define OPEN_GRANDEZZA_SOGLIE_VER        "211"
#define OPEN_GRANDEZZA_FREQ_AUTOTEST     "212"
#define OPEN_GRANDEZZA_TR_AG             "213"
#define OPEN_GRANDEZZA_TR_EG             "214"
#define OPEN_GRANDEZZA_CONT_PER_AUTOTEST "213"
#define OPEN_GRANDEZZA_STATO             "250"

#define MIN_AUTOTEST_FREQ 1
#define MAX_AUTOTEST_FREQ 180
#define AUTOTEST_SEND_TIMEOUT 2000


/*==================================================================================================
	StopngoItem class definition
==================================================================================================*/

StopngoItem::StopngoItem(int id, int cid, QString descr, QString where)
{
	this->id = id;
	this->cid = cid;
	this->descr = descr;
	this->where = where;
}

StopngoItem::StopngoItem()
{
	this->id = 0;
	this->cid = 0;
	this->descr = "";
	this->where = "0";
}


StopngoItem::~StopngoItem()
{
}


/*==================================================================================================
	BannPulsDynIcon class definition
==================================================================================================*/

BannPulsDynIcon::BannPulsDynIcon(QWidget *parent, const char *name) : bannPuls(parent, name)
{
}


BannPulsDynIcon::~BannPulsDynIcon()
{
}


void BannPulsDynIcon::status_changed(QPtrList<device_status> sl)
{
	stat_var curr_stat(stat_var::INFO_MCI);

	bool aggiorna = false;
	qDebug("supervisionMenu::status_changed()");
	device_status_mci *ds;                                                       //Scan the list
  	for (ds = (device_status_mci*)sl.first(); ds; ds = (device_status_mci*)sl.next())
	{
		if (ds->get_type() == device_status::SUPERVISION_MCI)
		{
			qDebug("Supervision status variation");
			ds->read(device_status_mci::MCI_STATUS_INDEX, curr_stat);
			int statusBmp = curr_stat.get_val();
			qDebug("status = %d", statusBmp);
			if (!(statusBmp & STATUS_BIT_OPEN_CLOSE))      //Closed
			{
				qDebug("Supervision: Change banner icon to CLOSED");
				SetIcons(ICON, ICON_STOPNGO_CHIUSO);
			}
			else                                           //Opened
			{
				if (statusBmp & STATUS_BIT_LOCKED)           //Open because of lock
				{
					qDebug("Supervision: Change banner icon to LOCKED");
					SetIcons(ICON, ICON_STOPNGO_BLOCCO);
				}
				else if (statusBmp & STATUS_BIT_CC)          //Open because of short circuit between L and N
				{
					qDebug("Supervision: Change banner icon to short circuit");
					SetIcons(ICON, ICON_STOPNGO_CORTOCIRCUITO);
				}
				else if (statusBmp & STATUS_BIT_EARTH_FAIL)  //Open because of earth failure
				{
					qDebug("Supervision: Change banner icon to EARTH FAILURE");
					SetIcons(ICON, ICON_STOPNGO_GUASTO_TERRA);
				}
				else if (statusBmp & STATUS_BIT_VMAX)        //Open because of out of Vmax
				{
					qDebug("Supervision: Change banner icon to VMAX");
					SetIcons(ICON, ICON_STOPNGO_SOVRATENSIONE);
				}
				else
				{
					qDebug("Supervision: Change banner icon to OPENED");
					SetIcons(ICON, ICON_STOPNGO_APERTO);
				}
			}
			aggiorna = true;
		}
		else
			qDebug("device status of unknown type (%d)", ds->get_type());
	}

	if (aggiorna)
		Draw();
}

/*==================================================================================================
	StopngoPage class definition
==================================================================================================*/

StopngoPage::StopngoPage(QWidget *parent, const char *name, QString where, int id, QString pageTitle) : QWidget(parent, name)
{
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
	setCursor(QCursor(blankCursor));
	showFullScreen();
#endif
	setGeometry(0, 0, MAX_WIDTH, MAX_HEIGHT);
	setFixedSize(QSize(MAX_WIDTH, MAX_HEIGHT));

	this->where = where;
	this->id = id;
	this->pageTitle = pageTitle;

	statusBmp    = 0;
	autotestFreq = 0;

	freqSendTimer = NULL;
	labelAutoArm = labelVerify = NULL;

	onBut       	= NULL;
	offBut      	= NULL;
	verifyBut   	= NULL;
	autotestBut 	= NULL;
	minusBut    	= NULL;
	plusBut     	= NULL;
	freqLcdNumber = NULL;

	AddItems();
}

StopngoPage::~StopngoPage()
{
	delete titleBar;
	delete okBut;
	delete statusIcon;
	delete autoArmBut;
	if (onBut)
		delete onBut;
	if (offBut)
		delete offBut;
	if (verifyBut)
		delete verifyBut;
	if (autotestBut)
		delete autotestBut;
	if (minusBut)
		delete minusBut;
	if (plusBut)
		delete plusBut;
	if (freqLcdNumber)
		delete freqLcdNumber;
	if (freqSendTimer)
		delete freqSendTimer;
	if (labelAutoArm)
		delete labelAutoArm;
	if (labelVerify)
		delete labelVerify;
}

void StopngoPage::AddItems()
{
	QFont aFont, tFont;
	FontManager::instance()->getFont(font_stopngo_page_labels, aFont);
	FontManager::instance()->getFont(font_stopngo_page_title, tFont);

	//Draw the title bar
	titleBar = new BtLabel(this, "Title Bar");
	titleBar->setGeometry(0, 0, MAX_WIDTH, TITLE_BAR_HEIGHT);
	titleBar->setFrameStyle(QFrame::NoFrame);
	titleBar->setIndent(TITLE_BAR_INDENT);
	titleBar->setFont(tFont);
	titleBar->setAlignment(AlignCenter|AlignTop);
	titleBar->setText(pageTitle);
	titleBar->show();

	//Draw the exit button
	okBut = new BtButton(this, "Exit button");
	okBut->setGeometry(BUT_POS_X, BUT_POS_Y, BUT_DIM, BUT_DIM);
	SetButtonIcon(okBut, ICON_FRECCIA_SX);
	connect(okBut, SIGNAL(clicked()), this, SIGNAL(Closed()));
	okBut->show();
	
	//Draw the status icon
	statusIcon = new BtLabel(this, "Status icon");
	statusIcon->setGeometry((MAX_WIDTH-STATUS_ICON_WIDTH)/2, FIRST_ROW_Y, STATUS_ICON_WIDTH, STATUS_ICON_HEIGHT);
	statusIcon->setFrameStyle(QFrame::NoFrame);
	SetStatusIcon(ICON_STOPNGO_CHIUSO);
	statusIcon->show();

	int armButX    = BUT_DIM/2;
	int armLabelW  = LABELS_WIDTH;
	int secondRowY = SECOND_ROW_Y;
	switch (id)
	{
	case STOPNGO:
		armButX = (MAX_WIDTH-BUT_DIM)/2;
		armLabelW  = MAX_WIDTH;
		secondRowY += 20;
		break;

	case STOPNGO_PLUS:
		secondRowY += 10;
		//Draw the ON button
		onBut = new BtButton(this, "On button");
		onBut->setGeometry(MAX_WIDTH-BUT_DIM, FIRST_ROW_Y, BUT_DIM, BUT_DIM);
		SetButtonIcon(onBut, ICON_ON);
		connect(onBut, SIGNAL(clicked()), this, SLOT(OnClick()));
		onBut->show();
		//Draw the OFF button
		offBut = new BtButton(this, "Off button");
		offBut->setGeometry(0, FIRST_ROW_Y, BUT_DIM, BUT_DIM);
		SetButtonIcon(offBut, ICON_OFF);
		connect(offBut, SIGNAL(clicked()), this, SLOT(OffClick()));
		offBut->show();
		//Draw the verify button label
		labelVerify = new BtLabel(this, "Verify");
		labelVerify->setGeometry(MAX_WIDTH/2, secondRowY+BUT_DIM, LABELS_WIDTH, LABELS_HEIGHT);
		labelVerify->setFrameStyle(QFrame::NoFrame);
		labelVerify->setFont(aFont);
		labelVerify->setAlignment(AlignCenter|AlignTop);
		labelVerify->setText(tr("Test"));
		labelVerify->show();
		//Draw the VERIFY button
		verifyBut = new BtButton(this, "Verify button");
		verifyBut->setGeometry(MAX_WIDTH-BUT_DIM-(BUT_DIM/2), secondRowY, BUT_DIM, BUT_DIM);
		SetButtonIcon(verifyBut, ICON_STOPNGO_A_VERIFICA_IMPIANTO);
		connect(verifyBut, SIGNAL(clicked()), this, SLOT(VerifyClick()));
		verifyBut->show();
		break;

	case STOPNGO_BTEST:
		//Draw the Autotest button label
		labelVerify = new BtLabel(this, "Autotest");
		labelVerify->setGeometry(MAX_WIDTH/2, secondRowY+BUT_DIM, LABELS_WIDTH, LABELS_HEIGHT);
		labelVerify->setFrameStyle(QFrame::NoFrame);
		labelVerify->setFont(aFont);
		labelVerify->setAlignment(AlignCenter|AlignTop);
		labelVerify->setText(tr("Self-test"));
		labelVerify->show();
		//Draw the AUTOTEST button
		autotestBut = new BtButton(this, "Autotest button");
		autotestBut->setGeometry(MAX_WIDTH-BUT_DIM-(BUT_DIM/2), secondRowY, BUT_DIM, BUT_DIM);
		SetButtonIcon(autotestBut, ICON_STOPNGO_A_AUTOTEST);
		connect(autotestBut, SIGNAL(clicked()), this, SLOT(AutotestClick()));
		autotestBut->show();
		//Draw the MINUS button
		minusBut = new BtButton(this, "Minus button");
		minusBut->setGeometry(0, THIRD_ROW_Y, BUT_DIM, BUT_DIM);
		minusBut->setAutoRepeat(true);
		SetButtonIcon(minusBut, ICON_MENO);
		connect(minusBut, SIGNAL(clicked()), this, SLOT(MinusClick()));
			connect(minusBut, SIGNAL(released()), this, SLOT(LoadTimer()));
		minusBut->show();
		//Draw the PLUS button
		plusBut = new BtButton(this, "Plus button");
		plusBut->setGeometry(MAX_WIDTH-BUT_DIM, THIRD_ROW_Y, BUT_DIM, BUT_DIM);
		plusBut->setAutoRepeat(true);
		SetButtonIcon(plusBut, ICON_PIU);
		connect(plusBut, SIGNAL(clicked()), this, SLOT(PlusClick()));
		connect(plusBut, SIGNAL(released()), this, SLOT(LoadTimer()));
		plusBut->show();
		  //Draw LCD number
		freqLcdNumber = new QLCDNumber(3, this, "autotest freq");
		freqLcdNumber->setGeometry(BUT_DIM, THIRD_ROW_Y, MAX_WIDTH-(BUT_DIM*2), BUT_DIM);
		freqLcdNumber->setFrameStyle(QFrame::Plain);
		freqLcdNumber->setLineWidth(0);
		freqLcdNumber->setSegmentStyle(QLCDNumber::Flat);
		freqLcdNumber->display("000");
		freqLcdNumber->show();
		break;
	}

	//Draw the auto arm button label
	labelAutoArm = new BtLabel(this, "Rearm");
	labelAutoArm->setGeometry(0, secondRowY+BUT_DIM, armLabelW, LABELS_HEIGHT);
	labelAutoArm->setFrameStyle(QFrame::NoFrame);
	labelAutoArm->setFont(aFont);
	labelAutoArm->setAlignment(AlignCenter|AlignTop);
	labelAutoArm->setText(tr("Self-recluser"));
	labelAutoArm->show();
	//Draw the auto arm button
	autoArmBut = new BtButton(this, "Arm button");
	autoArmBut->setGeometry(armButX, secondRowY, BUT_DIM, BUT_DIM);
	SetButtonIcon(autoArmBut, ICON_STOPNGO_A_RIARMO);
	connect(autoArmBut, SIGNAL(clicked()), this, SLOT(AutoArmClick()));
	autoArmBut->show();
}

void StopngoPage::setBGColor(int r, int g, int b)
{
	setBGColor(QColor::QColor(r,g,b));
}

void StopngoPage::setFGColor(int r, int g, int b)
{
	setFGColor(QColor::QColor(r,g,b));
}

void StopngoPage::setBGColor(QColor c)
{
	setPaletteBackgroundColor(c);

	titleBar->setPaletteBackgroundColor(c);
	okBut->setPaletteBackgroundColor(c);
	autoArmBut->setPaletteBackgroundColor(c);
	statusIcon->setPaletteBackgroundColor(c);

	if (onBut)
		onBut->setPaletteBackgroundColor(c);
	if (offBut)
		offBut->setPaletteBackgroundColor(c);
	if (verifyBut)
		verifyBut->setPaletteBackgroundColor(c);
	if (autotestBut)
		autotestBut->setPaletteBackgroundColor(c);
	if (minusBut)
		minusBut->setPaletteBackgroundColor(c);
	if (plusBut)
		plusBut->setPaletteBackgroundColor(c);
	if (freqLcdNumber)
		freqLcdNumber->setPaletteBackgroundColor(c);
	if (labelAutoArm)
		labelAutoArm->setPaletteBackgroundColor(c);
	if (labelVerify)
		labelVerify->setPaletteBackgroundColor(c);
}

void StopngoPage::setFGColor(QColor c)
{
	setPaletteForegroundColor(c);

	titleBar->setPaletteForegroundColor(c);
	okBut->setPaletteForegroundColor(c);
	autoArmBut->setPaletteForegroundColor(c);
	statusIcon->setPaletteForegroundColor(c);
	if (onBut)
		onBut->setPaletteForegroundColor(c);
	if (offBut)
		offBut->setPaletteForegroundColor(c);
	if (verifyBut)
		verifyBut->setPaletteForegroundColor(c);
	if (autotestBut)
		autotestBut->setPaletteForegroundColor(c);
	if (minusBut)
		minusBut->setPaletteForegroundColor(c);
	if (plusBut)
		plusBut->setPaletteForegroundColor(c);
	if (freqLcdNumber)
		freqLcdNumber->setPaletteForegroundColor(c);
	if (labelAutoArm)
		labelAutoArm->setPaletteForegroundColor(c);
	if (labelVerify)
		labelVerify->setPaletteForegroundColor(c);
}

int StopngoPage::setBGPixmap(char* backImage)
{
	QPixmap Back;
	if (Back.load(backImage))
	{
		setPaletteBackgroundPixmap(Back);
		QPixmap temp;
		temp.resize(BUT_DIM, BUT_DIM);

		bitBlt(&temp, 0, 0, &Back, BUT_POS_X, BUT_POS_Y, BUT_DIM, BUT_DIM);
		okBut->setPaletteBackgroundPixmap(temp);
		
		return 0;
	}
	return 1;
}

void StopngoPage::showPage()
{
	showFullScreen();
}

void StopngoPage::freezed(bool f)
{
	qDebug("StopngoPage::freezed()");
	setDisabled(f);
}

void StopngoPage::status_changed(QPtrList<device_status> sl)
{
	stat_var curr_stat(stat_var::INFO_MCI);
	stat_var curr_freq(stat_var::WORD);

	qDebug("StopngoPage::status_changed()");
	device_status_mci *ds;                                                       //Scan the list
	for (ds = (device_status_mci*)sl.first(); ds; ds = (device_status_mci*)sl.next())
	{
		if (ds->get_type() == device_status::SUPERVISION_MCI)
		{
			qDebug("StopngoPage status variation");
			ds->read(device_status_mci::MCI_STATUS_INDEX, curr_stat);
			int tmpStatus = curr_stat.get_val();
			qDebug("status = %d", tmpStatus);
			
			if (tmpStatus != statusBmp)
			{
				qDebug("StopngoPage - update page status info.");
				statusBmp = tmpStatus;

				// Status icon
				if (!(statusBmp & STATUS_BIT_OPEN_CLOSE)) // Closed
				{
					qDebug("StopngoPage: Change banner icon to CLOSED");
					SetStatusIcon(ICON_STOPNGO_CHIUSO);
					if (onBut)
						onBut->show();
					if (offBut)
						offBut->show();
					if (verifyBut)
					{
						verifyBut->hide();
						labelVerify->hide();
					}
					if (autotestBut)
					{
						autotestBut->show();
						labelVerify->show();
					}
				}
				else  // Opened
				{
					if (statusBmp & STATUS_BIT_LOCKED)  // Open because of lock
					{
						qDebug("StopngoPage: Change banner icon to LOCKED");
						SetStatusIcon(ICON_STOPNGO_BLOCCO);
						if (onBut)
							onBut->hide();
						if (offBut)
							offBut->hide();
					}
					else if (statusBmp & STATUS_BIT_CC) // Open because of short circuit between L and N
					{
						qDebug("StopngoPage: Change banner icon to short circuit");
						SetStatusIcon(ICON_STOPNGO_CORTOCIRCUITO);
						if (onBut)
							onBut->hide();
						if (offBut)
							offBut->hide();
					}
					else if (statusBmp & STATUS_BIT_EARTH_FAIL)  //Open because of earth failure
					{
						qDebug("StopngoPage: Change banner icon to EARTH FAILURE");
						SetStatusIcon(ICON_STOPNGO_GUASTO_TERRA);
						if (onBut)
							onBut->hide();
						if (offBut)
							offBut->hide();
					}
					else if (statusBmp & STATUS_BIT_VMAX) // Open because of out of Vmax
					{
						qDebug("StopngoPage: Change banner icon to VMAX");
						SetStatusIcon(ICON_STOPNGO_SOVRATENSIONE);
						if (onBut)
							onBut->hide();
						if (offBut)
							offBut->hide();
					}
					else
					{
						qDebug("StopngoPage: Change banner icon to OPENED");
						SetStatusIcon(ICON_STOPNGO_APERTO);
						if (onBut)
							onBut->show();
						if (offBut)
							offBut->show();
					}
					if (verifyBut)
					{
						verifyBut->show();
						labelVerify->show();
					}
					if (autotestBut)
					{
						autotestBut->hide();
						labelVerify->hide();
					}
				}

				//Auto-arm button
				if (statusBmp & STATUS_BIT_AUTOREARM_DISABLED) // Auto arm disabled
				{
					qDebug("StopngoPage: Change arm icon to DISABLED");
					SetButtonIcon(autoArmBut, ICON_STOPNGO_D_RIARMO);
				}
				else  // Auto arm enabled
				{
					qDebug("StopngoPage: Change arm icon to ENABLED");
					SetButtonIcon(autoArmBut, ICON_STOPNGO_A_RIARMO);
				}

				//Verify button
				if (id == STOPNGO_PLUS)
				{
					if (statusBmp & STATUS_BIT_VERIFY_DISABLED)  // Verify disabled
					{
						qDebug("StopngoPage: Change verify icon to DISABLED");
						SetButtonIcon(verifyBut, ICON_STOPNGO_D_VERIFICA_IMPIANTO);
					}
					else  // Verify enabled
					{
						qDebug("StopngoPage: Change verify icon to ENABLED");
						SetButtonIcon(verifyBut, ICON_STOPNGO_A_VERIFICA_IMPIANTO);
					}
				}

				//Autotest button
				if (id == STOPNGO_BTEST)
				{
					if (statusBmp & STATUS_BIT_AUTOTEST_DISABLED)  // Autotest disabled
					{
						qDebug("StopngoPage: Change autotest icon to DISABLED");
						SetButtonIcon(autotestBut, ICON_STOPNGO_D_AUTOTEST);
					}
					else  // Autotest enabled
					{
						qDebug("StopngoPage: Change autotest icon to ENABLED");
						SetButtonIcon(autotestBut, ICON_STOPNGO_A_AUTOTEST);
					}
				}
			}

			qDebug("StopngoPage autotest frequency variation");
			ds->read(device_status_mci::MCI_AUTOTEST_FREQ_INDEX, curr_freq);
			int tmpFreq = curr_freq.get_val();
			qDebug("autotest freq = %d", tmpFreq);
			if (tmpFreq > MAX_AUTOTEST_FREQ)
			{
				qDebug("autotest freq too high. Clamp to %d", MAX_AUTOTEST_FREQ);
				tmpFreq = MAX_AUTOTEST_FREQ;
			}
			if (autotestFreq != tmpFreq)
			{
				qDebug("StopngoPage - update page autotest frequency value.");
				autotestFreq = tmpFreq;
				SetFreqValue(autotestFreq);
			}
		}
		else
			qDebug("StopngoPage: device status of unknown type (%d)", ds->get_type());
	}
}

void StopngoPage::SetStatusIcon(const char* iconPath)
{
	if (!iconPath)
		return;
	
	statusIcon->setPixmap(*icons_library.getIcon(iconPath));
}

void StopngoPage::SetButtonIcon(BtButton *btn, const char *icnPath)
{
	if (!btn || !icnPath)
		return;
	
	btn->setPixmap(*icons_library.getIcon(icnPath));
	btn->setPressedPixmap(*icons_library.getIcon(getPressedIconName(icnPath)));
}

void StopngoPage::SetFreqValue(int val)
{
	if (!freqLcdNumber)
		return;
	
	char s[4];
	sprintf(s, "%03d", val);
	freqLcdNumber->display(s);
}

void StopngoPage::LoadTimer()
{
	qDebug("StopngoPage::LoadTimer() load timer to fire open frame");
	if (!freqSendTimer)
	{
		qDebug("StopngoPage::LoadTimer() create timer object.");
		freqSendTimer = new QTimer(this);
		connect(freqSendTimer, SIGNAL(timeout()), this, SLOT(FireFreqFrame()));
	}
	else
	{
		freqSendTimer->stop();
		qDebug("StopngoPage::LoadTimer() stop timer.");
	}
	
	qDebug("StopngoPage::LoadTimer() start timer.");
	freqSendTimer->start(AUTOTEST_SEND_TIMEOUT, true);
}

QString StopngoPage::getPressedIconName(const char *iconname)
{
	/** This method wraps the previous pressIconName function.
	*  The main fix introduced is to return the NOT-Pressed Icon Name if
	*  does not exist pressed icon.
	*/
	if (!iconname)
		return NULL;

	char pressIconName[MAX_PATH];
	getPressName((char*)iconname, &pressIconName[0], sizeof(pressIconName));
	
	/// If pressIconName file exists, return the press icon name
	/// otherwise the the same name of the NOT PRESSED icon is returned
	if (pressIconName == NULL || !QFile::exists(pressIconName))
	{
		qDebug("could not get pressed icon %s, using: %s", pressIconName, iconname);
		return QString(iconname);
	}
	else
	{
		qDebug("got pressed icon: %s", pressIconName);
		return QString(pressIconName);
	}
}

void StopngoPage::FireFreqFrame()
{
	qDebug("StopngoPage::FireFreqFrame() Enter");
	char frameOpen[30];
	sprintf(frameOpen, "*#%s*%s*#%s*%d##", OPEN_WHO, where.ascii(), OPEN_GRANDEZZA_FREQ_AUTOTEST, autotestFreq);
	qDebug("StopngoPage::FireFreqFrame() fire frame: %s", frameOpen);
	emit(sendFrame(frameOpen));
	sprintf(frameOpen, "*#%s*%s*%s##", OPEN_WHO, where.ascii(), OPEN_GRANDEZZA_FREQ_AUTOTEST);
	emit(sendFrame(frameOpen));
}

void StopngoPage::AutoArmClick()
{
	char frameOpen[30];
	sprintf(frameOpen, "*%s*%s*%s##", OPEN_WHO,
		(statusBmp & STATUS_BIT_AUTOREARM_DISABLED) ? OPEN_WHAT_AUTOARM_ON : OPEN_WHAT_AUTOARM_OFF, where.ascii());
	emit(sendFrame(frameOpen));
	sprintf(frameOpen, "*#%s*%s*%s##", OPEN_WHO, where.ascii(), OPEN_GRANDEZZA_STATO);
	emit(sendFrame(frameOpen));
}

void StopngoPage::OnClick()
{
	if ((statusBmp&STOPNGO_WARNING_MASK) || !(statusBmp&STATUS_BIT_OPEN_CLOSE))
		return;
	
	char frameOpen[30];
	sprintf(frameOpen, "*%s*%s*%s##", OPEN_WHO, OPEN_WHAT_CLOSE, where.ascii());
	emit(sendFrame(frameOpen));
}

void StopngoPage::OffClick()
{
	if (statusBmp&STATUS_BIT_OPEN_CLOSE)
		return;
	
	char frameOpen[30];
	sprintf(frameOpen, "*%s*%s*%s##", OPEN_WHO, OPEN_WHAT_OPEN, where.ascii());
	emit(sendFrame(frameOpen));
}

void StopngoPage::VerifyClick()
{
	char frameOpen[30];
	sprintf(frameOpen, "*%s*%s*%s##", OPEN_WHO,
		(statusBmp & STATUS_BIT_VERIFY_DISABLED)?OPEN_WHAT_VERIFY_ON:OPEN_WHAT_VERIFY_OFF, where.ascii());
	emit(sendFrame(frameOpen));
	sprintf(frameOpen, "*#%s*%s*%s##", OPEN_WHO, where.ascii(), OPEN_GRANDEZZA_STATO);
	emit(sendFrame(frameOpen));
}

void StopngoPage::AutotestClick()
{
	char frameOpen[30];
	sprintf(frameOpen, "*%s*%s*%s##", OPEN_WHO,
		(statusBmp & STATUS_BIT_AUTOTEST_DISABLED)?OPEN_WHAT_AUTOTEST_ON:OPEN_WHAT_AUTOTEST_OFF, where.ascii());
	emit(sendFrame(frameOpen));
	sprintf(frameOpen, "*#%s*%s*%s##", OPEN_WHO, where.ascii(), OPEN_GRANDEZZA_STATO);
	emit(sendFrame(frameOpen));
}

void StopngoPage::MinusClick()
{
	if (autotestFreq > MIN_AUTOTEST_FREQ)
		SetFreqValue(--autotestFreq);
}

void StopngoPage::PlusClick()
{
	if (autotestFreq < MAX_AUTOTEST_FREQ)
		SetFreqValue(++autotestFreq);
}



