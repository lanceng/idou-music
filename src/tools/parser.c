#include <stdio.h>  
#include <stdlib.h>  
#include <memory.h>  
#include <math.h>  
#define XMD_H  
#include "jpeglib.h"  
#include <setjmp.h>  
#include <afxwin.h>  
#include <Windows.h>  
#include <initguid.h>  
#include <ShlObj.h>  
  
#pragma comment(lib, "jpeg.lib")  
  
//id3v2 标签头结构体  
typedef struct  
{  
    char  identi[3];//必须为ID3，否则认为标签不存在  
    byte  major;    //版本号  
    byte  revsion;  //副版本号  
    byte  flags;    //标志位  
    byte  size[4]; //标签大小，不包括标签头的10个字节  
}ID3V2Header;  
  
//标签帧头结构体  
typedef struct  
{  
    char FrameId[4];//标签帧标识符，用来表明这个标签帧的内容  
    byte size[4];    //此标签帧的大小，不包括此标签头的10个字节  
    byte flags[2];  //标志位  
}ID3V2Frameheader;  
  
char *MusicName="Skylar Grey - I Love the way u lie.mp3";  
//char *MusicName="home.mp3";  
long width = 0;  
long height = 0;  
  
FILE *OpenMusic(int &Len)  
{  
    FILE *fp=NULL;  
    fp=fopen(MusicName,"rb");  
    if(!fp)  
    {  
        printf("无法打开文件\n");  
        fp=NULL;  
        return fp;  
    }  
    //把打开的音乐文件流的内部指针重置到文件开头  
    fseek(fp,0,SEEK_SET);  
      
    //读取标签头  
    ID3V2Header mp3ID3V2;  
    ZeroMemory(&mp3ID3V2,10);  
    fread(&mp3ID3V2,10,1,fp);  
    //判断有没有id3v2的标签头  
    if(0!=strncmp(mp3ID3V2.identi,"ID3",3))  
    {  
        printf("没有ID2V2标签\n");  
        fclose(fp);  
        fp=NULL;  
        return fp;  
    }  
  
    //计算处整个标签的大小  
    Len = (mp3ID3V2.size[0]&0x7f)*0x200000  
        +(mp3ID3V2.size[1]&0x7f)*0x4000  
        +(mp3ID3V2.size[2]&0x7f)*0x80  
        +(mp3ID3V2.size[3]&0x7f);  
  
    return fp;  
}  
  
BOOL GetPicInfo(FILE *fp, int &dwFrame, int Len, int &tempi)  
{  
    ID3V2Frameheader pFrameBuf;  
    ZeroMemory(&pFrameBuf,10);  
    fread(&pFrameBuf,10,1,fp);  
    int i=0;  
    //找到图片标签的所在位置  
    while((strncmp(pFrameBuf.FrameId,"APIC",4) != 0))  
    {  
        //判断是否有图片  
        if(i>Len)  
        {  
            printf("没有找到标识图像帧的标签\n");  
            return FALSE;  
        }  
        dwFrame= pFrameBuf.size[0]*0x1000000  
            +pFrameBuf.size[1]*0x10000  
            +pFrameBuf.size[2]*0x100  
            +pFrameBuf.size[3];  
        fseek(fp,dwFrame,SEEK_CUR);  
        ZeroMemory(&pFrameBuf,10);  
        fread(&pFrameBuf,10,1,fp);  
        i++;  
    }  
  
    //计算出图片标签的大小  
    dwFrame= pFrameBuf.size[0]*0x1000000  
        +pFrameBuf.size[1]*0x10000  
        +pFrameBuf.size[2]*0x100  
        +pFrameBuf.size[3];  
  
    char image_tag[7]={"0"};  
    char pic_type[5]={"0"};  
    fread(image_tag,6,1,fp);  
    //判断图片格式  
    i=0;  
    while(true)  
    {  
        if(i>dwFrame)  
        {  
            printf("没有找到标识图像类型的标签\n");  
            fclose(fp);  
            return FALSE;  
        }  
        if(0==(strcmp(image_tag,"image/")))  
        {  
            tempi+=6;  
            fread(pic_type,4,1,fp);  
            //mp3里面大多图片都是jpeg，也是以jpeg作为标志的  
            //也有以jpe，jpg，peg作为标志的  
            //不过也有png等格式的。  
            if(0==strncmp(pic_type,"jpeg",4))  
            {  
                tempi+=4;  
                break;  
            }  
            else if(0==strncmp(pic_type,"jpg",3))  
            {  
                tempi+=3;  
                fseek(fp,-1,SEEK_CUR);  
                break;  
            }  
            else if(0==strncmp(pic_type,"peg",3))  
            {  
                tempi+=3;  
                fseek(fp,-1,SEEK_CUR);  
                break;  
            }  
            else  
            {  
                printf("图片格式不是jpeg\n");  
                fclose(fp);  
                return FALSE;  
            }  
        }  
        else  
        {  
            i++;  
            fseek(fp,-5,SEEK_CUR);  
            fread(image_tag,6,1,fp);  
            tempi=tempi+1;  
            continue;  
        }  
    }  
    return TRUE;  
}  
  
