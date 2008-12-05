/****************************************************************
 **
 ** BTicino Touch scren Colori art. H4686
 **
 **xmlconfhandler.cpp
 **
 **classe per il parsing SAX di conf.xml
 **
 ****************************************************************/

#include "xmlconfhandler.h"
#include "main.h"
#include "homepage.h"
#include "sottomenu.h"
#include "impostatime.h"
#include "diffsonora.h"
#include "diffmulti.h"
#include "sveglia.h"
#include "generic_functions.h" // rearmWDT, setOrientation
#include "versio.h"
#include "antintrusion.h"
#include "thermalmenu.h"
#include "btmain.h"
#include "openclient.h"
#include "supervisionmenu.h"
#include "automation.h"
#include "lighting.h"
#include "scenario.h"
#include "videoentryphone.h"
#include "settings.h"

#include <QObject>
#include <QRegExp>
#include <QWidget>
#include <QDebug>
#include <QTimer>

#include <assert.h>


/// banTesti: text utilized during the initialization sequence when the menu is built up
static const char *banTesti[] =
{
	/*    0                            1                           2                         3                    */
	"ATTUAT_AUTOM",               "DIMMER 10",               "ATTUAT_AUTOM_INT",          "VUOTO",
	/*    4                            5                           6                         7                    */
	"SCENARIO",                   "GR_ATT_INT",              "GR_DIMMER",                 "CARICO",
	/*    8                            9                           10                        11                   */
	"ATT_AUT_INT_SIC",            "ATT_AUT_TEMP",            "GR_ATT_INT",                "ATT_AUT_PULS",
	/*    12                           13                          14                        15                   */
	"ATT_VCT_LS",                 "ATT_VCT_SER",             "SET_DATA",                  "VUOTO",
	/*    16                           17                          18                        19                   */
	"SORGENTE_AUX",               "SORG_RADIO",              "AMPLI",                     "GR_AMPLI",
	/*    20                           21                          22                        23                   */
	"SET_SVEGLIA",                "CALIB",                   "TERMO_99Z_PROBE",           "ZONANTI",
	/*    24                           25                          26                        27                   */
	"IMPANTI",                    "SUONO",                   "PROT",                      "VERS",
	/*    28                           29                          30                        31                   */
	"CONTR",                      "MOD_SCEN",                "DATA",                      "TEMP",
	/*    32                           33                          34                        35                   */
	"TIME",                       "ALL",                     "SPECIAL",                   "DIMMER 100",
	/*    36                           37                          38                        39                   */
	"ATT_AUT_TEMP_N",             "ATT_AUT_TEMP_F",          "SCENARIO EVOLUTO",          "SCENARIO SCHEDULATO",
	/*    40                           41                          42                        43                   */
	"VUOTO",                      "VUOTO",                   "VUOTO",                     "VUOTO",
	/*    44                           45                          46                        47                   */
	"GR_DIMMER100",               "SORG_RADIO",              "SORG_AUX",                  "AMBIENTE",
	/*    48                           49                          50                        51                   */
	"INSIEME_AMBIENTI",           "POSTO_ESTERNO",           "SORGENTE_MULTIM",           "SORGENTE_MULTIM_MC",
	/*    52                           53                          54                        55                   */
	"TERMO_99Z_PROBE_FANCOIL",    "TERMO_4Z_PROBE",          "TERMO_4Z_PROBE_FANCOIL",    "TERMO_NC_EXTPROBE",
	/*    56                           57                          58                        59                   */
	"TERMO_NC_PROBE",             "TERMO_HOME_NC_EXTPROBE",  "TERMO_HOME_NC_PROBE",       "STOPNGO_CLASS",
  /*    60                           61                          62                                             */
  "STOPNGO",                    "STOPNGO_PLUS",            "STOPNGO_BTEST",							"ARGH, empty!!!",
	//    64-67
	"ARGH, empty!!!", "ARGH, empty!!!", "ARGH, empty!!!", "ARGH, empty!!!",
	//    68-71
	"ARGH, empty!!!", "ARGH, empty!!!", "ARGH, empty!!!", "ARGH, empty!!!",
	//    72-75
	"ARGH, empty!!!", "ARGH, empty!!!", "ARGH, empty!!!", "ARGH, empty!!!",
	//    76-79
	"ARGH, empty!!!", "ARGH, empty!!!", "ARGH, empty!!!", "ARGH, empty!!!",
};

