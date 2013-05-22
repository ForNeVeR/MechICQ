#pragma once

#include "IniFile.h"
#include "GlobalOptions.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::IO;
using namespace System::Xml;
using namespace System::Text;

namespace MechICQ
{
	/// <summary>
	/// Summary for ProfileChooser
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class ProfileChooser : public System::Windows::Forms::Form
	{
	public:
		ProfileChooser(void)
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
		~ProfileChooser()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::ListBox^  profileBox;
	protected: 

	protected: 
	private: System::Windows::Forms::Button^  createProfileButton;
	private: System::Windows::Forms::Button^  okButton;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::TextBox^  profileNameBox;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::TextBox^  UINBox;


	private: System::Windows::Forms::CheckBox^  setDefaultProfileCheckBox;
	private: System::Windows::Forms::Button^  deleteProfileButton;


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
			this->profileBox = (gcnew System::Windows::Forms::ListBox());
			this->createProfileButton = (gcnew System::Windows::Forms::Button());
			this->okButton = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->profileNameBox = (gcnew System::Windows::Forms::TextBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->UINBox = (gcnew System::Windows::Forms::TextBox());
			this->setDefaultProfileCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->deleteProfileButton = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// profileBox
			// 
			this->profileBox->FormattingEnabled = true;
			this->profileBox->Location = System::Drawing::Point(12, 26);
			this->profileBox->Name = L"profileBox";
			this->profileBox->Size = System::Drawing::Size(170, 95);
			this->profileBox->TabIndex = 0;
			// 
			// createProfileButton
			// 
			this->createProfileButton->Location = System::Drawing::Point(191, 127);
			this->createProfileButton->Name = L"createProfileButton";
			this->createProfileButton->Size = System::Drawing::Size(143, 23);
			this->createProfileButton->TabIndex = 1;
			this->createProfileButton->Text = L"Создать";
			this->createProfileButton->UseVisualStyleBackColor = true;
			this->createProfileButton->Click += gcnew System::EventHandler(this, &ProfileChooser::createProfileButton_Click);
			// 
			// okButton
			// 
			this->okButton->Location = System::Drawing::Point(12, 179);
			this->okButton->Name = L"okButton";
			this->okButton->Size = System::Drawing::Size(322, 23);
			this->okButton->TabIndex = 2;
			this->okButton->Text = L"ОК";
			this->okButton->UseVisualStyleBackColor = true;
			this->okButton->Click += gcnew System::EventHandler(this, &ProfileChooser::okButton_Click);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(12, 9);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(107, 13);
			this->label1->TabIndex = 3;
			this->label1->Text = L"Выберите профиль:";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(188, 9);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(115, 13);
			this->label2->TabIndex = 4;
			this->label2->Text = L"Или создайте новый:";
			// 
			// profileNameBox
			// 
			this->profileNameBox->Location = System::Drawing::Point(234, 26);
			this->profileNameBox->Name = L"profileNameBox";
			this->profileNameBox->Size = System::Drawing::Size(100, 20);
			this->profileNameBox->TabIndex = 5;
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(188, 29);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(32, 13);
			this->label3->TabIndex = 6;
			this->label3->Text = L"Имя:";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(188, 55);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(29, 13);
			this->label4->TabIndex = 7;
			this->label4->Text = L"UIN:";
			// 
			// UINBox
			// 
			this->UINBox->Location = System::Drawing::Point(234, 52);
			this->UINBox->Name = L"UINBox";
			this->UINBox->Size = System::Drawing::Size(100, 20);
			this->UINBox->TabIndex = 8;
			// 
			// setDefaultProfileCheckBox
			// 
			this->setDefaultProfileCheckBox->AutoSize = true;
			this->setDefaultProfileCheckBox->Location = System::Drawing::Point(12, 156);
			this->setDefaultProfileCheckBox->Name = L"setDefaultProfileCheckBox";
			this->setDefaultProfileCheckBox->Size = System::Drawing::Size(281, 17);
			this->setDefaultProfileCheckBox->TabIndex = 11;
			this->setDefaultProfileCheckBox->Text = L"Использовать выбранный профиль по умолчанию";
			this->setDefaultProfileCheckBox->UseVisualStyleBackColor = true;
			// 
			// deleteProfileButton
			// 
			this->deleteProfileButton->Location = System::Drawing::Point(15, 127);
			this->deleteProfileButton->Name = L"deleteProfileButton";
			this->deleteProfileButton->Size = System::Drawing::Size(167, 23);
			this->deleteProfileButton->TabIndex = 12;
			this->deleteProfileButton->Text = L"Удалить";
			this->deleteProfileButton->UseVisualStyleBackColor = true;
			this->deleteProfileButton->Click += gcnew System::EventHandler(this, &ProfileChooser::deleteProfileButton_Click);
			// 
			// ProfileChooser
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(346, 216);
			this->Controls->Add(this->deleteProfileButton);
			this->Controls->Add(this->setDefaultProfileCheckBox);
			this->Controls->Add(this->UINBox);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->profileNameBox);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->okButton);
			this->Controls->Add(this->createProfileButton);
			this->Controls->Add(this->profileBox);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedToolWindow;
			this->Name = L"ProfileChooser";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"Выберите профиль";
			this->Load += gcnew System::EventHandler(this, &ProfileChooser::ProfileChooser_Load);
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &ProfileChooser::ProfileChooser_FormClosing);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

