#ifndef ITEMS_H
#define ITEMS_H

// This file contains widgets with some logic that are not banner.


#include <QWidget>
#include <QString>

class BtButton;


// A simple class to save and restore the status of a ItemTuning. This class
// has no public method or attribute, because it is designed to preserve the
// encapsulation of a ItemTuning (as the Memento Pattern).
class ItemTuningStatus
{
friend class ItemTuning;
private:
	int current_level;
};


/**
 * This class contains an item like with two buttons (like a banner, but
 * without a fixed width) for tuning.
 */
class ItemTuning : public QWidget
{
Q_OBJECT
public:
	ItemTuning(QString text, QString icon, QWidget *parent=0);

	ItemTuningStatus *getStatus();
	void setStatus(ItemTuningStatus *st);

public slots:
	void decreaseLevel();
	void increaseLevel();

signals:
	void valueChanged(int);

private slots:
	void changeIcons();

private:
	int current_level;
	QString icon_name;
	BtButton *left, *right;
};

#endif // ITEMS_H
