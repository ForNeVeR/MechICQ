#pragma once

using namespace System;

ref class GlobalOptions abstract sealed
{
public:
	static property String ^ProgramPath;
	static property String ^ProfilePath;
	static property Nullable<UInt64> Uin;
	static property String ^password;
};
