#include "stdafx.h"
#include "Form1.h"
#include "FORMS.h"

using namespace System;

using namespace ICQ_client;

Void Form1::button1_Click(System::Object^  sender, System::EventArgs^  e)
{
		FORMS::contactList->Show();
	switch(common->loginStatus)
	{

	case LS_DISCONNECTED:
		try
		{
			// �������������� �����
			common->mHandler = gcnew Thread(gcnew ThreadStart(&common->mHandling));
			// ��������� �������
			if(common->log)
				common->log->Close();
			common->log = gcnew StreamWriter("logfile.txt",false,Encoding::GetEncoding(1251));
			common->log->AutoFlush = true;
			// ����������� � ������� login.icq.com
			common->UIN = textBox1->Text;
			common->password = textBox2->Text;
			common->loginStatus = LS_LOGIN;
			common->loginClient = gcnew TcpClient(common->loginServerName,common->loginServerPort);
			// ��������� ����� �� ��������� ����������� �������
			common->server = common->loginClient->GetStream();
			common->mHandler->Start();
			timer1->Enabled = true;
			button1->Text = "��������";
		}
		catch(Exception^ e)
		{
			common->log->WriteLine("----------\n������ �����������. ����������� ��������.");
			common->log->Close();
			common->mHandler->Abort();
			logStatusLabel->Text = "���������";
			timer1->Enabled = false;
			progressBar1->Value = 0;
			button1->Text = "������������";
			common->loginStatus = LS_DISCONNECTED;
			common->loginStage = 0;
			MessageBox::Show("����������� � ������� �� �������. ��������� �� ������: " + e->Message,"������ �����������",MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
		}
		break;
	case LS_LOGIN:
		common->loginClient->Client->Disconnect(false);
	case LS_REALSERVER:
	case LS_CONNECTED:
		common->serverClient->Client->Disconnect(false);
		common->log->WriteLine("----------\n����������� �� �������.");
		common->log->Close();
		common->mHandler->Abort();
		logStatusLabel->Text = "���������";
		timer1->Enabled = false;
		progressBar1->Value = 0;
		button1->Text = "������������";
		common->loginStatus = LS_DISCONNECTED;
		common->loginStage = 0;
	}
};