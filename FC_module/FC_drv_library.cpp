#include "FC_drv_library.h"
#include <Windows.h>
#include "fc_ioctl.h"


extern bool Flag_VideoTest;
extern int video_removal;


HANDLE hdrv = INVALID_HANDLE_VALUE;

int iLastError;
int iMode;

int obuf[2097152];

HANDLE hlink;

ULONG ulAddress;


struct err{
	int iErr;
	char* pchrErr;
};
rData rdd;
#pragma region Коды ошибок

	const int val_magenta_magenta = 0x36003600;
	const int val_white_wh_bl = 0xFFFFFF00;
	const int val_wh_bl_white = 0xFF00FFFF;
	const int val_white_bl_wh = 0xFFFF00FF;
	const int val_bl_wh_white = 0x00FFFFFF;
	const int val_black_black = 0x00000000;
	const int val_white_red = 0xFFFFF800;
	const int val_red_red = 0xF800F800;
	const int val_red_white = 0xF800FFFF;
	const int val_blue_red = 0x001FF800;
	const int val_red_green = 0xF80007E0;
	const int val_red_blue = 0xF800001F;
	const int val_green_green = 0x07E007E0;

	const int val_blue_blue = 0x001F001F;
	const int val_blue_black = 0x001F0000;
	const int val_black_blue = 0x0000001F;
	const int val_blue_white = 0x001FFFFF;
	const int val_white_blue = 0xFFFF001F;

	// FC Core register addresses

	const int OpticsControl_REG_ADDR = 0x000; // Optics Control 0x000
	const int OpticsStatus_REG_ADDR = 0x004; // Optics Status 0x004
	const int PSMControl_REG_ADDR = 0x020; // PSM Control 0x020
	const int PSMStatus_REG_ADDR = 0x024; // PSM Status 0x024
	const int LinkControl_REG_ADDR = 0x010; // Link Control 0x010
	const int LinkStatus_REG_ADDR = 0x014; // Link Status 0x014
	const int Timeout_REG_ADDR = 0x02C; // Timeout reg 0x02C
	const int Credit_REG_ADDR = 0x030; // Credit reg 0x030
	const int OutStdRRDY_REG_ADDR = 0x038; // Outstanding RRDY reg 0x038

												   // FC Core Statistic register addresses
												   // Receive Link Error Statistics

	const int RxLinkFailure_STAT_ADDR = 0x080; // Rx Link Failure 0x080
	const int RxLossSync_STAT_ADDR = 0x084; // Rx Loss of Synchronization 0x084
	const int RxLossSig_STAT_ADDR = 0x088; // Rx Loss of Signal 0x088
	const int Rx10bErr_STAT_ADDR = 0x08C; // Rx Loss of Signal 0x08C
	const int RxAlignErr_STAT_ADD = 0x090; // Rx Alignment Error 0x090
	const int RxOsdErr_STAT_ADDR = 0x094; // Rx Ordered Set Disparity Error 0x094
	const int RxEOFa_STAT_ADDR = 0x098; // Rx EOF Abort 0x098
	const int RxEOFErr_STAT_ADDR = 0x09C; // Rx EOF Error 0x09C
	const int RxCRCErr_STAT_ADDR = 0x0A0; // Rx CRC Error 0x0A0

										  // Transmit Link Error Statistics

	const int TxEOFa_STAT_ADDR = 0x0A4; // Tx EOF Abort 0x0A4
	const int TxCRCErr_STAT_ADDR = 0x0A8; // Tx CRC Error 0x0A8
	const int TxParityErr_STAT_ADDR = 0x0AC; // Tx Parity Error 0x0AC
	const int TxEOFni_STAT_ADDR = 0x0B0; // Tx EOFni  0x0B0

										 // Receive Error Statistics

	const int RxMinFrameErr_STAT_ADDR = 0x0B4; // Rx Min Frame Length Error 0x0B4
	const int RxMaxFrameErr_STAT_ADDR = 0x0B8; // Rx Max Frame Length Error 0x0B8
	const int RxFrameCnt_STAT_ADDR = 0x0BC; // Rx Frame Count 0x0BC
	const int RxWordCnt_STAT_ADDR = 0x0C0; // Rx Word Count 0x0C0
	const int RxPSMErr_STAT_ADDR = 0x0C4; // Rx PSM Error Count 0x0C4

										  // Transmit Error Statistics
	const int TxFrameCnt_STAT_ADDR = 0x0C8; // Tx Frame Count 0x0C8
	const int TxWordCnt_STAT_ADDR = 0x0CC; // Tx Word Count 0x0CC
	const int TxKErr_STAT_ADDR = 0x0D0; // Tx K Error Count 0x0D0

	const int SOFC1_CODE = 0xBCB51717; // SOF Connect Class 1
	const int SOFI1_CODE = 0xBCB55757; // SOF Initiate Class 1
	const int SOFN1_CODE = 0xBCB53737; // SOF Normal Class 1
	const int SOFI2_CODE = 0xBCB55555; // SOF Initiate Class 2
	const int SOFN2_CODE = 0xBCB53535; // SOF Normal Class 2
	const int SOFI3_CODE = 0xBCB55656; // SOF Initiate Class 3
	const int SOFN3_CODE = 0xBCB53636; // SOF Normal Class 3
	const int SOFC4_CODE = 0xBCB51919; // SOF Activate Class 4
	const int SOFI4_CODE = 0xBCB55959; // SOF Initiate Class 4
	const int SOFN4_CODE = 0xBCB53939; // SOF Normal Class 4
	const int SOFF_CODE = 0xBCB55858; // SOF Fabric

	const int EOFT_CODEN = 0xBC957575; // EOF Terminate N
	const int EOFT_CODEP = 0xBCB57575; // EOF Terminate P	
	const int EOFDT_CODEN = 0xBC959595; // EOF Disconnect-Terminate Class 1	
	const int EOFDT_CODEP = 0xBCB59595; // EOF Deactivate-Terminate Class 4	
	const int EOFA_CODEN = 0xBC95F5F5; // EOF Abort N	
	const int EOFA_CODEP = 0xBCB5F5F5; // EOF Abort P
	const int EOFN_CODEN = 0xBC95D5D5; // EOF Normal N
									   //const EOFN_CODEN   =     0xBC95D6D6; // EOF Normal N
	const int EOFN_CODEP = 0xBCB5D5D5; // EOF Normal P
	const int EOFNI_CODEN = 0xBC8AD5D5; // EOF Normal Invalid N	
	const int EOFNI_CODEP = 0xBCAAD5D5; // EOF Normal Invalid P	
	const int EOFDTI_CODEN = 0xBC8A9595; // EOF Disconnect-Terminate-Invalid  Class 1	
	const int EOFDTI_CODEP = 0xBCAA9595; // EOF Deactivate-Terminate-Invalid  Class 4
	const int EOFRT_CODEN = 0xBC959999; // EOF Remove-Terminate Class 4 N	
	const int EOFRT_CODEP = 0xBCB59999; // EOF Remove-Terminate Class 4 P	
	const int EOFRTI_CODEN = 0xBC8A9999; // EOF Remove-Terminate-Invalid Class 4 N	
	const int EOFRTI_CODEP = 0xBCAA9999; // EOF Remove-Terminate-Invalid Class 4 P

	const int REMOVAL_128 = 0x00000080;
	const int REMOVAL_256 = 0x00000100;
	const int REMOVAL_512 = 0x00000200;
	const int REMOVAL_1024 = 0x00000400;
	const int REMOVAL_RX0_B0 = 0x00000000;
	const int REMOVAL_RX0_B1 = 0x00200000;
	const int REMOVAL_TX0_B0 = 0x00400000;
	const int REMOVAL_TX0_B1 = 0x00600000;
	const int REMOVAL_RX1_B0 = 0x00800000;
	const int REMOVAL_RX1_B1 = 0x00A00000;
	const int REMOVAL_TX1_B0 = 0x00C00000;
	const int REMOVAL_TX1_B1 = 0x00E00000;

	const int BUF_RX0_B0 = 0x0;
	const int BUF_TX0_B0 = 0x1;
	const int BUF_RX0_B1 = 0x2;
	const int BUF_TX0_B1 = 0x3;
	const int BUF_RX1_B0 = 0x4;
	const int BUF_TX1_B0 = 0x5;
	const int BUF_RX1_B1 = 0x6;
	const int BUF_TX1_B1 = 0x7;

	const int mask_irq_rx_packet_0 = 0x00000001;
	const int mask_irq_tx_packet_0 = 0x00000002;
	const int mask_irq_rx_buf_0 = 0x00000004;
	const int mask_irq_tx_buf_0 = 0x00000008;
	const int mask_irq_rx_packet_1 = 0x00000010;
	const int mask_irq_tx_packet_1 = 0x00000020;
	const int mask_irq_rx_buf_1 = 0x00000040;
	const int mask_irq_tx_buf_1 = 0x00000080;

	const int mask_active_buf_rx_0 = 0x00000100;
	const int mask_active_buf_tx_0 = 0x00000200;
	const int mask_active_buf_rx_1 = 0x00000400;
	const int mask_active_buf_tx_1 = 0x00000800;

	const int mask_enable_irq_rx_packet = 0x00000001;
	const int mask_enable_irq_rx_buf = 0x00000002;
	const int mask_enable_irq_tx_packet = 0x00010000;
	const int mask_enable_irq_tx_buf = 0x00020000;
