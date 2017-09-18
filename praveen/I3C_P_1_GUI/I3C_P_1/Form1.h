#pragma once

#include <string> 
#include "FTDI_SPI_main.h"
#include "libMPSSE_spi.h"
namespace I3C_P_1 {

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

/// <summary>
/// Summary for Form1
/// </summary>
public ref class Form1 : public System::Windows::Forms::Form
{
public:
	Form1(void)
	{
		InitializeComponent();
		spi_apis::spi_init(richTextBox1);
		//
		//TODO: Add the constructor code here
		//
	}

protected:
	/// <summary>
	/// Clean up any resources being used.
	/// </summary>
	~Form1()
	{
		if (components)
		{
			delete components;
		}
	}
private: System::Windows::Forms::MenuStrip^  menuStrip1;
protected: 
private: System::Windows::Forms::ToolStripMenuItem^  fileToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  exitToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  helpToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  aboutToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  helpToolStripMenuItem1;
private: System::Windows::Forms::ToolStripMenuItem^  exitToolStripMenuItem1;
private: System::Windows::Forms::RichTextBox^  richTextBox1;
private: System::Windows::Forms::Label^  label1;
private: System::Windows::Forms::TabPage^  tabPage4;
private: System::Windows::Forms::TabPage^  tabPage3;

private: System::Windows::Forms::TabPage^  tabPage1;
private: System::Windows::Forms::TabControl^  tabControl1;

private: System::Windows::Forms::ComboBox^  comboBox3;


private: System::Windows::Forms::Label^  label2;
private: System::Windows::Forms::RichTextBox^  richTextBox2;
private: System::Windows::Forms::ComboBox^  comboBox1;
private: System::Windows::Forms::Button^  button2;
private: System::Windows::Forms::Button^  button1;
private: System::Windows::Forms::Button^  button3;
private: System::Windows::Forms::Label^  label11;




private: System::Windows::Forms::Label^  label9;
private: System::Windows::Forms::RadioButton^  radioButton2;
private: System::Windows::Forms::RadioButton^  radioButton1;
private: System::Windows::Forms::Label^  label8;
private: System::Windows::Forms::TextBox^  textBox2;
private: System::Windows::Forms::Label^  label7;
private: System::Windows::Forms::TabPage^  tabPage2;
private: System::Windows::Forms::Label^  label6;
private: System::Windows::Forms::Label^  label5;


private: System::Windows::Forms::Label^  label4;

private: System::Windows::Forms::TextBox^  textBox1;
private: System::Windows::Forms::Label^  label3;
private: System::Windows::Forms::ComboBox^  comboBox2;
private: System::Windows::Forms::Label^  label16;

private: System::Windows::Forms::Label^  label15;
private: System::Windows::Forms::Label^  label14;
private: System::Windows::Forms::Label^  label13;
private: System::Windows::Forms::DataGridView^  dataGridView1;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  Dyna_Addr;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  PID;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  BCR;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  DCR;
private: System::Windows::Forms::Label^  label12;
private: System::Windows::Forms::TabPage^  tabPage5;


private: System::Windows::Forms::GroupBox^  groupBox1;

private: System::Windows::Forms::Label^  label23;
private: System::Windows::Forms::Label^  label22;
private: System::Windows::Forms::TextBox^  textBox5;
private: System::Windows::Forms::Label^  label21;
private: System::Windows::Forms::TextBox^  textBox4;
private: System::Windows::Forms::TextBox^  textBox3;
private: System::Windows::Forms::Label^  label20;
private: System::Windows::Forms::Label^  label19;
private: System::Windows::Forms::RadioButton^  radioButton3;
private: System::Windows::Forms::RadioButton^  radioButton4;
private: System::Windows::Forms::Label^  label10;
private: System::Windows::Forms::RadioButton^  radioButton6;
private: System::Windows::Forms::RadioButton^  radioButton5;
private: System::Windows::Forms::Label^  label18;
private: System::Windows::Forms::TextBox^  textBox6;
private: System::Windows::Forms::Label^  label17;
private: System::Windows::Forms::TextBox^  textBox7;
private: System::Windows::Forms::Label^  label24;



private:
	/// <summary>
	/// Required designer variable.
	/// </summary>
	System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
	/// <summary>
	/// Required method for Designer support - do not modify
	/// the contents of this method with the code editor.
	/// </summary>
	void InitializeComponent(void)
	{
		this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
		this->fileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->exitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->helpToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->aboutToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->helpToolStripMenuItem1 = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->exitToolStripMenuItem1 = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->richTextBox1 = (gcnew System::Windows::Forms::RichTextBox());
		this->label1 = (gcnew System::Windows::Forms::Label());
		this->tabPage4 = (gcnew System::Windows::Forms::TabPage());
		this->label18 = (gcnew System::Windows::Forms::Label());
		this->textBox6 = (gcnew System::Windows::Forms::TextBox());
		this->label17 = (gcnew System::Windows::Forms::Label());
		this->radioButton6 = (gcnew System::Windows::Forms::RadioButton());
		this->radioButton5 = (gcnew System::Windows::Forms::RadioButton());
		this->label10 = (gcnew System::Windows::Forms::Label());
		this->tabPage3 = (gcnew System::Windows::Forms::TabPage());
		this->label11 = (gcnew System::Windows::Forms::Label());
		this->label9 = (gcnew System::Windows::Forms::Label());
		this->radioButton2 = (gcnew System::Windows::Forms::RadioButton());
		this->radioButton1 = (gcnew System::Windows::Forms::RadioButton());
		this->label8 = (gcnew System::Windows::Forms::Label());
		this->textBox2 = (gcnew System::Windows::Forms::TextBox());
		this->label7 = (gcnew System::Windows::Forms::Label());
		this->comboBox3 = (gcnew System::Windows::Forms::ComboBox());
		this->tabPage1 = (gcnew System::Windows::Forms::TabPage());
		this->textBox7 = (gcnew System::Windows::Forms::TextBox());
		this->label24 = (gcnew System::Windows::Forms::Label());
		this->label16 = (gcnew System::Windows::Forms::Label());
		this->comboBox1 = (gcnew System::Windows::Forms::ComboBox());
		this->button2 = (gcnew System::Windows::Forms::Button());
		this->label2 = (gcnew System::Windows::Forms::Label());
		this->button1 = (gcnew System::Windows::Forms::Button());
		this->richTextBox2 = (gcnew System::Windows::Forms::RichTextBox());
		this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
		this->tabPage2 = (gcnew System::Windows::Forms::TabPage());
		this->label15 = (gcnew System::Windows::Forms::Label());
		this->label14 = (gcnew System::Windows::Forms::Label());
		this->label13 = (gcnew System::Windows::Forms::Label());
		this->label6 = (gcnew System::Windows::Forms::Label());
		this->label5 = (gcnew System::Windows::Forms::Label());
		this->label4 = (gcnew System::Windows::Forms::Label());
		this->textBox1 = (gcnew System::Windows::Forms::TextBox());
		this->label3 = (gcnew System::Windows::Forms::Label());
		this->comboBox2 = (gcnew System::Windows::Forms::ComboBox());
		this->tabPage5 = (gcnew System::Windows::Forms::TabPage());
		this->label23 = (gcnew System::Windows::Forms::Label());
		this->label22 = (gcnew System::Windows::Forms::Label());
		this->textBox5 = (gcnew System::Windows::Forms::TextBox());
		this->label21 = (gcnew System::Windows::Forms::Label());
		this->textBox4 = (gcnew System::Windows::Forms::TextBox());
		this->textBox3 = (gcnew System::Windows::Forms::TextBox());
		this->label20 = (gcnew System::Windows::Forms::Label());
		this->label19 = (gcnew System::Windows::Forms::Label());
		this->radioButton3 = (gcnew System::Windows::Forms::RadioButton());
		this->radioButton4 = (gcnew System::Windows::Forms::RadioButton());
		this->button3 = (gcnew System::Windows::Forms::Button());
		this->dataGridView1 = (gcnew System::Windows::Forms::DataGridView());
		this->Dyna_Addr = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
		this->PID = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
		this->BCR = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
		this->DCR = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
		this->label12 = (gcnew System::Windows::Forms::Label());
		this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
		this->menuStrip1->SuspendLayout();
		this->tabPage4->SuspendLayout();
		this->tabPage3->SuspendLayout();
		this->tabPage1->SuspendLayout();
		this->tabControl1->SuspendLayout();
		this->tabPage2->SuspendLayout();
		this->tabPage5->SuspendLayout();
		(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView1))->BeginInit();
		this->groupBox1->SuspendLayout();
		this->SuspendLayout();
		// 
		// menuStrip1
		// 
		this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->fileToolStripMenuItem, 
			this->helpToolStripMenuItem, this->helpToolStripMenuItem1});
		this->menuStrip1->Location = System::Drawing::Point(0, 0);
		this->menuStrip1->Name = L"menuStrip1";
		this->menuStrip1->Padding = System::Windows::Forms::Padding(7, 1, 0, 1);
		this->menuStrip1->Size = System::Drawing::Size(520, 24);
		this->menuStrip1->TabIndex = 0;
		this->menuStrip1->Text = L"menuStrip1";
		// 
		// fileToolStripMenuItem
		// 
		this->fileToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->exitToolStripMenuItem});
		this->fileToolStripMenuItem->Name = L"fileToolStripMenuItem";
		this->fileToolStripMenuItem->Size = System::Drawing::Size(37, 22);
		this->fileToolStripMenuItem->Text = L"File";
		this->fileToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::fileToolStripMenuItem_Click);
		// 
		// exitToolStripMenuItem
		// 
		this->exitToolStripMenuItem->Name = L"exitToolStripMenuItem";
		this->exitToolStripMenuItem->Size = System::Drawing::Size(92, 22);
		this->exitToolStripMenuItem->Text = L"Exit";
		this->exitToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::exitToolStripMenuItem_Click);
		// 
		// helpToolStripMenuItem
		// 
		this->helpToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->aboutToolStripMenuItem});
		this->helpToolStripMenuItem->Name = L"helpToolStripMenuItem";
		this->helpToolStripMenuItem->Size = System::Drawing::Size(54, 22);
		this->helpToolStripMenuItem->Text = L"Device";
		// 
		// aboutToolStripMenuItem
		// 
		this->aboutToolStripMenuItem->Name = L"aboutToolStripMenuItem";
		this->aboutToolStripMenuItem->Size = System::Drawing::Size(102, 22);
		this->aboutToolStripMenuItem->Text = L"Reset";
		// 
		// helpToolStripMenuItem1
		// 
		this->helpToolStripMenuItem1->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->exitToolStripMenuItem1});
		this->helpToolStripMenuItem1->Name = L"helpToolStripMenuItem1";
		this->helpToolStripMenuItem1->Size = System::Drawing::Size(44, 22);
		this->helpToolStripMenuItem1->Text = L"Help";
		// 
		// exitToolStripMenuItem1
		// 
		this->exitToolStripMenuItem1->Name = L"exitToolStripMenuItem1";
		this->exitToolStripMenuItem1->Size = System::Drawing::Size(107, 22);
		this->exitToolStripMenuItem1->Text = L"About";
		// 
		// richTextBox1
		// 
		this->richTextBox1->Location = System::Drawing::Point(6, 452);
		this->richTextBox1->Margin = System::Windows::Forms::Padding(3, 5, 3, 5);
		this->richTextBox1->Name = L"richTextBox1";
		this->richTextBox1->ReadOnly = true;
		this->richTextBox1->Size = System::Drawing::Size(493, 107);
		this->richTextBox1->TabIndex = 2;
		this->richTextBox1->Text = L"";
		this->richTextBox1->TextChanged += gcnew System::EventHandler(this, &Form1::richTextBox1_TextChanged);
		// 
		// label1
		// 
		this->label1->AutoSize = true;
		this->label1->Location = System::Drawing::Point(11, 432);
		this->label1->Name = L"label1";
		this->label1->Size = System::Drawing::Size(101, 15);
		this->label1->TabIndex = 3;
		this->label1->Text = L"Transaction LOG :";
		// 
		// tabPage4
		// 
		this->tabPage4->Controls->Add(this->label18);
		this->tabPage4->Controls->Add(this->textBox6);
		this->tabPage4->Controls->Add(this->label17);
		this->tabPage4->Controls->Add(this->radioButton6);
		this->tabPage4->Controls->Add(this->radioButton5);
		this->tabPage4->Controls->Add(this->label10);
		this->tabPage4->Location = System::Drawing::Point(4, 24);
		this->tabPage4->Margin = System::Windows::Forms::Padding(3, 5, 3, 5);
		this->tabPage4->Name = L"tabPage4";
		this->tabPage4->Padding = System::Windows::Forms::Padding(3, 5, 3, 5);
		this->tabPage4->Size = System::Drawing::Size(435, 123);
		this->tabPage4->TabIndex = 3;
		this->tabPage4->Text = L"I2C_Legacy_R/W";
		this->tabPage4->UseVisualStyleBackColor = true;
		this->tabPage4->Click += gcnew System::EventHandler(this, &Form1::tabPage4_Click);
		// 
		// label18
		// 
		this->label18->AutoSize = true;
		this->label18->Location = System::Drawing::Point(183, 61);
		this->label18->Name = L"label18";
		this->label18->Size = System::Drawing::Size(129, 15);
		this->label18->TabIndex = 22;
		this->label18->Text = L"(For Hex:enter \"0x..\" )";
		// 
		// textBox6
		// 
		this->textBox6->Location = System::Drawing::Point(111, 58);
		this->textBox6->Margin = System::Windows::Forms::Padding(3, 5, 3, 5);
		this->textBox6->Name = L"textBox6";
		this->textBox6->Size = System::Drawing::Size(66, 23);
		this->textBox6->TabIndex = 21;
		this->textBox6->TextChanged += gcnew System::EventHandler(this, &Form1::textBox6_TextChanged);
		// 
		// label17
		// 
		this->label17->AutoSize = true;
		this->label17->Location = System::Drawing::Point(36, 61);
		this->label17->Name = L"label17";
		this->label17->Size = System::Drawing::Size(69, 15);
		this->label17->TabIndex = 20;
		this->label17->Text = L"Slave Addr:";
		this->label17->Click += gcnew System::EventHandler(this, &Form1::label17_Click);
		// 
		// radioButton6
		// 
		this->radioButton6->AutoSize = true;
		this->radioButton6->Location = System::Drawing::Point(186, 24);
		this->radioButton6->Name = L"radioButton6";
		this->radioButton6->Size = System::Drawing::Size(75, 19);
		this->radioButton6->TabIndex = 19;
		this->radioButton6->TabStop = true;
		this->radioButton6->Text = L"I2C_Read";
		this->radioButton6->UseVisualStyleBackColor = true;
		this->radioButton6->CheckedChanged += gcnew System::EventHandler(this, &Form1::radioButton6_CheckedChanged);
		// 
		// radioButton5
		// 
		this->radioButton5->AutoSize = true;
		this->radioButton5->Location = System::Drawing::Point(100, 22);
		this->radioButton5->Name = L"radioButton5";
		this->radioButton5->Size = System::Drawing::Size(80, 19);
		this->radioButton5->TabIndex = 18;
		this->radioButton5->TabStop = true;
		this->radioButton5->Text = L"I2C_Write";
		this->radioButton5->UseVisualStyleBackColor = true;
		// 
		// label10
		// 
		this->label10->AutoSize = true;
		this->label10->Location = System::Drawing::Point(36, 24);
		this->label10->Name = L"label10";
		this->label10->Size = System::Drawing::Size(47, 15);
		this->label10->TabIndex = 17;
		this->label10->Text = L"Select :";
		// 
		// tabPage3
		// 
		this->tabPage3->Controls->Add(this->label11);
		this->tabPage3->Controls->Add(this->label9);
		this->tabPage3->Controls->Add(this->radioButton2);
		this->tabPage3->Controls->Add(this->radioButton1);
		this->tabPage3->Controls->Add(this->label8);
		this->tabPage3->Controls->Add(this->textBox2);
		this->tabPage3->Controls->Add(this->label7);
		this->tabPage3->Controls->Add(this->comboBox3);
		this->tabPage3->Location = System::Drawing::Point(4, 24);
		this->tabPage3->Margin = System::Windows::Forms::Padding(3, 5, 3, 5);
		this->tabPage3->Name = L"tabPage3";
		this->tabPage3->Padding = System::Windows::Forms::Padding(3, 5, 3, 5);
		this->tabPage3->Size = System::Drawing::Size(435, 123);
		this->tabPage3->TabIndex = 2;
		this->tabPage3->Text = L"Pvt_R/W";
		this->tabPage3->UseVisualStyleBackColor = true;
		// 
		// label11
		// 
		this->label11->AutoSize = true;
		this->label11->Location = System::Drawing::Point(17, 29);
		this->label11->Name = L"label11";
		this->label11->Size = System::Drawing::Size(47, 15);
		this->label11->TabIndex = 16;
		this->label11->Text = L"Select :";
		// 
		// label9
		// 
		this->label9->AutoSize = true;
		this->label9->Location = System::Drawing::Point(8, 99);
		this->label9->Name = L"label9";
		this->label9->Size = System::Drawing::Size(70, 15);
		this->label9->TabIndex = 11;
		this->label9->Text = L"Command :";
		// 
		// radioButton2
		// 
		this->radioButton2->AutoSize = true;
		this->radioButton2->Location = System::Drawing::Point(182, 25);
		this->radioButton2->Margin = System::Windows::Forms::Padding(3, 5, 3, 5);
		this->radioButton2->Name = L"radioButton2";
		this->radioButton2->Size = System::Drawing::Size(97, 19);
		this->radioButton2->TabIndex = 10;
		this->radioButton2->TabStop = true;
		this->radioButton2->Text = L"Private_Read";
		this->radioButton2->UseVisualStyleBackColor = true;
		// 
		// radioButton1
		// 
		this->radioButton1->AutoSize = true;
		this->radioButton1->Location = System::Drawing::Point(75, 25);
		this->radioButton1->Margin = System::Windows::Forms::Padding(3, 5, 3, 5);
		this->radioButton1->Name = L"radioButton1";
		this->radioButton1->Size = System::Drawing::Size(100, 19);
		this->radioButton1->TabIndex = 9;
		this->radioButton1->TabStop = true;
		this->radioButton1->Text = L"Private_write";
		this->radioButton1->UseVisualStyleBackColor = true;
		// 
		// label8
		// 
		this->label8->AutoSize = true;
		this->label8->Location = System::Drawing::Point(156, 52);
		this->label8->Name = L"label8";
		this->label8->Size = System::Drawing::Size(129, 15);
		this->label8->TabIndex = 8;
		this->label8->Text = L"(For Hex:enter \"0x..\" )";
		// 
		// textBox2
		// 
		this->textBox2->Location = System::Drawing::Point(85, 52);
		this->textBox2->Margin = System::Windows::Forms::Padding(3, 5, 3, 5);
		this->textBox2->Name = L"textBox2";
		this->textBox2->Size = System::Drawing::Size(66, 23);
		this->textBox2->TabIndex = 3;
		// 
		// label7
		// 
		this->label7->AutoSize = true;
		this->label7->Location = System::Drawing::Point(6, 59);
		this->label7->Name = L"label7";
		this->label7->Size = System::Drawing::Size(69, 15);
		this->label7->TabIndex = 2;
		this->label7->Text = L"Slave Addr:";
		// 
		// comboBox3
		// 
		this->comboBox3->FormattingEnabled = true;
		this->comboBox3->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"select", L"Pvt_Read", L"Pvt_Write"});
		this->comboBox3->Location = System::Drawing::Point(85, 96);
		this->comboBox3->Margin = System::Windows::Forms::Padding(3, 5, 3, 5);
		this->comboBox3->Name = L"comboBox3";
		this->comboBox3->Size = System::Drawing::Size(89, 23);
		this->comboBox3->TabIndex = 0;
		this->comboBox3->Text = L"select";
		// 
		// tabPage1
		// 
		this->tabPage1->Controls->Add(this->textBox7);
		this->tabPage1->Controls->Add(this->label24);
		this->tabPage1->Controls->Add(this->label16);
		this->tabPage1->Controls->Add(this->comboBox1);
		this->tabPage1->Location = System::Drawing::Point(4, 24);
		this->tabPage1->Margin = System::Windows::Forms::Padding(3, 5, 3, 5);
		this->tabPage1->Name = L"tabPage1";
		this->tabPage1->Padding = System::Windows::Forms::Padding(3, 5, 3, 5);
		this->tabPage1->Size = System::Drawing::Size(435, 123);
		this->tabPage1->TabIndex = 0;
		this->tabPage1->Text = L"Broadcast_CCC";
		this->tabPage1->UseVisualStyleBackColor = true;
		this->tabPage1->Click += gcnew System::EventHandler(this, &Form1::tabPage1_Click);
		// 
		// textBox7
		// 
		this->textBox7->Location = System::Drawing::Point(87, 55);
		this->textBox7->Name = L"textBox7";
		this->textBox7->Size = System::Drawing::Size(246, 23);
		this->textBox7->TabIndex = 7;
		this->textBox7->TextChanged += gcnew System::EventHandler(this, &Form1::textBox7_TextChanged);
		// 
		// label24
		// 
		this->label24->AutoSize = true;
		this->label24->Location = System::Drawing::Point(14, 55);
		this->label24->Name = L"label24";
		this->label24->Size = System::Drawing::Size(73, 15);
		this->label24->TabIndex = 8;
		this->label24->Text = L"Instruction :";
		// 
		// label16
		// 
		this->label16->AutoSize = true;
		this->label16->Location = System::Drawing::Point(10, 31);
		this->label16->Name = L"label16";
		this->label16->Size = System::Drawing::Size(70, 15);
		this->label16->TabIndex = 12;
		this->label16->Text = L"Command :";
		// 
		// comboBox1
		// 
		this->comboBox1->FormattingEnabled = true;
		this->comboBox1->Items->AddRange(gcnew cli::array< System::Object^  >(8) {L"select", L"ENEC", L"DISEC", L"ENTAS0", L"RSTDAA", 
			L"ENTDAA", L"SETMWL", L"SETMRL"});
		this->comboBox1->Location = System::Drawing::Point(87, 29);
		this->comboBox1->Margin = System::Windows::Forms::Padding(3, 5, 3, 5);
		this->comboBox1->Name = L"comboBox1";
		this->comboBox1->Size = System::Drawing::Size(140, 23);
		this->comboBox1->TabIndex = 0;
		this->comboBox1->Text = L"select";
		this->comboBox1->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::comboBox1_SelectedIndexChanged);
		// 
		// button2
		// 
		this->button2->Location = System::Drawing::Point(366, 251);
		this->button2->Margin = System::Windows::Forms::Padding(3, 5, 3, 5);
		this->button2->Name = L"button2";
		this->button2->Size = System::Drawing::Size(87, 25);
		this->button2->TabIndex = 4;
		this->button2->Text = L"Send";
		this->button2->UseVisualStyleBackColor = true;
		this->button2->Click += gcnew System::EventHandler(this, &Form1::button2_Click);
		// 
		// label2
		// 
		this->label2->AutoSize = true;
		this->label2->Location = System::Drawing::Point(11, 181);
		this->label2->Name = L"label2";
		this->label2->Size = System::Drawing::Size(135, 15);
		this->label2->TabIndex = 2;
		this->label2->Text = L"Master Write Message:";
		this->label2->Click += gcnew System::EventHandler(this, &Form1::label2_Click);
		// 
		// button1
		// 
		this->button1->Location = System::Drawing::Point(10, 320);
		this->button1->Margin = System::Windows::Forms::Padding(3, 5, 3, 5);
		this->button1->Name = L"button1";
		this->button1->Size = System::Drawing::Size(87, 25);
		this->button1->TabIndex = 3;
		this->button1->Text = L"Clear";
		this->button1->UseVisualStyleBackColor = true;
		this->button1->Click += gcnew System::EventHandler(this, &Form1::button1_Click);
		// 
		// richTextBox2
		// 
		this->richTextBox2->Location = System::Drawing::Point(14, 201);
		this->richTextBox2->Margin = System::Windows::Forms::Padding(3, 5, 3, 5);
		this->richTextBox2->Name = L"richTextBox2";
		this->richTextBox2->Size = System::Drawing::Size(337, 118);
		this->richTextBox2->TabIndex = 1;
		this->richTextBox2->Text = L"";
		this->richTextBox2->TextChanged += gcnew System::EventHandler(this, &Form1::richTextBox2_TextChanged);
		this->richTextBox2->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::richTextBox2_KeyDown);
		this->richTextBox2->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &Form1::richTextBox2_KeyPress);
		// 
		// tabControl1
		// 
		this->tabControl1->Controls->Add(this->tabPage1);
		this->tabControl1->Controls->Add(this->tabPage2);
		this->tabControl1->Controls->Add(this->tabPage3);
		this->tabControl1->Controls->Add(this->tabPage4);
		this->tabControl1->Controls->Add(this->tabPage5);
		this->tabControl1->Location = System::Drawing::Point(10, 24);
		this->tabControl1->Margin = System::Windows::Forms::Padding(3, 5, 3, 5);
		this->tabControl1->Name = L"tabControl1";
		this->tabControl1->SelectedIndex = 0;
		this->tabControl1->Size = System::Drawing::Size(443, 151);
		this->tabControl1->TabIndex = 1;
		this->tabControl1->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::tabControl1_SelectedIndexChanged);
		// 
		// tabPage2
		// 
		this->tabPage2->Controls->Add(this->label15);
		this->tabPage2->Controls->Add(this->label14);
		this->tabPage2->Controls->Add(this->label13);
		this->tabPage2->Controls->Add(this->label6);
		this->tabPage2->Controls->Add(this->label5);
		this->tabPage2->Controls->Add(this->label4);
		this->tabPage2->Controls->Add(this->textBox1);
		this->tabPage2->Controls->Add(this->label3);
		this->tabPage2->Controls->Add(this->comboBox2);
		this->tabPage2->Location = System::Drawing::Point(4, 24);
		this->tabPage2->Margin = System::Windows::Forms::Padding(3, 5, 3, 5);
		this->tabPage2->Name = L"tabPage2";
		this->tabPage2->Padding = System::Windows::Forms::Padding(3, 5, 3, 5);
		this->tabPage2->Size = System::Drawing::Size(435, 123);
		this->tabPage2->TabIndex = 1;
		this->tabPage2->Text = L"Direct_CCC";
		this->tabPage2->UseVisualStyleBackColor = true;
		// 
		// label15
		// 
		this->label15->AutoSize = true;
		this->label15->Location = System::Drawing::Point(7, 66);
		this->label15->Name = L"label15";
		this->label15->Size = System::Drawing::Size(70, 15);
		this->label15->TabIndex = 12;
		this->label15->Text = L"Command :";
		// 
		// label14
		// 
		this->label14->AutoSize = true;
		this->label14->Location = System::Drawing::Point(152, 36);
		this->label14->Name = L"label14";
		this->label14->Size = System::Drawing::Size(129, 15);
		this->label14->TabIndex = 10;
		this->label14->Text = L"(For Hex:enter \"0x..\" )";
		// 
		// label13
		// 
		this->label13->AutoSize = true;
		this->label13->Location = System::Drawing::Point(3, 36);
		this->label13->Name = L"label13";
		this->label13->Size = System::Drawing::Size(69, 15);
		this->label13->TabIndex = 9;
		this->label13->Text = L"Slave Addr:";
		// 
		// label6
		// 
		this->label6->AutoSize = true;
		this->label6->Location = System::Drawing::Point(3, 66);
		this->label6->Name = L"label6";
		this->label6->Size = System::Drawing::Size(0, 15);
		this->label6->TabIndex = 8;
		// 
		// label5
		// 
		this->label5->AutoSize = true;
		this->label5->Location = System::Drawing::Point(160, 36);
		this->label5->Name = L"label5";
		this->label5->Size = System::Drawing::Size(0, 15);
		this->label5->TabIndex = 7;
		// 
		// label4
		// 
		this->label4->AutoSize = true;
		this->label4->Location = System::Drawing::Point(9, 107);
		this->label4->Name = L"label4";
		this->label4->Size = System::Drawing::Size(0, 15);
		this->label4->TabIndex = 4;
		// 
		// textBox1
		// 
		this->textBox1->Location = System::Drawing::Point(78, 31);
		this->textBox1->Margin = System::Windows::Forms::Padding(3, 5, 3, 5);
		this->textBox1->Name = L"textBox1";
		this->textBox1->Size = System::Drawing::Size(66, 23);
		this->textBox1->TabIndex = 2;
		this->textBox1->Text = L"0x09";
		// 
		// label3
		// 
		this->label3->AutoSize = true;
		this->label3->Location = System::Drawing::Point(3, 36);
		this->label3->Name = L"label3";
		this->label3->Size = System::Drawing::Size(0, 15);
		this->label3->TabIndex = 1;
		// 
		// comboBox2
		// 
		this->comboBox2->FormattingEnabled = true;
		this->comboBox2->Items->AddRange(gcnew cli::array< System::Object^  >(8) {L"select", L"ENEC", L"DISEC", L"ENTAS0", L"RSTDAA", 
			L"ENTDAA", L"SETMWL", L"SETMRL"});
		this->comboBox2->Location = System::Drawing::Point(78, 61);
		this->comboBox2->Margin = System::Windows::Forms::Padding(3, 5, 3, 5);
		this->comboBox2->Name = L"comboBox2";
		this->comboBox2->Size = System::Drawing::Size(140, 23);
		this->comboBox2->TabIndex = 0;
		this->comboBox2->Text = L"select";
		this->comboBox2->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::comboBox2_SelectedIndexChanged);
		// 
		// tabPage5
		// 
		this->tabPage5->Controls->Add(this->label23);
		this->tabPage5->Controls->Add(this->label22);
		this->tabPage5->Controls->Add(this->textBox5);
		this->tabPage5->Controls->Add(this->label21);
		this->tabPage5->Controls->Add(this->textBox4);
		this->tabPage5->Controls->Add(this->textBox3);
		this->tabPage5->Controls->Add(this->label20);
		this->tabPage5->Controls->Add(this->label19);
		this->tabPage5->Controls->Add(this->radioButton3);
		this->tabPage5->Controls->Add(this->radioButton4);
		this->tabPage5->Location = System::Drawing::Point(4, 24);
		this->tabPage5->Name = L"tabPage5";
		this->tabPage5->Padding = System::Windows::Forms::Padding(3);
		this->tabPage5->Size = System::Drawing::Size(435, 123);
		this->tabPage5->TabIndex = 4;
		this->tabPage5->Text = L"Generic_R/W";
		this->tabPage5->UseVisualStyleBackColor = true;
		// 
		// label23
		// 
		this->label23->AutoSize = true;
		this->label23->Location = System::Drawing::Point(97, 54);
		this->label23->Name = L"label23";
		this->label23->Size = System::Drawing::Size(51, 15);
		this->label23->TabIndex = 26;
		this->label23->Text = L"MSB: 0x";
		// 
		// label22
		// 
		this->label22->AutoSize = true;
		this->label22->Location = System::Drawing::Point(223, 55);
		this->label22->Name = L"label22";
		this->label22->Size = System::Drawing::Size(46, 15);
		this->label22->TabIndex = 25;
		this->label22->Text = L"LSB: 0x";
		// 
		// textBox5
		// 
		this->textBox5->Location = System::Drawing::Point(417, 51);
		this->textBox5->Margin = System::Windows::Forms::Padding(3, 5, 3, 5);
		this->textBox5->Name = L"textBox5";
		this->textBox5->Size = System::Drawing::Size(41, 23);
		this->textBox5->TabIndex = 24;
		this->textBox5->Text = L"1";
		this->textBox5->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::textBox5_KeyDown);
		this->textBox5->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &Form1::textBox5_KeyPress);
		// 
		// label21
		// 
		this->label21->AutoSize = true;
		this->label21->Location = System::Drawing::Point(339, 54);
		this->label21->Name = L"label21";
		this->label21->Size = System::Drawing::Size(82, 15);
		this->label21->TabIndex = 23;
		this->label21->Text = L"Read_Length:";
		// 
		// textBox4
		// 
		this->textBox4->Location = System::Drawing::Point(151, 51);
		this->textBox4->Margin = System::Windows::Forms::Padding(3, 5, 3, 5);
		this->textBox4->MaxLength = 2;
		this->textBox4->Name = L"textBox4";
		this->textBox4->Size = System::Drawing::Size(66, 23);
		this->textBox4->TabIndex = 22;
		this->textBox4->Text = L"00";
		this->textBox4->TextChanged += gcnew System::EventHandler(this, &Form1::textBox4_TextChanged);
		this->textBox4->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::textBox4_KeyDown);
		this->textBox4->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &Form1::textBox4_KeyPress);
		// 
		// textBox3
		// 
		this->textBox3->Location = System::Drawing::Point(273, 51);
		this->textBox3->Margin = System::Windows::Forms::Padding(3, 5, 3, 5);
		this->textBox3->MaxLength = 2;
		this->textBox3->Name = L"textBox3";
		this->textBox3->Size = System::Drawing::Size(60, 23);
		this->textBox3->TabIndex = 21;
		this->textBox3->Text = L"00";
		this->textBox3->TextChanged += gcnew System::EventHandler(this, &Form1::textBox3_TextChanged);
		this->textBox3->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::textBox3_KeyDown);
		this->textBox3->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &Form1::textBox3_KeyPress);
		// 
		// label20
		// 
		this->label20->AutoSize = true;
		this->label20->Location = System::Drawing::Point(7, 55);
		this->label20->Name = L"label20";
		this->label20->Size = System::Drawing::Size(91, 15);
		this->label20->TabIndex = 20;
		this->label20->Text = L"Slave reg_addr:";
		this->label20->Click += gcnew System::EventHandler(this, &Form1::label20_Click);
		// 
		// label19
		// 
		this->label19->AutoSize = true;
		this->label19->Location = System::Drawing::Point(22, 31);
		this->label19->Name = L"label19";
		this->label19->Size = System::Drawing::Size(47, 15);
		this->label19->TabIndex = 19;
		this->label19->Text = L"Select :";
		// 
		// radioButton3
		// 
		this->radioButton3->AutoSize = true;
		this->radioButton3->Location = System::Drawing::Point(138, 31);
		this->radioButton3->Margin = System::Windows::Forms::Padding(3, 5, 3, 5);
		this->radioButton3->Name = L"radioButton3";
		this->radioButton3->Size = System::Drawing::Size(52, 19);
		this->radioButton3->TabIndex = 18;
		this->radioButton3->TabStop = true;
		this->radioButton3->Text = L"Read";
		this->radioButton3->UseVisualStyleBackColor = true;
		this->radioButton3->CheckedChanged += gcnew System::EventHandler(this, &Form1::radioButton3_CheckedChanged);
		// 
		// radioButton4
		// 
		this->radioButton4->AutoSize = true;
		this->radioButton4->Location = System::Drawing::Point(75, 31);
		this->radioButton4->Margin = System::Windows::Forms::Padding(3, 5, 3, 5);
		this->radioButton4->Name = L"radioButton4";
		this->radioButton4->Size = System::Drawing::Size(57, 19);
		this->radioButton4->TabIndex = 17;
		this->radioButton4->TabStop = true;
		this->radioButton4->Text = L"Write";
		this->radioButton4->UseVisualStyleBackColor = true;
		// 
		// button3
		// 
		this->button3->Location = System::Drawing::Point(10, 569);
		this->button3->Margin = System::Windows::Forms::Padding(3, 5, 3, 5);
		this->button3->Name = L"button3";
		this->button3->Size = System::Drawing::Size(87, 25);
		this->button3->TabIndex = 4;
		this->button3->Text = L"Clear";
		this->button3->UseVisualStyleBackColor = true;
		this->button3->Click += gcnew System::EventHandler(this, &Form1::button3_Click);
		// 
		// dataGridView1
		// 
		this->dataGridView1->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
		this->dataGridView1->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(4) {this->Dyna_Addr, 
			this->PID, this->BCR, this->DCR});
		this->dataGridView1->Location = System::Drawing::Point(10, 370);
		this->dataGridView1->Margin = System::Windows::Forms::Padding(3, 5, 3, 5);
		this->dataGridView1->Name = L"dataGridView1";
		this->dataGridView1->Size = System::Drawing::Size(443, 44);
		this->dataGridView1->TabIndex = 5;
		this->dataGridView1->CellContentClick += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &Form1::dataGridView1_CellContentClick);
		// 
		// Dyna_Addr
		// 
		this->Dyna_Addr->HeaderText = L"Dyna_Addr";
		this->Dyna_Addr->Name = L"Dyna_Addr";
		// 
		// PID
		// 
		this->PID->HeaderText = L"PID";
		this->PID->Name = L"PID";
		// 
		// BCR
		// 
		this->BCR->HeaderText = L"BCR";
		this->BCR->Name = L"BCR";
		// 
		// DCR
		// 
		this->DCR->HeaderText = L"DCR";
		this->DCR->Name = L"DCR";
		// 
		// label12
		// 
		this->label12->AutoSize = true;
		this->label12->Location = System::Drawing::Point(11, 350);
		this->label12->Name = L"label12";
		this->label12->Size = System::Drawing::Size(160, 15);
		this->label12->TabIndex = 6;
		this->label12->Text = L"Device Characteristics Table:";
		// 
		// groupBox1
		// 
		this->groupBox1->Controls->Add(this->label1);
		this->groupBox1->Controls->Add(this->label2);
		this->groupBox1->Controls->Add(this->button2);
		this->groupBox1->Controls->Add(this->tabControl1);
		this->groupBox1->Controls->Add(this->label12);
		this->groupBox1->Controls->Add(this->button1);
		this->groupBox1->Controls->Add(this->richTextBox1);
		this->groupBox1->Controls->Add(this->richTextBox2);
		this->groupBox1->Controls->Add(this->dataGridView1);
		this->groupBox1->Controls->Add(this->button3);
		this->groupBox1->Location = System::Drawing::Point(12, 49);
		this->groupBox1->Name = L"groupBox1";
		this->groupBox1->Size = System::Drawing::Size(490, 602);
		this->groupBox1->TabIndex = 7;
		this->groupBox1->TabStop = false;
		this->groupBox1->Text = L"I3C_MASTER";
		this->groupBox1->Enter += gcnew System::EventHandler(this, &Form1::groupBox1_Enter);
		// 
		// Form1
		// 
		this->AutoScaleDimensions = System::Drawing::SizeF(7, 15);
		this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
		this->ClientSize = System::Drawing::Size(520, 671);
		this->Controls->Add(this->groupBox1);
		this->Controls->Add(this->menuStrip1);
		this->Font = (gcnew System::Drawing::Font(L"Calibri", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
			static_cast<System::Byte>(0)));
		this->MainMenuStrip = this->menuStrip1;
		this->Margin = System::Windows::Forms::Padding(3, 5, 3, 5);
		this->Name = L"Form1";
		this->Text = L"I3C-DEMO";
		this->menuStrip1->ResumeLayout(false);
		this->menuStrip1->PerformLayout();
		this->tabPage4->ResumeLayout(false);
		this->tabPage4->PerformLayout();
		this->tabPage3->ResumeLayout(false);
		this->tabPage3->PerformLayout();
		this->tabPage1->ResumeLayout(false);
		this->tabPage1->PerformLayout();
		this->tabControl1->ResumeLayout(false);
		this->tabPage2->ResumeLayout(false);
		this->tabPage2->PerformLayout();
		this->tabPage5->ResumeLayout(false);
		this->tabPage5->PerformLayout();
		(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView1))->EndInit();
		this->groupBox1->ResumeLayout(false);
		this->groupBox1->PerformLayout();
		this->ResumeLayout(false);
		this->PerformLayout();

	}
