/* ���� ���� �������� � ���� ����� FORMS, �������� ��������� �� ��� ����� ���������. */

#pragma once

#include "ProfileChooser.h"
#include "ContactList.h"

using namespace ICQ_client;

ref struct FORMS
{
	static ProfileChooser^ profileChooser;
	static ContactList^ contactList;
};