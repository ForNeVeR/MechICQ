#include "ProfileChooser.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace MechICQ;

void ProfileChooser::ProfileChooser_Load(Object ^sender, EventArgs ^e)
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

void ProfileChooser::ProfileChooser_FormClosing(Object ^sender, FormClosingEventArgs ^e)
{
	options->Save();
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
		pOptions->setParameter("Main", "UIN", UINBox->Text);
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
	Close();
}
