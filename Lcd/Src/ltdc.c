#include "user_common.h"

#ifdef ENABLE_LTDC

#include "../Inc/ltdc.h"
#include "../Inc/lcd.h"

//////////////////////////////////////////////////////////////////////////////////
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//LTDC����
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/7/13
//�汾��V1.2
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved
//********************************************************************************
//�޸�˵��
//V1.1  20161026
//��LTDC_PanelID_Read��������delay�����-O2�Ż���ID���ܳ����BUG
//V1.2  20170606
//����10.1��RGB����֧��
//////////////////////////////////////////////////////////////////////////////////
//LTDC_HandleTypeDef  LTDC_Handler;     //LTDC���
//DMA2D_HandleTypeDef DMA2D_Handler;      //DMA2D���
extern LTDC_HandleTypeDef hltdc;

//���ݲ�ͬ����ɫ��ʽ,����֡��������
#if LCD_PIXFORMAT==LCD_PIXFORMAT_ARGB8888||LCD_PIXFORMAT==LCD_PIXFORMAT_RGB888
  u32 ltdc_lcd_framebuf[1280][800] __attribute__((at(LCD_FRAME_BUF_ADDR))); //����������ֱ���ʱ,LCD�����֡���������С
#else
  u16 ltdc_lcd_framebuf[1280][800] __attribute__((at(LCD_FRAME_BUF_ADDR))); //����������ֱ���ʱ,LCD�����֡���������С
#endif

u32 *ltdc_framebuf[2];          //LTDC LCD֡��������ָ��,����ָ���Ӧ��С���ڴ�����
_ltdc_dev lcdltdc;            //����LCD LTDC����Ҫ����

//��LCD����
//lcd_switch:1 ��,0���ر�
void LTDC_Switch(u8 sw)
{
  if (sw == 1) __HAL_LTDC_ENABLE(&hltdc);
  else if (sw == 0)__HAL_LTDC_DISABLE(&hltdc);
}

//����ָ����
//layerx:���,0,��һ��; 1,�ڶ���
//sw:1 ��;0�ر�
void LTDC_Layer_Switch(u8 layerx, u8 sw)
{
  if (sw == 1) __HAL_LTDC_LAYER_ENABLE(&hltdc, layerx);
  else if (sw == 0) __HAL_LTDC_LAYER_DISABLE(&hltdc, layerx);

  __HAL_LTDC_RELOAD_CONFIG(&hltdc);
}

//ѡ���
//layerx:���;0,��һ��;1,�ڶ���;
void LTDC_Select_Layer(u8 layerx)
{
  lcdltdc.activelayer = layerx;
}

//����LCD��ʾ����
//dir:0,������1,����
void LTDC_Display_Dir(u8 dir)
{
  lcdltdc.dir = dir; //��ʾ����

  if (dir == 0)   //����
  {
    lcdltdc.width = lcdltdc.pheight;
    lcdltdc.height = lcdltdc.pwidth;
  }
  else if (dir == 1) //����
  {
    lcdltdc.width = lcdltdc.pwidth;
    lcdltdc.height = lcdltdc.pheight;
  }
}

//���㺯��
//x,y:д������
//color:��ɫֵ
void LTDC_Draw_Point(u16 x, u16 y, u32 color)
{
  #if LCD_PIXFORMAT==LCD_PIXFORMAT_ARGB8888||LCD_PIXFORMAT==LCD_PIXFORMAT_RGB888

  if (lcdltdc.dir) //����
  {
    *(u32 *)((u32)ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * y + x)) = color;
  }
  else        //����
  {
    *(u32 *)((u32)ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * (lcdltdc.pheight - x) + y)) = color;
  }

  #else

  if (lcdltdc.dir) //����
  {
    *(u16 *)((u32)ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * y + x)) = color;
  }
  else        //����
  {
    *(u16 *)((u32)ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * (lcdltdc.pheight - x - 1) + y)) = color;
  }

  #endif
}

