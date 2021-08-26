#include "user_common.h"

//////////////////////////////////////////////////////////////////////////////////
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F429������
//NAND��������
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/11/25
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////

//NAND_HandleTypeDef NAND_Handler;    //NAND FLASH���
extern NAND_HandleTypeDef hnand1;
nand_attriute nand_dev;             //nand��Ҫ�����ṹ��

#define NAND_BANK3_CMD      (*(volatile uint8_t *)(NAND_ADDRESS | NAND_CMD))
#define NAND_BANK3_ADDR     (*(volatile uint8_t *)(NAND_ADDRESS | NAND_ADDR))
#define NAND_BANK3_8_BYTE   (*(volatile uint8_t *)(NAND_ADDRESS))
#define NAND_BANK3_32_BYTE  (*(volatile uint32_t *)(NAND_ADDRESS))
//��ʼ��NAND FLASH
u8 NAND_Init(void)
{
  FMC_NAND_PCC_TimingTypeDef ComSpaceTiming, AttSpaceTiming;
  hnand1.Instance = FMC_NAND_DEVICE;
  hnand1.Init.NandBank = FMC_NAND_BANK3;                        //NAND����BANK3��
  hnand1.Init.Waitfeature = FMC_NAND_PCC_WAIT_FEATURE_DISABLE;  //�رյȴ�����
  hnand1.Init.MemoryDataWidth = FMC_NAND_PCC_MEM_BUS_WIDTH_8;   //8λ���ݿ��
  hnand1.Init.EccComputation = FMC_NAND_ECC_DISABLE;            //��ʹ��ECC
  hnand1.Init.ECCPageSize = FMC_NAND_ECC_PAGE_SIZE_2048BYTE;    //ECCҳ��СΪ2k
  hnand1.Init.TCLRSetupTime = 0;                                //����TCLR(tCLR=CLE��RE����ʱ)=(TCLR+TSET+2)*THCLK,THCLK=1/180M=5.5ns
  hnand1.Init.TARSetupTime = 1;                                 //����TAR(tAR=ALE��RE����ʱ)=(TAR+TSET+2)*THCLK,THCLK=1/180M=5.5n��
  ComSpaceTiming.SetupTime = 2;       //����ʱ��
  ComSpaceTiming.WaitSetupTime = 3;   //�ȴ�ʱ��
  ComSpaceTiming.HoldSetupTime = 2;   //����ʱ��
  ComSpaceTiming.HiZSetupTime = 1;    //����̬ʱ��
  AttSpaceTiming.SetupTime = 2;       //����ʱ��
  AttSpaceTiming.WaitSetupTime = 3;   //�ȴ�ʱ��
  AttSpaceTiming.HoldSetupTime = 2;   //����ʱ��
  AttSpaceTiming.HiZSetupTime = 1;    //����̬ʱ��
  HAL_NAND_Init(&hnand1, &ComSpaceTiming, &AttSpaceTiming);
  NAND_Reset();                   //��λNAND
  user_delay_ms(100);
  nand_dev.id = NAND_ReadID();        //��ȡID
  NAND_ModeSet(4);              //����ΪMODE4,����ģʽ

  if (nand_dev.id == MT29F16G08ABABA) //NANDΪMT29F16G08ABABA
  {
    nand_dev.page_totalsize = 4320; //nandһ��page���ܴ�С������spare����
    nand_dev.page_mainsize = 4096;  //nandһ��page����Ч��������С
    nand_dev.page_sparesize = 224; //nandһ��page��spare����С
    nand_dev.block_pagenum = 128; //nandһ��block��������page��Ŀ
    nand_dev.plane_blocknum = 2048; //nandһ��plane��������block��Ŀ
    nand_dev.block_totalnum = 4096; //nand����block��Ŀ
  }
  else if (nand_dev.id == MT29F4G08ABADA) //NANDΪMT29F4G08ABADA
  {
    nand_dev.page_totalsize = 2112; //nandһ��page���ܴ�С������spare����
    nand_dev.page_mainsize = 2048; //nandһ��page����Ч��������С
    nand_dev.page_sparesize = 64; //nandһ��page��spare����С
    nand_dev.block_pagenum = 64;  //nandһ��block��������page��Ŀ
    nand_dev.plane_blocknum = 2048; //nandһ��plane��������block��Ŀ
    nand_dev.block_totalnum = 4096; //nand����block��Ŀ
  }
  else return 1;  //���󣬷���

  return 0;
}



//��ȡNAND FLASH��ID
//����ֵ:0,�ɹ�;
//    ����,ʧ��
u8 NAND_ModeSet(u8 mode)
{
  NAND_BANK3_CMD = NAND_FEATURE; //����������������
  NAND_BANK3_ADDR = 0X01; //��ַΪ0X01,����mode
  NAND_BANK3_8_BYTE = mode;      //P1����,����mode
  NAND_BANK3_8_BYTE = 0;
  NAND_BANK3_8_BYTE = 0;
  NAND_BANK3_8_BYTE = 0;

  if (NAND_WaitForReady() == NSTA_READY)
  {
    return 0;//�ɹ�
  }
  else
  {
    return 1;//ʧ��
  }
}