/*! pagTesti: text utilized during the initialization sequence when the menu is built up */
static const char pagTesti[14][20] = {"AUTOMAZIONE","ILLUMINAZIONE","ANTINTRUSIONE","CARICHI","TERMOREG","DIFSON","SCENARI","IMPOSTAZ",\
	"BACK","SPECIAL","VIDEOCITOF","SCENARI EVO", "DIFSON_MULTI", "SUPERVISIONE" };

/*******************************************
 *
 *******************************************/
xmlconfhandler::xmlconfhandler(BtMain *BM, homePage **h, homePage **sP, sottoMenu **se, sottoMenu **vc, sottoMenu **i, sottoMenu **s,
		sottoMenu **c, sottoMenu **im,  sottoMenu **a, ThermalMenu **t, diffSonora **dS, diffmulti **_dm, Antintrusion **ant,
		SupervisionMenu **sup, QWidget **pD, Client *c_c, Client *c_m , Client *c_r, versio *dG)
{
	home = h;
	specPage = sP;
	scenari_evoluti = se;
	videocitofonia = vc;
	illumino = i;
	scenari = s;
	carichi = c;
	imposta = im;
	automazioni = a;
	termo = t;
	difSon = dS;
	dm = _dm;
	antintr = ant;
	supervisione = sup;
	pagDefault = pD;
	BtM = BM;
	client_comandi = c_c;
	client_monitor = c_m;
	client_richieste = c_r;
	datiGen = dG;

	page_item_list_group = new QList<QString*>;
	page_item_list_group_m = new QList<QString*>;
	device_descr = "";
	// Start counting for wd refresh
	wdtime.start();
}

/*******************************************
 *
 *******************************************/
xmlconfhandler::~xmlconfhandler()
{
	while (!page_item_list_img.isEmpty())
		delete page_item_list_img.takeFirst();

	// NOTE: utilizzato solo dalla diffmulti!
	while (!page_item_list_img_m.isEmpty())
		delete page_item_list_img_m.takeFirst();

	while (!page_item_list_group->isEmpty())
		delete page_item_list_group->takeFirst();
	delete page_item_list_group;

	// NOTE: usato solo dalla diffmulti ma poi passato in giro..
	while (!page_item_list_group_m->isEmpty())
		delete page_item_list_group_m->takeFirst();
	delete page_item_list_group_m;
}


/*******************************************
 *
 * Inizio nuovo documento xml
 *
 *
 *******************************************/
void xmlconfhandler::set_page_item_defaults()
{
	page_item_id = 0;
	page_item_indirizzo = NULL;

	// TODO: questi sono tutti memory leak!
	page_item_list_img.clear();
	page_item_list_img_m.clear();
	page_item_list_group->clear();
	page_item_list_group_m->clear();
	
	par1 = par2 = 0;
	page_item_list_txt.clear();
	page_item_what = "";
	page_item_descr = "";
	page_item_where = "";
	page_icon = "";
	page_item_who = "";
	page_item_type = "0";
	page_item_mode = "0";
	itemNum=0;
}

/*******************************************
 *
 * Inizio nuovo documento xml
 *
 *
 *******************************************/
bool xmlconfhandler::startDocument()
{
	CurTagL1 = "";
	CurTagL2 = "";
	CurTagL3 = "";
	CurTagL4 = "";
	CurTagL5 = "";
	CurTagL6 = "";
	CurTagL7 = "";
	CurTagL8 = "";
	sottomenu_icon_name = "";
	sottomenu_id = 0;
	ok = 0;
	hompage_isdefined = 0;
	hompage_type = 0;
	hompage_id = 0xff;

	set_page_item_defaults();
	return TRUE;
}

/*******************************************
 *
 * Letta nuova tag
 *
 *
 *******************************************/
