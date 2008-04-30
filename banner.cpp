/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 ** banner.cpp
 **
 **Implementazione di un banner generico
 **
 ****************************************************************/

#include <qpixmap.h>
#include <qfile.h>
#include <qtimer.h>
#include <qapplication.h> //qapp

#include "banner.h"
#include "btbutton.h"
#include "main.h"
#include "btlabel.h"
#include "genericfunz.h"
#include "openclient.h"
#include "fontmanager.h"

// Init icons_library - Vecchio modo con la cache che è un membro statico di banner
// IconDispatcher  banner::icons_library;

banner::banner( QWidget *parent,const char *name )
: QWidget( parent, name )
{
	BannerIcon = NULL;
	BannerText = NULL;
	SecondaryText = NULL;
	sxButton = NULL;
	dxButton = NULL;
	csxButton = NULL;
	cdxButton = NULL;
	for (int idx=0;idx<MAX_NUM_ICON;idx++)
		Icon[idx] = NULL;
	for (int idx=0;idx<4;idx++)
		pressIcon[idx] = NULL;
	periodo = 0;
	numFrame = contFrame = 0;
	value = 0;
	attivo = 0;
	minValue = maxValue = 0;
	step = 1;
	animationTimer=NULL;
	numRighe=NUM_RIGHE;
	memset(chi,'\000',sizeof(chi));
	memset(group,FALSE,sizeof(group));
	pul=FALSE;
	serNum=1;
	stato=0;
}


banner::~banner()
{
	if (BannerIcon)
		delete (BannerIcon);
	if (BannerText)
		delete (BannerText);
	if (SecondaryText)
		delete (SecondaryText);
	if (dxButton)
		delete (dxButton);
	if (csxButton)
	{
		/*       disconnect(csxButton,SIGNAL(clicked()),this,SIGNAL(csxClick()));
			 disconnect(csxButton,SIGNAL(pressed()),this,SIGNAL(csxPressed()));
			 disconnect(csxButton,SIGNAL(released()),this,SIGNAL(csxReleased()));*/
		delete(csxButton);
	}
	if (cdxButton)

		delete (cdxButton);
	if (sxButton)
		delete (sxButton);
	BannerIcon = NULL;
	BannerText = NULL;
	SecondaryText = NULL;
	sxButton = NULL;
	dxButton = NULL;
	csxButton = NULL;
	cdxButton = NULL;
	for (int idx=0;idx<MAX_NUM_ICON;idx++)
		Icon[idx] = NULL;
	for (int idx=0;idx<4;idx++)
		pressIcon[idx] = NULL;
}


void banner::SetTextU( const QString & text )
{
	qtesto = text;
	qtesto.truncate( MAX_PATH*2-1 );
}

void banner::SetSecondaryTextU( const QString & text )
{
	qtestoSecondario = text;
	qtestoSecondario.truncate ( MAX_TEXT_2-1 );
}