#pragma endregion
private: System::Void richTextBox1_TextChanged(System::Object^  sender, System::EventArgs^  e) {
			
			}
private: System::Void button3_Click(System::Object^  sender, System::EventArgs^  e) {
			 richTextBox1->Text="\0";		
		 }
private: System::Void exitToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {

			Close();
		}
private: System::Void groupBox1_Enter(System::Object^  sender, System::EventArgs^  e) {
		}

private:
bool nonNumberEntered;
static int num_chars=0;
static int bccc_cmd=0;
static int dccc_cmd=0;
String ^str;
static String ^active_tab;
private: System::Void textBox5_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) 
{
	//Read _length og generic read

// Initialize the flag to false.
    nonNumberEntered = false;
// Determine whether the keystroke is a number from the top of the keyboard.
    if ( e->KeyCode < Keys::D0 || e->KeyCode > Keys::D9 )
    {
        // Determine whether the keystroke is a number from the keypad.
        if ( e->KeyCode < Keys::NumPad0 || e->KeyCode > Keys::NumPad9 )
        {
        // Determine whether the keystroke is a backspace.
        if ( e->KeyCode != Keys::Back )
        {
            // A non-numerical keystroke was pressed.
            // Set the flag to true and evaluate in KeyPress event.
            nonNumberEntered = true;
        }
        }
    }
    //If shift key was pressed, it's not a number.
    if (Control::ModifierKeys == Keys::Shift) {
        nonNumberEntered = true;
    }

}
private: System::Void textBox5_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) 
{
// Boolean flag used to determine when a character other than a number is entered.
if ( nonNumberEntered == true )
	{         // Stop the character from being entered into the control since it is non-numerical.
	e->Handled = true;
	}
}

