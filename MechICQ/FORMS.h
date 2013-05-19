/* Этот файл содержит в себе класс FORMS, хранящий указатели на все формы программы. */

#pragma once

#include "ProfileChooser.h"
#include "ContactList.h"

using namespace MechICQ;

ref struct FORMS
{
	static ProfileChooser^ profileChooser;
	static ContactList^ contactList;
};
