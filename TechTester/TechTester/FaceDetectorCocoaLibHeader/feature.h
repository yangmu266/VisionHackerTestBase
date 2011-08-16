#pragma once

/******************************************************************************\
*
*   A FILTER object contains a list of weighted rectangles
*
\******************************************************************************/

#include "wrect.h"
#include "image.h"


//#define USE_VARIANCE_FEATURE
#if defined (USE_VARIANCE_FEATURE)
#define MIN_VAR_FEATURE_AREA        36
#endif 

//#define INCLUDE_RANDOM_RCFEATURES
#if defined(INCLUDE_RANDOM_RCFEATURES) 
#define NUM_RANDOM_RCFEATURES   200000
#endif

//#define USE_LB_FEATURE

struct RCFEATURE
{
    int m_nRects;                       // number of weighted rectangles
    WEIGHTED_RECT *m_wRectArray;        // ptr to an array of weighted rectangle bjects

    RCFEATURE();
    RCFEATURE(const RCFEATURE *src, const float scale = 1);

    bool Init(int nRects);
    bool Init(int nRects, int nCurrentClassifier);
    bool Init(FILE *file, bool bBinary);
    bool Init(const BYTE* buffer, int iBufLen, int& iStreamPos, bool bBinary);
    bool Init(const RCFEATURE *src, const float scale = 1); 
    void Write(FILE *file, bool bBinary); 
    void Release(); 

    void ComputeOffsets(int width); 

    // CAUTION: in Eval(), x and y are the top left corner of the measured rectangle, the feature's 
    // offset will be added to the (x,y) coordinate 
    inline float Eval(I_IMAGE *pIImg, float norm, int x, int y); 
    inline float Eval(I_IMAGE *pIImg, float norm, int bufoffsetXY);    // fast version for detection 

    ~RCFEATURE();
};

struct LBFEATURE
{
    int m_nRects;                       // number of weighted rectangles
    WEIGHTED_RECT *m_wRectArray;        // ptr to an array of weighted rectangle bjects

    LBFEATURE();
    LBFEATURE(const LBFEATURE *src, const float scale = 1);

    bool Init(int nRects);
    bool Init(int nRects, int nCurrentClassifier);
    bool Init(FILE *file, bool bBinary);
    bool Init(const BYTE* buffer, int iBufLen, int& iStreamPos, bool bBinary);
    bool Init(const LBFEATURE *src, const float scale = 1); 
    void Write(FILE *file, bool bBinary); 
    void Release(); 

    void ComputeOffsets(int width); 

    // CAUTION: in Eval(), x and y are the top left corner of the measured rectangle, the feature's 
    // offset will be added to the (x,y) coordinate 
    inline float Eval(I_IMAGE *pIImg, float norm, int x, int y); 
    inline float Eval(I_IMAGE *pIImg, float norm, int bufoffsetXY);    // fast version for detection 

    ~LBFEATURE();
}; 

struct VARFEATURE
{
    int m_nRects;                       // number of weighted rectangles
    WEIGHTED_RECT *m_wRectArray;        // ptr to an array of weighted rectangle bjects

    VARFEATURE();
    VARFEATURE(const VARFEATURE *src, const float scale = 1);

    bool Init(int nRects);
    bool Init(int nRects, int nCurrentClassifier);
    bool Init(FILE *file, bool bBinary);
    bool Init(const BYTE* buffer, int iBufLen, int& iStreamPos, bool bBinary);
    bool Init(const VARFEATURE *src, const float scale = 1); 
    void Write(FILE *file, bool bBinary); 
    void Release(); 

    void ComputeOffsets(int width); 

    // CAUTION: in Eval(), x and y are the top left corner of the measured rectangle, the feature's 
    // offset will be added to the (x,y) coordinate 
    inline float Eval(IN_IMAGE *pIImg, float norm, int x, int y); 
    inline float Eval(IN_IMAGE *pIImg, float norm, int bufoffsetXY);    // fast version for detection 

    ~VARFEATURE();
};

struct FEATURE
{
    enum FEATURETYPE
    {
        UNKNOWN, 
        RECTFEATURE, 
        NORMFEATURE, 
        VARIANCEFEATURE, 
        LOCALBINFEATURE
    } m_nType; 
    union FEATUREPTR
    {
        RCFEATURE *pRCF; 
        VARFEATURE *pVF; 
        LBFEATURE *pLBF; 
    } m_pF; 

    bool Init(const FEATURE *src, const float scale = 1); 
//    void Init(int nCurrentClassifier);
    bool Init(FILE *file, bool bBinary); 
    bool Init(const BYTE* buffer, int iBufLen, int& iStreamPos, bool bBinary);
    void Write(FILE *file, bool bBinary); 
    void Release(); 

    void ComputeOffsets(int width); 

    float Eval(IN_IMAGE *pIImg, float norm, int x, int y); 
    float Eval(IN_IMAGE *pIImg, float norm, int bufoffsetXY);    // this is a faster version of evaluation 
                                                                // used for speeding up detection only

    static FEATURE* CreateCompleteFeatureArray(int width, int height, 
        int fwidth, int fheight, float fssize, float fsscale, int *pCount); 
    static void DeleteFeatureArray(FEATURE *featureArray);

    FEATURE(); 
    ~FEATURE(); 
}; 
