#include "ringtonesmanager.h"
#include "hardware_functions.h"

#include <QStringList>
#include <QDebug>
#include <QDir>

#include <unistd.h> // lseek, read
#include <fcntl.h>  // open

// TODO: this is not absolute for testing
#define RINGTONE_DIR "cfg/extra/2/"

RingtonesManager::RingtonesManager()
{
	QStringList filters;
	filters << "*.[wW][[aA][vV]";
	int id = 0;
	QDir d(RINGTONE_DIR);
	foreach (const QFileInfo &fi, scanDirectory(d, filters))
	{
		ringtone_to_file[id] = fi.filePath();
		qDebug() << fi.filePath();
		++id;
	}
	readRingtonesFromFlash();

	qDebug() << "Found the following ringtones: " << ringtone_to_file;
}

void RingtonesManager::playRingtone(RingtoneType t)
{
	// TODO: maybe we will need a stopRingtone too
	playSound(typeToFilePath(t));
}

void RingtonesManager::playRingtone(int ring)
{
	Q_ASSERT_X(ring >= 0 && ring < ringtone_to_file.size(), "RingtonesManager::playRingtone(int)",
		"Given ringtone is outside valid range.");
	playSound(ringtone_to_file[ring]);
}

void RingtonesManager::setRingtone(RingtoneType t, int ring)
{
	Q_ASSERT_X(ring >= 0 && ring < ringtone_to_file.size(), "RingtonesManager::setRingtone",
		"Given ringtone is outside valid range.");
	type_to_ringtone[t] = ring;
	// TODO: save the map to flash memory
}

int RingtonesManager::getRingtone(RingtoneType t)
{
	return type_to_ringtone[t];
}

int RingtonesManager::getRingtonesNumber()
{
	return ringtone_to_file.size();
}

QString RingtonesManager::typeToFilePath(RingtoneType t)
{
	return ringtone_to_file[type_to_ringtone[t]];
}

// TODO: this can be shared with AudioFileSelector::browseFiles()?
QList<QFileInfo> RingtonesManager::scanDirectory(const QDir &dir, const QStringList &filters)
{
	QList<QFileInfo> temp_files_list;

	foreach (const QFileInfo &fi, dir.entryInfoList(filters))
	{
		if (fi.fileName() != "." && fi.fileName() != "..")
			temp_files_list.append(fi);
	}

	return temp_files_list;
}

#define E2_BASE_RINGS       11673
#define DEV_E2 "/dev/nvram"
// how many different types of ringtones we have
#define RINGTONE_COUNT 11

void RingtonesManager::readRingtonesFromFlash()
{
#ifdef BT_EMBEDDED
	char saved_values[RINGTONE_COUNT];
	memset(saved_values, 0, RINGTONE_COUNT);
	int eeprom = open(DEV_E2, O_RDWR | O_SYNC, 0666);
	if (eeprom > 0)
	{
		lseek(eeprom, E2_BASE_RINGS, SEEK_SET);
		ssize_t bytes = read(eeprom, saved_values, RINGTONE_COUNT);
		close(eeprom);

		if (bytes < 0)
			qWarning() << "RingtonesManager::readRingtonesFromFlash: an error occurred while reading flash memory; using default values.";
		// saved_values is already all zero, we don't set random values here
		for (int i = 0; i < RINGTONE_COUNT; ++i)
			type_to_ringtone[static_cast<RingtoneType>(i)] = saved_values[i];
	}
#endif
}

RingtonesManager *bt_global::ringtones = 0;
