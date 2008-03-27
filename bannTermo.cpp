/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** bannTermo.cpp
 **
 **Pagina termoregolaizone
 **
 ****************************************************************/

#include "banntermo.h"
#include "main.h"
#include "sottomenu.h"
#include <qfont.h>
#include <qpixmap.h>


bannTermo::bannTermo( QWidget *parent, const char *name, QColor SecondForeground, devtype_t _devtype )
	: banner( parent, name )
{
	qDebug("bannTermo::bannTermo --> DEVICE TYPE = %d", devtype);
	
	devtype = _devtype;
	int h = (MAX_HEIGHT-MAX_HEIGHT/NUM_RIGHE-BUTMENPLUS_DIM_Y-DESCR_DIM_Y-TEMPMIS_Y-OFFSET_Y)/5;
	int set_buttons_y = h*3+DESCR_DIM_Y+TEMPMIS_Y;
	//int set_temp_text_y;

	// Create Label: -3 -2 -1 +0 +1 +2 +3
	for (int idx=0;idx<7;idx++)
	{
		texts[idx] = new BtLabel(this, "");
		texts[idx]->setGeometry(idx*MAX_WIDTH/7, 95, MAX_WIDTH/7, OFFSET_Y);
	}

	// Misured Temperature Label
	tempMis = new BtLabel(this,NULL);
	tempMis->setGeometry(0,h+DESCR_DIM_Y,MAX_WIDTH,TEMPMIS_Y);

	// Specific Widgets
	switch (devtype)
	{
	case THERMO_4_ZONES_FANCOIL:
	case THERMO_99_ZONES_FANCOIL:
		// set position for Set Temp Panel
		// set_temp_buttons_y = h*3+DESCR_DIM_Y+TEMPMIS_Y;

		fancoil_buttons = new ButtonsBar(this, 4);
		fancoil_buttons->setGeometry(0, set_buttons_y+OFFSET_Y, MAX_WIDTH, 100);
		QPixmap *icon;
		QPixmap *pressed_icon;

		icon         = icons_library.getIcon( QString("%1%2").arg(IMG_PATH).arg("fancoil1off.png") );
		pressed_icon = icons_library.getIcon( QString("%1%2").arg(IMG_PATH).arg("fancoil1on.png") );
		fancoil_buttons->setButtonIcons(0, *icon, *pressed_icon);

		icon         = icons_library.getIcon( QString("%1%2").arg(IMG_PATH).arg("fancoil2off.png") );
		pressed_icon = icons_library.getIcon( QString("%1%2").arg(IMG_PATH).arg("fancoil2on.png") );
		fancoil_buttons->setButtonIcons(1, *icon, *pressed_icon);

		icon         = icons_library.getIcon( QString("%1%2").arg(IMG_PATH).arg("fancoil3off.png") );
		pressed_icon = icons_library.getIcon( QString("%1%2").arg(IMG_PATH).arg("fancoil3on.png") );
		fancoil_buttons->setButtonIcons(2, *icon, *pressed_icon);

		icon         = icons_library.getIcon( QString("%1%2").arg(IMG_PATH).arg("fancoilAoff.png") );
		pressed_icon = icons_library.getIcon( QString("%1%2").arg(IMG_PATH).arg("fancoilAon.png") );
		fancoil_buttons->setButtonIcons(3, *icon, *pressed_icon);
		fancoil_buttons->setToggleButtons(true);
		// set Style
		fancoil_buttons->setBGColor(tempMis->backgroundColor());
		// connect Fancoil Buttons to the Handler
		break;
	case THERMO_4_ZONES:
	case THERMO_99_ZONES:
		break;

	case SINGLE_PROBE:
	case EXT_SINGLE_PROBE:

	default:
		qDebug("bannTermo::bannTermo(): Unknown devtype_t!");
	}

	// Panel to SET TEMPERATURE (button PLUS, MINUS, Set Temp Label
	addItem( BUT1, 0,                          set_buttons_y, BUTMENPLUS_DIM_X, BUTMENPLUS_DIM_Y );
	addItem( BUT2, MAX_WIDTH-BUTMENPLUS_DIM_X, set_buttons_y, BUTMENPLUS_DIM_X, BUTMENPLUS_DIM_Y );
	addItem( TEXT, 0,                          h,    MAX_WIDTH , DESCR_DIM_Y );
	addItem( ICON, BUTMENPLUS_DIM_X,           set_buttons_y, MAX_WIDTH-2*BUTMENPLUS_DIM_X, BUTMENPLUS_DIM_Y);
	nascondi(ICON);
	// TempImp è la label della Temperatura Impostata
	tempImp = new BtLabel(this,"0.00\272C");
	tempImp->setGeometry(BUTMENPLUS_DIM_X, set_buttons_y, MAX_WIDTH-2*BUTMENPLUS_DIM_X, BUTMENPLUS_DIM_Y);

	sondoffanti = new BtLabel(this, "");
	sondoffanti->setGeometry(0,h+DESCR_DIM_Y+TEMPMIS_Y,MAX_WIDTH,OFFSET_Y);

	qtemp = "-23.5\272C";
	qsetpoint = qtemp;
	secondForeground=QColor(SecondForeground);

	switch (devtype)
	{
	case THERMO_4_ZONES:
	case THERMO_4_ZONES_FANCOIL:
		nascondi(BUT1);
		nascondi(BUT2);
		break;
	case SINGLE_PROBE:
		// Hide PLUS/MINUS buttons
		nascondi(BUT1);
		nascondi(BUT2);
		// Hide label to set Temperature
		tempImp->hide();
		// Hide what ???
		sondoffanti->hide();
		// Hide Temperature Increment Labels
		for (int idx=0;idx<7;idx++)
			texts[idx]->hide();
		break;
	}

	val_imp    = 3;
	isOff      = 0;
	isAntigelo = 0;
}