#pragma endregion

err Errors[] =
{
	{FC_ERR_NOERR, "Нет ошибки"},
	{FC_ERR_NOT_INIT, "API FC_LIB не инициализировано"},
	{FC_ERR_ALRD_INIT, "API FC_LIB уже инициализировано"},
	{FC_ERR_DRV_NOT_OPENED, "Драйвер не открыт"},
	{FC_ERR_INV_PAR, "Неверный входной параметр"},
	{FC_ERR_DRV, "Ошибка исполнения команды драйвером"},
	{FC_ERR_EVENT, "Не удалось запустить Event"},
};

void __fastcall ShowErr(char* operation)
{
    char b[200];
    int iret = FC_GetLastError();
    sprintf(b, "%s:%d. %s", operation, iret, FC_GetDescError(iret));
}

int __fastcall FC_GetLastError(void)
{
	return iLastError;
}

char* __fastcall FC_GetDescError(int iCode)
{
	for(int i = 0; i < sizeof(Errors) / sizeof(err); i++)
		if(Errors[i].iErr == iCode)
			return Errors[i].pchrErr;
	return "Неизвестная ошибка";
}




bool __fastcall FC_Close(void)
{
	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_NOT_INIT;
		return false;
	}
  	CloseHandle(hdrv);
	hdrv = INVALID_HANDLE_VALUE;
