#pragma once

#ifndef _LATTICE_SEMI_I2C_H_FILE_
#define _LATTICE_SEMI_I2C_H_FILE_

//#pragma once

#include "stdafx.h"

#include <windows.h>
//============================================================================
//  Use of FTDI D2XX library:
//----------------------------------------------------------------------------
//  Include the following 2 lines in your header-file

#include "ftd2xx.h"
//============================================================================

#include <stdlib.h>
#include<stdio.h>
#include<cstring>

static FT_HANDLE ftHandle;

using namespace System::Windows::Forms;

//const BYTE AA_ECHO_CMD_1 = '\xAA';

//int spi_addr1, reg_addr1;

class background_variable
{
public:
	
	//static int poll_loop;
	volatile int location;
	int i2c_address;
	bool external_control[6];
	bool trim_polarity[6];
	int dac_delay_value[6];
	volatile int str_dac_val[6]; // dac value to add/sub
	volatile float trg_val[6];	//target value
	int adc_mux_val[6];
	int trim_dac_reg[6];
};
//public ref 
class spi_apis//:public Form1
{
public:
	//unsigned char * spi_apis::send(unsigned char* outBuffer_write, unsigned long sizeToTransfer)
	//static unsigned char * send(unsigned char*,unsigned long);
	static void poll_isr(System::Windows::Forms::RichTextBox ^ rich_textbox);
	static unsigned char* send(System::String ^,System::Windows::Forms::RichTextBox ^ rich_textbox);
	//static unsigned char* send_bccc(System::String ^,System::Windows::Forms::RichTextBox ^ rich_textbox);
	static unsigned char* send_bccc(System::String ^,int,System::Windows::Forms::RichTextBox ^ rich_textbox);
	static void send_ccc( System::Windows::Forms::RichTextBox ^ rich_textbox,int ccc,System::String ^);
	static void send_dccc( System::Windows::Forms::RichTextBox ^ rich_textbox,int ccc,System::String ^);
	//static void spi_init(System::Windows::Forms::RichTextBox ^ rich_textbox);
	static void spi_init(System::Windows::Forms::RichTextBox ^ rich_textbox);
	//void send_message(char*);
	static int* daa(void);
	static unsigned char* write_command(void);
	//static  unsigned char * read_command(FT_HANDLE, unsigned char*, unsigned long);
	static void toDoWork(background_variable *obj);
	static void HighSpeedSetI2CStart(void);
	static void HighSpeedSetI2CStop(void);
	static int i2c_read_register(int i2caddrs, int regadr, int* value, int num_of_rxbytes);//,System::Windows::Forms::TextBox^ *test);
	static int i2c_write_register(int i2caddrs, int regadr, int* value, int num_of_txbytes);
	static int vmon_read(int i2c_addrs, int adc_mux_value);
	static int SMBUS_Alert_process(int i2c_addrs);
	//static char* convert_unsigned_byte_to_string(unsigned char data);
};
//public ref 
class ftdi_init_api//:public Form1
{
public:

	static int connection_init(void);
	static int close_ftdi(void);
	static int detect_cable(void);
	static int check_for_I2c_addr(int i2caddrs);
	static int check_cable_I2caddrs(int i2caddrs);
	static int write_to_gpio(int val);
	static int read_from_gpio(void);
};
#endif