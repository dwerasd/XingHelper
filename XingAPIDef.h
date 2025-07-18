#pragma once


#include <Windows.h>

//------------------------------------------------------------------------------
// 메시지 정의
// 메시지의 ID값은 Connect시에 설정한 nStartMsgID와 더하여 사용하면 된다.
constexpr UINT XM_DEFAULT = WM_USER;							// 기본 메시지 ID
constexpr UINT XM_DISCONNECT = (XM_DEFAULT + 1);				// 서버와의 연결이 끊어졌을 경우 발생
constexpr UINT XM_RECEIVE_DATA = (XM_DEFAULT + 3);				// RequestData로 요청한 데이터가 서버로부터 받았을 때 발생
constexpr UINT XM_RECEIVE_REAL_DATA = (XM_DEFAULT + 4);			// AdviseData로 요청한 데이터가 서버로부터 받았을 때 발생
constexpr UINT XM_LOGIN = (XM_DEFAULT + 5);						// 서버로부터 로그인 결과 받았을때 발생
constexpr UINT XM_LOGOUT = (XM_DEFAULT + 6);					// 서버로부터 로그아웃 결과 받았을때 발생
constexpr UINT XM_TIMEOUT_DATA = (XM_DEFAULT + 7);				// RequestData로 요청한 데이터가 Timeout 이 발생했을때
constexpr UINT XM_RECEIVE_LINK_DATA = (XM_DEFAULT + 8);			// HTS 에서 연동 데이터가 발생했을 때	: by zzin 2013.11.11
constexpr UINT XM_RECEIVE_REAL_DATA_CHART = (XM_DEFAULT + 10);	// 실시간 자동 등록한 후 차트 조회 시, 지표실시간 데이터를 받았을 때  : by zzin 2013.08.14
constexpr UINT XM_RECEIVE_REAL_DATA_SEARCH = (XM_DEFAULT + 11);	// 종목검색 실시간 데이터를 받았을 때 			: by 2017.11.24 LSW

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Receive Flag
constexpr UINT_PTR REQUEST_DATA = 1;			// RequestData로 요청한 데이터가 서버로부터 받았을 때
constexpr UINT_PTR MESSAGE_DATA = 2;			// AdviseData로 요청한 데이터가 서버로부터 받았을 때
constexpr UINT_PTR SYSTEM_ERROR_DATA = 3;		// 서버로부터 시스템 에러 메시지를 받았을 때
constexpr UINT_PTR RELEASE_DATA = 4;			// RequestData로 요청한 데이터가 해제되었을 때
constexpr UINT_PTR LINK_DATA = 10;				// HTS 에서 연동 데이터가 발생했을 때	: by zzin 2013.11.11
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Structure 정의
#pragma pack( push, 1 )

// 조회TR 수신 Packet
typedef struct _RECV_PACKET
{
	int					nRqID{ 0 };						// Request ID
	int					nDataLength{ 0 };				// 받은 데이터 크기
	int					nTotalDataBufferSize{ 0 };		// lpData에 할당된 크기
	int					nElapsedTime{ 0 };				// 전송에서 수신까지 걸린시간(1/1000초)
	int					nDataMode{ 0 };					// 1:BLOCK MODE, 2:NON-BLOCK MODE
	char				szTrCode[10 + 1] = { 0 };			// AP Code
	char				cCont[1] = { 0 };					// '0' : 다음조회 없음, '1' : 다음조회 있음
	char				szContKey[18 + 1] = { 0 };			// 연속키, Data Header가 B 인 경우에만 사용
	char				szUserData[30 + 1] = { 0 };		// 사용자 데이터
	char				szBlockName[17] = { 0 };			// Block 명, Block Mode 일때 사용
	unsigned char* lpData{ nullptr };
} RECV_PACKET, * LPRECV_PACKET;

// 메시지 수신 Packet
typedef struct _MSG_PACKET
{
	int					nRqID{ 0 };						// Request ID
	int					nIsSystemError{ 0 };			// 0:일반메시지, 1:System Error 메시지
	char				szMsgCode[5 + 1] = { 0 };			// 메시지 코드
	int					nMsgLength{ 0 };				// Message 길이
	unsigned char* lpszMessageData{ nullptr };			// Message Data
} MSG_PACKET, * LPMSG_PACKET;

// 실시간TR 수신 Packet
typedef struct _REAL_RECV_PACKET
{
	char				szTrCode[3 + 1] = { 0 };

	int					nKeyLength{ 0 };
	char				szKeyData[32 + 1] = { 0 };
	char				szRegKey[32 + 1] = { 0 };

	int					nDataLength{ 0 };
	char* pszData{ nullptr };
} RECV_REAL_PACKET, * LPRECV_REAL_PACKET;


