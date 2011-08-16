

#pragma once

/******************************************************************************\
*
*   IMAGE
*
*       luminance image
*
\******************************************************************************/

#include "winTypes.h"

#include "wrect.h"


//#define _aligned_free delete
//#define _aligned_malloc malloc

class I_IMAGE; 
class IN_IMAGE; 

#define I2TYPE    __int64

namespace color_conv        // name space for color conversion 
{
    static unsigned char uchar_table[768]; 
    static unsigned char *fast_uchar = &uchar_table[256]; 
    static int fast_rgb2yuv_r_y[256]; 
    static int fast_rgb2yuv_g_y[256]; 
    static int fast_rgb2yuv_b_y[256]; 
    static int fast_rgb2yuv_r_u[256]; 
    static int fast_rgb2yuv_g_u[256]; 
    static int fast_rgb2yuv_b_u[256]; 
    static int fast_rgb2yuv_r_v[256]; 
    static int fast_rgb2yuv_g_v[256]; 
    static int fast_rgb2yuv_b_v[256]; 
    static int fast_yuv2rgb_y_rgb[256]; 
    static int fast_yuv2rgb_v_r[256]; 
    static int fast_yuv2rgb_u_g[256]; 
    static int fast_yuv2rgb_v_g[256]; 
    static int fast_yuv2rgb_u_b[256]; 
    static int init = 0;
}; 

class IMAGEC; 

// image class, we only handle gray scale images here
class IMAGE 
{
private: 

    int m_width;                // width of image
    int m_stride;               // stride of image 
    int m_height;               // height of image
    int m_memSize;              // memory size 
    BYTE *m_bData;               // ptr to image bits

    inline int    GetIndex(int x, int y) const
    {
        ASSERT (x >= 0 && x < m_width && y >= 0 && y < m_height); 
        const int index = y * m_stride + x; 
        return index;
    }

    //void          DrawPixel(int x, int y, float alpha, BYTE value);
    inline int    ComputeStride (int w) { return ((w+3)/4)*4; };     // color depth is always 1, 4-byte aligned

public:
    
    IMAGE();
    IMAGE(int width, int height, int stride=0, BYTE *pData=NULL);
    IMAGE(int width, int height, int stride, int bytePerPixel, BYTE *pData);
    ~IMAGE();

    bool          Realloc(int width, int height);
    void          Release(); 
    void          ZeroImage(); 
    inline BYTE   GetValue(int x, int y) const { const int index = GetIndex(x,y); return m_bData[index]; }; 
    inline void   SetValue(int x, int y, BYTE value) { const int index = GetIndex(x,y); m_bData[index] = value; }; 
    inline int    GetWidth() const { return m_width; }; 
    inline int    GetHeight() const { return m_height; }; 
    inline int    GetStride() const { return m_stride; }; 
    inline BYTE * GetDataPtr() const { return m_bData; }; 
    inline BYTE * GetPixelAddress(int x, int y) const {return (m_bData + GetIndex(x,y)); };

    // a few function for post filtering 
    void          PreProc4SVM(float *pfOutData); 
};

/******************************************************************************\
*
*   I_IMAGE class
*
*       Integral Image Class
*
*       For the storage of the integral image of the luminance of an image
*       Note: the size of the integral image is always the original image size + 1 pixel row and column
*       This is due to the definition of integral image as: 
*       II(x,y) = sum_{0<=x'<x,0<=y'<y} I(x',y'). 
*       The first row and column of the integral image is always 0
*
\******************************************************************************/

class I_IMAGE 
{
protected: 
    int m_width;                            // width of image
    int m_height;                           // height of image 
    int m_memSize;                          // memory size 
    unsigned int *m_iData;                   // ptr to memory

public:

    I_IMAGE();
    //
    // Allocates an empty integral image 
    // CAUTION: call I_IMAGE constructor using the original image's width and height!
    //
    I_IMAGE (int width, int height);
    ~I_IMAGE();

    // CAUTION: call Realloc() using the original image's width and height!
    bool          Realloc(int width, int height);
    void          Release(); 

    void Init(const IMAGE* pImage);
    void InitWithSubSample(const IN_IMAGE* pIImage, int x, int y, float scale); 
    //void Init(const IMAGE* pImageA, const IMAGE* pImageB);

    inline int GetIndex(int x, int y) const
    {
        ASSERT(0 <= x && x < m_width && 0 <= y && y < m_height);
        return (y * m_width + x);
    }; 