private: System::Void textBox4_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e)
{
	
	nonNumberEntered = false;
// Determine whether the keystroke is a number from the top of the keyboard.
    if ( e->KeyCode < Keys::D0 || e->KeyCode > Keys::D9 )
    {
        // Determine whether the keystroke is a number from the keypad.
        if ( e->KeyCode < Keys::NumPad0 || e->KeyCode > Keys::NumPad9 )
        {
        // Determine whether the keystroke is a backspace.
        if ( e->KeyCode != Keys::Back)
        {
            // A non-numerical keystroke was pressed.
            // Set the flag to true and evaluate in KeyPress event.
            nonNumberEntered = true;
        }
		if( (e->KeyCode >= Keys::A) && (e->KeyCode <= Keys::F))
		{
			nonNumberEntered = false;
		}
			
        }
    }

}

private: System::Void textBox4_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) 
{
		// Boolean flag used to determine when a character other than a number is entered.
	if ( nonNumberEntered == true )
	{         // Stop the character from being entered into the control since it is non-numerical.
		e->Handled = true;
	}
}
private: System::Void textBox3_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) 
		{
			 
	nonNumberEntered = false;
// Determine whether the keystroke is a number from the top of the keyboard.
    if ( e->KeyCode < Keys::D0 || e->KeyCode > Keys::D9 )
    {
        // Determine whether the keystroke is a number from the keypad.
        if ( e->KeyCode < Keys::NumPad0 || e->KeyCode > Keys::NumPad9 )
        {
        // Determine whether the keystroke is a backspace.
        if ( e->KeyCode == Keys::Back)
        {
            // A non-numerical keystroke was pressed.
            // Set the flag to true and evaluate in KeyPress event.
            nonNumberEntered = false;
        }
		if( (e->KeyCode >= Keys::A) && (e->KeyCode <= Keys::F))
		{
			nonNumberEntered = false;
		}
			
        }
    }

		}
