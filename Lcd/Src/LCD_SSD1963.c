#include "LCD_SSD1963.h"
#include "lcd_common.h"
#include "user_debug.h"
#include "lcd.h"

uint16_t  HDP=479;
uint16_t  HT=531;
uint16_t  HPS=43;
uint16_t  LPS=8;
uint8_t   HPW=10;

uint16_t  VDP=271;
uint16_t  VT=288;
uint16_t  VPS=12;
uint16_t  FPS=4;
uint8_t   VPW=10;


void LCD_WR_REG_DATA(int reg,int da)
{
  LCD_WR_REG(reg);
  LCD_WR_DATA(da);
}

uint16_t SSD1963_Get_IC_ID(void)
{
  uint16_t ic_id = 0;
  LCD_WR_REG(0XA1);
  ic_id = LCD_RD_DATA();//dummy read
  USER_DbgLog("1: %x  ",ic_id);
  ic_id = LCD_RD_DATA();//����0X00
  USER_DbgLog("2: %x  ",ic_id);
  ic_id = LCD_RD_DATA();
  USER_DbgLog("3: %x  ",ic_id);
  ic_id <<= 8;
  ic_id|=LCD_RD_DATA();
  USER_DbgLog("4: %x  \r\n",ic_id);
  return ic_id;
}
/**
 * @name SSD1963_Lcd_SetCursor
 * @brief ���ù��λ��
 * @param Xpos ������
 * @param Ypos ������
 */
void SSD1963_Lcd_SetCursor(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2)
{
  LCD_WR_REG(0x002A);//set_column_address,Set the column extent
  LCD_WR_DATA(x1>>8);
  LCD_WR_DATA(x1&0x00ff);
  LCD_WR_DATA(x2>>8);
  LCD_WR_DATA(x2&0x00ff);
  LCD_WR_REG(0x002b);//set_page_address,Set the page extent
  LCD_WR_DATA(y1>>8);
  LCD_WR_DATA(y1&0x00ff);
  LCD_WR_DATA(y2>>8);
  LCD_WR_DATA(y2&0x00ff);
//	LCD_WR_REG(0x002c);//write_memory_start
}

//��������
//Color:Ҫ���������ɫ
//void LCD_Clear(uint16_t Color)
//{
////	uint8_t VH,VL;
//	uint16_t i,j;
////	VH=Color>>8;
////	VL=Color;
//	SSD1963_Lcd_SetCursor(0,0,LCD_W-1,LCD_H-1);
//    for(i=0;i<LCD_W;i++)
//	  {
//	    for (j=0;j<LCD_H;j++)
//	    {
//        LCD_WR_DATA(Color);
//	    }

//	  }
//}
/**
 * @name SSD1963_Lcd_Fill
 * @brief ��ָ��������䵥����ɫ����ͼƬ��������ȵ���
 * @param (xsta,ysta)��ʼ���ꣻ
 * @param (xend,yend)�������ꣻ
 * @param  color Ҫ��ʾ����ɫ
 */
void SSD1963_Lcd_Fill(uint16_t xsta,uint16_t ysta,uint16_t xend,uint16_t yend,uint16_t color)
{
  uint16_t i,j;

  SSD1963_Lcd_SetCursor(xsta,ysta,xend,yend);      //���ù��λ��
  LCD_WriteRAM_Prepare();
  for(i=ysta; i<=yend; i++)
  {
    for(j=xsta; j<=xend; j++)
    {
      LCD_WR_DATA((uint16_t)color);
    }
  }

}
/**
 * @name SSD1963_Lcd_Color_Fill
 * @brief ��ָ�������������ɫ����ɫ���飩����ѡ���ļ���ͼ��ʱ����
 * @param (sx,sy)��ʼ���ꣻ
 * @param (ex,ey)�������ꣻ
 * @param  *color Ҫ������ɫ����ָ��
 */
void SSD1963_Lcd_Color_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey, const uint16_t* color)
{

  ex -= sx;
  X_1963(sx);
  ex += sx;
  ey -= sy;
  Y_1963(sy);
  ey += sy;

  SSD1963_Lcd_SetCursor(sx,sy,ex,ey);      //���ù��λ��
  LCD_WriteRAM_Prepare();
  for(int i = 0; i <= (ey - sy + 1)*( ex - sx + 1); i++ )
    LCD_WR_DATA(color[i]);

}
/**
 * @name SSD1963_Lcd_Scan_Dir
 * @brief ����ˢ��������ʾ����
 * @param width  ��ʾ�����
 * @param height ��ʾ���߶�
 * @param Xcmd ����X����������
          Ycmd ����Y����������
          dir  �������
 */
