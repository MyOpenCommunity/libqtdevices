/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** banntermo.h
 **
 **Pagina termoregolazione
 **
 ****************************************************************/

#ifndef BANNTERMO_H
#define BANNTERMO_H

#include "banner.h"
#include <qwidget.h>
#include "btlabel.h"
#include <qpixmap.h>
#include "buttons_bar.h"

/*!
 *
 * \class bannTermo
 * \brief This is a class that describes a banner thought to describe a thermoregulation zone.
 *
 * It occupies an etire page and has two buttons a text an icon eredithed from banner and other text areas to describe 
 * other data tipical of this banner.
 * \author Davide
 * \date lug 2005
 */

class bannTermo : public banner
{
Q_OBJECT
public:
	/**
	 * Which type of device this object controls.
	 */
	enum devtype_t
	{
		THERMO_99_ZONES,
		THERMO_99_ZONES_FANCOIL,
		THERMO_4_ZONES,
		THERMO_4_ZONES_FANCOIL,
		SINGLE_PROBE,
		EXT_SINGLE_PROBE,
	};

	bannTermo( QWidget *, const char *, QColor, devtype_t devtype );
	// void setIcons(const char *IconaMeno=0, const char *IconaPiu=0, const char * IconaMan, const char * IconaAuto);
	void Draw();

	// andrebbero resi privati e fare metodi per leggerli e scriverli
	// anche se bannTermo e termoPage sono strettamente legati
	// perché bannTermo è usato solo da termoPage!!

	char    temp[10];
	char    setpoint[10];
	uchar   val_imp, isOff, isAntigelo;
	BtLabel *tempImp;

	/// Fancoil banner. It is created only if needed
	ButtonsBar *fancoil_buttons;	
private:
	devtype_t devtype;
	BtLabel *texts[7];	
	BtLabel *sondoffanti;
	BtLabel *tempMis;
	QColor  secondForeground;
};


#define BUTMENPLUS_DIM_X	 	60
#define BUTMENPLUS_DIM_Y	 	60
#define BUTAUTOMAN_DIM_X	 	60
#define BUTAUTOMAN_DIM_Y	 	60
#define DESCR_DIM_Y		 	20
#define TEMPMIS_Y		 	80
#define OFFSET_Y		 	40

#endif //BANNTERMO_H