private: System::Void textBox3_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) 
		{
		if ( nonNumberEntered == true )
		{         // Stop the character from being entered into the control since it is non-numerical.
			e->Handled = true;
		}
		}

private: System::Void richTextBox2_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) 
{
		nonNumberEntered = false;
// Determine whether the keystroke is a number from the top of the keyboard.
	if ( e->KeyCode < Keys::D0 || e->KeyCode > Keys::D9 )
	{
	    // Determine whether the keystroke is a number from the keypad.
	    if ( e->KeyCode < Keys::NumPad0 || e->KeyCode > Keys::NumPad9 )
	    {
			// Determine whether the keystroke is a backspace.
			if ( e->KeyCode == Keys::Back)
			{
				// A non-numerical keystroke was pressed.
				// Set the flag to true and evaluate in KeyPress event.
				nonNumberEntered = false;
			}
			if( (e->KeyCode >= Keys::A) && (e->KeyCode <= Keys::F))
			{
				nonNumberEntered = false;
			}
				
	    }
	}
}
private: System::Void richTextBox2_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) 
{
	if(nonNumberEntered == true )
		{         // Stop the character from being entered into the control since it is non-numerical.
			e->Handled = true;
		}
}
		 /*static int cursor_location = 0;
private: System::Void richTextBox2_TextChanged(System::Object^  sender, System::EventArgs^  e) 
{
	int i =0;
	
	String ^ tmp_str = richTextBox2->Text;
	String ^ out_tmp_str ="";
	for(i=0;i<tmp_str->Length;i++){
		if((tmp_str[i] !=' ')){
			out_tmp_str = out_tmp_str + tmp_str[i];
		}
	}
	tmp_str =out_tmp_str;
	out_tmp_str = "";
	if(tmp_str->Length > 1){
		cursor_location = richTextBox2->SelectionStart;
		if(!tmp_str->Length%2 == 0){
			for(i=0;i<tmp_str->Length;i++){
				if(i%2 == 0){
					out_tmp_str = out_tmp_str + tmp_str[i];
				} else {
					out_tmp_str = out_tmp_str + tmp_str[i] + " ";
				}
			}

			if(cursor_location < tmp_str->Length) {
				richTextBox2->SelectionStart = cursor_location+1;
			} else {
				richTextBox2->SelectionStart = cursor_location+2;
			}
			richTextBox2->Text=out_tmp_str;
		}
	}
}*/
private: System::Void richTextBox2_TextChanged(System::Object^  sender, System::EventArgs^  e) 
{
	int i =0;
	String ^ tmp_str = richTextBox2->Text;
	String ^ out_tmp_str ="";

	for(i=0;i<tmp_str->Length;i++){
		if((tmp_str[i] !=' ')){
			out_tmp_str = out_tmp_str + tmp_str[i];
		}
	}
	tmp_str =out_tmp_str;
	out_tmp_str = "";
	if(tmp_str->Length > 1){
		if(!tmp_str->Length%2 == 0){
			for(i=0;i<tmp_str->Length;i++){
				if(i%2 == 0){
					out_tmp_str = out_tmp_str + tmp_str[i];
				}
				else
				{
					out_tmp_str = out_tmp_str + tmp_str[i] + " ";
				}
			}
			richTextBox2->Text=out_tmp_str;
			richTextBox2->SelectionStart = richTextBox2->Text->Length;
			richTextBox2->SelectionLength=0;
		}
	}
}
private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
	richTextBox2->Text="\0";		
}

		 //send button