void SSD1963_Lcd_Scan_Dir(uint16_t width, uint16_t height, uint16_t Xcmd, uint16_t Ycmd, uint8_t dir)
{
  uint16_t regval=0;
  uint16_t dirreg=0;

  switch(dir)
  {
  case L2R_U2D://������,���ϵ���
    regval|=(0<<7)|(0<<6)|(0<<5);
    break;
  case L2R_D2U://������,���µ���
    regval|=(1<<7)|(0<<6)|(0<<5);
    break;
  case R2L_U2D://���ҵ���,���ϵ���
    regval|=(0<<7)|(1<<6)|(0<<5);
    break;
  case R2L_D2U://���ҵ���,���µ���
    regval|=(1<<7)|(1<<6)|(0<<5);
    break;
  case U2D_L2R://���ϵ���,������
    regval|=(0<<7)|(0<<6)|(1<<5);
    break;
  case U2D_R2L://���ϵ���,���ҵ���
    regval|=(0<<7)|(1<<6)|(1<<5);
    break;
  case D2U_L2R://���µ���,������
    regval|=(1<<7)|(0<<6)|(1<<5);
    break;
  case D2U_R2L://���µ���,���ҵ���
    regval|=(1<<7)|(1<<6)|(1<<5);
    break;
  }

  dirreg=0X36;
//  regval|=0X08;//1963��ҪBGR,�޸�24λɫ��ʾ565RGB��ɫ������
  LCD_WR_REG(Xcmd);
  LCD_WR_DATA(0);
  LCD_WR_DATA(0);
  LCD_WR_DATA((width-1)>>8);
  LCD_WR_DATA((width-1)&0XFF);
  LCD_WR_REG(Ycmd);
  LCD_WR_DATA(0);
  LCD_WR_DATA(0);
  LCD_WR_DATA((height-1)>>8);
  LCD_WR_DATA((height-1)&0XFF);

  regval|=1<<12;
  LCD_WriteReg(dirreg,regval);
  //USER_EchoLogStr("dirreg=%d\r\n",LCD_ReadReg(dirreg));
}
/**
 * @name SSD1963_Lcd_Init
 * @brief ��ʾ����ʼ����
          LCD_WR_REG(0x00E2)��ʾ������������ǵ�ַҲ�ǼĴ�����ַ
          LCD_WR_DATA(0x002d)��ʾд�����ݣ����鿴SSD1963�������ֲ�
 * @param None

 */
void SSD1963_Lcd_Init(void)
{
  // ��λ����ֹ��������
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);
  HAL_Delay(50);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
  HAL_Delay(100);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);
  HAL_Delay(50);
  LCD_WR_REG(0x00E2);	//PLL multiplier, set PLL clock to 120M
  LCD_WR_DATA(0x002d);	    //N=0x36 for 6.5M, 0x23 for 10M crystal
  LCD_WR_DATA(0x0002);
  LCD_WR_DATA(0x0004);
  LCD_WR_REG(0x00E0);  // PLL enable
  LCD_WR_DATA(0x0001);
  HAL_Delay(1);//1ms
  LCD_WR_REG(0x00E0);
  LCD_WR_DATA(0x0003);
  HAL_Delay(5);//5ms
  LCD_WR_REG(0x0001);  // software reset
  HAL_Delay(5);//5ms
  LCD_WR_REG(0x00E6);	//PLL setting for PCLK, depends on resolution,����Ƶ�ʿ��Ը�����Ļ��˸������
  LCD_WR_DATA(0x0001);
  LCD_WR_DATA(0x00ff);
  LCD_WR_DATA(0x00be);

  LCD_WR_REG(0x00B0);	//LCD SPECIFICATION
  LCD_WR_DATA(0x0020);//Set TFT panel data width 24bit
  LCD_WR_DATA(0x0000);//Set LCD panel mode:Hsync+Vsync +DE mode;Set TFT type:TFT mode
  LCD_WR_DATA((HDP>>8)&0X00FF);  //Set Width
  LCD_WR_DATA(HDP&0X00FF);
  LCD_WR_DATA((VDP>>8)&0X00FF);  //Set Height
  LCD_WR_DATA(VDP&0X00FF);
  LCD_WR_DATA(0x0000);//Set RGB sequence:RGB(000000)
  HAL_Delay(5);//5ms
  LCD_WR_REG(0x00B4);	//HSYNC
  LCD_WR_DATA((HT>>8)&0X00FF);  //Set HT
  LCD_WR_DATA(HT&0X00FF);
  LCD_WR_DATA((HPS>>8)&0X00FF);  //Set HPS
  LCD_WR_DATA(HPS&0X00FF);
  LCD_WR_DATA(HPW);			   //Set HPW
  LCD_WR_DATA((LPS>>8)&0X00FF);  //SetLPS
  LCD_WR_DATA(LPS&0X00FF);
  LCD_WR_DATA(0x0000);

  LCD_WR_REG(0x00B6);	//VSYNC
  LCD_WR_DATA((VT>>8)&0X00FF);   //Set VT
  LCD_WR_DATA(VT&0X00FF);
  LCD_WR_DATA((VPS>>8)&0X00FF);  //Set VPS
  LCD_WR_DATA(VPS&0X00FF);
  LCD_WR_DATA(VPW);			   //Set VPW
  LCD_WR_DATA((FPS>>8)&0X00FF);  //Set FPS
  LCD_WR_DATA(FPS&0X00FF);


  LCD_WR_REG(0x0036); //rotation
  LCD_WR_DATA(0x0000);

  LCD_WR_REG(0x00F0); //Set pixel data interface
  LCD_WR_DATA(0x0003);//Set the pixel data format to 16bits(565)


  HAL_Delay(5);//5ms


  LCD_WR_REG(0x0029); //display on

  LCD_WR_REG(0x00BE); //set PWM for B/L
  LCD_WR_DATA(0x0006);
  LCD_WR_DATA(0x00f0);
  LCD_WR_DATA(0x0001);
  LCD_WR_DATA(0x00f0);
  LCD_WR_DATA(0x0000);
  LCD_WR_DATA(0x0000);

  LCD_WR_REG(0x00d0);
  LCD_WR_DATA(0x000d);

  //----------LCD RESET---GPIO0-------------------//
  LCD_WR_REG(0x00B8);
  LCD_WR_DATA(0x0000);    //GPIO3=input, GPIO[2:0]=output
  LCD_WR_DATA(0x0001);    //GPIO0 normal

  LCD_WR_REG(0x00BA);
  LCD_WR_DATA(0x0000);
}

