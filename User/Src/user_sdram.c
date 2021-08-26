#include "user_common.h"

#ifdef ENABLE_SDRAM

extern SDRAM_HandleTypeDef hsdram1;

#define Bank5_SDRAM_ADDR    ((u32)(0XC0000000)) //SDRAM��ʼ��ַ

//SDRAM���ò���
#define SDRAM_MODEREG_BURST_LENGTH_1             ((u16)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((u16)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((u16)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((u16)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((u16)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((u16)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((u16)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((u16)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((u16)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((u16)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((u16)0x0200)

#if 0
//SDRAM�ײ��������������ã�ʱ��ʹ��
//�˺����ᱻHAL_SDRAM_Init()����
//hsdram:SDRAM���
void HAL_SDRAM_MspInit(SDRAM_HandleTypeDef *hsdram)
{
  GPIO_InitTypeDef GPIO_Initure;
  __HAL_RCC_FMC_CLK_ENABLE();                 //ʹ��FMCʱ��
  __HAL_RCC_GPIOC_CLK_ENABLE();               //ʹ��GPIOCʱ��
  __HAL_RCC_GPIOD_CLK_ENABLE();               //ʹ��GPIODʱ��
  __HAL_RCC_GPIOE_CLK_ENABLE();               //ʹ��GPIOEʱ��
  __HAL_RCC_GPIOF_CLK_ENABLE();               //ʹ��GPIOFʱ��
  __HAL_RCC_GPIOG_CLK_ENABLE();               //ʹ��GPIOGʱ��
  GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3;
  GPIO_Initure.Mode = GPIO_MODE_AF_PP;        //���츴��
  GPIO_Initure.Pull = GPIO_PULLUP;            //����
  GPIO_Initure.Speed = GPIO_SPEED_HIGH;       //����
  GPIO_Initure.Alternate = GPIO_AF12_FMC;     //����ΪFMC
  HAL_GPIO_Init(GPIOC, &GPIO_Initure);         //��ʼ��PC0,2,3
  GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOD, &GPIO_Initure);    //��ʼ��PD0,1,8,9,10,14,15
  GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOE, &GPIO_Initure);    //��ʼ��PE0,1,7,8,9,10,11,12,13,14,15
  GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOF, &GPIO_Initure);    //��ʼ��PF0,1,2,3,4,5,11,12,13,14,15
  GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOG, &GPIO_Initure);     //��ʼ��PG0,1,2,4,5,8,15
}
#endif

//��SDRAM��������
//bankx:0,��BANK5�����SDRAM����ָ��
//      1,��BANK6�����SDRAM����ָ��
//cmd:ָ��(0,����ģʽ/1,ʱ������ʹ��/2,Ԥ������д洢��/3,�Զ�ˢ��/4,����ģʽ�Ĵ���/5,��ˢ��/6,����)
//refresh:��ˢ�´���
//regval:ģʽ�Ĵ����Ķ���
//����ֵ:0,����;1,ʧ��.
u8 user_fmc_sdram_send_cmd(u8 bankx, u8 cmd, u8 refresh, u16 regval)
{
  u32 target_bank = 0;
  FMC_SDRAM_CommandTypeDef Command;

  if (bankx == 0) target_bank = FMC_SDRAM_CMD_TARGET_BANK1;
  else if (bankx == 1) target_bank = FMC_SDRAM_CMD_TARGET_BANK2;

  Command.CommandMode = cmd;              //����
  Command.CommandTarget = target_bank;    //Ŀ��SDRAM�洢����
  Command.AutoRefreshNumber = refresh;    //��ˢ�´���
  Command.ModeRegisterDefinition = regval; //Ҫд��ģʽ�Ĵ�����ֵ

  if (HAL_SDRAM_SendCommand(&hsdram1, &Command, 0X1000) == HAL_OK) //��SDRAM��������
  {
    return 0;
  }
  else return 1;
}

//��ָ����ַ(WriteAddr+Bank5_SDRAM_ADDR)��ʼ,����д��n���ֽ�.
//pBuffer:�ֽ�ָ��
//WriteAddr:Ҫд��ĵ�ַ
//n:Ҫд����ֽ���
void user_fmc_sdram_write_buffer(u8 *pBuffer, u32 WriteAddr, u32 n)
{
  for (; n != 0; n--)
  {
    *(vu8 *)(Bank5_SDRAM_ADDR + WriteAddr) = *pBuffer;
    WriteAddr++;
    pBuffer++;
  }
}

//��ָ����ַ((WriteAddr+Bank5_SDRAM_ADDR))��ʼ,��������n���ֽ�.
//pBuffer:�ֽ�ָ��
//ReadAddr:Ҫ��������ʼ��ַ
//n:Ҫд����ֽ���
void user_fmc_sdram_read_buffer(u8 *pBuffer, u32 ReadAddr, u32 n)
{
  for (; n != 0; n--)
  {
    *pBuffer++ = *(vu8 *)(Bank5_SDRAM_ADDR + ReadAddr);
    ReadAddr++;
  }
}

