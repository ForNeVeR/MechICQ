#include "ContactList.h"
#include "GlobalOptions.h"
#include "IniFile.h"

using namespace System::IO;
using namespace System::Xml;

using namespace MechICQ;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	String ^OptionsFileName = "options.ini";

	// Enabling Windows XP visual effects before any controls are created:
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 
	
	GlobalOptions::ProgramPath = Path::GetDirectoryName(Application::ExecutablePath);

	// Get the default profile:
	auto options = gcnew IniFile(Path::Combine(GlobalOptions::ProgramPath, OptionsFileName));
	auto defaultProfile = options->getParameter("Profiles", "DefaultProfile");
	if (!String::IsNullOrEmpty(defaultProfile))
	{
		GlobalOptions::ProfilePath = Path::Combine(GlobalOptions::ProgramPath, defaultProfile);
		MessageBox::Show("Starting default profile " + defaultProfile);

		// Open profile options and get UIN:
		auto profileOptions = gcnew IniFile(Path::Combine(GlobalOptions::ProfilePath, OptionsFileName));
		auto uin = profileOptions->getParameter("Main", "UIN");
		if(!String::IsNullOrEmpty(uin))
		{
			GlobalOptions::Uin = Convert::ToUInt64(uin);
		}
	}

	Application::Run(gcnew ContactList());

	return 0;
}
