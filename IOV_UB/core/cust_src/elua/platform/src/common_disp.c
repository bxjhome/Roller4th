/**************************************************************************
 *              Copyright (C), AirM2M Tech. Co., Ltd.
 *
 * Name:    common_disp.c
 * Author:  liweiqiang
 * Version: V0.1
 * Date:    2013/3/26
 *
 * Description:
 *          display �ӿ�ʵ��
 **************************************************************************/

 #if defined LUA_DISP_LIB

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "type.h"
#include "platform.h"
#include "platform_disp.h"
#include "platform_image.h"
#include "platform_lcd.h"
#include "LzmaLib.h"
/*+\NEW\zhuth\2014.2.16\֧��pngͼƬ����ʾ*/
#ifdef AM_LPNG_SUPPORT
#include "png.h"
#endif
/*-\NEW\zhuth\2014.2.16\֧��pngͼƬ����ʾ*/


#define MAX_FONTS       10


PlatformLcdBus lcd_bus;

/*+\NEW\2013.4.10\���Ӻڰ�����ʾ֧�� */
// 1bitλͼ������֯��ʽͬwin32 1bit bmp
typedef struct DispBitmapTag
{
    u16 width;
    u16 height;
    u8 bpp;
    const u8 *data;
}DispBitmap;
/*-\NEW\2013.4.10\���Ӻڰ�����ʾ֧�� */

typedef struct FontInfoTag
{
    u8        width;
    u8        height;
    u8        size;
    u16       start;
    u16       end;
    const u8       *data;
}FontInfo;

static const u8 blankChar[16*16/8] = {0};

// font 
// ����16 ascii 0x20~0x7e
static const u8 sansFont16Data[]=
{
#include "font.dat"
};

static const FontInfo sansFont16 = 
{
    8,
    16,
    8*16/8,
    0x20,
    0x7E,
    sansFont16Data,
};

// ��������16
#if defined(FONT_HZ_COMPRESS)
static const u8 sansHzFont16DataZip[] =
{
#include "fonthz.zip.dat"
};

static const u8 *sansHzFont16Data = NULL;
#else
static const u8 sansHzFont16Data[] =
{
#include "fonthz.dat"
};

/*+\NEW\liweiqiang\2013.12.18\�������ı����ŵ���ʾ֧�� */
static const u8 sansHzFont16ExtData[] = 
{
    #include "fonthzext.dat"
};

/*��������С��������*/
static const u16 sansHzFont16ExtOffset[] =
{
//"������������������������������������������������������������"
    0xA1A2,0xA1A3,0xA1AA,0xA1AD,0xA1AE,0xA1AF,0xA1B0,0xA1B1,
    0xA1B2,0xA1B3,0xA1B4,0xA1B5,0xA1B6,0xA1B7,0xA1B8,0xA1B9,
    0xA1BA,0xA1BB,0xA1BE,0xA1BF,0xA3A1,0xA3A8,0xA3A9,0xA3AC,
    0xA3AD,0xA3AE,0xA3BA,0xA3BB,0xA3BF,0xE0C5
};
/*-\NEW\liweiqiang\2013.12.18\�������ı����ŵ���ʾ֧�� */
#endif

static FontInfo sansHzFont16 =
{
    16,
    16,
    (16+7)/8*16,
    0,
    0,
    NULL
};

static FontInfo dispFonts[MAX_FONTS];
static u8 curr_font_id = 0;
static FontInfo *dispHzFont;

// ����ɫ����ʾ��ɫ
 int disp_bkcolor = COLOR_WHITE;
 int disp_color = COLOR_BLACK;
/*+\new\liweiqiang\2014.10.21\���Ӳ�ͬ�ڰ������ɫ���� */
 int lcd_hwfillcolor = COLOR_BLACK;// lcd�������ɫ�趨,Ŀǰֻ֧�ֺڰ���,���ںڵװ��ֵ���Ҳ���ڰ׵׺��ֵ���
/*-\new\liweiqiang\2014.10.21\���Ӳ�ͬ�ڰ������ɫ���� */

// lcd �豸����
 u16 lcd_height;
 u16 lcd_width;
static u8 lcd_bpp;

// ��ʾ������
 u8 *framebuffer = NULL;

static void fontInit(void)
{
    static BOOL inited = FALSE;

    if(inited)
        return;

    inited = TRUE;

#if defined(LUA_DISP_SUPPORT_HZ)
#if defined(FONT_HZ_COMPRESS)
    LzmaDecodeBufToBuf(sansHzFont16DataZip, sizeof(sansHzFont16DataZip), &sansHzFont16Data);
#endif

    sansHzFont16.data = sansHzFont16Data;
#else
    sansHzFont16.data = NULL;
#endif
    
    memset(dispFonts, 0, sizeof(dispFonts));

    dispFonts[0] = sansFont16;
    dispHzFont = &sansHzFont16;
}


#ifdef AIR202_LUA_UI_SPC
void platform_spc_disp_init(PlatformDispInitParam *pParam)
{
    // ֻ֧��16λɫ��Ļ���ߺڰ���
    ASSERT(pParam->bpp == 16 || pParam->bpp == 1);

    lcd_bus = pParam->bus;

    lcd_width = pParam->width;
    lcd_height = pParam->height;

    lcd_bpp = pParam->bpp;

/*+\NEW\2013.4.10\���Ӻڰ�����ʾ֧�� */
    pParam->framebuffer = NULL;
/*-\NEW\2013.4.10\���Ӻڰ�����ʾ֧�� */

    // ��ʼ��lcd�豸
    platform_spc_lcd_init(pParam);
}
#else
void platform_disp_init(PlatformDispInitParam *pParam)
{
    // ֻ֧��16λɫ��Ļ���ߺڰ���
    ASSERT(pParam->bpp == 16 || pParam->bpp == 24 || pParam->bpp == 1);

    lcd_bus = pParam->bus;
    
    lcd_width = pParam->width;
    lcd_height = pParam->height;

    lcd_bpp = pParam->bpp;

    // ������ʾ������
    framebuffer = (u8*)((u32)malloc(lcd_width*lcd_height*lcd_bpp/8) | 0xa0000000);
    
/*+\NEW\2013.4.10\���Ӻڰ�����ʾ֧�� */
    pParam->framebuffer = framebuffer;
/*-\NEW\2013.4.10\���Ӻڰ�����ʾ֧�� */

/*+\bug0\zhy\2014.10.14\�ڰ���Ĭ��Ϊ �ڵװ���*/
    if(lcd_bpp == 1)
    {
        disp_bkcolor = COLOR_WHITE;
        disp_color = COLOR_BLACK;

/*+\new\liweiqiang\2014.10.21\���Ӳ�ͬ�ڰ������ɫ���� */
        if(pParam->hwfillcolor != -1){
            lcd_hwfillcolor = pParam->hwfillcolor;
        }
/*-\new\liweiqiang\2014.10.21\���Ӳ�ͬ�ڰ������ɫ���� */
    }
/*-\bug0\zhy\2014.10.14\�ڰ���Ĭ��Ϊ �ڵװ���*/

    fontInit();

    // ��ʼ��lcd�豸
    platform_lcd_init(pParam);
}
#endif

