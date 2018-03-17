//////////////////////////////////////////////////////////////////
//
// FC_moduleRtTimerThread.cpp - cpp file
//
// This file was generated using the RTX64 Application Template for Visual Studio.
//
// Created: 12/18/2017 9:44:27 AM 
// User: Tsimbalyuk.E.I
//
//////////////////////////////////////////////////////////////////

#include "FC_module.h"
#include "FC_command_library.h"
#include <iostream>
#include <fstream>
using namespace std;

extern byte current_channel;

extern string name_of_file;	//имя файла для записи
extern UINT64 number_of_string;


// RTX64 periodic timer handler function.  Refer to the RTX64 Samples directory for examples of
// periodic timers.
//

extern FC_Command FC_managed;
ReceivedBuffer receivedBuffer;
ReceivedBuffer sendBuffer;
bool resultOfSend = false;
bool is_conected	= false;
bool is_conected1	= false;


void write_to_file(UINT32 R_CTL, UINT32 D_ID, UINT32 CS_CTL, UINT32 S_ID, UINT32 TYPE, UINT32 F_CTL, UINT32 Seq_ID, UINT32 DF_CTL, UINT32 Seq_CNT, UINT32 OX_ID, UINT32 RX_ID, UINT32 Param, UINT32 message_id, UINT32 w2, UINT32 w3, UINT32 count_word, UINT32 * data, UINT32 _SOF, UINT32 _EOF)
{

	ofstream fout(name_of_file, std::fstream::in | std::fstream::out | std::fstream::app);

	fout << "|" << "\t" << number_of_string << "\t" << "|" << "\t" << "0x" << hex << R_CTL << "\t" << "|" << "\t" << "0x"<< hex << D_ID << "\t" << "|" << "\t" << "0x" << hex << CS_CTL << "\t" << "|" << "\t" << "0x" << hex << S_ID << "\t" << "|" << "\t" << "0x" << hex << TYPE << "\t" << "|" << "\t" << "0x" << hex << F_CTL << "\t" << "|" << "\t" << "0x" << hex << Seq_ID << "\t" << "|" << "\t" << "0x" << hex << DF_CTL << "\t" << "|" << "\t" << "0x" << hex << Seq_CNT << "\t" << "|" << "\t" << "0x" << hex << OX_ID << "\t" << "|" << "\t" << "0x" << hex << RX_ID << "\t" << "|" << "\t" << "0x" << hex << Param << "\t" << "|" << "\t" << message_id << "\t" << "|" << "\t" << "0x" << hex << w2 << "\t" << "|" << "\t" << "0x" << hex << w3 << "\t" << "|" << "\t" << count_word << "\t" << "|" << "\t" << _SOF << "\t" << "|" << "\t" << _EOF << "\t" << "|" ;
	
	for (UINT32 i = 0; i < (count_word / 4); i++)
	{
		fout << "\t" << "0x" << hex << *(data + i) << "\t" << "|";
	}
	 
	fout << "\n";

	number_of_string++;
	fout.close();
}
#ifdef UNDER_RTSS
void
RTFCNDCL
TimerHandler_connect(PVOID context)
#else
bool TimerHandler_connect()
#endif
{
	if ((is_conected1 == false) && (is_conected == false))
	{
		
		FC_managed.FC_UpdateState(current_channel);	
		
		is_conected = FC_managed.Connect_ch_command(current_channel);
		is_conected1 = FC_managed.FC_UpdateState(current_channel);
		
		if (is_conected == false)
		{
			printf("Channel %i disconnected \n", current_channel);
         
		}	
		if (is_conected1 == false)
		{
			printf("Channel %i disconnected 1 \n", current_channel);
		}
      if (is_conected == false && is_conected1 == false)
      {
         return false;
      }
      else
      {
         return true;
      }
	}
    // TO DO:
    // Your timer handler code here.
}
#ifdef UNDER_RTSS
void
RTFCNDCL
TimerHandler_receive(PVOID context)
#else
void TimerHandler_receive()
#endif
{
	
	receivedBuffer = FC_managed.receiveMessage(current_channel);
	if (receivedBuffer.buffer != NULL)
	{
		printf("Received data: %x\n", *(receivedBuffer.buffer + 0x17));

		write_to_file((*(receivedBuffer.buffer + 13)) >> 24, (*(receivedBuffer.buffer + 13) & 0xffffff), (*(receivedBuffer.buffer + 14)) >> 24, (*(receivedBuffer.buffer + 14) & 0xffffff), (*(receivedBuffer.buffer + 15)) >> 24, (*(receivedBuffer.buffer + 15) & 0xffffff), (*(receivedBuffer.buffer + 16)) >> 24, ((*(receivedBuffer.buffer + 16)) >> 16) & 0xFF, (*(receivedBuffer.buffer + 16)) >> 16, (*(receivedBuffer.buffer + 17)) >> 16, (*(receivedBuffer.buffer + 17)) & 0xFFFF, (*(receivedBuffer.buffer + 18)) , (*(receivedBuffer.buffer + 19)), (*(receivedBuffer.buffer + 20)), (*(receivedBuffer.buffer + 21)), (*(receivedBuffer.buffer + 22)), (receivedBuffer.buffer + 23), receivedBuffer._SOF, receivedBuffer._EOF);
	}
	
	// TO DO:
	// Your timer handler code here.
}