private: System::Void button2_Click(System::Object^  sender, System::EventArgs^  e) 
{
	//Transaction button
if(comboBox1->Text=="select")
{
	textBox7->Text ="Select any Broadcast CCC command";
}
else if(active_tab=="generic_rw")
	{
		//active_tab="NULL";
		//need to take the data in textbox 3(LSB) and 4(MSB) of I3C slave address
		String ^data=richTextBox2->Text;
	//	strcat(textBox4->Text,data);
		//data=textBox4->Text;
		//data=richTextBox2->Text;
		if(richTextBox2->Text->Length>2)
		{
			if(richTextBox1->Text!="")
			{
				richTextBox1->Text = "length of the string:"+richTextBox2->Text->Length+"\n";
				spi_apis::send(data,richTextBox1);
			}
		}
		else
		{
			richTextBox1->Text = "length of the string:"+richTextBox2->Text->Length+"\n";
			richTextBox1->Text = richTextBox1->Text+"Please provide at least byte\n";
		}
		
	}
	else if(active_tab=="BCCC")
	{
		//here we need to differntiate all bccc commands.
		String ^data_bccc=richTextBox2->Text;		
		//spi_apis::send_bccc(data_bccc,bccc_cmd,richTextBox1);
		spi_apis::send_ccc(richTextBox1,bccc_cmd,data_bccc);
		
	}
	else if(active_tab=="DCCC")
	{
		String ^data_dccc=richTextBox2->Text;
		spi_apis::send_dccc(richTextBox1,dccc_cmd,data_dccc);
	}
	else if(active_tab=="Private_rw")
	{
		
	}
	else if(active_tab=="I2C_rw")
	{
		
	}
	else
	{
		active_tab="NULL";
	}

/*
	else
	{
	  data = Convert::ToString(richTextBox2->Text);
	  data = data->Trim();
	  data = data->Replace(" ","");
	  spi_apis::send_ccc(richTextBox1,1,data);
	}
*/
	//active_tab="";
}
private: System::Void radioButton3_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			 richTextBox2->Enabled=false;
		 }
 private: System::Void fileToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
		  }
