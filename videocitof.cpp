#include <qfont.h>
#include <qpixmap.h>
#include <stdlib.h>
#include <qwidget.h>
#include <qcursor.h>
#include <qdatetime.h>

#include <qfile.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "openclient.h"
#include "sottomenu.h"
#include "device.h"
#include "frame_interpreter.h"
#include "device_cache.h"
#include "videocitof.h"
#include "items.h"
#include "genericfunz.h"
#include "btlabel.h"
#include "btbutton.h"

// Call notifier implementation

call_notifier::call_notifier(QWidget *parent, char *name, postoExt *ms, char* _txt1, char* _txt2, char* _txt3) :
  QFrame(parent, name)
{
    qDebug("call_notifier::call_notifier()");
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
    setCursor (QCursor (blankCursor));
#endif  
    QString where;
    my_station = ms;
    if(ms)
	ms->get_where(where);
    else
	// Unknown station, ANY where is ok
	where = "ANY";
    station_dev = btouch_device_cache.get_doorphone_device(where);
    if(!station_dev) {
	qDebug("Bad thing, cannot create device");
	return;
    }
    myTimer = new QTimer(this, "idle timer");
    connect(myTimer, SIGNAL(timeout()), this, SLOT(close()));
    // Pass incoming frames on to device
    connect(this, SIGNAL(frame_available(char *)), 
	    station_dev, SLOT(frame_rx_handler(char *)));
    // Get status changed events
    connect(station_dev, SIGNAL(status_changed(QPtrList<device_status>)),
	    this, SLOT(status_changed(QPtrList<device_status>)));
    station_dev->init();
    SetIcons(_txt1, _txt2, _txt3);
}

void call_notifier::get_where(QString& out)
{
    qDebug("call_notifier::get_where()");
    my_station->get_where(out);
}

void call_notifier::status_changed(QPtrList<device_status> dsl)
{
    // When we get here, we captured a call frame for sure, no need to 
    // read status
    qDebug("call_notifier::status_changed()");
    emit(frame_captured(this));
}

void call_notifier::showFullScreen()
{
    qDebug("call_notifier::showFullScreen()");
    QFrame::showFullScreen();
    myTimer->start(30000, true);
}

// FIXME: direct connection ?
void call_notifier::frame_available_handler(char *f)
{
    qDebug("call_notifier::frame_available_handler()");
    emit(frame_available(f));
}

void call_notifier::stairlight_pressed()
{
    qDebug("call_notifier::stairlight_pressed()");
    if(my_station)
	my_station->stairlight_pressed();
    myTimer->start(30000, true);
}

void call_notifier::stairlight_released()
{
    qDebug("call_notifier::stairlight_released()");
    if(my_station)
	my_station->stairlight_released();
    myTimer->start(30000);
}

void call_notifier::open_door_clicked()
{
    qDebug("call_notifier::open_door_clicked()");
    if(my_station)
	my_station->open_door_clicked();
    myTimer->start(30000);
}

void call_notifier::close()
{
    qDebug("call_notifier::close()");
    myTimer->stop();
    hide();
    emit(closed(this));
}

