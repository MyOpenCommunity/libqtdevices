#include "backlight.h"
#include "genericfunz.h"

BrightnessControl *BrightnessControl::_instance = 0;

static brightness_policy_t getBrightnessPolicy(brightness_state_t on_state,
		brightness_state_t off_state, brightness_state_t screensaver_state)
{
	brightness_policy_t policy;
	policy.push_back(on_state);
	policy.push_back(off_state);
	policy.push_back(screensaver_state);
	return policy;
}

BrightnessControl::BrightnessControl()
{
	setBrightnessPolicy(POLICY_HIGH);
}

bool BrightnessControl::setBrightnessPolicy(DefautPolicy policy)
{
	switch (policy)
	{
	case POLICY_OFF:
		setBrightnessPolicy(getBrightnessPolicy(
			brightness_state_t(BACKLIGHT_ON, 10),
			brightness_state_t(BACKLIGHT_OFF, 10),
			brightness_state_t(BACKLIGHT_OFF, 10)
			));
		break;
	case POLICY_LOW:
		setBrightnessPolicy(getBrightnessPolicy(
			brightness_state_t(BACKLIGHT_ON, 10),
			brightness_state_t(BACKLIGHT_ON, 255),
			brightness_state_t(BACKLIGHT_ON, 255)
			));
		break;
	default:
		//should we return false instead of raising a warning?
		qWarning("Unknown default policy, setting POLICY_HIGH");
		// fall through
	case POLICY_HIGH:
		setBrightnessPolicy(getBrightnessPolicy(
			brightness_state_t(BACKLIGHT_ON, 10),
			brightness_state_t(BACKLIGHT_ON, 210),
			brightness_state_t(BACKLIGHT_ON, 210)
			));
		break;
	}
	return true;
}

bool BrightnessControl::setBrightnessPolicy(const brightness_policy_t &bp)
{
	if (bp.size() != NUMBER_OF_STATES)
	{
		qWarning("Invalid brightness policy: there are not exactly %u states", NUMBER_OF_STATES);
		return false;
	}
	policy = bp;
	return true;
}

void BrightnessControl::setState(BrightnessState state)
{
	// FIXME: cosi' funziona ma forse e' meglio fare un cast a bool
	setBacklightOn(policy[state].first);

	setBrightnessLevel(policy[state].second);
}

BrightnessControl *BrightnessControl::instance()
{
	if (!_instance)
		_instance = new BrightnessControl();
	return _instance;
}