private: System::Void tabControl1_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {

			if(tabControl1->SelectedTab == tabControl1->TabPages["tabPage5"])
			{
				active_tab="generic_rw";
			
			}
			else if(tabControl1->SelectedTab == tabControl1->TabPages["tabPage1"])
			{
				active_tab="BCCC";
				
			}
			else if(tabControl1->SelectedTab == tabControl1->TabPages["tabPage2"])
			{
				active_tab="DCCC";
			
			}
			else if(tabControl1->SelectedTab == tabControl1->TabPages["tabPage3"])
			{
				active_tab="Private_rw";
			
			}
			else if(tabControl1->SelectedTab == tabControl1->TabPages["tabPage4"])
			{
				active_tab="I2C_rw";
			}
			else{
				active_tab="default";
			}
			
}
private: System::Void label20_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void textBox4_TextChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void textBox3_TextChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void dataGridView1_CellContentClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) {
		 }
private: System::Void label2_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void comboBox1_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) 
		 {
			if(comboBox1->Text=="ENEC")
			{
					bccc_cmd=1;
					textBox7->Text ="Provide one byte data to be written for 'ENEC' \n";
			}
			else if(comboBox1->Text=="DISEC")
			{
					textBox7->Text ="Provide one byte data to be written for 'DISEC' \n";
					bccc_cmd=2;
			}
			else if(comboBox1->Text=="ENTAS0")
			{
					textBox7->Text ="Click send to trasmit ENTAS0 command\n";
					bccc_cmd=3;
			}
			else if(comboBox1->Text=="RSTDAA")
			{
					textBox7->Text ="Click send to trasmit Broadcast CCC-RSTDAA command\n";
					bccc_cmd=4;
			}
			else if(comboBox1->Text=="ENTDAA")
			{
					bccc_cmd=5;
					textBox7->Text ="Provide dynamic address of the device which wins \n the first Dynamic address Aribitration\n";
			}
			else if(comboBox1->Text=="SETMWL")
			{
					textBox7->Text ="Provide one byte data to be written for 'SETMWL' \n";
					bccc_cmd=6;
			}
			else if(comboBox1->Text=="SETMRL")
			{
					textBox7->Text ="Provide one byte data to be written for 'SETMRL' \n";
					bccc_cmd=7;
			}
			else
			{
				bccc_cmd=0;
				textBox7->Text=" ";
			}
		 }
