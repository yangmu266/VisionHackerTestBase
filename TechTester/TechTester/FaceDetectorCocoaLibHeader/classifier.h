
#pragma once

/******************************************************************************\
*
*   
*
\******************************************************************************/

#include "globalpara.h"
#include "feature.h"

class CLASSIFIER
{
public: 
    CLASSIFIER(); 
    ~CLASSIFIER(); 

public: 
    int         m_nBranchFlag;      // flag to indicate if re-initialization is needed 
    int         m_nNumCat; 
    int         m_nBranchMapping[MAX_NUM_CATEGORIES]; 
    float       m_pfDScore[MAX_NUM_CATEGORIES][NUM_FEATURE_TH+1]; 
    float       m_pfMinPosScoreTh[MAX_NUM_CATEGORIES]; 

    // Points to external buffer containing the feature data for scale zero of this classifier
    FEATURE*    m_pFeature; 
    float       m_pfFeatureTh[NUM_FEATURE_TH]; 

    inline void  SetReInitFlag(int nFlag) { m_nBranchFlag = nFlag; }; 
    inline void  SetNumCat(int nNum) { m_nNumCat = nNum; }; 
    inline void  SetFeatureTh(float *pTh) { for (int i=0; i<NUM_FEATURE_TH; i++) m_pfFeatureTh[i] = pTh[i]; }; 
    inline void  SetDScore(float (*pScore)[NUM_FEATURE_TH+1]) 
    { 
        for (int i=0; i<m_nNumCat; i++)
            for (int j=0; j<NUM_FEATURE_TH+1; j++) 
                m_pfDScore[i][j] = pScore[i][j]; 
    }; 
    inline void  SetMinPosScoreTh(float *pMinTh) 
    { 
        for (int i=0; i<m_nNumCat; i++) 
            m_pfMinPosScoreTh[i] = pMinTh[i]; 
    }; 
    inline void UpdateScore(float fF, float *pfScore);
    inline bool Threashold(float *pfScore);
    inline bool UpdateScoreAndThreshold(float fF, float *pfScore)
    {
#if NUM_FEATURE_TH == 7 
        int idx; 
        if (fF >= m_pfFeatureTh[3]) 
        {
            if (fF >= m_pfFeatureTh[1]) 
                idx = int(fF < m_pfFeatureTh[0]); 
            else
                idx = 2+int(fF < m_pfFeatureTh[2]);
        }
        else
        {
            if (fF >= m_pfFeatureTh[5]) 
                idx = 4+int(fF < m_pfFeatureTh[4]);
            else
                idx = 6+int(fF < m_pfFeatureTh[6]);
        }
#else
        int idx = NUM_FEATURE_TH; 
        for (int i=NUM_FEATURE_TH-1; i>=0; i--,idx--) 
            if (fF < m_pfFeatureTh[i]) 
                break; 
#endif
        
        if (m_nBranchFlag) 
        {
            float tmpScore[MAX_NUM_CATEGORIES]; 
            for (int i=0; i<m_nNumCat; i++) 
                tmpScore[i] = pfScore[i]; 
            for (int i=0; i<m_nNumCat; i++) 
                pfScore[i] = tmpScore[m_nBranchMapping[i]]; 
        }
        
        if (m_nNumCat == 1) 
        {
            pfScore[0] += m_pfDScore[0][idx]; 
            return (pfScore[0] < m_pfMinPosScoreTh[0]); 
        }
        
        int nRejCnt = 0; 
        for (int i=0; i<m_nNumCat; i++) 
        {
            if (pfScore[i] <= MIN_SCORE) 
            {
                nRejCnt ++; 
                continue; 
            }
            pfScore[i] += m_pfDScore[i][idx]; 
            if (pfScore[i] < m_pfMinPosScoreTh[i]) 
            {
                pfScore[i] = MIN_SCORE; 
                nRejCnt ++; 
            }
        }
        return (nRejCnt == m_nNumCat); 
    } 

    inline bool ThreasholdConst(float *pfScore, float fTh) 
    {
        int nRejCnt = 0; 
        for (int i=0; i<m_nNumCat; i++) 
        {
            if (pfScore[i] < fTh) 
            {
                pfScore[i] = MIN_SCORE; 
                nRejCnt ++; 
            }
        }
        return (nRejCnt == m_nNumCat); 
    }; 

    void Init(FEATURE* pFeature, FILE *file, bool bBinary);
    void Init(FEATURE* pFeature, const BYTE* buffer, int iBufLen, int& iStreamPos, bool bBinary);

    void Write(FILE *file, bool bBinary); 

    static bool ReadClassifierFileHeader(FILE *file, bool bBinary); 
    static bool ReadClassifierFileHeader(const BYTE* buffer, int iBufLen, int& iStreamPos, bool bBinary);
    static CLASSIFIER * CreateScaledClassifierArray(CLASSIFIER *classifierArray, int nClassifiers, float scale); 
    static bool WriteClassifierFile(CLASSIFIER *classifierArray, int nClassifiers, const char *fileName, bool bBinary);
    static void WriteClassifierFileHeader(FILE *file, bool bBinary); 
    static void WriteClassifierArray(CLASSIFIER *classifierArray, int nClassifiers, FILE *file, bool bBinary); 
    static void DeleteClassifierArray(CLASSIFIER *classifierArray); 
    static void CreateClassifier(int nClassifiers, 
                                  __inout_ecount(nClassifiers) CLASSIFIER* pClassifiers, 
                                  __inout_ecount(nClassifiers) FEATURE* pScaledFeatures, 
                                  FILE *file, bool bBinary);
    static void CreateClassifier(int nClassifiers, 
                                  __inout_ecount(nClassifiers) CLASSIFIER* pClassifiers, 
                                  __inout_ecount(nClassifiers) FEATURE* pScaledFeatures, 
                                  const BYTE* buffer, int iBufLen, int& iStreamPos, bool bBinary);
    static void ScaleFeatures(__in_ecount(nScaledFeatures) FEATURE* pSrcScaledFeatures, __inout_ecount(nScaledFeatures) FEATURE* pDstScaledFeatures, int nScaledFeatures, float flScaleMagnitude);
};
