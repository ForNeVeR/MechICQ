#pragma once

#include "stdafx.cpp"
#include "CommonClass.h"

using namespace System;
using namespace System::Text;
using namespace System::Windows::Forms;

CommonClass::CommonClass()
{
}

// === Служеюные функции ===

String^ CommonClass::hexNumber(Byte byte)
// Функция выдаёт шестнадцатеричное число в виде строки типа "0A" в отличие от System::Convert::ToString(byte,16) (возвратит "a")
{
	String^ buff = System::Convert::ToString(byte,16);
	buff = buff->ToUpper();
	if (buff->Length < 2)
		buff = "0" + buff;
	return buff;
}

// === Функции обработки поступающих пакетов ===

bool CommonClass::flapHeaderOk(array<Byte>^ header)
// Функция, проверяет FLAP-заголовок пакета. Результат - в порядке ли заголовок. Просматривает
// только первые 2 байта пакета.
{
	if(header[0] == 0x2A && header[1] >= 1 && header[1] <= 5)
	{
		if(connectedToServer)
		{
			// Если мы уже подключены к серверу (т.е. изучаемый пакет - не SRV_HELLO), то необходимо контролировать sequence number'ы и сигналить в лог, если что.
			int wserverSeqNum = serverSeqNum + 1;
			if(wserverSeqNum >= SEQNUM_RESET_VALUE)
				wserverSeqNum = 0; // После этой операции wserverSeqNum - ожидаемый serverSeqNum
			serverSeqNum = header[2] * 256 + header[3];
			if(serverSeqNum != wserverSeqNum)
				log->WriteLine("[!] Полученный sequence number (" + hexNumber(header[2]) +" " + hexNumber(header[3]) + ") не соответствует ожидаемому (" + hexNumber(wserverSeqNum / 256) + " " + hexNumber(wserverSeqNum - (int)(wserverSeqNum / 256) * 256) + "). Вероятна потеря пакетов.");
		}
		return true;
		}
	else
		return false;
}

