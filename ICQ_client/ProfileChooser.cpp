#include "StdAfx.h"
#include "ProfileChooser.h"

#include "Forms.h"

using namespace System;

Void ProfileChooser::ProfileChooser_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e)
{
	options->Close();
	if(!FORMS::contactList)
		Application::Exit();
}