void bannTermo::Draw()
{
	tempMis -> setFont( QFont( "Times", 40, QFont::Bold ) );
	tempMis -> setAlignment(AlignHCenter|AlignVCenter);
	tempMis -> setText( qtemp );
	tempImp -> setFont( QFont( "Times", 25, QFont::Bold ) );
	tempImp -> setPaletteForegroundColor(secondForeground);
	tempImp -> setAlignment(AlignHCenter|AlignVCenter);
	tempImp -> setText( qsetpoint );
	if (isOff)
	{
		for (int idx=0;idx<7;idx++)
		{
			texts[idx] -> hide();
		}
		sondoffanti -> setAlignment(AlignHCenter|AlignVCenter);
		sondoffanti ->setPaletteForegroundColor(foregroundColor());
		sondoffanti -> setText("OFF"); // FIXME da tradurre?
		sondoffanti -> show();
		sondoffanti ->setPaletteForegroundColor(foregroundColor());
		sondoffanti -> setFont( QFont( "Times", 20, QFont::Bold ) );
	}
	else   if (isAntigelo)
	{
		for (int idx=0;idx<7;idx++)
		{
			texts[idx] -> hide();
		}
		sondoffanti -> setAlignment(AlignHCenter|AlignVCenter);
		sondoffanti ->setPaletteForegroundColor(foregroundColor());
		sondoffanti -> setText("*");
		sondoffanti -> show();   
		sondoffanti ->setPaletteForegroundColor(foregroundColor());
		sondoffanti -> setFont( QFont( "Times", 40, QFont::Bold ) );
	}
	else if (devtype != SINGLE_PROBE && devtype != EXT_SINGLE_PROBE)
	{
		for (int idx=0;idx<7;idx++)
		{
			QString q_app;
			q_app.sprintf("%+d", idx-3);
			texts[idx] -> setText( q_app );
			texts[idx] -> setFont( QFont( "Times", 20, QFont::Bold ) );
			texts[idx] -> setAlignment(AlignHCenter|AlignVCenter);
			texts[idx] -> show();
			if (val_imp==idx)
				texts[idx] ->setPaletteForegroundColor(secondForeground);
			else
				texts[idx] ->setPaletteForegroundColor(foregroundColor());
		}  
		sondoffanti->hide();
	}

	banner::Draw();
}