bool xmlconfhandler::startElement(const QString&, const QString&, const QString& qName, const QXmlAttributes&)
{
	car = 0;

	if (CurTagL1.isEmpty())
	{
		CurTagL1 = qName;
	}
	else if (CurTagL2.isEmpty())
	{
		CurTagL2 = qName;
	}
	else if (CurTagL3.isEmpty()) 
	{
		CurTagL3 = qName;
	}
	else if (CurTagL4.isEmpty()) 
	{
		CurTagL4 = qName;
	}
	else if (CurTagL5.isEmpty()) 
	{
		CurTagL5 = qName;
	}
	else if (CurTagL6.isEmpty()) 
	{
		CurTagL6 = qName;
	}
	else if (CurTagL7.isEmpty())
	{
		CurTagL7 = qName;
	}
	else if (CurTagL8.isEmpty())
	{
		CurTagL8 = qName;
	}
	else
	{
		qFatal("Too many level on xml conf file!!");
	}
	return TRUE;
}

void *xmlconfhandler::getAddr()
{
	// TODO: remove this obscene fix! Use a QVariant!
	static char pip[50];

	pip[0] = 0;
	void *pnt = 0;

	if (!page_item_what.isNull() && !page_item_what.isEmpty())
	{
		QString buf = page_item_what + "*" + page_item_where;
		strcpy(pip, buf.toAscii().constData());
	}
	else
		strcpy(pip, page_item_where.toAscii().constData());

	if (page_item_list_group->isEmpty())
		pnt = pip;
	else
		pnt = page_item_list_group;

	return pnt;
}

/*******************************************
 *
 * Esco da un livello
 *
 *
 *******************************************/
