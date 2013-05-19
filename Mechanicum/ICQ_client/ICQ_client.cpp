/* Пара общих замечаний: если какая-то функция в форме не затрагивает ничего, кроме неё самой,
   можно располагать определение этой функции в .h файле формы. Если функция затрагивает другие
   формы, обязательно её необходимо вынести в .cpp файл, в котором должно быть прописано
   #include "Forms.h"
   -Пара слов по поводу .ini-файлов-
   В главном файле настроек $APPPATH\options.ini содержится секция Profiles с параметрами
   DefaultProfile (профиль, используемый по умолчанию) и 1, 2, ... с именами профилей.
   Остальные файлы $APPPATH\<Имя_профиля>\options.ini содержат только UIN в секции Main.
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
	// Находим профиль по умолчанию в файле настроек
	IniFile^ options = gcnew IniFile(GlobalOptions::programPath + "\\options.ini");
	if(options->getParameter("Profiles","DefaultProfile") != "")
	{
		GlobalOptions::profilePath = GlobalOptions::programPath + "\\" + options->getParameter("Profiles","DefaultProfile");
		MessageBox::Show("Стартуем с профилем " + options->getParameter("Profiles","DefaultProfile"));
		options = nullptr;
		// Открываем файл настроек профиля и, если там есть UIN, используем его
		IniFile^ pOptions = gcnew IniFile(GlobalOptions::profilePath + "\\options.ini");
		if(pOptions->getParameter("Main","UIN") != "")
		{
			GlobalOptions::UIN = Convert::ToUInt64(pOptions->getParameter("Main","UIN"));
			MessageBox::Show(pOptions->getParameter("Main","UIN") + " = " + GlobalOptions::UIN);
			// Показываем контакт-лист
			FORMS::contactList = gcnew ContactList();
			FORMS::contactList->Show();
		}
		else
		{
			// Показываем окно выбора и создания профиля
			FORMS::profileChooser = gcnew ProfileChooser();
			FORMS::profileChooser->Show();
		}
	}
	else
	{
		// Показываем окно выбора и создания профиля
		FORMS::profileChooser = gcnew ProfileChooser();
		FORMS::profileChooser->Show();
	}
	// Переводим программу в рабочий режим
	Application::Run();

	return 0;
}