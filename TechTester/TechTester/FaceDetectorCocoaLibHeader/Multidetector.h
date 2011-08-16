
#pragma once

#include "Stdafx.h"

#include <float.h>
#include "classifier.h"
#include "image.h"
#include "feature.h"
#include "winTypes.h"
#include "ATLFake.h"

#include <vector>

namespace ILabs { class ILabsPostFilter; }

namespace MutilFaceDetector
{
//#define COUNT_PRUNE_EFFECT  
#define USE_FASTSKIP
#if defined (USE_FASTSKIP) 
#define MAX_NUM_4_SKIP                        10
#endif

#define MAX_IMG_WIDTH                        32768
#define DEFAULT_MAX_NUM_RAW_DET_RECT        1200
#define MAX_NUM_MERGE_RECT                    20000
#define REQUIRED_OVERLAP                    0.4
#define MERGED_OVERLAP                        0.4
#define MAX_DET_GROUP                       128  // the detection rect groups for each image, could be smaller
#define POST_FILTER_SCORE_GAP_TH            2.0

#ifndef MAX_NUM_SCALE
#define MAX_NUM_SCALE                       17 // original value is 32. 17 means 1065x1065 rects
                                               // enough when image resolution smaller than 1024
#endif


// Type for Overlapping of IRECTs.
typedef enum
{
   NONE = 0,
   HOR_OVERLAP,
   ALL_OVERLAP
} IRECT_OVERLAP;

typedef enum 
{
    PRE_FILTER, 
    POST_FILTER
} DET_FILTER_SETTING;
	
typedef struct
{
	int x_offset;
	int y_offset;
} PosOffset;
	
class ACCELERATOR
{
public :
	bool m_bUsed;
	bool m_bUseScale;
	bool m_bUsePos;
	//scale
	int m_maxScaleIte;
	int m_prevFrameScale;
	std::vector<int> m_scaleArray;
	
	
	//offset
	int m_maxPosOffsetIte;
	PosOffset m_prevPosOffset;
	std::vector<PosOffset> m_posOffsetArray;
	
	ACCELERATOR();
	~ACCELERATOR();
	
	void optimize(bool bUsed, int centerScale, PosOffset centerPosOffset);
	bool initScaleArray();
	bool initPosOffsetArray();
	
};

class MERGERECT
{

private :
    ATL::CAtlArray<double> m_saAreas;

    struct SCORED_RECT_DATA
    {
        SCORED_RECT rect;
        double area;
    };
    
    ATL::CAtlList<SCORED_RECT_DATA> m_listDetectedRects;
    ATL::CAtlArray<ID_SCORED_RECT> m_saGroupedRegions;
    ATL::CAtlArray<ATL::CAtlList<ID_SCORED_RECT*> > m_saScoredRectCollections;

private :
    // Compute the intersection of rc1 & rc2, and return the intersection is
    // Returns to what extent the rectangles overlap.
    IRECT_OVERLAP _IRECT_Intersect(IRECT* rc1, IRECT* rc2, IRECT* is);

    /// Checks for rectangle overlap and links the two rectangles if they do.
    void _RectangleOverlapHelper(int n_srcs, double requiredOverlap);

public:
    MERGERECT();
    ~MERGERECT();

    /// Average the coordinates of the rectangles to yield a new rectangle.
    void AverageRectList(int nScoredRectCollectionsIndex);

    //Find sets of overlapping rectangles and average each set into a single rectangle.
    //From n_srcs number of srcs, find overlapping groups.
    //Return 'n_dts number' of merged rectangles 'dsts', and associated group_ids for every srcs to find appropriate dsts.
    void MergeRectangles(SCORED_RECT* srcs, int n_srcs, 
                         SCORED_RECT* dsts, int* n_dsts, 
                         int Max_merged_detection);
	void UpdatePosForEarlyRemove( ATL::POSITION& pos, ATL::POSITION delPos );
};

/******************************************************************************\
*
*   DETECTOR
*
\******************************************************************************/

class DETECTOR 
{
    public: 
        DETECTOR (); 