//����SDRAM��ʼ������
static void _user_fmc_sdram_initialization_sequence(SDRAM_HandleTypeDef *hsdram)
{
  u32 temp = 0;
  //SDRAM��������ʼ������Ժ���Ҫ��������˳���ʼ��SDRAM
  user_fmc_sdram_send_cmd(0, FMC_SDRAM_CMD_CLK_ENABLE, 1, 0); //ʱ������ʹ��
  user_delay_us(500);                                  //������ʱ200us
  user_fmc_sdram_send_cmd(0, FMC_SDRAM_CMD_PALL, 1, 0);    //�����д洢��Ԥ���
  user_fmc_sdram_send_cmd(0, FMC_SDRAM_CMD_AUTOREFRESH_MODE, 8, 0); //������ˢ�´���
  //����ģʽ�Ĵ���,SDRAM��bit0~bit2Ϊָ��ͻ�����ʵĳ��ȣ�
  //bit3Ϊָ��ͻ�����ʵ����ͣ�bit4~bit6ΪCASֵ��bit7��bit8Ϊ����ģʽ
  //bit9Ϊָ����дͻ��ģʽ��bit10��bit11λ����λ
  temp = (u32)SDRAM_MODEREG_BURST_LENGTH_1          | //����ͻ������:1(������1/2/4/8)
         SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   | //����ͻ������:����(����������/����)
         SDRAM_MODEREG_CAS_LATENCY_3           | //����CASֵ:3(������2/3)
         SDRAM_MODEREG_OPERATING_MODE_STANDARD |   //���ò���ģʽ:0,��׼ģʽ
         SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;     //����ͻ��дģʽ:1,�������
  user_fmc_sdram_send_cmd(0, FMC_SDRAM_CMD_LOAD_MODE, 1, temp); //����SDRAM��ģʽ�Ĵ���
  //ˢ��Ƶ�ʼ�����(��SDCLKƵ�ʼ���),���㷽��:
  //COUNT=SDRAMˢ������/����-20=SDRAMˢ������(us)*SDCLKƵ��(Mhz)/����
  //����ʹ�õ�SDRAMˢ������Ϊ64ms,SDCLK=180/2=90Mhz,����Ϊ8192(2^13).
  //����,COUNT=64*1000*90/8192-20=683
  HAL_SDRAM_ProgramRefreshRate(&hsdram1, 683);
}

void user_fmc_sdram_init(void)
{
  #if 0
  FMC_SDRAM_TimingTypeDef SDRAM_Timing;
  hsdram1.Instance = FMC_SDRAM_DEVICE;                           //SDRAM��BANK5,6
  hsdram1.Init.SDBank = FMC_SDRAM_BANK1;                         //SDRAM����BANK5��
  hsdram1.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_9;   //������
  hsdram1.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_13;        //������
  hsdram1.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_16;     //���ݿ��Ϊ16λ
  hsdram1.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4; //һ��4��BANK
  hsdram1.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_3;             //CASΪ3
  hsdram1.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE; //ʧ��д����
  hsdram1.Init.SDClockPeriod = FMC_SDRAM_CLOCK_PERIOD_2;         //SDRAMʱ��ΪHCLK/2=180M/2=90M=11.1ns
  hsdram1.Init.ReadBurst = FMC_SDRAM_RBURST_ENABLE;              //ʹ��ͻ��
  hsdram1.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_1;          //��ͨ����ʱ
  SDRAM_Timing.LoadToActiveDelay = 2;                                 //����ģʽ�Ĵ���������ʱ����ӳ�Ϊ2��ʱ������
  SDRAM_Timing.ExitSelfRefreshDelay = 8;                              //�˳���ˢ���ӳ�Ϊ8��ʱ������
  SDRAM_Timing.SelfRefreshTime = 6;                                   //��ˢ��ʱ��Ϊ6��ʱ������
  SDRAM_Timing.RowCycleDelay = 6;                                     //��ѭ���ӳ�Ϊ6��ʱ������
  SDRAM_Timing.WriteRecoveryTime = 2;                                 //�ָ��ӳ�Ϊ2��ʱ������
  SDRAM_Timing.RPDelay = 2;                                           //��Ԥ����ӳ�Ϊ2��ʱ������
  SDRAM_Timing.RCDDelay = 2;                                          //�е����ӳ�Ϊ2��ʱ������
  HAL_SDRAM_Init(&hsdram1, &SDRAM_Timing);
  #endif
  _user_fmc_sdram_initialization_sequence(&hsdram1);//����SDRAM��ʼ������
}

#endif // ENABLE_SDRAM





