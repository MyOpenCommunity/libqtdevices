#ifndef _SCENEVOCOND_H_
#define _SCENEVOCOND_H_

#include "page.h"
#include "main.h"
#include "datetime.h" //BtTimeEdit

#include <QTimer>


class BtButton;
class device_condition;


/*!
  \class scenEvo_cond
  \brief This class represents a generic advanced scenario condition
  \author Ciminaghi
  \date April 2006
*/
class scenEvo_cond : public Page
{
Q_OBJECT
public:
	//! A type flag, used because RTTI is disabled.
	bool hasTimeCondition;

	/*!
	\brief returns description of condition
	*/
	virtual const char *getDescription();
	//! Set serial number
	void set_serial_number(int);
	//! Get serial number
	int get_serial_number();
	//! Inits condition
	virtual void inizializza();
	//! Returns true when condition is satisfied
	virtual bool isTrue();

public slots:
	//! Next button pressed
	virtual void Next();
	//! Prev button pressed
	virtual void Prev();
	//! OK button pressed
	virtual void OK();
	//! Apply button pressed
	virtual void Apply();
	//! Save condition
	virtual void save();
	//! Reset condition (on cancel)
	virtual void reset();

protected:
	// The constructor is protected to avoid the building of scenEvo_cond objects.
	scenEvo_cond();

private:
	int serial_number;

protected:
	int item_id;

signals:
	//! Emitted when user clicks Next icon
	void SwitchToNext();
	//! Emitted when user clicks Prev icon (NO MORE !!!)
	void SwitchToPrev();
	//! Emitted when user ckicks Prev icon
	void SwitchToFirst();
	//! Emitted when condition is true
	void verificata();
	//! Emitted when all conditions in list must be reset
	void resetAll();
	//! Emitted when all conditions in list are OK (must be saved and applied)
	void okAll();
	//! After a status changed, a condition is satisfied
	void condSatisfied();
};

/*!
  \class scenEvo_cond_h
  \brief This class represent a time based advanced scenario condition
  \author Ciminaghi
  \date April 2006
*/
class scenEvo_cond_h : public scenEvo_cond
{
Q_OBJECT
public:
	scenEvo_cond_h(int item_id, const QDomNode &config_node, bool has_next);
	/*!
	\brief Returns condition description
	*/
	const char *getDescription();

	//! Save condition
	void save();
	//! Return true when condition is satisfied
	bool isTrue();
public slots:
	//! OK method
	void OK();
	//! Apply method
	void Apply();
	//! Reset condition
	void reset();
	//! Timer expired method
	void scaduta();
	//! Just setup qt timer (based on cond_time)
	void setupTimer();
private:
	//! OK icon index (area #7)
	static const int A7_ICON_INDEX = 3;
	//! Area #6 (prev) icon index
	static const int A6_ICON_INDEX = 1;
	//! Area #8 (next) icon index
	static const int A8_ICON_INDEX = 2;

	QTime cond_time;
	BtButton *bottom_left, *bottom_center, *bottom_right;

	BtTimeEdit time_edit;
	QTimer timer;
};


/*!
\class scenEvo_cond_d
\brief This class represent a device based advanced scenario condition
\author Ciminaghi
\date April 2006
*/
class scenEvo_cond_d : public scenEvo_cond
{
Q_OBJECT
public:
	scenEvo_cond_d(int item_id, const QDomNode &config_node);

	/*!
	\brief Returns condition description in human language
	*/
	const char *getDescription();
	//! Save condition
	virtual void save();
	//! Return true when condition is satisfied
	bool isTrue();
public slots:
	//! OK method
	void OK();
	//!  Apply method
	void Apply();
	//! Up method
	void Up();
	//! Down method
	void Down();
	//! Reset condition
	void reset();
private:
	//! Button width/height
	static const int BUTTON_DIM = 60;
	//! Text area height
	static const int TEXT_Y_DIM = 20;

	//! Specific device condition
	device_condition *actual_condition;
	//! Inits condition
	void inizializza();
};

#endif // _SCENEVOCOND_H_
