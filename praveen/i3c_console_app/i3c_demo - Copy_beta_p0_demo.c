/*!
* \file sample_lhy.c
*/
/******************************************************************************/
/* 							 Include files										   */
/******************************************************************************/
/* Standard C libraries */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <conio.h>
#include <signal.h>
/* OS specific libraries */
#ifdef _WIN32
#include<windows.h>
#include <process.h>
#endif
/* Include D2XX header*/
#include "ftd2xx.h"
/* Include libMPSSE header */
#include "libMPSSE_spi.h"
/******************************************************************************/
/*								Macro and type defines							   */
/******************************************************************************/
/* Helper macros */
#define APP_CHECK_STATUS(exp) {if(exp!=FT_OK){printf("%s:%d:%s(): status(0x%x) \
!= FT_OK\n",__FILE__, __LINE__, __FUNCTION__,exp);exit(1);}else{;}};
#define CHECK_NULL(exp){if(exp==NULL){printf("%s:%d:%s():  NULL expression \
encountered \n",__FILE__, __LINE__, __FUNCTION__);exit(1);}else{;}};
/* Application specific macro definations */

/******************************************************************************/
/*								Global variables							  	    */
/******************************************************************************/
#define I3C_MASTER_RESPONSE_BYTE0_IS_LEGACY_I2C_MESSAGE	0x00
#define I3C_MASTER_RESPONSE_BYTE0_IS_PRIVATE_MESSAGE	0x01
#define I3C_MASTER_RESPONSE_BYTE0_IS_BROADCAST_CCC		0x02
#define I3C_MASTER_RESPONSE_BYTE0_IS_DIRECT_CCC			0x03
#define COMMAND_FAILED									0x01
#define COMMAND_SUCCESS									0x00
static FT_HANDLE ftHandle;
uint8 read_Buffer[100];
uint8 inBuffer[128];
uint8 *outBuffer_slave_isr_req='\0';
uint8 outBuffer_rw[128];
uint8 outBuffer_ISR_req_slave[] = {0x00,0x14,0x00,0x1c};
uint8 outBuffer_ISR_status_slave[] = {0x00,0x23,0x03,0x00,0x00};
uint8 outBuffer_write[] = {0x00,0x14,0x00,0xf6,0x11,0x22,0x33,0x44,0x55};
uint8 outBuffer_read_ID[] = {0x00,0xff,0x03,0x00,0x00};//last two 00's are dummys
//BCC buffer Commands
//msb,lsb,write_cmd,cmd_descriptor,ccc code,no.of slaves,data length
uint8 *outbuffer_bcc;

