#include "ConnectForm.h"
#include "FORMS.h"

using namespace System;

using namespace MechICQ;

Void ConnectForm::button1_Click(System::Object^  sender, System::EventArgs^  e)
{
		FORMS::contactList->Show();
	switch(common->loginStatus)
	{

	case LS_DISCONNECTED:
		try
		{
			// Инициализируем поток
			common->mHandler = gcnew Thread(gcnew ThreadStart(&common->mHandling));
			// Открываем логфайл
			if(common->log)
				common->log->Close();
			common->log = gcnew StreamWriter("logfile.txt",false,Encoding::GetEncoding(1251));
			common->log->AutoFlush = true;
			// Подключение к серверу login.icq.com
			common->UIN = textBox1->Text;
			common->password = textBox2->Text;
			common->loginStatus = LS_LOGIN;
			common->loginClient = gcnew TcpClient(common->loginServerName,common->loginServerPort);
			// Запускаем поток на прослушку поступающих пакетов
			common->server = common->loginClient->GetStream();
			common->mHandler->Start();
			timer1->Enabled = true;
			button1->Text = "Прервать";
		}
		catch(Exception^ e)
		{
			common->log->WriteLine("----------\nОшибка подключения. Подключение отменено.");
			common->log->Close();
			common->mHandler->Abort();
			logStatusLabel->Text = "Отключено";
			timer1->Enabled = false;
			progressBar1->Value = 0;
			button1->Text = "Подключиться";
			common->loginStatus = LS_DISCONNECTED;
			common->loginStage = 0;
			MessageBox::Show("Подключение к серверу не удалось. Сообщение об ошибке: " + e->Message,"Ошибка подключения",MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
		}
		break;
	case LS_LOGIN:
		common->loginClient->Client->Disconnect(false);
	case LS_REALSERVER:
	case LS_CONNECTED:
		common->serverClient->Client->Disconnect(false);
		common->log->WriteLine("----------\nОтключились от сервера.");
		common->log->Close();
		common->mHandler->Abort();
		logStatusLabel->Text = "Отключено";
		timer1->Enabled = false;
		progressBar1->Value = 0;
		button1->Text = "Подключиться";
		common->loginStatus = LS_DISCONNECTED;
		common->loginStage = 0;
	}
};
