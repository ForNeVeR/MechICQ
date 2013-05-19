/* Этот файл содержит класс, хранящий разные глобальные настройки программы типа UIN'а, пароля и
   т.д.
   Всякие общие #define'ы тоже кидать сюда.
*/

#pragma once

using namespace System;

ref struct GlobalOptions
{
	static String^ programPath;
	static String^ profilePath;
	static unsigned __int64 UIN;
	static String^ password;
};