void platform_get_lcd_info(u16 *pWidth, u16 *pHeight, u8 *pBpp)
{
    *pWidth = lcd_width;
    *pHeight = lcd_height;
    *pBpp = lcd_bpp;   
}

void platform_disp_close(void)
{
    if(framebuffer != NULL)
    {
        free(framebuffer);
    }
}

void platform_disp_clear(void)
{
/*+\NEW\2013.4.10\���Ӻڰ�����ʾ֧�� */
    if(lcd_bpp == 1)
    {
/*+\bug0\zhy\2014.10.14\�ڰ������ñ���ɫ�޸�*/
/*+\new\liweiqiang\2014.10.21\���Ӳ�ͬ�ڰ������ɫ���� */
        if(disp_bkcolor^lcd_hwfillcolor) // �����ɫ��һ���Ͳ����
/*-\new\liweiqiang\2014.10.21\���Ӳ�ͬ�ڰ������ɫ���� */
            memset(framebuffer, 0x00, lcd_width*lcd_height*lcd_bpp/8);
        else // �����ɫһ�¾����
            memset(framebuffer, 0xFF, lcd_width*lcd_height*lcd_bpp/8);
/*-\bug0\zhy\2014.10.14\�ڰ������ñ���ɫ�޸�*/
    }
    else 
/*-\NEW\2013.4.10\���Ӻڰ�����ʾ֧�� */
    {
        u16 *pPixel16;
        u16 row,col;
        
        pPixel16 = (u16*)framebuffer;
        
        for(col = 0; col < lcd_width; col++)
        {
            for(row = 0; row < lcd_height; row++)
            {
                pPixel16[row*lcd_width + col] = disp_bkcolor;
            }
        }
    }
}

void platform_disp_update(void)
{
    PlatformRect rect;
    
    rect.ltx = 0;
    rect.lty = 0;
    rect.rbx = lcd_width-1;
    rect.rby = lcd_height-1;
    
    platform_lcd_update(&rect, framebuffer);
}

/*+\NEW\2013.4.10\���Ӻڰ�����ʾ֧�� */
static void disp_bitmap_bpp1(const DispBitmap *pBitmap, u16 startX, u16 startY)
{
    u16 bx,by,x,y,page,bwbytes;
    u16 endX, endY;

    if(pBitmap->bpp != 1)
    {
        printf("[disp_bitmap_bpp1]: not support bpp %d", pBitmap->bpp);
        return;
    }
    
    endX = MIN(lcd_width,startX + pBitmap->width);
    endY = MIN(lcd_height,startY + pBitmap->height);

    bwbytes = (pBitmap->width+7)/8;

    for(x = startX,bx = 0; x < endX; x++,bx++)
    {
        for(y = startY,by = 0; y < endY; y++,by++)
        {
            page = y/8;
/*+\bug0\zhy\2014.10.14\�ڰ�������ǰ��ɫ�޸�*/
            if((disp_color^lcd_hwfillcolor) == 0) /*\new\liweiqiang\2014.10.21\���Ӳ�ͬ�ڰ������ɫ���� */
            {
                        if(pBitmap->data[bwbytes*by+bx/8]&(0x80>>(bx%8)))
                        {
                            framebuffer[page*lcd_width+x] |= 1<<(y%8);
                        }
                        else
                        {
                            //framebuffer[page*lcd_width+x] &= ~(1<<(y%8));
                        }
            }
            else
            {
                        if(pBitmap->data[bwbytes*by+bx/8]&(0x80>>(bx%8)))
                        {
                             framebuffer[page*lcd_width+x] &= ~(1<<(y%8));
                        }
                        else
                        {                  
                            //framebuffer[page*lcd_width+x] |= 1<<(y%8);
                        }
            }
/*-\bug0\zhy\2014.10.14\�ڰ�������ǰ��ɫ�޸�*/
        }
    }
}

static void disp_1bitbmp_bpp16(const DispBitmap *pBitmap, u16 startX, u16 startY)
{
    u16 bx,by,x,y,bwbytes;
    u16 endX, endY;
    u16 *buffer16 = (u16*)framebuffer;
    u16 temp;
    
    ASSERT(pBitmap->bpp == 1);

    if(lcd_bus != PLATFORM_LCD_BUS_SPI)
        temp = disp_color;
    else
        temp = (disp_color >> 8 ) | ((disp_color & 0xff) << 8);

    endX = MIN(lcd_width,startX + pBitmap->width);
    endY = MIN(lcd_height,startY + pBitmap->height);
    
    bwbytes = (pBitmap->width+7)/8;
    
    for(x = startX,bx = 0; x < endX; x++,bx++)
    {
        for(y = startY,by = 0; y < endY; y++,by++)
        {            
            if(pBitmap->data[bwbytes*by+bx/8]&(0x80>>(bx%8)))
            {
                //�����ɫ
                buffer16[y*lcd_width + x] = temp;
            }
            else
            {
                //��䱳��ɫ
                // ֱ�ӵ�����ʾ,�ݲ�֧�ֱ���ɫ����
                //buffer16[y*lcd_width + x] = disp_bkcolor;
            }
        }
    }
}
/*-\NEW\2013.4.10\���Ӻڰ�����ʾ֧�� */

