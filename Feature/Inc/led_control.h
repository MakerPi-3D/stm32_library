#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#ifdef HAS_LED_CONTROL

#ifdef __cplusplus
extern "C" {
#endif

extern bool feature_led_get_lighting_status(void);
extern void feature_led_set_lighting_status(bool value);
extern void feature_led_control(void);

#ifdef __cplusplus
} //extern "C"
#endif

#endif

#endif // LED_CONTROL_H