//   	delete IRQEvent;
        return true;
}





bool __fastcall FC_Init(void)
{
	//Инициализировано ли API FC_LIB
	if(hdrv != INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_ALRD_INIT;
		return false;
	}
	//Попытка открыть драйвер
 	hdrv = CreateFile("\\\\.\\fc_pcie0",
 					  GENERIC_READ | GENERIC_WRITE,
					  FILE_SHARE_READ | FILE_SHARE_WRITE,
					  NULL,
					  OPEN_EXISTING,
					  FILE_ATTRIBUTE_NORMAL,
					  NULL);
	//Открыли ли драйвер
	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_DRV_NOT_OPENED;
		return false;
	}

	iLastError = FC_ERR_NOERR;
	return true;
}


bool __fastcall FC_Reset_Core(int d_reset)
{
	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_NOT_INIT;
		return false;
	}

	ULONG rrr;
	ULONG jj;

	jj = d_reset;

	if(!DeviceIoControl(hdrv,IOCTL_RESET_CORE,&jj,4,NULL,0,&rrr,NULL)){
		iLastError = FC_ERR_DRV;
		return false;
	}

  	iLastError = FC_ERR_NOERR;
	return true;
}

bool __fastcall FC_WriteParameters(int param)
{
	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_NOT_INIT;
		return false;
	}

	ULONG rrr;
	ULONG jj;

	jj = param;

	if(!DeviceIoControl(hdrv,IOCTL_WRITE_PARAMETERS,&jj,4,NULL,0,&rrr,NULL)){
		iLastError = FC_ERR_DRV;
		return false;
	}

  	iLastError = FC_ERR_NOERR;
	return true;
}


rData __fastcall FC_ReadParameters(void)
{
    rData rStt;
	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_NOT_INIT;
        rStt.bStatus = false;
        rStt.rD = 0;
     	return rStt;
	}

        DWORD ulr, ull;
        if(DeviceIoControl(hdrv, IOCTL_READ_PARAMETERS, NULL, 0, &ull, 4, &ulr, NULL))

        {
       	iLastError = FC_ERR_NOERR;
                rStt.rD = ull;
                rStt.bStatus = true;
                return rStt;
        }
        else
        {
       	iLastError = FC_ERR_DRV;
        rStt.bStatus = false;
        rStt.rD = 0;
	return rStt;
        }
}

rData __fastcall FC_ReadStatus(void)
{
    rData rStt;
	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_NOT_INIT;
        rStt.bStatus = false;
        rStt.rD = 0;
     	return rStt;
	}

        DWORD ulr, ull;
        if(DeviceIoControl(hdrv, IOCTL_READ_STATUS, NULL, 0, &ull, 4, &ulr, NULL))

        {
       	iLastError = FC_ERR_NOERR;
                rStt.rD = ull;
                rStt.bStatus = true;
                return rStt;
        }
        else
        {
       	iLastError = FC_ERR_DRV;
        rStt.bStatus = false;
        rStt.rD = 0;
	return rStt;
        }
}

