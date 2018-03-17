#pragma once
#include <Windows.h>

#define DEVICE_NAME TEXT("\\\\.\\fc_pcie0")
#define EVENT_NAME  TEXT("Global\\FC_PCIE_IRQEvent0")

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::Threading;

typedef void (*TCBFunc)(void*);

namespace FC_drv_library 
{

	public ref class TThreadEvent //public Thread
	{
	public:
		TThreadEvent(bool Suspended)
		{
			hEvent = NULL;
			ThreadStart ^ chtrDo = (ThreadStart^)Delegate::CreateDelegate(ThreadStart::typeid, this, "Do");
			
			my_Thread = gcnew Thread(chtrDo);
			my_Thread->IsBackground = true;

			if (Suspended == false)
				my_Thread->Start();
		}
		 ~TThreadEvent(void)
		{
			 CloseHandle(hEvent);
		}

		private: void Do()
		{
		}
		
		public: void Resume()
		{
			my_Thread->Start();
		}

		public: bool  Suspended()
		{
			return my_Thread->ThreadState == ThreadState::Suspended ? true : false;
		}

		public: void  Suspend()
		{
			my_Thread->Suspend();
		}
		public: void  Terminate()
		{
			my_Thread->Abort();
		}
		public: void  WaitFor()
		{
			my_Thread->SpinWait(0);
		}
				 
		//void  __fastcall ESynhro(void);
		public: bool __fastcall SetEvent(LPCWSTR EventName)
		{
			bool status = true;

			hEvent = OpenEvent(SYNCHRONIZE, false, EventName);

			status = (hEvent == NULL) ? false : true;

			return status;
		}
		bool __fastcall SetCBFunction(TCBFunc SCBFunc, void *SContext)
		{
			bool  status = true;
			
			if (SCBFunc != NULL) 
			{
				CBFunc = SCBFunc;
				ContextCBFunc = SContext;
			}
			else 
				status = false;

			return status;
		}
		void  ESynhro(void)
		{
			CBFunc(ContextCBFunc);
		}
		void Execute(void)
		{
			while (1/*my_Thread->ThreadState == Terminated*/) {
				if (WaitForSingleObject(hEvent, 2000) != WAIT_TIMEOUT) {
					//Synchronize((TThreadMethod)&ESynhro);
					ESynhro();
				}
			}
		}
	private:
		Thread^ my_Thread;
		HANDLE	hEvent;
		TCBFunc			CBFunc;
		void			*ContextCBFunc;
	};


	/// <summary>
	/// Summary for UTIRQEventCLR
	/// </summary>
	public ref class TIRQEvent 
	{
#pragma region Работа с прерыванием
	private: 	bool	EventIsSet = false;
	private:	bool	CBFuncIsSet;
	private:	HANDLE	hEvent;
	private:	TThreadEvent ^ ThreadEvent;
#pragma endregion

	public:
		TIRQEvent(void)
		{
			ThreadEvent = gcnew TThreadEvent(true);
			EventIsSet = false;
			CBFuncIsSet = false;
		}

	public: bool ConnectToEvent(LPCWSTR EventName)
	{
		EventIsSet = ThreadEvent->SetEvent(EventName);
		return EventIsSet;

	}
	public: bool Stop(void)
	{
		bool status = true;
		if (ThreadEvent->Suspended()) 
			status = false;
		else 
			ThreadEvent->Suspend();
		return status;
	}
	public: bool __fastcall Start(void)
	{
		bool status = true;
		if (EventIsSet & CBFuncIsSet) 
			ThreadEvent->Resume();
		else 
			status = false;
		
		return status;
	}
	public: bool SetCBFunction(TCBFunc SCBFunc, void *SContext)
	{
		CBFuncIsSet = ThreadEvent->SetCBFunction(SCBFunc, SContext);
		return CBFuncIsSet;
	}
	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~TIRQEvent()
		{
			ThreadEvent->Resume();
			ThreadEvent->Terminate();
			ThreadEvent->WaitFor();
			//Sleep(500);
			delete ThreadEvent;
		}

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
	};
}
