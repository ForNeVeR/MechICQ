/*	Данный файл - определение класса IniFile.

	ОБЩИЕ ЗАМЕЧАНИЯ ПО ФОРМАТУ ФАЙЛА
	- Кодировка файла - Unicode.
	- Строки файла игнорируются в следующих случаях:
	  * игнорируются комментарии (строки, первым не-пробельным символом которых является точка с
	    запятой ';';
	  * игнорируются любые строки, не содержащие знака равенства и не являющиеся заголовками
	    секций.
	- Пробелы игнорируются в следующих случаях:
	  * игнорируются пробелы, находящиеся в начале строки;
	  * игнорируются пробелы после имени элемента, но до знака равенства '=';
	  * игнорируются пробелы после знака равенства '=', но до параметра элемента.
	  Пробелы в именах секций не игнорируются!
	- Любые символы игнорируются после закрывающей квадратной скобки '[' в имени секции.

	ТЕРМИНОЛОГИЯ
	- Комментарий - строка, начинающаяся с точки с запятой ';'. Пропускается при обработке файла.
	- Секция - имя, заключённое в квадратные скобки [ и ]. Является как бы "заголовком" для
      элементов, расположенных на следующих строках. Секция заканчивается с началом следующей или
	  с окончанием файла.
	TODO: Секция может быть объявлена в файле несколько раз; при этом все её включения будут
		  рассмотрены как одна и та же секция.
	TODO: Реализовать обработку "секции по умолчанию", куда будут попадать элементы без секций.
	- Элемент - запись формата "имя = значение" (про игнорирование пробелов см. выше). Если в
	  строке с объявлением элемента несколько знаков равенства, то разделяющим будет признан
	  первый, т.е. знак равенства не может находиться в имени элемента, но может - в значении.
	  Возможен поиск элемента по имени, но не по значению. Наличие элементов с одинаковыми именами
	  в одной секции не приветствуется; при чтении будет доступен только один из них.

	ВНИМАНИЕ!
	Секции и элементы с пустыми именами удаляются из выходного файла при закрытии экземпляра
	класса! Обращаться к таким секциям и параметрам каким-либо способом крайне не рекомендуется!

	Табуляции в начале строк не поддерживаются.
	Параметры вне секций, скорее всего, будут глючить.
*/

#pragma once

using namespace System::IO;
using namespace System;

ref class IniFile
{
	// Последовательность секций с их параметрами - динамическая структура
	// Параметр секции
	ref struct IniParam
	{
		String^ name;
		String^ val;
		IniParam^ next;
	};
	// Структура - секция ini файла
	public: ref struct IniSection
	{
		// Конструктор создаёт первый пустой параметр при создании секции
		IniSection(String^ sectionName)
		{
			this->name = sectionName;
			this->paramCount = 0;
		}
		String^ name;
		IniParam^ firstParam;
		int paramCount;
		IniSection^ next;
		IniParam^ getParam(String^ paramName) // Данная функция возвращает указатель на параметр с заданным именем или nullptr, если такого параметра нет
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
					// Находим последний параметр
					for(int i = 0; i < this->paramCount - 1; i ++)
					{
						buff = buff->next;
					}
					// И создаём новый
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
	// Конец объявления всех фич динамической структуры
	private: String^ path;
	
	public: IniFile(String^ filePath)
	// Конструктор. Считывает данные из ini файла и записывает в динамическую структуру.
	{
		sectionCount = 0;
		// Если файла не существует, создадим его и закончим
		this->path = filePath;
		if(File::Exists(this->path)) // Считываем содержимое файла, только если он существует
		{
			StreamReader^ ini = gcnew StreamReader(this->path);
			String^ sectionName = "";
			while(!ini->EndOfStream)
			{
				// Читаем файл и записываем в динамическую структуру
				String^ buff = ini->ReadLine();
				// Удаляем пробелы и табуляции в начале строки
				while(buff[0] == ' ')
					buff = buff->Remove(0,1);
				switch(buff[0])
				{
				case ';': // Пропускаем комментарии
					continue;
					break;
				case '[': // Секция
					sectionName = buff->Substring(1,buff->IndexOf(']') - 1);
					if(!getSection(sectionName))
						addSection(sectionName); // Если элементы одной секции раскиданы по файлу, на результат работы это не повлияет
					break;
				default: // Параметр
					if(buff == "")
						break; // Отдельное завершение для пустой строки
					// Удаляем пробелы в конце строки
					while(buff[buff->Length - 1] == ' ')
						buff = buff->Remove(buff->Length - 1);
					if(buff->IndexOf('=') <=0) // Если нет знака равенства или строка начинается с него (т.е. нет названия параметра), то игнорируем запись
						break;
					while(buff[buff->IndexOf('=') - 1] == ' ') // Удаляем пробелы перед знаком равенства
						buff = buff->Remove(buff->IndexOf('=') - 1,1);
					String^ paramName = buff->Substring(0,buff->IndexOf('='));
					buff = buff->Substring(buff->IndexOf('=') + 1);
					if(buff == "") // Если параметр элемента не задан
					{
						getSection(sectionName)->setParam(paramName,"");
						break;
					}
					while(buff[0] == ' ') // Очищаем начало параметра от пробелов
						buff = buff->Remove(0,1);
					// Конец параметра от пробелов очищать не нeжно, строка уже очищена
					String^ paramVal = buff;
					getSection(sectionName)->setParam(paramName,paramVal); // Заносим параметр в массив
				}
			}
			ini->Close();
		}
	}
	// Функция чтения параметра
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
	// Функция записи параметра
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
	// Функция закрытия файла, вносит все изменения в него
	public: Void Close()
	{
		try // Создаём файл, если его не существует
		{
			if(!Directory::Exists(Path::GetDirectoryName(this->path)))
			{
				// Создаём каталог
				Directory::CreateDirectory(Path::GetDirectoryName(this->path));
			}
			if(!File::Exists(this->path))
			{
				// Создаём файл
				FileStream^ fs = File::Create(this->path);
				fs->Close();
			}
		}
		catch(Exception^ e)
		{
			throw e;
		}
		StreamWriter^ ini = gcnew StreamWriter(this->path,false,System::Text::Encoding::Unicode);
		ini->WriteLine("; Внимание! Данный файл был сгенерирован автоматически и все комментарии и некорректные ключи будут удалены из него при следующем обновлении!");
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