private: System::Void ProfileChooser_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e);

IniFile^ options;

private: System::Void createProfileButton_Click(System::Object^  sender, System::EventArgs^  e)
{
	try
	{
		String^ profileName = profileNameBox->Text;
		// Заносим UIN в файл настроек профиля
		auto pOptions = gcnew IniFile(GlobalOptions::ProgramPath + "\\" + profileName + "\\options.ini"); 
		pOptions->setParameter("Main","UIN",UINBox->Text);
		pOptions->Save();

		// Заносим профиль в список на экране
		profileBox->Items->Add(profileNameBox->Text + " / " + UINBox->Text);
	}
	catch(Exception^ e)
	{
		MessageBox::Show("Невозможно создать профиль, ошибка: " + e->Message,"Ошибка создания профиля",MessageBoxButtons::OK, MessageBoxIcon::Error);
	}
};

private: System::Void ProfileChooser_Load(System::Object^  sender, System::EventArgs^  e)
{
	// Cчитываем названия профилей из файла настроек
	options = gcnew IniFile(GlobalOptions::ProgramPath + "\\options.ini");
	int profileCount = 0;
	String^ buff = options->getParameter("Profiles","ProfilesCount");
	if(buff != "")
		profileCount = Convert::ToInt16(buff);
	for(int i = 1; i <= profileCount; i ++)
	{
		// Считываем названия профилей и UIN'ы
		String^ profileName = options->getParameter("Profiles","" + i);
		IniFile^ pOptions = gcnew IniFile(GlobalOptions::ProgramPath + "\\" + profileName + "\\options.ini");
		if(pOptions->getParameter("Main","UIN") != "")
		{
			unsigned __int64 uin = Convert::ToUInt64(pOptions->getParameter("Main","UIN"));
			profileBox->Items->Add(profileName + " / " + uin);
		}
		pOptions = nullptr;
		i ++;
	}
	// Выбираем первый элемент списка
	if(profileBox->Items->Count != 0)
		profileBox->SelectedIndex = 0;
}

private: System::Void deleteProfileButton_Click(System::Object^  sender, System::EventArgs^  e)
{
	// Функция удаления профиля. Удаляет его из списка на экране.
	profileBox->Items->RemoveAt(profileBox->SelectedIndex);
}

private: System::Void okButton_Click(System::Object^  sender, System::EventArgs^  e)
{
	// Пересоздаём секцию Profiles файла настроек
	options->cleanSection("Profiles");
	if(setDefaultProfileCheckBox->Checked)
	{
		String^ buff = "";
		for(int i = 0; i < profileBox->Items[profileBox->SelectedIndex]->ToString()->Length; i ++)
		{
			if(profileBox->Items[profileBox->SelectedIndex]->ToString()[i+1] != '/')
				buff += profileBox->Items[profileBox->SelectedIndex]->ToString()[i];
			else
				break;
		}
		options->setParameter("Profiles","DefaultProfile",buff);
	}
	for(int i = 0; i < profileBox->Items->Count; i ++)
	{
		String^ buff = "";
		for(int j = 0; j < profileBox->Items[i]->ToString()->Length; j ++)
		{
			if(profileBox->Items[i]->ToString()[j+1] != '/')
				buff = String::Concat(buff,profileBox->Items[i]->ToString()[j]);
			else
				break;
		}
		options->setParameter("Profiles","" + (i + 1), buff);
	}
	options->setParameter("Profiles","ProfilesCount","" + profileBox->Items->Count);
	GlobalOptions::ProfilePath = GlobalOptions::ProgramPath + "\\" + profileBox->Items[profileBox->SelectedIndex];
	this->Close();
}

};
}
