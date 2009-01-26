#include "bann3_buttons.h"
#include "main.h" // MAX_WIDTG, MAX_HEIGHT, NUM_RIGHE,..

#define BAN3BUT_BUT_DIM 60
#define BAN3BUT_BUTCEN_DIM 120


bann3But::bann3But(QWidget *parent) : banner(parent)
{
	addItem(BUT1, 0 , 0 , BAN3BUT_BUT_DIM , BAN3BUT_BUT_DIM);
	addItem(BUT2, MAX_WIDTH - BAN3BUT_BUT_DIM , 0 , BAN3BUT_BUT_DIM , BAN3BUT_BUT_DIM);
	addItem(TEXT, 0 , BAN3BUT_BUT_DIM , MAX_WIDTH , MAX_HEIGHT/NUM_RIGHE - BAN3BUT_BUT_DIM);
	addItem(BUT3, MAX_WIDTH/2 - BAN3BUT_BUTCEN_DIM/2, 0 , BAN3BUT_BUTCEN_DIM, BAN3BUT_BUT_DIM);
	connect(this,SIGNAL(csxClick()),this,SIGNAL(centerClick()));
}
