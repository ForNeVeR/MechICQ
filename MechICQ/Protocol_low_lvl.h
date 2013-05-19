#pragma once

//#include "Form1.h"
//#include "ContactList.h"
//#include "CommonClass.cpp"

//#include "MHeader.h"
// Для loginStatus
#define LS_DISCONNECTED 0
#define LS_LOGIN 1
#define LS_REALSERVER 2
#define LS_CONNECTED 3


using namespace System;
using namespace System::Net::Sockets;
using namespace System::IO;
using namespace System::Threading;

//using namespace ICQ_client;

public ref class CommonClass
{
	// Конструктор
	public: CommonClass();

	// === Протокольная часть ===

	// Константы
	static String^ clientID = "Mechanicum 0.1";
	static String^ loginServerName = "login.icq.com";
	static TcpClient^ loginClient;
	static TcpClient^ serverClient;
	static int loginServerPort = 5190;
	static int PacketSize = 65542; // 65542 = 64 кб (пакет) + 6 байт (FLAP-заголовок).
	static int SEQNUM_RESET_VALUE = 0xFFFF; // 0x8000 ?
	static array<Byte>^ protocolNumber = gcnew array<Byte>{0x00, 0x00, 0x00, 0x01}; // Номер протокола? Везде в примерах 1, так что не станем испытывать судьбу
	static array<Byte>^ capabilities = gcnew array<Byte>
		{0x09, 0x46, 0x13, 0x49, 0x4C, 0x7F, 0x11, 0xD1, 0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}; // Client supports channel 2 extended, TLV(0x2711) based messages.
		 //0x09, 0x46, 0x13, 0x4E, 0x4C, 0x7F, 0x11, 0xD1, 0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}; // UTF-8
	static int TotalLoginStages = 17; // Пара переменных для показа процентов логина

	// Переменные
	// Структура rate-limits
	static int classCount;
	static array<int>^ rID;
	static array<int>^ windowSize;
	static array<int>^ clear;
	static array<int>^ alert;
	static array<int>^ limit;
	static array<int>^ disconnect;
	static array<int>^ current;
	static array<int>^ max;
	static array<int,2>^ snacPairs;
	static array<long long>^ lastTime; // Время последней отправки пакета

	static int loginStage = 0;
	static String^ UIN;
	static String^ password;
	static NetworkStream^ server;
	static StreamWriter^ log;
	static int seqNum; // Наш sequence number, должен заливаться во все исходящие пакеты
	static int serverSeqNum; // SeqNum пакетов, присылаемых сервером; нужен для контроля, не потеряли ли мы чего-либо
	static bool connectedToServer; // Если connected (т.е. получили от сервера SRV_HELLO), то следим за serverSeqNum
	static int loginStatus;
	static array<Byte>^ cookie;
	static array<int>^ supportedSnacs;
	static int maxProfileLen;
	static int maxCapabilities;
	static int maxCLSize;
	static int maxWLSize; // WL = watcher list
	static Thread^ mHandler;

	// Функции
	public: static String^ hexNumber(Byte byte);
	public: static bool flapHeaderOk(array<Byte>^ header);
	public: static int getTlv(array<Byte>^ packet, int pSize, int typeOfTlv, int offset);
	public: static System::Void determine (array<Byte>^ packet);
	public: static System::Void mHandling();
	public: static System::Void sendFlap(array<Byte>^ packet, int size);
	public: static int emptyFlap(array<Byte>^ packet, int channel);
	public: static int addData(array<Byte>^ packet, array<Byte>^ dataToAdd, int offset);
	public: static int makeSnac(array<Byte>^ packet, int family, int type, int flags, int requestID);
	public: static int addTlv(array<Byte>^ packet, int typeOfTlv, array<Byte>^ dataToAdd, int offset);
	public: static int createCLI_IDENT(array<Byte>^ packet);
	public: static int createCLI_COOKIE(array<Byte>^ packet);
	public: static int createCLI_SETxSTATUS(array<Byte>^ packet);
	public: static int createCLI_SEND_ICBM_CH1(array<Byte>^ packet, String^ toWho, array<Byte>^ messageToSend, bool sendOffline);

/*	// === Оконная часть ===

	// Константы

	// Переменные
//	public: static Form1^ form1;
//	public: static ContactList^ contactList;

	// Функции
	public: static System::Void updateLoginBar();
	public: static System::Void terminate();*/
};
