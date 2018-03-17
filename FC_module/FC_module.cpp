//////////////////////////////////////////////////////////////////
//
// FC_module.cpp - cpp file
//
// This file was generated using the RTX64 Application Template for Visual Studio.
//
// Created: 12/18/2017 9:44:27 AM 
// User: Tsimbalyuk.E.I
//
//////////////////////////////////////////////////////////////////

#include "FC_module.h"
#include "FC_command_library.h"
#include <ctime>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

ULONG sysmem[0x400000 * 4];
ULONG *b = sysmem;
UINT32 myBuff[2097152];

string name_of_file;	//имя файла для записи
UINT64 number_of_string = 0;

byte current_channel = 0;	//установка канала, с которым происходит работа
FC_Command FC_managed;

template <typename T> string NumberToString(T Number)
{
	ostringstream ss;
	ss << Number;
	return ss.str();
}

int _tmain(int argc, _TCHAR * argv[])
{	
	time_t my_time = time(0);
	struct tm * now = localtime(&my_time);
	name_of_file = "C:\\programming\\Excel\\test_" + NumberToString(now->tm_mday) + "_" + NumberToString(now->tm_mon + 1) + "_" + NumberToString(now->tm_year - 100) + "__" + NumberToString(now->tm_hour) + "_" + NumberToString(now->tm_min) + "_" + NumberToString(now->tm_sec);
	name_of_file += ".txt";	

	ofstream fout(name_of_file, std::fstream::in | std::fstream::out | std::fstream::app);
	fout << "Filename : " << name_of_file <<"\n";
	fout << "Date : " << "filename" << "\n";
	fout << "\n";
	int k = 11;
	string test = NumberToString(k);//((char)11);

	fout << "|" << "\t" << "id" << "\t" << "|" << "\t" << "R_CTL" << "\t" << "|" << "\t" << "D_ID" << "\t" << "|" << "\t" << "CS_CTL" << "\t" << "|" << "\t" << "S_ID"  << "\t" << "|" << "\t" << "TYPE" << "\t"  << "|" << "\t" << "F_CTL" << "\t" << "|" << "\t" << "Seq_ID" << "\t" << "|" << "\t" << "DF_CTL" << "\t" << "|" << "\t" << "Seq_CNT" << "\t" << "|" << "\t" << "OX_ID" << "\t" << "|" << "\t" << "RX_ID" << "\t" << "|" << "\t" << "Param" << "\t" << "|" << "\t" << "massege_id" << "\t" << "|" << "\t" << "2st" << "\t" << "|" << "\t" << "3rd" << "\t" << "|" << "\t" << "count_word" << "\t" << "|" << "\t" << "SOF" << "\t" << "|" << "\t" << "EOF" << "\t" << "|" << "\t" << "data" << "\t" << "|" << "\n";
	
	number_of_string++;

	fout.close();

#pragma region Заголовок кадра FC FS 2
	FC_managed.FC_FS2_Header.R_CTL = 0x0;
	FC_managed.FC_FS2_Header.D_ID = 0x10100;
	FC_managed.FC_FS2_Header.CS_CTL = 0x0;
	FC_managed.FC_FS2_Header.S_ID = 0x10500;
	FC_managed.FC_FS2_Header.TYPE = 0x49;
	FC_managed.FC_FS2_Header.F_CTL = 0x0a0018;
	FC_managed.FC_FS2_Header.SEQ_ID = 0x0; //Порядковый номер
	FC_managed.FC_FS2_Header.DF_CTL = 0x0;
	FC_managed.FC_FS2_Header.SEQ_CNT = 0x0;
	FC_managed.FC_FS2_Header.OX_ID = 0x2222;
	FC_managed.FC_FS2_Header.RX_ID = 0x3333;
	FC_managed.FC_FS2_Header.Parameter = 0x0;
	FC_managed.FC_FS2_Header.message_id = 0x0;
	FC_managed.FC_FS2_Header.word2 = 0x0;
	FC_managed.FC_FS2_Header.word3 = 0x0;
	FC_managed.FC_FS2_Header.message_size = 0x0;
#pragma endregion

    // for periodic timer code
    LARGE_INTEGER  liPeriod_connect;     // timer period
    HANDLE         hTimer_connect;       // timer handle
	LARGE_INTEGER  liPeriod_receive;     // timer period
	HANDLE         hTimer_receive;       // timer handle
	//FC_Command FC_managed;// = new FC_Command();
   
    //  RTX64 periodic timer code:
    //  TO DO: Set default timer period to your desired time.
    //         The period needs to be an even multiple of the HAL
    //         period found in the control panel.
    //         This example uses a period of 500 micro seconds.

	liPeriod_connect.QuadPart = 10000000;//5000;	Период 1 сек - 1Гц
	liPeriod_receive.QuadPart = 1000000;//5000;		Период 0,1 сек - 10Гц

#ifndef UNDER_RTSS
	while (!TimerHandler_connect())
	{
		Sleep(10);
	}

	while (true)
	{
	   TimerHandler_receive();
	}
#else
   // // Create a periodic timer
    if (!(hTimer_connect = RtCreateTimer(NULL,            // Ignored
                                 0,               // Stack size (0 means use default size)
                                 TimerHandler_connect,    // Timer handler
                                 NULL,            // Context argument to pass to handler
                                 RT_PRIORITY_MAX, // Timer thread priority
                                 CLOCK_2)))       // RTX64 HAL timer
    {
        //
        // TO DO:  exception code here
        // RtWprintf(_T("RtCreateTimer error = %d\n"),GetLastError());
        // NOTE: If this is changed to ExitProcess(), C++ static destructors in the main
        // module and all atexit-registered callbacks will not be called.
        exit(1);
    }
   
	//// Create a periodic timer
	if (!(hTimer_receive = RtCreateTimer(NULL,            // Ignored
		0,               // Stack size (0 means use default size)
		TimerHandler_receive,    // Timer handler
		NULL,            // Context argument to pass to handler
		RT_PRIORITY_MAX, // Timer thread priority
		CLOCK_2)))       // RTX64 HAL timer
	{
		//
		// TO DO:  exception code here
		// RtWprintf(_T("RtCreateTimer error = %d\n"),GetLastError());
		// NOTE: If this is changed to ExitProcess(), C++ static destructors in the main
		// module and all atexit-registered callbacks will not be called.
		exit(1);
	}
   
    if (!RtSetTimerRelative(hTimer_connect,
                            &liPeriod_connect,
                            &liPeriod_connect))
    {
        //
        // TO DO: exception code here
        // RtWprintf(_T("RtSetTimerRelative error = %d\n"),GetLastError());
        // NOTE: If this is changed to ExitProcess(), C++ static destructors in the main
        // module and all atexit-registered callbacks will not be called.
        exit(1);
    }
   
	if (!RtSetTimerRelative(hTimer_receive,
		&liPeriod_receive,
		&liPeriod_receive))
	{
		//
		// TO DO: exception code here
		// RtWprintf(_T("RtSetTimerRelative error = %d\n"),GetLastError());
		// NOTE: If this is changed to ExitProcess(), C++ static destructors in the main
		// module and all atexit-registered callbacks will not be called.
		exit(1);
	}
   
   
   
   
	
	
	// for periodic timer code
	if (!RtDeleteTimer(hTimer_connect))
	{
	    // RtWprintf(_T("RtDeleteTimer error = %d\n"),GetLastError());
	    // TO DO:  your exception code here
		    // NOTE: If this is changed to ExitProcess(), C++ static destructors in the main
	    // module and all atexit-registered callbacks will not be called.
	    exit(1);
	}
	// for periodic timer code
	if (!RtDeleteTimer(hTimer_receive))
	{
		// RtWprintf(_T("RtDeleteTimer error = %d\n"),GetLastError());
		// TO DO:  your exception code here
		// NOTE: If this is changed to ExitProcess(), C++ static destructors in the main
		// module and all atexit-registered callbacks will not be called.
		exit(1);
	}
#endif
    return 0;
}
