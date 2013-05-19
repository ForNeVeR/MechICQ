#include "ProfileChooser.h"

using namespace System;
using namespace MechICQ;

void ProfileChooser::ProfileChooser_FormClosing(Object ^sender, FormClosingEventArgs ^e)
{
	options->Close();
}
