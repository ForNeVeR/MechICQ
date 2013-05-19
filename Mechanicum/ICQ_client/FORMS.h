/* Этот файл содержит в себе класс FORMS, хранящий указатели на все формы программы. */

#pragma once

#include "ProfileChooser.h"
#include "ContactList.h"

using namespace ICQ_client;

ref struct FORMS
{
	static ProfileChooser^ profileChooser;
	static ContactList^ contactList;
};