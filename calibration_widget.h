#ifndef CALIBRATION_WIDGET_H
#define CALIBRATION_WIDGET_H

#include <QWSPointerCalibrationData>
#include <QPixmap>
#include <QWidget>

class BtButton;
class QTimer;


/**
 * The widget to calibrate the touchscreen. The procedure is composed by two
 * part: the actual calibration and a simple test with two buttons.
 * The new calibration is saved (into the calibration file) only if both the
 * part ends with success.
 */
class CalibrationWidget : public QWidget
{
Q_OBJECT
public:
	CalibrationWidget(bool minimal = false);

	// Return true if the calibration file exists.
	static bool exists();

	// Return true if the calibration file is valid
	static bool isValid();

	// Start or restart the calibration
	void startCalibration();

signals:
	void calibrationEnded();
	void calibrationStarted();

protected:
	virtual void paintEvent(QPaintEvent*);
	virtual void mouseReleaseEvent(QMouseEvent*);
	virtual void hideEvent(QHideEvent*);

private slots:
	// The method used to draw the crosshair during the movement between positions
	void drawCrosshair();
	// End the calibration (with success)
	void endCalibration();
	// Abort the changes with the wrong calibration and re-start another calibration.
	void rollbackCalibration();

private:
	// The positions of the crosshair are:
	// top-left, bottom-left, bottom-right, top-right, center
	// The last one is skip with minimal_version == true

	QTimer *crosshair_timer; // the timer used to move the crosshair between positions
	int delta_x, delta_y; // the deltas used to move the crosshair between positions

	// the current position of the crosshair. It can be different from the standard 5 positions
	// due to the animation after the "click".
	QPoint current_position;

	QWSPointerCalibrationData calibration_data;

	int current_location; // the index of the current crosshair position (yet to press)

	// A flag that marks if the calibration is in the normal form or minimal
	// (with only 4 checks and no buttons)
	bool minimal_version;

	// The file where Qt store the calibration data
	QString pointercal_file;

	// A flag that mark if we are in the actual calibration or in the test with buttons.
	bool test_buttons;

	// The two buttons used to check the calibration.
	BtButton *topleft_button, *bottomright_button;

	QTimer *buttons_timer; // the timer used to set a timeout for the test with buttons.

	QString text; // the text that contains the instructions for the user

#ifdef LAYOUT_BTOUCH
	QPixmap logo;
#endif
	// Start to track the crosshair movement
	void trackCrosshair();

	// Check if the calibration was doing right
	bool sanityCheck();

	// Start the test with buttons
	void startTestButtons();
};


#endif // CALIBRATION_WIDGET_H