//���㺯��
//x,y:��ȡ�������
//����ֵ:��ɫֵ
u32 LTDC_Read_Point(u16 x, u16 y)
{
  #if LCD_PIXFORMAT==LCD_PIXFORMAT_ARGB8888||LCD_PIXFORMAT==LCD_PIXFORMAT_RGB888

  if (lcdltdc.dir) //����
  {
    return *(u32 *)((u32)ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * y + x));
  }
  else        //����
  {
    return *(u32 *)((u32)ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * (lcdltdc.pheight - x) + y));
  }

  #else

  if (lcdltdc.dir) //����
  {
    return *(u16 *)((u32)ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * y + x));
  }
  else        //����
  {
    return *(u16 *)((u32)ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * (lcdltdc.pheight - x - 1) + y));
  }

  #endif
}

//LTDC��ʱ
void LTDC_Delay(vu32 i)
{
  while (i > 0)i--;
}

//LTDC������,DMA2D���
//(sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)
//color:Ҫ������ɫ
//��ʱ����ҪƵ���ĵ�����亯��������Ϊ���ٶȣ���亯�����üĴ����汾��
//���������ж�Ӧ�Ŀ⺯���汾�Ĵ��롣
void LTDC_Fill(u16 sx, u16 sy, u16 ex, u16 ey, u32 color)
{
  u32 psx, psy, pex, pey; //��LCD���Ϊ��׼������ϵ,����������仯���仯
  u32 timeout = 0;
  u16 offline;
  u32 addr;

  //����ϵת��
  if (lcdltdc.dir) //����
  {
    psx = sx;
    psy = sy;
    pex = ex;
    pey = ey;
  }
  else      //����
  {
    psx = sy;
    psy = lcdltdc.pheight - ex - 1;
    pex = ey;
    pey = lcdltdc.pheight - sx - 1;
  }

  offline = lcdltdc.pwidth - (pex - psx + 1);
  addr = ((u32)ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * psy + psx));
  __HAL_RCC_DMA2D_CLK_ENABLE(); //ʹ��DM2Dʱ��
  DMA2D->CR &= ~(DMA2D_CR_START); //��ֹͣDMA2D
  DMA2D->CR = DMA2D_R2M;    //�Ĵ������洢��ģʽ
  DMA2D->OPFCCR = LCD_PIXFORMAT; //������ɫ��ʽ
  DMA2D->OOR = offline;     //������ƫ��
  DMA2D->OMAR = addr;     //����洢����ַ
  DMA2D->NLR = (pey - psy + 1) | ((pex - psx + 1) << 16); //�趨�����Ĵ���
  DMA2D->OCOLR = color;         //�趨�����ɫ�Ĵ���
  DMA2D->CR |= DMA2D_CR_START;      //����DMA2D

  while ((DMA2D->ISR & (DMA2D_FLAG_TC)) == 0) //�ȴ��������
  {
    timeout++;

    if (timeout > 0X1FFFFF)break; //��ʱ�˳�
  }

  if (mcu_id == MCU_GD32F450IIH6)
  {
    LTDC_Delay(0x10);
  }

  DMA2D->IFCR |= DMA2D_FLAG_TC; //���������ɱ�־
}
//��ָ����������䵥����ɫ
//(sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)
//color:Ҫ������ɫ
//void LTDC_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u32 color)
//{
//  u32 psx,psy,pex,pey;  //��LCD���Ϊ��׼������ϵ,����������仯���仯
//  u32 timeout=0;
//  u16 offline;
//  u32 addr;
//    if(ex>=lcdltdc.width)ex=lcdltdc.width-1;
//  if(ey>=lcdltdc.height)ey=lcdltdc.height-1;
//  //����ϵת��
//  if(lcdltdc.dir) //����
//  {
//    psx=sx;psy=sy;
//    pex=ex;pey=ey;
//  }else     //����
//  {
//    psx=sy;psy=lcdltdc.pheight-ex-1;
//    pex=ey;pey=lcdltdc.pheight-sx-1;
//  }
//  offline=lcdltdc.pwidth-(pex-psx+1);
//  addr=((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*psy+psx));
//    if(LCD_PIXFORMAT==LCD_PIXEL_FORMAT_RGB565)  //�����RGB565��ʽ�Ļ���Ҫ������ɫת������16bitת��Ϊ32bit��
//    {
//        color=((color&0X0000F800)<<8)|((color&0X000007E0)<<5)|((color&0X0000001F)<<3);
//    }
//  //����DMA2D��ģʽ
//  DMA2D_Handler.Instance=DMA2D;
//  DMA2D_Handler.Init.Mode=DMA2D_R2M;      //�ڴ浽�洢��ģʽ
//  DMA2D_Handler.Init.ColorMode=LCD_PIXFORMAT; //������ɫ��ʽ
//  DMA2D_Handler.Init.OutputOffset=offline;    //���ƫ��
//  HAL_DMA2D_Init(&DMA2D_Handler);              //��ʼ��DMA2D
//    HAL_DMA2D_ConfigLayer(&DMA2D_Handler,lcdltdc.activelayer); //������
//    HAL_DMA2D_Start(&DMA2D_Handler,color,(u32)addr,pex-psx+1,pey-psy+1);//��������
//    HAL_DMA2D_PollForTransfer(&DMA2D_Handler,1000);//��������
//    while((__HAL_DMA2D_GET_FLAG(&DMA2D_Handler,DMA2D_FLAG_TC)==0)&&(timeout<0X5000))//�ȴ�DMA2D���
//    {
//        timeout++;
//    }
//    __HAL_DMA2D_CLEAR_FLAG(&DMA2D_Handler,DMA2D_FLAG_TC);       //���������ɱ�־
//}

