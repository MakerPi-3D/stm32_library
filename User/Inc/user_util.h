#ifndef USER_UTIL_H
#define USER_UTIL_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool user_is_float_data_equ(const float p_a, const float p_b); // Are the two floating point numbers equal
char user_decryption_code(char source, int key, int pos); // Get encrypted characters

#ifdef __cplusplus
} //extern "C"
#endif

#endif // USER_UTIL_H