static void getFontBitmap(DispBitmap *pBitmap, u16 charcode)
{
    const FontInfo *pInfo = &dispFonts[curr_font_id];

    pBitmap->bpp = 1;
    pBitmap->width = pInfo->width;
    pBitmap->height = pInfo->height;

    if(pInfo->data)
    {
        if(charcode >= pInfo->start && charcode <= pInfo->end)
        {
            u32 index = charcode - pInfo->start;

            pBitmap->data = pInfo->data + index*pInfo->size;            
        }
        else
        {
            pBitmap->data = blankChar;
        }
    }
    else
    {
        pBitmap->data = blankChar;
    }
}

static void getHzBitmap(DispBitmap *pBitmap, u16 charcode)
{
    const FontInfo *pInfo = dispHzFont;

    pBitmap->bpp = 1;
    pBitmap->width = pInfo->width;
    pBitmap->height = pInfo->height;

    if(pInfo->data)
    {
        u8 byte1, byte2;
        u32 index;

        byte1 = charcode>>8;
        byte2 = charcode&0x00ff;

        if(byte1 >= 0xB0 && byte1 <= 0xF7 &&
            byte2 >= 0xA1 && byte2 <= 0xFE)
        {
            index = (byte1 - 0xB0)*(0xFE - 0xA1 + 1) + byte2 - 0xA1;
            pBitmap->data = pInfo->data + index*pInfo->size;
        }
        else
        {
            pBitmap->data = blankChar;

        /*+\NEW\liweiqiang\2013.12.18\�������ı����ŵ���ʾ֧�� */
            for(index = 0; index < sizeof(sansHzFont16ExtOffset)/sizeof(u16); index++)
            {
                if(charcode < sansHzFont16ExtOffset[index])
                {
                    break;
                }

                if(charcode == sansHzFont16ExtOffset[index])
                {
                    pBitmap->data = sansHzFont16ExtData + index*pInfo->size;
                    break;
                }
            }
        /*-\NEW\liweiqiang\2013.12.18\�������ı����ŵ���ʾ֧�� */
        }
    }
    else
    {
        pBitmap->data = blankChar;
    }
}

static void getCharBitmap(DispBitmap *pBitmap, u16 charcode)
{
    if(charcode >= 0xA0A0)
    {
        getHzBitmap(pBitmap, charcode);
    }
    else
    {
        getFontBitmap(pBitmap, charcode);
    }
}

#if 0 // ����������ӿ�
static void disp_clear_rect(int ltx, int lty, int rbx, int rby)
{
    u16 *buffer16 = (u16*)framebuffer;
    int x_pos, y_pos;

    ltx = ltx >= lcd_width ? ltx : lcd_width;
    lty = lty >= lcd_height ? lty : lcd_height;
    rbx = rbx >= lcd_width ? rbx : lcd_width;
    rby = rby >= lcd_height ? rby : lcd_height;

    for(x_pos = ltx; x_pos < rbx; x_pos++)
        for(y_pos = lty; y_pos < rby; y_pos++)
            buffer16[y_pos*lcd_width + x_pos] = disp_bkcolor;
}
#endif

void platform_disp_puttext(const char *string, u16 x, u16 y)
{
    int len = strlen(string);
    const u8 *pText = (const u8 *)string;
    int i;
    DispBitmap bitmap;
    u16 charcode;

    if(x >= lcd_width)
        x = 0;

    if(y >= lcd_height)
        y = 0;

    for(i = 0; i < len; )
    {
        if(pText[i] == '\r' || pText[i] == '\n'){
            i++;
            goto next_line;
        }
        
        if(pText[i]&0x80)
        {
            if(pText[i+1]&0x80)
            {
                // gb2312 chinese char
                charcode = pText[i]<<8 | pText[i+1];
                i += 2;
            }
            else
            {
                charcode = '?';
                i += 1;
            }
        }
        else
        {
            // ascii char
            charcode = pText[i];
            i += 1;
        }

        getCharBitmap(&bitmap, charcode);

        if(lcd_bpp == 1)
        {
            if(bitmap.width == 14 && bitmap.height == 14)
            {
                disp_bitmap_bpp1(&bitmap, (u16)(x+1), (u16)(y+1));
                bitmap.width = 16;
                bitmap.height = 16;
            }
            else
            {
                disp_bitmap_bpp1(&bitmap, x, y);
            }
        }
        else
            disp_1bitbmp_bpp16(&bitmap, x, y);

next_line:   
        // �Զ�������ʾ
        if(x + bitmap.width >= lcd_width)
        {
            y += bitmap.height;
            x = 0;
        }
        else
        {
            x += bitmap.width;
        }

        //�Զ�����
        if( y >= lcd_height)
        {
            y = 0;
        }
    }
}

/*+\NEW\liweiqiang\2013.11.4\����BMPͼƬ��ʾ֧�� */
/*	Support for BMP files		*/
#ifdef WIN32
#define __attribute__(x)
#endif

#ifdef WIN32 
#pragma pack(push,pack1,1)
#endif
typedef struct _bitmap_file_header
{
    u16 file_type;
    u32 file_size;
    u16 reserved1;
    u16 reserved2;
    u32 bitmap_offset;
} __attribute__((packed)) bitmap_file_header ;

typedef struct _bitmap_info_header
{
    u32 header_size;
    u32 width;
    u32 height;
    u16 number_of_planes;
    u16 bits_per_pixel;
    u32 compression;
    u32 bitmap_size;
    u32 device_width;
    u32 device_height;
    u32 number_of_colors;
    u32 number_of_important_colors;
} __attribute__((packed)) bitmap_info_header ;
#ifdef WIN32 
#pragma pack(pop,pack1)
#endif

#define RGB24ToRGB16(r,g,b) (((r&0x00f8)<<8)|((g&0x00fc)<<3)|((b&0xf8)>>3))

