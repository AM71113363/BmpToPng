#include <windows.h>
#include <stdio.h>
#include "pnglib\png.h"

#define YES 1
#define NO 0

UCHAR *buffer=NULL;
UINT bufferLen=0;

BITMAPFILEHEADER *bFile;
BITMAPINFOHEADER *bInfo;
UCHAR    **rowBytesBuffer;
png_color_16 rgbData;

void SetColor(UCHAR red,UCHAR green,UCHAR blue)
{
    rgbData.red=red;
    rgbData.green=green;
    rgbData.blue=blue;
}

void SavePNG(UCHAR *name)
{
    png_structp png_ptr;
    png_infop info_ptr;
    FILE *fp;

    fp = fopen(name, "wb");
    if(fp==NULL)
      return;
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, name, NULL, NULL);
    if(png_ptr == NULL)
    {
       fclose(fp);
       return;
    }        
    info_ptr = png_create_info_struct(png_ptr);
    if(info_ptr == NULL)
    {
       fclose(fp);
       return;
    }

    png_init_io(png_ptr, fp);
    png_set_compression_level(png_ptr, 6);
    png_set_compression_mem_level(png_ptr, 9);
    png_set_IHDR(png_ptr, info_ptr, bInfo->biWidth, bInfo->biHeight, 8,2, 0, 0,0);
    png_set_tRNS(png_ptr, info_ptr, NULL, 0, &rgbData);
    png_write_info(png_ptr, info_ptr);
    png_set_bgr(png_ptr);
    png_write_image(png_ptr, rowBytesBuffer);
    png_write_end(png_ptr, info_ptr);
    png_destroy_write_struct(&png_ptr, &info_ptr);

    fflush(fp);
  fclose(fp);
return;
}

UCHAR GetInfo()
{
    UCHAR *bp, **rp; UCHAR Inverted;
    LONG n; DWORD imageBytes; DWORD imageRowBytes;
  
    bFile=(BITMAPFILEHEADER*)buffer;  
    bInfo = (BITMAPINFOHEADER*)(buffer + sizeof(BITMAPFILEHEADER));

    if(rowBytesBuffer){ free(rowBytesBuffer); rowBytesBuffer= NULL; }
    
    if(bFile->bfType != 0x4D42)
      return NO;	

    if(bInfo->biSize !=40)
      return NO;
	
    if(bInfo->biBitCount!=24)
      return NO;
	
    if(bInfo->biCompression != BI_RGB)
      return NO;
         
    if(bInfo->biHeight < 0)
    {
        bInfo->biHeight  = -bInfo->biHeight;
	Inverted = YES;
    }else{ Inverted  = NO; }
    if((bInfo->biWidth  <= 0) || (bInfo->biHeight <= 0)) 
      return NO;

    imageRowBytes = ((DWORD)bInfo->biWidth * 24 + 31) / 32 * 4;
    imageBytes = imageRowBytes * bInfo->biHeight;
    rowBytesBuffer = (UCHAR**)malloc((UINT)bInfo->biHeight * sizeof(UCHAR*));

    if(rowBytesBuffer == NULL)
      return NO;
    n  = bInfo->biHeight;
    rp = rowBytesBuffer;
    bp = &buffer[bFile->bfOffBits];

    if(Inverted==YES)
    {
        while(--n>=0)
        {
	    *rp++ = bp;
	    bp+=imageRowBytes;
	}
    }
    else
    {
	bp+=imageBytes;
	while(--n>=0)
        {
	    bp -= imageRowBytes;
	    *rp++ = bp;
	}
    }
  return YES;
}

UCHAR ReadDataFile(UCHAR *name)
{
    HANDLE fd;
    DWORD result=0;
    if(buffer!=NULL){ free(buffer); buffer=NULL;}
    fd = CreateFile(name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
    if(fd == INVALID_HANDLE_VALUE)
      return NO;
    bufferLen = GetFileSize(fd,NULL);
    if(bufferLen == 0xFFFFFFFF)
    {
        CloseHandle(fd);
        return NO;
    }
    buffer = (UCHAR*) malloc(bufferLen+1);
    if(buffer==NULL)
    { 
        CloseHandle(fd);
        return NO;
    }
    memset(buffer,0,bufferLen+1);
    if(ReadFile(fd, buffer, bufferLen, &result, NULL) == FALSE)
    {
        CloseHandle(fd);
        free(buffer); buffer=NULL;
        return NO;
    }
    if(result != bufferLen)
    {
        CloseHandle(fd);
        free(buffer); buffer=NULL;
        return NO;
    }
    CloseHandle(fd);   
  return GetInfo();
}

