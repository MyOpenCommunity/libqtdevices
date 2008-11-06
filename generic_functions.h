/****************************************************************
**
** BTicino Touch scren Colori art. H4686
**
**genericfunz.h
**
**funzioni generiche usate qua e là
**
****************************************************************/

#include <QString>
#include <QList>

 /*!
  \brief Transform a image file name into a pressed one.

  Return the filename of a image '.png' converted into a "p.png" file name.
*/
QString getPressName(QString name);

 /*!
  \brief Transform a image file name into a pressed one with a index taken from a \a antiintrusion \a zone name.

  The ".png" file name is converted into a "[number]p.png" file name. The first argument is the name to convert, the second is the converted name gave back from the procedure,  the third is a string like "zn" where n is the number of the \a antiintrusion \a zone and the fourth is the length of the file name.
*/
void getZoneName(char* , char* ,char*, char );

/*!
  Like getZoneName, but receives an "ambiente" number
*/
void getAmbName(char *, char *, char *, char);

QString getAmbName(QString name, QString amb);

 /*!
  \brief Sets the contrast of the device.

  The contrast is set to the value of the first argument. If the second argument is TRUE also the configuration file is updated otherwise it isn't.
*/
void setContrast(unsigned char,bool);

 /*!
  \brief Gets the contrast of the device.
*/
unsigned char getContrast();

 /*!
  \brief Sets the backlight On/Off according to the value of the argument.
*/
void setBacklight(bool);

/**
 * Sets the backlight of the device on or off according to the value of the argument.
 * This differs from setBacklight in that it only sets the backlight (ie. writes only to
 * device /proc/sys/dev/btweb/brightness).
 * \param value If true, sets on the backlight.
 */
void setBacklightOn(bool value);

/**
 * Sets the brightness level of the display.
 * \param level The new brightness level. Allowed values are 0 <= level <= 255
 * Using values outside this range yields no results.
 */
void setBrightnessLevel(int level);

 /*!
  \brief Gets the backlight state.
*/
bool getBacklight();

/**
 * Enables the beep.
 * \param buzzer_enable Enables the beep for this execution of BTouch, without changing conf.xml.
 * \param write_to_conf Writes to config file the value passed in buzzer_enable.
 */
void setBeep(bool buzzer_enable, bool write_to_conf);

 /*!
  \brief Sets the orientation of the device.
*/
void setOrientation( char*);

 /*!
  \brief Makes a system beep.
*/
void beep();

 /*!
  \brief Makes a system beep and the argument represent the duration in ms.
*/
void beep(int);

 /*!
  \brief Retrieves if the beep is enabled or not.
*/
bool getBeep();

 /*!
  \brief Retrieves the time elapsed from last pressure of the device.
*/
unsigned long getTimePress();

 /*!
  \brief WatchDog rearm.
*/
void rearmWDT();

 /*!
  \brief  Changes configuration file giving: fileName, id, fieldName, value, serial number of id recursion to change
*/
bool setCfgValue(QString file, int id, QString campo, QString valore, int serNumId);

 /*!
  \brief  Same as above. file name is the default one and \a serial \a number \a of \a id \a recursion \a to \a change is 1.
*/
bool setCfgValue(int , const char* , const char* );

 /*!
  \brief  Same as above. file name is the default one.
*/
bool setCfgValue(int , const char*, const char* ,int);

 /*!
  \brief  Copies the file whose fileName is the first argument in an other with the fileName as in the second argument..
*/
bool copyFile(char*, char*);

void  comChConf();
 /*!
  \brief Retrieves the name of the device.
*/

void getName(char *name);

void resetTimer(int signo);

void grabScreen(void* pWidget);

// Obscene hack, see function below...
static QString empty_string("");

/**
 * Utility function to access a list of string: if no element is present
 * at position \a idx, return empty string.
 */
inline QString *safeAt(QList<QString*> &list, unsigned int idx)
{
	if (static_cast<int>(idx) < list.size())
		return list.at(idx);
	else
		return &empty_string;
}
