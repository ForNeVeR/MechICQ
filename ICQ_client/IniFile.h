/*	������ ���� - ����������� ������ IniFile.

	����� ��������� �� ������� �����
	- ��������� ����� - Unicode.
	- ������ ����� ������������ � ��������� �������:
	  * ������������ ����������� (������, ������ ��-���������� �������� ������� �������� ����� �
	    ������� ';';
	  * ������������ ����� ������, �� ���������� ����� ��������� � �� ���������� �����������
	    ������.
	- ������� ������������ � ��������� �������:
	  * ������������ �������, ����������� � ������ ������;
	  * ������������ ������� ����� ����� ��������, �� �� ����� ��������� '=';
	  * ������������ ������� ����� ����� ��������� '=', �� �� ��������� ��������.
	  ������� � ������ ������ �� ������������!
	- ����� ������� ������������ ����� ����������� ���������� ������ '[' � ����� ������.

	������������
	- ����������� - ������, ������������ � ����� � ������� ';'. ������������ ��� ��������� �����.
	- ������ - ���, ����������� � ���������� ������ [ � ]. �������� ��� �� "����������" ���
      ���������, ������������� �� ��������� �������. ������ ������������� � ������� ��������� ���
	  � ���������� �����.
	TODO: ������ ����� ���� ��������� � ����� ��������� ���; ��� ���� ��� � ��������� �����
		  ����������� ��� ���� � �� �� ������.
	TODO: ����������� ��������� "������ �� ���������", ���� ����� �������� �������� ��� ������.
	- ������� - ������ ������� "��� = ��������" (��� ������������� �������� ��. ����). ���� �
	  ������ � ����������� �������� ��������� ������ ���������, �� ����������� ����� �������
	  ������, �.�. ���� ��������� �� ����� ���������� � ����� ��������, �� ����� - � ��������.
	  �������� ����� �������� �� �����, �� �� �� ��������. ������� ��������� � ����������� �������
	  � ����� ������ �� ��������������; ��� ������ ����� �������� ������ ���� �� ���.

	��������!
	������ � �������� � ������� ������� ��������� �� ��������� ����� ��� �������� ����������
	������! ���������� � ����� ������� � ���������� �����-���� �������� ������ �� �������������!

	��������� � ������ ����� �� ��������������.
	��������� ��� ������, ������ �����, ����� �������.
*/

#pragma once

using namespace System::IO;
using namespace System;