//��ָ�����������ָ����ɫ��,DMA2D���
//�˺�����֧��u16,RGB565��ʽ����ɫ�������.
//(sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)
//ע��:sx,ex,���ܴ���lcddev.width-1;sy,ey,���ܴ���lcddev.height-1!!!
//color:Ҫ������ɫ�����׵�ַ
void LTDC_Color_Fill(u16 sx, u16 sy, u16 ex, u16 ey, u16 *color)
{
  u32 psx, psy, pex, pey; //��LCD���Ϊ��׼������ϵ,����������仯���仯
  u32 timeout = 0;
  u16 offline;
  u32 addr;

  //����ϵת��
  if (lcdltdc.dir) //����
  {
    psx = sx;
    psy = sy;
    pex = ex;
    pey = ey;
  }
  else      //����
  {
    psx = sy;
    psy = lcdltdc.pheight - ex - 1;
    pex = ey;
    pey = lcdltdc.pheight - sx - 1;
  }

  offline = lcdltdc.pwidth - (pex - psx + 1);
  addr = ((u32)ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * psy + psx));
  __HAL_RCC_DMA2D_CLK_ENABLE(); //ʹ��DM2Dʱ��
  DMA2D->CR &= ~(DMA2D_CR_START); //��ֹͣDMA2D
  DMA2D->CR = DMA2D_M2M;    //�洢�����洢��ģʽ
  DMA2D->FGPFCCR = LCD_PIXFORMAT; //������ɫ��ʽ
  DMA2D->FGOR = 0;        //ǰ������ƫ��Ϊ0
  DMA2D->OOR = offline;     //������ƫ��
  DMA2D->FGMAR = (u32)color;  //Դ��ַ
  DMA2D->OMAR = addr;     //����洢����ַ
  DMA2D->NLR = (pey - psy + 1) | ((pex - psx + 1) << 16); //�趨�����Ĵ���
  DMA2D->CR |= DMA2D_CR_START;        //����DMA2D

  while ((DMA2D->ISR & (DMA2D_FLAG_TC)) == 0) //�ȴ��������
  {
    timeout++;

    if (timeout > 0X1FFFFF)break; //��ʱ�˳�
  }

  if (mcu_id == MCU_GD32F450IIH6)
  {
    LTDC_Delay(0x10);
  }

  DMA2D->IFCR |= DMA2D_FLAG_TC;     //���������ɱ�־
}

//LCD����
//color:��ɫֵ
void LTDC_Clear(u32 color)
{
  LTDC_Fill(0, 0, lcdltdc.width - 1, lcdltdc.height - 1, color);
}

//LTDCʱ��(Fdclk)���ú���
//Fvco=Fin*pllsain;
//Fdclk=Fvco/pllsair/2*2^pllsaidivr=Fin*pllsain/pllsair/2*2^pllsaidivr;