// HTS에서 API로 연동되어 수신되는 Packet
typedef struct _LINKDATA_RECV_MSG
{
	char				sLinkName[32] = { 0 };
	char				sLinkData[32] = { 0 };
	char				sFiller[64] = { 0 };
}LINKDATA_RECV_MSG, * LPLINKDATA_RECV_MSG;
//------------------------------------------------------------------------------

typedef BOOL(__stdcall* FP_CONNECT) (HWND, LPCSTR, int, int, int, int);
typedef BOOL(__stdcall* FP_ISCONNECTED) ();
typedef BOOL(__stdcall* FP_DISCONNECT) ();
typedef BOOL(__stdcall* FP_LOGIN) (HWND, LPCSTR, LPCSTR, LPCSTR, int, BOOL);
typedef BOOL(__stdcall* FP_LOGOUT) (HWND);
typedef int(__stdcall* FP_GETLASTERROR) ();
typedef int(__stdcall* FP_GETERRORMESSAGE) (int, LPSTR, int);

typedef int(__stdcall* FP_REQUEST) (HWND, LPCSTR, LPVOID, int, BOOL, LPCSTR, int);
typedef void(__stdcall* FP_RELEASEREQUESTDATA) (int);
typedef void(__stdcall* FP_RELEASEMESSAGEDATA) (LPARAM);

typedef BOOL(__stdcall* FP_ADVISEREALDATA) (HWND, LPCSTR, LPCSTR, int);
typedef BOOL(__stdcall* FP_UNADVISEREALDATA) (HWND, LPCSTR, LPCSTR, int);
typedef BOOL(__stdcall* FP_UNADVISEWINDOW) (HWND);

typedef int(__stdcall* FP_GETACCOUNTLISTCOUNT) ();
typedef BOOL(__stdcall* FP_GETACCOUNTLIST) (int, LPSTR, int);
typedef BOOL(__stdcall* FP_GETACCOUNTNAME) (LPCSTR, LPSTR, int);
typedef BOOL(__stdcall* FP_GETACCTDETAILNAME) (LPCSTR, LPSTR, int);
typedef BOOL(__stdcall* FP_GETACCTNICKNAME) (LPCSTR, LPSTR, int);

typedef void(__stdcall* FP_GETCOMMMEDIA) (LPSTR);
typedef void(__stdcall* FP_GETETKMEDIA) (LPSTR);
typedef void(__stdcall* FP_GETCLIENTIP) (LPSTR);
typedef void(__stdcall* FP_GETSERVERNAME) (LPSTR);
typedef void(__stdcall* FP_GETAPIPATH) (LPSTR);

typedef void(__stdcall* FP_SETHEADERINFO) (LPCSTR, LPCSTR);
typedef void(__stdcall* FP_SETUSEAPIVER) (LPCSTR);
typedef void(__stdcall* FP_SETMODE) (LPCSTR, LPCSTR);

typedef void(__stdcall* FP_GETPROCBRANCHNO) (LPSTR);
typedef BOOL(__stdcall* FP_GETUSEOVERFUTURE) ();
typedef BOOL(__stdcall* FP_GETUSEOVERSTOCK) ();
typedef BOOL(__stdcall* FP_GETUSEFX) ();

typedef int(__stdcall* FP_GETTRCOUNTPERSEC) (LPCSTR);
typedef int(__stdcall* FP_GETTRCOUNTBASESEC) (LPCSTR);
typedef int(__stdcall* FP_GETTRCOUNTREQUEST) (LPCSTR);
typedef int(__stdcall* FP_GETTRCOUNTLIMIT) (LPCSTR);

typedef void(__stdcall* FP_SETNOTIFYFLAG) (BOOL);

typedef int(__stdcall* FP_REQUESTSERVICE) (HWND, LPCSTR, LPCSTR);
typedef int(__stdcall* FP_REMOVESERVICE) (HWND, LPCSTR, LPCSTR);

typedef int(__stdcall* FP_REQUESTLINKTOHTS) (HWND, LPCSTR, LPCSTR, LPCSTR);
typedef void(__stdcall* FP_ADVISELINKFROMHTS) (HWND);
typedef void(__stdcall* FP_UNADVISELINKFROMHTS) (HWND);

typedef int(__stdcall* FP_DECOMPRESS) (LPCSTR, LPCSTR, int);
typedef BOOL(__stdcall* FP_ISCHARTLIB) ();

typedef void(__stdcall* FP_SETPROGRAMORDER) (BOOL);
typedef BOOL(__stdcall* FP_GETPROGRAMORDER) ();
