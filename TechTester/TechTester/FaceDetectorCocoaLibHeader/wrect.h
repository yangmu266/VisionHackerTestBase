#pragma once

#include "winTypes.h"

#include "globalpara.h"

#include "windows.h"
#include <math.h>
#include <iostream>
#include <ASSERT.h> 

#ifndef ASSERT
#ifdef DEBUG
#define ASSERT( x ) assert( x )
#else
#define ASSERT( x )
#endif
#endif

struct FPOINT
{
    float x;
    float y; 
}; 

#define FACETYPE_FRONTAL        0
#define FACETYPE_HALFPROFILE    1
#define FACETYPE_PROFILE        2
#define FACETYPE_NONE          -1

struct FEATUREPTS
{
    int type;   // type take one of the defined value above 
    FPOINT leye; 
    FPOINT reye; 
    FPOINT nose; 
    FPOINT lmouth; 
    FPOINT rmouth;
}; 

struct IRECT {
    int m_ixMin;
    int m_ixMax;
    int m_iyMin;
    int m_iyMax;

    IRECT(){}

    //
    //  Resets the integer rectangle object so that the origin is as close
    //  as possible to the specfied column and row, and the width and height
    //  are as close a possible to the non-interger values specified by
    //  the caller.
    //
    void Reset(float col, float row, float curWidth, float curHeight)
    {
        m_ixMin = (int)(col + 0.5f);
        m_ixMax = (int)(m_ixMin + curWidth + 0.5f);   // this make sure all rectangle has same width no
                                                    // matter what 'col' is
        m_iyMin = (int)(row + 0.5f);
        m_iyMax = (int)(m_iyMin + curHeight + 0.5f); 
    }

    void Reset(int col, int row, int curWidth, int curHeight)
    {
        m_ixMin = col;
        m_ixMax = m_ixMin + curWidth;
        m_iyMin = row;
        m_iyMax = m_iyMin + curHeight; 
    }

    void Reset(const IRECT& other) 
    {
        m_ixMin = other.m_ixMin; 
        m_ixMax = other.m_ixMax; 
        m_iyMin = other.m_iyMin; 
        m_iyMax = other.m_iyMax; 
    }

    //void MakeSquare()
    //{
    //    int xctr = (m_ixMin + m_ixMax + 1)/2; 
    //    int yctr = (m_iyMin + m_iyMax + 1)/2; 
    //    int width = m_ixMax - m_ixMin; 
    //    int height = m_iyMax - m_iyMin; 
    //    int size = max(width, height); 
    //    m_ixMin = (2*xctr - size + 1)/2; 
    //    m_ixMax = m_ixMin + size; 
    //    m_iyMin = (2*yctr - size + 1)/2; 
    //    m_iyMax = m_iyMin + size; 
    //}
    
    bool DetectMatchTight(const IRECT& other, float stepsize, float stepscale) const;
    bool DetectMatchLoose(const IRECT& other) const; 
    bool DetectMatchVeryLoose(const IRECT& other) const;
    bool DetectPersonMatchVeryTight(const IRECT& headRect, float stepsize, float stepscale) const; 

    double Distance(const IRECT& other) const 
    {
        float xctr1 = (m_ixMin + m_ixMax)/2.0f; 
        float yctr1 = (m_iyMin + m_iyMax)/2.0f; 
        float xctr2 = (other.m_ixMin + other.m_ixMax)/2.0f; 
        float yctr2 = (other.m_iyMin + other.m_iyMax)/2.0f; 
        return sqrt((xctr1-xctr2)*(xctr1-xctr2) + (yctr1-yctr2)*(yctr1-yctr2));
    }

    bool Contains(const FPOINT& pt) const
    {
        if (pt.x < m_ixMin || pt.y < m_iyMin ||
            m_ixMax < pt.x || m_iyMax < pt.y)
            return false;
        return true;
    }

    void Shift(int x, int y)
    {
        m_ixMin += x; 
        m_ixMax += x; 
        m_iyMin += y; 
        m_iyMax += y; 
    }

    void Scale(float fScale)
    {
        float xctr = (m_ixMin + m_ixMax)/2.0f; 
        float yctr = (m_iyMin + m_iyMax)/2.0f; 
        float newWidth = Width()*fScale; 
        float newHeight = Height()*fScale; 
        Reset(xctr-newWidth/2.0f, yctr-newHeight/2.0f, newWidth, newHeight); 
    }

    // computes the area of a rectangle.
    double Area()
    {
        return( (double) (m_ixMax - m_ixMin) * (m_iyMax - m_iyMin));
    }

    int Width() 
    {
        return m_ixMax - m_ixMin; 
    }

    int Height() 
    {
        return m_iyMax - m_iyMin; 
    }

    // less operator for sorting. a < b when a's left side is further left than b's left side.
    inline bool operator<(const IRECT& other) 
    {
        return (m_ixMin < other.m_ixMin);
    }


