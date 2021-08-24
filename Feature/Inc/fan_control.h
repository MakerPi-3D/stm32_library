#ifndef FAN_CONTROL_H
#define FAN_CONTROL_H

#ifdef HAS_FAN_CONTROL

#ifdef __cplusplus
extern "C" {
#endif

extern void feature_fan_control_init(void);
extern void feature_fan_control_e_pwm(int pwm_value);

#ifdef __cplusplus
} //extern "C"
#endif

#endif

#endif // FAN_CONTROL_H