/*+\NEW\liweiqiang\2013.12.6\֧��bpp16��bmp��ʾ */
/*+\NewReq NEW\zhutianhua\2013.12.24\��ʾͼƬ��ָ������*/
static int put_bmp_file_buff(const u8 *bitmap_buffer, int x, int y, int transcolor, int left, int top, int right, int bottom)
/*-\NewReq NEW\zhutianhua\2013.12.24\��ʾͼƬ��ָ������*/
{
    bitmap_file_header *p_fileHeader = (bitmap_file_header *)bitmap_buffer;
    bitmap_info_header *p_infoHeader = (bitmap_info_header *)(bitmap_buffer+sizeof(bitmap_file_header));
    const u8 *data_buf;
    u16 data_r,data_g,data_b;
    u16 width, height;
    u16 rowIndex,colIndex;
    u16 bitmapRowBytes;
    u16 rgb16;

/*+\NEW\liweiqiang\2013.11.12\������ʾͼƬx,y�����޷����� */
    int bitmapRowIndex,bmpColIndex;       
    u32 data_index;

    u16 *buffer16 = (u16*)framebuffer;
    u16 bmp_bpp = p_infoHeader->bits_per_pixel;
    u16 temp;
    
    /*+\NewReq NEW\zhutianhua\2013.12.24\��ʾͼƬ��ָ������*/
    if((left > right) || (top > bottom))
    {
        printf("put_bmp_buffer: rect error\n");
        return PLATFORM_ERR;
    }

    if((left == 0) && (top == 0) && (right == 0) && (bottom == 0))
    {
        width = MIN(p_infoHeader->width + x, lcd_width);
        height = MIN(p_infoHeader->height + y, lcd_height);  
        bottom = p_infoHeader->height - 1;
    }
    else
    {
        width = MIN(right - left + 1 + x, lcd_width);
        height = MIN(bottom - top + 1 + y, lcd_height); 
    }
    /*-\NewReq NEW\zhutianhua\2013.12.24\��ʾͼƬ��ָ������*/

    data_buf = bitmap_buffer + p_fileHeader->bitmap_offset;
    
    bitmapRowBytes = ((p_infoHeader->width*bmp_bpp + 31)&~31)>>3; //4�ֽڶ���
    
    /*+\new\liweiqiang\2014.9.9\���Ӻڰ�image�ںڰ����ϵ���ʾ�ӿ� */
    if (lcd_bpp == 1 && bmp_bpp == 1)
    {
        int page;
        u8 lcdfill = lcd_hwfillcolor == COLOR_WHITE ? 1:0;
        u8 bmpfill = 1;
        u8 finalfill;

        if (p_infoHeader->number_of_planes == 1)
        {
            const u8 *fill_plate_p = bitmap_buffer \
                + sizeof(bitmap_file_header) /* �����ļ�ͷ */ \
                + sizeof(bitmap_info_header) /* ������Ϣͷ */ \
                + 4 /* ����0ֵ�ĵ�ɫ����Ϣ */;

            if(fill_plate_p[0] == 0xff && fill_plate_p[1] == 0xff && fill_plate_p[2] == 0xff){
                // 1ֵ��ɫ�������ȫff ��ʾ��ɫ ����ֵ���ж�Ϊ��ɫ
                bmpfill = 1;
            } else {
                bmpfill = 0;
            }
        }

        // lcd�����ɫ��λͼ���ɫ��һ�½��
        finalfill = ((lcdfill^bmpfill) == 0) ? 0x80 : 0x00;

        for(rowIndex = y, bitmapRowIndex = p_infoHeader->height - top - 1; 
            rowIndex < height && bitmapRowIndex >= p_infoHeader->height - bottom - 1;
            rowIndex++, bitmapRowIndex--)
        {
            page = rowIndex/8;

            for(colIndex = x, bmpColIndex = left; colIndex < width; colIndex++, bmpColIndex++)
            {
                /* ����õ�Ϊ��Ҫ��� ����λͼ���ɫ��lcd���ɫһ�� ����� */
                if(0 == ((data_buf[bitmapRowBytes*bitmapRowIndex+bmpColIndex/8]&(0x80>>(bmpColIndex%8)))^(finalfill>>(bmpColIndex%8))))
                    framebuffer[page*lcd_width+colIndex] |= 1<<(rowIndex%8);
                /* ������� */
                else
                    framebuffer[page*lcd_width+colIndex] &= ~(1<<(rowIndex%8));
            }
        }

        return PLATFORM_OK;    
    }
    
    if(lcd_bpp != 16)
    {
        printf("put_bmp_buffer: not color lcd\n");
        return PLATFORM_ERR;
    }
    
    if(bmp_bpp != 24 && bmp_bpp != 16)
    {
        printf("put_bmp_buffer: bmp not support bpp %d\n", bmp_bpp);
        return PLATFORM_ERR;
    }
    /*-\new\liweiqiang\2014.9.9\���Ӻڰ�image�ںڰ����ϵ���ʾ�ӿ� */
    
    /*+\NewReq NEW\zhutianhua\2013.12.24\��ʾͼƬ��ָ������*/
    for(rowIndex = y, bitmapRowIndex = p_infoHeader->height - top - 1; 
        rowIndex < height && bitmapRowIndex >= p_infoHeader->height - bottom - 1;
        rowIndex++, bitmapRowIndex--)
    {
        for(colIndex = x, bmpColIndex = left; colIndex < width; colIndex++, bmpColIndex++)
    /*-\NewReq NEW\zhutianhua\2013.12.24\��ʾͼƬ��ָ������*/
        {
            data_index = bitmapRowIndex*bitmapRowBytes + bmpColIndex*bmp_bpp/8;

            if(24 == bmp_bpp)
            {
                data_r = data_buf[data_index+2];
                data_g = data_buf[data_index+1];
                data_b = data_buf[data_index];
                /*+\NEW\liweiqiang\2013.11.12\��ɫ����͸����ʾ */
                rgb16 = RGB24ToRGB16(data_r,data_g,data_b);
            }
            else
            {
                rgb16 = *(u16*)&data_buf[data_index];
            }

/*+\NEW\liweiqiang\2013.12.6\����ͼƬ͸��ɫ���� */
            if(-1 == transcolor || rgb16 != transcolor)
/*-\NEW\liweiqiang\2013.12.6\����ͼƬ͸��ɫ���� */
            {
	            if(lcd_bus != PLATFORM_LCD_BUS_SPI)
	                temp = rgb16;
	            else
                	temp = (rgb16 >> 8 ) | ((rgb16 & 0xff) << 8);

                buffer16[rowIndex*lcd_width+colIndex] = temp;
            }
            /*-\NEW\liweiqiang\2013.11.12\��ɫ����͸����ʾ */
        }
    }
/*-\NEW\liweiqiang\2013.11.12\������ʾͼƬy�����޷����� */

    
    return PLATFORM_OK;
}
/*-\NEW\liweiqiang\2013.12.6\֧��bpp16��bmp��ʾ */

