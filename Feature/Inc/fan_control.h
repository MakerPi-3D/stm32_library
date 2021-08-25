#ifndef FAN_CONTROL_H
#define FAN_CONTROL_H

#ifdef HAS_FAN_CONTROL

#ifdef __cplusplus
extern "C" {
#endif

extern int feature_get_extruder_fan_speed(void);
extern void feature_set_extruder_fan_speed(int value);
extern void feature_fan_control(void);

#ifdef __cplusplus
} //extern "C"
#endif

#endif

#endif // FAN_CONTROL_H

