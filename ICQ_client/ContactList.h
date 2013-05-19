#pragma once

#include "Protocol_low_lvl.h"
#include "GlobalOptions.h"
#include "IniFile.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace ICQ_client {

	/// <summary>
	/// Summary for ContactList
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class ContactList : public System::Windows::Forms::Form
	{
	public:
		ContactList(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~ContactList()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::ListBox^  listBox1;
	protected: 
	private: System::Windows::Forms::ComboBox^  comboBox1;

	protected: 








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
			this->listBox1 = (gcnew System::Windows::Forms::ListBox());
			this->comboBox1 = (gcnew System::Windows::Forms::ComboBox());
			this->SuspendLayout();
			// 
			// listBox1
			// 
			this->listBox1->FormattingEnabled = true;
			this->listBox1->Location = System::Drawing::Point(12, 12);
			this->listBox1->Name = L"listBox1";
			this->listBox1->Size = System::Drawing::Size(145, 108);
			this->listBox1->TabIndex = 0;
			// 
			// comboBox1
			// 
			this->comboBox1->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboBox1->FormattingEnabled = true;
			this->comboBox1->Items->AddRange(gcnew cli::array< System::Object^  >(2) {L"В сети", L"Отключен"});
			this->comboBox1->Location = System::Drawing::Point(12, 126);
			this->comboBox1->Name = L"comboBox1";
			this->comboBox1->Size = System::Drawing::Size(145, 21);
			this->comboBox1->TabIndex = 1;
			// 
			// ContactList
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(169, 170);
			this->Controls->Add(this->comboBox1);
			this->Controls->Add(this->listBox1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::SizableToolWindow;
			this->Name = L"ContactList";
			this->Text = L"ContactList";
			this->Load += gcnew System::EventHandler(this, &ContactList::ContactList_Load);
			this->ResumeLayout(false);

		}
#pragma endregion

/*public: static ContactList^ form; // Ссылка на форму внутри неё самой. Всё гениальное просто ;)
public: static CommonClass^ common;*/

/*private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e)
{
	// Отправка сообщения. Кодируем строку а остальное сделает функция создания пакета
	array<Byte>^ msg = gcnew array<Byte>(textBox2->Text->Length);
	for(int i = 0; i < textBox2->Text->Length; i++)
		msg[i] = textBox2->Text[i];
	//msg[msg->Length - 1] = 0;
	array<Byte>^ packet = gcnew array<Byte>(common->PacketSize);
	int pSize = common->createCLI_SEND_ICBM_CH1(packet,UINBox->Text,msg,true);
	common->sendFlap(packet,pSize);
}*/
/*private: System::Void button2_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void ContactList_Load(System::Object^  sender, System::EventArgs^  e) {
			 }*/
private: System::Void ContactList_Load(System::Object^  sender, System::EventArgs^  e)
{
	// Считываем последний статус и, если это не оффлайн, подключаемся и устанавливаем его.
	IniFile^ pOptions = gcnew IniFile(GlobalOptions::profilePath + "\\options.ini"); 
}
};
}
