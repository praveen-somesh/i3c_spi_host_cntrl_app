/*!
* \file sample_lhy.c
*/
/******************************************************************************/
/* 							 Include files									  */
/******************************************************************************/
/* Standard C libraries */
#include<stdio.h>
#include<conio.h>
#include<stdlib.h>
#include<string.h>
#include <conio.h>
#include <signal.h>
#include <time.h>
/* OS specific libraries */
#ifdef _WIN32
#include<windows.h>
// #include<time.h>
#include <process.h>
#endif
/* Include D2XX header*/
#include "ftd2xx.h"
/* Include libMPSSE header */
#include "libMPSSE_spi.h"
/* Include device_table header */
#include "device_table.h"
/******************************************************************************/
/*								Macro and type defines							   */
/******************************************************************************/
/* Helper macros */
#define APP_CHECK_STATUS(exp) {if(exp!=FT_OK){printf("%s:%d:%s(): status(0x%x) \
!= FT_OK\n",__FILE__, __LINE__, __FUNCTION__,exp);exit(1);}else{;}};
#define CHECK_NULL(exp){if(exp==NULL){printf("%s:%d:%s():  NULL expression \
encountered \n",__FILE__, __LINE__, __FUNCTION__);exit(1);}else{;}};
// void CALLBACK MasterPollIsr4ResponseProc(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime);
void MasterPollIsr4ResponseProc(void);
FTDI_API FT_STATUS FTDI_SPI_ReadWrite(FT_HANDLE handle, uint8 *inBuffer,
	uint8 *outBuffer, uint32 sizeToTransfer, uint32 *sizeTransferred,
	uint32 transferOptions);
/* Application specific macro definations */
#define I3C_MASTER_RESPONSE_BYTE0_IS_PRIVATE_MESSAGE	0x01
#define I3C_MASTER_RESPONSE_BYTE0_IS_BROADCAST_CCC		0x02
#define I3C_MASTER_RESPONSE_BYTE0_IS_DIRECT_CCC			0x03
#define I3C_MASTER_RESPONSE_BYTE0_IS_LEGACY_I2C_MESSAGE	0x04
#define COMMAND_FAILED									0x01
#define COMMAND_SUCCESS									0x00
#define SLAVE_ID										0x10
#define MASTER_ID										0x20
#define RSTDAA											0x06
#define ENTDAA											0x07
#define I3C_MASTER_IBI_RESPONSE_BYTE0_IS_UNRECOGNIZED   0x00
#define I3C_MASTER_IBI_RESPONSE_BYTE0_IS_IBI_REQ        0x01
#define I3C_MASTER_IBI_RESPONSE_BYTE0_IS_HOTJOIN_REQ    0x02
#define I3C_MASTER_IBI_RESPONSE_BYTE0_IS_SEC_MASTR_REQ  0x04
#define I3C_MASTER_IBI_RESPONSE_BYTE3_IS_REQ_ACK        0x00
#define I3C_MASTER_IBI_RESPONSE_BYTE3_IS_REQ_NACK       0x01

/******************************************************************************/
/*								DCT variables							  	    */
/******************************************************************************/
#define PID_5 0x04
#define PID_4 0x1E
#define PID_3 0xFF
#define PID_2_SLAVE_ID 0xF0
#define PID_1 0x00
#define PID_0 0x00
#define BCR_CONFIG 0x06
#define BCR_CONFIG_master 0x4E
#define DCR_CONFIG 0x00
uint8 pid5;
uint8 pid4;
uint8 pid3;
uint8 pid2;
uint8 pid1;
uint8 pid0;
uint8 bcr;
uint8 dcr;
uint8 dynamic_addr;
uint8 static_addr;
/******************************************************************************/
/*								Global variables							  	    */
/******************************************************************************/
static FT_HANDLE ftHandle;
uint8 read_Buffer[100];
uint8 inBuffer[128];
uint8 outBuffer_slave_isr_req[128];
uint8 outBuffer_rw[128];
uint8 outBuffer_ISR_req_slave[] = {0x00,0x10,0x00,0x1c};//write cmd//14->10
uint8 outBuffer_ISR_status_slave[] = {0x00,0x12,0x03,0x00,0x00};//read cmd//23 -> 12
uint8 outBuffer_read_ID[] = {0x00,0x00,0x03,0x00,0x00};//last two 00's are dummys//read cmd
//BCC buffer Commands

