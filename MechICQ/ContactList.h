#pragma once

#include "Protocol.h"
#include "GlobalOptions.h"
#include "IniFile.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace MechICQ
{
	/// <summary>
	/// Contact list form.
	/// </summary>
	public ref class ContactList : public System::Windows::Forms::Form
	{
	public:
		ContactList(void)
		{
			InitializeComponent();
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

	private:
		void ContactList_Load(System::Object ^sender, System::EventArgs ^e);

	private: System::Windows::Forms::ListBox^  listBox1;
	private: System::Windows::Forms::ComboBox^  comboBox1;
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
	};
}
