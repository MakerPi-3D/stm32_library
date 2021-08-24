#ifndef FILAMENT_CHECK_H
#define FILAMENT_CHECK_H

#ifdef HAS_FILAMENT_SENSOR

#ifdef __cplusplus
extern "C" {
#endif

extern void feature_filament_check_init(void);
extern void feature_filament_check(void);

#ifdef __cplusplus
} //extern "C"
#endif

#endif

#endif // FILAMENT_CHECK_H