// Private methods
void call_notifier::SetIcons(char* _txt1, char* _txt2, char* _txt3)
{
    setGeometry(0, 0, MAX_WIDTH, MAX_HEIGHT); 
    setFixedSize(QSize(MAX_WIDTH, MAX_HEIGHT));
    area1_ptr = new BtLabel(this, "Area1");
    area1_ptr->setGeometry(0, (MAX_HEIGHT/4)-2*LABEL_HEIGHT, 
			   MAX_WIDTH, LABEL_HEIGHT);
    area1_ptr->setFont( QFont( DEFAULT_FONT, 20, QFont::Bold ) );
    area1_ptr->setAlignment(AlignHCenter|AlignVCenter);
    QString s; 
    if(my_station)
	my_station->get_descr(s);
    else
	s = _txt1;
    area1_ptr->setText( s );
    if(my_station && my_station->get_light()) {
	area2_but = new BtButton(this, "Area2");
	area2_but->setGeometry(0, (MAX_HEIGHT/2)-BUTTON_DIM, BUTTON_DIM, BUTTON_DIM);
	connect(area2_but, SIGNAL(pressed()), this, 
		SLOT(stairlight_pressed()));
	connect(area2_but, SIGNAL(released()), this, 
		SLOT(stairlight_released()));
	area3_ptr = new BtLabel(this, "Area3");
	area3_ptr->setGeometry(BUTTON_DIM,
			       (MAX_HEIGHT/2)-(BUTTON_DIM/2)-(LABEL_HEIGHT/2),
			       LABEL_WIDTH, LABEL_HEIGHT);
	area3_ptr->setFont( QFont( DEFAULT_FONT, 20, QFont::Bold ) );
	area3_ptr->setAlignment(AlignHCenter|AlignVCenter);
	s = _txt2;
	area3_ptr->setText( s );
    } else {
	area2_but = NULL;
	area3_ptr = NULL;
    }
    if(my_station && my_station->get_key()) {
	area4_but = new BtButton(this, "Area4");
	area4_but->setGeometry(0, ((3*MAX_HEIGHT)/4)-BUTTON_DIM, 
			       BUTTON_DIM, BUTTON_DIM);
	connect(area4_but, SIGNAL(clicked()), this, SLOT(open_door_clicked()));
	area5_ptr = new BtLabel(this, "Area5");
	area5_ptr->setGeometry(BUTTON_DIM,
			       ((3*MAX_HEIGHT)/4)-(BUTTON_DIM/2)-(LABEL_HEIGHT/2),
			       LABEL_WIDTH, LABEL_HEIGHT);
	area5_ptr->setFont( QFont( DEFAULT_FONT, 20, QFont::Bold ) );
	area5_ptr->setAlignment(AlignHCenter|AlignVCenter);
	s = _txt3;
	area5_ptr->setText( s );
    } else {
	area4_but = NULL;
	area5_ptr = NULL;
    }
    area6_but = new BtButton(this, "Area6");
    area6_but->setGeometry(0, MAX_HEIGHT-BUTTON_DIM, BUTTON_DIM, BUTTON_DIM);
    connect(area6_but, SIGNAL(clicked()), this, SLOT(close()));
    SetButtonsIcons();
}

void call_notifier::SetButtonIcon(const char *ic, BtButton *b)
{
    if(!b) return;
    QPixmap* Icon1;
    QPixmap* Icon2;
    Icon1 = new QPixmap();
    char iconName[40];
    getPressName((char *)ic, iconName, sizeof(iconName));
    if(QFile::exists(ic)) {
	Icon1->load(ic);
	if(b)
	    b->setPixmap(*Icon1);
    }
    if(QFile::exists(iconName)) {
	Icon2 = new QPixmap();
	Icon2->load(iconName);
	if(b)
	    b->setPressedPixmap(*Icon2);
    }
    delete Icon1;
    if(Icon2) 
	delete Icon2;
}

void call_notifier::SetButtonsIcons()
{
    QString a2_icon, a4_icon, a6_icon;
    if(!my_station) {
	// FIXME !!, BAH
	a2_icon = IMG_PATH "btnlucescale.png"; 
	a4_icon = IMG_PATH "btnserratura.png";
	a6_icon = IMG_PATH "arrlf.png";
    } else {
	my_station->get_light_icon(a2_icon);
	my_station->get_key_icon(a4_icon);
	my_station->get_close_icon(a6_icon);
    }
    SetButtonIcon(a2_icon.ascii(), area2_but);
    SetButtonIcon(a4_icon.ascii(), area4_but);
    SetButtonIcon(a6_icon.ascii(), area6_but);
}

