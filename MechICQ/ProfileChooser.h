#pragma once

#include "IniFile.h"
#include "GlobalOptions.h"

namespace MechICQ
{
	/// <summary>
	/// Profile chooser form.
	/// </summary>
	public ref class ProfileChooser : public System::Windows::Forms::Form
	{
	public:
		ProfileChooser(void)
		{
			InitializeComponent();
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
	
	private:
		IniFile^ options;

		void ProfileChooser_Load(System::Object ^sender, System::EventArgs ^e);
		void ProfileChooser_FormClosing(System::Object ^sender, System::Windows::Forms::FormClosingEventArgs ^e);
		void deleteProfileButton_Click(System::Object ^sender, System::EventArgs ^e);
		void createProfileButton_Click(System::Object ^sender, System::EventArgs ^e);		
		void okButton_Click(System::Object ^sender, System::EventArgs ^e);

	private: System::Windows::Forms::ListBox^  profileBox;
	private: System::Windows::Forms::Button^  createProfileButton;
	private: System::Windows::Forms::Button^  okButton;
	private: System::Windows::Forms::Label^  profileListLabel;
	private: System::Windows::Forms::Label^  createProfileLabel;
	private: System::Windows::Forms::TextBox^  profileNameBox;
	private: System::Windows::Forms::Label^  profileNameLabel;
	private: System::Windows::Forms::Label^  profileUinLabel;
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
			this->profileListLabel = (gcnew System::Windows::Forms::Label());
			this->createProfileLabel = (gcnew System::Windows::Forms::Label());
			this->profileNameBox = (gcnew System::Windows::Forms::TextBox());
			this->profileNameLabel = (gcnew System::Windows::Forms::Label());
			this->profileUinLabel = (gcnew System::Windows::Forms::Label());
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
			this->createProfileButton->Text = L"Create";
			this->createProfileButton->UseVisualStyleBackColor = true;
			this->createProfileButton->Click += gcnew System::EventHandler(this, &ProfileChooser::createProfileButton_Click);
			// 
			// okButton
			// 
			this->okButton->Location = System::Drawing::Point(12, 179);
			this->okButton->Name = L"okButton";
			this->okButton->Size = System::Drawing::Size(322, 23);
			this->okButton->TabIndex = 2;
			this->okButton->Text = L"OK";
			this->okButton->UseVisualStyleBackColor = true;
			this->okButton->Click += gcnew System::EventHandler(this, &ProfileChooser::okButton_Click);
			// 
			// profileListLabel
			// 
			this->profileListLabel->AutoSize = true;
			this->profileListLabel->Location = System::Drawing::Point(12, 9);
			this->profileListLabel->Name = L"profileListLabel";
			this->profileListLabel->Size = System::Drawing::Size(89, 13);
			this->profileListLabel->TabIndex = 3;
			this->profileListLabel->Text = L"Available profiles:";
			// 
			// createProfileLabel
			// 
			this->createProfileLabel->AutoSize = true;
			this->createProfileLabel->Location = System::Drawing::Point(188, 9);
			this->createProfileLabel->Name = L"createProfileLabel";
			this->createProfileLabel->Size = System::Drawing::Size(95, 13);
			this->createProfileLabel->TabIndex = 4;
			this->createProfileLabel->Text = L"Create new profile:";
			// 
			// profileNameBox
			// 
			this->profileNameBox->Location = System::Drawing::Point(234, 26);
			this->profileNameBox->Name = L"profileNameBox";
			this->profileNameBox->Size = System::Drawing::Size(100, 20);
			this->profileNameBox->TabIndex = 5;
			// 
			// profileNameLabel
			// 
			this->profileNameLabel->AutoSize = true;
			this->profileNameLabel->Location = System::Drawing::Point(188, 29);
			this->profileNameLabel->Name = L"profileNameLabel";
			this->profileNameLabel->Size = System::Drawing::Size(38, 13);
			this->profileNameLabel->TabIndex = 6;
			this->profileNameLabel->Text = L"Name:";
			// 
			// profileUinLabel
			// 
			this->profileUinLabel->AutoSize = true;
			this->profileUinLabel->Location = System::Drawing::Point(188, 55);
			this->profileUinLabel->Name = L"profileUinLabel";
			this->profileUinLabel->Size = System::Drawing::Size(29, 13);
			this->profileUinLabel->TabIndex = 7;
			this->profileUinLabel->Text = L"UIN:";
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
			this->setDefaultProfileCheckBox->Size = System::Drawing::Size(168, 17);
			this->setDefaultProfileCheckBox->TabIndex = 11;
			this->setDefaultProfileCheckBox->Text = L"Use selected profile as default";
			this->setDefaultProfileCheckBox->UseVisualStyleBackColor = true;
			// 
			// deleteProfileButton
			// 
			this->deleteProfileButton->Location = System::Drawing::Point(15, 127);
			this->deleteProfileButton->Name = L"deleteProfileButton";
			this->deleteProfileButton->Size = System::Drawing::Size(167, 23);
			this->deleteProfileButton->TabIndex = 12;
			this->deleteProfileButton->Text = L"Delete";
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
			this->Controls->Add(this->profileUinLabel);
			this->Controls->Add(this->profileNameLabel);
			this->Controls->Add(this->profileNameBox);
			this->Controls->Add(this->createProfileLabel);
			this->Controls->Add(this->profileListLabel);
			this->Controls->Add(this->okButton);
			this->Controls->Add(this->createProfileButton);
			this->Controls->Add(this->profileBox);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedToolWindow;
			this->Name = L"ProfileChooser";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"Select profile";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &ProfileChooser::ProfileChooser_FormClosing);
			this->Load += gcnew System::EventHandler(this, &ProfileChooser::ProfileChooser_Load);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	};
}