    unsigned int GetValue(int x, int y) const; 
    double GetValueD(double x, double y); 
    // NOTE: the width and height returned are the corresponding regular image's width and heigh 
    inline int GetWidth() const { return m_width-1; }; 
    inline int GetHeight() const { return m_height-1; }; 
    inline int GetIWidth() const { return m_width; }; 
    inline int GetIHeight() const { return m_height; }; 
    inline unsigned int * GetDataPtr() const { return m_iData; }; 

    float ComputeAvg(IRECT *rc); 
	void CropScaleToImage(IMAGE *pCImg, const IRECT *pRect); 
};

/******************************************************************************\
*
*   IN_IMAGE class
*
*       Integral Image Class with normalization 
*
\******************************************************************************/
class IN_IMAGE : public I_IMAGE    // normalized integral image 
{
protected: 
    I2TYPE *m_llData; 

public:
    float  *m_sqrtTable; 

public: 
    IN_IMAGE();
    //
    // Allocates an empty integral image 
    // CAUTION: call I_IMAGE constructor using the original image's width and height!
    //
    IN_IMAGE (int width, int height);
    ~IN_IMAGE();

    // CAUTION: call Realloc() using the original image's width and height!
    bool          Realloc(int width, int height);
    void          Release(); 

    void Init(const IMAGE* pImage);

    I2TYPE GetValue2(int x, int y) const; 
    inline I2TYPE * GetDataPtr2() const { return m_llData; }; 

    float ComputeNorm(IRECT *rc); 
}; 

/******************************************************************************\
*
*   IMAGEC
*
*       Color 3 channel image.  
*
\******************************************************************************/

class IMAGEC
{
private: 

    int m_width;                // width of image
    int m_height;               // height of image
    int m_stride;               // stride of image
    int m_memSize;              // memory size 
    BYTE *m_bData;               // ptr to image bits

public: 
    enum COLORSPACE
    {
        UNSPECIFIED,
        RGB,
        BGR,
        YUV
    }; 

private: 
    COLORSPACE m_colorSpace;    // CAUTION: when loading an image from disk, the color space
                                // is set by default, for instance, if loading
                                // BMP, the color space will be BGR, if loading PPM
                                // or JPG, the color space will be RGB
                                // the user is responsible to convert the image to
                                // another color space if wanted. 

    inline int    GetIndex(int x, int y) const
    {
        ASSERT (x >= 0 && x < m_width && y >= 0 && y < m_height); 
        const int index = y * m_stride + x * 3; 
        return index;
    }
    inline int    ComputeStride (int w) { return ((w*3+3)/4)*4; };

    //void          DrawPixel(int x, int y, float alpha, BYTE color[3]);

public:
    
    IMAGEC();
    IMAGEC(int width, int height, COLORSPACE colorspace = UNSPECIFIED, int stride=0, BYTE *pData=NULL);
    ~IMAGEC();


    bool          Realloc(int width, int height);
    void          Release(); 
    inline void   GetValue(int x, int y, BYTE color[3])
    { 
        const int index = GetIndex(x,y); 
        color[0] = m_bData[index]; 
        color[1] = m_bData[index+1]; 
        color[2] = m_bData[index+2]; 
    }; 
    inline void   SetValue(int x, int y, BYTE color[3]) 
    { 
        const int index = GetIndex(x,y); 
        m_bData[index] = color[0]; 
        m_bData[index+1] = color[1]; 
        m_bData[index+2] = color[2]; 
    }; 
    inline int    GetWidth() const { return m_width; }; 
    inline int    GetDepth() const { return 3; }; 
    inline int    GetStride() const { return m_stride; }; 
    inline int    GetHeight() const { return m_height; }; 
    inline COLORSPACE GetColorSpace() const { return m_colorSpace; }; 
    inline void   SetColorSpace(COLORSPACE clr) { m_colorSpace = clr; }; 
    inline BYTE * GetDataPtr() const { return m_bData; }; 
    inline BYTE * GetPixelAddress(int x, int y) const {return (m_bData + GetIndex(x,y)); }; 

    //void          ConvertCFromY(IMAGE *pImg, COLORSPACE clrDst, bool bVFlip=false); 
    //void          ConvertCToC(IMAGEC *pImg, COLORSPACE clrDst, bool bVFlip=false); 
 
};