//��ȡNAND FLASH��ID
//��ͬ��NAND���в�ͬ��������Լ���ʹ�õ�NAND FALSH�����ֲ�����д����
//����ֵ:NAND FLASH��IDֵ
u32 NAND_ReadID(void)
{
  u8 deviceid[5];
  u32 id;
  NAND_BANK3_CMD = NAND_READID; //���Ͷ�ȡID����
  NAND_BANK3_ADDR = 0X00;
  //IDһ����5���ֽ�
  deviceid[0] = NAND_BANK3_8_BYTE;
  deviceid[1] = NAND_BANK3_8_BYTE;
  deviceid[2] = NAND_BANK3_8_BYTE;
  deviceid[3] = NAND_BANK3_8_BYTE;
  deviceid[4] = NAND_BANK3_8_BYTE;
  //þ���NAND FLASH��IDһ��5���ֽڣ�����Ϊ�˷�������ֻȡ4���ֽ����һ��32λ��IDֵ
  //����NAND FLASH�������ֲᣬֻҪ��þ���NAND FLASH����ôһ���ֽ�ID�ĵ�һ���ֽڶ���0X2C
  //�������ǾͿ����������0X2C��ֻȡ�������ֽڵ�IDֵ��
  id = ((u32)deviceid[1]) << 24 | ((u32)deviceid[2]) << 16 | ((u32)deviceid[3]) << 8 | deviceid[4];
  return id;
}
//��NAND״̬
//����ֵ:NAND״ֵ̬
//bit0:0,�ɹ�;1,����(���/����/READ)
//bit6:0,Busy;1,Ready
u8 NAND_ReadStatus(void)
{
  vu8 data = 0;
  NAND_BANK3_CMD = NAND_READSTA; //���Ͷ�״̬����
  data++;
  data++;
  data++;
  data++;
  data++; //����ʱ,��ֹ-O2�Ż�,���µĴ���.
  data = NAND_BANK3_8_BYTE;  //��ȡ״ֵ̬
  return data;
}
//�ȴ�NAND׼����
//����ֵ:NSTA_TIMEOUT �ȴ���ʱ��
//      NSTA_READY    �Ѿ�׼����
u8 NAND_WaitForReady(void)
{
  u8 status = 0;
  vu32 time = 0;

  while (1)           //�ȴ�ready
  {
    status = NAND_ReadStatus(); //��ȡ״ֵ̬

    if (status & NSTA_READY)break;

    time++;

    if (time >= 0X1FFFF)return NSTA_TIMEOUT; //��ʱ
  }

  return NSTA_READY;//׼����
}
//��λNAND
//����ֵ:0,�ɹ�;
//    ����,ʧ��
u8 NAND_Reset(void)
{
  NAND_BANK3_CMD = NAND_RESET; //��λNAND

  if (NAND_WaitForReady() == NSTA_READY)return 0; //��λ�ɹ�
  else return 1;                //��λʧ��
}
//�ȴ�RB�ź�Ϊĳ����ƽ
//rb:0,�ȴ�RB==0
//   1,�ȴ�RB==1
//����ֵ:0,�ɹ�
//       1,��ʱ
u8 NAND_WaitRB(vu8 rb)
{
  //  if (mcu_id == MCU_GD32F450IIH6)
  //  {
  //    NAND_Delay(0x200);
  //    return 0;
  //  }
  //  else if (mcu_id == MCU_STM32F429IGT6)
  //  {
  //    vu16 time = 0;
  //    while (time < 10000)
  //    {
  //      time++;
  //      if (NAND_RB == rb)return 0;
  //    }
  //    return 1;
  //  }
  //  else
  {
    NAND_Delay(0x200);
    return 0;
  }
}
//NAND��ʱ
void NAND_Delay(vu32 i)
{
  while (i > 0)i--;
}
//��ȡNAND Flash��ָ��ҳָ���е�����(main����spare��������ʹ�ô˺���)
//PageNum:Ҫ��ȡ��ҳ��ַ,��Χ:0~(block_pagenum*block_totalnum-1)
//ColNum:Ҫ��ȡ���п�ʼ��ַ(Ҳ����ҳ�ڵ�ַ),��Χ:0~(page_totalsize-1)
//*pBuffer:ָ�����ݴ洢��
//NumByteToRead:��ȡ�ֽ���(���ܿ�ҳ��)
//����ֵ:0,�ɹ�
//    ����,�������
u8 NAND_ReadPage(u32 PageNum, u16 ColNum, u8 *pBuffer, u16 NumByteToRead)
{
  vu16 i = 0;
  u8 res = 0;
  u8 eccnum = 0;  //��Ҫ�����ECC������ÿNAND_ECC_SECTOR_SIZE�ֽڼ���һ��ecc
  u8 eccstart = 0;  //��һ��ECCֵ�����ĵ�ַ��Χ
  u8 errsta = 0;
  u8 *p;
  NAND_BANK3_CMD = NAND_AREA_A;
  //���͵�ַ
  NAND_BANK3_ADDR = (u8)ColNum;
  NAND_BANK3_ADDR = (u8)(ColNum >> 8);
  NAND_BANK3_ADDR = (u8)PageNum;
  NAND_BANK3_ADDR = (u8)(PageNum >> 8);
  NAND_BANK3_ADDR = (u8)(PageNum >> 16);
  NAND_BANK3_CMD = NAND_AREA_TRUE1;
  //�������д����ǵȴ�R/B���ű�Ϊ�͵�ƽ����ʵ��Ҫ����ʱ���õģ��ȴ�NAND����R/B���š���Ϊ������ͨ��
  //��STM32��NWAIT����(NAND��R/B����)����Ϊ��ͨIO��������ͨ����ȡNWAIT���ŵĵ�ƽ���ж�NAND�Ƿ�׼��
  //�����ġ����Ҳ����ģ��ķ������������ٶȺܿ��ʱ���п���NAND��û���ü�����R/B��������ʾNAND��æ
  //��״̬��������ǾͶ�ȡ��R/B����,���ʱ��϶������ģ���ʵ��ȷʵ�ǻ����!���Ҳ���Խ���������
  //���뻻����ʱ����,ֻ������������Ϊ��Ч������û������ʱ������
  res = NAND_WaitRB(0);   //�ȴ�RB=0

  if (res)return NSTA_TIMEOUT; //��ʱ�˳�

  //����2�д����������ж�NAND�Ƿ�׼���õ�
  res = NAND_WaitRB(1);   //�ȴ�RB=1

  if (res)return NSTA_TIMEOUT; //��ʱ�˳�

  if (NumByteToRead % NAND_ECC_SECTOR_SIZE) //����NAND_ECC_SECTOR_SIZE����������������ECCУ��
  {
    //��ȡNAND FLASH�е�ֵ
    for (i = 0; i < NumByteToRead; i++)
    {
      *(vu8 *)pBuffer++ = NAND_BANK3_8_BYTE;
    }
  }
  else
  {
    eccnum = NumByteToRead / NAND_ECC_SECTOR_SIZE;  //�õ�ecc�������
    eccstart = ColNum / NAND_ECC_SECTOR_SIZE;
    p = pBuffer;

    for (res = 0; res < eccnum; res++)
    {
      FMC_Bank2_3->PCR3 |= 1 << 6;        //ʹ��ECCУ��

      for (i = 0; i < NAND_ECC_SECTOR_SIZE; i++) //��ȡNAND_ECC_SECTOR_SIZE������
      {
        *(vu8 *)pBuffer++ = NAND_BANK3_8_BYTE;
      }

      while (!(FMC_Bank2_3->SR3 & (1 << 6)));   //�ȴ�FIFO��

      nand_dev.ecc_hdbuf[res + eccstart] = FMC_Bank2_3->ECCR3; //��ȡӲ��������ECCֵ
      FMC_Bank2_3->PCR3 &= ~(1 << 6);       //��ֹECCУ��
    }

    i = nand_dev.page_mainsize + 0X10 + eccstart * 4; //��spare����0X10λ�ÿ�ʼ��ȡ֮ǰ�洢��eccֵ
    NAND_Delay(30);//�ȴ�tADL
    NAND_BANK3_CMD = 0X05;   //�����ָ��
    //���͵�ַ
    NAND_BANK3_ADDR = (u8)i;
    NAND_BANK3_ADDR = (u8)(i >> 8);
    NAND_BANK3_CMD = 0XE0;   //��ʼ������
    NAND_Delay(30);//�ȴ�tADL
    pBuffer = (u8 *)&nand_dev.ecc_rdbuf[eccstart];

    for (i = 0; i < 4 * eccnum; i++)      //��ȡ�����ECCֵ
    {
      *(vu8 *)pBuffer++ = NAND_BANK3_8_BYTE;
    }

    for (i = 0; i < eccnum; i++)        //����ECC
    {
      if (nand_dev.ecc_rdbuf[i + eccstart] != nand_dev.ecc_hdbuf[i + eccstart]) //�����,��ҪУ��
      {
        printf("err hd,rd:0x%x,0x%x\r\n", nand_dev.ecc_hdbuf[i + eccstart], nand_dev.ecc_rdbuf[i + eccstart]);
        printf("eccnum,eccstart:%d,%d\r\n", eccnum, eccstart);
        printf("PageNum,ColNum:%d,%d\r\n", PageNum, ColNum);
        res = NAND_ECC_Correction(p + NAND_ECC_SECTOR_SIZE * i, nand_dev.ecc_rdbuf[i + eccstart], nand_dev.ecc_hdbuf[i + eccstart]); //ECCУ��

        if (res)errsta = NSTA_ECC2BITERR;     //���2BIT������ECC����
        else errsta = NSTA_ECC1BITERR;      //���1BIT ECC����
      }
    }
  }

  if (NAND_WaitForReady() != NSTA_READY)errsta = NSTA_ERROR; //ʧ��

  return errsta;  //�ɹ�
}
//��ȡNAND Flash��ָ��ҳָ���е�����(main����spare��������ʹ�ô˺���),���Ա�(FTL����ʱ��Ҫ)
//PageNum:Ҫ��ȡ��ҳ��ַ,��Χ:0~(block_pagenum*block_totalnum-1)
//ColNum:Ҫ��ȡ���п�ʼ��ַ(Ҳ����ҳ�ڵ�ַ),��Χ:0~(page_totalsize-1)
//CmpVal:Ҫ�Աȵ�ֵ,��u32Ϊ��λ
//NumByteToRead:��ȡ����(��4�ֽ�Ϊ��λ,���ܿ�ҳ��)
//NumByteEqual:�ӳ�ʼλ�ó�����CmpValֵ��ͬ�����ݸ���
//����ֵ:0,�ɹ�
//    ����,�������
u8 NAND_ReadPageComp(u32 PageNum, u16 ColNum, u32 CmpVal, u16 NumByteToRead, u16 *NumByteEqual)
{
  u16 i = 0;
  u8 res = 0;
  NAND_BANK3_CMD = NAND_AREA_A;
  //���͵�ַ
  NAND_BANK3_ADDR = (u8)ColNum;
  NAND_BANK3_ADDR = (u8)(ColNum >> 8);
  NAND_BANK3_ADDR = (u8)PageNum;
  NAND_BANK3_ADDR = (u8)(PageNum >> 8);
  NAND_BANK3_ADDR = (u8)(PageNum >> 16);
  NAND_BANK3_CMD = NAND_AREA_TRUE1;
  //�������д����ǵȴ�R/B���ű�Ϊ�͵�ƽ����ʵ��Ҫ����ʱ���õģ��ȴ�NAND����R/B���š���Ϊ������ͨ��
  //��STM32��NWAIT����(NAND��R/B����)����Ϊ��ͨIO��������ͨ����ȡNWAIT���ŵĵ�ƽ���ж�NAND�Ƿ�׼��
  //�����ġ����Ҳ����ģ��ķ������������ٶȺܿ��ʱ���п���NAND��û���ü�����R/B��������ʾNAND��æ
  //��״̬��������ǾͶ�ȡ��R/B����,���ʱ��϶������ģ���ʵ��ȷʵ�ǻ����!���Ҳ���Խ���������
  //���뻻����ʱ����,ֻ������������Ϊ��Ч������û������ʱ������
  res = NAND_WaitRB(0);   //�ȴ�RB=0

  if (res)return NSTA_TIMEOUT; //��ʱ�˳�

  //����2�д����������ж�NAND�Ƿ�׼���õ�
  res = NAND_WaitRB(1);   //�ȴ�RB=1

  if (res)return NSTA_TIMEOUT; //��ʱ�˳�

  for (i = 0; i < NumByteToRead; i++) //��ȡ����,ÿ�ζ�4�ֽ�
  {
    if (NAND_BANK3_32_BYTE != CmpVal)break; //������κ�һ��ֵ,��CmpVal�����,���˳�.
  }

  *NumByteEqual = i;        //��CmpValֵ��ͬ�ĸ���

  if (NAND_WaitForReady() != NSTA_READY)return NSTA_ERROR; //ʧ��

  return 0; //�ɹ�
}
//��NANDһҳ��д��ָ�����ֽڵ�����(main����spare��������ʹ�ô˺���)
//PageNum:Ҫд���ҳ��ַ,��Χ:0~(block_pagenum*block_totalnum-1)
//ColNum:Ҫд����п�ʼ��ַ(Ҳ����ҳ�ڵ�ַ),��Χ:0~(page_totalsize-1)
//pBbuffer:ָ�����ݴ洢��
//NumByteToWrite:Ҫд����ֽ�������ֵ���ܳ�����ҳʣ���ֽ���������
//����ֵ:0,�ɹ�
//    ����,�������
u8 NAND_WritePage(u32 PageNum, u16 ColNum, u8 *pBuffer, u16 NumByteToWrite)
{
  vu16 i = 0;
  u8 res = 0;
  u8 eccnum = 0;  //��Ҫ�����ECC������ÿNAND_ECC_SECTOR_SIZE�ֽڼ���һ��ecc
  u8 eccstart = 0;  //��һ��ECCֵ�����ĵ�ַ��Χ
  NAND_BANK3_CMD = NAND_WRITE0;
  //���͵�ַ
  NAND_BANK3_ADDR = (u8)ColNum;
  NAND_BANK3_ADDR = (u8)(ColNum >> 8);
  NAND_BANK3_ADDR = (u8)PageNum;
  NAND_BANK3_ADDR = (u8)(PageNum >> 8);
  NAND_BANK3_ADDR = (u8)(PageNum >> 16);
  NAND_Delay(30);//�ȴ�tADL

  if (NumByteToWrite % NAND_ECC_SECTOR_SIZE) //����NAND_ECC_SECTOR_SIZE����������������ECCУ��
  {
    for (i = 0; i < NumByteToWrite; i++) //д������
    {
      NAND_BANK3_8_BYTE = *(vu8 *)pBuffer++;
    }
  }
  else
  {
    eccnum = NumByteToWrite / NAND_ECC_SECTOR_SIZE; //�õ�ecc�������
    eccstart = ColNum / NAND_ECC_SECTOR_SIZE;

    for (res = 0; res < eccnum; res++)
    {
      FMC_Bank2_3->PCR3 |= 1 << 6;        //ʹ��ECCУ��

      for (i = 0; i < NAND_ECC_SECTOR_SIZE; i++) //д��NAND_ECC_SECTOR_SIZE������
      {
        NAND_BANK3_8_BYTE = *(vu8 *)pBuffer++;
      }

      while (!(FMC_Bank2_3->SR3 & (1 << 6)));   //�ȴ�FIFO��

      nand_dev.ecc_hdbuf[res + eccstart] = FMC_Bank2_3->ECCR3; //��ȡӲ��������ECCֵ
      FMC_Bank2_3->PCR3 &= ~(1 << 6);       //��ֹECCУ��
    }

    i = nand_dev.page_mainsize + 0X10 + eccstart * 4; //����д��ECC��spare����ַ
    NAND_Delay(30);//�ȴ�
    NAND_BANK3_CMD = 0X85;   //���дָ��
    //���͵�ַ
    NAND_BANK3_ADDR = (u8)i;
    NAND_BANK3_ADDR = (u8)(i >> 8);
    NAND_Delay(30);//�ȴ�tADL
    pBuffer = (u8 *)&nand_dev.ecc_hdbuf[eccstart];

    for (i = 0; i < eccnum; i++)  //д��ECC
    {
      for (res = 0; res < 4; res++)
      {
        NAND_BANK3_8_BYTE = *(vu8 *)pBuffer++;
      }
    }
  }

  NAND_BANK3_CMD = NAND_WRITE_TURE1;

  if (NAND_WaitForReady() != NSTA_READY)return NSTA_ERROR; //ʧ��

  return 0;//�ɹ�
}
//��NANDһҳ�е�ָ����ַ��ʼ,д��ָ�����ȵĺ㶨����
//PageNum:Ҫд���ҳ��ַ,��Χ:0~(block_pagenum*block_totalnum-1)
//ColNum:Ҫд����п�ʼ��ַ(Ҳ����ҳ�ڵ�ַ),��Χ:0~(page_totalsize-1)
//cval:Ҫд���ָ������
//NumByteToWrite:Ҫд�������(��4�ֽ�Ϊ��λ)
//����ֵ:0,�ɹ�
//    ����,�������
u8 NAND_WritePageConst(u32 PageNum, u16 ColNum, u32 cval, u16 NumByteToWrite)
{
  u16 i = 0;
  NAND_BANK3_CMD = NAND_WRITE0;
  //���͵�ַ
  NAND_BANK3_ADDR = (u8)ColNum;
  NAND_BANK3_ADDR = (u8)(ColNum >> 8);
  NAND_BANK3_ADDR = (u8)PageNum;
  NAND_BANK3_ADDR = (u8)(PageNum >> 8);
  NAND_BANK3_ADDR = (u8)(PageNum >> 16);
  NAND_Delay(30);//�ȴ�tADL

  for (i = 0; i < NumByteToWrite; i++) //д������,ÿ��д4�ֽ�
  {
    NAND_BANK3_32_BYTE = cval;
  }

  NAND_BANK3_CMD = NAND_WRITE_TURE1;

  if (NAND_WaitForReady() != NSTA_READY)return NSTA_ERROR; //ʧ��

  return 0;//�ɹ�
}
//��һҳ���ݿ�������һҳ,��д��������
//ע��:Դҳ��Ŀ��ҳҪ��ͬһ��Plane�ڣ�
//Source_PageNo:Դҳ��ַ,��Χ:0~(block_pagenum*block_totalnum-1)
//Dest_PageNo:Ŀ��ҳ��ַ,��Χ:0~(block_pagenum*block_totalnum-1)
//����ֵ:0,�ɹ�
//    ����,�������
u8 NAND_CopyPageWithoutWrite(u32 Source_PageNum, u32 Dest_PageNum)
{
  u8 res = 0;
  u16 source_block = 0, dest_block = 0;
  //�ж�Դҳ��Ŀ��ҳ�Ƿ���ͬһ��plane��
  source_block = Source_PageNum / nand_dev.block_pagenum;
  dest_block = Dest_PageNum / nand_dev.block_pagenum;

  if ((source_block % 2) != (dest_block % 2))return NSTA_ERROR; //����ͬһ��plane��

  NAND_BANK3_CMD = NAND_MOVEDATA_CMD0; //��������0X00
  //����Դҳ��ַ
  NAND_BANK3_ADDR = (u8)0;
  NAND_BANK3_ADDR = (u8)0;
  NAND_BANK3_ADDR = (u8)Source_PageNum;
  NAND_BANK3_ADDR = (u8)(Source_PageNum >> 8);
  NAND_BANK3_ADDR = (u8)(Source_PageNum >> 16);
  NAND_BANK3_CMD = NAND_MOVEDATA_CMD1; //��������0X35
  //�������д����ǵȴ�R/B���ű�Ϊ�͵�ƽ����ʵ��Ҫ����ʱ���õģ��ȴ�NAND����R/B���š���Ϊ������ͨ��
  //��STM32��NWAIT����(NAND��R/B����)����Ϊ��ͨIO��������ͨ����ȡNWAIT���ŵĵ�ƽ���ж�NAND�Ƿ�׼��
  //�����ġ����Ҳ����ģ��ķ������������ٶȺܿ��ʱ���п���NAND��û���ü�����R/B��������ʾNAND��æ
  //��״̬��������ǾͶ�ȡ��R/B����,���ʱ��϶������ģ���ʵ��ȷʵ�ǻ����!���Ҳ���Խ���������
  //���뻻����ʱ����,ֻ������������Ϊ��Ч������û������ʱ������
  res = NAND_WaitRB(0);   //�ȴ�RB=0

  if (res)return NSTA_TIMEOUT; //��ʱ�˳�

  //����2�д����������ж�NAND�Ƿ�׼���õ�
  res = NAND_WaitRB(1);   //�ȴ�RB=1

  if (res)return NSTA_TIMEOUT; //��ʱ�˳�

  NAND_BANK3_CMD = NAND_MOVEDATA_CMD2; //��������0X85
  //����Ŀ��ҳ��ַ
  NAND_BANK3_ADDR = (u8)0;
  NAND_BANK3_ADDR = (u8)0;
  NAND_BANK3_ADDR = (u8)Dest_PageNum;
  NAND_BANK3_ADDR = (u8)(Dest_PageNum >> 8);
  NAND_BANK3_ADDR = (u8)(Dest_PageNum >> 16);
  NAND_BANK3_CMD = NAND_MOVEDATA_CMD3; //��������0X10

  if (NAND_WaitForReady() != NSTA_READY)return NSTA_ERROR; //NANDδ׼����

  return 0;//�ɹ�
}