//0-msb,1-lsb,2-write_cmd,3-cmd_descriptor,4-ccc code,5-no.of slaves,6-data length
uint8 outbuffer_bccc[128];
uint8 outBuffer_BCCC_COMMAND[] = {0x00,0x10,0x00,0x1a,0x00,0x01};//write cmd /14->10
///////DCC commands
int dcc_data_len=0;
uint8 outbuffer_dccc[128];
//0-msb,1-lsb,2-write_cmd,3-cmd_descriptor,4-ccc code,5-no.of slaves,6-slaveaddr + RW bit,7-data length
uint8 outBuffer_DCCC_COMMAND[] = {0x00,0x10,0x00,0x1b,0x80,0x01};//write cmd /14->10
//slave//
uint8 outBuffer_Response_pv_wr_buffer_hdcd[] = {0x01,0x00};
uint8 outBuffer_Response_set_bcc[128];
int set_ccc=0;
uint8 outBuffer_Response_pv_wr[256];
//
//Master
uint8 outbuffer_daa[128];
uint8 outBuffer_DAA[] = {0x00,0x10,0x08,0x1a,0x07,0x01,0x00};
uint8 outBuffer_Response_buffer[] = {0x00,0x11,0x05,0x00,0x00,0x00};//15 ->11
uint8 outBuffer_IBI_Response_buffer[] = {0x00,0x16,0x05,0x00,0x00,0x00};
uint8 MS_error_register_buffer[] = {0x00,0x13,0x05,0x00,0x00,0x00};//15 ->11
uint8 CAM_for_CR_buffer[] = {0x00,0x18,0x00,0x00};//15 ->11
uint8 CAM_for_CR_buffer_arr[5];
//PID BIR DIR DAA
uint8 outBuffer_pid_bir_dir_daa[] = {0xfd,0x00,0x25,0x1a,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
uint8 outbuffer_private_hdcd[] = {0x00,0x10,0x00,0x19};
uint8 outbuffer_private[256];
uint8 outbuffer_i2c_legacy_hdcd[] = {0x00,0x10,0x00,0x1C};
uint8 k, l;
//
//Slave
//
static int ibi_enable=0;
uint8 Device_ID_MS=0x00;
static int config_done=0;
uint8 slave_config_data[] = {0x00,0x01,0x02};
uint8 slave_congig_pointer[10];
static uint8 master_ibi_payload_max=0x04;
uint8 outbuffer_ibi_payload_size[] = {0x00,0x0d,0x02};
uint8 outbuffer_ibi_payload_size_arrary[4];

uint8 Error_ID=0x00;
int slaves_count=0;
int setmrl_done=0;
//Master
int Invalid=0;
int daa_done=0;
int private_read_len = 0;
int private_write_len = 0;
uint8 slave_0_addr = 0;
uint8 slave_1_addr = 0;
uint8 slave_addr = 0;
uint8 target_slave_addr = 0x00;
uint8 setnewda_dyna_addr_by_user = 0x00;
int slave_isr_len = 0;
int ccc_len=0;
int mresp_len =0;
int index2 = 0;
uint32 sizeToTransfer, sizeTransferred;
unsigned master = 0xf0;
unsigned slave_0 = 0xe0;
unsigned slave_1 = 0xe1;
unsigned slave_2 = 0xe2;
uint8 resp_command = 0x00;

uint8 ibi_resp_command = 0x00;
uint8 data_length = 0x00;
uint8 ibi_data_length = 0x00;
char is_entdaa_first_cmd[16];
char setmrl_byte[16] ;
int retry=0;
unsigned char CMD_BUFFR_NOT_FREE = 0;
unsigned char RESP_BUFFR_NOT_FREE = 0;
unsigned char IBI_nCMD = 0;
uint8   device_dyn_addr = 0x7E;
uint8   device_static_addr = 0x00;
uint8 soft_reset_issued = 0;
typedef enum {
IDLE,
CMD,
CMD_LEN,DATA,ECHO_RESPONSE,PRIVATE_WRITE,PRIVATE_READ,I2C_WRITE,I2C_READ
}response_states;
typedef enum {
IBI_IDLE,
IBI_TYPE,
IBI_LEN,IBI_DATA,IBI_ECHO_RESPONSE
}master_ibi_response_states;
response_states slave_resp = IDLE;
response_states master_resp = IDLE;
master_ibi_response_states master_ibi_resp = IDLE;
// enum response_states {IDLE,CMD,CMD_LEN,DATA,ECHO_RESPONSE};
// enum response_states slave_resp;
uint8 slave_resp_rcvd_data;
uint8 ibi_resp_rcvd_data;
uint8 inbuffer_slave[256];
uint8 inbuffer_ibi[256];
int poll_mesg = 1;
uint8 dct_valid =0;
uint8 CCC_CODE = 0x00;
uint8 rstdaa_dynamic_addr = 0x00;
uint8 setdasa_static_addr = 0x00;
uint8 setdasa_dynamic_addr = 0x00;
uint8 setnewda_old_dynamic_addr = 0x00;
uint8 setnewda_new_dynamic_addr = 0x00;
uint8 get_dynamic_addr = 0x00;
uint8 ibi_payload_size_new = 0x00;
uint8 ibi_payload_size_old = 0x00;

//struct device_characteristic_table_t *dct; //msd
//struct i3c_device_characteristic_t *dat;//msd

void sigfun(int sig)
{
			FT_STATUS status = FT_OK;
			status = SPI_CloseChannel(ftHandle);
			printf("\nYou have presses Ctrl-C ,>> Exiting application\n");
			printf("*******EXITED..Thank you*******\n");
			system("exit");
			return;
}
void timestamp(FILE * file)
{
    time_t ltime; /* calendar time */
    ltime=time(NULL); /* get current cal time */
 //   printf("%s",asctime( localtime(&ltime) ) );
      fprintf(file,"%s",asctime( localtime(&ltime) ) );
}
BOOL WINAPI ConsoleHandler(DWORD dwCtrlEvent){

	// FILE *stream ;
	// if((stream = freopen("file.txt", "w", stdout)) == NULL)
		// exit(-1);
	// printf("this is stdout output\n");
	// printf("this is stdout output\n");
	// printf("this is stdout output\n");
	// stream = freopen("CON", "w", stdout);
	//stream = freopen("file.txt", "w+", stdout)
	switch (dwCtrlEvent){
		case CTRL_C_EVENT : {
			sigfun(1);
			return FALSE;
			break;
		}
		case CTRL_BREAK_EVENT :
		case CTRL_CLOSE_EVENT :
		case CTRL_LOGOFF_EVENT :
		case CTRL_SHUTDOWN_EVENT :
			{
				FT_STATUS status = FT_OK;
				status = SPI_CloseChannel(ftHandle);
				if(master_CmdResp_FileObj != NULL){
					fclose(master_CmdResp_FileObj);
					fprintf(master_IbiResp_FileObj," >>Master Exiting Application\n");
				}
				if(slave_CmdResp_FileObj != NULL){
					fclose(slave_CmdResp_FileObj);
					fprintf(slave_CmdResp_FileObj," >> Slave Exiting Application\n");
				}
				if(master_IbiResp_FileObj != NULL){
					fclose(master_IbiResp_FileObj);
					fprintf(master_IbiResp_FileObj," >>Master Exiting Application\n");
				}
				printf("Exiting application\n");
				
				printf("*******EXITED..Thank you*******\n");
				system("exit");
				return TRUE;
			}
		default :
			return FALSE;
	}
}

int is_data_ready()
{
	FT_STATUS status = FT_OK;
	
	//polling the interrupt status register....
		//outBuffer_Response_buffer[2]=0x05;
		//Sleep(0.1);
	sizeToTransfer = 5;  
	sizeTransferred = 0;
	status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_ISR_status_slave, sizeToTransfer, &sizeTransferred,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	APP_CHECK_STATUS(status);
	
	if (((*(inBuffer + 4)) & 1 << 2))
	{
		if(((*(inBuffer + 4))) & (1 << 7)!=0)
		{
		
			sizeToTransfer = 5;  
			sizeTransferred = 0;
			status = SPI_ReadWrite(ftHandle, inBuffer, MS_error_register_buffer, sizeToTransfer, &sizeTransferred,
				SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
				SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
				SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
			APP_CHECK_STATUS(status);
			Error_ID=(*(inBuffer + 4));
			if (Error_ID==0x00){
			printf("\n	********** S-0 ERROR DETECTED **********\n");
			fprintf(slave_CmdResp_FileObj," \n>>**** S-0 ERROR DETECTED ****\n");			
			}
			else if (Error_ID==0x01){printf("\n	********** S-1 ERROR DETECTED **********\n");fprintf(slave_CmdResp_FileObj," >>**** S-1 ERROR DETECTED ****\n");}
			else if (Error_ID==0x02){printf("\n	********** S-2 ERROR DETECTED **********\n");fprintf(slave_CmdResp_FileObj," >>**** S-2 ERROR DETECTED ****\n");}
			else if (Error_ID==0x03){printf("\n	********** S-3 ERROR DETECTED **********\n");fprintf(slave_CmdResp_FileObj," >>**** S-3 ERROR DETECTED ****\n");}
		}
		return 1;
	}
	
	return 0;
}

uint8 check_isr()
{
	FT_STATUS status = FT_OK;
	uint8 retVal;
	
	//polling the interrupt status register....
		//outBuffer_Response_buffer[2]=0x05;
		//Sleep(0.1);
	sizeToTransfer = 5;  
	sizeTransferred = 0;
	status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_ISR_status_slave, sizeToTransfer, &sizeTransferred,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	APP_CHECK_STATUS(status);
	retVal = (*(inBuffer + 4));
	// printf("retVal: %d\n",retVal);
	return retVal;
}

int is_ibi_data_ready()
{
	FT_STATUS status = FT_OK;
	
	//polling the interrupt status register....
		//outBuffer_Response_buffer[2]=0x05;
		//Sleep(0.1);
	sizeToTransfer = 5;  
	sizeTransferred = 0;
	status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_ISR_status_slave, sizeToTransfer, &sizeTransferred,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	APP_CHECK_STATUS(status);
	
	if (((*(inBuffer + 4)) & 1 << 3))
	{
		return 1;
	}
	
	return 0;
}



int main()
{
	(void) signal(SIGINT, sigfun);
	int num_master = 0;
	int num_slave = 0;
	int select = 0, device = 0, device_id = 0, rw = 0, rw_loop = 0, rw_len = 0, rw_data = 0, rw_index = 0;
	int ccc = 0;
	int user_data = 0;
	int user_data_1 = 0;
	int user_data_2 = 0;
	int user_data_3 = 0;
	int cc = 0;
	int master_channels[16];
	int slave_channels[16];
	HANDLE hTimer = NULL;
	FT_STATUS status = FT_OK;
	FT_DEVICE_LIST_INFO_NODE devList = { 0 };
	ChannelConfig channelConf = { 0 };
	uint8 address = 0;
	uint32 channels = 0;
	uint16 data = 0;
	uint8 i = 0;
	int index = 0;
	// time_t time_now = time(NULL);
	char timestr[50];
	// Create a systemtime struct
	SYSTEMTIME thesystemtime;
	printf("***********************************************************\n");
	printf("         WELCOME to LATTICE SEMICONDUCTOR I3C DEMO   \n");
	printf("***********************************************************\n");
	printf("Please wait...\n");
   // Get current system time
    GetLocalTime(&thesystemtime);
	char *filename_str;
	char *filename_str_slave;
	uint8 latency = 255;
	channelConf.ClockRate = 6000000;
	channelConf.LatencyTimer = latency;
	channelConf.configOptions = SPI_CONFIG_OPTION_MODE3 | SPI_CONFIG_OPTION_CS_DBUS4 | SPI_CONFIG_OPTION_CS_ACTIVELOW;
	channelConf.Pin = 0xff000000;/*FinalVal-FinalDir-InitVal-InitDir (for dir 0=in, 1=out)*/
	/* init library */
#ifdef _MSC_VER
	Init_libMPSSE();
#endif
	if(SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler,TRUE) == FALSE){
	//// unable to install handler... 
	//// display message to the user
		printf("Unable to install handler!\n");
		return -1;
	}
	status = SPI_GetNumChannels(&channels);
	APP_CHECK_STATUS(status);
	printf("Number of available SPI channels are: %d\n", (int)channels);
	// status = SPI_GetChannelInfo(0,&devList);
	// APP_CHECK_STATUS(status);
	// printf("Information on channel number %d:\n",0);
	// printf("		ID=0x%x\n",devList.ID);
	printf("Please wait establishing connection with the hardware...\n");
	if (channels > 0)
	{
		/*for(i=0;i<channels;i++)
		{
		status = SPI_GetChannelInfo(i,&devList);
		APP_CHECK_STATUS(status);
		printf("Information on channel number %d:\n",i);
		// print the dev info
		printf("		Flags=0x%x\n",devList.Flags);
		printf("		Type=0x%x\n",devList.Type);
		printf("		ID=0x%x\n",devList.ID);
		printf("		LocId=0x%x\n",devList.LocId);
		printf("		SerialNumber=%s\n",devList.SerialNumber);
		printf("		Description=%s\n",devList.Description);
		printf("		ftHandle=0x%x\n",(unsigned int)devList.ftHandle);//is 0 unless open
		}*/
	
		if(channels == 1)
		{
		printf("Check the USB cable connection\n	EXIT\n");
		system("pause");
		return 0;
		}
		/* Open the first available channel */
		status = SPI_OpenChannel(cc, &ftHandle);
		APP_CHECK_STATUS(status);
		status = SPI_InitChannel(ftHandle, &channelConf);
		APP_CHECK_STATUS(status);
	
		//Reading the ID register to distinguish between the boards.
		*(inBuffer + 4)=0x00;
		sizeToTransfer = 5;
		sizeTransferred = 0;
		status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_read_ID, sizeToTransfer, &sizeTransferred,
			SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
			SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
			SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
		APP_CHECK_STATUS(status);
		
		Device_ID_MS=(*(inBuffer + 4));
		
		device=0;device_id = 0;
		if (((*(inBuffer + 4)) & 0xF0) == MASTER_ID)  { 
			device=1;	
			if(config_done==0)
			{
				//Master configuration commands
				for(index = 0; index < 3; index++){
					slave_congig_pointer[index] = slave_config_data[index];
					}
				for(index = 0; index < 9; index++){
				if(index==0){slave_congig_pointer[1]=0x01;slave_congig_pointer[3]=PID_5;}
				if(index==1){slave_congig_pointer[1]=0x02;slave_congig_pointer[3]=PID_4;}
				if(index==2){slave_congig_pointer[1]=0x03;slave_congig_pointer[3]=PID_3;}
				if(index==3){slave_congig_pointer[1]=0x04;slave_congig_pointer[3]=0x00;}
				if(index==4){slave_congig_pointer[1]=0x05;slave_congig_pointer[3]=PID_1;}
				if(index==5){slave_congig_pointer[1]=0x06;slave_congig_pointer[3]=PID_0;}
				if(index==6){slave_congig_pointer[1]=0x07;slave_congig_pointer[4]=BCR_CONFIG_master;}
				if(index==7){slave_congig_pointer[1]=0x08;slave_congig_pointer[4]=DCR_CONFIG;}
				if(index==8){slave_congig_pointer[1]=0x14;slave_congig_pointer[3]=0x87;}
				sizeToTransfer = 4;
				sizeTransferred = 0;
				status = SPI_ReadWrite(ftHandle, inBuffer, slave_congig_pointer, sizeToTransfer, &sizeTransferred,
					SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
					SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
					SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
				APP_CHECK_STATUS(status);
				}
				config_done=1;
			}
			printf("Connected to I3C master device\n");	
		}
		else if (((*(inBuffer + 4)) & 0xF0)	== SLAVE_ID) { 
			device=2;
		}
		else{
			printf("Device ID:0x%x not matched..\n EXIT\n",(*(inBuffer + 4)));
			system("pause");
			SPI_CloseChannel(ftHandle);
			return 0;
		}
		device_id = ((*(inBuffer + 4)) & 0x0F);
		printf("\n");
		CMD_BUFFR_NOT_FREE = 1;
		if(device==1)
		{
			// CreateTimerQueueTimer(&hTimer,NULL, MasterPollIsr4ResponseProc,NULL,1,500,WT_EXECUTEDEFAULT );
			// if(hTimer == NULL){
				// printf("No timer exists\n");
			// }
			
			timestr[sprintf(timestr,"_%02d_%02d_%04d_%02d_%02d_%02d", thesystemtime.wDay, thesystemtime.wMonth, thesystemtime.wYear,thesystemtime.wHour,thesystemtime.wMinute,thesystemtime.wSecond)+1] = '\0';
			// printf("time: %s\n",timestr);
			filename_str = (char*)malloc(strlen("master_cmd_response")+strlen(timestr)+strlen(".log"));
			if(filename_str != NULL) {
				filename_str[0] = '\0';
				strcat(filename_str,"master_cmd_response");
				strcat(filename_str,timestr);
				strcat(filename_str,".log");
			}
			master_CmdResp_FileObj = fopen(filename_str, "w+");
			fprintf(master_CmdResp_FileObj,"\n\n*************I3C Master Response Report ***************** \n");
			
			filename_str = (char*)malloc(strlen("master_ibi_response")+strlen(timestr)+strlen(".log"));
			if(filename_str != NULL) {
				filename_str[0] = '\0';
				strcat(filename_str,"master_ibi_response");
				strcat(filename_str,timestr);
				strcat(filename_str,".log");
			}
			master_IbiResp_FileObj = fopen(filename_str, "w+");
			fprintf(master_IbiResp_FileObj,"\n\n*************I3C Master IBI Response Report ***************** \n");
			choose_slave:
			printf("\n***********************************************************\n");
			printf("			I3C MASTER (%d)  \n",device_id);
			printf("***********************************************************\n");
			printf("Press \"y\" to initialize the I3C bus :");
			scanf("%s", &is_entdaa_first_cmd);
			if (is_entdaa_first_cmd[0] == 'y') 
			{
				// sizeToTransfer = 7;
				// sizeTransferred = 0;
				// for(index = 0; index < 7; index++){
					// outbuffer_daa[index] = outBuffer_DAA[index];
				// }
				// outbuffer_daa[5] = 0x01;
				// status = FTDI_SPI_ReadWrite(ftHandle, inBuffer, outbuffer_daa, sizeToTransfer, &sizeTransferred,
					// SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
					// SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
					// SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
				// APP_CHECK_STATUS(status);
				// printf(">>DAA Command sent\n");
				// daa_done=1;
				poll_mesg=0;
			}
		}
		if(device==2)
		{
			timestr[sprintf(timestr,"_%02d_%02d_%04d_%02d_%02d_%02d", thesystemtime.wDay, thesystemtime.wMonth, thesystemtime.wYear,thesystemtime.wHour,thesystemtime.wMinute,thesystemtime.wSecond)+1] = '\0';
			// printf("time: %s\n",timestr);
			filename_str_slave = (char*)malloc(strlen("slave_cmd_response")+strlen(timestr)+strlen(".log"));
			if(filename_str_slave != NULL) {
				filename_str_slave[0] = '\0';
				strcat(filename_str_slave,"slaver_cmd_response");
				strcat(filename_str_slave,timestr);
				strcat(filename_str_slave,".log");
			}
			slave_CmdResp_FileObj = fopen(filename_str_slave, "w+");
				fprintf(slave_CmdResp_FileObj,"\n\n*************I3C SLAVE Response Report***************** \n");
		}
		while(device==1)
		{
		
			if(config_done==0)
			{
				//Master configuration commands
				for(index = 0; index < 3; index++){
					slave_congig_pointer[index] = slave_config_data[index];
					}
				for(index = 0; index < 9; index++){
				if(index==0){slave_congig_pointer[1]=0x01;slave_congig_pointer[3]=PID_5;}
				if(index==1){slave_congig_pointer[1]=0x02;slave_congig_pointer[3]=PID_4;}
				if(index==2){slave_congig_pointer[1]=0x03;slave_congig_pointer[3]=PID_3;}
				if(index==3){slave_congig_pointer[1]=0x04;slave_congig_pointer[3]=0x00;}
				if(index==4){slave_congig_pointer[1]=0x05;slave_congig_pointer[3]=PID_1;}
				if(index==5){slave_congig_pointer[1]=0x06;slave_congig_pointer[3]=PID_0;}
				if(index==6){slave_congig_pointer[1]=0x07;slave_congig_pointer[4]=BCR_CONFIG_master;}
				if(index==7){slave_congig_pointer[1]=0x08;slave_congig_pointer[4]=DCR_CONFIG;}
				if(index==8){slave_congig_pointer[1]=0x14;slave_congig_pointer[3]=0x87;}
				sizeToTransfer = 4;
				sizeTransferred = 0;
				status = SPI_ReadWrite(ftHandle, inBuffer, slave_congig_pointer, sizeToTransfer, &sizeTransferred,
					SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
					SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
					SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
				APP_CHECK_STATUS(status);
				}
				config_done=1;
				//printf("	Master Configuration is Done\n");
			}
			while (device == 1)
			{
				if(poll_mesg==1)
				{
					printf("\n**********************************************************\n");
					printf("			I3C MASTER MENU(%d)  \n",device_id);
					printf("**********************************************************\n");
					printf("Your Options are:\n");
					printf("	1)Initialize I3C Bus \n");
					printf("	2) Print device characteristics table (valid after ENTDAA) \n");
					printf("	3) Print device address table (valid after initialization of I3C bus) \n");
					printf("	4) Broadcast CCC Messaging\n");
					printf("	5) Direct CCC Messaging\n");
					printf("	6) I3C Private Messaging Write\n");
					printf("	7) I3C Private Messaging Read\n");
					printf("	8) Generic Register Read/Write\n");
					printf("	9) I2C Legacy Messaging Write\n");
					printf("	10) I2C Legacy Messaging Read\n");
					printf("	11) SOFT RESET I3C master IP\n");
					printf("	12) Emit HDR EXIT for M3 error handling\n");
					printf("	13) Clear the LOG\n ");
					printf("	14) EXIT the APP\n>>: ");
					poll_mesg=0;
				}
				select = 0;
			
				if ( kbhit() || (is_entdaa_first_cmd[0] == 'y'))
				{
				//printf("got the Keyboard interrupt/n");
				if(is_entdaa_first_cmd[0] == 'y') {
				  select = 1;
				}
				else
				{
							scanf("%d", &select);
							printf(">>: ");
				}
				if( select == 1)//initialze the I3C bus
				{
				  fprintf(master_CmdResp_FileObj,"-----------------------------------------\n");
				  printf(">>Are there any slaves which can be allocated \n\t with dynamic address using SETDASA CCC?\n");
				  fprintf(master_CmdResp_FileObj,">>Are there any slaves which can be allocated \n\t with dynamic address using SETDASA CCC?\n");
				  printf(" Do you want to continue[y/n]: ");
				  fprintf(master_CmdResp_FileObj," Do you want to continue[y/n]:");
				  scanf("%s", &is_entdaa_first_cmd);
				  printf("\n");
				  fprintf(master_CmdResp_FileObj," You selected: %s \n",is_entdaa_first_cmd);
				  
				  if(is_entdaa_first_cmd[0] == 'y'){
					if(daa_done == 0){
					  for(index = 0; index < 6; index++){
						outbuffer_dccc[index] = outBuffer_DCCC_COMMAND[index];
					  }
					  while(is_entdaa_first_cmd[0] == 'y'){
						printf("	Enter the static address of I3C device	(hex) 0x:");
						scanf("%x", &slave_addr);
						fprintf(master_CmdResp_FileObj," static address :0x%02x\n",slave_addr);
						printf("\n");
						outbuffer_dccc[6] = slave_addr << 1;
						printf("	Enter the dynamic address to be assigned  (hex) 0x:");
						scanf("%x", &user_data);
						fprintf(master_CmdResp_FileObj," Dynamic address :0x%02x\n",user_data);
						printf("\n");
						outbuffer_dccc[2] =0x0c;outbuffer_dccc[4] = 0x87;outbuffer_dccc[7] = 0x01;sizeToTransfer = 9; outbuffer_dccc[8] = user_data<<1;
						outbuffer_dccc[0]=0x00;
						status = FTDI_SPI_ReadWrite(ftHandle, inBuffer, outbuffer_dccc, sizeToTransfer, &sizeTransferred,
						  SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
						  SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
						  SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
						APP_CHECK_STATUS(status);
						printf(">>SETDASA Command sent\n");
						fprintf(master_CmdResp_FileObj," -----------------------------------------\n");
						fprintf(master_CmdResp_FileObj," SPI_WRITE_DATA:");
						for(index=0;index<sizeToTransfer;index++){
							fprintf(master_CmdResp_FileObj,"8'h%02x,",outbuffer_dccc[index]);
							fprintf(master_CmdResp_FileObj,"\t");
						}
						fprintf(master_CmdResp_FileObj,"\n");
						fprintf(master_CmdResp_FileObj," >>SETDASA Command sent\n");
						fprintf(master_CmdResp_FileObj," -----------------------------------------\n");
						printf(" Press \"y\" if there is an another such device :");
						fprintf(master_CmdResp_FileObj," Press \"y\" if there is an another such device :\n");
						scanf("%s",&is_entdaa_first_cmd);
						fprintf(master_CmdResp_FileObj," YOU selected :%s\n",is_entdaa_first_cmd);
					  }
					} else {
					  printf("MIPI I3C specification does not allow \n\tSETDASA CCC to be issued on an initialized I3C bus\n");
					  printf("Please try again after issuing Broadcast CCC RSTDAA\n");
					  fprintf(master_CmdResp_FileObj," MIPI I3C specification does not allow \n\tSETDASA CCC to be issued on an initialized I3C bus\n Please try again after issuing Broadcast CCC RSTDAA\n");
					  poll_mesg=1;
					}
				  }
				  fprintf(master_CmdResp_FileObj," >>Issuing ENTDAA command now\n");
				  printf("Issuing ENTDAA command now\n");          
				  sizeToTransfer = 7;
				  sizeTransferred = 0;
				  for(index = 0; index < 7; index++){
					outbuffer_daa[index] = outBuffer_DAA[index];
				  }
				  outbuffer_daa[5] = 0x01;
				  status = FTDI_SPI_ReadWrite(ftHandle, inBuffer, outbuffer_daa, sizeToTransfer, &sizeTransferred,
					SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
					SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
					SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
				  APP_CHECK_STATUS(status);
				  printf(">>ENTDAA Command sent\n");
				  fprintf(master_CmdResp_FileObj," >>ENTDAA Command sent\n");
				  is_entdaa_first_cmd[0] = 'n';
				  daa_done=1;
				}
				else if (select == 2)//dct
				{
					if(dct_valid)
					{
						print_dct();
						poll_mesg=1;
					}
					else 
					{
						printf("I3C Bus is not initialized\n");
					}
					poll_mesg = 1;
					break;
				}
				else if (select == 3)//dat
				{
					if(dct_valid)
					{
						print_dat();
						poll_mesg=1;
					}
					else 
					{
						printf("I3C Bus is not initialized\n");
					}
					poll_mesg = 1;
					break;
				}
				else if (select == 4)//bccc
				{
					fprintf(master_CmdResp_FileObj," Selected Option: Broadcast CCC\n");
					printf(" Available Broadcast CCC commands are:\n");
					printf("	1.ENEC\n");
					printf("	2.DISEC\n");
					printf("	3.ENTAS0\n");
					printf("	4.RSTDAA\n");
					printf("	5.ENTDAA\n");
					printf("	6.SETMWL\n");
					printf("	7.SETMRL\n");
					printf("	8.Back to MAIN Master MENU\n");
					printf("	\n\n");
					// printf("Other Broadcast CCCs\n");
					// printf("	8.ENTAS1\n");
					// printf("	9.ENTAS2\n"); 
					// 	//printf("	10.ENTAS3\n");
					// printf("	11.DEFSLVS\n");
					// printf("	11.ENTTM\n");
					// printf("	12.ENTHDR0\n");
					// printf("	13.ENTHDR1\n");
					// printf("	14.ENTHDR2\n");
					// printf("	15.ENTHDR3\n");
					// printf("	16.ENTHDR4\n");
					// printf("	17.ENTHDR5\n");
					// printf("	18.ENTHDR6\n");
					// printf("	19.ENTHDR7\n");
					// 	//printf("	21.SETXTIME\n");
					// printf("	20.Back to MAIN Master MENU\n");
					printf("\tSelect the Broadcast CCC Command: ");
					scanf("%d", &ccc);
					if(ccc == 8){poll_mesg = 1;break;}
					if(ccc>8 || ccc < 1){printf("\n\n\tchoose valid Option");poll_mesg = 1;break;}
					 // if(ccc==21)
					 // {
						// system("cls");
						// poll_mesg = 1;
						// break;
					 // }
					if (ccc != 4 && ccc != 5 && ccc != 3)
					{
						if (ccc == 6 || ccc == 7)
						{
							printf("	Enter MSB of data length (hex)	0x:");
							scanf("%x", &user_data);
							fprintf(master_CmdResp_FileObj," Enter MSB of data length:0x%02x\n",user_data);
							printf("\n");
							printf("	Enter LSB of data length (hex)	0x:");
							scanf("%x", &user_data_1);
							fprintf(master_CmdResp_FileObj," Enter LSB of data length:0x%02x\n",user_data_1);
							printf("\n");
						}
						else
						{
							printf("	Enter the data to be written (hex)	0x:");
							scanf("%x", &user_data);
							fprintf(master_CmdResp_FileObj," Entered data to be written:0x%02x\n",user_data);
							printf("\n");
						}
						if (ccc == 7) 
						{
								
								printf("	Press \"y\" if we you want to configure IBI payload size\n>>: ");
								scanf("%s", &setmrl_byte);
								fprintf(master_CmdResp_FileObj," Press \"y\" if we you want to configure IBI payload size\n>>:",setmrl_byte[0]);
								if(setmrl_byte[0] == 'y')
								{
									printf("	Enter the data to be written (hex) 0x:");
									scanf("%x", &user_data_2);
									fprintf(master_CmdResp_FileObj," Entered data to be written:0x%02x\n",user_data_2);
									
									fprintf(master_CmdResp_FileObj," IBI payload BCCC SETMRL: %02x ",user_data_2);
									for(index = 0; index < 3; index++)
									{
									outbuffer_ibi_payload_size_arrary[index] = outbuffer_ibi_payload_size[index];
									}
									
									if(user_data_2 > master_ibi_payload_max)
									{
										master_ibi_payload_max=user_data_2;
										outbuffer_ibi_payload_size_arrary[3]=master_ibi_payload_max;
									}
									else 
									{
										outbuffer_ibi_payload_size_arrary[3]=master_ibi_payload_max;
									}
									sizeToTransfer = 4;
									sizeTransferred = 0;
									status = SPI_ReadWrite(ftHandle, inBuffer, outbuffer_ibi_payload_size_arrary, sizeToTransfer, &sizeTransferred,
									SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
									SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
									SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
									APP_CHECK_STATUS(status);
									printf("\n");
									sizeToTransfer = 10;
									outbuffer_bccc[9] = user_data_2;
									outbuffer_bccc[6] =0x03; 
								} else {
									sizeToTransfer = 9;
									outbuffer_bccc[6] =0x02; 
								}
						}
					}
					sizeTransferred = 0;
					for(index = 0; index < 6; index++){
						outbuffer_bccc[index] = outBuffer_BCCC_COMMAND[index];
					}
					fprintf(master_CmdResp_FileObj," -----------------------------------------\n");
					if (ccc == 1) { fprintf(master_CmdResp_FileObj," BCCC ENEC Command\n");outbuffer_bccc[2] =0x0A;outbuffer_bccc[4] =0x00; outbuffer_bccc[6] =0x01;outbuffer_bccc[7] = user_data; sizeToTransfer = 8; }
					else if (ccc == 2) {fprintf(master_CmdResp_FileObj," BCCC DISEC Command\n");outbuffer_bccc[2] =0x0A; outbuffer_bccc[4] =0x01;  outbuffer_bccc[6] =0x01;outbuffer_bccc[7] = user_data;sizeToTransfer = 8; }
					else if (ccc == 3) { fprintf(master_CmdResp_FileObj," BCCC ENTAS0 Command\n");outbuffer_bccc[2] =0x08;outbuffer_bccc[4] =0x02; outbuffer_bccc[6] =0x00;sizeToTransfer = 7;}
					else if (ccc == 4) {fprintf(master_CmdResp_FileObj," BCCC RSTDAA Command\n"); outbuffer_bccc[2] =0x08;outbuffer_bccc[4] =0x06; outbuffer_bccc[6] =0x00;sizeToTransfer = 7; daa_done = 0;}
					else if (ccc == 5) {fprintf(master_CmdResp_FileObj," BCCC ENTDAA Command\n"); outbuffer_bccc[2] =0x08;outbuffer_bccc[4] =0x07; outbuffer_bccc[6] =0x00;sizeToTransfer = 7; daa_done = 1;}
					else if (ccc == 6) { fprintf(master_CmdResp_FileObj," BCCC SETMWL Command\n");outbuffer_bccc[2] =0x0c;outbuffer_bccc[4] =0x09; outbuffer_bccc[6] =0x02; outbuffer_bccc[7] = user_data; outbuffer_bccc[8] = user_data_1;sizeToTransfer = 9; }
					else if (ccc == 7) {fprintf(master_CmdResp_FileObj," BCCC SETMRL Command\n"); outbuffer_bccc[2] =0x0e;outbuffer_bccc[4] =0x0a; outbuffer_bccc[7] = user_data; outbuffer_bccc[8] = user_data_1;}
					// else if (ccc == 8) { fprintf(master_CmdResp_FileObj," BCCC ENTAS1 Command\n");outbuffer_bccc[2] =0x0c;outbuffer_bccc[4] =0x03; outbuffer_bccc[6] =0x02; outbuffer_bccc[7] = user_data; outbuffer_bccc[8] = user_data_1;sizeToTransfer = 9; }
					// else if (ccc == 9) { fprintf(master_CmdResp_FileObj," BCCC ENTAS2 Command\n");outbuffer_bccc[2] =0x0d;outbuffer_bccc[4] =0x03; outbuffer_bccc[6] =0x02; outbuffer_bccc[7] = user_data; outbuffer_bccc[8] = user_data_1;sizeToTransfer = 9; }
					// else if (ccc == 10) { fprintf(master_CmdResp_FileObj," BCCC ENTAS3 Command\n");outbuffer_bccc[2] =0x0e;outbuffer_bccc[4] =0x03; outbuffer_bccc[6] =0x02; outbuffer_bccc[7] = user_data; outbuffer_bccc[8] = user_data_1;sizeToTransfer = 9; }
					
					// //else if (ccc == 11) { outbuffer_bccc[2] =0x0A;outbuffer_bccc[4] =0x0b; outbuffer_bccc[6] =0x01;outbuffer_bccc[7] = user_data;sizeToTransfer = 8;  }
					// else if (ccc == 12) { fprintf(master_CmdResp_FileObj," BCCC ENTHDR0 Command\n");outbuffer_bccc[2] =0x08;outbuffer_bccc[4] =0x20; outbuffer_bccc[6] =0x00;sizeToTransfer = 7;}
					// else if (ccc == 13) {fprintf(master_CmdResp_FileObj," BCCC ENTHDR1 Command\n");outbuffer_bccc[2] =0x08;outbuffer_bccc[4]=0x21; outbuffer_bccc[6] =0x00;sizeToTransfer = 7;}
					// else if (ccc == 14) {fprintf(master_CmdResp_FileObj," BCCC ENTHDR2 Command\n");outbuffer_bccc[2] =0x08;outbuffer_bccc[4]=0x22; outbuffer_bccc[6] =0x00;sizeToTransfer = 7;}
					// else if (ccc == 15) {fprintf(master_CmdResp_FileObj," BCCC ENTHDR3 Command\n");outbuffer_bccc[2] =0x08;outbuffer_bccc[4]=0x23; outbuffer_bccc[6] =0x00;sizeToTransfer = 7;}
					// else if (ccc == 16) {fprintf(master_CmdResp_FileObj," BCCC ENTHDR4 Command\n");outbuffer_bccc[2] =0x08;outbuffer_bccc[4]=0x24; outbuffer_bccc[6] =0x00;sizeToTransfer = 7;}
					// else if (ccc == 17) {fprintf(master_CmdResp_FileObj," BCCC ENTHDR5 Command\n");outbuffer_bccc[2] =0x08;outbuffer_bccc[4]=0x25; outbuffer_bccc[6] =0x00;sizeToTransfer = 7;}
					// else if (ccc == 18) {fprintf(master_CmdResp_FileObj," BCCC ENTHDR6 Command\n");outbuffer_bccc[2] =0x08;outbuffer_bccc[4]=0x26; outbuffer_bccc[6] =0x00;sizeToTransfer = 7;}
					// else if (ccc == 19) {fprintf(master_CmdResp_FileObj," BCCC ENTHDR7 Command\n");outbuffer_bccc[2] =0x08;outbuffer_bccc[4]=0x27; outbuffer_bccc[6] =0x00;sizeToTransfer = 7;}
					
					if(ccc==5)
					{
						printf("\tEnter the dynamic address of the device which-\n\twins the first Dynamic Address Arbitration 7'h: ");
						scanf("%x",&user_data);
						fprintf(master_CmdResp_FileObj,"Selected Dynamic Address:7'h%02x\n",user_data);
						printf("\n");
						slave_congig_pointer[1]=0x17;slave_congig_pointer[3]=user_data;
						sizeToTransfer = 4;
						sizeTransferred = 0;
						status = SPI_ReadWrite(ftHandle, inBuffer, slave_congig_pointer, sizeToTransfer, &sizeTransferred,
							SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
						APP_CHECK_STATUS(status);
						
						fprintf(master_CmdResp_FileObj," SPI_WRITE_DATA for CONFIG :");
						for(index=0;index<sizeToTransfer;index++){
							fprintf(master_CmdResp_FileObj,"8'h%02x,",slave_congig_pointer[index]);
							fprintf(master_CmdResp_FileObj,"\t");
						}
						fprintf(master_CmdResp_FileObj,"\n");
						fprintf(master_CmdResp_FileObj," Config. command Sent\n");
						sizeToTransfer = 6;
					}
					
					outbuffer_bccc[0]=0x00;
					status = FTDI_SPI_ReadWrite(ftHandle, inBuffer, outbuffer_bccc, sizeToTransfer,&sizeTransferred,
						SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
						SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
						SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
					APP_CHECK_STATUS(status);
					
					fprintf(master_CmdResp_FileObj," SPI_WRITE_DATA:");
						for(index=0;index<sizeToTransfer;index++){
							fprintf(master_CmdResp_FileObj,"8'h%02x,",outbuffer_bccc[index]);
							fprintf(master_CmdResp_FileObj,"\t");
						}
						fprintf(master_CmdResp_FileObj,"\n");
					// poll_mesg = 1;
					printf("	Broadcast CCC command Sent\n");
					fprintf(master_CmdResp_FileObj," Broadcast CCC command Sent\n");
					fprintf(master_CmdResp_FileObj,"-----------------------------------------\n");
					break;	
					/*	else{
						printf("Invalid Option..Choose again\n");
						poll_mesg = 1;
						break;
						}*/
				}
				else if (select == 5)//dccc
				{
					fprintf(master_CmdResp_FileObj," Direct CCC \n");
					for(index = 0; index < 6; index++){
						outbuffer_dccc[index] = outBuffer_DCCC_COMMAND[index];
					}
					/*printf("	Entered into Direct CCC\n");
							printf("	Enter the Slave address	(hex) 0x:");
							scanf("%x", &slave_addr);
							fprintf(master_CmdResp_FileObj," Selected Slave Addr:0x%02x\n",slave_addr);
							target_slave_addr=slave_addr;
							printf("\n");
							*/
							
						/*	printf("Select the Slave:\n1.Slave-%x	\n2.Slave-%x", slave_0_addr, slave_1_addr);
							printf("-->:");
							scanf("%d", &select);
							if (select == 1) { slave_addr = slave_0_addr; }
							else if (select == 2) { slave_addr = slave_1_addr; }	
						*/	
							printf(" Available Direct CCC commands are:\n");
							printf("	1.ENEC\n");
							printf("	2.DISEC\n");
							printf("	3.ENTAS0\n");
							printf("	4.RSTDAA\n");
							printf("	5.SETNEWDA\n");
							printf("	6.SETMWL\n");
							printf("	7.SETMRL\n");
							printf("	8.GETMWL\n");
							printf("	9.GETMRL\n");
							printf("	10.GETPID\n");
							printf("	11.GETBCR\n");
							printf("	12.GETDCR\n");
							printf("	13.SETDASA\n");
							printf("	14.Back to main master menu?\n");
							printf("	\n\n");
							// printf("	Other Direct CCCs:\n");
							// printf("	13.GETSTATUS\n");
							// printf("	14.GETACCMST\n");
							// printf("	15.GETMXDS\n");
							// printf("	16.GETHDRCAP\n");
							// printf("	17.GETXTIME\n");
							// printf("	18.ENTAS1\n");
							// printf("	19.ENTAS2\n");
							// printf("	20.ENTAS3\n");
							// printf("	21.SETDASA\n");
							// printf("	22.SETBRGTGT\n");
							// printf("	23.SETXTIME\n");
							// printf("	24.Go back from this options:\n");
							printf("\tSelect the Direct CCC Command: ");
							scanf("%d", &ccc);
							printf("\n");
							if(ccc==13)
								printf("	Enter the Static address(hex) 0x:");
							else {
								printf("	Enter the Slave address(hex) 0x:");
							}	
							scanf("%x", &slave_addr);
							fprintf(master_CmdResp_FileObj," Selected Slave Addr:0x%02x\n",slave_addr);
							target_slave_addr=slave_addr;
							printf("\n");
							outbuffer_dccc[6] = slave_addr << 1;
							if((ccc >= 8) && (ccc <= 12)){
								outbuffer_dccc[6] |= 0x01; //for read
							}
							if(ccc==14){poll_mesg = 1;break;}
							if(ccc > 14 || ccc < 1){poll_mesg = 1;printf(" Please choose valid option\n");break;}
							//if(ccc==24){poll_mesg = 1;break;}
							if(ccc == 1 || ccc == 2/* || ccc==17|| ccc==16 */)
							{
								printf("	Enter the data to be written (hex) 0x:");
								scanf("%x", &user_data);
								fprintf(master_CmdResp_FileObj,"Entered data:0x%02x\n",user_data);
								printf("\n");
							}
							if(ccc==6 || ccc==7/*|| ccc==17*/)
							{
								printf("	Enter MSB of data length   (hex) 0x:");
								scanf("%x", &user_data);
								fprintf(master_CmdResp_FileObj,"Entered MSB of data length:0x%02x\n",user_data);
								printf("\n");
								printf("	Enter of LSB data length   (hex) 0x:");
								scanf("%x", &user_data_1);
								fprintf(master_CmdResp_FileObj,"Entered LSB of data length:0x%02x\n",user_data_1);
								printf("\n");
							}	
							if(ccc==5)
							{
								printf("	Enter the new dynamic address to be allocated 7'h:");
								scanf("%x", &user_data);
								user_data = user_data<<1;
								setnewda_dyna_addr_by_user=user_data;
								fprintf(master_CmdResp_FileObj,"Entered new dynamic_Addr:0x%02x\n",setnewda_dyna_addr_by_user);
								printf("\n");
							}
							if(ccc==13)
							{
							printf("	Enter the dynamic address to be allocated 7'h:");
								scanf("%x", &user_data);
								user_data = user_data<<1;
								printf("\n");
							}
							/*if(ccc==17)
							{
							printf("	Enter the data to be written	(hex) 0x:");
								scanf("%x", &user_data_2);
								printf("\n");
							}*/
							if (ccc == 7) 
							{
								printf("Press 'y' if we you want to configure IBI payload size\n>>: ");
								scanf("%s", &setmrl_byte);
								fprintf(master_CmdResp_FileObj,"Press 'y' if we you want to configure IBI payload size\n>>:",setmrl_byte[0]);
								if(setmrl_byte[0] == 'y')
								{
									printf("	Enter the data to be written	(hex) 0x:");
									scanf("%x", &user_data_2);
									fprintf(master_CmdResp_FileObj,"Entered the data to be written for ibi_payload :0x%02x\n",user_data_2);
									fprintf(master_CmdResp_FileObj," IBI payload DCCC SETMRL: %02x ",user_data_2);
									for(index = 0; index < 3; index++)
									{
										outbuffer_ibi_payload_size_arrary[index] = outbuffer_ibi_payload_size[index];
									}
									if(user_data_2 > master_ibi_payload_max)
									{
										master_ibi_payload_max=user_data_2;
										outbuffer_ibi_payload_size_arrary[3]=master_ibi_payload_max;
									}
									else 
									{
										outbuffer_ibi_payload_size_arrary[3]=master_ibi_payload_max;
									}
									sizeToTransfer = 4;
									sizeTransferred = 0;
									status = SPI_ReadWrite(ftHandle, inBuffer, outbuffer_ibi_payload_size_arrary, sizeToTransfer, &sizeTransferred,
									SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
									SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
									SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
									APP_CHECK_STATUS(status);		
									printf("\n");
									sizeToTransfer = 11;
									outbuffer_dccc[10] = user_data_2;
									outbuffer_dccc[7] = 0x03;
								} else {
									sizeToTransfer = 10;
									outbuffer_dccc[7] = 0x02;
								}
							}
							
							//uint8 outBuffer_DCCC_COMMAND[] = {0x00,0x10,0x00,0x1b,0x80,0x01};//0-msb,1-lsb,2-cmd descrptor,3-cmd, 4-ccc , 5-no.of.slaves,6-slave address<<1 |0x01 7-data length
							sizeTransferred = 0;
							fprintf(master_CmdResp_FileObj," -----------------------------------------\n");
							if (ccc == 1)  { fprintf(master_CmdResp_FileObj," DCCC ENEC Command\n");outbuffer_dccc[2] =0x0c;outbuffer_dccc[4] = 0x80;outbuffer_dccc[7] = 0x01;sizeToTransfer = 9; outbuffer_dccc[8] = user_data; }
							else if (ccc == 2) { fprintf(master_CmdResp_FileObj," DCCC DISEC Command\n");outbuffer_dccc[2] =0x0c;outbuffer_dccc[4] = 0x81;outbuffer_dccc[7] = 0x01;sizeToTransfer = 9; outbuffer_dccc[8] = user_data; }
							else if (ccc == 3) { fprintf(master_CmdResp_FileObj," DCCC ENTAS0 Command\n");outbuffer_dccc[2] =0x0a;outbuffer_dccc[4] = 0x82;outbuffer_dccc[7] = 0x00;sizeToTransfer = 8;}
							else if (ccc == 4) { fprintf(master_CmdResp_FileObj," DCCC RSTDAA Command\n");outbuffer_dccc[2] =0x0a;outbuffer_dccc[4] = 0x86;outbuffer_dccc[7] = 0x00;sizeToTransfer = 8;}
							else if (ccc == 5) { fprintf(master_CmdResp_FileObj," DCCC SETNEWDA Command\n");outbuffer_dccc[2] =0x0c;outbuffer_dccc[4] = 0x88;outbuffer_dccc[7] = 0x01;sizeToTransfer = 9; outbuffer_dccc[8] = user_data; }
							else if (ccc == 6) { fprintf(master_CmdResp_FileObj," DCCC SETMWL Command\n");outbuffer_dccc[2] =0x0e;outbuffer_dccc[4] = 0x89;outbuffer_dccc[7] = 0x02;sizeToTransfer = 10; outbuffer_dccc[8] = user_data; outbuffer_dccc[9] = user_data_1; }
							else if (ccc == 7) { fprintf(master_CmdResp_FileObj," DCCC SETMRL Command\n");outbuffer_dccc[2] =0x10;outbuffer_dccc[4] = 0x8A;outbuffer_dccc[8] = user_data;outbuffer_dccc[9] = user_data_1;}
							else if (ccc == 8) { fprintf(master_CmdResp_FileObj," DCCC GETMWL Command\n");outbuffer_dccc[2] =0x0a;outbuffer_dccc[4] = 0x8B;outbuffer_dccc[7] = 0x02;sizeToTransfer = 8;}
							else if (ccc == 9) { fprintf(master_CmdResp_FileObj," DCCC GETMRL Command\n");outbuffer_dccc[2] =0x0a;outbuffer_dccc[4] = 0x8C;outbuffer_dccc[7] = 0x03;sizeToTransfer = 8;}
							else if (ccc == 10){ fprintf(master_CmdResp_FileObj," DCCC GETPID Command\n");outbuffer_dccc[2] =0x0a;outbuffer_dccc[4] = 0x8D;outbuffer_dccc[7] = 0x06;sizeToTransfer = 8;}
							else if (ccc == 11) { fprintf(master_CmdResp_FileObj," DCCC GETBCR Command\n");outbuffer_dccc[2] =0x0a;outbuffer_dccc[4] = 0x8E;outbuffer_dccc[7] = 0x01;sizeToTransfer = 8;}
							else if (ccc == 12) { fprintf(master_CmdResp_FileObj," DCCC GETDCR Command\n");outbuffer_dccc[2] =0x0a;outbuffer_dccc[4] = 0x8F;outbuffer_dccc[7] = 0x01;sizeToTransfer = 8;}
							/*
							else if (ccc == 13) { fprintf(master_CmdResp_FileObj," DCCC GETSTATUS Command\n");outbuffer_dccc[2] =0x0a;outbuffer_dccc[4] = 0x90;outbuffer_dccc[7] = 0x01;sizeToTransfer = 8;}
							else if (ccc == 14) { fprintf(master_CmdResp_FileObj," DCCC GETACCMST Command\n");outbuffer_dccc[2] =0x0a;outbuffer_dccc[4] = 0x91;outbuffer_dccc[7] = 0x01;sizeToTransfer = 8;}
							else if (ccc == 6) { fprintf(master_CmdResp_FileObj," DCCC SETMWL Command\n");outbuffer_dccc[2] =0x0e;outbuffer_dccc[4] = 0x89;outbuffer_dccc[7] = 0x02;sizeToTransfer = 10; outbuffer_dccc[8] = user_data; outbuffer_dccc[9] = user_data_1; }
							else if (ccc == 8) { fprintf(master_CmdResp_FileObj," DCCC GETMWL Command\n");outbuffer_dccc[2] =0x0a;outbuffer_dccc[4] = 0x8B;outbuffer_dccc[7] = 0x02;sizeToTransfer = 8;}
							else if (ccc == 7) { fprintf(master_CmdResp_FileObj," DCCC SETMRL Command\n");outbuffer_dccc[2] =0x10;outbuffer_dccc[4] = 0x8A;outbuffer_dccc[8] = user_data;outbuffer_dccc[9] = user_data_1;}
							else if (ccc == 9) { fprintf(master_CmdResp_FileObj," DCCC GETMRL Command\n");outbuffer_dccc[2] =0x0a;outbuffer_dccc[4] = 0x8C;outbuffer_dccc[7] = 0x03;sizeToTransfer = 8;}
							else if (ccc == 10){ fprintf(master_CmdResp_FileObj," DCCC GETPID Command\n");outbuffer_dccc[2] =0x0a;outbuffer_dccc[4] = 0x8D;outbuffer_dccc[7] = 0x06;sizeToTransfer = 8;}
							else if (ccc == 16){ fprintf(master_CmdResp_FileObj," DCCC GETHDRCAP Command\n");outbuffer_dccc[2] =0x0a;outbuffer_dccc[4] = 0x95;outbuffer_dccc[7] = 0x00;sizeToTransfer = 8;}
							else if (ccc == 17){ fprintf(master_CmdResp_FileObj," DCCC GETXTIME Command\n");outbuffer_dccc[2] =0x0a;outbuffer_dccc[4] = 0x99;outbuffer_dccc[7] = 0x00;sizeToTransfer = 8;}
							else if (ccc == 18){ fprintf(master_CmdResp_FileObj," DCCC ENTAS1 Command\n");outbuffer_dccc[2] =0x0a;outbuffer_dccc[4] = 0x83;outbuffer_dccc[7] = 0x00;sizeToTransfer = 8;}
							else if (ccc == 19){ fprintf(master_CmdResp_FileObj," DCCC ENTAS2 Command\n");outbuffer_dccc[2] =0x0a;outbuffer_dccc[4] = 0x84;outbuffer_dccc[7] = 0x00;sizeToTransfer = 8;}
							else if (ccc == 20){ fprintf(master_CmdResp_FileObj," DCCC ENTAS3 Command\n");outbuffer_dccc[2] =0x0a;outbuffer_dccc[4] = 0x85;outbuffer_dccc[7] = 0x00;sizeToTransfer = 8;}
							else if (ccc == 21) {
							
							if(daa_done == 0)
							{
								fprintf(master_CmdResp_FileObj," DCCC SETDASA Command\n");
								  outbuffer_dccc[2] =0x0c;outbuffer_dccc[4] = 0x87;outbuffer_dccc[7] = 0x01;sizeToTransfer = 9; outbuffer_dccc[8] = user_data; 
								}
								else {
								  printf("MIPI I3C specification does not allow SETDASA CCC to be issued on an initialized I3C bus\n");
								  printf("Please try again after issuing Broadcast CCC RSTDAA\n");
								  fprintf(master_CmdResp_FileObj," MIPI I3C specification does not allow SETDASA CCC to be issued on an initialized I3C bus\n Please try again after issuing Broadcast CCC RSTDAA\n");	
								}
							}
							else if(ccc==15){printf("To be defined\n");}
							else if(ccc==23){printf("To be defined\n");}
							*/
							else if (ccc == 13) //dccc setdasa //00 10 0C 1B 87 01 0E 01 12
							{
								if(daa_done == 0)
								{
									fprintf(master_CmdResp_FileObj," DCCC SETDASA Command\n");
									outbuffer_dccc[2] =0x0c;outbuffer_dccc[4] = 0x87;outbuffer_dccc[7] = 0x01;sizeToTransfer = 9; outbuffer_dccc[8] = user_data; 
								}
								else 
								{
									printf("MIPI I3C specification does not allow SETDASA CCC to be issued on an initialized I3C bus\n");
									printf("Please try again after issuing Broadcast CCC RSTDAA\n");
									fprintf(master_CmdResp_FileObj," MIPI I3C specification does not allow SETDASA CCC to be issued on an initialized I3C bus\n Please try again after issuing Broadcast CCC RSTDAA\n");	
									poll_mesg=1;
									break;
								}
							}
							outbuffer_dccc[0]=0x00;
							status = FTDI_SPI_ReadWrite(ftHandle, inBuffer, outbuffer_dccc, sizeToTransfer, &sizeTransferred,
								SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
								SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
								SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
							APP_CHECK_STATUS(status);							
							fprintf(master_CmdResp_FileObj," SPI_WRITE_DATA:");
							for(index=0;index<sizeToTransfer;index++){
							fprintf(master_CmdResp_FileObj,"8'h%02x,",outbuffer_dccc[index]);
							fprintf(master_CmdResp_FileObj,"\t");
						}
						fprintf(master_CmdResp_FileObj,"\n");
						// poll_mesg = 1;
						printf("	Master sent Direct CCC command to slave\n");
						fprintf(master_CmdResp_FileObj," Master sent Direct CCC command to slave \n");
						fprintf(master_CmdResp_FileObj," -----------------------------------------\n");
						break;
						/*else
						{
							printf("Invalid Option..Choose again\n");
							poll_mesg = 1;
							break;
						}*/
				}
				else if (select == 6) //private write
				{
					char rand_data;
					unsigned char str[128] = {0};
					const char *hex_digits = "0123456789ABCDEF";
					int n,max;
					fprintf(master_CmdResp_FileObj," -----------------------------------------\n");
					fprintf(master_CmdResp_FileObj," Private Write\n");
					rw_index = 4;
					printf("	Enter the Slave address	7'h:");
					scanf("%x", &slave_addr);
					fprintf(master_CmdResp_FileObj," Selected Slave_Addr:0x%02x\n",slave_addr);
					printf("\n");
					printf("	Enter how many bytes you want to write to slave(1-124): ");
					scanf("%d", &private_write_len);
					fprintf(master_CmdResp_FileObj," Selected Data Length:%d\n",private_write_len);
					printf("\n");
					if(private_write_len > 125)
					{
						printf("	Not supported more than 125\n");
						poll_mesg=1;
						break;
					}
					/*
					choose_sw:
					printf("Select the Slave:\n1.Slave-%x	\n2.Slave-%x", slave_0_addr, slave_1_addr);
					printf("-->:");
					scanf("%d", &select);
					if(select!=(1||2))
					{
					printf("Enter Valid Option:");
					goto choose_sw;
					}
					if (select == 1) { slave_addr = slave_0_addr; }
					else if (select == 2) { slave_addr = slave_1_addr; }
					*/
					
					for(index = 0; index < 4; index++){
						outbuffer_private[index] = outbuffer_private_hdcd[index];
					}
					//outbuffer_private = outbuffer_private_hdcd;
					outbuffer_private[2] = (private_write_len+3) << 1 | 0x00;
					outbuffer_private[rw_index++] = slave_addr << 1 | 0x00;//write
					outbuffer_private[rw_index++] = private_write_len;//write
					
					printf("	Do you want send random data automatically ?\n\n\t[y/n]:",rand_data);
					scanf("\n%c",&rand_data);
					srand ( time(NULL) );
					if(rand_data == 'Y' || rand_data == 'y')
					{
						for(i = 0; i < private_write_len; i++) 
						{
							sprintf(str + i, "%x", rand() % 16);
							outbuffer_private[rw_index++]=str[i];
						}
					}
					else{
						for (rw_loop = 0; rw_loop < private_write_len; rw_loop++)
						{
							printf("	Please enter data[%03d]	(hex) 0x:", rw_loop);
							scanf("%x", &rw_data);
							outbuffer_private[rw_index++] = rw_data;
							printf("\n");
						}
					}	
					sizeToTransfer = 6 + private_write_len;
					sizeTransferred = 0;
					status = FTDI_SPI_ReadWrite(ftHandle, inBuffer, outbuffer_private, sizeToTransfer, &sizeTransferred,
						SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
						SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
						SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
					APP_CHECK_STATUS(status);
					// poll_mesg = 1;
					fprintf(master_CmdResp_FileObj," SPI_WRITE_DATA:");
					for(index=0;index<sizeToTransfer;index++){
						fprintf(master_CmdResp_FileObj,"8'h%02x,",outbuffer_private[index]);
						fprintf(master_CmdResp_FileObj,"\t");
					}
					fprintf(master_CmdResp_FileObj,"\n Private write command Sent\n");
					fprintf(master_CmdResp_FileObj," -----------------------------------------\n");
					printf("	Private write command Sent\n");
					break;
				}
				else if (select == 7) //private read
				{
					fprintf(master_CmdResp_FileObj," -----------------------------------------\n");
					fprintf(master_CmdResp_FileObj," Private Read\n");
					rw_index = 4;
					printf("	Enter the I3C Slave address	7'h:");
					scanf("%x", &slave_addr);
					fprintf(master_CmdResp_FileObj," Selected Slave_Addr:0x%02x\n",slave_addr);
					printf("\n");
					printf("	Enter how many bytes you want to Read from slave(1-124): ");
					scanf("%d", &private_read_len);
					fprintf(master_CmdResp_FileObj," Selected Data Length:%d\n",private_read_len);
					printf("\n");
					if(private_read_len > 128)
					{
						printf("	Not supported more than 128\n");
						poll_mesg=1;
						break;
					}
					
					/*choose_sr:
					printf("Select the Slave:\n1.Slave-%x	\n2.Slave-%x", slave_0_addr, slave_1_addr);
					printf("-->:");
					scanf("%d", &select);
					if(select==1||select==2)
					{
						if (select == 1) { slave_addr = slave_0_addr; }
						else if (select == 2) { slave_addr = slave_1_addr; }
						
						for(index = 0; index < 4; index++){
							outbuffer_private[index] = outbuffer_private_hdcd[index];
						}
						outbuffer_private[rw_index++] = slave_addr << 1 | 0x01;//read
						outbuffer_private[rw_index++] = private_read_len ;
						sizeToTransfer = 6;
						sizeTransferred = 0;
						status = SPI_ReadWrite(ftHandle, inBuffer, outbuffer_private, sizeToTransfer, &sizeTransferred,
							SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
						APP_CHECK_STATUS(status);
						poll_mesg = 1;
						break;
					}
					else{
					printf("Please Enter Valid Option (1 or 2)");
					goto choose_sr;
					}
					*/
					for(index = 0; index < 4; index++){
						outbuffer_private[index] = outbuffer_private_hdcd[index];
					}
					outbuffer_private[2] = 0x06;
					outbuffer_private[rw_index++] = slave_addr << 1 | 0x01;//read
					outbuffer_private[rw_index++] = private_read_len ;
					sizeToTransfer = 6;
					sizeTransferred = 0;
					status = FTDI_SPI_ReadWrite(ftHandle, inBuffer, outbuffer_private, sizeToTransfer, &sizeTransferred,
						SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
						SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
						SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
					APP_CHECK_STATUS(status);
					// poll_mesg = 1;
					fprintf(master_CmdResp_FileObj," SPI_WRITE_DATA:");
					for(index=0;index<sizeToTransfer;index++){
						fprintf(master_CmdResp_FileObj,"8'h%02x,",outbuffer_private[index]);
						fprintf(master_CmdResp_FileObj,"\t");
					}
					fprintf(master_CmdResp_FileObj,"\n Private Read command Sent\n");
					fprintf(master_CmdResp_FileObj,"-----------------------------------------\n");
					printf("	Private Read command Sent\n");
					break;
				}
				else if (select == 8)//generic RW
				{
					choose_grw:
							printf(" Choose SPI:\n	1.Read\n	2.write\n 3.Go back from this menu\n Select:");
							scanf("%d", &rw);
							printf("\n");
							if(rw==3){
								 poll_mesg = 1;
								break;
							}
							if(rw ==1 || rw ==2)
							{	fprintf(master_CmdResp_FileObj,"-----------------------------------------\n");
								if (rw == 2)
								{
									fprintf(master_CmdResp_FileObj," >> Generic Write\n");
									rw_index = 0;
									printf("	Enter MSB address	(hex) 0x:");
									scanf("%x", &rw_data);
									fprintf(master_CmdResp_FileObj," >>MSB address:0x%02x\n",rw_data);
									printf("\n");
									outBuffer_rw[rw_index++] = rw_data;
									printf("	Enter LSB address	(hex) 0x:");
									scanf("%x", &rw_data);
									fprintf(master_CmdResp_FileObj," >>LSB address:0x%02x\n",rw_data);
									printf("\n");
									outBuffer_rw[rw_index++] = rw_data;
									printf("	Enter how many bytes of data you want to Write(0-124)	:");
									scanf("%d", &rw_len);
									if(rw_len > 125)
									{
										printf("	Not supported more than 128\n");
										poll_mesg=1;
										break;
									}
									//hex_var=rw_len;
									fprintf(master_CmdResp_FileObj," >>Data length:%d\n",rw_len);
									printf("\n");
									// outBuffer_rw[rw_index++] = 0x00;
									outBuffer_rw[rw_index++] = (rw_len << 1) | 0x00;//write command lsb bit:0
									for (rw_loop = 0; rw_loop < rw_len; rw_loop++)
									{
										printf("	Please enter data[%03d]	(hex) 0x:", rw_loop);
										scanf("%x", &rw_data);
										outBuffer_rw[rw_index++] = rw_data;
										printf("\n");
									}
									sizeToTransfer = rw_len + 3;
									sizeTransferred = 0;
									status = FTDI_SPI_ReadWrite(ftHandle, inBuffer, outBuffer_rw, sizeToTransfer, &sizeTransferred,
										SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
										SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
										SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
									APP_CHECK_STATUS(status);
									printf("Debug SPI Write Command Sent.\n");
									fprintf(master_CmdResp_FileObj,"SPI Write Command\n");
									for(index=0;index<sizeToTransfer;index++){
										fprintf(master_CmdResp_FileObj,"8'h%02x,",outBuffer_rw[index]);
										fprintf(master_CmdResp_FileObj,"\t");
									}
							fprintf(master_CmdResp_FileObj,"\n");
									printf("Generic SPI Write Command Sent.   \n");
									poll_mesg = 1;
									break;
								}
								else if (rw == 1)
								{
									fprintf(master_CmdResp_FileObj," >> Generic Read\n");
									rw_index = 0;
									printf("	Enter MSB address (hex) 0x:");
									scanf("%x", &rw_data);
									fprintf(master_CmdResp_FileObj," >>MSB address:0x%02x\n",rw_data);
									printf("\n");
									outBuffer_rw[rw_index++] = rw_data;
									printf("	Enter LSB address (hex) 0x:");
									scanf("%x", &rw_data);
									fprintf(master_CmdResp_FileObj," >>LSB address:0x%02x\n",rw_data);
									printf("\n");
									outBuffer_rw[rw_index++] = rw_data;
									printf("	Enter how many bytes of data you want to Read(1-124):");
									scanf("%d", &rw_len);
									fprintf(master_CmdResp_FileObj," >>Data length:%d\n",rw_len);
									printf("\n");
									if(rw_len > 125)
									{
										printf("	Not supported more than 128\n");
										poll_mesg=1;
										break;
									}
									outBuffer_rw[rw_index++] = (rw_len << 1) | 0x01;//write command lsb:1
									sizeToTransfer = rw_len + 4;//extra dummy for read
									sizeTransferred = 0;
									status = FTDI_SPI_ReadWrite(ftHandle, inBuffer, outBuffer_rw, sizeToTransfer, &sizeTransferred,
										SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
										SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
										SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
									APP_CHECK_STATUS(status)
										printf("Received Data:\n");
									for (l = 4; l < (sizeToTransfer); l++) {
										printf("0x%x	", *(inBuffer + l));
										fprintf(master_CmdResp_FileObj,"8'h%02x,",*(inBuffer + l));
										fprintf(master_CmdResp_FileObj,"\t");
									}
									fprintf(master_CmdResp_FileObj,"\n");
									//printf("Received Data is: %x\n", *(inBuffer + 4));
									poll_mesg = 1;
									break;
								}
							fprintf(master_CmdResp_FileObj," -----------------------------------------\n");
							}//end of if
							else{
								printf("Enter valid Option\n");
								goto choose_grw;
								}
				}
				
				else if (select == 9) //I2C write
				{
					fprintf(master_CmdResp_FileObj," -----------------------------------------\n");
					fprintf(master_CmdResp_FileObj," I2C Write\n");
					rw_index = 4;
					printf("	Enter the Slave address	7'h:");
					scanf("%x", &slave_addr);
					fprintf(master_CmdResp_FileObj," Selected Slave_Addr:0x%02x\n",slave_addr);
					printf("\n");
					// printf("	Enter how many bytes you want to write to slave(1-124): ");
					// scanf("%d", &private_write_len);
					private_write_len=0x01;
					fprintf(master_CmdResp_FileObj," Selected I2C Write Data Length:%d\n",private_write_len);
					printf("\n");
					
					for(index = 0; index < 4; index++){
					outbuffer_private[index] = outbuffer_i2c_legacy_hdcd[index];
					}
							outbuffer_private[2]=(3+private_write_len)<<1;
							outbuffer_private[rw_index++] = slave_addr << 1 | 0x00;//write
							outbuffer_private[rw_index++] = private_write_len;//write
							// printf("	Enter the data to be written into RGB LEB on I2C slave\n");
							for (rw_loop = 0; rw_loop < private_write_len; rw_loop++)
							{
								printf("	Enter the data to be written to RGB LEB of I2C slave(0-7):");
								scanf("%x", &rw_data);
								outbuffer_private[rw_index++] = rw_data;
								printf("\n");
							}
							sizeToTransfer = 6 + private_write_len;
							sizeTransferred = 0;
							status = FTDI_SPI_ReadWrite(ftHandle, inBuffer, outbuffer_private, sizeToTransfer, &sizeTransferred,
								SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
								SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
								SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
							APP_CHECK_STATUS(status);
							fprintf(master_CmdResp_FileObj," SPI_WRITE_DATA:");
							for(index=0;index<sizeToTransfer;index++){
								fprintf(master_CmdResp_FileObj,"8'h%02x,",outbuffer_private[index]);
								fprintf(master_CmdResp_FileObj,"\t");
							}
							fprintf(master_CmdResp_FileObj," I2C write command Sent\n");
							fprintf(master_CmdResp_FileObj," -----------------------------------------\n");
							printf("	I2C write command Sent\n");
							break;
				}
				else if (select == 10) //I2C read
				{
					fprintf(master_CmdResp_FileObj," -----------------------------------------\n");
					fprintf(master_CmdResp_FileObj," I2C Read\n");
					rw_index = 4;
					printf("	Enter the I2C Slave address	7'h:");
					scanf("%x", &slave_addr);
					fprintf(master_CmdResp_FileObj," Selected I2C Slave_Addr:0x%02x\n",slave_addr);
					printf("\n");
					// printf("	Enter how many bytes you want to Read from slave(1-124): ");
					// scanf("%d", &private_read_len);
					private_read_len=0x01;
					fprintf(master_CmdResp_FileObj," Selected Data Length:%d\n",private_read_len);
					printf("\n");
					for(index = 0; index < 4; index++){
						outbuffer_private[index] = outbuffer_i2c_legacy_hdcd[index];
					}
					//outbuffer_private = outbuffer_i2c_legacy_hdcd;
					outbuffer_private[2] = 0x06;
					outbuffer_private[rw_index++] = slave_addr << 1 | 0x01;//read
					outbuffer_private[rw_index++] = private_read_len ;
					sizeToTransfer = 6;
					sizeTransferred = 0;
					status = FTDI_SPI_ReadWrite(ftHandle, inBuffer, outbuffer_private, sizeToTransfer, &sizeTransferred,
						SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
						SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
						SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
					APP_CHECK_STATUS(status);
					// poll_mesg = 1;
					fprintf(master_CmdResp_FileObj," SPI_WRITE_DATA:");
					for(index=0;index<sizeToTransfer;index++){
						fprintf(master_CmdResp_FileObj,"8'h%02x,",outbuffer_private[index]);
						fprintf(master_CmdResp_FileObj,"\t");
					}
					fprintf(master_CmdResp_FileObj,"\n I2C Read command Sent\n");
					fprintf(master_CmdResp_FileObj," -----------------------------------------\n");
					printf("	I2C Read command Sent\n");
					break;
				}
				else if(select ==11)
				{	
						//soft reset
						fprintf(master_CmdResp_FileObj," -----------------------------------------\n");
						fprintf(master_CmdResp_FileObj,"\n Soft Reset the Master IP Issued\n");
						sizeToTransfer = 4;
						sizeTransferred = 0;
						outBuffer_rw[0]=0x00;
						outBuffer_rw[1]=0x14;
						outBuffer_rw[2]=0x02;
						outBuffer_rw[3]=0xc7;
						status = FTDI_SPI_ReadWrite(ftHandle, inBuffer, outBuffer_rw, sizeToTransfer, &sizeTransferred,
							SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
						APP_CHECK_STATUS(status);
						poll_mesg=1;
						fprintf(master_CmdResp_FileObj," SPI_WRITE_DATA:");
					for(index=0;index<sizeToTransfer;index++){
						fprintf(master_CmdResp_FileObj,"8'h%02x,",outBuffer_rw[index]);
						fprintf(master_CmdResp_FileObj,"\t");
					}
					fprintf(master_CmdResp_FileObj,"\n Soft Reset command Sent\n");
					fprintf(master_CmdResp_FileObj," -----------------------------------------\n");
						printf("\t*******SOFT RESET DONE*******\n");
						// reset_dct();
						soft_reset_issued = 1;
						config_done=0;
						break;
				
				}
				else if(select ==12)
				{
					//hdr exit pattern
					fprintf(master_CmdResp_FileObj," -----------------------------------------\n");
					fprintf(master_CmdResp_FileObj," Emit HDR EXIT for M3 Error Handling\n");
					sizeToTransfer = 2;
						sizeTransferred = 0;
						outBuffer_rw[0]=0x00;
						outBuffer_rw[1]=0x1b;
						status = FTDI_SPI_ReadWrite(ftHandle, inBuffer, outBuffer_rw, sizeToTransfer, &sizeTransferred,
							SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
						APP_CHECK_STATUS(status);
						poll_mesg=1;
						fprintf(master_CmdResp_FileObj," SPI_WRITE_DATA:");
					for(index=0;index<sizeToTransfer;index++){
						fprintf(master_CmdResp_FileObj,"8'h%02x,",outBuffer_rw[index]);
						fprintf(master_CmdResp_FileObj,"\t");
					}
					fprintf(master_CmdResp_FileObj,"\n Emit HDR EXIT command Sent\n");
					fprintf(master_CmdResp_FileObj," -----------------------------------------\n");
						printf("\t*******M3 error handling, HDR EXIT pattern transmission done*******\n");
						break;
				
				}
				else if(select ==13)
				{
				fprintf(slave_CmdResp_FileObj," YOU Cleared the LOG\n");
				system("cls");
				poll_mesg = 1;
				}
				else if (select == 14)
				{
					fprintf(master_CmdResp_FileObj," -----------------------------------------\n");
					fprintf(master_CmdResp_FileObj," Closing the SPI Channel\n");
					fprintf(master_CmdResp_FileObj," -----------------------------------------\n");
					status = SPI_CloseChannel(ftHandle);
				//	break;
					system("exit");
					printf("*******Press -> CTRL+C  to Quit*******");
					
					
				}			
				else
				{
						//	select = 0;
							char tmp[1];							
							printf(" \n	-- Invalid Option-- \n");
							//printf(" \n	Hit enter to continue\n");
							poll_mesg = 1;
							scanf("\n %s",&tmp);
							// Invalid=1;
							break;
				}
				poll_mesg = 1;
			}//end of if codtn keybd
			else {
				MasterPollIsr4ResponseProc();
			}
			
		} // end of master device while loop
		if(Invalid==1)
			{
				poll_mesg=1;
				// break;
				
			}
	}	//end of master while 1
		
		while (device == 2)
		{
			if(config_done==0)
			{
				//Slave configuration commands
				for(index = 0; index < 3; index++){
					slave_congig_pointer[index] = slave_config_data[index];
					}
				fprintf(slave_CmdResp_FileObj," SPI_WRITE_DATA for Slave Configuration:\n");
				for(index = 0; index < 9; index++)
				{
				if(index==0){slave_congig_pointer[1]=0x01;slave_congig_pointer[3]=PID_5;}
				if(index==1){slave_congig_pointer[1]=0x02;slave_congig_pointer[3]=PID_4;}
				if(index==2){slave_congig_pointer[1]=0x03;slave_congig_pointer[3]=PID_3 ;}
				// if(index==3){slave_congig_pointer[1]=0x04;slave_congig_pointer[3]=PID_2_SLAVE_ID | Device_ID_MS;}
				if(index==3){slave_congig_pointer[1]=0x04;slave_congig_pointer[3]=thesystemtime.wHour;}
				// if(index==4){slave_congig_pointer[1]=0x05;slave_congig_pointer[3]=PID_1;}
				if(index==4){slave_congig_pointer[1]=0x05;slave_congig_pointer[3]=thesystemtime.wMinute;}
				if(index==5){slave_congig_pointer[1]=0x06;slave_congig_pointer[3]=thesystemtime.wMilliseconds;}
				if(index==6){slave_congig_pointer[1]=0x07;slave_congig_pointer[3]=BCR_CONFIG;}
				if(index==7){slave_congig_pointer[1]=0x08;slave_congig_pointer[3]=DCR_CONFIG;}
				//static address
				if(index==8)
				{
					slave_congig_pointer[1]=0x1e;
						 if(channels == 2){slave_congig_pointer[3]=0x50;}
					else if(channels == 3){slave_congig_pointer[3]=0x51;}
					else if(channels == 4){slave_congig_pointer[3]=0x52;}
					else if(channels == 5){slave_congig_pointer[3]=0x53;}
					else if(channels == 6){slave_congig_pointer[3]=0x54;}
					else if(channels == 7){slave_congig_pointer[3]=0x55;}
					else if(channels == 8){slave_congig_pointer[3]=0x56;}
					else if(channels == 9){slave_congig_pointer[3]=0x57;}
					else if(channels == 10){slave_congig_pointer[3]=0x58;}
					else if(channels == 11){slave_congig_pointer[3]=0x59;}
					else if(channels == 12){slave_congig_pointer[3]=0x60;}
					device_static_addr=slave_congig_pointer[3];	
				} 
				sizeToTransfer = 4;
				sizeTransferred = 0;
				status = SPI_ReadWrite(ftHandle, inBuffer, slave_congig_pointer, sizeToTransfer, &sizeTransferred,
					SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
					SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
					SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
				APP_CHECK_STATUS(status);
				
				for(index2=0;index2<sizeToTransfer;index2++)
				{
					fprintf(slave_CmdResp_FileObj,"8'h%02x,",slave_congig_pointer[index2]);
					fprintf(slave_CmdResp_FileObj,"\t");
				}
				}
				config_done=1;
				printf("Slave Configuration is Done\n");
				fprintf(slave_CmdResp_FileObj," Slave Configuration is Done\n");
				fprintf(slave_CmdResp_FileObj," Time stamp :");
				timestamp(slave_CmdResp_FileObj);
				
			}
			slave_resp = IDLE;
			select = 0;
			while (1)
			{
				if (poll_mesg == 1)
				{
				//	printf(" Connected to I3C slave device --> %d \n",device);
					printf("**************************************************************************\n");
					printf("    I3C SLAVE MENU |Dynamic Address: 7'h%02x |Static Address: 7'h%02x|  \n",device_dyn_addr,device_static_addr);
					printf("**************************************************************************\n");
					printf(" Your Options are:\n");
					printf("	1)Slave Interrupt Request \n");
					printf("	2)Generic Register Read/Write\n");
					printf("	3)Write in to master read FIFO \n");
					printf("	4)SOFT RESET\n");
					printf("	5)Clear the LOG\n");
					printf("	6)EXIT the APP\n");
				//	printf("@Polling Interrupt Status register.....\nSelect:");
					poll_mesg = 0;
				}
				
				if (kbhit())
				{   
					fprintf(master_CmdResp_FileObj," -----------------------------------------\n");
					scanf("%d", &select);
					if (select == 1)
					{
					if(ibi_enable)
					{
						rw_index = 4;
						printf("\n	---------------------------------------------\n");
						printf("	---------------------------------------------\n");
						printf("	>> Slave Interrupt Request...   \n");
						fprintf(slave_CmdResp_FileObj," >> Slave Interrupt Request\n");
						if(setmrl_done==1)
						{
						printf("	Enter how many Bytes you want to Write to Slave(1-%d):",ibi_payload_size_new);
						}
						else 
						{
						printf("	Enter how many Bytes you want to Write to Slave(1-4):");
						}
						scanf("%d", &slave_isr_len);
						fprintf(slave_CmdResp_FileObj," >> selected data length:%d\n",slave_isr_len);
						printf("\n");
						if(setmrl_done==1)
						{	
							if(slave_isr_len>ibi_payload_size_new || slave_isr_len < 1)
							{
								printf("	---------------Invalid length  exited from slave ibi\n------------------\n\n");
								fprintf(slave_CmdResp_FileObj," >> Invalid Data length selected\n");
								poll_mesg = 1;
								ibi_payload_size_old=ibi_payload_size_new;
								break;
							}
						}
						else if(setmrl_done!=1){
							if(slave_isr_len>4 || slave_isr_len < 1)
							{
							printf("	---------------Invalid length exited from slave ibi\n------------------\n\n");
							fprintf(slave_CmdResp_FileObj," >> Invalid Data length selected\n");
							poll_mesg = 1;
							break;
							}
						}
						//sending the interrupt request....
							for(index = 0; index < 4; index++){
						outBuffer_slave_isr_req[index] = outBuffer_ISR_req_slave[index];
							}
							//outBuffer_slave_isr_req = outBuffer_ISR_req_slave;
							outBuffer_slave_isr_req[rw_index++] = slave_isr_len;//write
							outBuffer_slave_isr_req[2]=(2+slave_isr_len) << 1;
							for (rw_loop = 0; rw_loop < slave_isr_len; rw_loop++)
							{
								printf("	Please enter data[%03d](hex)0x :", rw_loop);
								scanf("%x", &rw_data);
								outBuffer_slave_isr_req[rw_index++] = rw_data;
								printf("\n");
							}
							fprintf(slave_CmdResp_FileObj," SPI_WRITE_DATA:IBI REQ sent to Master\n");
							sizeToTransfer = 5 + slave_isr_len;
							sizeTransferred = 0;
							status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_slave_isr_req, sizeToTransfer, &sizeTransferred,
								SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
								SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
								SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
							APP_CHECK_STATUS(status);
							fprintf(slave_CmdResp_FileObj,"Time stamp:");
							timestamp(slave_CmdResp_FileObj);
							for(index=0;index<sizeToTransfer;index++){
								fprintf(slave_CmdResp_FileObj,"8'h%02x,",outBuffer_slave_isr_req[index]);
								fprintf(slave_CmdResp_FileObj,"\t");
							}
							fprintf(slave_CmdResp_FileObj,"\n");
							printf("	Interrupt Request sent to the Master...   \n");
							
							fprintf(slave_CmdResp_FileObj," --------------------------------------");
							
							printf("	---------------------------------------------\n");
							printf("	---------------------------------------------\n\n");
							poll_mesg = 1;
							break;
							}
							else 
							{
							printf("	---------------IBI-Disabled------------------\n\n");
							fprintf(slave_CmdResp_FileObj," >> Slave IBI-Disabled\n");
							poll_mesg = 1;
							break;
							}
						}
					else if (select == 2)
					{
						poll_mesg = 1;
						choose_grws:
						printf(" Choose SPI:\n	1.Read\n	2.write\n 3.Go back from this menu\n Select:");
						scanf("%d", &rw);
						printf("\n");
						if(rw==3){
							break;
						}
						if (rw == 2) //write
						{
							fprintf(slave_CmdResp_FileObj," >> Generic Write\n");
							rw_index = 0;
							printf("	Enter MSB address	>> (hex) 0x:");
							scanf("%x", &rw_data);
							fprintf(slave_CmdResp_FileObj," >>MSB address:0x%02x\n",rw_data);
							printf("\n");
							outBuffer_rw[rw_index++] = rw_data;
							printf("	Enter LSB address	>>  (hex) 0x");
							scanf("%x", &rw_data);
							fprintf(slave_CmdResp_FileObj," >>LSB address:0x%02x\n",rw_data);
							printf("\n");
							outBuffer_rw[rw_index++] = rw_data;
							printf("	Enter how many bytes of data you want to Write(1-124) :");
							scanf("%d", &rw_len);
							if(rw_len > 125)
							{
								printf("	Not supported more than 128\n");
								poll_mesg=1;
								break;
							}
							fprintf(slave_CmdResp_FileObj," >>Data length:%d\n",rw_len);
							printf("\n");
							outBuffer_rw[rw_index++] = (rw_len<<1)|0x00;//write command lsb:0
							for (rw_loop = 0; rw_loop < rw_len; rw_loop++)
							{
								printf("	Please enter data[%03d] : 0x", rw_loop);
								scanf("%x", &rw_data);
								outBuffer_rw[rw_index++] = rw_data;
								printf("\n");
							}
							sizeToTransfer = rw_len + 3;
							sizeTransferred = 0;
							status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_rw, sizeToTransfer, &sizeTransferred,
								SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
								SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
								SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
							APP_CHECK_STATUS(status);
							printf("Debug SPI Write Command Sent.\n");
							fprintf(slave_CmdResp_FileObj," SPI Write Command\n");
							for(index=0;index<sizeToTransfer;index++){
								fprintf(slave_CmdResp_FileObj,"8'h%02x,",outBuffer_rw[index]);
								fprintf(slave_CmdResp_FileObj,"\t");
							}
							fprintf(slave_CmdResp_FileObj,"\n");
							break;
						}
						else if (rw == 1)//read
						{
							fprintf(slave_CmdResp_FileObj," >> Generic Read\n");
							rw_index = 0;
							printf("	Enter MSB address	>> (hex) 0x:");
							scanf("%x", &rw_data);
							fprintf(slave_CmdResp_FileObj," >>MSB address:0x%02x\n",rw_data);
							printf("\n");
							outBuffer_rw[rw_index++] = rw_data;
							printf("	Enter LSB address	>> (hex) 0x:");
							scanf("%x", &rw_data);
							fprintf(slave_CmdResp_FileObj," >>LSB address:0x%02x\n",rw_data);
							printf("\n");
							outBuffer_rw[rw_index++] = rw_data;
							printf("	Enter how many bytes of data you want to Read(1-124): ");
							scanf("%d", &rw_len);
							fprintf(slave_CmdResp_FileObj," >>Data length:%d\n",rw_len);
							printf("\n");
							if(rw_len > 125)
							{
								printf("	Not supported more than 128\n");
								poll_mesg=1;
								break;
							}
							outBuffer_rw[rw_index++] = (rw_len << 1) | 0x01;//write command lsb:1
							sizeToTransfer = rw_len + 4;
							sizeTransferred = 0;
							status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_rw, sizeToTransfer, &sizeTransferred,
								SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
								SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
								SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
							APP_CHECK_STATUS(status);
								printf("Debug Read-Received Data:");
								for (l = 4; l < (sizeToTransfer); l++){
									printf("	0x%x",*(inBuffer + l));
									fprintf(slave_CmdResp_FileObj,"8'h%02x,",*(inBuffer + l));
									fprintf(slave_CmdResp_FileObj,"\t");	
								}
								fprintf(slave_CmdResp_FileObj,"\n");
							printf("\n\n");
							break;
						}
					else
					{
						
						select = 0;
						printf("Enter valid Option:\n");
						goto choose_grws;;
					}
				}
					else if(select==3)
					{
					unsigned char str[128] = {0};
					char rand_data_2;
					fprintf(slave_CmdResp_FileObj," Write in to master read FIFO\n");
					rw_index=0;
					printf("	Enter how many bytes of data you want to write to master read FIFO(1-124):");
							scanf("%d", &rw_len);
							fprintf(slave_CmdResp_FileObj," >> selected data length:%d\n",rw_len);
							printf("\n");
							if(rw_len > 125)
							{
								printf("	Not supported more than 128\n");
								poll_mesg=1;
								break;
							}
							outBuffer_rw[rw_index++] = 0x02;//msb
						outBuffer_rw[rw_index++] = 0x00;//lsb
							outBuffer_rw[rw_index++] = (rw_len<<1)|0x00;//write command lsb:0
							
							printf("	Do you want send random data automatically ?\n\n\t[y/n]:",rand_data_2);
							scanf("\n%c",&rand_data_2);
							srand ( time(NULL) );
							printf("\n-----------------------------------------------------------\n");
							printf("	---------------------------------------------\n");
							if(rand_data_2 == 'Y' || rand_data_2 == 'y')
							{
								for(index = 0; index < rw_len; index++) 
								{
									sprintf(str + index, "%x", rand() % 16);
									outBuffer_rw[rw_index++]=str[index];
									printf("	| Write[%03d] : 0x%02x\n",index,str[index]);
								}
							}
							else
							{
								for (rw_loop = 0; rw_loop < rw_len; rw_loop++)
								{
									printf("	Please enter data[%03d] : 0x", rw_loop);
									scanf("%x", &rw_data);
									outBuffer_rw[rw_index++] = rw_data;
									printf("\n");
								}
							}
						printf("\t **Operation done**\n");
						fprintf(slave_CmdResp_FileObj," ----------Master read fifo filled at time stamp:-------------------------------");
						timestamp(slave_CmdResp_FileObj);
						printf("	---------------------------------------------\n");
						printf("	---------------------------------------------\n");
						sizeToTransfer = rw_len + 3;//4->3
						sizeTransferred = 0;
						status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_rw, sizeToTransfer, &sizeTransferred,
							SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
						APP_CHECK_STATUS(status);
						
						fprintf(slave_CmdResp_FileObj," SPI Write Command\n");
							for(index=0;index<sizeToTransfer;index++){
								fprintf(slave_CmdResp_FileObj,"8'h%02x",outBuffer_rw[index]);
								fprintf(slave_CmdResp_FileObj,"\t");
							}
							fprintf(slave_CmdResp_FileObj,"\n");
							
						poll_mesg=1;
						break;
					}
					else if(select ==4)
					{	//soft reset salve
						fprintf(slave_CmdResp_FileObj," ----------Soft Reset the Slave IP\n time stamp:-------------------------------");
						timestamp(slave_CmdResp_FileObj);
						sizeToTransfer = 4;//4->3
						sizeTransferred = 0;
						outBuffer_rw[0]=0x00;
						outBuffer_rw[1]=0x14;
						outBuffer_rw[2]=0x02;
						outBuffer_rw[3]=0x40;
						status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_rw, sizeToTransfer, &sizeTransferred,
							SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
						APP_CHECK_STATUS(status);
						fprintf(slave_CmdResp_FileObj," SPI Write Command\n");
							for(index=0;index<sizeToTransfer;index++){
								fprintf(slave_CmdResp_FileObj,"8'h%02x,",outBuffer_rw[index]);
								fprintf(slave_CmdResp_FileObj,"\t");
							}
							fprintf(slave_CmdResp_FileObj,"\n");
						poll_mesg=1;
						printf("\t*******SOFT RESET DONE*******\n");
						fprintf(slave_CmdResp_FileObj," Soft Reset done\n");
						config_done=0;
						soft_reset_issued=1;
						break;
					}
					else if(select==5)
					{
					fprintf(slave_CmdResp_FileObj," YOU Cleared the LOG\n");
					system("cls");
					poll_mesg=1;
					break;
					}
					else if(select ==6)
					{
						status = SPI_CloseChannel(ftHandle);
						//break;
						system("exit");
						fprintf(slave_CmdResp_FileObj," Closing SPI Channel\n");
						printf("*******Press -> CTRL+C  *******");
						
					}
					else if(select ==0)
					{
						poll_mesg = 1;
						break;
					}
					else
					{	
						// select = 0;
						char tmp[1];							
						printf(" \n	-- Invalid Option-- \n");
						// printf(" \n	Hit enter to continue\n");
						poll_mesg = 1;
						scanf("\n %s",&tmp);
						// Invalid=1;
						break;
					}
				fprintf(slave_CmdResp_FileObj," -----------------------------------------\n");
				}//end of if keybd hit
				else
				{
				switch(slave_resp) {
				
					if(soft_reset_issued) {
					slave_resp = IDLE;
					
					soft_reset_issued=0;
					}

					case IDLE  :
					{	
						slave_resp = IDLE;
						if(is_data_ready())
							slave_resp = CMD;
						break;
					}
					case CMD  :
					{
						fprintf(slave_CmdResp_FileObj," ----------Got Slave Response\n time stamp:-------------------------------");
						timestamp(slave_CmdResp_FileObj);
						slave_resp = CMD;
						outBuffer_Response_buffer[2]=0x03;
						sizeToTransfer = 5;  
						sizeTransferred = 0;
						status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_Response_buffer, sizeToTransfer, &sizeTransferred,
							SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
						APP_CHECK_STATUS(status);
						fprintf(slave_CmdResp_FileObj," ----------Reading Response buffer for resp_code\n time stamp:-------------------------------");
						timestamp(slave_CmdResp_FileObj);
						fprintf(slave_CmdResp_FileObj," SPI Write Command\n");
							for(index=0;index<sizeToTransfer;index++){
								fprintf(slave_CmdResp_FileObj,"8'h%02x,",outBuffer_Response_buffer[index]);
								fprintf(slave_CmdResp_FileObj,"\t");
							}
							fprintf(slave_CmdResp_FileObj,"\n");
						resp_command=*(inBuffer + 4);
						slave_resp =CMD_LEN;
						fprintf(slave_CmdResp_FileObj," -----------------------------------------\n");
						fprintf(slave_CmdResp_FileObj," Slave_Resp_code :8'h%02x\n",resp_command);
						break;
					}
					case CMD_LEN  :
					{	
						slave_resp = CMD_LEN;
						if(is_data_ready()){
							outBuffer_Response_buffer[2]=0x03;
							sizeToTransfer = 5;  
							sizeTransferred = 0;
							status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_Response_buffer, sizeToTransfer, &sizeTransferred,
								SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
								SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
								SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
							APP_CHECK_STATUS(status);
							fprintf(slave_CmdResp_FileObj," ----------Reading Response buffer for data length\n time stamp:-------------------------------");
							timestamp(slave_CmdResp_FileObj);
							fprintf(slave_CmdResp_FileObj," SPI Write Command\n");
							for(index=0;index<sizeToTransfer;index++){
								fprintf(slave_CmdResp_FileObj,"8'h%02x,",outBuffer_Response_buffer[index]);
								fprintf(slave_CmdResp_FileObj,"\t");
							}
							fprintf(slave_CmdResp_FileObj,"\n");
							
							data_length=*(inBuffer + 4);
							if(resp_command == 0x05)
							{
								slave_resp=PRIVATE_WRITE;
							}
							else if(resp_command == 0x03)
							{
								slave_resp=PRIVATE_READ;
							}
							else
							{
								slave_resp =DATA;
							}
							fprintf(slave_CmdResp_FileObj," Data_length :8'h%02x\n",data_length);
						}
						slave_resp_rcvd_data = 0;
						
						break;
					}
					case DATA  :
					{
						slave_resp =DATA;
						if(is_data_ready()){
							// outBuffer_Response_buffer[2]=0x03;
							// sizeToTransfer = 5;  
							outBuffer_Response_buffer[2]=((uint32)data_length<<1)|0x01;
							sizeToTransfer = 4+(uint32)data_length;
							sizeTransferred = 0;
							status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_Response_buffer, sizeToTransfer, &sizeTransferred,
								SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
								SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
								SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
							APP_CHECK_STATUS(status);
							// inbuffer_slave[slave_resp_rcvd_data++] = *(inBuffer + 4);
							// slave_resp = DATA;
							
							while(slave_resp_rcvd_data != data_length)
							{
								inbuffer_slave[slave_resp_rcvd_data] = *(inBuffer + 4 + slave_resp_rcvd_data);
								fprintf(slave_CmdResp_FileObj,"Received data : \n data[%d]: 8'h%02x\n",slave_resp_rcvd_data,inbuffer_slave[slave_resp_rcvd_data]);
								slave_resp_rcvd_data++;
							}
							fprintf(master_CmdResp_FileObj," -----------------------------------------\n");
							slave_resp = ECHO_RESPONSE;
							// if(slave_resp_rcvd_data == data_length){
								// slave_resp = ECHO_RESPONSE;
							// }
						}
						break;
					}	
					case PRIVATE_WRITE:
					{	
						fprintf(master_CmdResp_FileObj," -----------------------------------------\n");
						fprintf(master_CmdResp_FileObj," Private Write\n");
						printf("\n-----------------------------------------------------------\n");
						printf("	---------------------------------------------\n");
						//printf("	Private written length in decimal:%d\n",*(inBuffer + 5));
						printf("	Private written data length :%d \n",data_length);
						fprintf(slave_CmdResp_FileObj,"	Private written data length 0x:%x \n",data_length);
						for(index = 0; index < 3; index++){
							outBuffer_Response_pv_wr[index]=outBuffer_Response_pv_wr_buffer_hdcd[index];
						}
						outBuffer_Response_pv_wr[2]=data_length << 1 | 0x01;
						sizeToTransfer = 4+(uint32)data_length;
						sizeTransferred = 0;

						status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_Response_pv_wr, sizeToTransfer, &sizeTransferred,
							SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
						APP_CHECK_STATUS(status);
						
						for(index=0;index<sizeToTransfer;index++){
							fprintf(slave_CmdResp_FileObj,"8'h%02x,",outBuffer_Response_pv_wr[index]);
							fprintf(slave_CmdResp_FileObj,"\t");
						}
						fprintf(slave_CmdResp_FileObj,"\n");
						slave_resp_rcvd_data=0;
						printf("	| Private Messaging Written data\n");
						fprintf(slave_CmdResp_FileObj,"	| Private Messaging Written Data\n");
						for (l = 4,index=0; l < (sizeToTransfer); l++,index++)
						{
						inbuffer_slave[slave_resp_rcvd_data++] = *(inBuffer + l);
							printf("	| Write[%03d] : 0x%02x\n ",index,*(inBuffer+l));
							fprintf(slave_CmdResp_FileObj,"8'h%02x,",*(inBuffer + l));
							fprintf(slave_CmdResp_FileObj,"\t");
						}
						fprintf(slave_CmdResp_FileObj,"\n");
						printf("	---------------------------------------------\n");
						printf("	---------------------------------------------\n\n");
						fprintf(slave_CmdResp_FileObj,"	---------------------------------------------\n\n");
					poll_mesg=1;
					
					slave_resp =IDLE;
					break;
					}
					case PRIVATE_READ:
					{
						fprintf(slave_CmdResp_FileObj," Private Read Response\n");
						printf("\n	---------------------------------------------\n");
						printf("	---------------------------------------------\n");
						printf("	|Private Read\n");
						printf("	|Private Read data length is:%x \n", data_length);
						fprintf(slave_CmdResp_FileObj," Private Read data length:%d\n",data_length);
						printf("	---------------------------------------------\n");
						printf("	---------------------------------------------\n\n");
						fprintf(slave_CmdResp_FileObj,"	---------------------------------------------\n\n");
						poll_mesg=1;
						slave_resp =IDLE;
						break;
					}
					/*case I2C_WRITE:
					{
					
					break;
					}case I2C_READ:
					{
						printf("\n	---------------------------------------------\n");
						printf("	---------------------------------------------\n");
						printf("	|I2C Read\n");
						printf("	|I2C Read data length is:%x \n", data_length);
						printf("	---------------------------------------------\n");
						printf("	---------------------------------------------\n\n");
						poll_mesg=1;
						slave_resp =IDLE;
						break;
					}*/
					
					case ECHO_RESPONSE:
					{
						
						if(resp_command == 0x11)
						{
							fprintf(slave_CmdResp_FileObj," ENTDAA Response\n");
							printf("\n	---------------------------------------------\n");
							printf("	---------------------------------------------\n");
							printf("	|\n");
							printf("	| Received Data length is 0x:%x\n",data_length);
							fprintf(slave_CmdResp_FileObj," Data length:%d\n",data_length);
							printf("	| Slave-> Dynamic Address Assignment Done\n");
							printf("	| Slave-> Dynamic Address Assigned  is 0x:%x \n",*(inbuffer_slave+0));
							fprintf(slave_CmdResp_FileObj," Assigned Dync_Addr:0x%02x\n",*(inbuffer_slave+0));
							printf("	---------------------------------------------\n");
							printf("	---------------------------------------------\n\n");
							fprintf(slave_CmdResp_FileObj,"	---------------------------------------------\n\n");
							device_dyn_addr = *(inbuffer_slave+0);
							poll_mesg=1;
						}
						else if (resp_command == 0x10)//daa reset
						{
							fprintf(slave_CmdResp_FileObj," RSTDAA Response\n");
							printf("\n	---------------------------------------------\n");
							printf("	---------------------------------------------\n");
							printf("	|\n");
							printf("	| Received Data length is 0x:%x\n",data_length);
							fprintf(slave_CmdResp_FileObj," Received data length:%d\n",data_length);
							printf("	| Slave-> RSTDAA\n");
							printf("	| Slave-> Reset Dynamic address  is 0x:%x \n", *(inbuffer_slave + 0));
							fprintf(slave_CmdResp_FileObj," Reset Dync_Addr:0x%02x\n",*(inbuffer_slave+0));
							printf("	---------------------------------------------\n");
							printf("	---------------------------------------------\n\n");
							fprintf(slave_CmdResp_FileObj,"	---------------------------------------------\n\n");
							device_dyn_addr = 0x7E;
							poll_mesg=1;
						}
						else if (resp_command == 0x41)// Slave ISR IBI
						{
							fprintf(slave_CmdResp_FileObj,"\n	 IBI Response\n");
							printf("\n	---------------------------------------------\n");
							printf("	---------------------------------------------\n");
							printf("	|\n");
							printf("\n	|IBI Response Came\n");
							printf("	|\n");
							if (0x01 & data_length)
							{
								printf("	ACK by Master \n");
								fprintf(slave_CmdResp_FileObj," ACK by Master\n");
								printf("	| IBI Payload Left data length is:%x \n",data_length);
								fprintf(slave_CmdResp_FileObj," IBI Payload Left :%d:\n",data_length);
								printf("	---------------------------------------------\n");
								printf("	---------------------------------------------\n\n");
								fprintf(slave_CmdResp_FileObj,"	---------------------------------------------\n\n");
							}
							else{
								fprintf(slave_CmdResp_FileObj," NACK by Master\n");
								printf("	NACK by Master\n");
							}
							
							poll_mesg=1;
						}
					
						else if ((resp_command == 0x31 )|| (resp_command == 0x51))//BCC or DCC
						{
						
							printf("	---------------------------------------------\n");
							printf("	---------------------------------------------\n");
							// printf("	| Received Data length is 0x:%x\n",data_length);
							ccc_len=data_length;
							printf("	| Received CCC length is %d\n",ccc_len);
							fprintf(slave_CmdResp_FileObj," Received data length:%d\n",ccc_len);
						
								 if(*(inbuffer_slave + 0)== 0x00){fprintf(slave_CmdResp_FileObj," BCCC ENEC");printf("	| BCCC ENEC\n");ibi_enable=1;}
							else if(*(inbuffer_slave + 0)== 0x01){fprintf(slave_CmdResp_FileObj," BCCC DISEC");printf("	| BCCC DISEC\n");ibi_enable=0;}
							else if(*(inbuffer_slave + 0)== 0x02){fprintf(slave_CmdResp_FileObj," BCCC ENTAS0");printf("	| BCCC ENTAS0\n");}
							else if(*(inbuffer_slave + 0)== 0x06){fprintf(slave_CmdResp_FileObj," BCCC RSTDAA");printf("	| BCCC RSTDAA\n");}
							else if(*(inbuffer_slave + 0)== 0x07){fprintf(slave_CmdResp_FileObj," BCCC RSTDAA");printf("	| BCCC RSTDAA\n");}
							else if(*(inbuffer_slave + 0)== 0x09){fprintf(slave_CmdResp_FileObj," BCCC SETMWL");printf("	| BCCC SETMWL\n");set_ccc=1;}
							else if(*(inbuffer_slave + 0)== 0x0A)
							{
								fprintf(slave_CmdResp_FileObj," BCCC SETMRL");
								printf("	| BCCC SETMRL\n");set_ccc=2;
								ibi_payload_size_new=inbuffer_slave[3];
								if(ibi_payload_size_old< ibi_payload_size_new)
									setmrl_done=1;
								else {
									setmrl_done=0;
								}					
									}
							else if(*(inbuffer_slave + 0)== 0x28){fprintf(slave_CmdResp_FileObj," BCCC SETXTIME");printf("	| BCCC SETXTIME\n");}
								//DCC 
							else if(*(inbuffer_slave + 0)== 0x80){fprintf(slave_CmdResp_FileObj," DCCC ENEC");printf("	| DCCC ENEC\n");ibi_enable=1;}
							else if(*(inbuffer_slave + 0)== 0x81){fprintf(slave_CmdResp_FileObj," DCCC DISEC");printf("	| DCCC DISEC\n");ibi_enable=0;}
							else if(*(inbuffer_slave + 0)== 0x82){fprintf(slave_CmdResp_FileObj," DCCC ENTSA0");printf("	| DCCC ENTSA0\n");}
							else if(*(inbuffer_slave + 0)== 0x87){fprintf(slave_CmdResp_FileObj," DCCC SETDASA");printf("	| DCCC SETDASA\n");}
							else if(*(inbuffer_slave + 0)== 0x86){fprintf(slave_CmdResp_FileObj," DCCC RSTDAA");printf("	| DCCC RSTDAA\n");device_dyn_addr = 0x7E;}		
							else if(*(inbuffer_slave + 0)== 0x88){fprintf(slave_CmdResp_FileObj," DCCC SETNEWDA");printf("	| DCCC SETNEWDA\n");}
							else if(*(inbuffer_slave + 0)== 0x89){fprintf(slave_CmdResp_FileObj," DCCC SETMWL");printf("	| DCCC SETMWL\n");set_ccc=1;}
							else if(*(inbuffer_slave + 0)== 0x8A)
							{
								fprintf(slave_CmdResp_FileObj," DCCC SETMRL");
								printf("	| DCCC SETMRL\n");
								set_ccc=2;
								ibi_payload_size_new=inbuffer_slave[3];
								setmrl_done=1;
							}
							else if(*(inbuffer_slave + 0)== 0x8B){fprintf(slave_CmdResp_FileObj," DCCC GETMWL");printf("	| DCCC GETMWL\n");}			
							else if(*(inbuffer_slave + 0)== 0x8C){fprintf(slave_CmdResp_FileObj," DCCC GETMRL");printf("	| DCCC GETMRL\n");}
							else if(*(inbuffer_slave + 0)== 0x8D){fprintf(slave_CmdResp_FileObj," DCCC GETPID");printf("	| DCCC GETPID\n");}
							else if(*(inbuffer_slave + 0)== 0x8E){fprintf(slave_CmdResp_FileObj," DCCC GETBCR");printf("	| DCCC GETBCR\n");}
							else if(*(inbuffer_slave + 0)== 0x8F){fprintf(slave_CmdResp_FileObj," DCCC GETDCR");printf("	| DCCC GETDCR\n");}
							else if(*(inbuffer_slave + 0)== 0x90){fprintf(slave_CmdResp_FileObj," DCCC GETSTATUS");printf("	| DCCC GETSTATUS\n");}
							// else if(*(inbuffer_slave + 0)== 0x90){fprintf(slave_CmdResp_FileObj," DCCC GETSTATUS");printf("	| DCCC GETSTATUS\n");}					
							// printf("	|Associated Data:");
							for (rw_loop = 1; rw_loop < ccc_len; rw_loop++)
							{
								if(*(inbuffer_slave + 0)== 0x87 || *(inbuffer_slave + 0)== 0x88){
								printf("	| Associated Data[%d] : 0x%x\n",rw_loop-1,inbuffer_slave[rw_loop]>>1);
								fprintf(slave_CmdResp_FileObj,"	| Associated Data[%d] : 0x%x\n",rw_loop-1,inbuffer_slave[rw_loop]>>1);
								device_dyn_addr=inbuffer_slave[rw_loop]>>1;
								}
								else
								{
									printf("	| Associated Data[%d] : 0x%x\n",rw_loop-1,inbuffer_slave[rw_loop]);
									fprintf(slave_CmdResp_FileObj,"	| Associated Data[%d] : 0x%x\n",rw_loop-1,inbuffer_slave[rw_loop]);
								}
								// if(*(inbuffer_slave + 0)== 0x88){
									// device_dyn_addr = inbuffer_slave[rw_loop]>>1;
								// }
							}
							printf("	\n");
							fprintf(slave_CmdResp_FileObj,"	\n");
							// printf("	\n");
							printf("	---------------------------------------------\n");
							printf("	---------------------------------------------\n");
							poll_mesg=1;
							if(set_ccc==1)//mwl
							{
								outBuffer_Response_set_bcc[0]=0x00;
								outBuffer_Response_set_bcc[1]=0x09;
								outBuffer_Response_set_bcc[2]=data_length << 1;
								for(rw_loop = 1,index = 3; rw_loop <= data_length; rw_loop++,index++){
									outBuffer_Response_set_bcc[index]=inbuffer_slave[rw_loop];
								}
								sizeToTransfer = 2+(uint32)data_length;
								sizeTransferred = 0;
								status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_Response_set_bcc, sizeToTransfer, &sizeTransferred,
									SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
									SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
									SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
								APP_CHECK_STATUS(status);
								set_ccc=0;
								//fprintf(slave_CmdResp_FileObj," ----------Got Slave Response\n time stamp:-------------------------------");
								fprintf(slave_CmdResp_FileObj," SPI_WRITE_DATA to update SETMWL :");
								fprintf(slave_CmdResp_FileObj," Time stamp :");
								timestamp(slave_CmdResp_FileObj);
								for(index=0;index<sizeToTransfer;index++)
								{
									fprintf(slave_CmdResp_FileObj,"8'h%02x,",outBuffer_Response_set_bcc[index]);
									fprintf(slave_CmdResp_FileObj,"\t");
								}
								fprintf(slave_CmdResp_FileObj,"\n");
							}
							else if(set_ccc==2)//mrl
							{
								outBuffer_Response_set_bcc[0]=0x00;
								outBuffer_Response_set_bcc[1]=0x0b;
								outBuffer_Response_set_bcc[2]=data_length << 1;
								for(rw_loop = 1,index = 3; rw_loop <= data_length; rw_loop++,index++){
									outBuffer_Response_set_bcc[index]=inbuffer_slave[rw_loop];
								}
								sizeToTransfer = 2+(uint32)data_length;
								sizeTransferred = 0;
								status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_Response_set_bcc, sizeToTransfer, &sizeTransferred,
									SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
									SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
									SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
								APP_CHECK_STATUS(status);
								set_ccc=0;
								fprintf(slave_CmdResp_FileObj," SPI_WRITE_DATA to update SETMRL :");
								fprintf(slave_CmdResp_FileObj," Time stamp :");
								timestamp(slave_CmdResp_FileObj);
								for(index=0;index<sizeToTransfer;index++)
								{
									fprintf(slave_CmdResp_FileObj,"8'h%02x,",outBuffer_Response_set_bcc[index]);
									fprintf(slave_CmdResp_FileObj,"\t");
								}
								fprintf(slave_CmdResp_FileObj,"\n");
							}
						}
						slave_resp =IDLE;
						break;
					}			
					default:
					slave_resp =IDLE;
				}
			}//end of else kbhit()			
		}//end of slave 1 while(1)
		if(Invalid==1)
		{
			poll_mesg=1;
			// break;	
		}
	}//end of while slave-0 changed to if
		status = SPI_CloseChannel(ftHandle);
}//end if channel<0
	// stream = freopen("file.txt", "w+", stdout);
#ifdef _MSC_VER
	Cleanup_libMPSSE();
#endif
#ifndef __linux__
	system("pause");
#endif
	return 0;
} //end of main
// void CALLBACK MasterPollIsr4ResponseProc(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime) {
void MasterPollIsr4ResponseProc()
{
	int index = 0;
	uint8 isr_status;
	uint8 data_valid =0;
	FT_STATUS status = FT_OK;
	if(soft_reset_issued) {
		IBI_nCMD = 0;
		data_valid = 0;
		master_ibi_resp = IBI_IDLE;
		master_resp = IDLE;
		fprintf(master_IbiResp_FileObj," soft reset issued\n\n");
		fprintf(master_CmdResp_FileObj," soft reset issued\n\n");
		soft_reset_issued = 0;
		return;
	}
	if(CMD_BUFFR_NOT_FREE){
		RESP_BUFFR_NOT_FREE = 0;
		return;
	}
	isr_status = check_isr();
	if(((isr_status) & (1 << 3)) != 0){
		IBI_nCMD = 1;
		data_valid = 1;
	}
	else if(((isr_status) & (1 << 2)) != 0){
		IBI_nCMD = 0;
		data_valid = 1;
	}
	else if((isr_status) & (1 << 7)!=0)
	{
		
		sizeToTransfer = 5;  
		sizeTransferred = 0;
		status = SPI_ReadWrite(ftHandle, inBuffer, MS_error_register_buffer, sizeToTransfer, &sizeTransferred,
			SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
			SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
			SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
		APP_CHECK_STATUS(status);
		
	Error_ID=(*(inBuffer + 4));
	
	if (Error_ID==0x00){printf("	********** M-0 ERROR DETECTED **********");fprintf(master_CmdResp_FileObj," >>**** M-0 ERROR DETECTED ****\n");	}
	else if (Error_ID==0x01){printf("	********** M-1 ERROR DETECTED **********");fprintf(master_CmdResp_FileObj," >>**** M-1 ERROR DETECTED ****\n");}
	else if (Error_ID==0x02){printf("	********** M-2 ERROR DETECTED **********");fprintf(master_CmdResp_FileObj," >>**** M-2 ERROR DETECTED ****\n");}
	
	}
	else {
		data_valid = 0;
	}
	// printf("isr_status %d\n",isr_status);
	RESP_BUFFR_NOT_FREE = 1;
	// printf("master_resp: %d\n",master_resp);
	// printf("master_ibi_resp: %d\n",master_ibi_resp);
	if(IBI_nCMD){
			switch(master_ibi_resp) {	
			case IBI_IDLE  :
			{	
				master_ibi_resp = IBI_IDLE;
				if(data_valid)
					master_ibi_resp = IBI_TYPE;
				break;
			}
			case IBI_TYPE  :
			{	
				master_ibi_resp = IBI_TYPE;
				outBuffer_IBI_Response_buffer[2]=0x03;
				sizeToTransfer = 5;  
				sizeTransferred = 0;
				status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_IBI_Response_buffer, sizeToTransfer, &sizeTransferred,
					SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
					SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
					SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
				APP_CHECK_STATUS(status);
				ibi_resp_command=*(inBuffer + 4);
				master_ibi_resp =IBI_LEN;
				fprintf(master_IbiResp_FileObj," ibi_Resp :8'h%02x\n",ibi_resp_command);
				break;
			}
			case IBI_LEN  :
			{	
				master_ibi_resp = IBI_LEN;
				if(data_valid){
					outBuffer_IBI_Response_buffer[2]=0x03;
					sizeToTransfer = 5;  
					sizeTransferred = 0;
					status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_IBI_Response_buffer, sizeToTransfer, &sizeTransferred,
						SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
						SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
						SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
					APP_CHECK_STATUS(status);
					ibi_data_length=*(inBuffer + 4);
					master_ibi_resp =IBI_DATA;
				}
				ibi_resp_rcvd_data = 0;
				fprintf(master_IbiResp_FileObj," ibi_data_len :8'h%02x\n",ibi_data_length);
				break;
			}
			case IBI_DATA  :
			{
				master_ibi_resp =IBI_DATA;
				if(data_valid){
					outBuffer_IBI_Response_buffer[2]=((uint32)ibi_data_length<<1)|0x01;
					sizeToTransfer = 4+(uint32)ibi_data_length;
					sizeTransferred = 0;
					status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_IBI_Response_buffer, sizeToTransfer, &sizeTransferred,
						SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
						SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
						SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
					APP_CHECK_STATUS(status);
					while(ibi_resp_rcvd_data != ibi_data_length){
						inbuffer_ibi[ibi_resp_rcvd_data] = *(inBuffer + 4 + ibi_resp_rcvd_data);
						
						
						fprintf(master_IbiResp_FileObj,"ibi_data[%d]: 8'h%02x\n",ibi_resp_rcvd_data,inbuffer_slave[ibi_resp_rcvd_data]);
						ibi_resp_rcvd_data++;
						
						
					}
					master_ibi_resp = IBI_ECHO_RESPONSE;
				}
				fprintf(master_IbiResp_FileObj,"--------------------------------\n");
				fprintf(master_IbiResp_FileObj,"--------------------------------\n");
				break;
			}
			case IBI_ECHO_RESPONSE  :
			{
				
				printf("\n	---------------------------------------------\n");
				printf("	---------------------------------------------\n");
				if(ibi_resp_command == I3C_MASTER_IBI_RESPONSE_BYTE0_IS_UNRECOGNIZED)
				{
					printf("	This IBI request was not recognized by I3C master\n");
					fprintf(master_IbiResp_FileObj,"	This IBI request was not recognized by I3C master\n");
					printf("\tRequest detected from I3C device with slave address: 7'h%x,1'b%d\n",inbuffer_ibi[0]>>1,inbuffer_slave[0]&0x01);
					fprintf(master_IbiResp_FileObj,"\tRequest detected from I3C device with slave address: 7'h%x,1'b%d\n",inbuffer_ibi[0]>>1,inbuffer_slave[0]&0x01);
					if((ibi_data_length == 2) && (inbuffer_ibi[1] == 0x01)) {
						printf("\tMaster didn't acknowledge this IBI request\n");
						fprintf(master_IbiResp_FileObj,"\tMaster didn't acknowledge this IBI request\n");
					}
					else {
						if(inbuffer_ibi[1] == 0x00){
							printf("\tMaster acknowledged this IBI request\n");
							fprintf(master_IbiResp_FileObj,"\tMaster acknowledged this IBI request\n");
						}
						for(index = 2; index < ibi_data_length; index++){
							printf("\tpayload byte[%03d]: 0x%02x\n",index-1,inbuffer_ibi[index]);
							fprintf(master_IbiResp_FileObj,"\tpayload byte[%03d]: 0x%02x\n",index-1,inbuffer_ibi[index]);
						}
					}
					poll_mesg=1;
				}
				else if (ibi_resp_command == I3C_MASTER_IBI_RESPONSE_BYTE0_IS_IBI_REQ)
				{
					printf("	This IBI request was recognized by I3C master as normal IBI request from slave device\n");
					fprintf(master_IbiResp_FileObj,"	This IBI request was recognized by I3C master as normal IBI request from slave device\n");
					printf("\tRequest detected from I3C device with slave address: 7'h%x,1'b%d\n",inbuffer_ibi[0]>>1,inbuffer_slave[0]&0x01);
					fprintf(master_IbiResp_FileObj,"\tRequest detected from I3C device with slave address: 7'h%x,1'b%d\n",inbuffer_ibi[0]>>1,inbuffer_slave[0]&0x01);
					if((ibi_data_length == 2) && (inbuffer_ibi[1] == 0x01)) {
						printf("\tMaster didn't acknowledge this IBI request\n");
						fprintf(master_IbiResp_FileObj,"\tMaster didn't acknowledge this IBI request\n");
					}
					else {
						if(inbuffer_ibi[1] == 0x00){
							printf("\tMaster acknowledged this IBI request\n");
							fprintf(master_IbiResp_FileObj,"\tMaster acknowledged this IBI request\n");
						}
						for(index = 2; index < ibi_data_length; index++){
							printf("\tpayload byte[%03d]: 0x%02x\n",index-1,inbuffer_ibi[index]);
							fprintf(master_IbiResp_FileObj,"\tpayload byte[%03d]: 0x%02x\n",index-1,inbuffer_ibi[index]);
						}
					}
					poll_mesg=1;
				}
				else if (ibi_resp_command == I3C_MASTER_IBI_RESPONSE_BYTE0_IS_HOTJOIN_REQ)
				{
					printf("	This IBI request was recognized by I3C master as HOT JOIN request\n");
					fprintf(master_IbiResp_FileObj,"	This IBI request was recognized by I3C master as HOT JOIN request\n");
					printf("\tRequest detected from I3C device with slave address: 7'h%x,1'b%d\n",inbuffer_ibi[0]>>1,inbuffer_slave[0]&0x01);
					fprintf(master_IbiResp_FileObj,"\tRequest detected from I3C device with slave address: 7'h%x,1'b%d\n",inbuffer_ibi[0]>>1,inbuffer_slave[0]&0x01);
					if((ibi_data_length == 2) && (inbuffer_ibi[1] == 0x01)) {
						printf("\tMaster didn't acknowledge this IBI request\n");
						fprintf(master_IbiResp_FileObj,"\tMaster didn't acknowledge this IBI request\n");
					}
					else {
						if(inbuffer_ibi[1] == 0x00){
							printf("\tMaster acknowledged this IBI request\n");
							fprintf(master_IbiResp_FileObj,"\tMaster acknowledged this IBI request\n");
						}
						for(index = 2; index < ibi_data_length; index++){
							printf("\tpayload byte[%03d]: 0x%02x\n",index-1,inbuffer_ibi[index]);
							fprintf(master_IbiResp_FileObj,"\tpayload byte[%03d]: 0x%02x\n",index-1,inbuffer_ibi[index]);
						}
					}
					poll_mesg=1;
				}
				else if (ibi_resp_command == I3C_MASTER_IBI_RESPONSE_BYTE0_IS_SEC_MASTR_REQ)
				{
					printf("	This IBI request was recognized by I3C master as secondary master request\n");
					fprintf(master_IbiResp_FileObj,"	This IBI request was recognized by I3C master as secondary master request\n");
					printf("\tRequest detected from I3C device with slave address: 7'h%x,1'b%d\n",inbuffer_ibi[0]>>1,inbuffer_slave[0]&0x01);
					fprintf(master_IbiResp_FileObj,"\tRequest detected from I3C device with slave address: 7'h%x,1'b%d\n",inbuffer_ibi[0]>>1,inbuffer_slave[0]&0x01);
					if((ibi_data_length == 2) && (inbuffer_ibi[1] == 0x01)) {
						printf("\tMaster didn't acknowledge this IBI request\n");
						fprintf(master_IbiResp_FileObj,"\tMaster didn't acknowledge this IBI request\n");
					}
					else {
						if(inbuffer_ibi[1] == 0x00){
							printf("\tMaster acknowledged this IBI request\n");
							fprintf(master_IbiResp_FileObj,"\tMaster acknowledged this IBI request\n");
						}
						for(index = 2; index < ibi_data_length; index++){
							printf("\tpayload byte[%03d]: 0x%02x\n",index-1,inbuffer_ibi[index]);
							fprintf(master_IbiResp_FileObj,"\tpayload byte[%03d]: 0x%02x\n",index-1,inbuffer_ibi[index]);
						}
					}					
					poll_mesg=1;
				}
				else {
					printf("IBI response not recognized\n ");
					fprintf(master_IbiResp_FileObj,"IBI response not recognized\n ");
				}
				
				printf("\n	---------------------------------------------\n");
				fprintf(master_IbiResp_FileObj,"\n	---------------------------------------------\n");
				printf("	---------------------------------------------\n");
				fprintf(master_IbiResp_FileObj,"	---------------------------------------------\n");
				
			master_ibi_resp =IBI_IDLE;
			break;
		}
		default:
			master_ibi_resp =IBI_IDLE;
		}
		// IBI_nCMD = 0;
	}
	else {
		switch(master_resp) {	
			case IDLE  :
			{	
				master_resp = IDLE;
				if(data_valid)
					master_resp = CMD;
				break;
			}
			case CMD  :
			{	
				master_resp = CMD;
				outBuffer_Response_buffer[2]=0x03;
				sizeToTransfer = 5;  
				sizeTransferred = 0;
				// printf("Data popped CMD\n");
				status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_Response_buffer, sizeToTransfer, &sizeTransferred,
					SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
					SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
					SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
				APP_CHECK_STATUS(status);
				resp_command=*(inBuffer + 4);
				master_resp =CMD_LEN;
				fprintf(master_CmdResp_FileObj," Resp :8'h%02x\n",resp_command);
				break;
			}
			case CMD_LEN:
			{	
				master_resp = CMD_LEN;
				if(data_valid){
					// printf("Data popped CMD_LEN\n");
					outBuffer_Response_buffer[2]=0x03;
					sizeToTransfer = 5;  
					sizeTransferred = 0;
					status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_Response_buffer, sizeToTransfer, &sizeTransferred,
						SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
						SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
						SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
					APP_CHECK_STATUS(status);
					data_length=*(inBuffer + 4);
					master_resp =DATA;
					fprintf(master_CmdResp_FileObj,"data_len : 8'h%02x\n",data_length);
				}
				slave_resp_rcvd_data = 0;
				break;
			}
			case DATA  :
			{
				master_resp =DATA;
				if(data_valid){
					outBuffer_Response_buffer[2]=((uint32)data_length<<1)|0x01;
					sizeToTransfer = 4+(uint32)data_length;
					sizeTransferred = 0;
					// printf("Data popped DATA\n");
					status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_Response_buffer, sizeToTransfer, &sizeTransferred,
						SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
						SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
						SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
					APP_CHECK_STATUS(status);
					while(slave_resp_rcvd_data != data_length){
						inbuffer_slave[slave_resp_rcvd_data] = *(inBuffer + 4 + slave_resp_rcvd_data);
						fprintf(master_CmdResp_FileObj,"data[%d]: 8'h%02x\n",slave_resp_rcvd_data,inbuffer_slave[slave_resp_rcvd_data]);
						slave_resp_rcvd_data++;
					}
					CCC_CODE=*(inBuffer + 4);
					master_resp = ECHO_RESPONSE;
					fprintf(master_CmdResp_FileObj,"--------------------------------\n");
					fprintf(master_CmdResp_FileObj,"--------------------------------\n");
				}
				break;
			}
			case ECHO_RESPONSE  :
			{
				
				printf("\n	---------------------------------------------\n");
				fprintf(master_CmdResp_FileObj,"\n---------------------------------------------\n");
				printf("	---------------------------------------------\n");
				fprintf(master_CmdResp_FileObj,"---------------------------------------------\n");
				if(resp_command == I3C_MASTER_RESPONSE_BYTE0_IS_PRIVATE_MESSAGE)
				{
					printf("	This is a I3C private ");
					if(inbuffer_slave[0] & 0x01){
						printf("read ");
						fprintf(master_CmdResp_FileObj,"read ");
					}
					else {
						printf("write ");
						fprintf(master_CmdResp_FileObj,"write ");
					}
					printf("message response\n");
					fprintf(master_CmdResp_FileObj,"message response\n");
					printf("\tI3C master tried to establish contact with slave address: 7'h%x\n",inbuffer_slave[0]>>1);
					fprintf(master_CmdResp_FileObj,"\tI3C master tried to establish contact with slave address: 7'h%x\n",inbuffer_slave[0]>>1);
					if((data_length == 2) && (inbuffer_slave[1] == 0x01)) {
						printf("\tSlave with address 7'h%02x failed to acknowledge this message\n",inbuffer_slave[0]>>1);
						fprintf(master_CmdResp_FileObj,"\tSlave with address 7'h%02x failed to acknowledge this message\n",inbuffer_slave[0]>>1);
					}
					else {
						printf("\tSlave with address 7'h%x acknowledged this message\n",inbuffer_slave[0]>>1);
						fprintf(master_CmdResp_FileObj,"\tSlave with address 7'h%x acknowledged this message\n",inbuffer_slave[0]>>1);
						fprintf(master_CmdResp_FileObj,"\n");
						printf("\n");
						for(index = 1; index < data_length -1; index++){
							if(inbuffer_slave[0] & 0x01){
								printf("\tPvt.Read byte[%03d]: 0x%02x\n",index-1,inbuffer_slave[index]);
								fprintf(master_CmdResp_FileObj,"\tPvt.Read byte[%03d]: 0x%02x\n",index-1,inbuffer_slave[index]);
							}
							else{
								printf("\tPvt.Write byte[%03d]: 0x%02x\n",index-1,inbuffer_slave[index]);
								fprintf(master_CmdResp_FileObj,"\tPvt.Write byte[%03d]: 0x%02x\n",index-1,inbuffer_slave[index]);
							}
						}
					}
					poll_mesg=1;
				}
				else if (resp_command == I3C_MASTER_RESPONSE_BYTE0_IS_BROADCAST_CCC)
				{
					printf("	This is a Broadcast CCC message response\n");
					fprintf(master_CmdResp_FileObj,"This is a Broadcast CCC message response\n");
					printf("\tBroadcast CCC transmitted: 8'h%02x\n",inbuffer_slave[0]);
					fprintf(master_CmdResp_FileObj,"Broadcast CCC transmitted: 8'h%02x\n",inbuffer_slave[0]);
					if((data_length == 2) && (inbuffer_slave[1] == 0x01)) {
						printf("\tNo slave acknowledged 7'h7EW\n");
						fprintf(master_CmdResp_FileObj,"No slave acknowledged 7'h7EW\n");
					}
					else {
						for(index = 1; index < data_length -1; index++){
							printf("\tpayload byte[%03d]: 0x%02x\n",index - 1,inbuffer_slave[index]);
							fprintf(master_CmdResp_FileObj,"\tpayload byte[%03d]: 0x%02x\n",index - 1,inbuffer_slave[index]);
						}
					}
					if(inbuffer_slave[0] == RSTDAA){
						dct_valid = 0;
						reset_dct();
						//deldevice_characteristic_table_t(dct);//msd
					}
					else if(inbuffer_slave[0] == ENTDAA){
						printf("\tBroadcast CCC transmitted was ENTDAA\n");
						fprintf(master_CmdResp_FileObj,"Broadcast CCC transmitted was ENTDAA\n");
						if((data_length == 2) && (inbuffer_slave[1] == 0x01)) {
							printf("\tNo slave acknowledged 7'h7EW\n");
							fprintf(master_CmdResp_FileObj,"\tNo slave acknowledged 7'h7EW\n");
						}
						
						else 
						{
								slaves_count=((data_length - 2)/9);
								printf("\tNumber of slaves detected during ENTDAA: %d\n", slaves_count);
								fprintf(master_CmdResp_FileObj,"Number of slaves detected during ENTDAA: %d\n", slaves_count);
								
								for(index = 0;index < slaves_count;index++)
								{
									pid5		   = inbuffer_slave[(index*9)+1];
									pid4           = inbuffer_slave[(index*9)+2];
									pid3           = inbuffer_slave[(index*9)+3];
									pid2           = inbuffer_slave[(index*9)+4];
									pid1           = inbuffer_slave[(index*9)+5];
									pid0           = inbuffer_slave[(index*9)+6];
									bcr            = inbuffer_slave[(index*9)+7];
									dcr            = inbuffer_slave[(index*9)+8];
									dynamic_addr   = inbuffer_slave[(index*9)+9]>>1;
									static_addr    = 0x00;
									create_slave_node_end(pid5,pid4,pid3,pid2,pid1,pid0,bcr,dcr,dynamic_addr,static_addr);
								//CAM_FOR_BCR
							
									sizeToTransfer = 4;
									sizeTransferred = 0;
									// printf("Data popped DATA\n");
									for(index2= 0; index2 < 2; index2++){
										CAM_for_CR_buffer_arr[index2] = CAM_for_CR_buffer[index2];
									}
									//for(index=0;index< slaves_count;index++)
									//{
									CAM_for_CR_buffer_arr[2]=dynamic_addr;
									CAM_for_CR_buffer_arr[3]=bcr;
									status = SPI_ReadWrite(ftHandle, inBuffer, CAM_for_CR_buffer_arr, sizeToTransfer, &sizeTransferred,
										SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
										SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
										SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
									APP_CHECK_STATUS(status);
									//}
								}
								
								print_dct();
								dct_valid = 1;
								
						}
					}
					else if(inbuffer_slave[0] == 0x00)
					{
					//bccc enec response
					for(index = 0; index < 3; index++){
					slave_congig_pointer[index] = slave_config_data[index];
					}
					slave_congig_pointer[1]=0x14;slave_congig_pointer[3]=0x86;
					sizeToTransfer = 4;
					sizeTransferred = 0;
					status = SPI_ReadWrite(ftHandle, inBuffer, slave_congig_pointer, sizeToTransfer, &sizeTransferred,
						SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
						SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
						SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
					APP_CHECK_STATUS(status);
					}
					else if(inbuffer_slave[0] == 0x01)
					{
					//bccc dsec response
					for(index = 0; index < 3; index++){
					slave_congig_pointer[index] = slave_config_data[index];
					}
					slave_congig_pointer[1]=0x14;slave_congig_pointer[3]=0x87;
					sizeToTransfer = 4;
					sizeTransferred = 0;
					status = SPI_ReadWrite(ftHandle, inBuffer, slave_congig_pointer, sizeToTransfer, &sizeTransferred,
						SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
						SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
						SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
					APP_CHECK_STATUS(status);
					}
					// else {
						// printf("\tBroadcast CCC transmitted: 8'h%02x\n",inbuffer_slave[0]);
						// fprintf(master_CmdResp_FileObj,"Broadcast CCC transmitted: 8'h%02x\n",inbuffer_slave[0]);
						// if((data_length == 2) && (inbuffer_slave[1] == 0x01)) {
							// printf("\tNo slave acknowledged 7'h7EW\n");
							// fprintf(master_CmdResp_FileObj,"No slave acknowledged 7'h7EW\n");
						// }
						// else {
							// for(index = 1; index < data_length -1; index++){
								// printf("\tpayload byte[%d]: 0x%02x\n",index - 1,inbuffer_slave[index]);
								// fprintf(master_CmdResp_FileObj,"\tpayload byte[%d]: 0x%02x\n",index - 1,inbuffer_slave[index]);
							// }
						// }
					// }
					poll_mesg=1;
				}
				else if (resp_command == I3C_MASTER_RESPONSE_BYTE0_IS_DIRECT_CCC)
				{
					printf("	This is a Direct CCC message response\n");
					printf("\tDirect CCC transmitted: 8'h%02x\n",inbuffer_slave[0]);
					fprintf(master_CmdResp_FileObj,"Direct CCC -resp\n");
					
					if((data_length == 3) && (inbuffer_slave[2] == 0x01)) {
							printf("\tNo slave acknowledged 7'h7EW or dynamic address 7'h%0x\n",inbuffer_slave[1]>>1);
					}
					else {
						/*for(index = 1; index < data_length -1; index++){
							printf("\t payload byte[%d]: 0x%02x\n",index -1,inbuffer_slave[index]);
						}*/
						
						
						printf("\n	Slave Dynamic address: 0x%02x\n\n",inbuffer_slave[1]>>1);
						fprintf(master_CmdResp_FileObj,"\n Slave Address: 0x%02x\n",inbuffer_slave[1]);
						for(index = 2; index < data_length -1; index++){
							
							printf("	payload byte[%03d]: 0x%02x\n",index -2,inbuffer_slave[index]);
							fprintf(master_CmdResp_FileObj,"\n  payload byte[%03d]: 0x%02x\n",index -2,inbuffer_slave[index]);
						}
						if(inbuffer_slave[index]){
							printf("\tError occur while Transmission of this Direct CCC message\n");
							fprintf(master_CmdResp_FileObj,"\tError occur while Transmission of this Direct CCC message\n");
						}
					}
					
					if(CCC_CODE ==0x80)
					{
						for(index = 0; index < 3; index++){
							slave_congig_pointer[index] = slave_config_data[index];
						}
						slave_congig_pointer[1]=0x14;slave_congig_pointer[3]=0x86;
						sizeToTransfer = 4;
						sizeTransferred = 0;
						status = SPI_ReadWrite(ftHandle, inBuffer, slave_congig_pointer, sizeToTransfer, &sizeTransferred,
							SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
						APP_CHECK_STATUS(status);
					}
					else if(CCC_CODE ==0x81)
					{
						printf("	DCCC ENEC Response\n");
						fprintf(master_CmdResp_FileObj,"DCCC ENEC Response\n");
						for(index = 0; index < 3; index++){
							slave_congig_pointer[index] = slave_config_data[index];
						}
						slave_congig_pointer[1]=0x14;slave_congig_pointer[3]=0x87;
						sizeToTransfer = 4;
						sizeTransferred = 0;
						status = SPI_ReadWrite(ftHandle, inBuffer, slave_congig_pointer, sizeToTransfer, &sizeTransferred,
							SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
						APP_CHECK_STATUS(status);
					}
					else if(CCC_CODE==0x86){
					//DCCC rstdaa
						printf("	RSTDAA Response\n");
						fprintf(master_CmdResp_FileObj,"RSTDAA Response\n");
							rstdaa_dynamic_addr=inbuffer_slave[1]>>1;
							if((data_length == 3)  && (inbuffer_slave[2] == 0x00)){
								delete(rstdaa_dynamic_addr);
								print_dct();
							}
					}
					else if(CCC_CODE==0x87){
						//DCCC setdasa
						printf("	SETDASA Response\n");
						fprintf(master_CmdResp_FileObj,"SETDASA Response\n");
							setdasa_static_addr=inbuffer_slave[1]>>1;
							// setdasa_static_addr=setdasa_static_addr[1]<<1;
							setdasa_dynamic_addr=inbuffer_slave[2]>>1;
							// setdasa_dynamic_addr=setdasa_dynamic_addr<<1;
							if((data_length > 3) && (inbuffer_slave[data_length - 1] == 0x00)){
								create_slave_node_end(0,0,0,0,0,0,0,0,setdasa_dynamic_addr,setdasa_static_addr);
								print_dct();
							}
						
					}
					else if(CCC_CODE==0x88){
						//DCCC setnewda
						printf("	SETNEWDA Response\n");
						fprintf(master_CmdResp_FileObj,"DCCC SETNEWDA Response\n");
							setnewda_old_dynamic_addr=inbuffer_slave[1]>>1;
							setnewda_new_dynamic_addr=inbuffer_slave[2]>>1;
							// change_dynamic_addr(target_slave_addr,setnewda_dyna_addr_by_user);
							// change_dynamic_addr(target_slave_addr,setnewda_new_dynamic_addr);
							if((data_length > 3) && (inbuffer_slave[data_length - 1] == 0x00)){
								// printf("\t setnewda_old_dynamic_addr : 0x%02x\n",setnewda_old_dynamic_addr);
								// printf("\t setnewda_new_dynamic_addr :0x%02x\n",setnewda_new_dynamic_addr);
								change_dynamic_addr(setnewda_old_dynamic_addr,setnewda_new_dynamic_addr);
								print_dct();
							}
					}
						// else if(CCC_CODE==0x88)
					// {
						// printf("	SETNEWDA\n");
						// change_dynamic_addr(target_slave_addr,setnewda_dyna_addr_by_user);
						// printf("\t target_slave_addr : 0x%02x\n",target_slave_addr);
						// printf("\t setnewda_dyna_addr_by_user :0x%02x\n",setnewda_dyna_addr_by_user);
						// print_dct();
					// }
					else if(CCC_CODE==0x8d)
					{
						//pid
						printf("	GETPID Response\n");
						fprintf(master_CmdResp_FileObj,"DCCC GETPID Response\n");
						get_dynamic_addr=inbuffer_slave[1]>>1;
						if((data_length > 3) && (inbuffer_slave[data_length - 1] == 0x00)){
							change_pid(get_dynamic_addr,&inbuffer_slave[2]);
							// get_pid1=inbuffer_slave[2];
							// get_pid2=inbuffer_slave[3];
							// get_pid3=inbuffer_slave[4];
							// get_pid4=inbuffer_slave[5];
							// get_pid5=inbuffer_slave[6];
							// get_pid6=inbuffer_slave[7];
							print_dct();
						}
						
					}
					else if(CCC_CODE==0x8e)
					{
						printf("	GETBCR Response\n");
						fprintf(master_CmdResp_FileObj,"DCCC GETBCR Response\n");
						get_dynamic_addr=inbuffer_slave[1]>>1;
						if((data_length > 3) && (inbuffer_slave[data_length - 1] == 0x00)){
							change_bcr(get_dynamic_addr,inbuffer_slave[2]);
							print_dct();
						//CAM_FOR_BCR
						
							sizeToTransfer = 5;
							sizeTransferred = 0;
							// printf("Data popped DATA\n");
							for(index = 0; index < 2; index++){
								CAM_for_CR_buffer_arr[index] = CAM_for_CR_buffer[index];
							}
							//for(index=0;index< slaves_count;index++)
							//{
							CAM_for_CR_buffer_arr[2]=get_dynamic_addr;
							CAM_for_CR_buffer_arr[3]=inbuffer_slave[2];
							status = SPI_ReadWrite(ftHandle, inBuffer, CAM_for_CR_buffer_arr, sizeToTransfer, &sizeTransferred,
								SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
								SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
								SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
							APP_CHECK_STATUS(status);
							//}
						}
					}
					else if(CCC_CODE==0x8f)
					{
						printf("	GETDCR Response\n");
						fprintf(master_CmdResp_FileObj,"DCCC GETDCR Response\n");
						get_dynamic_addr=inbuffer_slave[1]>>1;
						if((data_length > 3) && (inbuffer_slave[data_length - 1] == 0x00)){
							change_dcr(get_dynamic_addr,inbuffer_slave[2]);
							print_dct();
						}
					}
				
					poll_mesg=1;
				}
				else if (resp_command == I3C_MASTER_RESPONSE_BYTE0_IS_LEGACY_I2C_MESSAGE)
				{
					printf("	This is a Legacy I2C ");
					fprintf(master_CmdResp_FileObj,"	This is a Legacy I2C ");
					if(inbuffer_slave[0] & 0x01){
						printf("read ");
						fprintf(master_CmdResp_FileObj,"read ");
					}
					else {
						printf("write ");
						fprintf(master_CmdResp_FileObj,"write ");
					}
					printf("message response\n");
					fprintf(master_CmdResp_FileObj,"message response\n");
					printf("\tI3C master tried to establish contact with slave address 7'h%02x\n",inbuffer_slave[0]>>1);
					fprintf(master_CmdResp_FileObj,"\tI3C master tried to establish contact with slave address 7'h%02x\n",inbuffer_slave[0]>>1);
					if((data_length == 2) && (inbuffer_slave[1] == 0x01)) {
						printf("\tSlave with address 7'h%02x failed to acknowledge this message\n",inbuffer_slave[0]>>1);
						fprintf(master_CmdResp_FileObj,"\tSlave with address 7'h%02x failed to acknowledge this message\n",inbuffer_slave[0]>>1);
					}
					else {
						printf("\tSlave with address 7'h%02x acknowledged this message\n",inbuffer_slave[0]>>1);
						fprintf(master_CmdResp_FileObj,"\tSlave with address 7'h%02x acknowledged this message\n",inbuffer_slave[0]>>1);
						
						/*setdasa_static_addr=inbuffer_slave[0]>>1;
						// setdasa_static_addr=setdasa_static_addr<<1;
						setdasa_dynamic_addr=0x00;
						create_slave_node_end(0,0,0,0,0,0,0,0,setdasa_dynamic_addr,setdasa_static_addr);
						print_dct();
						*/
						
				printf("  -----------------------------------------------------------------------\n");
				printf("  -----------------------------------------------------------------------\n");
				printf(" \n");
				fprintf(master_CmdResp_FileObj,"  -----------------------------------------------------------------------\n");
				fprintf(master_CmdResp_FileObj,"\n");
							for(index = 1; index < data_length -1; index++){
							if(inbuffer_slave[0] & 0x01){
								printf("\tI2C.Read byte[%d]: 0x%02x\n",index-1,inbuffer_slave[index]);
								fprintf(master_CmdResp_FileObj,"\tI2C.Read byte[%03d]: 0x%02x\n",index-1,inbuffer_slave[index]);
							}
							else{
								printf("\tI2C.Write byte[%d]: 0x%02x\n",index-1,inbuffer_slave[index]);
								fprintf(master_CmdResp_FileObj,"\tI2C.Write byte[%03d]: 0x%02x\n",index-1,inbuffer_slave[index]);
							}
						}
						// for(index = 2; index < data_length -1; index++){
							
							// printf("	payload byte[%03d]: 0x%02x\n",index -2,inbuffer_slave[index]);
							// fprintf(master_CmdResp_FileObj,"\n  payload byte[%03d]: 0x%02x\n",index -2,inbuffer_slave[index]);
						// }
					}
					
					poll_mesg=1;
				}
				// else {
					// printf("Command not matched\n ");
				// }
				
				
			master_resp =IDLE;
			break;
		}
		default:
			master_resp =IDLE;
		}
		// IBI_nCMD = 1;
	}
	data_valid = 0;
	RESP_BUFFR_NOT_FREE = 0;
	return;
}
FTDI_API FT_STATUS FTDI_SPI_ReadWrite(FT_HANDLE handle, uint8 *inBuffer,
	uint8 *outBuffer, uint32 sizeToTransfer, uint32 *sizeTransferred,
	uint32 transferOptions)
{
	CMD_BUFFR_NOT_FREE = 1;
	FT_STATUS status = FT_OK;
	while(RESP_BUFFR_NOT_FREE);
	status = SPI_ReadWrite(handle, inBuffer,
				outBuffer, sizeToTransfer, sizeTransferred,
				transferOptions);
				// fprintf(master_CmdResp_FileObj," SPI_WRITE_DATA:");
				// for(index=0;index<sizeToTransfer;index++){
						// fprintf(master_CmdResp_FileObj,"8'h%02x,",outBuffer[index]);
						// fprintf(master_CmdResp_FileObj,"\t");
					// }
					// fprintf(master_CmdResp_FileObj,"\n");
	CMD_BUFFR_NOT_FREE = 0;
	return status;
}