/*+\NEW\zhuth\2014.2.14\֧��pngͼƬ����ʾ*/
#ifdef AM_LPNG_SUPPORT
static int put_png_file_buff(const char *filename, int x, int y, int transcolor, int left, int top, int right, int bottom, int transtype)
{
    png_FILE_p fp;
    png_structp read_ptr;
    png_infop read_info_ptr;
    png_bytep row_buf = NULL;
    png_uint_32 width, height, row_idx, tmp_idx;
    png_byte color_type,channel;
    png_size_t row_bytes;

    u16 *buffer16 = (u16*)framebuffer;
    u16 data_r,data_g,data_b;
    u8 data_alpha,proc;
    u16 rgb16;
    u32 bgrgb888,fgrgb888,dstrgb888;
    short fr,fg,fb;
    short br,bg,bb;
    u8 dr,dg,db;
    u8 fpercent, bpercent;
    u32 rgb888;
    u8 *buffer24 = (u8*)framebuffer;
    u32 layer_width = 128;
    u16 temp;

    
    printf("[put_png_file_buff]: \n");
   
    if(strcmp(filename,"BAT.png") == 0)
    {
        printf("%s,%d,%d,%d,%d,%d,%d,%d", filename, x, y, transcolor, left, top, right, bottom);
    }

    if((left > right) || (top > bottom))
    {
        printf("[put_png_file_buff]: rect error\n");
        return PLATFORM_ERR;
    }

    fp = fopen(filename, "rb");
    if(NULL == fp)
    {
        printf("[put_png_file_buff]: %s file not exist\n", filename);
        return PLATFORM_ERR;
    }

    read_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    read_info_ptr = png_create_info_struct(read_ptr);    

    #ifdef PNG_SETJMP_SUPPORTED
    if (setjmp(png_jmpbuf(read_ptr)))
    {
       printf("[put_png_file_buff]: %s libpng read error\n", filename);
       png_free(read_ptr, row_buf);
       row_buf = NULL;
       png_destroy_read_struct(&read_ptr, &read_info_ptr, NULL);
       fclose(fp);
       return PLATFORM_ERR;
    } 
    #endif

    png_init_io(read_ptr, fp);
    png_set_bgr(read_ptr);
    
    png_read_info(read_ptr, read_info_ptr);
    width = png_get_image_width(read_ptr, read_info_ptr);
    height = png_get_image_height(read_ptr, read_info_ptr);
    color_type = png_get_color_type(read_ptr, read_info_ptr);
    channel = png_get_channels(read_ptr, read_info_ptr);
    row_bytes = png_get_rowbytes(read_ptr, read_info_ptr);
    /*if(strcmp(filename,"BAT.png") == 0)
    {
        printf("[put_png_file_buff]: width=%d,height=%d,color_type=%d,channel=%d,row_bytes=%d\n", width, height, color_type, channel,row_bytes);
    }*/
    if((3 != channel) && (4 != channel))
    {
        printf("[put_png_file_buff]: channel %d error\n", channel);
        png_free(read_ptr, row_buf);
        row_buf = NULL;
        png_destroy_read_struct(&read_ptr, &read_info_ptr, NULL);
        fclose(fp);
        return PLATFORM_ERR;
    }

    row_buf = (png_bytep)png_malloc(read_ptr, row_bytes);
    if(NULL == row_buf)
    {
        printf("[put_png_file_buff]: %d row no buf error\n", row_bytes);
        png_destroy_read_struct(&read_ptr, &read_info_ptr, NULL);
        fclose(fp);
        return PLATFORM_ERR;
    }

    if((left == 0) && (top == 0) && (right == 0) && (bottom == 0))
    {
        left = 0;
        top = 0;
        right = width - 1;
        bottom = height - 1;
        
        if(lcd_bpp == 24 && channel == 3)
        {
            int write_index = ((y)*layer_width + x) * 3;
            
            printf("[put_png_file_buff]: FAST READ\n");

            for(row_idx=0; row_idx< height; row_idx++)
            {
                png_read_row(read_ptr, row_buf, NULL);
                memcpy(&buffer24[write_index],
                        row_buf,
                        row_bytes);
                
                write_index += layer_width * 3;
            }

            #ifdef PNG_STORE_UNKNOWN_CHUNKS_SUPPORTED
              #ifdef PNG_READ_UNKNOWN_CHUNKS_SUPPORTED
                    png_free_data(read_ptr, read_info_ptr, PNG_FREE_UNKN, -1);
              #endif
            #endif

            png_free(read_ptr, row_buf);
            row_buf = NULL;
            png_destroy_read_struct(&read_ptr, &read_info_ptr, NULL);
            fclose(fp);

            return PLATFORM_OK;

        }
    }

    for(row_idx=0; row_idx<height; row_idx++)
    {
    	 if(row_idx > bottom)
    	 {
    	     break;
    	 }
    	 
        png_read_row(read_ptr, row_buf, NULL);
        if(row_idx < top)
    	 {
    	     continue;
    	 }
    	 
        for(tmp_idx=left; tmp_idx<=right; tmp_idx++)
        {
            proc = 0;
            
            if(channel==4)
            {
               data_alpha = row_buf[tmp_idx*channel + 3];
            }

            if(lcd_bpp == 16)
            {
                fr = row_buf[tmp_idx*channel + 2];
                fg = row_buf[tmp_idx*channel + 1];
                fb = row_buf[tmp_idx*channel + 0];

                rgb16 = RGB24ToRGB16(fr,fg,fb);
                
                if(-1 == transcolor || rgb16 != transcolor || transtype == 2)
                {
                    if(channel==4)
                    {
                        //0:��������ȫ͸����ȫ͸�ĵ�����͸������
                        //1:������������͸����
                        //2:��������ȫ͸����ȫ͸�ĵ�����transcolor��ͬ��͸���ȴ���
                        if((transtype==0 && data_alpha==0)
                            || (transtype==1)
                            || (transtype==2)
                            )
                        {
                            proc = 1;

				            if(lcd_bus == PLATFORM_LCD_BUS_SPI)
                            {
			                	temp = (buffer16[(y+row_idx-top)*layer_width+(x+tmp_idx-left)] >> 8 ) | 
                                    ((buffer16[(y+row_idx-top)*layer_width+(x+tmp_idx-left)] & 0xff) << 8);
                            }   
                            else
                            {
                                temp = buffer16[(y+row_idx-top)*layer_width+(x+tmp_idx-left)];
                            }
                            
                            br = (temp >> 8) & 0xf8;
                            bg = (temp >> 3) & 0xfc;
                            bb = (temp << 3) & 0xf8;
                            
                            
                           if(transtype==2 && data_alpha != 0)
                            {
                                fpercent = (float)transcolor/255;
                            }
                            else
                            {
                                fpercent = (float)data_alpha/255;
                            }
                            
                            bpercent = 1 - fpercent;
                            
                            dr = fr * fpercent + br * bpercent; 
                            dg = fg * fpercent + bg * bpercent; 
                            db = fb * fpercent + bb * bpercent; 


				            if(lcd_bus != PLATFORM_LCD_BUS_SPI)
				                temp = RGB24ToRGB16(dr, dg, db);
				            else
			                	temp = (RGB24ToRGB16(dr, dg, db) >> 8 ) | ((RGB24ToRGB16(dr, dg, db) & 0xff) << 8);

                            buffer16[(y+row_idx-top)*layer_width+(x+tmp_idx-left)] = temp;
                            
                        }                    
                    }
                    
                    if(proc==0)
                    {
					
			            if(lcd_bus != PLATFORM_LCD_BUS_SPI)
			                temp = rgb16;
			            else
		                	temp = (rgb16 >> 8 ) | ((rgb16 & 0xff) << 8);

                        buffer16[(y+row_idx-top)*layer_width+(x+tmp_idx-left)] = temp;
                    }
                }
            }
            else  if(lcd_bpp == 24)
            {
            
                fr = row_buf[tmp_idx*channel + 2];
                fg = row_buf[tmp_idx*channel + 1];
                fb = row_buf[tmp_idx*channel + 0];

                if(channel==4)
                {
                    //0:��������ȫ͸����ȫ͸�ĵ�����͸������
                    //1:������������͸����
                    //2:��������ȫ͸����ȫ͸�ĵ�����transcolor��ͬ��͸���ȴ���
                    if(data_alpha == 0xff)
                    {
                        buffer24[((y+row_idx-top)*layer_width+(x+tmp_idx-left))*3 + 2] = fr;// dstrgb888 >> 16;
                        buffer24[((y+row_idx-top)*layer_width+(x+tmp_idx-left))*3 + 1] = fg;//(dstrgb888    & 0xff00)>> 8;
                        buffer24[((y+row_idx-top)*layer_width+(x+tmp_idx-left))*3 + 0] = fb;// dstrgb888 &0xff;
						continue;
                    }
                    else if(data_alpha == 0)
                    {
                        continue;
                    }
                    else
                    {
					    if(transtype== 2 && data_alpha != 0)
						{
						    fpercent = transcolor;
						}
						else
						{
						    fpercent = data_alpha;
						}
                        br = buffer24[((y+row_idx-top)*layer_width + (x+tmp_idx-left)) * 3 + 2] ;
                        bg = buffer24[((y+row_idx-top)*layer_width + (x+tmp_idx-left)) * 3 + 1] ;
                        bb = buffer24[((y+row_idx-top)*layer_width + (x+tmp_idx-left)) * 3 + 0] ;
                                                    
                        dr = ((((fr - br)* fpercent) >> 8) ) + br; 
                        dg = ((((fg - bg)* fpercent) >> 8) ) + bg; 
                        db = ((((fb - bb)* fpercent) >> 8) ) + bb; 
                        
                        buffer24[((y+row_idx-top)*layer_width+(x+tmp_idx-left))*3 + 2] = dr;// dstrgb888 >> 16;
                        buffer24[((y+row_idx-top)*layer_width+(x+tmp_idx-left))*3 + 1] = dg;//(dstrgb888  & 0xff00)>> 8;
                        buffer24[((y+row_idx-top)*layer_width+(x+tmp_idx-left))*3 + 0] = db;// dstrgb888 &0xff;
                    }     
                }
				else
				{
                    buffer24[((y+row_idx-top)*layer_width+(x+tmp_idx-left))*3 + 2] = fr;// dstrgb888 >> 16;
                    buffer24[((y+row_idx-top)*layer_width+(x+tmp_idx-left))*3 + 1] = fg;//(dstrgb888    & 0xff00)>> 8;
                    buffer24[((y+row_idx-top)*layer_width+(x+tmp_idx-left))*3 + 0] = fb;// dstrgb888 &0xff;
				}

                
            }
        }
    }

    #ifdef PNG_STORE_UNKNOWN_CHUNKS_SUPPORTED
      #ifdef PNG_READ_UNKNOWN_CHUNKS_SUPPORTED
            png_free_data(read_ptr, read_info_ptr, PNG_FREE_UNKN, -1);
      #endif
    #endif

    png_free(read_ptr, row_buf);
    row_buf = NULL;
    png_destroy_read_struct(&read_ptr, &read_info_ptr, NULL);
    fclose(fp);
    
    return PLATFORM_OK;
}
#endif
/*-\NEW\zhuth\2014.2.14\֧��pngͼƬ����ʾ*/