        DETECTOR( const char *pszCascadeFileName,
            int maxNumScale = MAX_NUM_SCALE,
            int maxNumRawDetRect = DEFAULT_MAX_NUM_RAW_DET_RECT); 

        DETECTOR( const char *pszCascadeFileName, 
            const char *pszLbpPostFileterFileName, 
            int maxNumScale = MAX_NUM_SCALE,
            int maxNumRawDetRect = DEFAULT_MAX_NUM_RAW_DET_RECT); 

        // Removed to prevent compiler from picking wrong constructor as it gets confused
        // with the use of default paramters which to use.
        

        ~DETECTOR();
        void Init(); 
        void Release(); 

    private: 
        IN_IMAGE    *m_IImg;                // ptr to integral image
        I_IMAGE     *m_IImgM;               // ptr to mask image's integral image

        int             m_nPrevIImgWidth; 
        int             m_nPrevMinScale; 
        int             m_nPrevMaxScale; 

        int          m_nClassifiers;        // number of classifiers
        CLASSIFIER*  m_aClassifiers; 
        FEATURE*     m_apScaledFeatures[MAX_NUM_SCALE];
        // Buffer that holds all of the scaled feature data into which the elements of m_ScaledFeatures points
        FEATURE*     m_aScaledFeaturesBuffer;

        int          m_nClassifiersCat[MAX_NUM_CATEGORIES]; 
        CLASSIFIER*  m_apClassifiersCat[MAX_NUM_CATEGORIES]; 
        // Buffer that holds all of the category classifiers. m_apClassifiersCat points into this buffer at the start of each offset.
        CLASSIFIER*  m_aClassifiersCatBuffer;
        FEATURE*     m_aapScaledFeaturesCat[MAX_NUM_SCALE][MAX_NUM_CATEGORIES];
        // Buffers that hold all of the scaled feature data into which the elements of m_ScaledFeaturesCat points
        FEATURE*     m_aScaledFeaturesBufferCat;

        int          m_nWidth[MAX_NUM_SCALE]; 
        int          m_nHeight[MAX_NUM_SCALE]; 
        int          m_nStepW[MAX_NUM_SCALE]; 
        int          m_nStepH[MAX_NUM_SCALE]; 
        int             m_nNumCat; 

        bool         m_bValid; 
        int          m_nNumFeatureTh;       // number of thresholds for each feature 
        float        m_fFinalScoreTh;
        int          m_nMaxNumScale;
        int          m_nMaxNumRawDetRect; 

        int          m_nNumRawDetRect; 
        SCORED_RECT *m_pRawDetRect; 
        int          m_nNumMergedDetRect; 
        SCORED_RECT *m_pMergedDetRect; 

        MERGERECT    m_MergeRect;
	
		ACCELERATOR  m_Accelerator;
	
		//whether use the information of prev frame
		bool         m_bUsed;
        
        IMAGE* m_pFaceImg;

    #if defined (USE_FASTSKIP) 
        int            *m_pnSkipArray; 
    #endif 

        float         m_fImgData[BASE_WIDTH*BASE_HEIGHT]; 
        float        *m_fScoreVect; 

        enum EPostFilter
        {
            PostFilterNone,
            SvmPostFilter,
            ILabsPostFilter
        };
        
        EPostFilter  m_ePostFilter;

        float         m_fSVMPostFilterTh[MAX_NUM_CATEGORIES]; 
        int             m_nPCADim[MAX_NUM_CATEGORIES]; 
        float        *m_fPCAVect[MAX_NUM_CATEGORIES]; 
        //SimpleCompressedPolyModel m_SVMModel[MAX_NUM_CATEGORIES]; 
        int          m_nNumPostFilteredDetRect; 
        SCORED_RECT *m_pPostFilteredDetRect; 
        ILabs::ILabsPostFilter* m_pILabsPostFilter;

