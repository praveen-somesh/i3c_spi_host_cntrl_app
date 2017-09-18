/*!
* \file sample-static.c
*
* \author FTDI
* \date 20110512
*
* Copyright © 2000-2014 Future Technology Devices International Limited
*
*
* THIS SOFTWARE IS PROVIDED BY FUTURE TECHNOLOGY DEVICES INTERNATIONAL LIMITED ``AS IS'' AND ANY EXPRESS
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL FUTURE TECHNOLOGY DEVICES INTERNATIONAL LIMITED
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*
* Project: libMPSSE
* Module: SPI Sample Application - Interfacing 94LC56B SPI EEPROM
*
* Rivision History:
* 0.1  - 20110512 - Initial version
* 0.2  - 20110801 - Changed LatencyTimer to 255
* 					 Attempt to open channel only if available
*					 Added & modified macros
*					 Included stdlib.h
* 0.3  - 20111212 - Added comments
* 0.41 - 20140903 - Fixed compilation warnings
*					 Added testing of SPI_ReadWrite()
*/
#include "stdafx.h"
/******************************************************************************/
/* 							 Include files										   */
/******************************************************************************/
/* Standard C libraries */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<string>

#include<string>
/* OS specific libraries */
#ifdef _WIN32
#include<windows.h>
#endif

/* Include D2XX header*/
#include "ftd2xx.h"

/* Include libMPSSE header */
#include "libMPSSE_spi.h"
#include"FTDI_SPI_main.h"
#include"Form1.h"



/******************************************************************************/
/*								Macro and type defines							   */
/******************************************************************************/
/* Helper macros */

#define APP_CHECK_STATUS(exp) {if(exp!=FT_OK){printf("%s:%d:%s(): status(0x%x) \
!= FT_OK\n",__FILE__, __LINE__, __FUNCTION__,exp);exit(1);}else{;}};
#define CHECK_NULL(exp){if(exp==NULL){printf("%s:%d:%s():  NULL expression \
encountered \n",__FILE__, __LINE__, __FUNCTION__);exit(1);}else{;}};

/* Application specific macro definations */
#define SPI_DEVICE_BUFFER_SIZE		256
#define SPI_WRITE_COMPLETION_RETRY		10
#define START_ADDRESS_EEPROM 	0x00 /*read/write start address inside the EEPROM*/
#define END_ADDRESS_EEPROM		0x10
#define RETRY_COUNT_EEPROM		10	/* number of retries if read/write fails */
#define CHANNEL_TO_OPEN			0	/*0 for first available channel, 1 for next... */
#define SPI_SLAVE_0				0
#define SPI_SLAVE_1				1
#define SPI_SLAVE_2				2
#define DATA_OFFSET				4
#define USE_WRITEREAD			1

/******************************************************************************/
/*								Global variables							  	    */
/******************************************************************************/
//static FT_HANDLE ftHandle;

//int i3c_gui::poll_loop=1;
static uint8 buffer[SPI_DEVICE_BUFFER_SIZE] = { 0 };
uint32 sizeToTransfer, sizeTransferred;
uint8 inBuffer[100];
uint8 *inBuffer_fun;
uint8 outBuffer_read_ID[] = { 0x00,0x00,0x03,0x00,0x00};//last two 00's are dummyss
unsigned master = 0x20;
unsigned slave_0 = 0x10;
unsigned slave_1 = 0x11;
//Bcc Commands
//new
uint8 outbuffer_bccc[128];
uint8 outBuffer_BCCC_COMMAND[] = {0x00,0x10,0x00,0x1a,0x00,0x01};
uint8 outbuffer_dccc[128];
uint8 outBuffer_DCCC_COMMAND[] = {0x00,0x10,0x00,0x1b,0x80,0x01};
int daa_done=0;
//old
uint8 outBuffer_ENEC[] = { 0x00,0x10,0x00,0x1a,0x00,0x01,0x01 };
uint8 outBuffer_DISEC[] = { 0x00,0x10,0x00,0x1a,0x01,0x01,0x01 };
uint8 outBuffer_ENTAS0[] = { 0x00,0x10,0x00,0x1a,0x02,0x01,0x01 };
uint8 outBuffer_RSTDAA[] = { 0x00,0x10,0x00,0x1a,0x06,0x01,0x00 };
uint8 outBuffer_ENTDAA[] = { 0x00,0x10,0x00,0x1a,0x07,0x01,0x00 };
uint8 outBuffer_SETMWL[] = { 0x00,0x10,0x00,0x1a,0x09,0x01,0x02 };
uint8 outBuffer_SETMTL[] = { 0x00,0x10,0x00,0x1a,0x0a,0x01,0x03 };
uint8 outBuffer_SETXTIME[] = { 0x00,0x10,0x00,0x1a,0x28,0x01,0x02 };

