#include "ContactList.h"

#include "ConnectForm.h"

using namespace MechICQ;

void ContactList::ContactList_Load(System::Object ^sender, System::EventArgs ^e)
{
	// Show the connect dialog:
	if (GlobalOptions::Uin.Equals(nullptr))
	{
		auto connectForm = gcnew ConnectForm();
		auto result = connectForm->ShowDialog(this);
		if (result != System::Windows::Forms::DialogResult::OK)
		{
			Application::Exit();
		}
	}
}