static int put_jpg_file_buff(const char *filename, int x, int y, int transcolor, int left, int top, int right, int bottom)
{
    image_t image_info;
    char* actname;
    int res, devid;
    u16* buffer = (u16*)framebuffer;

  // Look for device, return error if not found or if function not implemented

    
    if((left > right) || (top > bottom))
    {
        printf("[put_jpg_file_buff]: rect error\n");
        return PLATFORM_ERR;
    }


    if(!platform_image_decode(filename, &image_info))
    {
        int startPos; 
        int i;
        int j;
        
        if((left == 0) && (top == 0) && (right == 0) && (bottom == 0))
        {
            left = 0;
            top = 0;
            right = image_info.width - 1;
            bottom = image_info.height - 1;
        }

        startPos = top *image_info.width + left;

        for(i = top;  i <= bottom; i ++)
        {

#if 0
            for(j = 0 ; j <= right - left ; j ++) 
            {
                framebuffer[(y*lcd_width + x + j) << 1] = image_info.buffer[(startPos + j) ] >> 0x8;
                framebuffer[((y*lcd_width + x + j) << 1) + 1] = image_info.buffer[(startPos + j)] & 0xff;
            }

#else
            memcpy(&buffer[y*lcd_width + x],
                &image_info.buffer[startPos], 
                (right - left + 1) * 2);
#endif
            
            startPos += image_info.width;
            y++;
        }

        platform_free_image_decodedata(image_info.buffer);
    }
    
    return PLATFORM_OK;
}