bool xmlconfhandler::endElement(const QString&, const QString&, const QString&)
{
	if (wdtime.elapsed() > 1000)
	{
		wdtime.restart();
		qDebug("Invoking rearmWDT()");
		rearmWDT();
	}

	if (!car)
		characters(QString("\000"));  // se ho un tag vuoto riempio con '\000' il suo campo

	if (!CurTagL1.compare("configuratore"))
	{
		if (!CurTagL2.compare("displaypages"))
		{
			if (!CurTagL3.compare("homepage") && (CurTagL4.isEmpty()))
			{
				qDebug() << "HOMEPAGE isdefined:" << hompage_isdefined << "id:" << hompage_id;
				if (hompage_isdefined)
				{
					if (hompage_isdefined)
						idPageDefault = hompage_id;
					else
						idPageDefault = 0xFF;
				}
			}
			else if (!CurTagL3.compare("pagemenu"))
			{
				/** PAGEMENU
				 *  nel file conf.xml c'è homepage, pagemenu e page0, page1,..., pagen
				 *  pagemenu è il menù principale che viene usato anche come homepage se homepage non è definita.
				 *  Ogni pagina è un sottomenu e contiene vari item.
				 */
				if (CurTagL4.startsWith("item") && CurTagL5.isEmpty())
				{
					switch (sottomenu_id)
					{
					case AUTOMAZIONE:
					case ILLUMINAZIONE:
					case ANTIINTRUSIONE:
					case CARICHI:
					case TERMOREGOLAZIONE:
					case DIFSON:
					case DIFSON_MULTI:
					case SCENARI:
					case IMPOSTAZIONI:  // addbutton normali
					case SCENARI_EVOLUTI:
					case VIDEOCITOFONIA:
					case SUPERVISIONE:
						(*home)->addButton(sottomenu_left,sottomenu_top,sottomenu_icon_name, (char)sottomenu_id);
						break;
					case DATA:
						(*home)->addDate(sottomenu_left+10,sottomenu_top+10,220,60,QFrame::Plain,3);
						break;
					case OROLOGIO:
						(*home)->addClock(sottomenu_left+10,sottomenu_top+10,220,60,QFrame::Plain,3);
						break;
					case TEMPERATURA:
					case TERMO_HOME_NC_PROBE:
						(*home)->addTemp(sottomenu_where,sottomenu_left+10,sottomenu_top+10,220,60,QFrame::Plain,3,"");
						break;
					case TERMO_HOME_NC_EXTPROBE:
						(*home)->addTemp(sottomenu_where,sottomenu_left+10,sottomenu_top+10,220,60,QFrame::Plain,3,"", "1");
						break;
					}

					// PREPARO PER QUELLA DOPO
					sottomenu_descr = "";
					sottomenu_left = 0;
					sottomenu_top = 0;
					sottomenu_where = "";
					sottomenu_icon_name = "";

				} // if (!CurTagL4.startsWith("item"))
			} // if (!CurTagL3.compare("pagemenu"))
			else if (CurTagL3.startsWith("page"))
			{
				// Aggiungo gli item delle pagine
				if (((CurTagL4.startsWith("device") || CurTagL4.startsWith("item") || !CurTagL4.compare("command")) && CurTagL5.isEmpty()))
				{
					qDebug("INSERTED ITEM:ID %d",page_item_id);
					qDebug("INS ITEM: %s",banTesti[page_item_id]);
					qDebug() << "DESCR: " << page_item_descr;
					for (int i = page_item_list_img.size(); i < 11; ++i)
						page_item_list_img.append(new QString(""));

					for (int i = page_item_list_txt.size(); i < 4; ++i)
						page_item_list_txt.append("");

					sottoMenu *pageAct = NULL;
					void *addr = 0;
					switch (page_id)
					{
					case AUTOMAZIONE:
						pageAct = *automazioni;
						break;

					case  ILLUMINAZIONE:
						pageAct = *illumino;
						break;

					case CARICHI:
						{
						pageAct = *carichi;
						addr = getAddr();

						QByteArray buf_img1 = page_item_list_txt.at(0).toAscii();
						QByteArray buf_img2 = page_item_list_txt.at(1).toAscii();
						QByteArray buf_img3 = page_item_list_txt.at(2).toAscii();
						QByteArray buf_img4 = page_item_list_txt.at(3).toAscii();

						pageAct->addItemU((char)page_item_id, page_item_descr, addr, page_item_list_img,
							par1, par2, buf_img1.data(), buf_img2.data(),
							buf_img3.data(), buf_img4.data(), par3, par4);
						}
						break;

					case TERMOREGOLAZIONE:
						pageAct = *termo;
						break;

					case SCENARI:
						pageAct = *scenari;
						break;

					case SCENARI_EVOLUTI:
						pageAct = *scenari_evoluti;
						break;

					case VIDEOCITOFONIA:
						pageAct = *videocitofonia;
						break;

					case IMPOSTAZIONI:
						pageAct = *imposta;
						break;

					case ANTIINTRUSIONE:
						// Nothing to do..
						break;
					case DIFSON:
						break;

					case DIFSON_MULTI:
						break;

					case SPECIAL:
						qDebug("special");
						switch(page_item_id)
						{
						case TEMPERATURA:
						case TERMO_HOME_NC_PROBE:
							(*specPage)->addTemp(page_item_where,10,(itemNum-1)*80+10,220,60,(int)QFrame::Plain,3,page_item_descr);
							break;
						case TERMO_HOME_NC_EXTPROBE:
							(*specPage)->addTemp(page_item_where,10,(itemNum-1)*80+10,220,60,(int)QFrame::Plain,3,page_item_descr, "1");
							break;
						case DATA:
							//sottomenu_left,sottomenu_top,
							(*specPage)->addDate(10,(itemNum-1)*80+10,220,60,QFrame::Plain,3);
							break;
						case OROLOGIO:
							//sottomenu_left,sottomenu_top,
							(*specPage)->addClock(10,(itemNum-1)*80+10,220,60,QFrame::Plain,3);
							break;
						case CMDSPECIAL:
							QByteArray buf_img = page_item_list_img.at(0)->toAscii();
							(*specPage)->addButton(60,260, buf_img.data(),SPECIAL, page_item_who,page_item_what,page_item_where,(char)page_item_type.toInt(&ok, 10));
							(*specPage)->addDescr(page_item_descr, 60,240,180,20,QFrame::Plain,3);
							pageAct = NULL;
							break;
						}
					default:
						qDebug("xmlconfhandler: sottoMenu type unknown!!");
					}

					set_page_item_defaults();
				} // if (CurTagL4.startsWith("item") && CurTagL5.isEmpty())
				else if (CurTagL3.startsWith("page") && CurTagL4.isEmpty())
				{
					QWidget *pageAct = NULL;
					// Esco dalla pagina
					switch (page_id)
					{
					case SUPERVISIONE:
						pageAct = *supervisione;
						(*supervisione)->forceDraw();
						QObject::connect(*home,SIGNAL(Supervisione()),*supervisione,SLOT(showPg()));
						QObject::connect(client_monitor,SIGNAL(frameIn(char *)),*supervisione,SIGNAL(gestFrame(char *)));
						QObject::connect(*supervisione,SIGNAL(richStato(QString)),client_richieste,SLOT(richStato(QString)));
						break;

					case AUTOMAZIONE:
						pageAct = *automazioni;
						(*automazioni)->forceDraw();
						QObject::connect(*home,SIGNAL(Automazione()),*automazioni,SLOT(showFullScreen()));
						break;

					case ILLUMINAZIONE:
						pageAct = *illumino;
						(*illumino)->forceDraw();
						QObject::connect(*home,SIGNAL(Illuminazione()),*illumino,SLOT(showFullScreen()));
						QObject::connect(*illumino,SIGNAL(richStato(QString)),client_richieste,SLOT(richStato(QString)));
						break;

					case ANTIINTRUSIONE:
						pageAct = *antintr;
						(*antintr)->draw();
						QObject::connect(*home,SIGNAL(Antiintrusione()),*antintr,SLOT(showFullScreen()));
						QObject::connect(client_comandi,SIGNAL(openAckRx()),*antintr,SIGNAL(openAckRx()));
						QObject::connect(client_comandi,SIGNAL(openNakRx()),*antintr,SIGNAL(openNakRx()));
						QObject::connect(client_monitor,SIGNAL(frameIn(char *)),*antintr,SLOT(gesFrame(char *)));
						break;

					case CARICHI:
						pageAct = *carichi;
						(*carichi)->forceDraw();
						QObject::connect(*home,SIGNAL(Carichi()),*carichi,SLOT(showFullScreen()));
						break;

					case TERMOREGOLAZIONE:
					case TERMOREG_MULTI_PLANT:
						pageAct = *termo;
						(*termo)->forceDraw();
						QObject::connect(*home,SIGNAL(Termoregolazione()),*termo,SLOT(showPage()));
						QObject::connect(client_monitor,SIGNAL(frameIn(char *)),*termo,SIGNAL(gestFrame(char *)));
						break;

					case DIFSON:
						pageAct = *difSon;
						(*difSon)->draw();
						QObject::connect(*home,SIGNAL(Difson()),*difSon,SLOT(showFullScreen()));
						QObject::connect(client_monitor,SIGNAL(frameIn(char *)),*difSon,SLOT(gestFrame(char *)));
						break;

					case DIFSON_MULTI:
						pageAct = *dm;
						(*dm)->forceDraw();
						QObject::connect(*home,SIGNAL(Difmulti()),*dm,SLOT(showFullScreen()));
						QObject::connect(client_monitor,SIGNAL(frameIn(char *)),*dm,SLOT(gestFrame(char *)));
						break;

					case SCENARI:
						pageAct = *scenari;
						(*scenari)->forceDraw();
						QObject::connect(*home,SIGNAL(Scenari()),*scenari,SLOT(showFullScreen()));
						break;

					case IMPOSTAZIONI:
						pageAct = *imposta;
						(*imposta)->forceDraw();
						QObject::connect(*home,SIGNAL(Settings()),*imposta,SLOT(showFullScreen()));
						QObject::connect(client_monitor,SIGNAL(frameIn(char *)),*imposta,SIGNAL(gestFrame(char *)));
						QObject::connect(*imposta,SIGNAL(startCalib()), BtM, SLOT(startCalib()));
						QObject::connect(*imposta,SIGNAL(endCalib()),BtM, SLOT(endCalib()));
						break;

					case SCENARI_EVOLUTI:
						pageAct = *scenari_evoluti;
						qDebug("******* scenari_evoluti = %p, impostazioni = %p ******", *scenari_evoluti,*imposta);
						(*scenari_evoluti)->forceDraw();
						QObject::connect(*home,SIGNAL(ScenariEvoluti()),*scenari_evoluti,SLOT(showFullScreen()));
						break;

					case VIDEOCITOFONIA:
						pageAct = *videocitofonia;
						qDebug("******* videocitofonia = %p ", *videocitofonia);
						(*videocitofonia)->forceDraw();
						QObject::connect(*home,SIGNAL(Videocitofonia()),*videocitofonia,SLOT(showFullScreen()));
						break;

					case SPECIAL:
						pageAct = *specPage;
						QObject::connect(client_monitor,SIGNAL(frameIn(char *)),*specPage,SLOT(gestFrame(char *)));
						break;
					} // switch page_id

					QObject::connect(pageAct, SIGNAL(Closed()), pageAct, SLOT(hide()));
					QObject::connect(pageAct, SIGNAL(Closed()), *home, SLOT(showFullScreen()));

					// Pulisco per il prox
					set_page_item_defaults();
					page_id = 0;
				}
			} //  if (CurTagL3.startsWith("page"))
		} //  if (!CurTagL2.compare("displaypages"))
	} // if (!CurTagL1.compare("configuratore"))

	if (!CurTagL8.isEmpty())
	{
		CurTagL8 = "";
	}
	else if (!CurTagL7.isEmpty())
	{
		CurTagL7 = "";
	}
	else if (!CurTagL6.isEmpty())
	{
		CurTagL6 = "";
	}
	else if (!CurTagL5.isEmpty())
	{
		CurTagL5 = "";
	}
	else if (!CurTagL4.isEmpty())
	{
		CurTagL4 = "";
	}
	else if (!CurTagL3.isEmpty())
	{
		CurTagL3 = "";
	}
	else if (!CurTagL2.isEmpty())
	{
		CurTagL2 = "";
	}
	else if (!CurTagL1.isEmpty())
	{
		CurTagL1 = "";
	}

	return TRUE;
}

