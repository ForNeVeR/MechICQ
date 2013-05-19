#pragma once

#include "stdafx.cpp"
#include "CommonClass.h"

using namespace System;
using namespace System::Text;
using namespace System::Windows::Forms;

CommonClass::CommonClass()
{
}

// === ��������� ������� ===

String^ CommonClass::hexNumber(Byte byte)
// ������� ����� ����������������� ����� � ���� ������ ���� "0A" � ������� �� System::Convert::ToString(byte,16) (��������� "a")
{
	String^ buff = System::Convert::ToString(byte,16);
	buff = buff->ToUpper();
	if (buff->Length < 2)
		buff = "0" + buff;
	return buff;
}

// === ������� ��������� ����������� ������� ===

bool CommonClass::flapHeaderOk(array<Byte>^ header)
// �������, ��������� FLAP-��������� ������. ��������� - � ������� �� ���������. �������������
// ������ ������ 2 ����� ������.
{
	if(header[0] == 0x2A && header[1] >= 1 && header[1] <= 5)
	{
		if(connectedToServer)
		{
			// ���� �� ��� ���������� � ������� (�.�. ��������� ����� - �� SRV_HELLO), �� ���������� �������������� sequence number'� � ��������� � ���, ���� ���.
			int wserverSeqNum = serverSeqNum + 1;
			if(wserverSeqNum >= SEQNUM_RESET_VALUE)
				wserverSeqNum = 0; // ����� ���� �������� wserverSeqNum - ��������� serverSeqNum
			serverSeqNum = header[2] * 256 + header[3];
			if(serverSeqNum != wserverSeqNum)
				log->WriteLine("[!] ���������� sequence number (" + hexNumber(header[2]) +" " + hexNumber(header[3]) + ") �� ������������� ���������� (" + hexNumber(wserverSeqNum / 256) + " " + hexNumber(wserverSeqNum - (int)(wserverSeqNum / 256) * 256) + "). �������� ������ �������.");
		}
		return true;
		}
	else
		return false;
}

int CommonClass::getTlv(array<Byte>^ packet, int pSize, int typeOfTlv, int offset)
// ������� ����� ������ ������� ����� ������������ TLV-������ ��� 0, ���� ����� �� ������.
// ��� ������ ����� ������ ������ (���� ����������-�� > 64 ��! �� ��������� �� � ���!)
// ����� ������� �������� (�� ������, ���� � ������ � ������ ���� SNAC-��������� ��� ��� ����-������.
{
	int type;
	int size;
	for(int i = offset; i < pSize; )
	{
		type = packet[i] * 256 + packet[i+1];
		size = packet[i+2] * 256 + packet[i+3];
		if (type == typeOfTlv)
		{
			return i;
			break;
		}
		i += size + 4;
	}
	return 0;
}