    //
    //  Returns true if the other rectangle is a subset of this rectangle.   
    //
    //bool Contains(const IRECT& other) const
    //{
    //    if (other.m_ixMin < m_ixMin || other.m_iyMin < m_iyMin ||
    //        m_ixMax < other.m_ixMax || m_iyMax < other.m_iyMax)
    //        return false;
    //    return true;
    //}

    //
    //  Returns true if the other rectangle is the same as this rectangle.   
    //
    //bool Equal(const IRECT& other) const
    //{
    //    if (other.m_ixMin == m_ixMin && other.m_iyMin == m_iyMin &&
    //        m_ixMax == other.m_ixMax && m_iyMax == other.m_iyMax)
    //        return true;
    //    return false;
    //}

    void Clamp(int xMin, int yMin, int xMax, int yMax)
    {
        if (m_ixMin < xMin) m_ixMin = xMin; 
        if (m_iyMin < yMin) m_iyMin = yMin; 
        if (m_ixMax > xMax) m_ixMax = xMax; 
        if (m_iyMax > yMax) m_iyMax = yMax; 
    }

    //void RandSubRect(int width, int height, int subwidth, int subheight)
    //{
    //    float rw = 1.0f*width/subwidth; 
    //    float rh = 1.0f*height/subheight; 
    //    int w, h; 
    //    if (rw <= rh) 
    //    {
    //        w = rand()%width; 
    //        while (w < subwidth) 
    //            w = rand()%width; 
    //        h = w*subheight/subwidth; 
    //    }
    //    else
    //    {
    //        h = rand()%height; 
    //        while (h < subheight) 
    //            h = rand()%height; 
    //        w = h*subwidth/subheight; 
    //    }
    //    m_ixMin = rand()%(width-w); 
    //    m_iyMin = rand()%(height-h); 
    //    m_ixMax = m_ixMin + w; 
    //    m_iyMax = m_iyMin + h; 
    //}

    //
    //  Constructor for this integer rectangle.
    //
    IRECT(int xMin, int xMax, int yMin, int yMax) : 
        m_ixMin(xMin),
        m_ixMax(xMax), 
        m_iyMin(yMin), 
        m_iyMax(yMax)
    {
        ASSERT(xMin <= xMax);
        ASSERT(yMin <= yMax);
    }
};

/******************************************************************************\
*
*   Class of weighted rectangles
*
*   These objects represent the the 'atomic' rectangles of the filters.
*   They are associated with a specific image and have a weight.
*
\******************************************************************************/

struct WEIGHTED_RECT
{
    int          m_iimgBufOffset_v00; 
    int          m_iimgBufOffset_v01; 
    int          m_iimgBufOffset_v10; 
    int          m_iimgBufOffset_v11; 

    float     m_weight;
    IRECT     m_rect;

//    void Init(int nRect, int nCurrentClassifier);
    void Init(FILE *file, bool bBinary);
    void Init(const BYTE* buffer, int iBufLen, int& iStreamPos, bool bBinary);

    void Write(FILE *file, bool bBinary); 
};

/*****************************************************************************\
*
*   Class of rectangles with scores 
*
******************************************************************************/
struct SCORED_RECT
{
    float m_score; 
    int m_cat; 
    float m_rgfScore[MAX_NUM_CATEGORIES];
    IRECT m_rect; 
}; 

struct LABEL_RECT
{
    char m_label; 
    int m_scale; 
    IRECT m_rect; 
}; 

struct GT_RECT
{
    float m_theta;      // rotation angle, clock wise 
    IRECT m_rect; 

    bool Contains(const FPOINT& pt) const
    {
        float xctr = (m_rect.m_ixMin + m_rect.m_ixMax)/2.0f; 
        float yctr = (m_rect.m_iyMin + m_rect.m_iyMax)/2.0f; 
        float alpha = -atan2(pt.y-yctr, pt.x-xctr); 
        float r = sqrt((pt.x-xctr)*(pt.x-xctr) + (pt.y-yctr)*(pt.y-yctr)); 
        FPOINT rPt; 
        rPt.x = xctr + r*cos(alpha + m_theta); 
        rPt.y = yctr + r*sin(alpha + m_theta); 
        return m_rect.Contains(rPt); 
    }
};


/******************************************************************************\
*
*   Class of IDed rectangles
*
*   This type is used when the rectnagles are sorted.
*   They are associated with a specific ID.
*
\******************************************************************************/

struct ID_SCORED_RECT
{
    int            id_;                // original id
    int            sid;            // id after sorting
    int            group_id;        // group id (initially set to sid)
    SCORED_RECT*        rc;


    ID_SCORED_RECT()
    {
        id_ = -1;
        group_id = -1;
        rc = NULL;
    }
};

extern void MapFPts2GTRc(FEATUREPTS *pPts, GT_RECT *pRc, bool bCorrectLR = false); 