//��һҳ���ݿ�������һҳ,���ҿ���д������
//ע��:Դҳ��Ŀ��ҳҪ��ͬһ��Plane�ڣ�
//Source_PageNo:Դҳ��ַ,��Χ:0~(block_pagenum*block_totalnum-1)
//Dest_PageNo:Ŀ��ҳ��ַ,��Χ:0~(block_pagenum*block_totalnum-1)
//ColNo:ҳ���е�ַ,��Χ:0~(page_totalsize-1)
//pBuffer:Ҫд�������
//NumByteToWrite:Ҫд������ݸ���
//����ֵ:0,�ɹ�
//    ����,�������
u8 NAND_CopyPageWithWrite(u32 Source_PageNum, u32 Dest_PageNum, u16 ColNum, u8 *pBuffer, u16 NumByteToWrite)
{
  u8 res = 0;
  vu16 i = 0;
  u16 source_block = 0, dest_block = 0;
  u8 eccnum = 0;  //��Ҫ�����ECC������ÿNAND_ECC_SECTOR_SIZE�ֽڼ���һ��ecc
  u8 eccstart = 0;  //��һ��ECCֵ�����ĵ�ַ��Χ
  //�ж�Դҳ��Ŀ��ҳ�Ƿ���ͬһ��plane��
  source_block = Source_PageNum / nand_dev.block_pagenum;
  dest_block = Dest_PageNum / nand_dev.block_pagenum;

  if ((source_block % 2) != (dest_block % 2))return NSTA_ERROR; //����ͬһ��plane��

  NAND_BANK3_CMD = NAND_MOVEDATA_CMD0; //��������0X00
  //����Դҳ��ַ
  NAND_BANK3_ADDR = (u8)0;
  NAND_BANK3_ADDR = (u8)0;
  NAND_BANK3_ADDR = (u8)Source_PageNum;
  NAND_BANK3_ADDR = (u8)(Source_PageNum >> 8);
  NAND_BANK3_ADDR = (u8)(Source_PageNum >> 16);
  NAND_BANK3_CMD = NAND_MOVEDATA_CMD1; //��������0X35
  //�������д����ǵȴ�R/B���ű�Ϊ�͵�ƽ����ʵ��Ҫ����ʱ���õģ��ȴ�NAND����R/B���š���Ϊ������ͨ��
  //��STM32��NWAIT����(NAND��R/B����)����Ϊ��ͨIO��������ͨ����ȡNWAIT���ŵĵ�ƽ���ж�NAND�Ƿ�׼��
  //�����ġ����Ҳ����ģ��ķ������������ٶȺܿ��ʱ���п���NAND��û���ü�����R/B��������ʾNAND��æ
  //��״̬��������ǾͶ�ȡ��R/B����,���ʱ��϶������ģ���ʵ��ȷʵ�ǻ����!���Ҳ���Խ���������
  //���뻻����ʱ����,ֻ������������Ϊ��Ч������û������ʱ������
  res = NAND_WaitRB(0);   //�ȴ�RB=0

  if (res)return NSTA_TIMEOUT; //��ʱ�˳�

  //����2�д����������ж�NAND�Ƿ�׼���õ�
  res = NAND_WaitRB(1);   //�ȴ�RB=1

  if (res)return NSTA_TIMEOUT; //��ʱ�˳�

  NAND_BANK3_CMD = NAND_MOVEDATA_CMD2; //��������0X85
  //����Ŀ��ҳ��ַ
  NAND_BANK3_ADDR = (u8)ColNum;
  NAND_BANK3_ADDR = (u8)(ColNum >> 8);
  NAND_BANK3_ADDR = (u8)Dest_PageNum;
  NAND_BANK3_ADDR = (u8)(Dest_PageNum >> 8);
  NAND_BANK3_ADDR = (u8)(Dest_PageNum >> 16);
  //����ҳ���е�ַ
  NAND_Delay(30);//�ȴ�tADL

  if (NumByteToWrite % NAND_ECC_SECTOR_SIZE) //����NAND_ECC_SECTOR_SIZE����������������ECCУ��
  {
    for (i = 0; i < NumByteToWrite; i++) //д������
    {
      NAND_BANK3_8_BYTE = *(vu8 *)pBuffer++;
    }
  }
  else
  {
    eccnum = NumByteToWrite / NAND_ECC_SECTOR_SIZE; //�õ�ecc�������
    eccstart = ColNum / NAND_ECC_SECTOR_SIZE;

    for (res = 0; res < eccnum; res++)
    {
      FMC_Bank2_3->PCR3 |= 1 << 6;        //ʹ��ECCУ��

      for (i = 0; i < NAND_ECC_SECTOR_SIZE; i++) //д��NAND_ECC_SECTOR_SIZE������
      {
        NAND_BANK3_8_BYTE = *(vu8 *)pBuffer++;
      }

      while (!(FMC_Bank2_3->SR3 & (1 << 6)));   //�ȴ�FIFO��

      nand_dev.ecc_hdbuf[res + eccstart] = FMC_Bank2_3->ECCR3; //��ȡӲ��������ECCֵ
      FMC_Bank2_3->PCR3 &= ~(1 << 6);       //��ֹECCУ��
    }

    i = nand_dev.page_mainsize + 0X10 + eccstart * 4; //����д��ECC��spare����ַ
    NAND_Delay(30);//�ȴ�
    NAND_BANK3_CMD = 0X85;   //���дָ��
    //���͵�ַ
    NAND_BANK3_ADDR = (u8)i;
    NAND_BANK3_ADDR = (u8)(i >> 8);
    NAND_Delay(30);//�ȴ�tADL
    pBuffer = (u8 *)&nand_dev.ecc_hdbuf[eccstart];

    for (i = 0; i < eccnum; i++)  //д��ECC
    {
      for (res = 0; res < 4; res++)
      {
        NAND_BANK3_8_BYTE = *(vu8 *)pBuffer++;
      }
    }
  }

  NAND_BANK3_CMD = NAND_MOVEDATA_CMD3; //��������0X10

  if (NAND_WaitForReady() != NSTA_READY)return NSTA_ERROR; //ʧ��

  return 0; //�ɹ�
}
//��ȡspare���е�����
//PageNum:Ҫд���ҳ��ַ,��Χ:0~(block_pagenum*block_totalnum-1)
//ColNum:Ҫд���spare����ַ(spare�����ĸ���ַ),��Χ:0~(page_sparesize-1)
//pBuffer:�������ݻ�����
//NumByteToRead:Ҫ��ȡ���ֽ���(������page_sparesize)
//����ֵ:0,�ɹ�
//    ����,�������
u8 NAND_ReadSpare(u32 PageNum, u16 ColNum, u8 *pBuffer, u16 NumByteToRead)
{
  u8 temp = 0;
  u8 remainbyte = 0;
  remainbyte = nand_dev.page_sparesize - ColNum;

  if (NumByteToRead > remainbyte) NumByteToRead = remainbyte; //ȷ��Ҫд����ֽ���������spareʣ��Ĵ�С

  temp = NAND_ReadPage(PageNum, ColNum + nand_dev.page_mainsize, pBuffer, NumByteToRead); //��ȡ����
  return temp;
}
//��spare����д����
//PageNum:Ҫд���ҳ��ַ,��Χ:0~(block_pagenum*block_totalnum-1)
//ColNum:Ҫд���spare����ַ(spare�����ĸ���ַ),��Χ:0~(page_sparesize-1)
//pBuffer:Ҫд��������׵�ַ
//NumByteToWrite:Ҫд����ֽ���(������page_sparesize)
//����ֵ:0,�ɹ�
//    ����,ʧ��
u8 NAND_WriteSpare(u32 PageNum, u16 ColNum, u8 *pBuffer, u16 NumByteToWrite)
{
  u8 temp = 0;
  u8 remainbyte = 0;
  remainbyte = nand_dev.page_sparesize - ColNum;

  if (NumByteToWrite > remainbyte) NumByteToWrite = remainbyte; //ȷ��Ҫ��ȡ���ֽ���������spareʣ��Ĵ�С

  temp = NAND_WritePage(PageNum, ColNum + nand_dev.page_mainsize, pBuffer, NumByteToWrite); //��ȡ
  return temp;
}
//����һ����
//BlockNum:Ҫ������BLOCK���,��Χ:0-(block_totalnum-1)
//����ֵ:0,�����ɹ�
//    ����,����ʧ��
u8 NAND_EraseBlock(u32 BlockNum)
{
  if (nand_dev.id == MT29F16G08ABABA)BlockNum <<= 7; //�����ַת��Ϊҳ��ַ
  else if (nand_dev.id == MT29F4G08ABADA)BlockNum <<= 6;

  NAND_BANK3_CMD = NAND_ERASE0;
  //���Ϳ��ַ
  NAND_BANK3_ADDR = (u8)BlockNum;
  NAND_BANK3_ADDR = (u8)(BlockNum >> 8);
  NAND_BANK3_ADDR = (u8)(BlockNum >> 16);
  NAND_BANK3_CMD = NAND_ERASE1;

  if (NAND_WaitForReady() != NSTA_READY)return NSTA_ERROR; //ʧ��

  return 0; //�ɹ�
}
//ȫƬ����NAND FLASH
void NAND_EraseChip(void)
{
  u8 status;
  u16 i = 0;

  for (i = 0; i < nand_dev.block_totalnum; i++) //ѭ���������еĿ�
  {
    status = NAND_EraseBlock(i);

    if (status)
    {
      printf("Erase %d block fail!!��������Ϊ%d\r\n", i, status); //����ʧ��
    }
  }
}