bool __fastcall FC_WriteStatus(int st)
{
	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_NOT_INIT;
		return false;
	}

	ULONG rrr;
	ULONG jj;

	jj = st;

	if(!DeviceIoControl(hdrv,IOCTL_WRITE_STATUS,&jj,4,NULL,0,&rrr,NULL)){
		iLastError = FC_ERR_DRV;
		return false;
	}

  	iLastError = FC_ERR_NOERR;
	return true;
}


bool __fastcall FC_WriteAdr_master(int adr)
{
	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_NOT_INIT;
		return false;
	}

	ULONG rrr;
	ULONG jj;

	jj = adr;

	if(!DeviceIoControl(hdrv,IOCTL_SET_ADR_MASTER,&jj,4,&ulAddress,4,&rrr,NULL)){
		iLastError = FC_ERR_DRV;
		return false;
	}

  	iLastError = FC_ERR_NOERR;
	return true;
}

rData __fastcall FC_ReadAdr_master(void)
{
    rData rStt;
	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_NOT_INIT;
        rStt.bStatus = false;
        rStt.rD = 0;
     	return rStt;
	}

        DWORD ulr, ull;
        if(DeviceIoControl(hdrv, IOCTL_READ_ADR_MASTER, NULL, 0, &ull, 4, &ulr, NULL))

        {
       	iLastError = FC_ERR_NOERR;
                rStt.rD = ull;
                rStt.bStatus = true;
                return rStt;
        }
        else
        {
       	iLastError = FC_ERR_DRV;
        rStt.bStatus = false;
        rStt.rD = 0;
	return rStt;
        }
}

bool FC_SET_RAM_TX_RX(int tx_rx)
{
	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_NOT_INIT;
		return false;
	}

	ULONG rrr;
	ULONG jj;

	jj = tx_rx;

	if(!DeviceIoControl(hdrv,IOCTL_SET_RAM_TX_RX,&jj,4,NULL,0,&rrr,NULL)){
		iLastError = FC_ERR_DRV;
		return false;
	}

  	iLastError = FC_ERR_NOERR;
	return true;
}

rData __fastcall FC_Read_RAM_TX_RX(void)
{
    rData rStt;
	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_NOT_INIT;
        rStt.bStatus = false;
        rStt.rD = 0;
     	return rStt;
	}

        DWORD ulr, ull;
        if(DeviceIoControl(hdrv, IOCTL_READ_RAM_TX_RX, NULL, 0, &ull, 4, &ulr, NULL))

        {
       	iLastError = FC_ERR_NOERR;
                rStt.rD = ull;
                rStt.bStatus = true;
                return rStt;
        }
        else
        {
       	iLastError = FC_ERR_DRV;
        rStt.bStatus = false;
        rStt.rD = 0;
	return rStt;
        }
}

bool __fastcall FC_En_Dis_Timer(int tim)
{
	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_NOT_INIT;
		return false;
	}

	ULONG rrr;
	ULONG jj;

	jj = tim;

	if(!DeviceIoControl(hdrv,IOCTL_EN_DIS_TIMER,&jj,4,NULL,0,&rrr,NULL)){
		iLastError = FC_ERR_DRV;
		return false;
	}

  	iLastError = FC_ERR_NOERR;
	return true;
}

rData __fastcall FC_Read_Timer(void)
{
    rData rStt;
	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_NOT_INIT;
        rStt.bStatus = false;
        rStt.rD = 0;
     	return rStt;
	}

        DWORD ulr, ull;
        if(DeviceIoControl(hdrv, IOCTL_READ_TIMER, NULL, 0, &ull, 4, &ulr, NULL))

        {
       	iLastError = FC_ERR_NOERR;
                rStt.rD = ull;
                rStt.bStatus = true;
                return rStt;
        }
        else
        {
       	iLastError = FC_ERR_DRV;
        rStt.bStatus = false;
        rStt.rD = 0;
	return rStt;
        }
}

bool __fastcall FC_Reset_Count_Frame_TX()
{
	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_NOT_INIT;
		return false;
	}

	ULONG rrr;
	ULONG jj;

	jj = 0;

	if(!DeviceIoControl(hdrv,IOCTL_RESET_COUNT_FRAME_TX,&jj,4,NULL,0,&rrr,NULL)){
		iLastError = FC_ERR_DRV;
		return false;
	}

  	iLastError = FC_ERR_NOERR;
	return true;
}