void GetPicRGB(FILE *fp, int dwFrame, int tempi)  
{  
    TCHAR lpTempPathBuffer[MAX_PATH];  
    TCHAR szTempFileName[MAX_PATH];  
    DWORD dwRetVal=0;  
    UINT uRetVal=0;  
    BYTE  *pPicData;  
    unsigned char * bmpDataBuffer=NULL;  
  
    //这两个tag的是表明图片数据的开始  
    //jpeg图片开始的标志是0xFFD8  
    BYTE jpeg_header_tag1;  
    BYTE jpeg_header_tag2;  
    fseek(fp,0,SEEK_CUR);  
    fread(&jpeg_header_tag1,1,1,fp);  
    fseek(fp,0,SEEK_CUR);  
    fread(&jpeg_header_tag2,1,1,fp);  
  
    //计算出图片数据开始的地方  
    int i=0;  
    while(true)  
    {  
        if(i>dwFrame)  
        {  
            printf("没有找到图像数据\n");  
            fclose(fp);  
            bmpDataBuffer=NULL;  
        }  
        i++;  
        if((255==jpeg_header_tag1) && (216==jpeg_header_tag2))  
        {  
            pPicData = new BYTE[dwFrame-tempi];  
            ZeroMemory(pPicData,dwFrame-tempi);  
            //设定文件流的指针位置，并把图片的数据读入pPicData  
            fseek(fp,-2,SEEK_CUR);  
            fread(pPicData,dwFrame-tempi,1,fp);  
            fclose(fp);  
  
            fp=fopen("temp.jpeg","w+b");  
            fwrite(pPicData,dwFrame-tempi,1,fp);  
            delete []pPicData;  
            DeleteFile(szTempFileName);  
            break;  
        }  
        else  
        {  
            fseek(fp,-1,SEEK_CUR);  
            fread(&jpeg_header_tag1,1,1,fp);  
            fseek(fp,0,SEEK_CUR);  
            fread(&jpeg_header_tag2,1,1,fp);  
            tempi++;  
            continue;  
        }  
    }  
    struct jpeg_decompress_struct cinfo;  
    struct jpeg_error_mgr jerr;  
    cinfo.err = jpeg_std_error(&jerr);  
    jpeg_create_decompress(&cinfo);  
    fseek(fp,0,SEEK_SET);  
    jpeg_stdio_src(&cinfo,fp);  
      
    jpeg_read_header(&cinfo,1);  
    width = cinfo.image_width;  
    height = cinfo.image_height;  
  
    JSAMPARRAY buffer;  
    jpeg_start_decompress(&cinfo);  
    bmpDataBuffer=new unsigned char[width*height*3];  
  
    buffer=(*cinfo.mem->alloc_sarray)  
    ((j_common_ptr) &cinfo, JPOOL_IMAGE, (width*3), 1);  
  
    //获得图像rgb源数据，存入数组bmpDataBuffer  
    while(cinfo.output_scanline < cinfo.output_height)  
    {  
        int j=0;  
        jpeg_read_scanlines(&cinfo, buffer, 1);  
        if(cinfo.num_components==1)  
        {  
            //这个地方是把256转成24.我们得到的256的bmp图片，是在头信息的地方加了一个调色板  
            //调色板的大小是1024，是四个字节表示一种颜色，一共256种颜色  
            //256bmp下面不是rgb数据，是针对于调色板的一个索引，一个字节，8位，共可以表示2的8次方，也就是256种颜色  
            //如果num_componets=1也就表示是一种黑白的图片，黑白的图片的调色板的每种颜色的rgb是相等的，就是如果rgb相等  
            //显示出来的颜色是黑白的。这时我们可以用两种bmp来显示这个图片  
            //一个是256，调色板加索引数据  
            //另一种是24位图，24的bmp是没有调色板，后面3位表示一个颜色的rgb，所以是2^8*2^8*2^8，共可以表示2^24种颜色  
            //基于上面256灰度图的索引值就是rgb值，因为3个相等，所以可以把上面的256灰度的bmp去掉调色板，把每个索引用  
            //3位表示，值和索引一样，3个值也一样，这样就转成了一个24位的灰度图  
            for(j=0;j<width;j++)  
            {  
                bmpDataBuffer[(height - cinfo.output_scanline)*(width*3)+3*j] = buffer[0][j]; //R  
                j++;  
                bmpDataBuffer[(height - cinfo.output_scanline)*(width*3)+3*j+1] = buffer[0][j]; //G  
                j++;  
                bmpDataBuffer[(height - cinfo.output_scanline)*(width*3)+3*j+2] = buffer[0][j]; //B  
            }  
        }  
        if(cinfo.num_components==3)  
        {  
            for(j=0;j<(width*3);j++)  
            {  
                bmpDataBuffer[(height - cinfo.output_scanline)*(width*3)+j] = buffer[0][j+2]; //R  
                j++;  
                bmpDataBuffer[(height - cinfo.output_scanline)*(width*3)+j] = buffer[0][j]; //G  
                j++;  
                bmpDataBuffer[(height - cinfo.output_scanline)*(width*3)+j] = buffer[0][j-2]; //B  
            }  
        }  
          
    }  
    jpeg_finish_decompress(&cinfo);  
    jpeg_destroy_decompress(&cinfo);  
  
    //关闭临时文件指针，删除临时文件  
    fclose(fp);  
    remove(szTempFileName);  
  
    BITMAPFILEHEADER fileHeader;  
    BITMAPINFOHEADER infoHeader;  
  
    //bmp header(fileHeader)  
    ZeroMemory(&fileHeader, sizeof(BITMAPFILEHEADER));  
    fileHeader.bfType = 'MB';  
    fileHeader.bfSize = sizeof(BITMAPFILEHEADER)+ sizeof(BITMAPINFOHEADER) + (height * width * 3);  
    fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER)+ sizeof(BITMAPINFOHEADER) ;  
          
    //bmp infoheader(infoHeader)  
    ZeroMemory(&infoHeader, sizeof(infoHeader));  
    infoHeader.biSize = sizeof(infoHeader);  
    infoHeader.biWidth = width;  
    infoHeader.biHeight = height;  
    infoHeader.biPlanes = 1;  
    infoHeader.biBitCount = 24;  
    infoHeader.biCompression = BI_RGB;  
    infoHeader.biSizeImage = width * height;  
    infoHeader.biXPelsPerMeter = 0;  
    infoHeader.biYPelsPerMeter = 0;  
    infoHeader.biClrUsed = 0;  
    infoHeader.biClrImportant = 0;  
  
    FILE *fpbmp=fopen("a.bmp","w+b");  
    fwrite(&fileHeader,sizeof(fileHeader),1,fpbmp);  
    fwrite(&infoHeader,sizeof(infoHeader),1,fpbmp);  
    fwrite(bmpDataBuffer,width*height*3,1,fpbmp);  
    fclose(fpbmp);  
}  
  