        void Prepare4FastComputing(int minScale, int maxScale); 
        bool Classify (IRECT *rc, int nScale, float *score, float *pfScore, int *num_or_cat); 
        // if Classify returns true, num_or_cat returns the subcategory id, otherwise, num_or_cat returns the number of weak 
        // classifiers the rectable has passed before being rejected (only up to the main classifier, not the individual subcategory classifiers)
        bool ClassifyM (IRECT *rc, int nScale, float maskTh, DET_FILTER_SETTING fltSetting, float *score, int *num_or_cat); 
        void SetPruneMinPosThreshold (IRECT *rc, int nScale); 
        bool MergeRawDetRect(); 

        //void LoadSVMPostFilter (HMODULE hModule, DWORD dwSvmPostFilterResource); 
        //void LoadILabsPostFilter (HMODULE hModule, DWORD dwILabsPostFilterResource); 
        //void SVMPostFilterResults();
        //void ILabsPostFilterResults(); 

        void InitializeRectAndScaleData(int maxNumScale, int maxNumRawDetRect);
        void InitializePruneAndSkipData();

        void CreateClassifierArray();
        void CreateClassifierArrayCat();
        void CreateFeatureBuffer();
        void CreateFeatureBufferCat();

        void ScaleFeatures(int nClassifiers);
        void ScaleFeaturesCat(int nNumCat, int nClassifiers);

    #if defined(COUNT_PRUNE_EFFECT)
        __int64 *m_pnPruneCount; 
    #endif 

    public:

        //void LoadSVMPostFilter (const char *pszSvmPostFilterFileName); 

        void LoadILabsPostFilter (const char *pszLbpFilterFileName); 

        void Load( const char *pszCascadeFileName, 
                  int maxNumScale = MAX_NUM_SCALE,
                  int maxNumRawDetRect = DEFAULT_MAX_NUM_RAW_DET_RECT); 

        void Load( const char *pszCascadeFileName, 
                   const char *pszLbpPostFilterFileName, 
                  int maxNumScale = MAX_NUM_SCALE,
                  int maxNumRawDetRect = DEFAULT_MAX_NUM_RAW_DET_RECT); 

        float GetFinalScoreTh()        { return m_fFinalScoreTh; }; 
        void  SetFinalScoreTh(float th) { m_fFinalScoreTh = th; }; 
        int   GetNumClassifiers()    { return m_nClassifiers; }; 

    #if defined(COUNT_PRUNE_EFFECT)
        __int64 *GetPruneCount()    { return m_pnPruneCount; }; 
    #endif

        // the return value is the number of rectangles detected, up to MAX_NUM_DET_RECT
        void DetectObject (IN_IMAGE* pIImg, int minScale=0, int maxScale=MAX_NUM_SCALE-1, int isIpad = 0);

        // detection with a mask image 
        // there are two detection filter settings: pre-filter and post-filter 
        // in pre-filter, rectangles that below mastTh will be rejected immediately, leading to a very fast rejection 
        // in post-filter, rectangles go through the feature based detector first. the output results then run through
        // the masking process. A parameter, POST_FILTER_SCORE_GAP_TH, can be set such that if a rectangle passes the 
        // detector with score POST_FILTER_SCORE_GAP_TH higher than the rejection score, masking will be ignored. 

        // in general, if the user is very confident that the masking is very accurate, pre-filter is recommended 
        // otherwise, post-filter is recommended. 
        void DetectObject (IN_IMAGE* pIImg, I_IMAGE *pIImgM, float maskTh, 
                           DET_FILTER_SETTING fltSetting, 
                           int minScale=0, int maxScale=MAX_NUM_SCALE-1, int isIpad = 0);

        // Additionally allocate memory to store the computed feature values for all detected faces.    
        //void DetectObjectWithFeatures (I_IMAGE* pIImg, int minScale=0, int maxScale=MAX_NUM_SCALE-1);
        int     GetDetResults(SCORED_RECT **ppRc, bool merged);
        // threshold values first, raw values later for det_i'th detection
        //void GetFeatureValues(float *values, int det_i);
        bool IsValid() { return m_bValid; }; 

        void SaveNewClassifier(char *fileName); 
    };
} /// namespace MutilFaceDetector