int show_jpg_file(const char *filename, int* width, int* height)
{
    image_t image_info;
    char* actname;
    int res, devid;
    u8* buffer = NULL;
    int startY = 0;
    int left, top, right, bottom;
    
  // Look for device, return error if not found or if function not implemented
    if((left > right) || (top > bottom))
    {
        printf("[put_jpg_file_buff]: rect error\n");
        return PLATFORM_ERR;
    }


    if(!platform_image_decode(filename, &image_info))
    {
        int startPos; 
        int i;
        int j;
        u8 temp;
        
        left = 0;
        top = 0;
        right = image_info.width - 1;
        bottom = image_info.height - 1;
        
        startPos = 0;
        
        buffer = (u8*)image_info.buffer;

        if(lcd_bus == PLATFORM_LCD_BUS_SPI)
        {
            for(i = top;  i <= bottom; i ++)
            {
                for(j = 0 ; j <= right - left ; j ++) 
                {
                    temp = image_info.buffer[(startPos + j)];
                    
                    buffer[(startPos + j) << 1] = image_info.buffer[(startPos + j) ] >> 0x8;
                    buffer[((startPos + j) << 1) + 1] = temp;
                }

                #if 0
                memcpy(&buffer[y*lcd_width + x],
                    &image_info.buffer[startPos], 
                    (right - left + 1) * 2);
                #endif
                
                startPos += image_info.width;
            }
        }

        *width = image_info.width;
        *height = image_info.height;

        #ifdef AIR202_LUA_UI_SPC
            lcd_bus_spc_spi_write_data_buf(buffer, image_info.width * image_info.height * (lcd_bpp / 8));
        #endif
        platform_free_image_decodedata(image_info.buffer);
    }
    
    return PLATFORM_OK;
}




/*+\bug NEW\zhutianhua\2013.12.24\��ʾͼƬ��ָ������*/
int platform_disp_putimage(const char *filename, u16 x, u16 y, int transcolor, u16 left, u16 top, u16 right, u16 bottom, int transtype)
/*-\bug NEW\zhutianhua\2013.12.24\��ʾͼƬ��ָ������*/
{
    u8 *buff;
    FILE *fp;
    u32 len;
    int result;

    /*+\NEW\zhuth\2014.2.16\֧��pngͼƬ����ʾ*/
    if(strstr(filename, ".bmp") || strstr(filename, ".BMP"))
    {
        fp = fopen(filename, "rb");

        if(NULL == fp)
        {
            printf("[putimage]: %s file not exist\n", filename);
            return PLATFORM_ERR;
        }
    
        fseek(fp, 0, SEEK_END);
        len = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        buff = malloc(len);
        if(NULL == buff)
        {
            printf("[putimage]:not enought memory\n");
            fclose(fp);
            return PLATFORM_ERR;
        }
        
        fread(buff, 1, len, fp);
        fclose(fp);
    
        /*+\NewReq NEW\zhutianhua\2013.12.24\��ʾͼƬ��ָ������*/
        result = put_bmp_file_buff(buff, x, y, transcolor, left, top, right, bottom);
        /*-\NewReq NEW\zhutianhua\2013.12.24\��ʾͼƬ��ָ������*/
        free(buff);
    }
    #ifdef AM_LPNG_SUPPORT
    else if(strstr(filename, ".png") || strstr(filename, ".PNG"))
    {
        result = put_png_file_buff(filename, x, y, transcolor, left, top, right, bottom, transtype);
    }
    #endif
    /*-\NEW\zhuth\2014.2.16\֧��pngͼƬ����ʾ*/

    else if(strstr(filename, ".jpg") || strstr(filename, ".JPG"))
    {
        result = put_jpg_file_buff(filename, x, y, transcolor, left, top, right, bottom);
    }   
    else
    {
        printf("[putimage]:only support bmp and png!\n");
        return PLATFORM_ERR;
    }
    
    return result;
}
/*-\NEW\liweiqiang\2013.11.4\����BMPͼƬ��ʾ֧�� */