QString banner::getPressedIconName( const char *iconname )
{
	/** This method wraps the previous pressIconName function.
	 *  The main fix introduced is to return the NOT-Pressed Icon Name if
	 *  does not exist pressed icon.
	 */
	if (!iconname)
		return NULL;

	char pressIconName[MAX_PATH];
	//getPressName( (char*)iconname, &pressIconName[0], MAX_PATH );
	getPressName( (char*)iconname, &pressIconName[0], sizeof(pressIconName) );
	
	/// If pressIconName file exists, return the press icon name
	/// otherwise the the same name of the NOT PRESSED icon is returned
	if ( pressIconName == NULL || !QFile::exists(pressIconName) )
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

void banner::SetIcons( uchar idIcon, const char *actuallcon )
{
	Icon[idIcon]      = icons_library.getIcon( actuallcon );
	pressIcon[idIcon] = icons_library.getIcon( getPressedIconName(actuallcon) );
}

void banner::SetIcons( const char *actuallcon ,char tipo)
{
	if (tipo==1)
	{	
		Icon[0]      = icons_library.getIcon( actuallcon );
		pressIcon[0] = icons_library.getIcon( getPressedIconName(actuallcon) );
	}
	if (tipo==3)
	{
		Icon[3]      = icons_library.getIcon( actuallcon );
		pressIcon[3] = icons_library.getIcon( getPressedIconName(actuallcon) );
	}
}

void banner::SetIcons( const char *sxIcon , const char *dxIcon)
{
	if (sxIcon)
	{
		Icon[0]      = icons_library.getIcon( sxIcon );
		pressIcon[0] = icons_library.getIcon( getPressedIconName(sxIcon) );
		qDebug("Icon[0] <- %s", sxIcon);
	}

	if (dxIcon)
	{
		Icon[1]      = icons_library.getIcon( dxIcon );
		pressIcon[1] = icons_library.getIcon( getPressedIconName(dxIcon) );
		qDebug("Icon[1] <- %s", dxIcon);
	}
}

void banner::SetIcons( const char *sxIcon , const char *dxIcon,const char *centerIcon)
{
	if (sxIcon)
	{
		Icon[0]      = icons_library.getIcon( sxIcon );
		pressIcon[0] = icons_library.getIcon( getPressedIconName(sxIcon) );
		qDebug("Icon[0] <- %s", sxIcon);
	}

	if (dxIcon)
	{
		Icon[1]      = icons_library.getIcon( dxIcon );
		pressIcon[1] = icons_library.getIcon( getPressedIconName(dxIcon) );
		qDebug("Icon[1] <- %s", dxIcon);
	}

	if (centerIcon)
	{
		Icon[3]      = icons_library.getIcon( centerIcon );
		pressIcon[3] = icons_library.getIcon( getPressedIconName(centerIcon) );
		qDebug("Icon[3] <- %s", centerIcon);
	}

	impostaAttivo(1);
}

void banner::SetIcons( const char *sxIcon, const char *dxIcon, const char*centerActiveIcon, const char*centerInactiveIcon )
{
	if (sxIcon)
	{
		Icon[0]      = icons_library.getIcon( sxIcon );
		pressIcon[0] = icons_library.getIcon( getPressedIconName(sxIcon) );
		qDebug("Icon[0] <- %s", sxIcon);
	}

	if (dxIcon)
	{
		Icon[1]      = icons_library.getIcon( dxIcon );
		pressIcon[1] = icons_library.getIcon( getPressedIconName(dxIcon) );
		qDebug("Icon[1] <- %s", dxIcon);
	}

	if (centerActiveIcon)
	{
		Icon[3]      = icons_library.getIcon( centerActiveIcon );
		pressIcon[3] = icons_library.getIcon( getPressedIconName(centerActiveIcon) );
		qDebug("Icon[3] <- %s", centerActiveIcon);
	}

	if (centerInactiveIcon)
	{
		Icon[2]      = icons_library.getIcon( centerInactiveIcon );
		pressIcon[2] = icons_library.getIcon( getPressedIconName(centerInactiveIcon) );
		qDebug("Icon[2] <- %s", centerInactiveIcon);
	}
}

void banner::SetIcons( const char *sxIcon , const char *dxIcon, const char*centerInactiveIcon, const char*centerUpIcon, const char*centerDownIcon)
{
	if (sxIcon)
	{
		Icon[0]      = icons_library.getIcon( sxIcon );
		pressIcon[0] = icons_library.getIcon( getPressedIconName(sxIcon) );
		qDebug("Icon[0] <- %s", sxIcon);
	}

	if (dxIcon)
	{
		Icon[1]      = icons_library.getIcon( dxIcon );
		pressIcon[1] = icons_library.getIcon( getPressedIconName(dxIcon) );
		qDebug("Icon[1] <- %s", dxIcon);
	}

	if (centerInactiveIcon)
	{
		Icon[2]      = icons_library.getIcon( centerInactiveIcon );
		pressIcon[2] = icons_library.getIcon( getPressedIconName(centerInactiveIcon) );
		qDebug("Icon[2] <- %s", centerInactiveIcon);
	}

	if (centerUpIcon)
	{
		Icon[3]      = icons_library.getIcon( centerUpIcon );
		pressIcon[3] = icons_library.getIcon( getPressedIconName(centerUpIcon) );
		qDebug("Icon[3] <- %s", centerUpIcon);
	}

	if (centerDownIcon)
	{
		Icon[4]      = icons_library.getIcon( centerDownIcon );
		pressIcon[4] = icons_library.getIcon( getPressedIconName(centerDownIcon) );
		qDebug("Icon[4] <- %s", centerDownIcon);
	}
}


void banner::SetIcons( const char *sxIcon , const char *dxIcon,const char*centerActiveIcon,const char*centerInactiveIcon,int period, int number)
{
	if (sxIcon)
	{
		Icon[0]      = icons_library.getIcon( sxIcon );
		pressIcon[0] = icons_library.getIcon( getPressedIconName(sxIcon) );
		qDebug("Icon[0] <- %s", sxIcon);
	}

	if (dxIcon)
	{
		Icon[1]      = icons_library.getIcon( dxIcon );
		pressIcon[1] = icons_library.getIcon( getPressedIconName(dxIcon) );
		qDebug("Icon[1] <- %s", dxIcon);
	}

	if (centerInactiveIcon)
	{
		Icon[2]      = icons_library.getIcon( centerInactiveIcon );
		pressIcon[2] = icons_library.getIcon( getPressedIconName(centerInactiveIcon) );
		qDebug("Icon[2] <- %s", centerInactiveIcon);
	}

	if  ( (centerActiveIcon) && (number) )
	{
		#if 0
		char pressIconName[MAX_PATH];
		for (uchar idx=1;idx<=number;idx++)
		{
			memset(pressIconName,'\000',sizeof(pressIconName));

			strncpy(pressIconName,centerActiveIcon,strstr(centerActiveIcon,".")-centerActiveIcon);
			strcat(pressIconName,QString::number(idx));
			strcat(pressIconName,strstr(centerActiveIcon,"."));

			if (!Icon[2+idx])
				Icon[2+idx] = icons_library.getIcon(pressIconName, "PNG", Qt::ThresholdDither | Qt::DitherMode_Mask | Qt::AvoidDither);
		}
		#endif
		for (int i = 1; i <= number; i++)
		{
			//QString root_of_name = QString(centerActiveIcon).section(".", 0);
			QString root_of_name = getNameRoot(centerActiveIcon, ".png");
			QString name = QString("%1%2.png").arg( root_of_name ).arg( i );
			Icon[2+i] = icons_library.getIcon(name, "PNG", Qt::ThresholdDither | Qt::DitherMode_Mask | Qt::AvoidDither);
		}
			

			
	}
	else if (centerActiveIcon)
	{
		Icon[3]      = icons_library.getIcon( centerActiveIcon );
		pressIcon[3] = icons_library.getIcon( getPressedIconName(centerActiveIcon) );
		qDebug("Icon[3] <- %s", centerActiveIcon);
	}

	periodo=period;
	numFrame=number;
}

void banner::SetIcons( const char *sxIcon , const char *dxIcon,const char*centerActiveIcon,const char*centerInactiveIcon,char inactiveLevel)
{
	SetIcons( sxIcon , dxIcon, centerActiveIcon, centerInactiveIcon, (const char*)NULL, inactiveLevel );
}

void banner::SetIcons( const char *sxIcon , const char *dxIcon,const char*centerActiveIcon,const char*centerInactiveIcon, const char*breakIcon, char inactiveLevel)
{
	//char nomeFile[MAX_PATH];

	if (sxIcon)
	{
		Icon[0]      = icons_library.getIcon( sxIcon );
		pressIcon[0] = icons_library.getIcon( getPressedIconName(sxIcon) );
		qDebug("Icon[0] <- %s", sxIcon);
		/*
		if (!Icon[0])
			Icon[0] = icons_library.getIcon(sxIcon);

		getPressName((char*)sxIcon, &nomeFile[0],sizeof(nomeFile));

		if (QFile::exists(nomeFile) && !pressIcon[0])
			pressIcon[0] = icons_library.getIcon(nomeFile);
		*/
	}

	if (dxIcon)
	{
		Icon[1]      = icons_library.getIcon( dxIcon );
		pressIcon[1] = icons_library.getIcon( getPressedIconName(dxIcon) );
		qDebug("Icon[1] <- %s", dxIcon);
		
		/*
		getPressName((char*)dxIcon, &nomeFile[0],sizeof(nomeFile));

		if (!Icon[1])
			Icon[1] = icons_library.getIcon(dxIcon);

		if (QFile::exists(nomeFile) && !pressIcon[1])
			pressIcon[1] = icons_library.getIcon(nomeFile);
		*/
	}

	
	/* OLD WAY
	memset(nomeFile,'\000',sizeof(nomeFile));
	strncpy(nomeFile,centerInactiveIcon,strstr(centerInactiveIcon,".")-centerInactiveIcon);
	strcat(nomeFile,"sxl0");
	strcat(nomeFile,strstr(centerInactiveIcon,"."));

	if (!Icon[2])
		Icon[2] = icons_library.getIcon(nomeFile);
	qDebug("Icon[2] <- %s", nomeFile);
	nomeFile[strstr(nomeFile,".")-nomeFile-4]='d';

	if (!Icon[3])
		Icon[3] = icons_library.getIcon(nomeFile);
	qDebug("Icon[3] <- %s", nomeFile);
	*/
	
	// Load base Icon that can be *sxl0 or *dxl0
	//FIXME FIXME FIXME (anche sopra)
	QString inactive_root_of_name = getNameRoot(centerInactiveIcon, ".png");
	QString active_root_of_name   = getNameRoot(centerActiveIcon, ".png");
	QString nomeFile;

	qDebug("________________________________________________________________________________");
	qDebug(inactive_root_of_name);
	qDebug(active_root_of_name);
	qDebug("________________________________________________________________________________");

	// Set first 2 icons from inactive root
	Icon[2] = icons_library.getIcon( QString("%1sxl0.png").arg(inactive_root_of_name) );
	Icon[3] = icons_library.getIcon( QString("%1dxl0.png").arg(inactive_root_of_name) );
	
	qDebug("New Icon[2] <- "+QString("%1sxl0.png").arg(inactive_root_of_name));
	qDebug("New Icon[3] <- "+QString("%1dxl0.png").arg(inactive_root_of_name));
	/*
	char idy=0;
	char suff[10];

	for (char idx=minValue;idx<=maxValue;idx+=step,idy++)
	{
		memset(nomeFile,'\000',sizeof(nomeFile));
		strncpy(nomeFile,centerActiveIcon,strstr(centerActiveIcon,".")-centerActiveIcon);
		sprintf(&suff[0],"sxl%d",idx);
		strcat(nomeFile,&suff[0]);
		strcat(nomeFile,strstr(centerActiveIcon,"."));

		if (!Icon[4+idy*2])
			Icon[4+idy*2] = icons_library.getIcon(nomeFile);
		//ok
		qDebug("Lev = %d, Icon[%d] <- %s", idx, 4+idy*2, nomeFile);

		if (inactiveLevel)
		{
			memset(nomeFile,'\000',sizeof(nomeFile));
			strncpy(nomeFile,centerInactiveIcon,strstr(centerInactiveIcon,".")-centerInactiveIcon);
			sprintf(&suff[0],"sxl%d",idx);
			strcat(nomeFile,&suff[0]);
			strcat(nomeFile,strstr(centerInactiveIcon,"."));

			if (!Icon[22+idy*2])
				Icon[22+idy*2] = icons_library.getIcon(nomeFile);

			qDebug("Icon[%d] <- %s", 22+idy*2, nomeFile);
		}
	}
	*/
	for (int i = minValue, y = 0; i <= maxValue; i+=step, y++)
	{
		nomeFile = QString("%1sxl%2.png").arg(active_root_of_name).arg(i);
		Icon[4+y*2] = icons_library.getIcon(nomeFile);
		qDebug("New Icon[%d] <- %s", 4+y*2, nomeFile.ascii());
		if (inactiveLevel)
		{
			nomeFile = QString("%1sxl%2.png").arg(inactive_root_of_name).arg(i);
			Icon[22+y*2] = icons_library.getIcon(nomeFile);
			qDebug("New Icon[%d] <- %s", 22+y*2, nomeFile.ascii());
		}
	}

	
	/*
	for (char idx=minValue,idy=0;idx<=maxValue;idx+=step,idy++)
	{
		memset(nomeFile,'\000',sizeof(nomeFile));
		strncpy(nomeFile,centerActiveIcon,strstr(centerActiveIcon,".")-centerActiveIcon);
		sprintf(&suff[0],"dxl%d",idx);
		strcat(nomeFile,&suff[0]);
		strcat(nomeFile,strstr(centerActiveIcon,"."));

		if (!Icon[5+idy*2])
			Icon[5+idy*2] = icons_library.getIcon(nomeFile);

		qDebug("Lev = %d, Icon[%d] <- %s", idx, 5+idy*2, nomeFile);

		if (inactiveLevel)
		{
			memset(nomeFile,'\000',sizeof(nomeFile));
			strncpy(nomeFile,centerInactiveIcon,strstr(centerInactiveIcon,".")-centerInactiveIcon);
			sprintf(&suff[0],"dxl%d",idx);
			strcat(nomeFile,&suff[0]);
			strcat(nomeFile,strstr(centerInactiveIcon,"."));

			if (!Icon[23+idy*2])
				Icon[23+idy*2] = icons_library.getIcon(nomeFile);

			qDebug("Icon[%d] <- %s", 23+idy*2, nomeFile);
		}
	}
	*/
	for (int i = minValue, y = 0; i <= maxValue; i+=step, y++)
	{
		nomeFile = QString("%1dxl%2.png").arg(active_root_of_name).arg(i);
		Icon[5+y*2] = icons_library.getIcon(nomeFile);
		qDebug("New Icon[%d] <- %s", 5+y*2, nomeFile.ascii());
		if (inactiveLevel)
		{
			nomeFile = QString("%1dxl%2.png").arg(inactive_root_of_name).arg(i);
			Icon[23+y*2] = icons_library.getIcon(nomeFile);
			qDebug("New Icon[%d] <- %s", 23+y*2, nomeFile.ascii());
		}
	}
	
	if (breakIcon)
	{
		/*
		memset(nomeFile,'\000',sizeof(nomeFile));
		strncpy(nomeFile,breakIcon,strstr(breakIcon,".")-breakIcon);
		strcat(nomeFile,"sx");
		strcat(nomeFile,strstr(breakIcon,"."));

		if (!Icon[44])
			Icon[44] = icons_library.getIcon(nomeFile);

		qDebug("Icon[%d] <- %s", 44, nomeFile);
		nomeFile[strstr(nomeFile,".")-nomeFile-2]='d';

		if (!Icon[45])
			Icon[45] = icons_library.getIcon(nomeFile);

		qDebug("Icon[%d] <- %s", 45, nomeFile);
		*/
		//QString break_root_of_name = QString(breakIcon).section( ".", 0);
		QString break_root_of_name = getNameRoot(breakIcon, ".png");
	
		nomeFile = QString("%1sx.png").arg(break_root_of_name);
		Icon[44] = icons_library.getIcon(nomeFile);
		qDebug("New Icon[%d] <- %s", 44, nomeFile.ascii());

		nomeFile = QString("%1dx.png").arg(break_root_of_name);
		Icon[45] = icons_library.getIcon(nomeFile);
		qDebug("New Icon[%d] <- %s", 45, nomeFile.ascii());
		
	}
}


void banner::addItem(char item,int x,int y,int dimX, int dimY)
{
	QWidget * Item=NULL;

	switch (item)
	{
	case BUT1: 
		sxButton = new BtButton(this,"Bottone di sinistra"); 
		connect(sxButton,SIGNAL(clicked()),this,SIGNAL(sxClick()));
		connect(sxButton,SIGNAL(pressed()),this,SIGNAL(sxPressed()));
		connect(sxButton,SIGNAL(released()),this,SIGNAL(sxReleased()));
		Item = sxButton;
		break;
	case BUT2: 
		dxButton = new BtButton(this,"Bottone di destra");
		connect(dxButton,SIGNAL(clicked()),this,SIGNAL(dxClick()));
		connect(dxButton,SIGNAL(pressed()),this,SIGNAL(dxPressed()));
		connect(dxButton,SIGNAL(released()),this,SIGNAL(dxReleased()));
		Item = dxButton;
		break;
	case TEXT: 
		BannerText = new BtLabel(this,"Testo"); Item = BannerText; 
		break;
	case ICON:
		BannerIcon = new BtLabel(this,"Icona"); Item = BannerIcon; 
		break;
	case ICON2: 
		BannerIcon2 = new BtLabel(this, "Icona2"); 
		Item = BannerIcon2; 
		break;
	case BUT3:
		csxButton = new BtButton(this,"Bottone di centrosinistra");  
		connect(csxButton,SIGNAL(clicked()),this,SIGNAL(csxClick()));
		connect(csxButton,SIGNAL(pressed()),this,SIGNAL(csxPressed()));
		connect(csxButton,SIGNAL(released()),this,SIGNAL(csxReleased()));
		Item = csxButton;
		break;
	case BUT4: 
		cdxButton = new BtButton(this,"Bottone di centrodestra");
		connect(cdxButton,SIGNAL(clicked()),this,SIGNAL(cdxClick()));
		connect(cdxButton,SIGNAL(pressed()),this,SIGNAL(cdxPressed()));
		connect(cdxButton,SIGNAL(released()),this,SIGNAL(cdxReleased()));
		Item = cdxButton;
		break;
	case TEXT2: 
		SecondaryText = new BtLabel(this,"Testo secondario"); Item = SecondaryText;
		break;
	}
	Item->setGeometry(x,y,dimX,dimY);
	//   Item->setPaletteBackgroundColor( QColor :: QColor(BG_R,BG_G,BG_B));
	/*   if ((parentWidget(FALSE)  ->parentWidget(FALSE) ))
	     {
	     Item -> setPaletteBackgroundColor( (parentWidget(FALSE)  -> parentWidget(FALSE) ) -> backgroundColor());
	     Item -> setPaletteForegroundColor( (parentWidget(FALSE)  -> parentWidget(FALSE) ) -> foregroundColor());
	     }*/
}

void banner::nascondi(char item)
{
	switch (item){
		case BUT1: if(sxButton)
				   sxButton->hide();
			   break;
		case BUT2: if(dxButton)
				   dxButton->hide();
			   break;
		case TEXT: if(BannerText)
				   BannerText->hide();
			   break;
		case ICON: if(BannerIcon)
				   BannerIcon->hide();
			   break;
		case BUT3: if(csxButton )
				   csxButton->hide();
			   break;
		case BUT4: if(cdxButton)
				   cdxButton->hide();
			   break;
		case TEXT2: if (SecondaryText)
				    SecondaryText->hide();
			    break;
	}
}
void banner::mostra(char item)
{
	switch (item){
		case BUT1: if(sxButton)
				   sxButton->show();
			   break;
		case BUT2: if(dxButton)
				   dxButton->show();
			   break;
		case TEXT: if(BannerText)
				   BannerText->show();
			   break;
		case ICON: if(BannerIcon)
				   BannerIcon->show();
			   break;
		case BUT3: if(csxButton )
				   csxButton->show();
			   break;
		case BUT4: if(cdxButton)
				   cdxButton->show();
			   break;
		case TEXT2: if (SecondaryText)
				    SecondaryText->show();
			    break;
	}
}   

void banner::Draw()
{
	qDebug("banner::Draw(), attivo = %d, value = %d", attivo, value);
	if ( (sxButton) && (Icon[0]) )
	{
		sxButton->setPixmap(*Icon[0]);
		if (pressIcon[0])
			sxButton->setPressedPixmap(*pressIcon[0]);
	}

	if ( (dxButton) && (Icon[1]) )
	{
		dxButton->setPixmap(*Icon[1]);
		if (pressIcon[1])
			dxButton->setPressedPixmap(*pressIcon[1]);
	}
	if (minValue==maxValue)
	{
		QPixmap *pntIcon=NULL;

		if  (attivo==1)
			pntIcon=Icon[3+contFrame];
		if  (attivo==2)
		{
			pntIcon=Icon[4+contFrame];
		}
		else if (!attivo)
			pntIcon=Icon[2];
		if ( (pntIcon) && (BannerIcon) )
		{
			BannerIcon->repaint();
			BannerIcon->setPixmap(*pntIcon);
			BannerIcon->repaint();
		}

		if ( (Icon[2]) && (csxButton) )
		{
			csxButton->setPixmap(*Icon[2]);
			if (pressIcon[2])
				csxButton->setPressedPixmap(*pressIcon[2]);
		}

		if ( (cdxButton) && (Icon[3]) )
		{
			cdxButton->setPixmap(*Icon[3]);
			if (pressIcon[3])
				cdxButton->setPressedPixmap(*pressIcon[3]);
		}
		qApp-> flush();
	}
	else
	{
		if (attivo==1)
		{
			if ( (Icon[4+((value-step)/step)*2]) && (csxButton) )
			{
				csxButton->setPixmap(*Icon[4+((value-step)/step)*2]);
				qDebug("* Icon[%d]", 4+((value-step)/step)*2);
			}
			if ( (cdxButton) && (Icon[5+((value-step)/step)*2]) )
			{
				cdxButton->setPixmap(*Icon[5+((value-step)/step)*2]);
				qDebug("** Icon[%d]", 5+((value-step)/step)*2);
			}
		}
		else if (attivo==0)
		{
			if (Icon[22])
			{
				if ( (Icon[22+(value-step)/step*2]) && (csxButton) )
				{
					csxButton->setPixmap(*Icon[22+((value-step)/step)*2]);			
					qDebug("*** Icon[%d]", 22+((value-step)/step)*2);
				}

				if ( (cdxButton) && (Icon[23+((value-step)/step)*2]) )
				{
					cdxButton->setPixmap(*Icon[23+((value-step)/step)*2]);    				
					qDebug("**** Icon[%d]", 23+((value-step)/step)*2);
				}
			}	  	
			else
			{
				if ( (Icon[2]) && (csxButton) )
				{
					csxButton->setPixmap(*Icon[2]);	
					qDebug("***** Icon[%d]", 2);
				}

				if ( (cdxButton) && (Icon[3]) )
				{
					cdxButton->setPixmap(*Icon[3]); 
					qDebug("****** Icon[%d]", 3);
				}
			}
		}
		else if (attivo==2)
		{
			if ( (Icon[44]) && (csxButton) )
			{
				csxButton->setPixmap(*Icon[44]);		    
				qDebug("******* Icon[%d]", 44);
			}

			if ( (cdxButton) && (Icon[45]) )
			{
				cdxButton->setPixmap(*Icon[45]);    
				qDebug("******* Icon[%d]", 45);
			}
		}

	}

	/*TODO
	  icona animata -> armare timer per cambiare centerActiveIcon
	  qui devo copiare period e number e in draw attivare il timer*/
	if ( (periodo) && (numFrame) )
	{	   
		if (!animationTimer)
		{
			animationTimer = new QTimer(this,"clock");
			connect(animationTimer,SIGNAL(timeout()),this,SLOT(animate()));
		}
		if ( !(animationTimer->isActive() ) && (attivo))
			animationTimer->start(periodo);
		/* connect(animationTimer,SIGNAL(timeout()),this,SLOT(animate()));*/
	}



	if (BannerText)
	{ 
		QFont aFont;
		FontManager::instance()->getFont( font_banner_BannerText, aFont );
		BannerText->setAlignment(AlignHCenter|AlignVCenter);
		BannerText->setFont( aFont );
		BannerText->setText(qtesto);
		//     qDebug("TESTO: %s", testo);
	}
	if (SecondaryText)
	{	
		QFont aFont;
		FontManager::instance()->getFont( font_banner_SecondaryText, aFont );
		SecondaryText->setAlignment(AlignHCenter|AlignVCenter);
		SecondaryText->setFont( aFont );
		SecondaryText->setText(qtestoSecondario);
	}
}

void banner::impostaAttivo(char Attivo)
{
	attivo=Attivo;
	if ( (animationTimer) && (!Attivo) )
	{
		animationTimer->stop();
		qDebug("KILLanimationTimer");   
	}
}

void banner::setAddress(char* indirizzo)
{
	strncpy (address,indirizzo,sizeof(address));
	//  address=new char (*indirizzo);
	//    address=indirizzo;
}

void banner::gestFrame(char*){}


void banner::setBGColor(int r, int g, int b)
{	
	setBGColor( QColor :: QColor(r,g,b));    
}
void banner::setFGColor(int r, int g, int b)
{
	setFGColor(QColor :: QColor(r,g,b));
}

void banner::setBGColor(QColor c)
{
	setPaletteBackgroundColor(c);
	if (BannerIcon )
		BannerIcon ->setPaletteBackgroundColor(c) ;
	if (BannerText)
		BannerText->setPaletteBackgroundColor(c);
	if (SecondaryText)
		SecondaryText->setPaletteBackgroundColor(c);
	if (sxButton)
		sxButton->setPaletteBackgroundColor(c);
	if (dxButton)
		dxButton->setPaletteBackgroundColor(c);
	if (csxButton)
		csxButton->setPaletteBackgroundColor(c);
	if (cdxButton)
		cdxButton->setPaletteBackgroundColor(c);  
}
void banner::setFGColor(QColor c)	
{
	setPaletteForegroundColor(c);
	if (BannerIcon )
		BannerIcon ->setPaletteForegroundColor(c) ;
	if (BannerText)
		BannerText->setPaletteForegroundColor(c);
	if (SecondaryText)
		SecondaryText->setPaletteForegroundColor(c);
	if (sxButton)
		sxButton->setPaletteForegroundColor(c);
	if (dxButton)
		dxButton->setPaletteForegroundColor(c);
	if (csxButton)
		csxButton->setPaletteForegroundColor(c);
	if (cdxButton)
		cdxButton->setPaletteForegroundColor(c);  
}





void banner::setValue(char val)
{
	if ( (val>=minValue) && (val<=maxValue) )
		value=val;
}
void banner::aumValue()
{
	if (value<maxValue)
		value+=step;
}
void banner::decValue()
{
	if (value>minValue)
		value-=step;
}

void banner::setMaxValue(char val)
{
	maxValue=val;
}
void banner::setMinValue(char val)
{
	minValue=val;
}
void banner::setRange(char minval,char maxval)
{
	maxValue=maxval;
	minValue=minval;
}

void banner::setStep(char s)
{
	step = s;
}

unsigned char banner::isActive()
{
	return attivo;
	/*    if (attivo)
	      return(TRUE);
	      return(FALSE);*/
}

char banner::getValue()
{
	return(value);
}

void banner::animate()
{
	contFrame++;
	if (contFrame>=numFrame)
		contFrame=0;
	Draw();
}

void banner::setAnimationParams(int per ,int num)
{
	periodo=per;
	numFrame=num;
}

void banner::getAnimationParams(int& per, int& num)
{
	per = periodo;
	num = numFrame;
}

void banner::setNumRighe( uchar n )
{
	numRighe=n;
}

char* banner::getAddress( )
{
	return (&address[0]);
}

void banner::setChi(char* indirizzo)
{
	strncpy(&chi[0],indirizzo,sizeof(chi));
}
char* banner::getChi( )
{
	return (&chi[0]);
}
void banner::setGroup(bool* gr)
{
	qDebug("%s setted GROUP",getAddress());
	for (char idx=0;idx<9;idx++)
	{
		group[idx]=gr[idx];
	}
}
bool* banner::getGroup( )
{
	return (&group[0]);
}
void banner::setPul( )
{
	pul=TRUE;
	qDebug("%s setted PUL",getAddress());
}
bool banner::getPul( )
{
	return(pul);
}

bool banner::isForMe(openwebnet * m)
{
	if( strcmp( m->Extract_chi(), "1") )
		 return false ;
	if( ! strcmp( m->Extract_dove(), getAddress()) )
		 return true;
	// BAH
	return ( ! getPul() && ((!strcmp(m->Extract_dove(),"0")) ||
			((strlen(m->Extract_dove())==1) && 
			(!strncmp(m->Extract_dove(), getAddress(), 1)) ) || 
			((!strncmp(m->Extract_dove(),"#",1)) && 
			*(getGroup()+(atoi(m->Extract_dove()+1))-1))));
}

void banner:: inizializza(bool forza){}

//char* banner::getChi(){return(NULL);}
void  banner::rispStato(char*){}



void banner::hide(){QWidget::hide(); }
void banner::show(){QWidget::show();}
void banner::openAckRx(void)
{
	qDebug("openAckRx()");
}
void banner::openNakRx(void)
{
	qDebug("openNakRx()");
}
void banner::setSerNum(int s){serNum=s;}
int banner::getSerNum(){return(serNum);}
char banner::getId(){return(id);}
void banner::setId(char i){id=i;}
unsigned char banner::getState(){return (stato); }
char* banner::getManIcon(){return NULL;}
char* banner::getAutoIcon(){return NULL;}

void banner::ambChanged(char *, bool, void *) { };

void banner::parentChanged(QWidget *newParent) { };
void banner::grandadChanged(QWidget *newGrandad) { };
void banner::addAmb(char *) {};
