
#pragma once

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Israel Innovation Labs 11-APR-10
// This post-filter is a subset of OneAlbum method for rejection false-positives in face detector
// Due to time constraints, the code was manually adapted to the specific case / code of the detector in Photo Gallery
// The algorithm works as follows:
// 1. Inputs: 
//        a. Gray scale face candidate (Output of face detection cascade (VJ))
//        b. Score of face detection cascade
//        c. Category of detected face (0 to 4, based on Cha Zhang algorithm)
// 2. Process:
//        a. Create simple LBP representation with 4*4 grid  (total 59*4*4=944 features)
//        b. Add cascade score as additional feature
//        c. Run this representation into pre-trained non-linear classifier
//        d. Compare output to threshold (>th => face)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "winTypes.h"

#include <float.h>
#include <stdio.h>

namespace ILabs
{
#define MAX_DET_GRP_CLASSIFIERS (32)

class Poly2Classifier
{
public:
    Poly2Classifier(FILE *file);
    Poly2Classifier(__in_ecount(nBufferLength) const BYTE* pbBuffer, const int nBufferLength, int& iStreamPos);
    ~Poly2Classifier();

    float Classify(__in_ecount(len) float *X, int len);

    bool IsInited() const;

private:
    void Init();
    void Release();
    bool Load(FILE *file);
    bool Load(__in_ecount(nBufferLength) const BYTE* pbBuffer, const int nBufferLength, int& iStreamPos);

    // Reads float vector represented by int16 values and the value of the least significant bit.
    // If vector != null, the data will be read into the buffer vector points to.
    // In this case vectorLength specifies the buffer size. If the actual number of
    // vector elements in the file differs from vectorLength, the function will fail.
    // 
    // If vector == null, the function will allocate the buffer. 
    // vectorLength specifies the expected number of elements. If actual number
    // of elements differs from the expected one, the function will fail. If the number
    // of elements is not known, vectorLength should be 0.
    void InitFloatVector(float* &vector, const int &vectorLength, __int16 *src, float lsb);
    bool ReadFloatVector(float* &vector, int &vectorLength, FILE* file);
    bool ReadFloatVector( const BYTE* pbBuffer, const int nBufferLength, float* &vector, int &vectorLength, int& iStreamPos);

    // Reads float matrix represented by int16 values and the value of the least significant bit.
    // Matrix size should be known.
    void InitMatrix(float** &matrix, float* &matrixBuffer, int cols, int rows, __int16* intVector, float& lsb);
    bool ReadFloatMatrix(float** &matrix, float* &matrixBuffer, int cols, int rows, FILE* file);
    bool ReadFloatMatrix( const BYTE* pbBuffer, const int nBufferLength, float** &matrix, float* &matrixBuffer, int cols, int rows, int& iStreamPos);

    // Reads in data into internal swap buffer m_pIntVector for later use in filling float arrays
    bool ReadIntVectorAndLsb( int& length, float& lsb, FILE* file);
    bool ReadIntVectorAndLsb( const BYTE* pbBuffer, const int nBufferLength, int& length, float& lsb, int& iStreamPos);

    int   m_ftrsNum;
    int   m_projNum;
    float *m_w1;     // ftrsNum vector
    float *m_wz;     // projNum vector
    float **m_p2;  // projNum*ftrsNum matrix
    float* m_p2Buffer; // buffer containing all rows of data for m_p2. m_p2 points into this buffer at each row's start
    float **m_w2;  // projNum*ftrsNum matrix
    float* m_w2Buffer; // buffer containing all rows of data for m_w2. m_w2 points into this buffer at each row's start
    float m_bias;

    // Swap buffer for reading sixteen bit ints from input data to copy into float buffers later. Only reallocate if the buffer needs to be larger than m_nIntVectorSize
    __int16* m_pIntVector;
    int m_nIntVectorSize;
    
    bool  m_bInited;
};

};