uint8 * read_command(FT_HANDLE ftHandle, uint8* outBuffer_read_ID, uint32 sizeToTransfer)
{
	FT_STATUS status = FT_OK;
	status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_read_ID, sizeToTransfer, &sizeTransferred,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	APP_CHECK_STATUS(status);
	printf("status=0x%x sizeTransferred=%u\n", status, sizeTransferred);
	//for(int l=0;l<(sizeToTransfer/8);l++)
	//	printf("0x%x\n",(unsigned)inBuffer[l]);
	printf("\n");
	//printf("address returing 0x%x\n",(unsigned)inBuffer);
	return inBuffer;
}
void spi_apis::spi_init(System::Windows::Forms::RichTextBox ^ rich_textbox)
{
	char *string;
	FT_STATUS status;
	FT_DEVICE_LIST_INFO_NODE devList = { 0 };
	ChannelConfig channelConf = { 0 };
	uint8 address = 0;
	uint32 channels = 0;
	uint16 data = 0;
	uint8 i = 0;
	uint8 latency = 255;
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
	printf("Number of available SPI channels = %d\n", (int)channels);
	if (channels>0)
	{
		/*	for(i=0;i<channels;i++)
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

	//	for (int cc = 0; cc < channels / 2; cc++)		//here cc is channel counter
		//{
			printf("Opening and initialising  channel");
			status = SPI_OpenChannel(0, &ftHandle);
			APP_CHECK_STATUS(status);
			printf("\nhandle=0x%x status=0x%x\n", (unsigned int)ftHandle, status);
			channelConf.LatencyTimer = 200;
			status = SPI_InitChannel(ftHandle, &channelConf);
			APP_CHECK_STATUS(status);
			//Reading the ID register to distinguish between the boards.
			sizeToTransfer = 5;
			sizeTransferred = 0;
			inBuffer_fun = read_command(ftHandle,outBuffer_read_ID,sizeToTransfer);
//			printf("address 0x%x\n",(unsigned)inBuffer_fun);
//			for(int l=0;l<(sizeToTransfer/8);l++)
//			printf(" ID:	0x%x\n", (unsigned)(*(inBuffer_fun + 3)));
//			String ^ str = this->richTextBox1->Text + " I3C demo..send pressed.";
//			System::Windows::Forms::pkForm->richTextBox1->Text = L" FROM the main.cpp ";			
//			System::Windows::Forms::Form^ pkForm = gcnew Form();  //only one error
//			Form::pkForm->textBox1->Text = L" FROM the main.cpp ";	
//			MyForm1 ^ myform1 = gcnew Form1();
//			Form1::myForm1->textBox1->Text = L" FROM the main.cpp ";
//			System::Windows::Forms::myform1->textBox1->Text = L" FROM the main.cpp ";
//		    MyForm^ myform1 = gcnew MyForm();
//	        Form::richTextBox1->Text = L" FROM the main.cpp ";
			string = "master";
			//spi_apis::send_message(string);
			if ((*(inBuffer_fun + 4)) == master) {  rich_textbox->Text = "Connected to I3C Master\n";}
			else if ((*(inBuffer_fun + 4)) == slave_0 ){ rich_textbox->Text = "Connected to I3C SLAVE\n"; }
			else if ((*(inBuffer_fun + 4)) == slave_1 ) { rich_textbox->Text = "Connected to I3C SLAVE\n";}
			else{ rich_textbox->Text = "Device not matched check the connection.\n";}
			
		//}//end of for loop
		//	 i3c_gui::poll_loop=1;
		//	poll_isr(rich_textbox);
	}
}
int * spi_apis::daa(void)
{
	//#if USE_WRITEREAD
	FT_STATUS status;
	uint8  l;
	uint8 inBuffer[100];
	uint32 sizeToTransfer, sizeTransferred;
	uint8 outBuffer_DAA[] = { 0x00,0x14,0x00,0x4a,0x07,0x01,0x00 };
	sizeToTransfer = 7;
	status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_DAA, sizeToTransfer, &sizeTransferred,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	APP_CHECK_STATUS(status);

	int * obuffer;
	return obuffer;
}

unsigned char * spi_apis::write_command(void) {
	//unsigned char * inbuf;

	//char * str="FT_OK"; //wkng with ft_ok when ftdi is not initialized
	uint8 *str;
	char * str2 = "FT_OK";
	uint8 inBuffer[100];
	uint8 outBuffer_write[] = {0xff,0xff,0x03,0x00,0x00};
	uint32 sizeToTransfer = 0;
	uint32 sizeTransfered=0;
	FT_STATUS status;

	sizeToTransfer = 5;
	sizeTransfered = 0;
	status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_write, sizeToTransfer, &sizeTransfered,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	//APP_CHECK_STATUS(status);

	if (status != FT_OK) {
		str2 = "%s:%d:%s(): status(0x%x) \
!= FT_OK\n";
		return (uint8*)str;
	}
	str = inBuffer;
	//inbuf = inBuffer;
	return str;
}
//unsigned char * spi_apis::send(unsigned char* outBuffer_write,unsigned long sizeToTransfer) {
unsigned char * spi_apis::send(System::String ^str,System::Windows::Forms::RichTextBox ^ rich_textbox) {
	char * str2 = "FT_OK";
	uint8 inBuffer[100];
	//uint8 outBuffer_write[] = {0x00};
	uint8 outBuffer_write[100];
	uint8 *sendbuf;
	uint32 sizeTransfered = 0;
	FT_STATUS status;
	uint32 sizeToTransfer = 0;
	char hai;

	int i =0;
	//System::String ^ tmp_str = str;
	System::String ^ out_tmp_str ="";
	rich_textbox->Text = rich_textbox->Text +"Received string"+str+"\n";

	for(i=0;i<str->Length;i++)
	{
		if((str[i] !=' '))
		{
			out_tmp_str = out_tmp_str + str[i];
		}
	}
	str =out_tmp_str;
	out_tmp_str = "";
	rich_textbox->Text = rich_textbox->Text +"After Removing spaces"+str+"\n";
//	if(str->Length==1)
//	{
//		outBuffer_write[k++] = (0x0f & 0x00 | (0xf0 & (str[i] << 4));
//	}
//	else

		for (int i = 0, j = 1, k = 0; i < str->Length; i)
		{
			//string hexOutput = String.Format("{0:X}", value);

			outBuffer_write[k++] = (0x0f & str[j]) | (0xf0 & (str[i] << 4));
			//outBuffer_write[k++] = BitConverter.ToHex((0x0f & str[j]) | (0xf0 & (str[i] << 4)));
			i = ++j;
			++j;	
		}
//	}


	/*for (int i = 0, j = 1, k = 0,space=1; i < str->Length; i,space++)
	{
		if(space%3 !=0)
		{
			if (str->Length % 2 == 0)
			{
				outBuffer_write[k++] = (0x0f & str[j]) | (0xf0 & (str[i] << 4));
				i = ++j;
				++j;
			}
			else
			{
				if (j!= str->Length)
				{
					outBuffer_write[k++] = (0x0f & str[j]) | (0xf0 & (str[i] << 4));
					i = ++j;
					++j;
				}	
				else
				{
					outBuffer_write[k++] = (0x0f & str[j-1]);
					break;
				}
			}
		}
		else
		{
			i = ++j;
			++j;
		}
	*/

	if (str->Length % 2 == 0)
		sizeToTransfer = str->Length / 2;
	else
		sizeToTransfer = (str->Length / 2) + 1;

	status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_write,sizeToTransfer, &sizeTransfered,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	APP_CHECK_STATUS(status);
	if (status != FT_OK) {
		str2 = "%s:%d:%s(): status(0x%x) \
!= FT_OK\n";
		
	}
	return inBuffer;
}
unsigned char * spi_apis::send_bccc(System::String ^str, int bcc,System::Windows::Forms::RichTextBox ^ rich_textbox) 
{
	char * str2 = "FT_OK";
	uint8 inBuffer[100];
	uint8 *outbuffer_bcc;
	uint8 *sendbuf;
	uint32 sizeTransfered = 0;
	FT_STATUS status;
	uint32 sizeToTransfer = 0;
	if (bcc == 1) { outbuffer_bcc = outBuffer_ENEC; sizeToTransfer = 8; /*	outbuffer_bcc[7] = user_data;*/}
	else if (bcc == 2) { outbuffer_bcc = outBuffer_DISEC; sizeToTransfer = 8;/* outbuffer_bcc[7] = user_data*/}
	else if (bcc == 3) { outbuffer_bcc = outBuffer_ENTAS0; sizeToTransfer = 8;/* outbuffer_bcc[7] = user_data */}
	else if (bcc == 4) { outbuffer_bcc = outBuffer_RSTDAA; sizeToTransfer = 7; }
	else if (bcc == 5) { outbuffer_bcc = outBuffer_ENTDAA; sizeToTransfer = 7; }
	else if (bcc == 6) { outbuffer_bcc = outBuffer_SETMWL; sizeToTransfer = 9;/* outbuffer_bcc[7] = user_data; outbuffer_bcc[8] = user_data_1;*/ }
	else if (bcc == 7) { outbuffer_bcc = outBuffer_SETMTL; sizeToTransfer = 10;/* outbuffer_bcc[7] = user_data; outbuffer_bcc[8] = user_data_1; outbuffer_bcc[9] = user_data_2;*/ }
	else if (bcc == 8) { outbuffer_bcc = outBuffer_SETXTIME; sizeToTransfer = 9; /*outbuffer_bcc[7] = user_data; outbuffer_bcc[8] = user_data_1;*/ }
	else{
		rich_textbox->Text = "Please select any BCCC command \n";
		return 0;

	}
	for (int i = 0, j = 1, k = 7; i < str->Length; i)
	{
		if (str->Length % 2 == 0)
		{
			outbuffer_bcc[k++] = (0x0f & str[j]) | (0xf0 & (str[i] << 4));
			i = ++j;
			++j;
		}
		else
		{
			if (j != str->Length)
			{
				outbuffer_bcc[k++] = (0x0f & str[j]) | (0xf0 & (str[i] << 4));
				i = ++j;
				++j;
			}
			else
			{
				outbuffer_bcc[k++] = (0x0f & str[j - 1]);
				break;
			}
		}
	}
	status = SPI_ReadWrite(ftHandle, inBuffer, outbuffer_bcc, sizeToTransfer, &sizeTransfered,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	APP_CHECK_STATUS(status);
	if (status != FT_OK) {
		str2 = "%s:%d:%s(): status(0x%x) \
!= FT_OK\n";

	}
	return inBuffer;
}
 void spi_apis::send_ccc( System::Windows::Forms::RichTextBox ^ rich_textbox,int ccc,System::String ^ str)
 {
 int index=0;
 int user_data = 0;
 int user_data_1 = 0;
 int user_data_2 = 0;
 int user_data_3 = 0;
 FT_STATUS status;
	 switch(ccc)
	 {
	 
		 case 1:
		 {
			 rich_textbox->Text = rich_textbox->Text +"Broadcast CCC-ENEC\n";
			 break;
		 }
		 case 2:
		 {
			 rich_textbox->Text = rich_textbox->Text +"Broadcast CCC-DISEC\n";
			 break;
		 }
		 case 3:
		 {
			 rich_textbox->Text = rich_textbox->Text +"Broadcast CCC-ENTAS0\n";
			 break;
		 }
		 case 4:
		 {
			 rich_textbox->Text = rich_textbox->Text +"Broadcast CCC-RSTDAA\n";
			 break;
		 }
		 case 5:
		 {
			 rich_textbox->Text = rich_textbox->Text +"Broadcast CCC-ENTDAA\n";
			 break;
		 }
		 case 6:
		 {
			 rich_textbox->Text = rich_textbox->Text +"Broadcast CCC-SETMWL\n";
			 break;
		 }
		 case 7:
		 {
			 rich_textbox->Text = rich_textbox->Text +"Broadcast CCC-SETMRL\n";
			 break;
		 }
	
    }
	sizeTransferred = 0;
	for(index = 0; index < 6; index++){
		outbuffer_bccc[index] = outBuffer_BCCC_COMMAND[index];
	}
	if (ccc == 1) { outbuffer_bccc[2] =0x0A;outbuffer_bccc[4] =0x00; outbuffer_bccc[6] =0x01;outbuffer_bccc[7] =((0x0f & str[1]) | (0xf0 & (str[0] << 4))); sizeToTransfer = 8; }
	else if (ccc == 2) {outbuffer_bccc[2] =0x0A; outbuffer_bccc[4] =0x01;  outbuffer_bccc[6] =0x01;outbuffer_bccc[7] = user_data;sizeToTransfer = 8; }
	else if (ccc == 3) { outbuffer_bccc[2] =0x08;outbuffer_bccc[4] =0x02; outbuffer_bccc[6] =0x00;sizeToTransfer = 7;}
	else if (ccc == 4) { outbuffer_bccc[2] =0x08;outbuffer_bccc[4] =0x06; outbuffer_bccc[6] =0x00;sizeToTransfer = 7;daa_done=0;}
	else if (ccc == 5) { outbuffer_bccc[2] =0x08;outbuffer_bccc[4] =0x07; outbuffer_bccc[6] =0x00;sizeToTransfer = 7;daa_done=1;}
	else if (ccc == 6) { outbuffer_bccc[2] =0x0c;outbuffer_bccc[4] =0x09; outbuffer_bccc[6] =0x02; outbuffer_bccc[7] = user_data; outbuffer_bccc[8] = user_data_1;sizeToTransfer = 9; }
	else if (ccc == 7) { outbuffer_bccc[2] =0x0e;outbuffer_bccc[4] =0x0a; outbuffer_bccc[6] =0x03; outbuffer_bccc[7] = user_data; outbuffer_bccc[8] = user_data_1; outbuffer_bccc[9] = user_data_2;sizeToTransfer = 10;}
		status = SPI_ReadWrite(ftHandle, inBuffer, outbuffer_bccc, sizeToTransfer,&sizeTransferred,
						SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
						SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
						SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
					APP_CHECK_STATUS(status);			
return; 
 }
 void spi_apis::send_dccc( System::Windows::Forms::RichTextBox ^ rich_textbox,int ccc,System::String ^ str)
 {
 int index=0;
 int user_data = 0;
 int user_data_1 = 0;
 int user_data_2 = 0;
 int user_data_3 = 0;
 FT_STATUS status;
	 switch(ccc)
	 {
	 
		 case 1:
		 {
			 rich_textbox->Text = rich_textbox->Text +"Broadcast CCC-ENEC\n";
			 break;
		 }
		 case 2:
		 {
			 rich_textbox->Text = rich_textbox->Text +"Broadcast CCC-DISEC\n";
			 break;
		 }
		 case 3:
		 {
			 rich_textbox->Text = rich_textbox->Text +"Broadcast CCC-ENTAS0\n";
			 break;
		 }
		 case 4:
		 {
			 rich_textbox->Text = rich_textbox->Text +"Broadcast CCC-RSTDAA\n";
			 break;
		 }
		 case 5:
		 {
			 rich_textbox->Text = rich_textbox->Text +"Broadcast CCC-ENTDAA\n";
			 break;
		 }
		 case 6:
		 {
			 rich_textbox->Text = rich_textbox->Text +"Broadcast CCC-SETMWL\n";
			 break;
		 }
		 case 7:
		 {
			 rich_textbox->Text = rich_textbox->Text +"Broadcast CCC-SETMRL\n";
			 break;
		 }
	
    }
	sizeTransferred = 0;
	for(index = 0; index < 6; index++){
						outbuffer_dccc[index] = outBuffer_DCCC_COMMAND[index];
	}

	if (ccc == 1)  { outbuffer_dccc[2] =0x0c;outbuffer_dccc[4] = 0x80;outbuffer_dccc[7] = 0x01;sizeToTransfer = 9; outbuffer_dccc[8] = user_data; }
	else if (ccc == 2)  {outbuffer_dccc[2] =0x0c;outbuffer_dccc[4] = 0x81;outbuffer_dccc[7] = 0x01;sizeToTransfer = 9; outbuffer_dccc[8] = user_data; }
	else if (ccc == 3)  {outbuffer_dccc[2] =0x0a;outbuffer_dccc[4] = 0x82;outbuffer_dccc[7] = 0x00;sizeToTransfer = 8;}
	else if (ccc == 4)  {outbuffer_dccc[2] =0x0a;outbuffer_dccc[4] = 0x86;outbuffer_dccc[7] = 0x00;sizeToTransfer = 8;}
	else if (ccc == 5)  {outbuffer_dccc[2] =0x0c;outbuffer_dccc[4] = 0x88;outbuffer_dccc[7] = 0x01;sizeToTransfer = 9; outbuffer_dccc[8] = user_data; }
	else if (ccc == 6)  {outbuffer_dccc[2] =0x0e;outbuffer_dccc[4] = 0x89;outbuffer_dccc[7] = 0x02;sizeToTransfer = 10; outbuffer_dccc[8] = user_data; outbuffer_dccc[9] = user_data_1; }
	else if (ccc == 7)  {outbuffer_dccc[2] =0x10;outbuffer_dccc[4] = 0x8A;outbuffer_dccc[8] = user_data;outbuffer_dccc[9] = user_data_1;}
	else if (ccc == 8)  {outbuffer_dccc[2] =0x0a;outbuffer_dccc[4] = 0x8B;outbuffer_dccc[7] = 0x02;sizeToTransfer = 8;}
	else if (ccc == 9)  {outbuffer_dccc[2] =0x0a;outbuffer_dccc[4] = 0x8C;outbuffer_dccc[7] = 0x03;sizeToTransfer = 8;}
	else if (ccc == 10) {outbuffer_dccc[2] =0x0a;outbuffer_dccc[4] = 0x8D;outbuffer_dccc[7] = 0x06;sizeToTransfer = 8;}
	else if (ccc == 11) {outbuffer_dccc[2] =0x0a;outbuffer_dccc[4] = 0x8E;outbuffer_dccc[7] = 0x01;sizeToTransfer = 8;}
	else if (ccc == 12) {outbuffer_dccc[2] =0x0a;outbuffer_dccc[4] = 0x8F;outbuffer_dccc[7] = 0x01;sizeToTransfer = 8;}
	else if (ccc == 13) //dccc setdasa //00 10 0C 1B 87 01 0E 01 12
							{
								if(daa_done == 0)
								{
									rich_textbox->Text = rich_textbox->Text +"DCCC SETDASA Command\n";
									//fprintf(master_CmdResp_FileObj," DCCC SETDASA Command\n");
									outbuffer_dccc[2] =0x0c;outbuffer_dccc[4] = 0x87;outbuffer_dccc[7] = 0x01;sizeToTransfer = 9; outbuffer_dccc[8] = user_data; 
								}
								else 
								{
									rich_textbox->Text = rich_textbox->Text +"MIPI I3C specification does not allow SETDASA CCC \n to be issued on an initialized I3C bus\n Please try again after issuing Broadcast CCC RSTDAA\n";
									//printf("MIPI I3C specification does not allow SETDASA CCC to be issued on an initialized I3C bus\n");
									//printf("Please try again after issuing Broadcast CCC RSTDAA\n");
									//fprintf(master_CmdResp_FileObj," MIPI I3C specification does not allow SETDASA CCC to be issued on an initialized I3C bus\n Please try again after issuing Broadcast CCC RSTDAA\n");	
									
									//break;
								}
							}

	outbuffer_dccc[0]=0x00;
	status = SPI_ReadWrite(ftHandle, inBuffer, outbuffer_dccc, sizeToTransfer, &sizeTransferred,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	APP_CHECK_STATUS(status);		
return; 
 }
/*void spi_apis::read_command(void) {
	uint8 k,l;
	uint8 inBuffer[100];
	uint8 outBuffer_write[] = { 0x00,0x14,0x00,0xf6 };
	uint8 outBuffer_read[] = { 0x00,0x14,0x01,0x00 };
	//uint8 outBuffer_write1[] = { 0x00,0x00,0x00,0x99 };
	//uint8 outBuffer_read1[] = { 0x00,0x00,0x01,0x00 };
	uint32 sizeToTransfer, sizeTransferred;
	FT_STATUS status;

	sizeToTransfer = 32;
	sizeTransferred = 0;
	
#if o //BYTES
	status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_read1, sizeToTransfer, &sizeTransferred,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
#else // BITS
status = SPI_ReadWrite(ftHandle, inBuffer, outBuffer_read, sizeToTransfer, &sizeTransferred,
	SPI_TRANSFER_OPTIONS_SIZE_IN_BITS |
	SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
	SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
#endif
APP_CHECK_STATUS(status);

printf("status=0x%x sizeTransferred=%u\n", status, sizeTransferred);
for (l = 0; l<(sizeToTransfer / 8); l++)
	printf("0x%x\n", (unsigned)inBuffer[l]);
printf("\n");

}*/


 /*
 void spi_apis::poll_isr(System::Windows::Forms::RichTextBox ^ rich_textbox)
 {
	while(i3c_gui::poll_loop){
	
	 rich_textbox->Text =  rich_textbox->Text+ "Polling\n";
	 Sleep(1000);

	}

	return;
}
*/