void call_notifier::setBGColor(int r, int g, int b)
{
    setBGColor( QColor :: QColor(r,g,b)); 
}

void call_notifier::setFGColor(int r, int g, int b)
{
    setFGColor( QColor :: QColor(r,g,b)); 
}

void call_notifier::setBGColor(QColor c)
{	
    qDebug("call_notifier::setBGColor()");
    setPaletteBackgroundColor(c);    
    if(area1_ptr)
	area1_ptr->setPaletteBackgroundColor(c);
    if(area3_ptr)
	area3_ptr->setPaletteBackgroundColor(c);
    if(area5_ptr)
	area5_ptr->setPaletteBackgroundColor(c);
    if(area2_but)
	area2_but->setPaletteBackgroundColor(c);
    if(area4_but)
	area4_but->setPaletteBackgroundColor(c);
    if(area6_but)
	area6_but->setPaletteBackgroundColor(c);
}

void call_notifier::setFGColor(QColor c)
{	
    setPaletteForegroundColor(c);    
    if(area1_ptr)
	area1_ptr->setPaletteForegroundColor(c);
    if(area3_ptr)
	area3_ptr->setPaletteForegroundColor(c);
    if(area5_ptr)
	area5_ptr->setPaletteForegroundColor(c);
    if(area2_but)
	area2_but->setPaletteForegroundColor(c);
    if(area4_but)
	area4_but->setPaletteForegroundColor(c);
    if(area6_but)
	area6_but->setPaletteForegroundColor(c);
}

void call_notifier::freezed(bool f)
{
  qDebug("call_notifier::freezed(%d)", f);

}

// Call notifier manager implementation
call_notifier_manager::call_notifier_manager()
{
    qDebug("call_notifier_manager::call_notifier_manager()");
    unknown_station_notifier = NULL;
    known_station = false;
}

void call_notifier_manager::add_call_notifier(call_notifier *cn)
{
    qDebug("call_notifier_manager::add_call_notifier()");
    //members.append(cn);
    connect(this, SIGNAL(frame_available(char *)),
	    cn, SLOT(frame_available_handler(char *)));
    connect(cn, SIGNAL(frame_captured(call_notifier *)),
	    this, SLOT(frame_captured_handler(call_notifier *)));
    connect(cn, SIGNAL(closed(call_notifier *)), 
	    this, SIGNAL(call_notifier_closed(call_notifier *)));
    connect(this, SIGNAL(freezed(bool)), cn, SLOT(freezed(bool)));
}

void call_notifier_manager::set_unknown_call_notifier(call_notifier *cn)
{
    qDebug("call_notifier_manager::set_unknown_call_notifier");
    unknown_station_notifier = cn;
#if 0
    // Don't pass frames to unknown station notifier via signal
    // call_notifier_manager::gestFrame will do that via direct call
    connect(this, SIGNAL(frame_available(char *)),
	    cn, SLOT(frame_available_handler(char *)));
#endif
    connect(cn, SIGNAL(frame_captured(call_notifier *)),
	    this, SLOT(frame_captured_handler(call_notifier *)));
    connect(cn, SIGNAL(closed(call_notifier *)), 
	    this, SIGNAL(call_notifier_closed(call_notifier *)));
}

void call_notifier_manager::gestFrame(char *f)
{
    qDebug("call_notifier_manager::gestFrame()");
    known_station = false;
    emit(frame_available(f));
    // Has one of the known stations' call_notifier objects captured 
    // this frame ?
    if((!known_station) && unknown_station_notifier) {
	qDebug("forwarding frame to unknown status call notifier");
	unknown_station_notifier->frame_available_handler(f);
    }
}

void call_notifier_manager::frame_captured_handler(call_notifier *cn)
{
    qDebug("frame_captured_handler()");
    known_station = true;
    // A frame has been captured by a call notifier (cn)
    cn->showFullScreen();
    emit(frame_captured(cn));
}
