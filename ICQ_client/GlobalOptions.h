/* ���� ���� �������� �����, �������� ������ ���������� ��������� ��������� ���� UIN'�, ������ �
   �.�.
   ������ ����� #define'� ���� ������ ����.
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