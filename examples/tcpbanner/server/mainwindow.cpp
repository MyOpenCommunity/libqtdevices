/*
 * BTouch - Graphical User Interface to control MyHome System
 *
 * Copyright (C) 2010 BTicino S.p.A.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include "mainwindow.h"

#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

#include "messagereceiver.h"


MainWindow::MainWindow(QWidget *parent)
	: QWidget(parent), receiver(new MessageReceiver(this)), text_edit(new QTextEdit)
{
	connect(receiver, SIGNAL(messageReceived(QString)), text_edit, SLOT(append(QString)));
	buildUi();
}

void MainWindow::buildUi()
{
	text_edit->setReadOnly(true);

	QHBoxLayout *main_layout = new QHBoxLayout(this);
	main_layout->addWidget(text_edit, 1);

	QVBoxLayout *button_layout = new QVBoxLayout;
	main_layout->addLayout(button_layout);

	QPushButton *clear_button = new QPushButton(tr("&Clear"));
	connect(clear_button, SIGNAL(clicked()), text_edit, SLOT(clear()));
	button_layout->addWidget(clear_button);

	QPushButton *quit_button = new QPushButton(tr("&Quit"));
	connect(quit_button, SIGNAL(clicked()), SLOT(close()));
	button_layout->addWidget(quit_button);

	button_layout->addStretch();
}