void jpgtobmp(const char *strSourceFileName, const char *strDestFileName)  
{  
    BITMAPFILEHEADER bfh;       // bmp文件头  
    BITMAPINFOHEADER bih;       // bmp头信息  
    RGBQUAD rq[256];            // 调色板  
    int nAdjust; // 用于字节对齐  
    char indata[1000000]; // 用于存放解压缩前的图像数据，该数据直接从jpg文件读取  
  
    BYTE *data= NULL;  
    int nComponent = 0;  
  
    // 声明解压缩对象及错误信息管理器  
    struct jpeg_decompress_struct cinfo;  
    struct jpeg_error_mgr jerr;  
  
    cinfo.err = jpeg_std_error(&jerr);  
    jpeg_create_decompress(&cinfo);  
  
    FILE *f = fopen(strSourceFileName,"rb");  
    if (f==NULL)  
    {  
        printf("Open file error!\n");  
        return;  
    }  
    // 下面代码用于解压缩，从本行开始解压缩  
    jpeg_stdio_src(&cinfo, f );  
    jpeg_read_header(&cinfo, TRUE);  
    nAdjust = cinfo.image_width*cinfo.num_components%4;  
    if (nAdjust) nAdjust = 4-nAdjust;  
    data = new BYTE[(cinfo.image_width*cinfo.num_components+nAdjust)*cinfo.image_height];     
  
    jpeg_start_decompress(&cinfo);  
  
    JSAMPROW row_pointer[1];  
    while (cinfo.output_scanline < cinfo.output_height)  
    {  
        row_pointer[0] = &data[(cinfo.output_height - cinfo.output_scanline-1)*(cinfo.image_width*cinfo.num_components+nAdjust)];  
        jpeg_read_scanlines(&cinfo,row_pointer ,  
            1);  
    }  
    jpeg_finish_decompress(&cinfo);  
    jpeg_destroy_decompress(&cinfo);  
    // 上面代码用于解压缩，到本行为止解压缩完成  
  
    fclose(f);  
  
    // 以下代码讲解压缩后的图像存入文件，可以根据实际应用做其他处理，如传输  
    f=fopen(strDestFileName,"wb");  
    if (f==NULL)   
    {  
        delete [] data;  
        //delete [] pDataConv;  
        return;  
    }  
    // 写文件头  
    memset(&bfh,0,sizeof(bfh));  
    bfh.bfSize = sizeof(bfh)+sizeof(bih);  
    bfh.bfOffBits = sizeof(bfh)+sizeof(bih);  
    if (cinfo.num_components==1)  
    {  
        bfh.bfOffBits += 1024;  
        bfh.bfSize += 1024;  
    }  
  
    bfh.bfSize += (cinfo.image_width*cinfo.num_components+nAdjust)*cinfo.image_height;  
    bfh.bfType = 0x4d42;  
    fwrite(&bfh,sizeof(bfh),1,f);  
    // 写图像信息  
    bih.biBitCount = cinfo.num_components*8;  
    bih.biSize = sizeof(bih);  
    bih.biWidth = cinfo.image_width;  
    bih.biHeight = cinfo.image_height;  
    bih.biPlanes = 1;  
    bih.biCompression = 0;  
    bih.biSizeImage = (cinfo.image_width*cinfo.num_components+nAdjust)*cinfo.image_height;  
    bih.biXPelsPerMeter = 0;  
    bih.biYPelsPerMeter = 0;  
    bih.biClrUsed = 0;  
    bih.biClrImportant = 0;  
    fwrite(&bih,sizeof(bih),1,f);  
    // 写调色板  
    if (cinfo.num_components ==1)  
    {  
        for (int i=0;i<256;i++)  
        {  
            rq[i].rgbBlue =i;  
            rq[i].rgbGreen = i;  
            rq[i].rgbRed = i;  
            rq[i].rgbReserved = 0;  
        }  
        fwrite(rq,1024,1,f);  
    }  
  
    if (cinfo.num_components==3)  
    {  
        // 调整rgb顺序  
        for (int j=0;j<bih.biHeight;j++)  
            for (int i = 0;i<bih.biWidth;i++)  
            {  
                BYTE red = data[j*(cinfo.image_width*cinfo.num_components+nAdjust)+i*3];  
                data[j*(cinfo.image_width*cinfo.num_components+nAdjust)+i*3] = data[j*(cinfo.image_width*cinfo.num_components+nAdjust)+i*3+2];  
                data[j*(cinfo.image_width*cinfo.num_components+nAdjust)+i*3+2] = red;  
            }  
    }  
    fwrite(data,(cinfo.image_width*cinfo.num_components+nAdjust)*cinfo.image_height,1,f);  
    fclose(f);  
    delete [] data;  
  
}  
  
  
  