/*******************************************
 *
 * Letto nuovo valore
 *
 *
 *******************************************/
bool xmlconfhandler::characters(const QString & qValue)
{
	car = 1;

	if (!CurTagL1.compare("configuratore"))
	{
		if (!CurTagL2.compare("displaypages"))
		{
			if (!CurTagL3.compare("orientation"))
			{
				setOrientation(qValue.toAscii().data());
			}

			// Leggo info homepage
			else if (!CurTagL3.compare("homepage"))
			{
				if (!CurTagL4.compare("isdefined"))
					hompage_isdefined = qValue.toInt(&ok, 10);
				else if (!CurTagL4.compare("type"))
					hompage_type = qValue.toInt(&ok, 10);
				else if (!CurTagL4.compare("id"))
					hompage_id = qValue.toInt(&ok, 10);
			}

			// Leggo info pagemenu
			else if (!CurTagL3.compare("pagemenu"))
			{

				// Leggo tutti i tag che iniziano per item
				if (CurTagL4.startsWith("id"))
				{
					*home = new homePage;
					QObject::connect(client_monitor,SIGNAL(frameIn(char *)),*home,SLOT(gestFrame(char *)));
					if ((!hompage_id) && (hompage_isdefined))
						*pagDefault=*home;
				}
				else if (CurTagL4.startsWith("item"))
				{
					if (!CurTagL5.compare("id"))
					{
						sottomenu_id = qValue.toInt(&ok, 10);

					} else if (!CurTagL5.compare("descr"))
						sottomenu_descr = qValue;
					else if (!CurTagL5.compare("left"))
						sottomenu_left = qValue.toInt(&ok, 10);
					else if (!CurTagL5.compare("top"))
						sottomenu_top = qValue.toInt(&ok, 10);
					else if (!CurTagL5.compare("cimg1"))
					{
						sottomenu_icon_name = QString(IMG_PATH);
						sottomenu_icon_name.append(qValue);
						qDebug() << "PAGEMENU:icon_name " << sottomenu_icon_name;
					}
					else if (!CurTagL5.compare("where"))
						sottomenu_where = qValue;
				} // if (!CurTagL4.startsWith("item"))
			} // if (!CurTagL3.startsWith("pagemenu"))

			// Legge le varie pagine: page1, page2,..., pagen
			else if (CurTagL3.startsWith("page"))
			{
				if (CurTagL4 == "id")
				{
					QWidget* pageAct = NULL;
					page_id = qValue.toInt(&ok, 10);
					qDebug("INSERTING PAGE: %s",pagTesti[page_id-1]);

					QDomNode page_node = getPageNode(page_id);
					assert(!page_node.isNull() && "Page node on xml configuration file not found!");

					switch (page_id)
					{
					case AUTOMAZIONE:
						*automazioni = new Automation(0, page_node);
						pageAct = *automazioni;
						break;

					case ILLUMINAZIONE:
						*illumino = new Lighting(0, page_node);
						pageAct = *illumino;
						break;

					case DIFSON_MULTI:
						*dm = new diffmulti(0, page_node);
						pageAct = *dm;
						page_item_id_m = 0;
						page_item_where_m = "";
						page_item_list_img_m.clear();
						break;

					case SCENARI:
						*scenari = new Scenario(0, page_node);
						pageAct = *scenari;
						break;

					case CARICHI:
						*carichi = new sottoMenu;
						pageAct = *carichi;
						break;

					case DIFSON:
						*difSon = new diffSonora(0, page_node);
						pageAct = *difSon;
						break;

					case ANTIINTRUSIONE:
						*antintr = new Antintrusion(0, page_node);
						pageAct = *antintr;
						break;

					case TERMOREGOLAZIONE:
						*termo = new ThermalMenu(0, page_node);
						pageAct = *termo;
						break;

					case TERMOREG_MULTI_PLANT:
						*termo = new ThermalMenu(0, page_node);
						pageAct = *termo;
						break;

					case IMPOSTAZIONI:
						*imposta = new Settings(0, page_node);
						QObject::connect(*imposta,SIGNAL(setPwd(bool, QString)), BtM, SLOT(setPwd(bool, QString)));
						pageAct = *imposta;
						break;

					case SCENARI_EVOLUTI:
						*scenari_evoluti = new Scenario(0, page_node);
						QObject::connect(BtM, SIGNAL(resettimer()), *scenari_evoluti, SIGNAL(resettimer()));
						pageAct = *scenari_evoluti;
						break;

					case VIDEOCITOFONIA:
						*videocitofonia = new VideoEntryPhone(0, page_node);
						pageAct = *videocitofonia;
						break;

					case SUPERVISIONE:
						*supervisione = new SupervisionMenu(0, page_node);
						pageAct = *supervisione;
						break;

					case SPECIAL:
						(*specPage) = new homePage;
						(*specPage)->addButton(0,260,ICON_FRECCIA_SX ,BACK);
						pageAct = *specPage;
						break;
					} // switch (page_id)

					pageAct->hide();
					if (idPageDefault == page_id && hompage_isdefined)
						*pagDefault = pageAct;

				} // if (!CurTagL4.compare("id"))
				else if ((CurTagL4.startsWith("item") || CurTagL4.startsWith("device")) && (!CurTagL5.startsWith("device")))
				{
					CurTagL4_copy=CurTagL4;
					if (CurTagL4.startsWith("item"))
						CurTagL4_copy.remove("item");
					else
						CurTagL4_copy.remove("device");

					itemNum = CurTagL4_copy.toInt(&ok, 10);
					if (!CurTagL5.compare("id"))
					{
						page_item_id = qValue.toInt(&ok, 10);
					}
					else if (!CurTagL5.compare("descr"))
						page_item_descr = qValue;
					else if (!CurTagL5.compare("mode"))
						page_item_mode = qValue;
					else if (!CurTagL5.compare("where"))
						page_item_where = qValue;
					else if (!CurTagL5.compare("what"))
						page_item_what = qValue;
					else if (CurTagL5.startsWith("element"))
					{
						if (!CurTagL6.compare("where"))
						{
							page_item_list_group->append(new QString(qValue));
						}
					}
					else if ((CurTagL5.startsWith("cimg")) || (!CurTagL5.compare("value")) ||
						(!CurTagL5.compare("hour")) || (!CurTagL5.compare("minute")))
					{
						// TODO: ore e minuti che ci fanno nella lista delle immagini???? TOGLIERE!!!
						// TODO: anche il value e' nella lista delle immagini!
						qDebug() << "FOR PAGEITEM:IMG=" << qValue;
						QString sValue = qValue;
						if  (CurTagL5.startsWith("cimg"))
							sValue.prepend(IMG_PATH);
						qDebug() << "cimg " << sValue;
						page_item_list_img.append(new QString(sValue));
					}
					else if (CurTagL5.startsWith("txt"))
					{
						page_item_list_txt.append(qValue);
					}
					else if (!CurTagL5.compare("type"))
					{
						par2 = qValue.toInt(&ok, 10);
					}
					else if (!CurTagL5.compare("alarmset"))
					{
						par3 = qValue.toInt(&ok, 10);
					}
					else if (!CurTagL5.compare("enabled"))
					{
						par1 = qValue.toInt(&ok, 10);
					}
					else if (!CurTagL5.compare("enable"))
					{
						par3 = qValue.toInt(&ok, 10);
					}
				} // if (CurTagL4.startsWith("item"))
				else if (CurTagL4.startsWith("item") && CurTagL5.startsWith("device"))
				{
					// Multichannel sound system
					CurTagL5_copy = CurTagL5;
					CurTagL5_copy.remove("device");
					itemNum = CurTagL5_copy.toInt(&ok, 10);
					if (!CurTagL6.compare("id"))
					{
						page_item_id_m = qValue.toInt(&ok, 10);
						qWarning("PAGEITEM:ID %d",page_item_id_m);
					}
					else if (CurTagL6.startsWith("descr"))
					{
						device_descr = qValue;
					}
					else if (CurTagL6.startsWith("where"))
					{
						if (!CurTagL6.compare("where"))
						{
							page_item_list_group_m->clear();
							page_item_where_m = qValue;
							qDebug() << "WHERE = " << qValue;
						}
						else
						{
							page_item_list_group_m->append(new QString(qValue));
							qDebug() << "WHERE_MULTI " << qValue;
						}
					}
					else if (CurTagL6.startsWith("cimg"))
					{
						QString sValue = qValue;
						sValue.prepend(IMG_PATH);
						qDebug() << "cimg " << sValue;
						page_item_list_img_m.append(new QString(sValue));
					}
				} // CurTagL4.startsWith("item") && CurTagL5.startsWith("device")
				else if (!CurTagL4.compare("command"))
				{
					if (!CurTagL5.compare("id"))
					{
						page_item_id = qValue.toInt(&ok, 10);
					}
					else if (!CurTagL5.compare("descr"))
						page_item_descr = qValue;
					else if (!CurTagL5.compare("type"))
						page_item_type = qValue;
					else if (!CurTagL5.compare("who"))
						page_item_who = qValue;
					else if (!CurTagL5.compare("what"))
						page_item_what = qValue;
					else if (!CurTagL5.compare("where"))
						page_item_where = qValue;
					else if (CurTagL5.startsWith("cimg"))
					{
						QString sValue = qValue;
						sValue.prepend(IMG_PATH);
						page_item_list_img.append(new QString(sValue));
					}
				}  //else if (!CurTagL4.compare("command"))
				else if (!CurTagL4.compare("type"))
				{
					par3 = qValue.toInt(&ok, 10);
				}
			} // else if (CurTagL4.startsWith("page"))
		} // if (!CurTagL2.startsWith("displaypages"))
		else if (!CurTagL2.compare("setup"))
		{
			if (!CurTagL3.compare("scs"))
			{
				if (!CurTagL4.compare("coordinate_scs"))
				{
					if (!CurTagL5.compare("diag_addr"))
					{
						datiGen->setAddr(qValue.toInt(&ok, 16)-768);
					}
				}
			}
			else if (!CurTagL3.compare("generale"))
			{
				if (!CurTagL4.compare("modello"))
					datiGen->setModel(qValue);
			}
		}
	} // if (!CurTagL1.startsWith("configuratore"))

	return TRUE;
}