int CommonClass::getTlv(array<Byte>^ packet, int pSize, int typeOfTlv, int offset)
// Функция выдаёт индекс первого байта запрошенного TLV-пакета или 0, если пакет не найден.
// Для работы нужен размер пакета (ведь переменная-то > 64 кб! Не проверять же её всю!)
// Также задаётся смещение (на случай, если в пакете в начале есть SNAC-заголовки или ещё чего-нибудь.
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
// Данная функция определяет тип поступившего пакета и дальнейшие действия.
{
	// Пакет мы проверили сразу по получении, так что тут никаких проблем с ним возникнуть не должно.
	log->WriteLine("Обработка пакета...");
	// Определим канал, по которому пришёл пакет...
	int channel = packet[1];
	log->WriteLine("Пакет пришёл по каналу " + channel + ".");
	serverSeqNum = packet[2] * 256 + packet[3];
	log->WriteLine("Sequence number: " + hexNumber(packet[2]) + " " + hexNumber(packet[3]));
	int pSize = packet[4] * 256 + packet[5] + 6;
	log->WriteLine("Длина пакета: " + (pSize - 6) + " (+ 6 байт заголовок).");
	switch(channel)
	{
	case 1: // New connection negitiation
		// SRV_HELLO
		if(packet[4] == 0 && packet[5] == 4 && packet[6] == 0 && packet[7] == 0 && packet[8] == 0 && packet[9] == 1)
		{
			connectedToServer = true;
			log->WriteLine("Получен пакет SRV_HELLO: подключение к серверу установлено.");
			loginStage ++;
			// Теперь, если мы подключены к логин-серверу, нужно отослать ему CLI_IDENT вместе с логином, паролем и прочей фигнёй.
			if(loginStatus == LS_LOGIN)
			{
				// В принципе, packet мы уже обработали и он нам не нужен, так что можно использовать его.
				// Поскольку мы только что подключились к серверу, о чём свидетельствует SRV_HELLO, нужно сформировать новый seqNum
				System::Random^ rand = gcnew System::Random;
				seqNum = rand->Next(SEQNUM_RESET_VALUE); // Не знаю, может ли в этом случае seqNum стать == 0x8000, однако это всё равно будет отловлено при создании пакета
				pSize = createCLI_IDENT(packet); // packet заполняется нужными данными и готов к отправке
				sendFlap(packet,pSize);
			}
			// А если к реальному серверу, ему нужно отослать CLI_COOKIE
			if(loginStatus == LS_REALSERVER)
			{
				System::Random^ rand = gcnew System::Random;
				seqNum = rand->Next(SEQNUM_RESET_VALUE); // Не знаю, может ли в этом случае seqNum стать == 0x8000, однако это всё равно будет отловлено при создании пакета
				pSize = createCLI_COOKIE(packet); // packet заполняется нужными данными и готов к отправке
				sendFlap(packet,pSize);				
			}
			break;
		}
	case 2: // SNAC data
		{
			// Определим family и type
			int family = packet[6] * 256 + packet[7];
			int type = packet[8] * 256 + packet[9];
			int flags = packet[10] * 256 + packet[11];
			int requestID = packet[12] * 0x1000000 + packet[13] * 0x10000 + packet[14] * 0x100 + packet[15];
			log->WriteLine("Получен SNAC-пакет: family = " + family + ", type = " + type + ", flags = " + hexNumber(packet[10]) + " " + hexNumber(packet[11]) + ", request ID: " + hexNumber(packet[12]) + " " + hexNumber(packet[13]) + " " + hexNumber(packet[14]) + " " + hexNumber(packet[15]));
			if(family == 1 && type == 3) // SRV_FAMILIES
			{
				loginStage ++;
				// Этот пакет содержит список поддерживаемых сервисов, так что их надо запомнить.
				int offset = 16;
				supportedSnacs = gcnew array<int>((pSize - 16) / 2);
				for(int i = 0; i < (pSize - 16) / 2; i++)
				{
					supportedSnacs[i] = packet[offset + i * 2] * 256 + packet[offset + i * 2 + 1];
				}
				log->WriteLine("Получен SNAC(1,3): SRV_FAMILIES, содержит список поддерживаемых сервисов.");
				log->Write("Список:");
				for(int i = 0; i < supportedSnacs->Length; i++)
					log->Write(" " + hexNumber((supportedSnacs[i] / 256)) + hexNumber((supportedSnacs[i] - (int)(supportedSnacs[i] / 256) * 256)));
				log->WriteLine();
				// Теперь нужно отправить пакет с нужными сервисами и их версиями
				pSize = makeSnac(packet,1,0x17,0,1); // Создаём пустой SNAC пакет с указанными параметрами
				array<Byte>^ data = gcnew array<Byte>(supportedSnacs->Length * 4);
				// Следующий цикл задаёт необходимую версию для каждого поддерживаемого сервиса равной 1
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
				log->WriteLine("Получен пакет SNAC(1,7): SRV_RATE_LIMIT_INFO, заполняем информацию о rate-limits.");
				classCount = packet[16] * 256 + packet[17];
				// Считываем основную инфу
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
				// Теперь считываем группы SNAC-пакетов, относящиеся к тому или иному лимиту.
				// Пары family-type SNAC-пакетов заносятся в массив как family * 0x10000 + type
				snacPairs = gcnew array<int,2>(classCount,(pSize - (6 + 2 + 30 * classCount + 4)) / 4); // Это на всякий случай, исходим из худшего, т.е. что вся оставшаяся часть пакета представляет из себя одну группу.
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
				log->WriteLine("Вся информация о rate-limits будет сохранена в файл ratelim.txt");
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
				// Теперь нужно отослать на сервер пакет CLI_RATES_ACK (SNAC(01,08)), содержащий просто список групп rate-limits.
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
				// После того, как этот пакет отправлен, нужно отправить также ещё один небольшой пакетик для начала следующей стадии логина.
				log->WriteLine("Начата стадия логина 3: настройка сервисов.");
				pSize = makeSnac(packet,2,2,0,0); // CLI_LOCATION_RIGHTS_REQ
				loginStage ++;
				sendFlap(packet,pSize);
				break;
			}
			if(family == 1 && type == 0x13) // SRV_MOTD
			{
				log->WriteLine("Получен пакет SRV_MOTD: пакет проигнорирован.");
				break;
			}
			if(family == 1 && type == 0x18) // SRV_FAMILIES_VERSIONS
			{
				loginStage ++;
				log->WriteLine("Получен пакет SRV_FAMILIES_VERSIONS, но нам на него пофиг.");
				// На этой стадии логина можно отправлять CLI_RATES_REQUEST, чем мы и займёмся
				pSize = makeSnac(packet,1,6,0,0);
				sendFlap(packet,pSize);
				// Обожаю этот пакет!
				break;
			}
			if(family == 2 && type == 3)
			{
				loginStage ++;
				log->WriteLine("Получен пакет SNAC(02,03): SRV_LOCATION_RIGHTS_REPLY. Извлечём maxProfileLen и maxCapabilities.");
				// Не знаю правда, зачем нам эти данные...
				int offset = getTlv(packet,pSize,01,16) + 4;
				maxProfileLen = packet[offset] * 256 + packet[offset + 1];
				offset = getTlv(packet,pSize,0x02,16);
				maxCapabilities = packet[offset] * 256 + packet[offset + 1];
				log->WriteLine("maxProfileLen = " + maxProfileLen + ", maxCapabilities = " + maxCapabilities + ".");
				// И продолжаем нескончаемый процесс логина
				// Нам нужно отправить capabilities клиента при помощи пакета SNAC(02,04): CLI_SET_LOCATION_INFO
				pSize = makeSnac(packet,0x02,0x04,0x00,0x00);
				pSize = addTlv(packet,0x05,capabilities,pSize);
				sendFlap(packet,pSize);
				// Заодно нужно запросить ограничения контакт-листа.
				// Это делается пакетом SNAC(03,02): CLI_BUDDYLIST_RIGHTS_REQ
				pSize = makeSnac(packet,0x03,0x02,0x00,0x00);
				sendFlap(packet,pSize);
				break;
			}
			if(family == 3 && type == 3)
			{
				loginStage ++;
				log->WriteLine("Получен пакет SNAC(03,03): SRV_BUDDYLIST_RIGHTS_REPLY. Извлечём максимальный размер контакт-листа и списка видимых/невидимых.");
				// Подразумевается, что оба рассматриваемых параметра по 2 байта, хотя в мануале явно длина не указана.
				int offset = getTlv(packet,pSize,01,16) + 4;
				maxCLSize = packet[offset] * 256 + packet[offset + 1];
				offset = getTlv(packet,pSize,0x02,16);
				maxWLSize = packet[offset] * 256 + packet[offset + 1];
				log->WriteLine("maxCLSize = " + maxCLSize + ", maxWLSize = " + maxWLSize + ".");
				// И продолжаем нескончаемый процесс логина
				// Нам нужно запросить ICBM-параметры
				pSize = makeSnac(packet,0x04,0x04,0x00,0x00);
				sendFlap(packet,pSize);
				break;
			}
			if(family = 0x04 && type == 0x05) // SNAC(04,05): SRV_ICBM_PARAMS
			{
				loginStage ++;
				log->WriteLine("Получен пакет SNAC(04,05): SRV_ICBM_PARAMS, но в данной версии он игнорируется.");
				// Тут, по идее, нужно отправить SNAC(04,02) для настройки этих параметров, но нас типа устраивают параметры по умолчанию
				loginStage ++;
				// SNAC(09,02)/SNAC(09,03) про PRM (макс. размер визибл-инвизибл-листа (а что тогда такое watcher list?)
				loginStage ++;
				// TODO: SNAC(13,02/03/05/0f) тоже попробуем пропустить, ибо нафик не упало пока что (это каким-то образом влияет на работу с контакт-листом).
				loginStage ++;
				// Отправим пакет, инициирующий SSI
				pSize = makeSnac(packet,0x13,0x07,0,0);
				loginStage ++;
				sendFlap(packet,pSize);
				pSize = createCLI_SETxSTATUS(packet); // Отправляем инфу о статусе
				loginStage ++;
				sendFlap(packet,pSize);
				// И отправляем CLI_READY!
				// Ладно, тупо попробуем взять его из примера
				pSize = emptyFlap(packet,0x02);
				array<Byte>^ data = gcnew array<Byte>
					{0x00, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
					 0x00, 0x01, 0x00, 0x03, 0x01, 0x10, 0x04, 0x7B, 0x00, 0x13, 0x00, 0x02, 0x01, 0x10, 0x04, 0x7B,
					 0x00, 0x02, 0x00, 0x01, 0x01, 0x01, 0x04, 0x7B, 0x00, 0x03, 0x00, 0x01, 0x01, 0x10, 0x04, 0x7B,
					 0x00, 0x15, 0x00, 0x01, 0x01, 0x10, 0x04, 0x7B, 0x00, 0x04, 0x00, 0x01, 0x01, 0x10, 0x04, 0x7B,
					 0x00, 0x06, 0x00, 0x01, 0x01, 0x10, 0x04, 0x7B, 0x00, 0x09, 0x00, 0x01, 0x01, 0x10, 0x04, 0x7B,
					 0x00, 0x0A, 0x00, 0x01, 0x01, 0x10, 0x04, 0x7B, 0x00, 0x0B, 0x00, 0x01, 0x01, 0x10, 0x04, 0x7B};
				// Странно, но прокатывает о_О
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
			log->WriteLine("Получен пакет SRV_COOKIE: получен cookie и адрес сервера.");
			log->WriteLine("Получение IP-адреса и порта сервера...");
			// Достаём IP-адрес сервера и порт
			int offset = getTlv(packet,pSize,5,6); // Теперь offset содержит индекс первого байта TLV, содержащего адрес сервера
			int adrSize = packet[offset + 2] * 256 + packet[offset + 3];
			array<Byte>^ ip = gcnew array<Byte>(4);
			int number = 0; // Какую цифру ip сейчас задаём <0 - 3 - ip, 4 - порт.>
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
			log->Write("Из строки \"");
			for(int i = 0; i < adrSize; i++)
			{
				log->Write(packet[offset + 4 + i]);
			}
			log->WriteLine("\" получен адрес: " + ip[0] + "." + ip[1] + "." + ip[2] + "." + ip[3] + ":" + port + ".");
			// Прежде, чем подключаться к серверу, нужно ещё запомнить cookie.
			offset = getTlv(packet,pSize,6,6); // Cookie хранится в TLV 6
			int cookieSize = packet[offset + 2] * 256 + packet[offset + 3];
			if (cookieSize != 256)
				log->WriteLine("[!] Размер cookie (" + cookieSize + ") не соответствует ожидаемому (256). Продолжаю выполнение...");
			cookie = gcnew array<Byte>(cookieSize);
			for(int i = 0; i < cookieSize; i++)
				cookie[i] = packet[offset+4+i];
			log->Write("Сохранённая cookie [" + cookie->Length + " байт]:");
			for(int i = 0; i < cookie->Length; i++)
			{
				log->Write(" " + hexNumber(cookie[i]));
			}
			log->WriteLine();
			// Подключаемся к новому серверу
			loginStatus = LS_REALSERVER;
			connectedToServer = false;
			loginClient->Client->Disconnect(true);
			serverClient = gcnew TcpClient();
			serverClient->Connect(gcnew System::Net::IPAddress(ip),port);
			server = serverClient->GetStream();
			connectedToServer = true; // Забыл, зачем нужна эта переменная :(
			break;
		}
		// AUTH_FAILED
		// Этот пакет, судя по документации, должен также содержать TLV 00 0C с неизвестным содержимым, однако на практике этого не происходит.
		if(getTlv(packet,pSize,1,6) != 0 && getTlv(packet,pSize,4,6) != 0 && getTlv(packet,pSize,8,6) != 0)
		{
			loginStage ++;
			log->WriteLine("Получен пакет AUTH_FAILED: авторизация не удалась.");
			log->WriteLine("Код ошибки: " + hexNumber(packet[getTlv(packet,pSize,8,6) + 4]) + " " + hexNumber(packet[getTlv(packet,pSize,8,6) + 5]));
			MessageBox::Show("Авторизация не удалась: ошибка " + hexNumber(packet[getTlv(packet,pSize,8,6) + 4]) + " " + hexNumber(packet[getTlv(packet,pSize,8,6) + 5]),"Ошибка авторизации",MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
			loginStatus = LS_DISCONNECTED;
			break;
		}
	default:
		log->WriteLine("[!] Не могу определить тип пакета. Пакет игнорирован.");
	}
}

System::Void CommonClass::mHandling()
// Данная функция работает в потоке и обрабатывает все получаемые с серверов сообщения.
// Запускается, естественно, пару раз для каждой попытки подключения.
{
	// Переменные
	array<Byte>^ packet = gcnew array<Byte>(PacketSize);
	// Приступаем к работе
	while(server->Read(packet,0,6)) // Получаем заголовок поступившего пакета
	{
		log->Write("Получен заголовок FLAP-пакета:");
		for(int i = 0; i < 6; i++)
		{
			log->Write(" " + hexNumber(packet[i]));
		}
		log->WriteLine();
		if(flapHeaderOk(packet))
		{
			int pSize = packet[4] * 256 + packet[5];
			server->Read(packet,6,pSize);
			// Пакет загружен, засунем его в лог и будем разбираться дальше, т.е. передаём его определяющей функции.
			log->Write("Получено содержимое FLAP-пакета:");
			for(int i = 6; i < pSize + 6; i++)
			{
				log->Write(" " + hexNumber(packet[i]));
			}
			log->WriteLine();
			determine(packet);
		}
	}
}

// === Функции создания и отправки пакетов ===

System::Void CommonClass::sendFlap(array<Byte>^ packet, int size)
// Функция отправляет пакет на текущий подключенный сервер и заливает этот пакет в лог.
{
	// Работа с rate-limits
	// Сначала определим, SNAC ли пакет перед нами
	int channel = packet[1]; // SNAC пакеты же идут только по второму каналу!
	if(channel == 0x02 && classCount != 0)
	{
		// Выясним SNAC family и type
		int family = packet[6] * 256 + packet[7];
		int type = packet[8] * 256 + packet[9];
		int group = -1; // Искомая группа
		// Теперь находим, какой группе rate-limits соответствует данный пакет
		int maxCount = snacPairs->Length / classCount; // Таково максимальное число пар в каждой группе limits
		for(int i = 0; i < classCount; i ++)
		{
			for(int j = 0; j < maxCount; j ++)
			{
				int tFamily = snacPairs[i,j] / 0x10000;
				int tType = snacPairs[i,j] - tFamily * 0x10000;
				if (family == tFamily && type == tType)
				{
					group = i;
					// Завершаем оба цикла (сработает?)
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
		// TODO: Что делать, если текущий уровень превысил alert или limit?
		//		 (Что делать, если уровень превысил disconnect, выбрать всё равно нельзя :)
	}
	log->Write("Отправка на сервер следующего пакета:");
	for(int i = 0; i < size; i++)
	{
	log->Write(" " + hexNumber(packet[i]));
	}
	log->WriteLine();
	server->Write(packet,0,size);
}

int CommonClass::emptyFlap(array<Byte>^ packet, int channel)
// Инициализирует пустой FLAP-пакет, т.е. добавляет только заголовок в 6 байт, содержащий номер канала и sequence number.
// Возвращает индекс первого байта, подлжащего записи (т.е. 6), что можно также назвать длиной пакета.
{
	packet[0] = 0x2A;
	packet[1] = channel;
	seqNum += 1;
	if(seqNum >= SEQNUM_RESET_VALUE)
		seqNum = 0;
	// seqNum вычислен, запишем его в байты 2-3
	packet[2] = seqNum / 256;
	packet[3] = seqNum - (int)(seqNum / 256) * 256;
	packet[4] = 0x00; // Место под размер данных; сейчас их 0 байт
	packet[5] = 0x00;
	return 6;
}
		 
int CommonClass::addData(array<Byte>^ packet, array<Byte>^ dataToAdd, int offset)
// Добавляет данные (не TLV) в пакет со смещением. Возвращает индекс первого свободного байта.
{
	for(int i = 0; i < dataToAdd->Length; i++)
		packet[offset + i] = dataToAdd[i];
	int fullLen = offset - 6 + dataToAdd->Length;
	// Запишем длину пакета
	packet[4] = fullLen / 256;
	packet[5] = fullLen - (int)(fullLen / 256) * 256;
	return offset + dataToAdd->Length;
}

int CommonClass::makeSnac(array<Byte>^ packet, int family, int type, int flags, int requestID)
// Создаёт пустой FLAP-пакет с указанным SNAC-заголовком.
// Возвращает размер пакета
{
	// Проверка, поддерживается ли указанный сервис (family) сервером
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
		log->WriteLine("Указанный SNAC family (" + family + ") не поддерживается сервером, но всё равно будет отправлен.");
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
// Данная функция добавляет в имеющийся FLAP-пакет TLV (Type-Length-Value) содержимое для его
// последующей отправки на сервер. Возвращает индекс первого свободного байта пакета (или его длину).
// offset - индекс байта, в который нужно начинать писать!
{
	packet[offset] = typeOfTlv / 256;
	packet[offset + 1] = typeOfTlv - (int)(typeOfTlv / 256) * 256;
	packet[offset + 2] = dataToAdd->Length / 256;
	packet[offset + 3] = dataToAdd->Length - (int)(dataToAdd->Length / 256) * 256;
	for(int i = 0; i < dataToAdd->Length; i++)
		packet[offset + 4 + i] = dataToAdd[i];
	int fullLen = offset - 6 + 4 + dataToAdd->Length;
	// Запишем длину пакета
	packet[4] = fullLen / 256;
	packet[5] = fullLen - (int)(fullLen / 256) * 256;
	return offset + 4 + dataToAdd->Length;
}

// === Функции создания КОНКРЕТНЫХ видов пакетов ===

int CommonClass::createCLI_IDENT(array<Byte>^ packet)
// Формирует пакет CLI_IDENT (нужен для передачи логин-серверу уина и пароля).
// Возвращает размер пакета.
{
	int pSize = emptyFlap(packet,1); // Отправлять будем по каналу 1
	// Сначала в пакет заливаются просто 4 байта, без TLV
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
	// TODO: Тут надо будет написать нечто значимое
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
	// Сlient build number (TLV 0x1A)
	tlv = gcnew array<Byte>{0x00,0x02};
	pSize = addTlv(packet,0x1A,tlv,pSize);
	// Distribution number (TLV 0x14)
	tlv = gcnew array<Byte>{0x00,0x00,0x00,0x01};
	pSize = addTlv(packet,0x14,tlv,pSize);
	// Client language (TLV 0x0F)
	// Возьмём эти 2 поля как в примерах
	tlv = gcnew array<Byte>{'e','n'};
	pSize = addTlv(packet,0x0F,tlv,pSize);
	// Client country (TLV 0x0E)
	tlv = gcnew array<Byte>{'u','s'};
	pSize = addTlv(packet,0x0E,tlv,pSize);
	return pSize;
}

int CommonClass::createCLI_COOKIE(array<Byte>^ packet)
// Формирует пакет CLI_COOKIE (нужен для передачи серверу от логин-сервера).
// Возвращает размер пакета.
{
	int pSize = emptyFlap(packet,1); // Отправлять будем по каналу 1
	// Сначала в пакет заливаются просто 4 байта, без TLV
	pSize = addData(packet,protocolNumber,pSize);
	// Cookie (TLV 6)
	pSize = addTlv(packet,6,cookie,pSize);
	// Теперь массив cookie нам не нужен, так что уберём его
	cookie = nullptr;
	return pSize;
}

int CommonClass::createCLI_SETxSTATUS(array<Byte>^ packet)
// Формирует пакет CLI_SETxSTATUS: статус, протокол и прочая инфа.
// Возвращает размер пакета.
{
	int pSize = makeSnac(packet,0x01,0x1E,0x00,0x00);
	array<Byte>^ tlv = gcnew array<Byte>{0,0,0,0}; // Временно; статус "онлайн" без всяких фишег.
	pSize = addTlv(packet,0x06,tlv,pSize);
	// Далее могут последовать ещё TLV, но не в этот раз
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
	// Это был fragment identifier (array of required capabilities)
	tlv[5] = 0x01;
	tlv[6] = 0x01;
	// Длина оставшегося сообщения
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
	// Не знаю, должна ли строка быть NULL-terminated, но надеюсь, что хуже от этого не будет... Да поможет нам Дух Машины.
	pSize = addTlv(packet,2,tlv,pSize);
	if(sendOffline)
	{
		data = gcnew array<Byte>{0x00,0x06,0x00,0x00};
		pSize = addData(packet,data,pSize);
	}
	return pSize;
}