System::Void CommonClass::determine (array<Byte>^ packet)
// ������ ������� ���������� ��� ������������ ������ � ���������� ��������.
{
	// ����� �� ��������� ����� �� ���������, ��� ��� ��� ������� ������� � ��� ���������� �� ������.
	log->WriteLine("��������� ������...");
	// ��������� �����, �� �������� ������ �����...
	int channel = packet[1];
	log->WriteLine("����� ������ �� ������ " + channel + ".");
	serverSeqNum = packet[2] * 256 + packet[3];
	log->WriteLine("Sequence number: " + hexNumber(packet[2]) + " " + hexNumber(packet[3]));
	int pSize = packet[4] * 256 + packet[5] + 6;
	log->WriteLine("����� ������: " + (pSize - 6) + " (+ 6 ���� ���������).");
	switch(channel)
	{
	case 1: // New connection negitiation
		// SRV_HELLO
		if(packet[4] == 0 && packet[5] == 4 && packet[6] == 0 && packet[7] == 0 && packet[8] == 0 && packet[9] == 1)
		{
			connectedToServer = true;
			log->WriteLine("������� ����� SRV_HELLO: ����������� � ������� �����������.");
			loginStage ++;
			// ������, ���� �� ���������� � �����-�������, ����� �������� ��� CLI_IDENT ������ � �������, ������� � ������ �����.
			if(loginStatus == LS_LOGIN)
			{
				// � ��������, packet �� ��� ���������� � �� ��� �� �����, ��� ��� ����� ������������ ���.
				// ��������� �� ������ ��� ������������ � �������, � ��� ��������������� SRV_HELLO, ����� ������������ ����� seqNum
				System::Random^ rand = gcnew System::Random;
				seqNum = rand->Next(SEQNUM_RESET_VALUE); // �� ����, ����� �� � ���� ������ seqNum ����� == 0x8000, ������ ��� �� ����� ����� ��������� ��� �������� ������
				pSize = createCLI_IDENT(packet); // packet ����������� ������� ������� � ����� � ��������
				sendFlap(packet,pSize);
			}
			// � ���� � ��������� �������, ��� ����� �������� CLI_COOKIE
			if(loginStatus == LS_REALSERVER)
			{
				System::Random^ rand = gcnew System::Random;
				seqNum = rand->Next(SEQNUM_RESET_VALUE); // �� ����, ����� �� � ���� ������ seqNum ����� == 0x8000, ������ ��� �� ����� ����� ��������� ��� �������� ������
				pSize = createCLI_COOKIE(packet); // packet ����������� ������� ������� � ����� � ��������
				sendFlap(packet,pSize);				
			}
			break;
		}
	case 2: // SNAC data
		{
			// ��������� family � type
			int family = packet[6] * 256 + packet[7];
			int type = packet[8] * 256 + packet[9];
			int flags = packet[10] * 256 + packet[11];
			int requestID = packet[12] * 0x1000000 + packet[13] * 0x10000 + packet[14] * 0x100 + packet[15];
			log->WriteLine("������� SNAC-�����: family = " + family + ", type = " + type + ", flags = " + hexNumber(packet[10]) + " " + hexNumber(packet[11]) + ", request ID: " + hexNumber(packet[12]) + " " + hexNumber(packet[13]) + " " + hexNumber(packet[14]) + " " + hexNumber(packet[15]));
			if(family == 1 && type == 3) // SRV_FAMILIES
			{
				loginStage ++;
				// ���� ����� �������� ������ �������������� ��������, ��� ��� �� ���� ���������.
				int offset = 16;
				supportedSnacs = gcnew array<int>((pSize - 16) / 2);
				for(int i = 0; i < (pSize - 16) / 2; i++)
				{
					supportedSnacs[i] = packet[offset + i * 2] * 256 + packet[offset + i * 2 + 1];
				}
				log->WriteLine("������� SNAC(1,3): SRV_FAMILIES, �������� ������ �������������� ��������.");
				log->Write("������:");
				for(int i = 0; i < supportedSnacs->Length; i++)
					log->Write(" " + hexNumber((supportedSnacs[i] / 256)) + hexNumber((supportedSnacs[i] - (int)(supportedSnacs[i] / 256) * 256)));
				log->WriteLine();
				// ������ ����� ��������� ����� � ������� ��������� � �� ��������
				pSize = makeSnac(packet,1,0x17,0,1); // ������ ������ SNAC ����� � ���������� �����������
				array<Byte>^ data = gcnew array<Byte>(supportedSnacs->Length * 4);
				// ��������� ���� ����� ����������� ������ ��� ������� ��������������� ������� ������ 1
				for(int i = 0; i < supportedSnacs->Length; i++)
				{
					data[4 * i] = supportedSnacs[i] / 256;
					data[4 * i + 1] = supportedSnacs[i] - (int)(supportedSnacs[i] / 256) * 256;
					data[4 * i + 2] = 0;
					data[4 * i + 3] = 1;
				}
				pSize = addData(packet,data,pSize);
				sendFlap(packet,pSize);
				break;
			}
			if(family == 1 && type == 7)
			{
				loginStage ++;
				log->WriteLine("������� ����� SNAC(1,7): SRV_RATE_LIMIT_INFO, ��������� ���������� � rate-limits.");
				classCount = packet[16] * 256 + packet[17];
				// ��������� �������� ����
				rID = gcnew array<int>(classCount);
				windowSize = gcnew array<int>(classCount);
				clear = gcnew array<int>(classCount);
				alert = gcnew array<int>(classCount);
				limit= gcnew array<int>(classCount); 
				disconnect = gcnew array<int>(classCount);
				current = gcnew array<int>(classCount);
				max = gcnew array<int>(classCount);
				lastTime = gcnew array<long long>(classCount);
				for(int i = 0; i < classCount; i++)
				{
					int offset = 18 + 30 * i;
					rID[i] = packet[offset] * 0x100 + packet[offset + 1];
					offset += 2;
					windowSize[i] = packet[offset] * 0x1000000 + packet[offset + 1] * 0x10000 + packet[offset + 2] * 0x100 + packet[offset + 3];
					offset += 4;
					clear[i] = packet[offset] * 0x1000000 + packet[offset + 1] * 0x10000 + packet[offset + 2] * 0x100 + packet[offset + 3];
					offset += 4;
					alert[i] = packet[offset] * 0x1000000 + packet[offset + 1] * 0x10000 + packet[offset + 2] * 0x100 + packet[offset + 3];
					offset += 4;
					limit[i] = packet[offset] * 0x1000000 + packet[offset + 1] * 0x10000 + packet[offset + 2] * 0x100 + packet[offset + 3];
					offset += 4;
					disconnect[i] = packet[offset] * 0x1000000 + packet[offset + 1] * 0x10000 + packet[offset + 2] * 0x100 + packet[offset + 3];
					offset += 4;
					current[i] = packet[offset] * 0x1000000 + packet[offset + 1] * 0x10000 + packet[offset + 2] * 0x100 + packet[offset + 3];
					offset += 4;
					max[i] = packet[offset] * 0x1000000 + packet[offset + 1] * 0x10000 + packet[offset + 2] * 0x100 + packet[offset + 3];
				}
				// ������ ��������� ������ SNAC-�������, ����������� � ���� ��� ����� ������.
				// ���� family-type SNAC-������� ��������� � ������ ��� family * 0x10000 + type
				snacPairs = gcnew array<int,2>(classCount,(pSize - (6 + 2 + 30 * classCount + 4)) / 4); // ��� �� ������ ������, ������� �� �������, �.�. ��� ��� ���������� ����� ������ ������������ �� ���� ���� ������.
				int offset = 6 + 10 + 2 + 30 * classCount;
				int i = 0;
				while(i < pSize)
				{
					int id = packet[i + offset] * 256 + packet[i + offset + 1];
					int count = packet[i + offset + 2] * 256 + packet[i + offset + 3];
					for(int j = 0; j < classCount; j++)
					{
						if(id == rID[j])
						{
							for(int k = 0; k < count; k++)
							{
								int family = packet[offset + i + 4 + k * 4] * 256 + packet[offset + i + 4 + k * 4 + 1];
								int type = packet[offset + i + 4 + k * 4 + 2] * 256 + packet[offset + i + 4 + k * 4 + 3];
								snacPairs[j,k] = family * 0x10000 + type;
							}
							break;
						}
					}
					i += (4 + count * 4);
				}
				log->WriteLine("��� ���������� � rate-limits ����� ��������� � ���� ratelim.txt");
				StreamWriter^ ratelim = gcnew StreamWriter("ratelim.txt",false,Encoding::GetEncoding(1251));
				ratelim->WriteLine("=== RATE-LIMITS ===");
				ratelim->WriteLine("(csv format)");
				ratelim->WriteLine("id,windowSize,clear,alert,limit,disconnect,current,max,snacPair1,snacPair2,...");
				for(int i = 0; i < classCount; i++)
				{
					ratelim->Write("" + rID[i] + "," + windowSize[i] + "," + clear[i] + "," + alert[i] + "," + limit[i] + "," + disconnect[i] + "," + current[i] + "," + max[i]);
					int j = 0;
					while(snacPairs[i,j] != 0)
					{
						ratelim->Write("," + (snacPairs[i,j] / 0x10000) + " " + (snacPairs[i,j] - (int)(snacPairs[i,j] / 0x10000) * 0x10000));
						j++;
					}
					ratelim->WriteLine();
				}
				ratelim->Close();
				ratelim = nullptr;
				// ������ ����� �������� �� ������ ����� CLI_RATES_ACK (SNAC(01,08)), ���������� ������ ������ ����� rate-limits.
				pSize = makeSnac(packet,1,8,0,0);
				for(int i = 0; i < classCount; i++)
				{
					array<Byte>^ data = gcnew array<Byte>(2);
					data[0] = rID[i] / 256;
					data[1] = rID[i] - (int)(rID[i] / 256) * 256;
					pSize = addData(packet,data,pSize);
				}
				loginStage ++;
				sendFlap(packet,pSize);
				// ����� ����, ��� ���� ����� ���������, ����� ��������� ����� ��� ���� ��������� ������� ��� ������ ��������� ������ ������.
				log->WriteLine("������ ������ ������ 3: ��������� ��������.");
				pSize = makeSnac(packet,2,2,0,0); // CLI_LOCATION_RIGHTS_REQ
				loginStage ++;
				sendFlap(packet,pSize);
				break;
			}
			if(family == 1 && type == 0x13) // SRV_MOTD
			{
				log->WriteLine("������� ����� SRV_MOTD: ����� ��������������.");
				break;
			}
			if(family == 1 && type == 0x18) // SRV_FAMILIES_VERSIONS
			{
				loginStage ++;
				log->WriteLine("������� ����� SRV_FAMILIES_VERSIONS, �� ��� �� ���� �����.");
				// �� ���� ������ ������ ����� ���������� CLI_RATES_REQUEST, ��� �� � �������
				pSize = makeSnac(packet,1,6,0,0);
				sendFlap(packet,pSize);
				// ������ ���� �����!
				break;
			}
			if(family == 2 && type == 3)
			{
				loginStage ++;
				log->WriteLine("������� ����� SNAC(02,03): SRV_LOCATION_RIGHTS_REPLY. �������� maxProfileLen � maxCapabilities.");
				// �� ���� ������, ����� ��� ��� ������...
				int offset = getTlv(packet,pSize,01,16) + 4;
				maxProfileLen = packet[offset] * 256 + packet[offset + 1];
				offset = getTlv(packet,pSize,0x02,16);
				maxCapabilities = packet[offset] * 256 + packet[offset + 1];
				log->WriteLine("maxProfileLen = " + maxProfileLen + ", maxCapabilities = " + maxCapabilities + ".");
				// � ���������� ������������ ������� ������
				// ��� ����� ��������� capabilities ������� ��� ������ ������ SNAC(02,04): CLI_SET_LOCATION_INFO
				pSize = makeSnac(packet,0x02,0x04,0x00,0x00);
				pSize = addTlv(packet,0x05,capabilities,pSize);
				sendFlap(packet,pSize);
				// ������ ����� ��������� ����������� �������-�����.
				// ��� �������� ������� SNAC(03,02): CLI_BUDDYLIST_RIGHTS_REQ
				pSize = makeSnac(packet,0x03,0x02,0x00,0x00);
				sendFlap(packet,pSize);
				break;
			}
			if(family == 3 && type == 3)
			{
				loginStage ++;
				log->WriteLine("������� ����� SNAC(03,03): SRV_BUDDYLIST_RIGHTS_REPLY. �������� ������������ ������ �������-����� � ������ �������/���������.");
				// ���������������, ��� ��� ��������������� ��������� �� 2 �����, ���� � ������� ���� ����� �� �������.
				int offset = getTlv(packet,pSize,01,16) + 4;
				maxCLSize = packet[offset] * 256 + packet[offset + 1];
				offset = getTlv(packet,pSize,0x02,16);
				maxWLSize = packet[offset] * 256 + packet[offset + 1];
				log->WriteLine("maxCLSize = " + maxCLSize + ", maxWLSize = " + maxWLSize + ".");
				// � ���������� ������������ ������� ������
				// ��� ����� ��������� ICBM-���������
				pSize = makeSnac(packet,0x04,0x04,0x00,0x00);
				sendFlap(packet,pSize);
				break;
			}
			if(family = 0x04 && type == 0x05) // SNAC(04,05): SRV_ICBM_PARAMS
			{
				loginStage ++;
				log->WriteLine("������� ����� SNAC(04,05): SRV_ICBM_PARAMS, �� � ������ ������ �� ������������.");
				// ���, �� ����, ����� ��������� SNAC(04,02) ��� ��������� ���� ����������, �� ��� ���� ���������� ��������� �� ���������
				loginStage ++;
				// SNAC(09,02)/SNAC(09,03) ��� PRM (����. ������ ������-��������-����� (� ��� ����� ����� watcher list?)
				loginStage ++;
				// TODO: SNAC(13,02/03/05/0f) ���� ��������� ����������, ��� ����� �� ����� ���� ��� (��� �����-�� ������� ������ �� ������ � �������-������).
				loginStage ++;
				// �������� �����, ������������ SSI
				pSize = makeSnac(packet,0x13,0x07,0,0);
				loginStage ++;
				sendFlap(packet,pSize);
				pSize = createCLI_SETxSTATUS(packet); // ���������� ���� � �������
				loginStage ++;
				sendFlap(packet,pSize);
				// � ���������� CLI_READY!
				// �����, ���� ��������� ����� ��� �� �������
				pSize = emptyFlap(packet,0x02);
				array<Byte>^ data = gcnew array<Byte>
					{0x00, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
					 0x00, 0x01, 0x00, 0x03, 0x01, 0x10, 0x04, 0x7B, 0x00, 0x13, 0x00, 0x02, 0x01, 0x10, 0x04, 0x7B,
					 0x00, 0x02, 0x00, 0x01, 0x01, 0x01, 0x04, 0x7B, 0x00, 0x03, 0x00, 0x01, 0x01, 0x10, 0x04, 0x7B,
					 0x00, 0x15, 0x00, 0x01, 0x01, 0x10, 0x04, 0x7B, 0x00, 0x04, 0x00, 0x01, 0x01, 0x10, 0x04, 0x7B,
					 0x00, 0x06, 0x00, 0x01, 0x01, 0x10, 0x04, 0x7B, 0x00, 0x09, 0x00, 0x01, 0x01, 0x10, 0x04, 0x7B,
					 0x00, 0x0A, 0x00, 0x01, 0x01, 0x10, 0x04, 0x7B, 0x00, 0x0B, 0x00, 0x01, 0x01, 0x10, 0x04, 0x7B};
				// �������, �� ����������� �_�
				pSize = addData(packet,data,pSize);
				loginStage ++;
				sendFlap(packet,pSize);
				loginStatus = LS_CONNECTED;
				break;
			}
		}
	case 4: // Close connection negotiation
		// SRV_COOKIE
		if(getTlv(packet,pSize,1,6) != 0 && getTlv(packet,pSize,5,6) != 0 && getTlv(packet,pSize,6,6) != 0 && loginStatus == LS_LOGIN)
		{
			loginStage ++;
			log->WriteLine("������� ����� SRV_COOKIE: ������� cookie � ����� �������.");
			log->WriteLine("��������� IP-������ � ����� �������...");
			// ������ IP-����� ������� � ����
			int offset = getTlv(packet,pSize,5,6); // ������ offset �������� ������ ������� ����� TLV, ����������� ����� �������
			int adrSize = packet[offset + 2] * 256 + packet[offset + 3];
			array<Byte>^ ip = gcnew array<Byte>(4);
			int number = 0; // ����� ����� ip ������ ����� <0 - 3 - ip, 4 - ����.>
			int port = 0;
			for(int i = 0; i < adrSize; )
			{
				if(number > 3)
				{
					port = port * 10 + packet[offset + 4 + i] - 0x30;
				}
				else
				{
					if(packet[offset + 4 + i] != '.' && packet[offset + 4 + i] != ':')
					{
						ip[number] = ip[number] * 10 + packet[offset + 4 + i] - 0x30;
					}
					else
					{
						number += 1;
					}
				}
				i += 1;
			}
			log->Write("�� ������ \"");
			for(int i = 0; i < adrSize; i++)
			{
				log->Write(packet[offset + 4 + i]);
			}
			log->WriteLine("\" ������� �����: " + ip[0] + "." + ip[1] + "." + ip[2] + "." + ip[3] + ":" + port + ".");
			// ������, ��� ������������ � �������, ����� ��� ��������� cookie.
			offset = getTlv(packet,pSize,6,6); // Cookie �������� � TLV 6
			int cookieSize = packet[offset + 2] * 256 + packet[offset + 3];
			if (cookieSize != 256)
				log->WriteLine("[!] ������ cookie (" + cookieSize + ") �� ������������� ���������� (256). ��������� ����������...");
			cookie = gcnew array<Byte>(cookieSize);
			for(int i = 0; i < cookieSize; i++)
				cookie[i] = packet[offset+4+i];
			log->Write("���������� cookie [" + cookie->Length + " ����]:");
			for(int i = 0; i < cookie->Length; i++)
			{
				log->Write(" " + hexNumber(cookie[i]));
			}
			log->WriteLine();
			// ������������ � ������ �������
			loginStatus = LS_REALSERVER;
			connectedToServer = false;
			loginClient->Client->Disconnect(true);
			serverClient = gcnew TcpClient();
			serverClient->Connect(gcnew System::Net::IPAddress(ip),port);
			server = serverClient->GetStream();
			connectedToServer = true; // �����, ����� ����� ��� ���������� :(
			break;
		}
		// AUTH_FAILED
		// ���� �����, ���� �� ������������, ������ ����� ��������� TLV 00 0C � ����������� ����������, ������ �� �������� ����� �� ����������.
		if(getTlv(packet,pSize,1,6) != 0 && getTlv(packet,pSize,4,6) != 0 && getTlv(packet,pSize,8,6) != 0)
		{
			loginStage ++;
			log->WriteLine("������� ����� AUTH_FAILED: ����������� �� �������.");
			log->WriteLine("��� ������: " + hexNumber(packet[getTlv(packet,pSize,8,6) + 4]) + " " + hexNumber(packet[getTlv(packet,pSize,8,6) + 5]));
			MessageBox::Show("����������� �� �������: ������ " + hexNumber(packet[getTlv(packet,pSize,8,6) + 4]) + " " + hexNumber(packet[getTlv(packet,pSize,8,6) + 5]),"������ �����������",MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
			loginStatus = LS_DISCONNECTED;
			break;
		}
	default:
		log->WriteLine("[!] �� ���� ���������� ��� ������. ����� �����������.");
	}
}

System::Void CommonClass::mHandling()
// ������ ������� �������� � ������ � ������������ ��� ���������� � �������� ���������.
// �����������, �����������, ���� ��� ��� ������ ������� �����������.
{
	// ����������
	array<Byte>^ packet = gcnew array<Byte>(PacketSize);
	// ���������� � ������
	while(server->Read(packet,0,6)) // �������� ��������� ������������ ������
	{
		log->Write("������� ��������� FLAP-������:");
		for(int i = 0; i < 6; i++)
		{
			log->Write(" " + hexNumber(packet[i]));
		}
		log->WriteLine();
		if(flapHeaderOk(packet))
		{
			int pSize = packet[4] * 256 + packet[5];
			server->Read(packet,6,pSize);
			// ����� ��������, ������� ��� � ��� � ����� ����������� ������, �.�. ������� ��� ������������ �������.
			log->Write("�������� ���������� FLAP-������:");
			for(int i = 6; i < pSize + 6; i++)
			{
				log->Write(" " + hexNumber(packet[i]));
			}
			log->WriteLine();
			determine(packet);
		}
	}
}

// === ������� �������� � �������� ������� ===

System::Void CommonClass::sendFlap(array<Byte>^ packet, int size)
// ������� ���������� ����� �� ������� ������������ ������ � �������� ���� ����� � ���.
{
	// ������ � rate-limits
	// ������� ���������, SNAC �� ����� ����� ����
	int channel = packet[1]; // SNAC ������ �� ���� ������ �� ������� ������!
	if(channel == 0x02 && classCount != 0)
	{
		// ������� SNAC family � type
		int family = packet[6] * 256 + packet[7];
		int type = packet[8] * 256 + packet[9];
		int group = -1; // ������� ������
		// ������ �������, ����� ������ rate-limits ������������� ������ �����
		int maxCount = snacPairs->Length / classCount; // ������ ������������ ����� ��� � ������ ������ limits
		for(int i = 0; i < classCount; i ++)
		{
			for(int j = 0; j < maxCount; j ++)
			{
				int tFamily = snacPairs[i,j] / 0x10000;
				int tType = snacPairs[i,j] - tFamily * 0x10000;
				if (family == tFamily && type == tType)
				{
					group = i;
					// ��������� ��� ����� (���������?)
					break;
					break;
				}
			}
		}
		DateTime^ time = DateTime::Now;
		if(lastTime[group] = 0)
			current[group] = (windowSize[group] - 1)/windowSize[group] * current[group];		
		else
			current[group] = (windowSize[group] - 1)/windowSize[group] * current[group] + 1/windowSize[group] * (lastTime[group] - time->Ticks * 10^6);
		lastTime[group] = time->Ticks * 10 ^ 6;
		// TODO: ��� ������, ���� ������� ������� �������� alert ��� limit?
		//		 (��� ������, ���� ������� �������� disconnect, ������� �� ����� ������ :)
	}
	log->Write("�������� �� ������ ���������� ������:");
	for(int i = 0; i < size; i++)
	{
	log->Write(" " + hexNumber(packet[i]));
	}
	log->WriteLine();
	server->Write(packet,0,size);
}

int CommonClass::emptyFlap(array<Byte>^ packet, int channel)
// �������������� ������ FLAP-�����, �.�. ��������� ������ ��������� � 6 ����, ���������� ����� ������ � sequence number.
// ���������� ������ ������� �����, ���������� ������ (�.�. 6), ��� ����� ����� ������� ������ ������.
{
	packet[0] = 0x2A;
	packet[1] = channel;
	seqNum += 1;
	if(seqNum >= SEQNUM_RESET_VALUE)
		seqNum = 0;
	// seqNum ��������, ������� ��� � ����� 2-3
	packet[2] = seqNum / 256;
	packet[3] = seqNum - (int)(seqNum / 256) * 256;
	packet[4] = 0x00; // ����� ��� ������ ������; ������ �� 0 ����
	packet[5] = 0x00;
	return 6;
}
		 
int CommonClass::addData(array<Byte>^ packet, array<Byte>^ dataToAdd, int offset)
// ��������� ������ (�� TLV) � ����� �� ���������. ���������� ������ ������� ���������� �����.
{
	for(int i = 0; i < dataToAdd->Length; i++)
		packet[offset + i] = dataToAdd[i];
	int fullLen = offset - 6 + dataToAdd->Length;
	// ������� ����� ������
	packet[4] = fullLen / 256;
	packet[5] = fullLen - (int)(fullLen / 256) * 256;
	return offset + dataToAdd->Length;
}

int CommonClass::makeSnac(array<Byte>^ packet, int family, int type, int flags, int requestID)
// ������ ������ FLAP-����� � ��������� SNAC-����������.
// ���������� ������ ������
{
	// ��������, �������������� �� ��������� ������ (family) ��������
	bool yes = false;
	for(int i = 0; i < supportedSnacs->Length; i++)
	{
		if(supportedSnacs[i] = family)
		{
			yes = true;
			break;
		}
	}
	if(!yes)
		log->WriteLine("��������� SNAC family (" + family + ") �� �������������� ��������, �� �� ����� ����� ���������.");
	int pSize = emptyFlap(packet,2);
	array<Byte>^ header = gcnew array<Byte>(10);
	// family
	header[0] = family / 256;
	header[1] = family - (int)(family / 256) * 256;
	// type
	header[2] = type / 256;
	header[3] = type - (int)(type / 256) * 256;
	// flags
	header[4] = flags / 0x100;
	header[5] = flags - header[4] * 0x100;
	// requestID
	header[6] = requestID / 0x1000000;
	header[7] = requestID / 0x10000 - header[6] * 0x1000;
	header[8] = requestID / 0x100 - header[7] * 0x100 - header[6] * 0x10000;
	header[9] = requestID - header[8] * 0x100 - header[7] * 0x10000 - header[6] * 0x1000000;
	pSize = addData(packet,header,pSize);
	return pSize;
}

int CommonClass::addTlv(array<Byte>^ packet, int typeOfTlv, array<Byte>^ dataToAdd, int offset)
// ������ ������� ��������� � ��������� FLAP-����� TLV (Type-Length-Value) ���������� ��� ���
// ����������� �������� �� ������. ���������� ������ ������� ���������� ����� ������ (��� ��� �����).
// offset - ������ �����, � ������� ����� �������� ������!
{
	packet[offset] = typeOfTlv / 256;
	packet[offset + 1] = typeOfTlv - (int)(typeOfTlv / 256) * 256;
	packet[offset + 2] = dataToAdd->Length / 256;
	packet[offset + 3] = dataToAdd->Length - (int)(dataToAdd->Length / 256) * 256;
	for(int i = 0; i < dataToAdd->Length; i++)
		packet[offset + 4 + i] = dataToAdd[i];
	int fullLen = offset - 6 + 4 + dataToAdd->Length;
	// ������� ����� ������
	packet[4] = fullLen / 256;
	packet[5] = fullLen - (int)(fullLen / 256) * 256;
	return offset + 4 + dataToAdd->Length;
}

// === ������� �������� ���������� ����� ������� ===

int CommonClass::createCLI_IDENT(array<Byte>^ packet)
// ��������� ����� CLI_IDENT (����� ��� �������� �����-������� ���� � ������).
// ���������� ������ ������.
{
	int pSize = emptyFlap(packet,1); // ���������� ����� �� ������ 1
	// ������� � ����� ���������� ������ 4 �����, ��� TLV
	pSize = addData(packet,protocolNumber,pSize);
	// UIN (TLV 1)
	array<Byte>^ tlv = gcnew array<Byte>(UIN->Length);
	for(int i = 0; i < UIN->Length; i++)
		tlv[i] = UIN[i];
	pSize = addTlv(packet,1,tlv,pSize);
	// Password (TLV 2)
	tlv = gcnew array<Byte>(password->Length);
	array<Byte>^ roasting = gcnew array<Byte>{0xF3, 0x26, 0x81, 0xC4, 0x39, 0x86, 0xDB, 0x92, 0x71, 0xA3, 0xB9, 0xE6, 0x53, 0x7A, 0x95, 0x7C};
	for(int i = 0; i < password->Length; i++)
		tlv[i] = password[i] ^ roasting[i];
	pSize = addTlv(packet,2,tlv,pSize);
	// Client ID string (TLV 3)
	tlv = gcnew array<Byte>(clientID->Length);
	for(int i = 0; i < clientID->Length; i++)
		tlv[i] = clientID[i];
	pSize = addTlv(packet,3,tlv,pSize);
	// Client ID (TLV 0x16)
	// TODO: ��� ���� ����� �������� ����� ��������
	tlv = gcnew array<Byte>{0x00, 0x01};
	pSize = addTlv(packet,0x16,tlv,pSize);
	// Client major version (TLV 0x17)
	tlv = gcnew array<Byte>{0x00,0x00};
	pSize = addTlv(packet,0x17,tlv,pSize);
	// Client minor version (TLV 0x18)
	tlv = gcnew array<Byte>{0x00, 0x01};
	pSize = addTlv(packet,0x18,tlv,pSize);
	// Client lesser version (TLV 0x19)
	tlv = gcnew array<Byte>{0x00,0x00};
	pSize = addTlv(packet,0x19,tlv,pSize);
	// �lient build number (TLV 0x1A)
	tlv = gcnew array<Byte>{0x00,0x02};
	pSize = addTlv(packet,0x1A,tlv,pSize);
	// Distribution number (TLV 0x14)
	tlv = gcnew array<Byte>{0x00,0x00,0x00,0x01};
	pSize = addTlv(packet,0x14,tlv,pSize);
	// Client language (TLV 0x0F)
	// ������ ��� 2 ���� ��� � ��������
	tlv = gcnew array<Byte>{'e','n'};
	pSize = addTlv(packet,0x0F,tlv,pSize);
	// Client country (TLV 0x0E)
	tlv = gcnew array<Byte>{'u','s'};
	pSize = addTlv(packet,0x0E,tlv,pSize);
	return pSize;
}

int CommonClass::createCLI_COOKIE(array<Byte>^ packet)
// ��������� ����� CLI_COOKIE (����� ��� �������� ������� �� �����-�������).
// ���������� ������ ������.
{
	int pSize = emptyFlap(packet,1); // ���������� ����� �� ������ 1
	// ������� � ����� ���������� ������ 4 �����, ��� TLV
	pSize = addData(packet,protocolNumber,pSize);
	// Cookie (TLV 6)
	pSize = addTlv(packet,6,cookie,pSize);
	// ������ ������ cookie ��� �� �����, ��� ��� ����� ���
	cookie = nullptr;
	return pSize;
}

int CommonClass::createCLI_SETxSTATUS(array<Byte>^ packet)
// ��������� ����� CLI_SETxSTATUS: ������, �������� � ������ ����.
// ���������� ������ ������.
{
	int pSize = makeSnac(packet,0x01,0x1E,0x00,0x00);
	array<Byte>^ tlv = gcnew array<Byte>{0,0,0,0}; // ��������; ������ "������" ��� ������ �����.
	pSize = addTlv(packet,0x06,tlv,pSize);
	// ����� ����� ����������� ��� TLV, �� �� � ���� ���
	return pSize;
}

int CommonClass::createCLI_SEND_ICBM_CH1(array<Byte>^ packet, String^ toWho, array<Byte>^ messageToSend, bool sendOffline)
{
	int pSize = makeSnac(packet,0x04,0x06,0,0);
	array<Byte>^ data = gcnew array<Byte>{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
										0x00,0x01,			// channel
										toWho->Length}; 
	pSize = addData(packet,data,pSize);
	data = gcnew array<Byte>(toWho->Length);
	for(int i = 0; i < toWho->Length; i++)
		data[i] = toWho[i];
	pSize = addData(packet,data,pSize);
	/*array<Byte>^ tlv = gcnew array<Byte>(messageToSend->Length + 8);
	int uin = 0;
	for(int i = 0; i < UIN->Length; i++)
		uin = uin * 10 + UIN[i] - 30;
	tlv[3] = uin / 0x1000000;
	tlv[2] = uin / 0x10000 - data[3] * 0x100;
	tlv[1] = uin / 0x100 - data[2] * 0x100 - data[3] * 0x10000;
	tlv[0] = uin - data[1] * 0x100 - data[2] * 0x10000 - data[3] * 0x1000000;
	tlv[4] = 0x01; // Plain text message (TYPE)
	tlv[5] = 0x01; // Normal message (FLAG)
	tlv[7] = messageToSend->Length / 0x100;
	tlv[6] = messageToSend->Length - tlv[7] * 0x100;
	int offset = 8;
	for(int i = 0; i < messageToSend->Length; i ++)
		tlv[offset + i] = messageToSend[i];
	pSize = addTlv(packet,5,tlv,pSize);*/
	array<Byte>^ tlv = gcnew array<Byte>(messageToSend->Length + 14);
	tlv[0] = 0x05;
	tlv[1] = 0x01;
	tlv[2] = 0x00;
	tlv[3] = 0x01;
	tlv[4] = 0x01;
	// ��� ��� fragment identifier (array of required capabilities)
	tlv[5] = 0x01;
	tlv[6] = 0x01;
	// ����� ����������� ���������
	tlv[7] = (messageToSend->Length + 4) / 0x100;
	tlv[8] = (messageToSend->Length + 4) - tlv[7] * 0x100;
	// Charset number, lang. number
	tlv[9] = 0x00;
	tlv[10] = 0x00;
	tlv[11] = 0xFF;
	tlv[12] = 0xFF;
	int offset = 13;
	for(int i = 0; i < messageToSend->Length; i ++)
		tlv[offset + i] = messageToSend[i];
	// �� ����, ������ �� ������ ���� NULL-terminated, �� �������, ��� ���� �� ����� �� �����... �� ������� ��� ��� ������.
	pSize = addTlv(packet,2,tlv,pSize);
	if(sendOffline)
	{
		data = gcnew array<Byte>{0x00,0x06,0x00,0x00};
		pSize = addData(packet,data,pSize);
	}
	return pSize;
}