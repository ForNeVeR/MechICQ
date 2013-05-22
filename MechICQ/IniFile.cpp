#include "IniFile.h"

// Save the file.
void IniFile::Save()
{
	// Create the directory:
	auto directoryPath = Path::GetDirectoryName(path);
	if(!Directory::Exists(directoryPath))
	{
		Directory::CreateDirectory(directoryPath);
	}

	StreamWriter ini(path, false, System::Text::Encoding::Unicode);
	ini.WriteLine("; Auto generated file. Change at your own risk.");
	auto sbuff = firstSection;
	for (int i = 0; i < sectionCount; ++i)
	{
		if (sbuff->name != "")
		{
			ini.WriteLine("[" + sbuff->name + "]");
			auto pbuff = sbuff->firstParam;
			for (int j = 0; j < sbuff->paramCount; ++j)
			{
				if(pbuff->name != "")
				{
					ini.WriteLine(pbuff->name + "=" + pbuff->val);
				}
				if(sbuff->paramCount - j > 0)
				{
					pbuff = pbuff->next;
				}
			}
		}

		if(sectionCount - i > 0)
		{
			sbuff = sbuff->next;
		}
	}
}