//Fvco:VCOƵ��
//Fin:����ʱ��Ƶ��һ��Ϊ1Mhz(����ϵͳʱ��PLLM��Ƶ���ʱ��,��ʱ����ͼ)
//pllsain:SAIʱ�ӱ�Ƶϵ��N,ȡֵ��Χ:50~432.
//pllsair:SAIʱ�ӵķ�Ƶϵ��R,ȡֵ��Χ:2~7
//pllsaidivr:LCDʱ�ӷ�Ƶϵ��,ȡֵ��Χ:RCC_PLLSAIDIVR_2/4/8/16,��Ӧ��Ƶ2~16
//����:�ⲿ����Ϊ25M,pllm=25��ʱ��,Fin=1Mhz.
//����:Ҫ�õ�20M��LTDCʱ��,���������:pllsain=400,pllsair=5,pllsaidivr=RCC_PLLSAIDIVR_4
//Fdclk=1*400/5/4=400/20=20Mhz
//����ֵ:0,�ɹ�;1,ʧ�ܡ�
u8 LTDC_Clk_Set(u32 pllsain, u32 pllsair, u32 pllsaidivr)
{
  RCC_PeriphCLKInitTypeDef PeriphClkIniture;
  //LTDC�������ʱ�ӣ���Ҫ�����Լ���ʹ�õ�LCD�����ֲ������ã�
  PeriphClkIniture.PeriphClockSelection = RCC_PERIPHCLK_LTDC; //LTDCʱ��
  PeriphClkIniture.PLLSAI.PLLSAIN = pllsain;
  PeriphClkIniture.PLLSAI.PLLSAIR = pllsair;
  PeriphClkIniture.PLLSAIDivR = pllsaidivr;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkIniture) == HAL_OK) //��������ʱ��
  {
    return 0;   //�ɹ�
  }
  else return 1;  //ʧ��
}

//LTDC,���մ�������,������LCD�������ϵΪ��׼
//ע��:�˺���������LTDC_Layer_Parameter_Config֮��������.
//layerx:��ֵ,0/1.
//sx,sy:��ʼ����
//width,height:��Ⱥ͸߶�
void LTDC_Layer_Window_Config(u8 layerx, u16 sx, u16 sy, u16 width, u16 height)
{
  HAL_LTDC_SetWindowPosition(&hltdc, sx, sy, layerx); //���ô��ڵ�λ��
  HAL_LTDC_SetWindowSize(&hltdc, width, height, layerx); //���ô��ڴ�С
}

//LTDC,������������.
//ע��:�˺���,������LTDC_Layer_Window_Config֮ǰ����.
//layerx:��ֵ,0/1.
//bufaddr:����ɫ֡������ʼ��ַ
//pixformat:��ɫ��ʽ.0,ARGB8888;1,RGB888;2,RGB565;3,ARGB1555;4,ARGB4444;5,L8;6;AL44;7;AL88
//alpha:����ɫAlphaֵ,0,ȫ͸��;255,��͸��
//alpha0:Ĭ����ɫAlphaֵ,0,ȫ͸��;255,��͸��
//bfac1:���ϵ��1,4(100),�㶨��Alpha;6(101),����Alpha*�㶨Alpha
//bfac2:���ϵ��2,5(101),�㶨��Alpha;7(111),����Alpha*�㶨Alpha
//bkcolor:��Ĭ����ɫ,32λ,��24λ��Ч,RGB888��ʽ
//����ֵ:��
void LTDC_Layer_Parameter_Config(u8 layerx, u32 bufaddr, u8 pixformat, u8 alpha, u8 alpha0, u8 bfac1, u8 bfac2, u32 bkcolor)
{
  LTDC_LayerCfgTypeDef pLayerCfg;
  pLayerCfg.WindowX0 = 0;                     //������ʼX����
  pLayerCfg.WindowY0 = 0;                     //������ʼY����
  pLayerCfg.WindowX1 = lcdltdc.pwidth;        //������ֹX����
  pLayerCfg.WindowY1 = lcdltdc.pheight;       //������ֹY����
  pLayerCfg.PixelFormat = pixformat;      //���ظ�ʽ
  pLayerCfg.Alpha = alpha;              //Alphaֵ���ã�0~255,255Ϊ��ȫ��͸��
  pLayerCfg.Alpha0 = alpha0;            //Ĭ��Alphaֵ
  pLayerCfg.BlendingFactor1 = (u32)bfac1 << 8; //���ò���ϵ��
  pLayerCfg.BlendingFactor2 = (u32)bfac2 << 8; //���ò���ϵ��
  pLayerCfg.FBStartAdress = bufaddr;        //���ò���ɫ֡������ʼ��ַ
  pLayerCfg.ImageWidth = lcdltdc.pwidth;      //������ɫ֡�������Ŀ��
  pLayerCfg.ImageHeight = lcdltdc.pheight;    //������ɫ֡�������ĸ߶�
  pLayerCfg.Backcolor.Red = (u8)(bkcolor & 0X00FF0000) >> 16; //������ɫ��ɫ����
  pLayerCfg.Backcolor.Green = (u8)(bkcolor & 0X0000FF00) >> 8; //������ɫ��ɫ����
  pLayerCfg.Backcolor.Blue = (u8)bkcolor & 0X000000FF;    //������ɫ��ɫ����
  HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, layerx); //������ѡ�еĲ�
}

