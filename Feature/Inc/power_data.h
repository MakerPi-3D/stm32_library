#ifndef POWER_DATA_H
#define POWER_DATA_H

#ifdef HAS_POWER_RECOVERY

#ifdef __cplusplus
extern "C" {
#endif

extern void feature_pow_data_init(void);
extern void feature_pow_data_set(void);
extern void feature_pow_data_reset_flag(void);
extern void feature_pow_data_start_print_init(void);
extern void feature_pow_data_delete_file_from_sd(void);
extern void feature_pow_data_save_file_path_name(char *path_file_name, char *file_name);
extern void feature_pow_data_set_file_path_name(const char *filePathName);

#ifdef __cplusplus
} //extern "C"
#endif

#endif

#endif // POWER_DATA_H