rData __fastcall FC_Read_Count_Frame_TX(void)
{
    rData rStt;
	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_NOT_INIT;
        rStt.bStatus = false;
        rStt.rD = 0;
     	return rStt;
	}

        DWORD ulr, ull;
        if(DeviceIoControl(hdrv, IOCTL_READ_COUNT_FRAME_TX, NULL, 0, &ull, 4, &ulr, NULL))

        {
       	iLastError = FC_ERR_NOERR;
                rStt.rD = ull;
                rStt.bStatus = true;
                return rStt;
        }
        else
        {
       	iLastError = FC_ERR_DRV;
        rStt.bStatus = false;
        rStt.rD = 0;
	return rStt;
        }
}

bool __fastcall FC_Reset_Count_Frame_RX()
{
	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_NOT_INIT;
		return false;
	}

	ULONG rrr;
	ULONG jj;

	jj = 0;

	if(!DeviceIoControl(hdrv,IOCTL_RESET_COUNT_FRAME_RX,&jj,4,NULL,0,&rrr,NULL)){
		iLastError = FC_ERR_DRV;
		return false;
	}

  	iLastError = FC_ERR_NOERR;
	return true;
}

rData __fastcall FC_Read_Count_Frame_RX(void)
{
    rData rStt;
	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_NOT_INIT;
        rStt.bStatus = false;
        rStt.rD = 0;
     	return rStt;
	}

        DWORD ulr, ull;
        if(DeviceIoControl(hdrv, IOCTL_READ_COUNT_FRAME_RX, NULL, 0, &ull, 4, &ulr, NULL))

        {
       	iLastError = FC_ERR_NOERR;
                rStt.rD = ull;
                rStt.bStatus = true;
                return rStt;
        }
        else
        {
       	iLastError = FC_ERR_DRV;
        rStt.bStatus = false;
        rStt.rD = 0;
	return rStt;
        }
}

bool __fastcall FC_Start_Fc_Tx(int param)
{
	if (hdrv == INVALID_HANDLE_VALUE) {
		iLastError = FC_ERR_NOT_INIT;
		return false;
	}

	ULONG rrr;
	ULONG jj;

	jj = param;

	if (!DeviceIoControl(hdrv, IOCTL_START_FC_TX, &jj, 4, NULL, 0, &rrr, NULL)) {
		iLastError = FC_ERR_DRV;
		return false;
	}

	iLastError = FC_ERR_NOERR;
	return true;
}

bool __fastcall FC_Write_Idle_Seq_Tx(int idle)
{
	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_NOT_INIT;
		return false;
	}

	ULONG rrr;
	ULONG jj;

	jj = idle;

	if(!DeviceIoControl(hdrv,IOCTL_WRITE_IDLE_SEQ,&jj,4,NULL,0,&rrr,NULL)){
		iLastError = FC_ERR_DRV;
		return false;
	}

  	iLastError = FC_ERR_NOERR;
	return true;
}

rData __fastcall FC_Read_Idle_Seq_Tx(void)
{
    rData rStt;
	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_NOT_INIT;
        rStt.bStatus = false;
        rStt.rD = 0;
     	return rStt;
	}

        DWORD ulr, ull;
        if(DeviceIoControl(hdrv, IOCTL_READ_IDLE_SEQ, NULL, 0, &ull, 4, &ulr, NULL))

        {
       	iLastError = FC_ERR_NOERR;
                rStt.rD = ull;
                rStt.bStatus = true;
                return rStt;
        }
        else
        {
       	iLastError = FC_ERR_DRV;
        rStt.bStatus = false;
        rStt.rD = 0;
	return rStt;
        }
}

bool __fastcall FC_WriteAdr_Fc_Manage(int adr)
{
	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_NOT_INIT;
		return false;
	}

	ULONG rrr;
	ULONG jj;

	jj = adr;

	if(!DeviceIoControl(hdrv,IOCTL_WRITE_ADR_FC_MANAGE,&jj,4,NULL,0,&rrr,NULL)){
		iLastError = FC_ERR_DRV;
		return false;
	}

  	iLastError = FC_ERR_NOERR;
	return true;
}

bool __fastcall FC_WriteData_Fc_Manage(int adr)
{
	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_NOT_INIT;
		return false;
	}

	ULONG rrr;
	ULONG jj;

	jj = adr;

	if(!DeviceIoControl(hdrv,IOCTL_WRITE_DATA_FC_MANAGE,&jj,4,NULL,0,&rrr,NULL)){
		iLastError = FC_ERR_DRV;
		return false;
	}

  	iLastError = FC_ERR_NOERR;
	return true;
}

