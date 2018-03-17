#pragma once
#include <Windows.h>
#include <cstdio>
#include "FC_drv_library.h"

	extern ULONG sysmem[0x400000 * 4];
	extern ULONG *b;// = sysmem;
	extern UINT32 myBuff[2097152];	

	const int REMOVAL[8] = { 
		0x00000000 /* RX0_B0 */, 
		0x00400000 /* TX0_B0 */, 
		0x00200000 /* RX0_B1 */, 
		0x00600000 /* TX0_B1 */, 
		0x00800000 /* RX1_B0 */, 
		0x00C00000 /* TX1_B0 */, 
		0x00A00000 /* RX1_B1 */, 
		0x00E00000  /* TX1_B1 */ 
	};
	struct ReceivedBuffer
	{	
		UINT32 * buffer;
		size_t length;
		UINT32 _SOF;
		UINT32 _EOF;
	};		
		
	class FC_Command 
	{

#pragma region Коды ошибок
	static	const int LinkStatus_REG_ADDR = 0x014; // Link Status 0x014

	public:	struct err {
				int iErr;
				char* pchrErr;
			};
	public:	rData rdd;

	public: static	const int SOFC1_CODE = 0xBCB51717; // SOF Connect Class 1
	public: static	const int SOFI1_CODE = 0xBCB55757; // SOF Initiate Class 1
	public: static	const int SOFN1_CODE = 0xBCB53737; // SOF Normal Class 1
	public: static	const int SOFI2_CODE = 0xBCB55555; // SOF Initiate Class 2
	public: static	const int SOFN2_CODE = 0xBCB53535; // SOF Normal Class 2
	public: static	const int SOFI3_CODE = 0xBCB55656; // SOF Initiate Class 3
	public: static	const int SOFN3_CODE = 0xBCB53636; // SOF Normal Class 3
	public: static	const int SOFC4_CODE = 0xBCB51919; // SOF Activate Class 4
	public: static	const int SOFI4_CODE = 0xBCB55959; // SOF Initiate Class 4
	public: static	const int SOFN4_CODE = 0xBCB53939; // SOF Normal Class 4
	public: static	const int SOFF_CODE = 0xBCB55858; // SOF Fabric

	public: static	const int EOFT_CODEN = 0xBC957575; // EOF Terminate N
	public: static	const int EOFT_CODEP = 0xBCB57575; // EOF Terminate P	
	public: static	const int EOFDT_CODEN = 0xBC959595; // EOF Disconnect-Terminate Class 1	
	public: static	const int EOFDT_CODEP = 0xBCB59595; // EOF Deactivate-Terminate Class 4	
	public: static	const int EOFA_CODEN = 0xBC95F5F5; // EOF Abort N	
	public: static	const int EOFA_CODEP = 0xBCB5F5F5; // EOF Abort P
	public: static	const int EOFN_CODEN = 0xBC95D5D5; // EOF Normal N
											   //const EOFN_CODEN   =     0xBC95D6D6; // EOF Normal N
	public: static const int EOFN_CODEP = 0xBCB5D5D5; // EOF Normal P
	public: static	const int EOFNI_CODEN = 0xBC8AD5D5; // EOF Normal Invalid N	
	public: static	const int EOFNI_CODEP = 0xBCAAD5D5; // EOF Normal Invalid P	
	public: static	const int EOFDTI_CODEN = 0xBC8A9595; // EOF Disconnect-Terminate-Invalid  Class 1	
	public: static	const int EOFDTI_CODEP = 0xBCAA9595; // EOF Deactivate-Terminate-Invalid  Class 4
	public: static	const int EOFRT_CODEN = 0xBC959999; // EOF Remove-Terminate Class 4 N	
	public: static	const int EOFRT_CODEP = 0xBCB59999; // EOF Remove-Terminate Class 4 P	
	public: static	const int EOFRTI_CODEN = 0xBC8A9999; // EOF Remove-Terminate-Invalid Class 4 N	
	public: static	const int EOFRTI_CODEP = 0xBCAA9999; // EOF Remove-Terminate-Invalid Class 4 P

	public: err Errors[7] = 
	{ 
		{ FC_ERR_NOERR, "Нет ошибки" },
		{ FC_ERR_NOT_INIT, "API FC_LIB не инициализировано" },
		{ FC_ERR_ALRD_INIT, "API FC_LIB уже инициализировано" },
		{ FC_ERR_DRV_NOT_OPENED, "Драйвер не открыт" },
		{ FC_ERR_INV_PAR, "Неверный входной параметр" },
		{ FC_ERR_DRV, "Ошибка исполнения команды драйвером" },
		{ FC_ERR_EVENT, "Не удалось запустить Event" },
	};
#pragma endregion


		private:	HANDLE	hdrv;
		public:		INT32	iLastError;


		public: ReceivedBuffer receivedBuffer;		

#pragma region Статус инициализации платы
		public: bool bFC_init_state;
		

		public: char* sFC_init_state;
		
#pragma endregion
		public: char* sLastError;	
		private: int countTXframe;
		private: int countRXframe;

		public: UINT32 _SOF;
		public: UINT32 _EOF;

		public: FC_Command()
		{
			hdrv = INVALID_HANDLE_VALUE;
			iLastError = 0;
			bFC_init_state = false;
			sFC_init_state = bFC_init_state ? "Инициализация платы пройдена успешно" : "Инициализация платы не пройдена";
			sLastError = FC_GetDescError(iLastError);		
			countTXframe = 0;
			countRXframe = 0;
			_SOF = SOFI3_CODE;
			_EOF = EOFDT_CODEN;
			bFC_init_state = FC_Init();
			FC_WriteAdr_master(0);
			FC_Get_Adr(&b);
        
		}

	

#pragma region Заголовок кадра FC FS 2
		union
		{
			UINT32 m_uint_32_FC_FS_2[10];
			struct
			{
				UINT32 R_CTL	:	8;
				UINT32 D_ID		:	24;
				UINT32 CS_CTL	:	8;
				UINT32 S_ID		:	24;
				UINT32 TYPE		:	8;
				UINT32 F_CTL	:	24;
				UINT32 SEQ_ID	:	8;
				UINT32 DF_CTL	:	8;
				UINT32 SEQ_CNT	:	16;
				UINT32 OX_ID	:	16;
				UINT32 RX_ID	:	16;
				UINT32 Parameter :	32;
				UINT32 message_id	: 32;	//Идентификатор сообщения доп.заголовка
				UINT32 word2		: 32;	//0x0 доп.заголовка
				UINT32 word3		: 32;	//0x0 доп.заголовка
				UINT32 message_size : 32;	//Размер сообщения доп.заголовка
			}FC_FS2_Header;
		};

#pragma endregion
		//**********************************************************************************
		//Опрос состояния FC канала
		//Вход:
		//		Номер канала		
		//Выход:
		//		Состояние канала 
		//**********************************************************************************
		
#pragma region Опрос состояния канала
		public: bool FC_UpdateState(unsigned int ch)
		{
			FC_SET_RAM_TX_RX(ch << 2);
			FC_WriteAdr_Fc_Manage(LinkStatus_REG_ADDR);
			Sleep(10);
			rdd = FC_ReadData_Fc_Manage();
			
			if (((rdd.rD & 0x001F0000) >> 16) != 0x00000001)
				return false;
			else
				return true;
		}
#pragma endregion

		//**********************************************************************************
		//Установка соединения FC канала
		//Вход:
		//		Номер канала		
		//Выход:
		//		Результат установления соединения 
		//**********************************************************************************
		public: bool Connect_ch_command(unsigned int ch)
		{
			return Connect_ch(ch);
		}		
		
		//**********************************************************************************
		//Отправка сообщение по FC каналу
		//Вход:
		//		Номер канала		
		//		ReceivedBuffer::buffer - выходной буффер				(NULL, если буфер пуст)
		//		ReceivedBuffer::length - размер выходного буффера		(NULL, если буфер пуст)
		//Выход:
		//		Результат отправки
		//**********************************************************************************
#pragma region Отправка сообщения
		public: bool sendMessage(int channel, ReceivedBuffer sendBuffer)
		{
			countTXframe = FC_Read_Count_Frame_TX().rD;
			int REG_RAM_TX_RX = 1;			
			REG_RAM_TX_RX = (REG_RAM_TX_RX & 0xFFFFFFFB) | (channel << 2);
			int Param = 0x800000;
			FC_WriteParameters(Param);
			memset(myBuff, 0x0, sizeof(myBuff));
			FC_SET_RAM_TX_RX(REG_RAM_TX_RX);

			rData k = FC_Read_RAM_TX_RX();

			myBuff[0] = 14 + sendBuffer.length;
			myBuff[1] = 0x40;
			myBuff[2] = myBuff[0] - 3;
			myBuff[3] = _SOF; 
			myBuff[4] = _EOF;
			myBuff[5] =	m_uint_32_FC_FS_2[0];
			myBuff[6] =	m_uint_32_FC_FS_2[1];
			myBuff[7] =	m_uint_32_FC_FS_2[2];
			myBuff[8] =	m_uint_32_FC_FS_2[3];
			myBuff[9] =	m_uint_32_FC_FS_2[4];
			myBuff[10] = m_uint_32_FC_FS_2[5];			
			myBuff[11] = m_uint_32_FC_FS_2[6];
			myBuff[12] = m_uint_32_FC_FS_2[7];
			myBuff[13] = m_uint_32_FC_FS_2[8];
			myBuff[14] = m_uint_32_FC_FS_2[9];			
			for (size_t i = 0; i < sendBuffer.length; i++)
			{
				myBuff[15 + i] = sendBuffer.buffer[i];
			}

			FC_WriteBlockMem(myBuff);
			FC_Start_Fc_Tx(1);
			if ((countTXframe + 1) == FC_Read_Count_Frame_TX().rD)
				return true;
			else
				return false;
		}

#pragma endregion				
			
		
		//**********************************************************************************
		//Получение сообщение по FC каналу
		//Вход:
		//		Номер канала		
		//Выход:
		//		ReceivedBuffer::buffer - входной буффер				(NULL, если буфер пуст)
		//		ReceivedBuffer::length - размер входного буффера	(NULL, если буфер пуст)
		//**********************************************************************************
#pragma region Получение сообщения
		
		public: ReceivedBuffer receiveMessage(int channel)
		{
			receivedBuffer.buffer = NULL;
			receivedBuffer.length = NULL;
			if (countRXframe != FC_Read_Count_Frame_RX().rD)
			{
				delete[] receivedBuffer.buffer;// = NULL;
				

				countRXframe = FC_Read_Count_Frame_RX().rD;
				FC_WriteStatus(1);
				int REG_RAM_TX_RX = 0;
				REG_RAM_TX_RX = (REG_RAM_TX_RX & 0xFFFFFFFB) | (channel << 2);
				int Param = 0x0;
				FC_WriteParameters(Param);
				memset(myBuff, 0x0, sizeof(myBuff));
				FC_SET_RAM_TX_RX(REG_RAM_TX_RX);


				int num_word = 1010;
				num_word = b[0 + REMOVAL[REG_RAM_TX_RX]] + 12;

				memset(myBuff, 0x0, sizeof(myBuff));

				for (int i = 0; i < num_word; i++)
				{
					myBuff[i] = b[i + REMOVAL[REG_RAM_TX_RX]];
				}

				FC_WriteStatus(0);				
				receivedBuffer.length = num_word;
				receivedBuffer.buffer = new UINT32[receivedBuffer.length]; 
				memcpy_s(receivedBuffer.buffer, receivedBuffer.length * sizeof(receivedBuffer.buffer), &myBuff[0], receivedBuffer.length * sizeof(receivedBuffer.buffer));
				receivedBuffer._SOF = myBuff[0xC];
				receivedBuffer._EOF = myBuff[0x16 + (myBuff[0x16] / 4) + 2];				
			}
			
			return receivedBuffer;
		}

		//**********************************************************************************
		//Подсчет количества 1 битов в 32-битном слове
		//Вход:
		//		32-битной слово	
		//Выход:
		//		Количесто единичных битов в 32-битном слове
		//**********************************************************************************
		private: UINT32 bitCounter(UINT32 n)
		{
			UINT32 counter = 0;
			while (n != 0)
			{
				counter += n & 0x1;
				n = n >> 1;
			}
			return counter;
		}
		
#pragma endregion

	};
