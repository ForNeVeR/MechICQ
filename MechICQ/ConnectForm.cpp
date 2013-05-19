#include "ConnectForm.h"

#include "GlobalOptions.h"
#include "ProfileChooser.h"

using namespace System;
using namespace MechICQ;

void ConnectForm::Form_Load(System::Object ^sender, System::EventArgs ^e)
{
	// Show profile creation dialog:
	if (GlobalOptions::Uin.Equals(nullptr))
	{
		auto profileDialog = gcnew ProfileChooser();
		auto result = profileDialog->ShowDialog(this);
		if (result != System::Windows::Forms::DialogResult::OK)
		{
			DialogResult = result;
			Close();
			return;
		}
	}

	progressBar1->Maximum = protocol->TotalLoginStages;
}

void ConnectForm::Timer_Tick(System::Object ^sender, System::EventArgs ^e)
{
	switch(protocol->loginStatus)
	{
	case LS_DISCONNECTED:
		protocol->loginStage = 0;
		logStatusLabel->Text = "Отключено";
		protocol->mHandler->Abort();
		progressBar1->Value = 0;
		timer1->Enabled = false;
		break;
	case LS_LOGIN:
	case LS_REALSERVER:
		{
			progressBar1->Value = protocol->loginStage;
			int percent =  100 * (float) protocol->loginStage / protocol->TotalLoginStages;
			logStatusLabel->Text = "Подключение... (" + percent + "%)";
			break;
		}
	case LS_CONNECTED:
		logStatusLabel->Text = "Подключено";
		progressBar1->Value = progressBar1->Maximum;
		button1->Text = "Отключиться";
		timer1->Enabled = false;

		// TODO: Return OK dialog result and close the form.
		break;
	}
}

void ConnectForm::ConnectButton_Click(System::Object ^sender, System::EventArgs ^e)
{
	switch(protocol->loginStatus)
	{
	case LS_DISCONNECTED:
		try
		{
			// Инициализируем поток
			protocol->mHandler = gcnew Thread(gcnew ThreadStart(&protocol->mHandling));
			// Открываем логфайл
			if(protocol->log)
				protocol->log->Close();
			protocol->log = gcnew StreamWriter("logfile.txt",false,Encoding::GetEncoding(1251));
			protocol->log->AutoFlush = true;
			// Подключение к серверу login.icq.com
			protocol->UIN = textBox1->Text;
			protocol->password = textBox2->Text;
			protocol->loginStatus = LS_LOGIN;
			protocol->loginClient = gcnew TcpClient(protocol->loginServerName,protocol->loginServerPort);
			// Запускаем поток на прослушку поступающих пакетов
			protocol->server = protocol->loginClient->GetStream();
			protocol->mHandler->Start();
			timer1->Enabled = true;
			button1->Text = "Прервать";
		}
		catch(Exception^ e)
		{
			protocol->log->WriteLine("----------\nОшибка подключения. Подключение отменено.");
			protocol->log->Close();
			protocol->mHandler->Abort();
			logStatusLabel->Text = "Отключено";
			timer1->Enabled = false;
			progressBar1->Value = 0;
			button1->Text = "Подключиться";
			protocol->loginStatus = LS_DISCONNECTED;
			protocol->loginStage = 0;
			MessageBox::Show("Подключение к серверу не удалось. Сообщение об ошибке: " + e->Message,"Ошибка подключения",MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
		}
		break;
	case LS_LOGIN:
		protocol->loginClient->Client->Disconnect(false);
	case LS_REALSERVER:
	case LS_CONNECTED:
		protocol->serverClient->Client->Disconnect(false);
		protocol->log->WriteLine("----------\nОтключились от сервера.");
		protocol->log->Close();
		protocol->mHandler->Abort();
		logStatusLabel->Text = "Отключено";
		timer1->Enabled = false;
		progressBar1->Value = 0;
		button1->Text = "Подключиться";
		protocol->loginStatus = LS_DISCONNECTED;
		protocol->loginStage = 0;
	}
};
