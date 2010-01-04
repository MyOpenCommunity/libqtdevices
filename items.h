#ifndef ITEMS_H
#define ITEMS_H

// This file contains widgets with some logic that are not banner.


#include <QWidget>
#include <QString>

class BtButton;


class ItemTuning : public QWidget
{
Q_OBJECT
public:
	ItemTuning(QString text, QString icon, QWidget *parent=0);

public slots:
	void decreaseLevel();
	void increaseLevel();
	void changeIcons();

signals:
	void valueChanged(int);

private:
	int current_level;
	QString icon_name;
	BtButton *left, *right;
};

#endif // ITEMS_H
