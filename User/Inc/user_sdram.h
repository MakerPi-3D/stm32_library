#ifndef _USER_SDRAM_H
#define _USER_SDRAM_H

#ifdef ENABLE_SDRAM

#ifdef __cplusplus
extern "C" {
#endif

extern void user_fmc_sdram_init(void);
extern u8 user_fmc_sdram_send_cmd(u8 bankx, u8 cmd, u8 refresh, u16 regval);
extern void user_fmc_sdram_write_buffer(u8 *pBuffer, u32 WriteAddr, u32 n);
extern void user_fmc_sdram_read_buffer(u8 *pBuffer, u32 ReadAddr, u32 n);

#ifdef __cplusplus
} //extern "C" {
#endif

#endif // ENABLE_SDRAM

#endif //_USER_SDRAM_H

