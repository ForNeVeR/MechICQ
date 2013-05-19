/* ���� ����� ���������: ���� �����-�� ������� � ����� �� ����������� ������, ����� �� �����,
   ����� ����������� ����������� ���� ������� � .h ����� �����. ���� ������� ����������� ������
   �����, ����������� � ���������� ������� � .cpp ����, � ������� ������ ���� ���������
   #include "Forms.h"
   -���� ���� �� ������ .ini-������-
   � ������� ����� �������� $APPPATH\options.ini ���������� ������ Profiles � �����������
   DefaultProfile (�������, ������������ �� ���������) � 1, 2, ... � ������� ��������.
   ��������� ����� $APPPATH\<���_�������>\options.ini �������� ������ UIN � ������ Main.
*/

#include "stdafx.h"

#include "Forms.h"
#include "GlobalOptions.h"
#include "IniFile.h"

using namespace System::IO;
using namespace System::Xml;

using namespace ICQ_client;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 
	
	GlobalOptions::programPath = Path::GetDirectoryName(Application::ExecutablePath);
	// ������� ������� �� ��������� � ����� ��������
	IniFile^ options = gcnew IniFile(GlobalOptions::programPath + "\\options.ini");
	if(options->getParameter("Profiles","DefaultProfile") != "")
	{
		GlobalOptions::profilePath = GlobalOptions::programPath + "\\" + options->getParameter("Profiles","DefaultProfile");
		MessageBox::Show("�������� � �������� " + options->getParameter("Profiles","DefaultProfile"));
		options = nullptr;
		// ��������� ���� �������� ������� �, ���� ��� ���� UIN, ���������� ���
		IniFile^ pOptions = gcnew IniFile(GlobalOptions::profilePath + "\\options.ini");
		if(pOptions->getParameter("Main","UIN") != "")
		{
			GlobalOptions::UIN = Convert::ToUInt64(pOptions->getParameter("Main","UIN"));
			MessageBox::Show(pOptions->getParameter("Main","UIN") + " = " + GlobalOptions::UIN);
			// ���������� �������-����
			FORMS::contactList = gcnew ContactList();
			FORMS::contactList->Show();
		}
		else
		{
			// ���������� ���� ������ � �������� �������
			FORMS::profileChooser = gcnew ProfileChooser();
			FORMS::profileChooser->Show();
		}
	}
	else
	{
		// ���������� ���� ������ � �������� �������
		FORMS::profileChooser = gcnew ProfileChooser();
		FORMS::profileChooser->Show();
	}
	// ��������� ��������� � ������� �����
	Application::Run();

	return 0;
}