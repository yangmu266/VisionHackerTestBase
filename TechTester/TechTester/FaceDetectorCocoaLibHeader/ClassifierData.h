
#pragma once

// Pre-defined array sizes to help the compiler infer array structure
// in multidimensional arrays

#define NUM_CLASSIFIERS 700
#define NUM_THRESHOLDS 7
#define NUM_DSCORES 8
#define MAX_NUM_RECTS 4
#define RECTS_DIMENSIONS 4

#define __declspec 
/*
extern __declspec(selectany) const int knBaseWidth = 24;
extern __declspec(selectany) const int knBaseHeight = 24;

extern __declspec(selectany) const float kflFinalThreshold = -1.000000;

extern __declspec(selectany) const int knNumClassifiers = 700;
extern __declspec(selectany) const int knNumThresholds = 7;
 */
const int knBaseWidth = 24;
const int knBaseHeight = 24;

const float kflFinalThreshold = -1.000000;

const int knNumClassifiers = 700;
const int knNumThresholds = 7;



  






