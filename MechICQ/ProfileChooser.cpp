#include "ProfileChooser.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace MechICQ;

void ProfileChooser::ProfileChooser_Load(Object ^sender, EventArgs ^e)
{
	// Cчитываем названия профилей из файла настроек
	_options = gcnew IniFile(GlobalOptions::ProgramPath + "\\options.ini");
	int profileCount = 0;
	String^ buff = _options->getParameter("Profiles","ProfilesCount");
	if(buff != "")
		profileCount = Convert::ToInt16(buff);
	for(int i = 1; i <= profileCount; i ++)
	{
		// Считываем названия профилей и UIN'ы
		String^ profileName = _options->getParameter("Profiles","" + i);
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

void ProfileChooser::ProfileChooser_FormClosing(Object ^sender, FormClosingEventArgs ^e)
{
	_options->Save();
}

void ProfileChooser::deleteProfileButton_Click(Object ^sender, EventArgs ^e)
{
	auto index = profileBox->SelectedIndex;
	if (index != -1)
	{
		profileBox->Items->RemoveAt(index);
	}
}

void ProfileChooser::createProfileButton_Click(Object ^sender, EventArgs ^e)
{
	try
	{
		String^ profileName = profileNameBox->Text;
		// Заносим UIN в файл настроек профиля
		auto pOptions = gcnew IniFile(GlobalOptions::ProgramPath + "\\" + profileName + "\\options.ini"); 
		pOptions->SetParameter("Main", "UIN", UINBox->Text);
		pOptions->Save();

		// Заносим профиль в список на экране
		profileBox->Items->Add(profileNameBox->Text + " / " + UINBox->Text);
	}
	catch(Exception ^e)
	{
		MessageBox::Show("Невозможно создать профиль, ошибка: " + e->Message, "Ошибка создания профиля", MessageBoxButtons::OK, MessageBoxIcon::Error);
	}
}

void ProfileChooser::okButton_Click(Object ^sender, EventArgs ^e)
{
	auto index = profileBox->SelectedIndex;
	if (index == -1)
	{
		return;
	}
	
	auto profileName = safe_cast<String^>(profileBox->SelectedItem);

	_options->CleanSection("Profiles");
	if (setDefaultProfileCheckBox->Checked)
	{		
		_options->SetParameter("Profiles", "DefaultProfile", profileName);
	}

	auto items = profileBox->Items;
	auto i = 0;
	for each (String ^profile in items)
	{
		_options->SetParameter("Profiles", Convert::ToString(++i), profile);
	}

	_options->SetParameter("Profiles", "ProfilesCount", Convert::ToString(items->Count));
	GlobalOptions::ProfilePath = Path::Combine(GlobalOptions::ProgramPath, profileName);

	DialogResult = System::Windows::Forms::DialogResult::OK;
	Close();
}