ref class IniFile
{
	// ������������������ ������ � �� ����������� - ������������ ���������
	// �������� ������
	ref struct IniParam
	{
		String^ name;
		String^ val;
		IniParam^ next;
	};
	// ��������� - ������ ini �����
	public: ref struct IniSection
	{
		// ����������� ������ ������ ������ �������� ��� �������� ������
		IniSection(String^ sectionName)
		{
			this->name = sectionName;
			this->paramCount = 0;
		}
		String^ name;
		IniParam^ firstParam;
		int paramCount;
		IniSection^ next;
		IniParam^ getParam(String^ paramName) // ������ ������� ���������� ��������� �� �������� � �������� ������ ��� nullptr, ���� ������ ��������� ���
		{
			if(firstParam)
			{
				IniParam^ buff = firstParam;
				for(int i = 0; i < paramCount; i ++)
				{
					if(buff->name == paramName)
						return buff;
					if(buff->next)
						buff = buff->next;
				}
			}
			return nullptr;
		}
		Void setParam(String^ paramName, String^ paramVal)
		{
			if(!firstParam)
			{
				firstParam = gcnew IniParam();
				firstParam->name = paramName;
				firstParam->val = paramVal;
				paramCount ++;
			}
			else
			{
				IniParam^ buff = getParam(paramName);
				if(buff)
					buff->val = paramVal;
				else
				{
					IniParam^ buff = firstParam;
					// ������� ��������� ��������
					for(int i = 0; i < this->paramCount - 1; i ++)
					{
						buff = buff->next;
					}
					// � ������ �����
					buff->next = gcnew IniParam();
					buff = buff->next;
					buff->name = paramName;
					buff->val = paramVal;
					paramCount ++;
				}
			}
		}
	};
	IniSection^ getSection(String^ sectionName)
	{
		if(firstSection)
		{
			IniSection^ buff = firstSection;
			for(int i = 0; i < this->sectionCount; i ++)
			{
				if(buff->name == sectionName)
					return buff;
				buff = buff->next;
			}
		}
		return nullptr;
	}
	Void cleanSection(String^ sectionName)
	{
		IniSection^ section = getSection(sectionName);
		section->firstParam = nullptr;
		section->paramCount = 0;
	}
	Void cleanFile()
	{
		this->firstSection = nullptr;
		this->sectionCount = 0;
	}
	Void addSection(String^ sectionName)
	{
		if(!firstSection)
			firstSection = gcnew IniSection(sectionName);
		else
		{
			IniSection^ buff = firstSection;
			for(int i = 0; i < sectionCount - 1; i ++)
			{
				buff = buff->next;
			}
			buff->next = gcnew IniSection(sectionName);
		}
		sectionCount ++;
	}
	int sectionCount;
	IniSection^ firstSection;
	// ����� ���������� ���� ��� ������������ ���������
	private: String^ path;
	
	public: IniFile(String^ filePath)
	// �����������. ��������� ������ �� ini ����� � ���������� � ������������ ���������.
	{
		sectionCount = 0;
		// ���� ����� �� ����������, �������� ��� � ��������
		this->path = filePath;
		if(File::Exists(this->path)) // ��������� ���������� �����, ������ ���� �� ����������
		{
			StreamReader^ ini = gcnew StreamReader(this->path);
			String^ sectionName = "";
			while(!ini->EndOfStream)
			{
				// ������ ���� � ���������� � ������������ ���������
				String^ buff = ini->ReadLine();
				// ������� ������� � ��������� � ������ ������
				while(buff[0] == ' ')
					buff = buff->Remove(0,1);
				switch(buff[0])
				{
				case ';': // ���������� �����������
					continue;
					break;
				case '[': // ������
					sectionName = buff->Substring(1,buff->IndexOf(']') - 1);
					if(!getSection(sectionName))
						addSection(sectionName); // ���� �������� ����� ������ ��������� �� �����, �� ��������� ������ ��� �� ��������
					break;
				default: // ��������
					if(buff == "")
						break; // ��������� ���������� ��� ������ ������
					// ������� ������� � ����� ������
					while(buff[buff->Length - 1] == ' ')
						buff = buff->Remove(buff->Length - 1);
					if(buff->IndexOf('=') <=0) // ���� ��� ����� ��������� ��� ������ ���������� � ���� (�.�. ��� �������� ���������), �� ���������� ������
						break;
					while(buff[buff->IndexOf('=') - 1] == ' ') // ������� ������� ����� ������ ���������
						buff = buff->Remove(buff->IndexOf('=') - 1,1);
					String^ paramName = buff->Substring(0,buff->IndexOf('='));
					buff = buff->Substring(buff->IndexOf('=') + 1);
					if(buff == "") // ���� �������� �������� �� �����
					{
						getSection(sectionName)->setParam(paramName,"");
						break;
					}
					while(buff[0] == ' ') // ������� ������ ��������� �� ��������
						buff = buff->Remove(0,1);
					// ����� ��������� �� �������� ������� �� �e���, ������ ��� �������
					String^ paramVal = buff;
					getSection(sectionName)->setParam(paramName,paramVal); // ������� �������� � ������
				}
			}
			ini->Close();
		}
	}
	// ������� ������ ���������
	public: String^ getParameter(String^ sectionName, String^ paramName)
		{
			IniSection^ section = getSection(sectionName);
			if(section)
			{
				IniParam^ param = section->getParam(paramName);
				if(param)
					return param->val;
			}
			return "";
		}
	// ������� ������ ���������
	public: Void setParameter(String^ sectionName, String^ paramName, String^ paramVal)
	{
		IniSection^ section = getSection(sectionName);
		if(!section)
		{		
			addSection(sectionName);
			section = getSection(sectionName);
		}
		section->setParam(paramName,paramVal);
	}
	// ������� �������� �����, ������ ��� ��������� � ����
	public: Void Close()
	{
		try // ������ ����, ���� ��� �� ����������
		{
			if(!Directory::Exists(Path::GetDirectoryName(this->path)))
			{
				// ������ �������
				Directory::CreateDirectory(Path::GetDirectoryName(this->path));
			}
			if(!File::Exists(this->path))
			{
				// ������ ����
				FileStream^ fs = File::Create(this->path);
				fs->Close();
			}
		}
		catch(Exception^ e)
		{
			throw e;
		}
		StreamWriter^ ini = gcnew StreamWriter(this->path,false,System::Text::Encoding::Unicode);
		ini->WriteLine("; ��������! ������ ���� ��� ������������ ������������� � ��� ����������� � ������������ ����� ����� ������� �� ���� ��� ��������� ����������!");
		IniSection^ sbuff = firstSection;
		for(int i = 0; i < sectionCount; i ++)
		{
			if(sbuff->name != "")
			{
				ini->WriteLine("[" + sbuff->name + "]");
				IniParam^ pbuff = sbuff->firstParam;
				for(int j = 0; j < sbuff->paramCount; j ++)
				{
					if(pbuff->name != "")
						ini->WriteLine(pbuff->name + "=" + pbuff->val);
					if(sbuff->paramCount - j > 0)
						pbuff = pbuff->next;
				}
			}
			if(sectionCount - i > 0)
				sbuff = sbuff->next;
		}
		ini->Close();
	}
};