private: System::Void tabPage4_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void radioButton6_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void label24_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void tabPage1_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void textBox7_TextChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void label17_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void textBox6_TextChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void comboBox2_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) 
		 {
			if(comboBox2->Text=="ENEC")
			{
					dccc_cmd=1;
					textBox7->Text ="Provide one byte data to be written for 'ENEC' \n";
			}
			else if(comboBox2->Text=="DISEC")
			{
					textBox7->Text ="Provide one byte data to be written for 'DISEC' \n";
					dccc_cmd=2;
			}
			else if(comboBox2->Text=="ENTAS0")
			{
					textBox7->Text ="Provide one byte data to be written for 'DISEC' \n";
					dccc_cmd=3;
			}else if(comboBox2->Text=="RSTDAA")
			{
					textBox7->Text ="Provide one byte data to be written for 'DISEC' \n";
					dccc_cmd=4;
			}
			else if(comboBox2->Text=="SETNEWDA")
			{
					dccc_cmd=5;
					textBox7->Text ="Provide new dynamic address for slave\n";
			}
			else if(comboBox2->Text=="SETMWL")
			{
					textBox7->Text ="Provide one byte data to be written for 'SETMWL' \n";
					dccc_cmd=6;
			}
			else if(comboBox2->Text=="SETMRL")
			{
					textBox7->Text ="Provide one byte data to be written for 'SETMRL' \n";
					dccc_cmd=7;
			}
			else if(comboBox2->Text=="GETMWL")
			{
					textBox7->Text ="Provide one byte data to be written for 'GETMWL' \n";
					dccc_cmd=8;
			}
			else if(comboBox2->Text=="GETMRL")
			{
					textBox7->Text ="Provide one byte data to be written for 'GETMRL' \n";
					dccc_cmd=9;
			}
			else if(comboBox2->Text=="GETPID")
			{
					textBox7->Text ="Provide one byte data to be written for 'GETPID' \n";
					dccc_cmd=10;
			}
			else if(comboBox2->Text=="GETBCR")
			{
					textBox7->Text ="Provide one byte data to be written for 'GETBCR' \n";
					dccc_cmd=11;
			}
			else if(comboBox2->Text=="GETDCR")
			{
					textBox7->Text ="Provide one byte data to be written for 'GETDCR' \n";
					dccc_cmd=12;
			}
			else if(comboBox2->Text=="SETDASA")
			{
					textBox7->Text ="Provide one byte data to be written for 'SETDASA' \n";
					dccc_cmd=13;
			}
			else
			{
				dccc_cmd=0;
				textBox7->Text=" ";
			}
		 }
};//end of public ref class Form1
}//end of namespace