rData __fastcall FC_ReadData_Fc_Manage(void)
{
    rData rStt;
	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_NOT_INIT;
        rStt.bStatus = false;
        rStt.rD = 0;
     	return rStt;
	}

        DWORD ulr, ull;
        if(DeviceIoControl(hdrv, IOCTL_READ_DATA_FC_MANAGE, NULL, 0, &ull, 4, &ulr, NULL))

        {
       	iLastError = FC_ERR_NOERR;
                rStt.rD = ull;
                rStt.bStatus = true;
                return rStt;
        }
        else
        {
       	iLastError = FC_ERR_DRV;
        rStt.bStatus = false;
        rStt.rD = 0;
	return rStt;
        }
}

rData __fastcall FC_ReadSTATUS_RX(void)
{
    rData rStt;
	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_NOT_INIT;
        rStt.bStatus = false;
        rStt.rD = 0;
     	return rStt;
	}

        DWORD ulr, ull;
        if(DeviceIoControl(hdrv, IOCTL_READ_STATUS_RX, NULL, 0, &ull, 4, &ulr, NULL))

        {
       	iLastError = FC_ERR_NOERR;
                rStt.rD = ull;
                rStt.bStatus = true;
                return rStt;
        }
        else
        {
       	iLastError = FC_ERR_DRV;
        rStt.bStatus = false;
        rStt.rD = 0;
	return rStt;
        }
}

rData __fastcall FC_ReadSTATISTICS(void)
{
    rData rStt;
	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_NOT_INIT;
        rStt.bStatus = false;
        rStt.rD = 0;
     	return rStt;
	}

        DWORD ulr, ull;
        if(DeviceIoControl(hdrv, IOCTL_READ_STATISTICS, NULL, 0, &ull, 4, &ulr, NULL))

        {
       	iLastError = FC_ERR_NOERR;
                rStt.rD = ull;
                rStt.bStatus = true;
                return rStt;
        }
        else
        {
       	iLastError = FC_ERR_DRV;
        rStt.bStatus = false;
        rStt.rD = 0;
	return rStt;
        }
}


rbData __fastcall FC_ReadCONFIG_STATUS()
{
    rbData rStt;

	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_NOT_INIT;
        rStt.bStatus = false;
        rStt.rbD = 0;
     	return rStt;
	}

        DWORD ulr;

        if(DeviceIoControl(hdrv, IOCTL_READ_CONFIG_STATUS, NULL, 0, &obuf,32,&ulr, NULL))

        {
       	iLastError = FC_ERR_NOERR;
                rStt.rbD = obuf;
                rStt.bStatus = true;
                return rStt;
        }
        else
        {
       	iLastError = FC_ERR_DRV;
        rStt.bStatus = false;
        rStt.rbD = 0;
	return rStt;
        }
}

bool __fastcall FC_Reset_TX(int data)
{
	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_NOT_INIT;
		return false;
	}

	ULONG rrr;
	ULONG jj;

	jj = data;

	if(!DeviceIoControl(hdrv,IOCTL_RESET_TX,&jj,4,NULL,0,&rrr,NULL)){
		iLastError = FC_ERR_DRV;
		return false;
	}

  	iLastError = FC_ERR_NOERR;
	return true;
}

bool __fastcall FC_Reset_RX(int data)
{
	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_NOT_INIT;
		return false;
	}

	ULONG rrr;
	ULONG jj;

	jj = data;

	if(!DeviceIoControl(hdrv,IOCTL_RESET_RX,&jj,4,NULL,0,&rrr,NULL)){
		iLastError = FC_ERR_DRV;
		return false;
	}

  	iLastError = FC_ERR_NOERR;
	return true;
}

bool __fastcall FC_Write_TX_Slot(int data)
{
	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_NOT_INIT;
		return false;
	}

	ULONG rrr;
	ULONG jj;

	jj = data;

	if(!DeviceIoControl(hdrv,IOCTL_WRITE_TX_SLOT,&jj,4,NULL,0,&rrr,NULL)){
		iLastError = FC_ERR_DRV;
		return false;
	}

  	iLastError = FC_ERR_NOERR;
	return true;
}

bool __fastcall FC_Write_RX_Slot(int data)
{
	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_NOT_INIT;
		return false;
	}

	ULONG rrr;
	ULONG jj;

	jj = data;

	if(!DeviceIoControl(hdrv,IOCTL_WRITE_RX_SLOT,&jj,4,NULL,0,&rrr,NULL)){
		iLastError = FC_ERR_DRV;
		return false;
	}

  	iLastError = FC_ERR_NOERR;
	return true;
}

