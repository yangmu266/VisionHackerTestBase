
#pragma once

#include <iostream>
#include "windows.h"
#include <ASSERT.h>
#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <math.h>
#include <float.h>
#include "globalpara.h"

#ifndef ASSERT
#ifdef _DEBUG
#define ASSERT( x ) assert( x )
#else
#define ASSERT( x )
#endif
#endif

// Scope if's in macro's so the don't silently pair with subsequent elses
/*
#define VERIFY_OR_THROW_ERROR(__expr__, __error__) {if (!(__expr__)) { ASSERT(false); throw __error__; }}
#define VERIFY_OR_THROW(__expr__) {if (!(__expr__)) {ASSERT(false); throw E_UNEXPECTED;}}
#define VERIFY_NEW_OR_THROW(__ptr__) {if ((__ptr__) == NULL) {ASSERT(false); throw E_OUTOFMEMORY;}}
#define VERIFY_PTR_OR_THROW(__ptr__) {if ((__ptr__) == NULL) {ASSERT(false); throw E_POINTER;}}

*/ 
 #define VERIFY_OR_THROW_ERROR(__expr__, __error__) {if (!(__expr__)) { std::cerr<<__error__<<std::endl; ASSERT(false);  }}
 #define VERIFY_OR_THROW(__expr__) {if (!(__expr__)) {ASSERT(false); }}
 #define VERIFY_NEW_OR_THROW(__ptr__) {if ((__ptr__) == NULL) {ASSERT(false); }}
 #define VERIFY_PTR_OR_THROW(__ptr__) {if ((__ptr__) == NULL) {ASSERT(false); }}
 