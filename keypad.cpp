#include "keypad.h"
#include "banner.h"
#include "fontmanager.h" // bt_global::font
#include "btbutton.h"

#include <QLabel>
#include <QDebug>
#include <QButtonGroup>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>


// Keypad implementation

Keypad::Keypad()
{
	static const char *icons[] =
	{
		ICON_ZERO, ICON_UNO, ICON_DUE, ICON_TRE, ICON_QUATTRO,
		ICON_CINQUE, ICON_SEI, ICON_SETTE, ICON_OTTO, ICON_NOVE
	};

	BtButton *ok = new BtButton;
	BtButton *canc = new BtButton;
	BtButton *digits[10];

	QButtonGroup *buttons = new QButtonGroup;

	for (int i = 0; i < 10; ++i)
	{
		digits[i] = new BtButton;
		digits[i]->setImage(icons[i]);
		buttons->addButton(digits[i], i);
	}

	digitLabel = new QLabel;
	QLabel *pwdLabel = new QLabel;

	ok->setImage(ICON_OK);
	canc->setImage(ICON_CANC);

	mode = CLEAN;

	pwdLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	pwdLabel->setFont(bt_global::font->get(FontManager::TEXT));
	pwdLabel->setText(tr("PASSWORD:"));

	digitLabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
	digitLabel->setFont(bt_global::font->get(FontManager::TEXT));

	connect(buttons, SIGNAL(buttonClicked(int)), SLOT(buttonClicked(int)));
	connect(canc, SIGNAL(clicked()), SLOT(canc()));
	connect(ok, SIGNAL(clicked()), SLOT(ok()));

	// digits, ok, cancel buttons
	QGridLayout *k = new QGridLayout;
	k->setContentsMargins(0, 0, 0, 0);
	// k->setSpacing(5);

	for (int i = 0; i < 9; ++i)
		k->addWidget(digits[i + 1], i / 3, i % 3);

	k->addWidget(canc, 3, 0);
	k->addWidget(digits[0], 3, 1);
	k->addWidget(ok, 3, 2);

	// bottom labels
	QHBoxLayout *p = new QHBoxLayout;
	p->setContentsMargins(0, 0, 0, 0);
	p->setSpacing(0);

	p->addWidget(pwdLabel, 1);
	p->addWidget(digitLabel, 1);

	// top layout
	QVBoxLayout *l = new QVBoxLayout(this);
	l->setContentsMargins(0, 0, 0, 10);
	l->setSpacing(0);

	// when modifying this, modify insertLayout below
	l->addLayout(k);
	l->addLayout(p);
}

void Keypad::insertLayout(QLayout *l)
{
	((QVBoxLayout*)layout())->insertLayout(1, l);
}

void Keypad::showEvent(QShowEvent *event)
{
	draw();
}

void Keypad::draw()
{
	qDebug("tastiera::draw(), mode = %d", mode);
	if (mode == CLEAN)
		digitLabel->setText(text);
	else
		digitLabel->setText(QString(text.length(),'*'));
}

void Keypad::buttonClicked(int number)
{
	qDebug() << "button clicked " << number;
	if (text.length() < 5)
		text += QString::number(number);
	draw();
}

void Keypad::canc()
{
	if (text.length() > 0)
		text.chop(1);
	else
	{
		text = "";
		emit Closed();
	}
	draw();
}

void Keypad::ok()
{
	emit Closed();
}

void Keypad::setMode(Type t)
{
	mode = t;
	draw();
}

QString Keypad::getText()
{
	return text;
}

void Keypad::resetText()
{
	text = "";
	draw();
}


// KeypadWithState implementation

KeypadWithState::KeypadWithState(int s[8])
{
	QHBoxLayout *l = new QHBoxLayout;
	l->setContentsMargins(5, 5, 5, 5);
	l->setSpacing(2);

	QFont aFont = bt_global::font->get(FontManager::TEXT);
	QString zone_style = "QLabel { background-color:white; color:black; }";

	for (int i = 0; i < 8; i++)
	{
		QLabel *state = new QLabel;

		state->setFont(aFont);
		state->setAlignment(Qt::AlignCenter);

		if (s[i] == -1)
		{
			state->setText("-");
		}
		else
		{
			state->setText(QString::number(i + 1));
			if (s[i])
				state->setStyleSheet(zone_style);
		}

		l->addWidget(state);
	}

	insertLayout(l);
}
