/**
 * \file
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief This class implements the managing of Automation section
 *
 * \author Gianni Valdambrini <aleister@develer.com>
 * \date November 2008
 */

#ifndef AUTOMATION_H
#define AUTOMATION_H

#include "sottomenu.h"


class Automation : public sottoMenu
{
Q_OBJECT
public:
	Automation(QWidget *parent, QDomNode config_node);

	virtual int addItemU(char tipo, const QString &descrizione, void *indirizzo,
		QList<QString*> &icon_names,int periodo = 0, int numFrame = 0,
		char *descr1 = NULL, char *descr2 = NULL, char *descr3 = NULL, char *descr4 = NULL,
		int par3=0, int par4=0,
		QList<QString*> *lt = NULL, QList<scenEvo_cond*> *lc = NULL,
		QString action="", QString light="", QString key="", QString unk="",
		QList<int> sstart = QList<int>(),
		QList<int> sttop = QList<int>(), QString txt1="", QString txt2="", QString txt3="")  { return 0; }

	virtual int addItemU(char tipo, const QString &descrizione, QList<QString> indirizzo,
		QList<QString*> &icon_names,int periodo = 0, int numFrame = 0) { return 0; }
};

#endif