void jpgtobmp1(const char *strSourceFileName, const char *strDestFileName)  
{  
    BITMAPFILEHEADER bfh;       // bmp文件头  
    BITMAPINFOHEADER bih;       // bmp头信息  
    RGBQUAD rq[256];            // 调色板  
    int nAdjust; // 用于字节对齐  
    char indata[1000000]; // 用于存放解压缩前的图像数据，该数据直接从jpg文件读取  
  
    BYTE *data= NULL;  
    BYTE *data1=NULL;  
    int nComponent = 3;  
    int just;  
  
    // 声明解压缩对象及错误信息管理器  
    struct jpeg_decompress_struct cinfo;  
    struct jpeg_error_mgr jerr;  
  
    cinfo.err = jpeg_std_error(&jerr);  
    jpeg_create_decompress(&cinfo);  
  
    FILE *f = fopen(strSourceFileName,"rb");  
    if (f==NULL)  
    {  
        printf("Open file error!\n");  
        return;  
    }  
    // 下面代码用于解压缩，从本行开始解压缩  
    jpeg_stdio_src(&cinfo, f );  
    jpeg_read_header(&cinfo, TRUE);  
    nAdjust = cinfo.image_width*cinfo.num_components%4;  
    if (nAdjust) nAdjust = 4-nAdjust;  
  
    just = cinfo.image_width*nComponent%4;  
    if (just) just = 4-just;  
  
    data = new BYTE[(cinfo.image_width*cinfo.num_components+nAdjust)*cinfo.image_height];     
    data1=new BYTE[(cinfo.image_width*nComponent+just)*cinfo.image_height];  
    jpeg_start_decompress(&cinfo);  
  
    JSAMPROW row_pointer[1];  
    while (cinfo.output_scanline < cinfo.output_height)  
    {  
        row_pointer[0] = &data[(cinfo.output_height - cinfo.output_scanline-1)*(cinfo.image_width*cinfo.num_components+nAdjust)];  
        jpeg_read_scanlines(&cinfo,row_pointer ,  
            1);  
    }  
    jpeg_finish_decompress(&cinfo);  
    jpeg_destroy_decompress(&cinfo);  
    // 上面代码用于解压缩，到本行为止解压缩完成  
  
    fclose(f);  
  
    // 以下代码讲解压缩后的图像存入文件，可以根据实际应用做其他处理，如传输  
    f=fopen(strDestFileName,"wb");  
    if (f==NULL)   
    {  
        delete [] data;  
        //delete [] pDataConv;  
        return;  
    }  
    // 写文件头  
    memset(&bfh,0,sizeof(bfh));  
    bfh.bfSize = sizeof(bfh)+sizeof(bih);  
    bfh.bfOffBits = sizeof(bfh)+sizeof(bih);  
  
    bfh.bfSize += (cinfo.image_width*nComponent+just)*cinfo.image_height;  
    bfh.bfType = 0x4d42;  
    fwrite(&bfh,sizeof(bfh),1,f);  
    // 写图像信息  
    bih.biBitCount = nComponent*8;  
    bih.biSize = sizeof(bih);  
    bih.biWidth = cinfo.image_width;  
    bih.biHeight = cinfo.image_height;  
    bih.biPlanes = 1;  
    bih.biCompression = 0;  
    bih.biSizeImage = (cinfo.image_width*nComponent+just)*cinfo.image_height;  
    bih.biXPelsPerMeter = 0;  
    bih.biYPelsPerMeter = 0;  
    bih.biClrUsed = 0;  
    bih.biClrImportant = 0;  
    fwrite(&bih,sizeof(bih),1,f);  
  
    for(int testi=0; testi<((cinfo.image_width*cinfo.num_components+nAdjust)*cinfo.image_height); testi++)  
    {  
        data1[testi*3]=data[testi];  
        data1[testi*3+1]=data[testi];  
        data1[testi*3+2]=data[testi];  
    }  
  
  
    fwrite(data1,(cinfo.image_width*nComponent+just)*cinfo.image_height,1,f);  
    fclose(f);  
    delete [] data;  
  
}  
  
  
  
int main()  
{  
    FILE *fp=NULL;  
  
    //ID3大小  
    int Len=0;  
    fp=OpenMusic(Len);  
    if(NULL==fp)  
    {  
        return 0;  
    }  
  
    //图片帧大小  
    int dwFrame=0;  
    //记录图片标签数据中不是图片数据的字节数  
    int tempi=0;  
    if(FALSE==GetPicInfo(fp,dwFrame,Len,tempi))  
    {  
        return 0;  
    }  
  
    //获取图片数据  
    GetPicRGB(fp,dwFrame,tempi);  
    jpgtobmp("temp.jpeg","b.bmp");  
    jpgtobmp1("temp.jpeg","c.bmp");  
    return 1;  
}  