//LCD��ʼ������
void LTDC_Init(void)
{
  u16 lcdid = 0;
  HAL_LTDC_DeInit(&hltdc);
  lcdid = ltdc_id;    //��ȡLCD���ID

  if (lcdid == 0X4342)
  {
    lcdltdc.pwidth = 480;       //�����,��λ:����
    lcdltdc.pheight = 272;      //���߶�,��λ:����
    lcdltdc.hbp = 43;           //ˮƽ����
    lcdltdc.hfp = 8;            //ˮƽǰ��
    lcdltdc.hsw = 1;            //ˮƽͬ�����
    lcdltdc.vbp = 12;           //��ֱ����
    lcdltdc.vfp = 4;            //��ֱǰ��
    lcdltdc.vsw = 10;           //��ֱͬ�����

    if (mcu_id == MCU_GD32F450IIH6)
      LTDC_Clk_Set(288, 4, RCC_PLLSAIDIVR_8); //��������ʱ�� 9Mhz
    else
      LTDC_Clk_Set(288, 4, RCC_PLLSAIDIVR_8); //��������ʱ�� 9Mhz

    //������������.
  }
  else if (lcdid == 0X7084)
  {
    lcdltdc.pwidth = 800;       //�����,��λ:����
    lcdltdc.pheight = 480;      //���߶�,��λ:����
    lcdltdc.hsw = 1;          //ˮƽͬ�����
    lcdltdc.vsw = 1;          //��ֱͬ�����
    lcdltdc.hbp = 46;         //ˮƽ����
    lcdltdc.vbp = 23;         //��ֱ����
    lcdltdc.hfp = 210;        //ˮƽǰ��
    lcdltdc.vfp = 22;         //��ֱǰ��
    LTDC_Clk_Set(396, 3, RCC_PLLSAIDIVR_4); //��������ʱ�� 33M(�����˫��,��Ҫ����DCLK��:18.75Mhz  300/4/4,�Ż�ȽϺ�)
  }
  else if (lcdid == 0X7016)
  {
    lcdltdc.pwidth = 1024;    //�����,��λ:����
    lcdltdc.pheight = 600;    //���߶�,��λ:����
    lcdltdc.hsw = 20;         //ˮƽͬ�����
    lcdltdc.vsw = 3;          //��ֱͬ�����
    lcdltdc.hbp = 140;        //ˮƽ����
    lcdltdc.vbp = 20;         //��ֱ����
    lcdltdc.hfp = 160;        //ˮƽǰ��
    lcdltdc.vfp = 12;         //��ֱǰ��

    if (mcu_id == MCU_GD32F450IIH6)
      LTDC_Clk_Set(110, 2, RCC_PLLSAIDIVR_2); //��������ʱ�� 27.5M
    else
      LTDC_Clk_Set(360, 2, RCC_PLLSAIDIVR_4); //��������ʱ�� 9.9M

    //LTDC_Clk_Set(360,2,RCC_PLLSAIDIVR_4);//��������ʱ��  45Mhz
    //������������.
  }
  else if (lcdid == 0X7018)
  {
    lcdltdc.pwidth = 1280;    //�����,��λ:����
    lcdltdc.pheight = 800;    //���߶�,��λ:����
    //������������.
  }
  else if (lcdid == 0X8017)
  {
    lcdltdc.pwidth = 1024;    //�����,��λ:����
    lcdltdc.pheight = 768;    //���߶�,��λ:����
    //������������.
  }
  else if (lcdid == 0X1018)   //10.1��1280*800 RGB��
  {
    lcdltdc.pwidth = 1280;  //�����,��λ:����
    lcdltdc.pheight = 800;  //���߶�,��λ:����
    lcdltdc.hbp = 140;    //ˮƽ����
    lcdltdc.hfp = 10;       //ˮƽǰ��
    lcdltdc.hsw = 10;     //ˮƽͬ�����
    lcdltdc.vbp = 10;     //��ֱ����
    lcdltdc.vfp = 10;     //��ֱǰ��
    lcdltdc.vsw = 3;      //��ֱͬ�����
    LTDC_Clk_Set(360, 2, RCC_PLLSAIDIVR_4); //��������ʱ��  45Mhz
  }

  lcddev.width = lcdltdc.pwidth;
  lcddev.height = lcdltdc.pheight;
  #if LCD_PIXFORMAT==LCD_PIXFORMAT_ARGB8888||LCD_PIXFORMAT==LCD_PIXFORMAT_RGB888
  ltdc_framebuf[0] = (u32 *)&ltdc_lcd_framebuf;
  lcdltdc.pixsize = 4;      //ÿ������ռ4���ֽ�
  #else
  lcdltdc.pixsize = 2;      //ÿ������ռ2���ֽ�
  ltdc_framebuf[0] = (u32 *)&ltdc_lcd_framebuf;
  #endif
  //LTDC����
  hltdc.Instance = LTDC;
  hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;       //ˮƽͬ������
  hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;       //��ֱͬ������
  hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;       //����ʹ�ܼ���

  if (lcdid == 0X1018)hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IIPC; //����ʱ�Ӽ���
  else hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;   //����ʱ�Ӽ���

  hltdc.Init.HorizontalSync = lcdltdc.hsw - 1;      //ˮƽͬ�����
  hltdc.Init.VerticalSync = lcdltdc.vsw - 1;        //��ֱͬ�����
  hltdc.Init.AccumulatedHBP = lcdltdc.hsw + lcdltdc.hbp - 1; //ˮƽͬ�����ؿ��
  hltdc.Init.AccumulatedVBP = lcdltdc.vsw + lcdltdc.vbp - 1; //��ֱͬ�����ظ߶�
  hltdc.Init.AccumulatedActiveW = lcdltdc.hsw + lcdltdc.hbp + lcdltdc.pwidth - 1; //��Ч���
  hltdc.Init.AccumulatedActiveH = lcdltdc.vsw + lcdltdc.vbp + lcdltdc.pheight - 1; //��Ч�߶�
  hltdc.Init.TotalWidth = lcdltdc.hsw + lcdltdc.hbp + lcdltdc.pwidth + lcdltdc.hfp - 1; //�ܿ��
  hltdc.Init.TotalHeigh = lcdltdc.vsw + lcdltdc.vbp + lcdltdc.pheight + lcdltdc.vfp - 1; //�ܸ߶�
  hltdc.Init.Backcolor.Red = 0;         //��Ļ�������ɫ����
  hltdc.Init.Backcolor.Green = 0;       //��Ļ��������ɫ����
  hltdc.Init.Backcolor.Blue = 0;        //��Ļ����ɫ��ɫ����
  HAL_LTDC_Init(&hltdc);
  //������
  LTDC_Layer_Parameter_Config(0, (u32)ltdc_framebuf[0], LCD_PIXFORMAT, 255, 0, 6, 7, 0X000000); //���������
  LTDC_Layer_Window_Config(0, 0, 0, lcdltdc.pwidth, lcdltdc.pheight); //�㴰������,��LCD�������ϵΪ��׼,��Ҫ����޸�!
  LTDC_Display_Dir(1);      //Ĭ������
  LTDC_Select_Layer(0);       //ѡ���1��
  user_pin_lcd_backlight_ctrl(true);                  //��������
  LTDC_Clear(0XFFFFFFFF);     //����
}


#endif