rData __fastcall FC_Read_TX_Slot(void)
{
    rData rStt;
	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_NOT_INIT;
        rStt.bStatus = false;
        rStt.rD = 0;
     	return rStt;
	}

        DWORD ulr, ull;
        if(DeviceIoControl(hdrv, IOCTL_READ_TX_SLOT, NULL, 0, &ull, 4, &ulr, NULL))

        {
       	iLastError = FC_ERR_NOERR;
                rStt.rD = ull;
                rStt.bStatus = true;
                return rStt;
        }
        else
        {
       	iLastError = FC_ERR_DRV;
        rStt.bStatus = false;
        rStt.rD = 0;
	return rStt;
        }
}

rData __fastcall FC_Read_RX_Slot(void)
{
    rData rStt;
	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_NOT_INIT;
        rStt.bStatus = false;
        rStt.rD = 0;
     	return rStt;
	}

        DWORD ulr, ull;
        if(DeviceIoControl(hdrv, IOCTL_READ_RX_SLOT, NULL, 0, &ull, 4, &ulr, NULL))

        {
       	iLastError = FC_ERR_NOERR;
                rStt.rD = ull;
                rStt.bStatus = true;
                return rStt;
        }
        else
        {
       	iLastError = FC_ERR_DRV;
        rStt.bStatus = false;
        rStt.rD = 0;
	return rStt;
        }
}

rData __fastcall FC_ReadMem(int adr)
{
    rData rStt;
	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_NOT_INIT;
        rStt.bStatus = false;
        rStt.rD = 0;
     	return rStt;
	}

        DWORD ulr, ull;
        if(DeviceIoControl(hdrv, IOCTL_MEM_READ, &adr, 4, &ull, 4, &ulr, NULL))

        {
       	iLastError = FC_ERR_NOERR;
                rStt.rD = ull;
                rStt.bStatus = true;
                return rStt;
        }
        else
        {
       	iLastError = FC_ERR_DRV;
        rStt.bStatus = false;
        rStt.rD = 0;
	return rStt;
        }
}

bool __fastcall FC_WriteMem(int *adr_data)
{
	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_NOT_INIT;
		return false;
	}

	ULONG rrr;

	if(!DeviceIoControl(hdrv,IOCTL_MEM_WRITE,adr_data,8,NULL,0,&rrr,NULL)){
		iLastError = FC_ERR_DRV;
		return false;
	}

  	iLastError = FC_ERR_NOERR;
	return true;
}

rbData __fastcall FC_ReadBlockMem(int *num_adr)
{
    rbData rStt;

    int ibuf[2];

	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_NOT_INIT;
        rStt.bStatus = false;
        rStt.rbD = 0;
     	return rStt;
	}

    DWORD ulr;

    if(DeviceIoControl(hdrv, IOCTL_MEM_BLOCK_READ, num_adr, 8, &obuf, num_adr[0]*4, &ulr, NULL))
	{
       	iLastError = FC_ERR_NOERR;
                rStt.rbD = obuf;
                rStt.bStatus = true;
                return rStt;
    }
    else
    {
       	iLastError = FC_ERR_DRV;
        rStt.bStatus = false;
        rStt.rbD = 0;
		return rStt;
    }
}

bool __fastcall FC_WriteBlockMem(UINT32 *adr_data)
{
	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_NOT_INIT;
		return false;
	}

	ULONG rrr;

	if(!DeviceIoControl(hdrv,IOCTL_MEM_BLOCK_WRITE,adr_data,adr_data[0]*4 + 8,NULL,0,&rrr,NULL)){
		iLastError = FC_ERR_DRV;
		return false;
	}

  	iLastError = FC_ERR_NOERR;
	return true;
}


bool __fastcall FC_Get_Adr(void* b)
{
	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_NOT_INIT;
		return false;
	}

	if(b == NULL){
		iLastError = FC_ERR_INV_PAR;
		return false;
	}

	*(ULONG*)b = ulAddress ;

	iLastError = FC_ERR_NOERR;
	return true;
}

rData __fastcall FC_ReadFlag_irq(void)
{
    rData rStt;
	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_NOT_INIT;
        rStt.bStatus = false;
        rStt.rD = 0;
     	return rStt;
	}

        DWORD ulr, ull;
        if(DeviceIoControl(hdrv, IOCTL_CHKEVENT, NULL, 0, &ull, 4, &ulr, NULL))

        {
       	iLastError = FC_ERR_NOERR;
                rStt.rD = ull;
                rStt.bStatus = true;
                return rStt;
        }
        else
        {
       	iLastError = FC_ERR_DRV;
        rStt.bStatus = false;
        rStt.rD = 0;
	return rStt;
        }
}

bool __fastcall FC_ResetFlag_irq(void)
{
	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_NOT_INIT;
		return false;
	}

        DWORD ulr, ull;

	if(DeviceIoControl(hdrv, IOCTL_SETEVENT, NULL, 0, NULL, 0, &ulr, NULL))
        {
       	iLastError = FC_ERR_NOERR;
        return true;
        }
        else
        {
       	iLastError = FC_ERR_DRV;
        return false;
        }

}


