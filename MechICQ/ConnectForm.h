#pragma once

#include "Protocol.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::Net::Sockets;
using namespace System::Net;
using namespace System::IO;
using namespace System::Text;
using namespace System::Threading;

namespace MechICQ
{
	/// <summary>
	/// Server connection form.
	/// </summary>
	public ref class ConnectForm : public System::Windows::Forms::Form
	{
	private:
		Protocol ^protocol;

	public:
		ConnectForm(void)
		{
			InitializeComponent();
			protocol = gcnew Protocol();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~ConnectForm()
		{
			if (components)
			{
				delete components;
			}
		}

	private:
		void Form_Load(System::Object ^sender, System::EventArgs ^e);
		void ConnectButton_Click(System::Object ^sender, System::EventArgs ^e);
		void Timer_Tick(System::Object^  sender, System::EventArgs^  e);

	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::TextBox^  textBox2;
	private: System::Windows::Forms::Label^  logStatusLabel;
	private: System::Windows::Forms::ProgressBar^  progressBar1;
	private: System::Windows::Forms::Timer^  timer1;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::IContainer^  components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->textBox2 = (gcnew System::Windows::Forms::TextBox());
			this->logStatusLabel = (gcnew System::Windows::Forms::Label());
			this->progressBar1 = (gcnew System::Windows::Forms::ProgressBar());
			this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
			this->SuspendLayout();
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(12, 64);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(162, 23);
			this->button1->TabIndex = 0;
			this->button1->Text = L"Подключиться";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &ConnectForm::ConnectButton_Click);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(12, 15);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(29, 13);
			this->label1->TabIndex = 1;
			this->label1->Text = L"UIN:";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(12, 41);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(56, 13);
			this->label2->TabIndex = 2;
			this->label2->Text = L"Password:";
			// 
			// textBox1
			// 
			this->textBox1->Location = System::Drawing::Point(74, 12);
			this->textBox1->Name = L"textBox1";
			this->textBox1->Size = System::Drawing::Size(100, 20);
			this->textBox1->TabIndex = 3;
			this->textBox1->Text = L"100000000";
			// 
			// textBox2
			// 
			this->textBox2->Location = System::Drawing::Point(74, 38);
			this->textBox2->Name = L"textBox2";
			this->textBox2->Size = System::Drawing::Size(100, 20);
			this->textBox2->TabIndex = 4;
			this->textBox2->Text = L"password";
			// 
			// logStatusLabel
			// 
			this->logStatusLabel->Location = System::Drawing::Point(12, 119);
			this->logStatusLabel->Name = L"logStatusLabel";
			this->logStatusLabel->Size = System::Drawing::Size(162, 17);
			this->logStatusLabel->TabIndex = 5;
			this->logStatusLabel->Text = L"Отключено";
			this->logStatusLabel->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// progressBar1
			// 
			this->progressBar1->Location = System::Drawing::Point(12, 93);
			this->progressBar1->Name = L"progressBar1";
			this->progressBar1->Size = System::Drawing::Size(162, 23);
			this->progressBar1->TabIndex = 6;
			// 
			// timer1
			// 
			this->timer1->Tick += gcnew System::EventHandler(this, &ConnectForm::Timer_Tick);
			// 
			// ConnectForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(188, 138);
			this->Controls->Add(this->progressBar1);
			this->Controls->Add(this->logStatusLabel);
			this->Controls->Add(this->textBox2);
			this->Controls->Add(this->textBox1);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->button1);
			this->Name = L"ConnectForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"Mechanicum";
			this->Load += gcnew System::EventHandler(this, &ConnectForm::Form_Load);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	};
}
