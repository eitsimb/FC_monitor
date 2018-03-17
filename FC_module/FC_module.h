//////////////////////////////////////////////////////////////////
//
// FC_module.h - header file
//
// This file was generated using the RTX64 Application Template for Visual Studio.
//
// Created: 12/18/2017 9:44:27 AM 
// User: Tsimbalyuk.E.I
//
//////////////////////////////////////////////////////////////////

#pragma once
//This define will deprecate all unsupported Microsoft C-runtime functions when compiled under RTSS.
//If using this define, #include <rtapi.h> should remain below all windows headers
//#define UNDER_RTSS_UNSUPPORTED_CRT_APIS

#include <SDKDDKVer.h>

#include <windows.h>
#include <tchar.h>


#ifdef UNDER_RTSS
#include <rtapi.h>    // RTX64 APIs that can be used in real-time and Windows applications.
#include <rtssapi.h>  // RTX64 APIs that can only be used in real-time applications.
#endif // UNDER_RTSS


 
// function prototype for periodic timer function
#ifdef UNDER_RTSS
void
RTFCNDCL
TimerHandler_connect(void * nContext);

void
RTFCNDCL
TimerHandler_receive(void * nContext);
#else 
   bool  TimerHandler_connect();
   void  TimerHandler_receive();
#endif
 