/*+\NEW\liweiqiang\2013.12.7\���Ӿ�����ʾ֧�� */
int platform_disp_drawrect(int x1, int y1, int x2, int y2, int color)
{
    int i,j;
    x1 = MIN(x1, lcd_width-1);
    x2 = MIN(x2, lcd_width-1);
    y1 = MIN(y1, lcd_height-1);
    y2 = MIN(y2, lcd_height-1);

    if(x1 >= x2 || y1 >= y2)
    {
        printf("[platform_disp_drawrect]: range error %d %d %d %d!\n", x1, y1, x2, y2);
        return PLATFORM_ERR;
    }

    if(lcd_bpp == 16)
    {
        u16 *buf16 = (u16*)framebuffer;
        u16 temp;
        
        if(-1 == color)
        {
            //�����ο�
            int height = y2 - y1;
            int pixels = height*lcd_width;
            
            buf16 += y1*lcd_width;

            if(lcd_bus != PLATFORM_LCD_BUS_SPI)
                temp = disp_color;
            else
                temp = (disp_color >> 8 ) | ((disp_color & 0xff) << 8);

                
            for(i = x1; i <= x2; i++)
            {
                buf16[i] = temp;
                buf16[pixels+i] = temp;
            }

            for(j = y1; j <= y2; j++)
            {
                buf16[x1] = temp;
                buf16[x2] = temp;
                buf16 += lcd_width;
            }
        }
        else
        {
            //�����ɫ������
            buf16 += y1*lcd_width;

            for(j = y1; j <= y2; j++)
            {
                for(i = x1; i <= x2; i++)
                {
                    buf16[i] = color;
                }
                buf16 += lcd_width;
            }
        }
    }
/*+\bug0\zhy\2014.10.15\�ڰ������*/
    else if(lcd_bpp == 1)
    {
        if(color == COLOR_BLACK || color == COLOR_WHITE)
        {
            u16 x,y,page;
            
            if((color^lcd_hwfillcolor) == 0) /*\new\liweiqiang\2014.10.21\���Ӳ�ͬ�ڰ������ɫ���� */
            {

                for(x = x1; x < x2; x++)
                {
                    for(y = y1;y < y2; y++)
                    {
                        page = y/8;
                        framebuffer[page*lcd_width+x] |= 1<<(y%8);
                    }
                }
            }
            else
            {
                for(x = x1; x < x2; x++)
                {
                    for(y = y1;y < y2; y++)
                    {
                        page = y/8;
                        framebuffer[page*lcd_width+x] &= ~(1<<(y%8));
                    }
                }
            }
        }
        else
        {
            printf("[platform_disp_drawrect]: lcd_bpp = 1,color must be balck or white\n");
        }
    }
/*-\bug0\zhy\2014.10.15\�ڰ������*/
    else
    {
        // �ݲ�֧��������
        printf("[platform_disp_drawrect]: not support bpp %d\n", lcd_bpp);
        return PLATFORM_ERR;
    }

    return PLATFORM_OK;
}
/*-\NEW\liweiqiang\2013.12.7\���Ӿ�����ʾ֧�� */

/*+\NEW\liweiqiang\2013.12.9\����ǰ��ɫ\����ɫ���� */
int platform_disp_setcolor(int color)
{
    int old_color = disp_color;
    disp_color = color;
    return old_color;
}

int platform_disp_setbkcolor(int color)
{
    int old_color = disp_bkcolor;
    disp_bkcolor = color;
    return old_color;
}
/*-\NEW\liweiqiang\2013.12.9\����ǰ��ɫ\����ɫ���� */

/*+\NEW\liweiqiang\2013.12.9\���ӷ������������� */
#ifdef WIN32 
#pragma pack(push,pack1,1)
#endif
typedef struct FontFileInfoTag
{
    u8        width;
    u8        height;
    u8        type;
    u16       start;
    u16       end;
}__attribute__((packed)) FontFileInfo;
#ifdef WIN32 
#pragma pack(pop,pack1)
#endif

static int load_file_data(const char *name, u8 **buf_pp)
{
    FILE *fp = NULL;
    int size;
    char *buf = NULL;
    
    fp = fopen(name, "rb");
    
    if(fp == NULL)
    {
        printf("[load_file_data]: file not exist!\n");
        goto load_error;
    }
    
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    buf = malloc(size);
    if(NULL ==  buf)
    {
        printf("[load_file_data]: no memory\n");
        goto load_error;
    }
    fseek(fp, 0, SEEK_SET);
    fread(buf, 1, size, fp);
    fclose(fp);
    
    *buf_pp = buf;
    return size;
    
load_error:
    if(fp)
        fclose(fp);
    
    if(buf)
        free(buf);
    return 0;
}

int platform_disp_loadfont(const char *name)
{
    u8 *buf = NULL;
    int size = 0;
    FontFileInfo *fileInfo_p;
    u32 charsize;
    BOOL found = FALSE;
    int i;
    int retcode = 0;

    size = load_file_data(name, &buf);
    
    if(size <= sizeof(FontFileInfo))
    {
        retcode = -1;
        goto load_font_error;
    }

    fileInfo_p = (FontFileInfo *)buf;

    if(fileInfo_p->type != 0)
    {
        // ֻ֧�������ַ���ʽ���ֿ�
        retcode = -2;
        goto load_font_error;
    }

    if(fileInfo_p->end < fileInfo_p->start)
    {
        retcode = -3;
        goto load_font_error;
    }
    
    charsize = (fileInfo_p->width+7)/8;
    charsize *= fileInfo_p->height;

    if(charsize*(fileInfo_p->end - fileInfo_p->start + 1) != size - sizeof(FontFileInfo))
    {
        retcode = -4;
        goto load_font_error;
    }

    for(i = 0; i < MAX_FONTS && !found; i++)
    {
        if(dispFonts[i].data == NULL)
        {
            found = TRUE;
            break;
        }
    }
    
    if(!found)
    {
        retcode = -5;
        goto load_font_error;
    }

    dispFonts[i].width = fileInfo_p->width;
    dispFonts[i].height = fileInfo_p->height;
    dispFonts[i].size = charsize;
    dispFonts[i].start = fileInfo_p->start;
    dispFonts[i].end = fileInfo_p->end;
    dispFonts[i].data = buf + sizeof(FontFileInfo);

    return i;

load_font_error:
    if(buf)
        free(buf);

    printf("[platform_disp_loadfont]:error code %d\n", retcode);

    return -1;
}

int platform_disp_setfont(int id)
{
    int old_font_id;

    /*+\NEW\liweiqiang\2013.12.10\����������id���»��� */
    if(id < 0 || id >= MAX_FONTS || NULL == dispFonts[id].data)
    /*-\NEW\liweiqiang\2013.12.10\����������id���»��� */
    {
        printf("[platform_disp_setfont]: error font id\n");
        return -1;
    }

    old_font_id = curr_font_id;
    curr_font_id = id;
    return old_font_id;
}
/*-\NEW\liweiqiang\2013.12.9\���ӷ������������� */

#endif

