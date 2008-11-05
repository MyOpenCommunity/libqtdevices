#ifndef BACKLIGHT_H
#define BACKLIGHT_H

#include <qvaluevector.h>
#include <qpair.h>

enum BacklightStatus
{
	BACKLIGHT_OFF = 0,
	BACKLIGHT_ON = 1
};

enum BrightnessState
{
	ON = 0,
	OFF = 1,
	SCREENSAVER = 2
};

typedef QPair<BacklightStatus, unsigned> brightness_state_t;
typedef QValueVector< brightness_state_t > brightness_policy_t;


/**
 * This is a helper class to set the brightness of the display.
 * To use it, first create and set a brightness policy and later
 * call setState() to set a particular state of the policy.
 * A brightness policy is a vector of NUMBER_OF_STATES QPair elements. Each
 * element is a pair of BacklightStatus, that indicates if the backlight
 * should be on or off, and unsigned, a value of brightness (it is a range
 * from 0=max brightness to 255=min brightness).
 */
class BrightnessControl
{
public:

	/// Number of possible states (On, Off, Screensaver)
	static const unsigned NUMBER_OF_STATES = static_cast<unsigned>(SCREENSAVER) + 1;

	/**
	 * Sets the brightness policy. The brightness policy must be of this form:
	 * index 0 (== ON): state for on
	 * index 1 (== OFF): state for off
	 * index 2 (== SCREENSAVER): state for screensaver
	 */

	static bool setBrightnessPolicy(const brightness_policy_t &bp);
	/**
	 * Regulates brightness and backlight as stored in the policy.
	 * \param state The state to be set.
	 */
	static void setState(BrightnessState state);
private:
	static brightness_policy_t policy;
};


#endif //BACKLIGHT_H