bool __fastcall FC_IsReady(bool *bl)
{
	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_NOT_INIT;
		return false;
	}

        DWORD ulr, ull, tt;
        if(!DeviceIoControl(hdrv, IOCTL_CHKEVENT, NULL, 0, &ull, 4, &ulr, NULL))
        {
        iLastError = FC_ERR_DRV;
        return false;
        }
        tt = ull;
        if (tt!=0)
        {
	        if(!DeviceIoControl(hdrv, IOCTL_SETEVENT, NULL, 0, NULL, 0, &ulr, NULL))
                {
       	        iLastError = FC_ERR_DRV;
                return false;
                }
                else
                {
       	        iLastError = FC_ERR_NOERR;
                }
        *bl = true;
        }
        else
        {
        *bl = false;
        }
return true;
}

rbData __fastcall FC_ReadConfig_Pci(void)
{

    rbData rStt;

	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_NOT_INIT;
        rStt.bStatus = false;
        rStt.rbD = 0;
     	return rStt;
	}

        DWORD ulr;

        if(DeviceIoControl(hdrv, IOCTL_READ_PCI_CONFIGURATION, NULL, 0, &obuf, 64*4, &ulr, NULL))

        {
       	iLastError = FC_ERR_NOERR;
                rStt.rbD = obuf;
                rStt.bStatus = true;
                return rStt;
        }
        else
        {
       	iLastError = FC_ERR_DRV;
        rStt.bStatus = false;
        rStt.rbD = 0;
	return rStt;
        }
}

bool __fastcall FC_WriteApplybackpressure(int apl)
{
	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_NOT_INIT;
		return false;
	}

	ULONG rrr;
	ULONG jj;

	jj = apl;

	if(!DeviceIoControl(hdrv,IOCTL_WRITE_APPLYBACKPRESSURE,&jj,4,NULL,0,&rrr,NULL)){
		iLastError = FC_ERR_DRV;
		return false;
	}

  	iLastError = FC_ERR_NOERR;
	return true;
}

rData __fastcall FC_ReadApplybackpressure(void)
{
    rData rStt;
	if(hdrv == INVALID_HANDLE_VALUE){
		iLastError = FC_ERR_NOT_INIT;
        rStt.bStatus = false;
        rStt.rD = 0;
     	return rStt;
	}

        DWORD ulr, ull;
        if(DeviceIoControl(hdrv, IOCTL_READ_APPLYBACKPRESSURE, NULL, 0, &ull, 4, &ulr, NULL))

        {
       	iLastError = FC_ERR_NOERR;
                rStt.rD = ull;
                rStt.bStatus = true;
                return rStt;
        }
        else
        {
       	iLastError = FC_ERR_DRV;
        rStt.bStatus = false;
        rStt.rD = 0;
	return rStt;
        }
}

bool Connect_ch(unsigned int ch)
{
	if (ch < 2)
	{
		int param_ch = ch << 2;

		FC_SET_RAM_TX_RX(param_ch);

		FC_WriteAdr_Fc_Manage(OpticsControl_REG_ADDR);
		FC_WriteData_Fc_Manage(0x00000000);

		FC_WriteAdr_Fc_Manage(Timeout_REG_ADDR);
		FC_WriteData_Fc_Manage(0x27100000);
		//FC_WriteData_Fc_Manage(0x00000000);

		FC_WriteAdr_Fc_Manage(OpticsControl_REG_ADDR);
		FC_WriteData_Fc_Manage(0x00000000);

		FC_WriteAdr_Fc_Manage(Credit_REG_ADDR);
		//FC_WriteData_Fc_Manage(0x00000010);
		FC_WriteData_Fc_Manage(0x00000000);

		FC_WriteAdr_Fc_Manage(LinkControl_REG_ADDR);
		FC_WriteData_Fc_Manage(0x00000008);

		FC_WriteAdr_Fc_Manage(LinkStatus_REG_ADDR);

		Sleep(10);

		rdd = FC_ReadData_Fc_Manage();

		if (!rdd.bStatus)
		{
			//ShowErr("Ошибка открытия драйвера");
			printf("Error while open driver\n");
		}

		if (((rdd.rD & 0x001F0000) >> 16) != 0x00000001)
		{
			return false;
		}

		FC_WriteAdr_Fc_Manage(PSMControl_REG_ADDR);
		FC_WriteData_Fc_Manage(0x00000001);
	}
	else
		return false;

	return true;
}