//��ȡECC������λ/ż��λ
//oe:0,ż��λ
//   1,����λ
//eccval:�����eccֵ
//����ֵ:������eccֵ(���16λ)
u16 NAND_ECC_Get_OE(u8 oe, u32 eccval)
{
  u8 i;
  u16 ecctemp = 0;

  for (i = 0; i < 24; i++)
  {
    if ((i % 2) == oe)
    {
      if ((eccval >> i) & 0X01)ecctemp += 1 << (i >> 1);
    }
  }

  return ecctemp;
}
//ECCУ������
//eccrd:��ȡ����,ԭ�������ECCֵ
//ecccl:��ȡ����ʱ,Ӳ�������ECCֻ
//����ֵ:0,����������
//    ����,ECC����(�д���2��bit�Ĵ���,�޷��ָ�)
u8 NAND_ECC_Correction(u8 *data_buf, u32 eccrd, u32 ecccl)
{
  u16 eccrdo, eccrde, eccclo, ecccle;
  u16 eccchk = 0;
  u16 errorpos = 0;
  u32 bytepos = 0;
  eccrdo = NAND_ECC_Get_OE(1, eccrd); //��ȡeccrd������λ
  eccrde = NAND_ECC_Get_OE(0, eccrd); //��ȡeccrd��ż��λ
  eccclo = NAND_ECC_Get_OE(1, ecccl); //��ȡecccl������λ
  ecccle = NAND_ECC_Get_OE(0, ecccl); //��ȡecccl��ż��λ
  eccchk = eccrdo ^ eccrde ^ eccclo ^ ecccle;

  if (eccchk == 0XFFF) //ȫ1,˵��ֻ��1bit ECC����
  {
    errorpos = eccrdo ^ eccclo;
    printf("errorpos:%d\r\n", errorpos);
    bytepos = errorpos / 8;
    data_buf[bytepos] ^= 1 << (errorpos % 8);
  }
  else        //����ȫ1,˵��������2bit ECC����,�޷��޸�
  {
    printf("2bit ecc error or more\r\n");
    return 1;
  }

  return 0;
}