uint8 outBuffer_ENEC[] = {0x00,0x14,0x00,0x1a,0x00,0x01,0x01};
uint8 outBuffer_DISEC[] = {0x00,0x14,0x00,0x1a,0x01,0x01,0x01};
uint8 outBuffer_ENTAS0[] = {0x00,0x14,0x00,0x1a,0x02,0x01,0x00};
uint8 outBuffer_RSTDAA[] = {0x00,0x14,0x00,0x1a,0x06,0x01,0x00};
uint8 outBuffer_ENTDAA[] = {0x00,0x14,0x00,0x1a,0x07,0x01,0x00};
uint8 outBuffer_SETMWL[] = {0x00,0x14,0x00,0x1a,0x09,0x01,0x02};
uint8 outBuffer_SETMRL[] = {0x00,0x14,0x00,0x1a,0x0a,0x01,0x03};
uint8 outBuffer_SETXTIME[] = {0x00,0x14,0x00,0x1a,0x28,0x01,0x02};
///////DCC commands
int dcc_data_len=0;
uint8 *outbuffer_dcc;
uint8 outBuffer_DCC_COMMAND[] = {0x00,0x14,0x00,0x1b,0x80,0x01};
//slave//
uint8 outBuffer_Response_pv_wr_buffer_hdcd[] = {0x01,0x00};
uint8 *outBuffer_Response_pv_wr;
//
//master
uint8 *outbuffer_daa;
uint8 outBuffer_DAA[] = {0x00,0x14,0x00,0x1a,0x07,0x01,0x00};
uint8 outBuffer_Response_buffer[] = {0x00,0x15,0x05,0x00,0x00,0x00};
//PID BIR DIR DAA
uint8 outBuffer_pid_bir_dir_daa[] = {0xfd,0x00,0x25,0x1a,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
uint8 outbuffer_private_hdcd[] = {0x00,0x14,0x00,0x19};
uint8 *outbuffer_private;
uint8 k, l;
//master
int daa_done=0;
int private_read_len = 0;
int private_write_len = 0;
uint8 slave_0_addr = 0;
uint8 slave_1_addr = 0;
uint8 slave_addr = 0;
int slave_isr_len = 0;
int ccc_len=0;
int mresp_len =0;
uint32 sizeToTransfer, sizeTransferred;
unsigned master = 0xff;
unsigned slave_0 = 0xe0;
unsigned slave_1 = 0xe1;
unsigned slave_2 = 0xe2;
uint8 resp_command = 0x00;
uint8 data_length = 0x00;
char slaves_count=0;


void sigfun(int sig)
{
			FT_STATUS status = FT_OK;
			printf("You have presses Ctrl-C , please press again to exit\n");
			status = SPI_CloseChannel(ftHandle);
			printf("*******EXITED..Thank you*******");
			system("exit");
			//system("pause");
	//(void) signal(SIGINT, SIG_DFL);
}
int main()
{
	(void) signal(SIGINT, sigfun);
	int select = 0, device = 0, poll_mesg = 1, rw = 0, rw_loop = 0, rw_len = 0, rw_data = 0, rw_index = 0;
	int ccc = 0;
	int user_data = 0;
	int user_data_1 = 0;
	int user_data_2 = 0;
	int cc = 0;
	FT_STATUS status = FT_OK;
	FT_DEVICE_LIST_INFO_NODE devList = { 0 };
	ChannelConfig channelConf = { 0 };
	uint8 address = 0;
	uint32 channels = 0;
	uint16 data = 0;
	uint8 i = 0;
	uint8 latency = 255;//not affecting flow with this value
	channelConf.ClockRate = 6000000;
	channelConf.LatencyTimer = latency;
	channelConf.configOptions = SPI_CONFIG_OPTION_MODE3 | SPI_CONFIG_OPTION_CS_DBUS4 | SPI_CONFIG_OPTION_CS_ACTIVELOW;
	channelConf.Pin = 0x00000000;/*FinalVal-FinalDir-InitVal-InitDir (for dir 0=in, 1=out)*/

	/* init library */
#ifdef _MSC_VER
	Init_libMPSSE();
#endif
	status = SPI_GetNumChannels(&channels);
	APP_CHECK_STATUS(status);
	printf("***********************************************************\n");
	printf("         WELCOME to LATTICE SEMICONDUCTOR I3C DEMO   \n");
	printf(" ********************************************************\n");
	printf("Number of available SPI channels are: %d\n\n", (int)channels);
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
		// printf("Opening and initialising  channel:	>> %d", cc);
		status = SPI_OpenChannel(cc, &ftHandle);
		APP_CHECK_STATUS(status);
		// printf("\nHandle=0x%x Status=0x%x\n", (unsigned int)ftHandle, status);
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
		// printf("Status=0x%x , SizeTransferred=%u\n", status, sizeTransferred);
		// printf("\n Device ID: 0x%x\n", (unsigned)(*(inBuffer + 4)));
		device=0;
		if ((*(inBuffer + 4)) == master)  { 
			device=1;
			printf(" Connected to I3C master device\n");
			// printf(" Master \n\n ");   
			//printf("Enter 1 to set as AP as Master controller:"); 
		}
		else if ((*(inBuffer + 4)) == slave_0) { 
		device=2;
		// printf(" Slave(0)\n\n");  
		//printf("Enter 2 to set as AP as Slave_0 controller:"); 
		}
		else if ((*(inBuffer + 4)) == slave_1) {
		device=3;
		// printf(" Slave(1)\n\n");  
		//printf("Enter 3 to set as AP as Slave_1 controller:"); 
		}
		else if ((*(inBuffer + 4)) == slave_2) { 
		device=4;
		// printf(" Slave(2)\n\n");  
		//printf("Enter 4 to set as AP as Slave_2 controller:"); 
		}
		else{
		printf("Device ID not matched..\n EXIT");
		return 0;
		}
		printf("\n");
		
		
		if(device==1)
		{
			choose_slave:
			printf("\n***********************************************************\n");
			printf("			I3C MASTER  \n");
			printf(" **********************************************************\n");
			
			
			printf(" Press y to transmit ENTDAA as first command :");
			scanf("%c", &slaves_count);
			if (slaves_count == 'y') 
			{
				sizeToTransfer = 7;
				sizeTransferred = 0;
				outbuffer_daa = outBuffer_DAA;
				outbuffer_daa[5] = 0x01;
				status = SPI_ReadWrite(ftHandle, inBuffer, outbuffer_daa, sizeToTransfer, &sizeTransferred,
					SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
					SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
					SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
				APP_CHECK_STATUS(status);
				printf(">>DAA Command sent\n");
				daa_done=1;
			}
		}
		while(device==1)
		{
		printf("\n***********************************************************\n");
			printf("			I3C MASTER MENU   \n");
			printf(" **********************************************************\n");
			printf(" Your Options are:\n");
			//printf("	1) Configure I3C Master IP\n");
			//printf("	1) Dynamic Address Assignment-DAA\n");
			printf("	2) BCCC Messaging\n");
			printf("	3) DCCC Messaging\n");
			printf("	4) I3C Private Messaging Write\n");
			printf("	5) I3C Private Messaging Read\n");
			printf("	6) Generic Register Read/Write\n");
			printf("	7) EXIT the APP\n");
		
		
		while (device == 1)
		{
			
			if (poll_mesg == 1) 
			{
				printf("@Polling Status register.....\nSelect:");
				//printf("\n");
				poll_mesg = 0;
			}
			select = 0;
			//polling the interrupt status register....
				sizeToTransfer = 5;
				sizeTransferred = 0;
				status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_ISR_status_slave, sizeToTransfer, &sizeTransferred,
					SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
					SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
					SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
				APP_CHECK_STATUS(status);
			if (((*(inBuffer + 4)) & 1 << 2))//isr status
			{
					//read response buffer register
					sizeToTransfer = 6;
					sizeTransferred = 0;
					status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_Response_buffer, sizeToTransfer, &sizeTransferred,
						SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
						SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
						SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
					APP_CHECK_STATUS(status);
					if ((*(inBuffer + 4)) == I3C_MASTER_RESPONSE_BYTE0_IS_LEGACY_I2C_MESSAGE)
					{
						printf(" THIS_IS_LEGACY_I2C_MESSAGE\n");
					}
					else if ((*(inBuffer + 4)) == I3C_MASTER_RESPONSE_BYTE0_IS_PRIVATE_MESSAGE)
					{
						printf(" THIS_IS_PRIVATE_MESSAGE\n");
					}
					else if ((*(inBuffer + 4)) == I3C_MASTER_RESPONSE_BYTE0_IS_BROADCAST_CCC)
					{
						printf(" THIS_IS_BROADCAST_CCC\n");
					}
					else if ((*(inBuffer + 4)) == I3C_MASTER_RESPONSE_BYTE0_IS_DIRECT_CCC)
					{
						printf(" THIS_IS_DIRECT_CCC\n");
					}
					else
					{
						printf(" Invalid response\n");
					}
					printf("	| Received Data length is 0x:%x\n",*(inBuffer + 5));
					mresp_len=*(inBuffer + 5);
					
					for (rw_loop = 0; rw_loop < mresp_len; rw_loop++)
					{
						outBuffer_Response_buffer[2]=0x03;
						sizeToTransfer = 5;
						sizeTransferred = 0;
						status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_Response_buffer,sizeToTransfer, &sizeTransferred,
							SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
						APP_CHECK_STATUS(status);
						printf("payload_byte[%d]: 0x%x \n",rw_loop,*(inBuffer+4));
					}
					if ((*(inBuffer + rw_loop)) == COMMAND_SUCCESS)
					{
						printf("Success: Command Executed.\n");
					}
					else if ((*(inBuffer + rw_loop)) == COMMAND_FAILED)
					{
						printf("Fatal: Command Aborted.\n");
					}
					
					
					
					else if ((*(inBuffer + 4)) == 0x01)//  && (*(inBuffer + 5)) == 0x04)
					{
						printf("\n---------------------------------------------\n");
						printf("***********************************************************\n");
						printf("	Dynamic Address Allocation Done..Master got the Slave address\n");
						printf("	Number of Available SPI Slave are:%x", (*(inBuffer + 5)));
						printf(" **********************************************************\n");
						printf("	Reading PID BIR DIR DAA of Available slaves \n");
						sizeToTransfer = 22;
						sizeTransferred = 0;
						status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_pid_bir_dir_daa, sizeToTransfer, &sizeTransferred,
							SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
						APP_CHECK_STATUS(status);
						printf("	PID_0: %x %x %x %x %x %x", *(inBuffer + 4), *(inBuffer + 5), *(inBuffer + 6), *(inBuffer + 7), *(inBuffer + 8), *(inBuffer + 9));
						printf("	BIR_0: %x", *(inBuffer + 10));
						printf("	DIR_0: %x", *(inBuffer + 11));
						printf("	DAA_slave_0_address: %x", *(inBuffer + 12));
						slave_0_addr = *(inBuffer + 12);
						printf("	PID_1: %x %x %x %x %x %x", *(inBuffer + 13), *(inBuffer + 14), *(inBuffer + 15), *(inBuffer + 16), *(inBuffer + 17), *(inBuffer + 18));
						printf("	BIR_1: %x", *(inBuffer + 19));
						printf("	DIR_1: %x", *(inBuffer + 20));
						printf("	DAA_slave_1_address: %x", *(inBuffer + 21));
						slave_1_addr = *(inBuffer + 21);
						printf("---------------------------------------------\n");
					}
					else if ((*(inBuffer + 4)) == 0x04)//Private write
					{
						printf("\n---------------------------------------------\n");
						printf("	Private Write Message Command\n");
						if ((0x01 & *(inBuffer + 5)) == 1)
						{
							printf("\n	>>Got ACK from SLAVE \n");
						}
						else {
							printf("	>>Got NACK form SLAVE:Data length to be written is:%x \n", *(inBuffer + 5));
						}
						printf("---------------------------------------------\n");
					}
					else if ((*(inBuffer + 4)) == 0x09)//Private Read
					{
						printf("\n---------------------------------------------\n");
						printf("	Private Read Message Command\n");
						if ((0x01 & *(inBuffer + 5)) == 1)
						{
							printf("	Got ACK from SLAVE \n");
							//read response buffer register
							sizeToTransfer = private_read_len - *(inBuffer + 5) + 1;
							sizeTransferred = 0;
							outBuffer_Response_buffer[2] = sizeToTransfer;
							status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_Response_buffer, sizeToTransfer, &sizeTransferred,
								SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
								SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
								SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
							APP_CHECK_STATUS(status);
							printf("Received Data[%d]\n:");
							for (l = 4; l < (sizeToTransfer); l++)
								printf("0x%x   ", l - 4, *(inBuffer + l));
						}
						else {
							printf("	>>Got NACK form SLAVE:Data length to be written is:%x \n", *(inBuffer + 5));
						}
						printf("---------------------------------------------\n");
					}
					poll_mesg = 1;
			}//end of if isr check
			
			else if (kbhit())
			{
					scanf("%d", &select);
			
			if (select == 1) //DAA
			{
				outbuffer_daa[0]=0x00;
				sizeToTransfer = 7;
					status = SPI_ReadWrite(ftHandle, inBuffer, outbuffer_daa, sizeToTransfer, &sizeTransferred,
						SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
						SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
						SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
					APP_CHECK_STATUS(status);
				printf("Status=0x%x , SizeTransferred=%u\n", status, sizeTransferred);
				printf("\n	>>DAA Command Sent");
				poll_mesg=1;
				break;
			}
			else if (select == 2)//bcc
			{
				printf(" Available BCC commands are:\n");
				printf("	1.ENEC\n");
						printf("	2.DISEC\n");
						printf("	3.ENTAS0\n");
						printf("	4.RSTDAA\n");
						printf("	5.ENTDAA\n");
						printf("	6.SETMWL\n");
						printf("	7.SETMRL\n");
						//printf("	8.SETXTIME\n");
						printf("	8.EXIT from this MENU\n");
						printf("  Select the BCC Command:");
						scanf("%d", &ccc);
						if(ccc == 8){poll_mesg = 1;break;}
						if(ccc>8 || ccc < 1){poll_mesg = 1;break;}
						if(ccc==4) daa_done=0;
						if (ccc != 4 && ccc != 5 && ccc != 3)
						{
							printf("	Enter the data to be written	0x:");
							scanf("%x", &user_data);
							printf("\n");
							if (ccc == 6 || ccc == 7)
							{
								printf("	Enter one more byte:	0x:");
								scanf("%x", &user_data_1);
								printf("\n");
							}
							if (ccc == 7) {
								printf("	Enter one more byte:	0x");
								scanf("%x", &user_data_2);
								printf("\n");
							}
						}
						sizeTransferred = 0;
						
							 if (ccc == 1) { outbuffer_bcc = outBuffer_ENEC; sizeToTransfer = 8; outbuffer_bcc[7] = user_data; }
						else if (ccc == 2) { outbuffer_bcc = outBuffer_DISEC; sizeToTransfer = 8; outbuffer_bcc[7] = user_data; }
						else if (ccc == 3) { outbuffer_bcc = outBuffer_ENTAS0; sizeToTransfer = 7;}
						else if (ccc == 4) { outbuffer_bcc = outBuffer_RSTDAA; sizeToTransfer = 7;}
						else if (ccc == 5) { outbuffer_bcc = outBuffer_ENTDAA; sizeToTransfer = 7;}
						else if (ccc == 6) { outbuffer_bcc = outBuffer_SETMWL; sizeToTransfer = 9; outbuffer_bcc[7] = user_data; outbuffer_bcc[8] = user_data_1; }
						else if (ccc == 7) { outbuffer_bcc = outBuffer_SETMRL; sizeToTransfer = 10; outbuffer_bcc[7] = user_data; outbuffer_bcc[8] = user_data_1; outbuffer_bcc[9] = user_data_2;}
						//else if (ccc == 8) { outbuffer_bcc = outBuffer_SETXTIME; sizeToTransfer = 9; outbuffer_bcc[7] = user_data; outbuffer_bcc[8] = user_data_1; }
						outbuffer_bcc[0]=0x00;
						status = SPI_ReadWrite(ftHandle, inBuffer, outbuffer_bcc, sizeToTransfer,&sizeTransferred,
							SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
						APP_CHECK_STATUS(status);
						poll_mesg = 1;
						printf("	BCC command Sent\n");
						break;
						
						
					/*	else{
						printf("Invalid Option..Choose again\n");
						poll_mesg = 1;
						break;
						}*/
			}
			else if (select == 3)//dcc
			{
				outbuffer_dcc=outBuffer_DCC_COMMAND;
						printf("	Entered into DCC\n");
						printf("	Enter the Slave address	0x:");
						scanf("%x", &slave_addr);
						printf("\n");
					/*	printf("Select the Slave:\n1.Slave-%x	\n2.Slave-%x", slave_0_addr, slave_1_addr);
						printf("-->:");
						scanf("%d", &select);
						if (select == 1) { slave_addr = slave_0_addr; }
						else if (select == 2) { slave_addr = slave_1_addr; }	
					*/	
						printf(" Available DCC commands are:\n");
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
						//printf("	13.GETSTATUS\n");
						printf("	13.Go back from this options:\n");
						printf("  Select the DCC Command:\n");
						scanf("%d", &ccc);
						printf("\n");
						outbuffer_dcc[6] = slave_addr << 1;
						if((ccc > 7) && (ccc < 14)){
							outbuffer_dcc[6] |= 0x01;
						}
						if(ccc>13 || ccc < 1){poll_mesg = 1;break;}
						if(ccc==13){poll_mesg = 1;break;}
						if(ccc==4) daa_done=0;
						if (ccc == 1 || ccc == 2 || ccc==5 || ccc==6 || ccc==7)
						{
							printf("	Enter the data to be written	0x:");
							scanf("%x", &user_data);
							printf("\n");
						}
						if(ccc==6 || ccc==7)
						{
						printf("	Enter the data to be written	0x:");
							scanf("%x", &user_data_1);
							printf("\n");
						}
						if(ccc==7)
						{
						printf("	Enter the data to be written	0x:");
							scanf("%x", &user_data_2);
							printf("\n");
						}
						//uint8 outBuffer_DCC_COMMAND[] = {0x00,0x14,0x00,0x1b,0x80,0x01};//0-msb,1-lsb,2-cmd descrptor,3-cmd, 4-ccc , 5-no.of.slaves,6-slave addreess<<1 |0x01 7-data length
						
						
						
						sizeTransferred = 0;
							 if (ccc == 1)  {outbuffer_dcc[4] = 0x80;outbuffer_dcc[7] = 0x01;sizeToTransfer = 9; outbuffer_dcc[8] = user_data; }
						else if (ccc == 2) {outbuffer_dcc[4] = 0x81;outbuffer_dcc[7] = 0x01;sizeToTransfer = 9; outbuffer_dcc[8] = user_data; }
						else if (ccc == 3) {outbuffer_dcc[4] = 0x82;outbuffer_dcc[7] = 0x00;sizeToTransfer = 8;}
						else if (ccc == 4) {outbuffer_dcc[4] = 0x86;outbuffer_dcc[7] = 0x00;sizeToTransfer = 8;}
						else if (ccc == 5) {outbuffer_dcc[4] = 0x88;outbuffer_dcc[7] = 0x01;sizeToTransfer = 9; outbuffer_dcc[8] = user_data; }
						else if (ccc == 11) {outbuffer_dcc[4] = 0x8E;outbuffer_dcc[7] = 0x01;sizeToTransfer = 8;}
						else if (ccc == 12) {outbuffer_dcc[4] = 0x8F;outbuffer_dcc[7] = 0x01;sizeToTransfer = 8;}
						else if (ccc == 6) {outbuffer_dcc[4] = 0x89;outbuffer_dcc[7] = 0x02;sizeToTransfer = 10; outbuffer_dcc[8] = user_data; outbuffer_dcc[9] = user_data_1; }
						else if (ccc == 8) {outbuffer_dcc[4] = 0x8B;outbuffer_dcc[7] = 0x02;sizeToTransfer = 8;}
						//else if (ccc == 13) {outbuffer_dcc[4] = 0x90;outbuffer_dcc[7] = 0x02;sizeToTransfer = 8;}
						else if (ccc == 7) {outbuffer_dcc[4] = 0x8A;outbuffer_dcc[7] = 0x03;sizeToTransfer = 11; outbuffer_dcc[8] = user_data;outbuffer_dcc[9] = user_data_1;outbuffer_dcc[10] = user_data_2;}
						else if (ccc == 9) {outbuffer_dcc[4] = 0x8C;outbuffer_dcc[7] = 0x03;sizeToTransfer = 8;}
						else if (ccc == 10) {outbuffer_dcc[4] = 0x8D;outbuffer_dcc[7] = 0x06;sizeToTransfer = 8;}
						outbuffer_dcc[0]=0x00;
						status = SPI_ReadWrite(ftHandle, inBuffer, outbuffer_dcc, sizeToTransfer, &sizeTransferred,
							SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
						APP_CHECK_STATUS(status);
						poll_mesg = 1;
						printf("	DCC command Sent\n");
						break;
						/*else {
							printf("Invalid Option..Choose again\n");
							poll_mesg = 1;
							break;
						}*/
			}
			else if (select == 4) //private write
			{
						rw_index = 4;
						printf("	Enter how many bytes you want to write to slave(1-127): ");
						scanf("%d", &private_write_len);
						printf("\n");
						printf("	Enter the Slave address	0x:");
						scanf("%x", &slave_addr);
						printf("\n");
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
						outbuffer_private = outbuffer_private_hdcd;
						outbuffer_private[rw_index++] = slave_addr << 1 | 0x00;//write
						outbuffer_private[rw_index++] = private_write_len;//write
						for (rw_loop = 0; rw_loop < private_write_len; rw_loop++)
						{
							printf("	Please enter data[%d]	0x:", rw_loop);
							scanf("%x", &rw_data);
							outbuffer_private[rw_index++] = rw_data;
							printf("\n");
						}
						sizeToTransfer = 6 + private_write_len;
						sizeTransferred = 0;
						status = SPI_ReadWrite(ftHandle, inBuffer, outbuffer_private, sizeToTransfer, &sizeTransferred,
							SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
						APP_CHECK_STATUS(status);
						poll_mesg = 1;
						printf("	Private write command Sent\n");
						break;
			}
			else if (select == 5) //private read
			{		
						rw_index = 4;
						printf("	Enter how many bytes you want to Read from slave(1-127): ");
						scanf("%d", &private_read_len);
						printf("\n");
						printf("	Enter the I3C Slave address	0x:");
						scanf("%x", &slave_addr);
						printf("\n");
						/*choose_sr:
						printf("Select the Slave:\n1.Slave-%x	\n2.Slave-%x", slave_0_addr, slave_1_addr);
						printf("-->:");
						scanf("%d", &select);
						if(select==1||select==2)
						{
							if (select == 1) { slave_addr = slave_0_addr; }
							else if (select == 2) { slave_addr = slave_1_addr; }
							outbuffer_private = outbuffer_private_hdcd;
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
						outbuffer_private = outbuffer_private_hdcd;
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
							printf("	Private Read command Sent\n");
							break;
			}
			else if (select == 6)//generic RW
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
						{
							if (rw == 2)
							{
								rw_index = 0;
								printf("	Enter MSB address	0x:");
								scanf("%x", &rw_data);
								printf("\n");
								outBuffer_rw[rw_index++] = rw_data;
								printf("	Enter LSB address	0x:");
								scanf("%x", &rw_data);
								printf("\n");
								outBuffer_rw[rw_index++] = rw_data;
								printf("	Enter how many bytes of data you want to Write(0-127)	:");
								scanf("%d", &rw_len);
								//hex_var=rw_len;
								printf("\n");
								outBuffer_rw[rw_index++] = 0x00;//write command lsb bit:0
								for (rw_loop = 0; rw_loop < rw_len; rw_loop++)
								{
									printf("	Please enter data[%d]	0x:", rw_loop);
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
								printf("Generic SPI Write Command Sent.   \n");
								poll_mesg = 1;
								break;
							}
							else if (rw == 1)
							{
								rw_index = 0;
								printf("	Enter MSB address 0x:");
								scanf("%x", &rw_data);
								printf("\n");
								outBuffer_rw[rw_index++] = rw_data;
								printf("	Enter LSB address 0x:");
								scanf("%x", &rw_data);
								printf("\n");
								outBuffer_rw[rw_index++] = rw_data;
								printf("	Enter how many bytes of data you want to Read(1-127):");
								scanf("%d", &rw_len);
								printf("\n");
								outBuffer_rw[rw_index++] = (rw_len << 1) | 0x01;;//write command lsb:1
								sizeToTransfer = rw_len + 4;//extra dummy for read
								sizeTransferred = 0;
								status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_rw, sizeToTransfer, &sizeTransferred,
									SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
									SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
									SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
								APP_CHECK_STATUS(status)
									printf("Received Data:\n");
								for (l = 4; l < (sizeToTransfer); l++) {
									printf("0x%x	", *(inBuffer + l));
								}
								//printf("Received Data is: %x\n", *(inBuffer + 4));
								poll_mesg = 1;
								break;
							}
						}//end of if
						else{
							printf("Enter valid Option\n");
							goto choose_grw;
							}
			}
			else if (select == 7)
			{
				status = SPI_CloseChannel(ftHandle);
				break;
				system("exit");
				printf("*******EXITED..Thank you*******");
			}			
			else
			{
					//	select = 0;
						printf("Invalid Option..Choose again\n\n");
						poll_mesg = 1;
						break;
			}
				}//end of else  if codtn keybd
			} // end of master device while loop
		}	//end of master while 1
		while (device == 2 || 3 || 4)
		{
			device=device-1;
			printf(" Connected to I3C slave device --> %d \n",device);
			//printf("***********************************************************\n");
			//printf("         		 I3C SLAVE-(%d) MENU   \n",device);
			//printf("**********************************************************\n");
			//printf(" Your Options are:\n");
			//printf("	1)Slave Interrupt Request \n");
			//printf("	2)Generic Register Read/Write\n");
			//printf("	3)Write in to master read FIFO \n");
			//printf("	4)EXIT the APP\n");
			select = 0;
			while (1)
			{
				if (poll_mesg == 1)
				{
				//	printf(" Connected to I3C slave device --> %d \n",device);
					printf("***********************************************************\n");
					printf("         		 I3C SLAVE-(%d) MENU   \n",device);
					printf("**********************************************************\n");
					printf(" Your Options are:\n");
					//printf("	1)Slave Interrupt Request \n");
					printf("	2)Generic Register Read/Write\n");
					printf("	3)Write in to master read FIFO \n");
					printf("	4)EXIT the APP\n");
					printf("@Polling Status register.....\nSelect:");
					poll_mesg = 0;
				}
				//polling the interrupt status register....
				//outBuffer_Response_buffer[2]=0x05;
				sizeToTransfer = 5;  
				sizeTransferred = 0;
				status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_ISR_status_slave, sizeToTransfer, &sizeTransferred,
					SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
					SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
					SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
				APP_CHECK_STATUS(status);
				//printf("\nReceived Data:");
				//for (l = 4; l < (sizeToTransfer); l++)
					//printf("0x%x ", *(inBuffer + l));
				//printf("\n");
				//if (((*(inBuffer + 4)) >> 2 & 0x01 == 0x01) //isr status
				if (((*(inBuffer + 4)) & 1 << 2))//isr status
				{
						//read response buffer register
						outBuffer_Response_buffer[2]=0x03;
						sizeToTransfer = 5;
						sizeTransferred = 0;
						
						//41 11 10 05  03 31 51
						while(1)
						{
						
						
							status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_Response_buffer, sizeToTransfer, &sizeTransferred,
								SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
								SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
								SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
							APP_CHECK_STATUS(status);
						
							resp_command =*(inBuffer + 4);
							
							if(resp_command == 0x41 ||resp_command == 0x11 ||resp_command == 0x10 ||resp_command ==0x05 
							||resp_command == 0x03 ||resp_command == 0x31 ||resp_command == 0x51)
							{
							break;
							}
						}
					
						sizeToTransfer = 5;
						sizeTransferred = 0;
						status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_Response_buffer, sizeToTransfer, &sizeTransferred,
							SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
						APP_CHECK_STATUS(status);
						
						data_length =*(inBuffer + 4);
						
					if (resp_command == 0x41)// Slave ISR IBI
					{
						printf("\n	---------------------------------------------\n");
						printf("	---------------------------------------------\n");
						printf("	|\n");
						printf("\n	|IBI Response Came\n");
						printf("	|\n");
						if (0x01 & data_length == 1)
						{
							printf("	Got ACK from SLAVE \n");
						}
						printf("	| IBI Payload Left data length is:%x \n",data_length);
						printf("	---------------------------------------------\n");
						printf("	---------------------------------------------\n\n");
						poll_mesg=1;
					}
					else if (resp_command == 0x11) //DAA done
					{
						
						printf("\n	---------------------------------------------\n");
						printf("	---------------------------------------------\n");
						printf("	|\n");
						printf("	|\n");
						printf("	| Received Data length is 0x:%x\n",data_length);
						outBuffer_Response_buffer[2]=0x03;
						sizeToTransfer = 4+(uint32)data_length;
						sizeTransferred = 0;
						status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_Response_buffer,sizeToTransfer, &sizeTransferred,
							SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
						APP_CHECK_STATUS(status);
						printf("	| Slave-> Dynamic Address Assignment Done\n");
						printf("	| Slave-> Dynamic Address Assigned  is 0x:%x \n",*(inBuffer + 4));
						printf("	---------------------------------------------\n");
						printf("	---------------------------------------------\n\n");
						poll_mesg=1;					
					}
					else if (resp_command == 0x10)//daa reset
					{
						printf("\n	---------------------------------------------\n");
						printf("	---------------------------------------------\n");
						printf("	|\n");
						printf("	|\n");
						printf("	| Received Data length is 0x:%x\n",data_length);
						outBuffer_Response_buffer[2]=0x03;
						sizeToTransfer = 4+(uint32)data_length;
						sizeTransferred = 0;
						status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_Response_buffer,sizeToTransfer, &sizeTransferred,
							SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
						APP_CHECK_STATUS(status);
						printf("	| Slave-> DAA RESET\n");
						printf("	| Slave-> Reset Dynamic address  is 0x:%x \n", *(inBuffer + 4));
						printf("	---------------------------------------------\n");
						printf("	---------------------------------------------\n\n");
						poll_mesg=1;
					}
					else if (resp_command == 0x05)//private write slave
					{
						printf("\n-----------------------------------------------------------\n");
						printf("	---------------------------------------------\n");
						//printf("	Private written length in decimal:%d\n",*(inBuffer + 5));
						printf("	Private written length 0x:%x in dec:%d\n",data_length,data_length);
						outBuffer_Response_pv_wr=outBuffer_Response_pv_wr_buffer_hdcd;
						outBuffer_Response_pv_wr[2]=data_length << 1 | 0x01;
						sizeToTransfer = 4+(uint32)data_length;
						sizeTransferred = 0;
						status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_Response_pv_wr, sizeToTransfer, &sizeTransferred,
							SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
						APP_CHECK_STATUS(status);
						printf("	Private Written Data:\n");
						for (l = 4; l < (sizeToTransfer); l++)
							printf("	0x%x\n ",*(inBuffer+l));
						printf("\n");
						printf("---------------------------------------------\n");
						printf("---------------------------------------------\n\n");
					poll_mesg=1;
					}
					else if (resp_command == 0x03)//private read
					{
						printf("\n	---------------------------------------------\n");
						printf("	---------------------------------------------\n");
						printf("	|Private Read\n");
						printf("	|Private Read data length is:%x \n", data_length);
						printf("	---------------------------------------------\n");
						printf("	---------------------------------------------\n\n");
						poll_mesg=1;
					}
					else if ((resp_command == 0x31 )|| (resp_command == 0x51))//BCC or DCC
				    {
					printf("	---------------------------------------------\n");
					printf("	---------------------------------------------\n");
					printf("	| Received Data length is 0x:%x\n",data_length);
					ccc_len=data_length;
					printf("	| Received CCC length is %d\n",ccc_len);
					outBuffer_Response_buffer[2]=0x03;
					sizeToTransfer = 5;
					sizeTransferred = 0;
					status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_Response_buffer,sizeToTransfer, &sizeTransferred,
						SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
						SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
						SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
					APP_CHECK_STATUS(status)
					/*sizeToTransfer = 4+(uint32)*(inBuffer + 5);
					sizeTransferred = 0;
					status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_Response_buffer,sizeToTransfer, &sizeTransferred,
						SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
						SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
						SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
					APP_CHECK_STATUS(status);*/
					printf("	|\n");
					printf("	|\n");
						//BCC
						 if(*(inBuffer + 4)== 0x00){printf("	| BCC ENEC\n");}
					else if(*(inBuffer + 4)== 0x01){printf("	| BCC DISEC\n");}
					else if(*(inBuffer + 4)== 0x02){printf("	| BCC ENTAS0\n");}
					else if(*(inBuffer + 4)== 0x06){printf("	| BCC RSTDAA\n");}
					else if(*(inBuffer + 4)== 0x07){printf("	| BCC ENTDAA\n");}
					else if(*(inBuffer + 4)== 0x09){printf("	| BCC SETMWL\n");}
					else if(*(inBuffer + 4)== 0x0A){printf("	| BCC SETMRL\n");}
					else if(*(inBuffer + 4)== 0x28){printf("	| BCC SETXTIME\n");}
						//DCC
					else if(*(inBuffer + 4)== 0x80){printf("	| DCC ENEC\n");}
					else if(*(inBuffer + 4)== 0x81){printf("	| DCC DISEC\n");}
					else if(*(inBuffer + 4)== 0x82){printf("	| DCC ENTSA0\n");}
					else if(*(inBuffer + 4)== 0x86){printf("	| DCC RSTDAA\n");}		
					else if(*(inBuffer + 4)== 0x88){printf("	| DCC SETNEWDA\n");}
					else if(*(inBuffer + 4)== 0x89){printf("	| DCC SETMWL\n");}
					else if(*(inBuffer + 4)== 0x8A){printf("	| DCC SETMRL\n");}
					else if(*(inBuffer + 4)== 0x8B){printf("	| DCC GETMWL\n");}			
					else if(*(inBuffer + 4)== 0x8C){printf("	| DCC GETMRL\n");}
					else if(*(inBuffer + 4)== 0x8D){printf("	| DCC GETPID\n");}
					else if(*(inBuffer + 4)== 0x8E){printf("	| DCC GETBCR\n");}
					else if(*(inBuffer + 4)== 0x8F){printf("	| DCC GETDCR\n");}
					else if(*(inBuffer + 4)== 0x90){printf("	| DCC GETSTATUS\n");}
					else if(*(inBuffer + 4)== 0x90){printf("	| DCC GETSTATUS\n");}					
					printf("	|Associated Data:");
					for (rw_loop = 0; rw_loop < ccc_len-1; rw_loop++)
					{
						outBuffer_Response_buffer[2]=0x03;
						sizeToTransfer = 5;
						sizeTransferred = 0;
						status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_Response_buffer,sizeToTransfer, &sizeTransferred,
							SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
						APP_CHECK_STATUS(status);
						printf("	0x%x ",*(inBuffer+4));
					}
					printf("	|\n");
					printf("	|\n");
					printf("	---------------------------------------------\n");
					printf("	---------------------------------------------\n");
					poll_mesg=1;
				}
				}//end of //isr status if
				else if (kbhit())
				{
					scanf("%d", &select);
					if (select == 1)
					{
						rw_index = 4;
						printf("\n	---------------------------------------------\n");
						printf("	---------------------------------------------\n");
						printf("	>> Slave Interrupt Request...   \n");
						poll_mesg = 1;
						printf("	Enter how many bytes you want to write to slave:(hex)");
						scanf("%x", &slave_isr_len);
						printf("\n");
						//sending the interrupt request....
						outBuffer_slave_isr_req = outBuffer_ISR_req_slave;
						outBuffer_slave_isr_req[rw_index++] = slave_isr_len;//write
						for (rw_loop = 0; rw_loop < slave_isr_len; rw_loop++)
						{
							printf("	Please enter data[%d](hex)0x:", rw_loop);
							scanf("%x", &rw_data);
							outBuffer_slave_isr_req[rw_index++] = rw_data;
							printf("\n");
						}
						sizeToTransfer = 4 + slave_isr_len;
						sizeTransferred = 0;
						status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_slave_isr_req, sizeToTransfer, &sizeTransferred,
							SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
						APP_CHECK_STATUS(status);
						printf("	Interrupt Request sent to the Master...   \n");
						printf("	---------------------------------------------\n");
						printf("	---------------------------------------------\n\n");
						poll_mesg = 1;
						break;
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
						if (rw == 2)
						{
							rw_index = 0;
							printf("	Enter MSB address	>> 0x:");
							scanf("%x", &rw_data);
							printf("\n");
							outBuffer_rw[rw_index++] = rw_data;
							printf("	Enter LSB address	>> 0x");
							scanf("%x", &rw_data);
							printf("\n");
							outBuffer_rw[rw_index++] = rw_data;
							printf("	Enter how many bytes of data you want to Write(1-127) :");
							scanf("%d", &rw_len);
							printf("\n");
							outBuffer_rw[rw_index++] = 0x00;//write command lsb:0
							for (rw_loop = 0; rw_loop < rw_len; rw_loop++)
							{
								printf("	Please enter data[%d]: 0x", rw_loop);
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
							break;
						}
						else if (rw == 1)
						{
							rw_index = 0;
							printf("	Enter MSB address	>> 0x:");
							scanf("%x", &rw_data);
							printf("\n");
							outBuffer_rw[rw_index++] = rw_data;
							printf("	Enter LSB address	>> 0x:");
							scanf("%x", &rw_data);
							printf("\n");
							outBuffer_rw[rw_index++] = rw_data;
							printf("	Enter how many bytes of data you want to Read(1-127): ");
							scanf("%d", &rw_len);
							printf("\n");
							outBuffer_rw[rw_index++] = (rw_len << 1) | 0x01;//write command lsb:1
							sizeToTransfer = rw_len + 4;
							sizeTransferred = 0;
							status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_rw, sizeToTransfer, &sizeTransferred,
								SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
								SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
								SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
							APP_CHECK_STATUS(status);
								printf("Received Data:");
								for (l = 4; l < (sizeToTransfer); l++)
									printf("	0x%x",*(inBuffer + l));
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
					rw_index=0;
					printf("	Enter how many bytes of data you want to write to master read FIFO(1-127) :");
							scanf("%d", &rw_len);
							printf("\n");
							outBuffer_rw[rw_index++] = 0x02;//msb
						outBuffer_rw[rw_index++] = 0x00;//lsb
							outBuffer_rw[rw_index++] = (rw_len<<1)|0x00;//write command lsb:0
							for (rw_loop = 0; rw_loop < rw_len; rw_loop++)
							{
								printf("	Please enter data[%d]: 0x", rw_loop);
								scanf("%x", &rw_data);
								outBuffer_rw[rw_index++] = rw_data;
								printf("\n");
							}
						sizeToTransfer = rw_len + 4;
						sizeTransferred = 0;
						status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_rw, sizeToTransfer, &sizeTransferred,
							SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
							SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
						APP_CHECK_STATUS(status);
						poll_mesg=1;
					}
					else if(select ==4)
					{
						status = SPI_CloseChannel(ftHandle);
						break;
						system("exit");
						printf("*******EXITED..Thank you*******");
					}
					else
					{
						printf("Enter valid Option:\n");
						poll_mesg=1;
					}

				}//end of if keybd hit
			}//end of slave 1 while(1)
		}//end of while slave-0 changed to if
		status = SPI_CloseChannel(ftHandle);
	}//end if channel<0
#ifdef _MSC_VER
	Cleanup_libMPSSE();
#endif
#ifndef __linux__
	system("pause");
#endif
	return 0;
} //end of main