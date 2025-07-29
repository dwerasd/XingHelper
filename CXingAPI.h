#pragma once


#include <DarkCore/DSingleton.h>
#include <DarkCore/DEvent.h>

#include <Packet/t0434.h>			// 선물/옵션 체결/미체결
#include <Packet/t0441.h>			// 선물/옵션 잔고평가(이동평균)

#include <packet/t2101.h>
#include <Packet/t2301.h>
#include <Packet/t8432.h>
#include <Packet/t8433.h>
#include <Packet/t8435.h>

#include <Packet/OC0.h>
#include <Packet/OH0.h>
#include <Packet/O01.h>				// 주문한 선물 접수
#include <packet/C01.h>				// 주문한 선옵 주문 체결
#include <packet/H01.h>				// 주문한 정정 또는 취소


#include <packet/CFOAT00100.h>
#include <packet/CFOAT00200.h>
#include <packet/CFOAT00300.h>
#include <packet/CFOAQ50600.h>
#include <packet/CFOFQ02400.h>
#include <packet/CFOBQ10500.h>

#include "XingAPIDef.h"
#include "XingHelper.h"



enum
{
	_STATUS_NONE_ = 0
	, _STATUS_CONNECTED_
	, _STATUS_REQUEST_LOGIN_
	, _STATUS_FINISH_LOGIN_
};

class IXingAPI
	: public dk::C_SINGLETON<IXingAPI>
{
private:
	static HWND hDefaultWnd;
	BOOL bMockServer{ FALSE };
	HMODULE hXingAPI{ nullptr };

	volatile LONG nStatusXingAPI{ 0 }; // Ensure 'volatile' is used for atomic operations

	xing::VEC_SUBSCRIBE_INFOS vSubscribeInfos; // 구독 정보 리스트
	// key 는 중복 가능, 데이터도 중복 가능, key + 데이터는 중복 불가능.
	inline void reg_sub_info(LPCSTR _pKey, LPCSTR _pData = nullptr, const int _nDataUnitLen = 0)
	{
		vSubscribeInfos.push_back(std::make_pair(_pKey, xing::_SUBSCRIBE_INFO{}));
		auto& pLastRegInfo = vSubscribeInfos.back();
		if (_pData)
		{
			pLastRegInfo.second.szData = _pData;
			pLastRegInfo.second.nDataUnitLen = _nDataUnitLen;
		}
	}

	inline bool is_reg_sub_info(LPCSTR _pKey, LPCSTR _pData) const	// 이미 동일 key와 데이터가 등록되어 있는지 확인
	{
		auto it = std::find_if(vSubscribeInfos.begin(), vSubscribeInfos.end(),
			[_pKey, _pData](const auto& pair) {
				return pair.first == _pKey && pair.second.szData == _pData;
			});
		return it != vSubscribeInfos.end();
	}

	typedef std::unordered_map<std::string, xing::C_TR_TIME_CHECKER> UMAP_TR_TIMER_CHECKER;
	UMAP_TR_TIMER_CHECKER m_trTimers;

	dk::C_EVENT_MANUAL_RESET m_reqEvent;

protected:
	FP_CONNECT					m_fpConnect{ nullptr };
	FP_ISCONNECTED				m_fpIsConnected{ nullptr };
	FP_DISCONNECT				m_fpDisconnect{ nullptr };
	FP_LOGIN					m_fpLogin{ nullptr };
	FP_LOGOUT					m_fpLogout{ nullptr };

	FP_GETLASTERROR				m_fpGetLastError{ nullptr };
	FP_GETERRORMESSAGE			m_fpGetErrorMessage{ nullptr };

	FP_REQUEST					m_fpRequest{ nullptr };
	FP_ADVISEREALDATA			m_fpAdviseRealData{ nullptr };
	FP_UNADVISEREALDATA			m_fpUnadviseRealData{ nullptr };
	FP_UNADVISEWINDOW			m_fpUnadviseWindow{ nullptr };
	FP_RELEASEREQUESTDATA		m_fpReleaseRequestData{ nullptr };
	FP_RELEASEMESSAGEDATA		m_fpReleaseMessageData{ nullptr };

	FP_GETACCOUNTLISTCOUNT		m_fpGetAccountListCount{ nullptr };
	FP_GETACCOUNTLIST			m_fpGetAccountList{ nullptr };
	FP_GETACCOUNTNAME			m_fpGetAccountName{ nullptr };
	FP_GETACCTDETAILNAME		m_fpGetAcctDetailName{ nullptr };
	FP_GETACCTNICKNAME			m_fpGetAcctNickname{ nullptr };

	FP_GETCOMMMEDIA				m_fpGetCommMedia{ nullptr };
	FP_GETETKMEDIA				m_fpGetETKMedia{ nullptr };
	FP_GETCLIENTIP				m_fpGetClientIP{ nullptr };
	FP_GETSERVERNAME			m_fpGetServerName{ nullptr };
	FP_GETAPIPATH				m_fpGetAPIPath{ nullptr };

	FP_SETHEADERINFO			m_fpSetHeaderInfo{ nullptr };
	FP_SETUSEAPIVER				m_fpSetUseAPIVer{ nullptr };
	FP_SETMODE					m_fpSetMode{ nullptr };

	FP_GETPROCBRANCHNO			m_fpGetProcBranchNo{ nullptr };
	FP_GETUSEOVERFUTURE			m_fpGetUseOverFuture{ nullptr };
	FP_GETUSEOVERSTOCK			m_fpGetUseOverStock{ nullptr };
	FP_GETUSEFX					m_fpGetUseFX{ nullptr };

	FP_GETTRCOUNTPERSEC			m_fpGetTRCountPerSec{ nullptr };
	FP_GETTRCOUNTBASESEC		m_fpGetTRCountBaseSec{ nullptr };
	FP_GETTRCOUNTREQUEST		m_fpGetTRCountRequest{ nullptr };
	FP_GETTRCOUNTLIMIT			m_fpGetTRCountLimit{ nullptr };

	FP_SETNOTIFYFLAG			m_fpSetNotifyFlag{ nullptr };

	FP_REQUESTSERVICE			m_fpRequestService{ nullptr };
	FP_REMOVESERVICE			m_fpRemoveService{ nullptr };

	FP_REQUESTLINKTOHTS			m_fpRequestLinkToHTS{ nullptr };
	FP_ADVISELINKFROMHTS		m_fpAdviseLinkFromHTS{ nullptr };
	FP_UNADVISELINKFROMHTS		m_fpUnAdviseLinkFromHTS{ nullptr };

	FP_DECOMPRESS				m_fpDecompress{ nullptr };
	FP_ISCHARTLIB				m_fpIsChartLib{ nullptr };

	FP_SETPROGRAMORDER			m_fpSetProgramOrder{ nullptr };
	FP_GETPROGRAMORDER			m_fpGetProgramOrder{ nullptr };

	bool init_functions();

	// 서버연결
	BOOL Connect(LPCSTR pszSvrIP, const int nPort, const int nStartMsgID = WM_USER, const int nTimeOut = -1, const int nSendMaxPacketSize = 1024, const HWND _hWnd = hDefaultWnd);
	BOOL Login(LPCSTR pszID, LPCSTR pszPwd, LPCSTR pszCertPwd, const int nType, const BOOL bShowCertErrDlg = TRUE, const HWND _hWnd = hDefaultWnd);

public:
	IXingAPI(HWND _hWnd = nullptr);
	virtual ~IXingAPI();
	//static IXingAPI& GetInstance(HWND _hWnd = nullptr)
	//{
	//	static IXingAPI instance(_hWnd);
	//	return instance;
	//}

	std::string strFuOpAccNo; // 현재 로그인된 계좌번호
	std::string strFuOpAccPW; // 계좌 비밀번호

	void Initialize(HWND _hWnd);
	void Destroy();

	BOOL Connect(const BOOL _bMockServer, const int _nStartMsgID = WM_USER);
	BOOL Login(LPCSTR _pID, LPCSTR _pPW, LPCSTR _pCR);
	inline BOOL Login(std::string_view _pID, std::string_view _pPW, std::string_view _pCR)
	{
		return Login(_pID.data(), _pPW.data(), _pCR.data());
	}

	void set_status(LONG _nStatus) { ::InterlockedExchange(&this->nStatusXingAPI, _nStatus); }
	LONG get_status() const { return ::InterlockedCompareExchange(const_cast<LONG*>(&this->nStatusXingAPI), 0, 0); } // Use const_cast to match the function signature

	
	BOOL	IsConnected();
	BOOL	Disconnect();

	// 로그인
	
	BOOL	Logout(HWND _hWnd = hDefaultWnd);

	int		GetLastError() const;
	LPCSTR	GetErrorMessage(int _nErrorCode) const;

	// 조회성TR 관련
	int Request(LPCSTR _pTrCode, const LPVOID lpData, const int nDataSize, const BOOL bNext = FALSE, LPCSTR pszNextKey = "", const int nTimeOut = 30, const HWND _hWnd = hDefaultWnd);
	inline int Request(LPCSTR _pTrCode, const LPVOID lpData, const int nDataSize, const HWND _hWnd)
	{
		return this->Request(_pTrCode, lpData, nDataSize, FALSE, "", 30, _hWnd);
	}
	int RequestWait(LPCSTR _pTrCode, const LPVOID lpData, const int nDataSize, const HWND _hWnd = hDefaultWnd);
	int RequestForce(LPCSTR _pTrCode, const LPVOID lpData, const int nDataSize, const HWND _hWnd = hDefaultWnd);
	BOOL	ReleaseRequestData(int nRequestID) const;
	BOOL	ReleaseMessageData(LPARAM lp) const;

	// 실시간 관련
	BOOL AdviseRealData(LPCSTR pszTrNo, LPCSTR pszData = "", const int nDataUnitLen = 8, const HWND _hWnd = hDefaultWnd);
	inline BOOL AdviseRealData(std::string_view pszTrNo, std::string_view pszData)
	{
		const int nDataUnitLen = static_cast<int>(pszData.length());
		return AdviseRealData(pszTrNo.data(), pszData.data(), nDataUnitLen, hDefaultWnd);
	}
	BOOL	UnadviseRealData(LPCSTR pszTrNo, LPCSTR pszData = "", const int nDataUnitLen = 8, const HWND _hWnd = hDefaultWnd) const;
	BOOL	UnadviseWindow(const HWND _hWnd = hDefaultWnd) const;

	// 계좌 관련
	int		GetAccountListCount() const;
	BOOL	GetAccountList(int nIndex, LPSTR pszAcc, int nAccSize) const;
	BOOL	GetAccountName(LPCSTR pszAcc, LPSTR pszAccName, int nAccNameSize) const;
	BOOL	GetAcctDetailName(LPCSTR pszAcc, LPSTR pszAcctDetailName, int nAcctDetailNameSize) const;
	BOOL	GetAcctNickname(LPCSTR pszAcc, LPSTR pszAcctNickname, int nAcctNicknameSize) const;

	// 정보얻기
	LPCSTR	GetCommMedia() const;			// 통신매체를 구한다.
	LPCSTR	GetETKMedia() const;			// 당사매체를 구한다.
	LPCSTR	GetClientIP() const;			// 공인IP를 구한다.
	LPCSTR	GetServerName() const;			// 서버명을 구한다.
	LPCSTR GetAPIPath() const;			// 실행중인 xingAPI의 경로

	void	SetHeaderInfo(LPCSTR szType, LPCSTR szValue);
	void	SetUseAPIVer(LPCSTR szUserAPIVer);
	BOOL	SetMode(LPCSTR pszMode, LPCSTR pszValue);

	LPCSTR	GetProcBranchNo() const;			// 처리점을 구한다.
	BOOL	GetUseOverFuture() const;			// 해외선물 사용권한	
	BOOL	GetUseOverStock() const;			// 해외주식 사용권한	
	BOOL	GetUseFX() const;			// FX 사용권한
	BOOL	IsChartLib() const;			// 차트라이브러리 연결

	int		GetTRCountPerSec(LPCSTR pszCode) const;				// N초당 전송 가능 횟수 : N초내 가능 횟수
	int		GetTRCountBaseSec(LPCSTR pszCode) const;				// N초당 전송 가능 횟수 : N초
	int		GetTRCountRequest(LPCSTR pszCode) const;				// TR 호출 횟수
	int		GetTRCountLimit(LPCSTR pszCode) const;				// 10분당 전송 가능 횟수

	void	SetNotifyFlag(BOOL bNotifyFlag);	// 긴급메시지, 서버접속 단절통지 등의 통보 설정 (지원 예정)

	void	SetProgramOrder(BOOL bProgramOrder) const;				// 프로그램매매 설정
	BOOL	GetProgramOrder() const;									// 프로그램매매 설정여부 확인

	//------------------------------------------------------------------------------
	// 부가 서비스 TR 조회
	//     반환값       - Request() 함수와 이용 방식 동일
	//     hWnd			- 조회 결과를 받을 윈도우의 핸들
	//
	//
	// ★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	// ★★★★★ 
	// ★★★★★  [1] 종목 검색        
	// ★★★★★
	// ★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//
	//     pszCode		- "t1833" (HTS '[1807] 종목검색' 에서 'API 로 내보내기' 저장한 조건의 종목을 검색하는 TR)
	//     lpData		- 'API로 내보내기' 한 파일의 전체 경로 지정, NULL 입력시 파일다이얼로그 표시
	//
	//     ex) HTS '[1807] 종목검색' 에서 'API 로 내보내기' 저장한 파일이 "D:\test.adf"
	//			pszCode = "t1833", pszData = "D:\test.adf"   
	//			int nReqID = RequestService( hWnd, "t1833", "D:\test.adf" );	
	// 
	// ★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	// ★★★★★ 
	// ★★★★★  [2] ~ [3] 차트 기초데이터를 이용해 지표데이터를 제공
	// ★★★★★
	// ★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//
	//	지표데이터는 챠트 기초데이터를 수신받아 API내부에서 가공하여 제공하는 것으로 조회 및 실시간 응답에 
	//  다소 시간이 걸릴 수 있습니다.
	//  자료실게시판 내에 "ChartApiSample(VC++ 2012)" 샘플을 참고하시기 바랍니다.
	//
	//  ※ 조회시 실시간 자동등록을 하면, 실시간 지표데이터 발생시 XM_RECEIVE_REAL_DATA_CHART 로 메시지가 수신
	//	  WPARAM = 해당 없음 
	//	  LPARAM = LPRECV_REAL_PACKET (RECV_REAL_PACKET 의 pszData  = 조회 TR의 outblock1과 동일)
	//
	//
	//  [2] 차트 지표데이터 조회 (HTS '[4201]xing차트1'의 수식관리자내 지표와 동일하며, DevCenter 메뉴 [부가서비스-수식관리자] 를 통해 지표 저장)
	//
	//     pszCode		- "ChartIndex" (차트 지표데이터 조회용 TR)
	//     lpData		- "ChartIndex" TR내 Inblock의 데이터 구조체
	//
	//     ex) "MACD" 지표데이터 조회
	//         ChartIndexInBlock sInBlock;
	//         sInBlock.indexid		= 지표ID	 // 최초 조회시 공백, '동일 종목 - 동일 지표' 조회시 이전 조회 ChartIndexOutBlock의 indexid
	//         sInBlock.indexname	= "MACD" 
	//         sInBlock.market		= "1"		 // 주식
	//         ...생략.. 
	//		   RemoveService( hWnd, "ChartIndex", sInBlock.indexid );				
	//         int nReqID = RequestService( hWnd, "ChartIndex", &sInBlock );	
	// 
	// 
	// [3] 차트 엑셀데이터 조회 (HTS '[4201]xing차트1'의 수식관리자내 지표와 동일하며, DevCenter 메뉴 [부가서비스-수식관리자] 를 통해 지표 저장)
	//     직접 저장한 차트 기초데이터를 엑셀 포맷으로 변경한 후, RequestService() 호출 시 지표데이터로 가공하여 제공  
	//     ("xingAPI 설치폴더/엑셀샘플/ChartExcelData.xls" 참고)
	//
	//     pszCode		- "ChartExcel" (차트 지표데이터 조회용 TR)
	//     lpData		- "ChartExcel" TR내 Inblock의 데이터 구조체
	//
	//     ex) 직접 쌓은 시고저종 데이터를 엑셀 포맷으로 변환하여 저장한 파일이 "C:\ebest\xingAPI\엑셀샘플\ChartExcelData.xls"
	//         ChartExcelInBlock sInBlock;
	//          sInBlock.indexid		= 지표ID	 // 최초 조회시 공백, '동일 종목 - 동일 지표' 조회시 이전 조회 ChartIndexOutBlock의 indexid
	//         sInBlock.indexname		= "MACD"		
	//         sInBlock.excelfilename	= "C:\ebest\xingAPI\엑셀샘플\ChartExcelData.xls"
	//         ...생략.. 
	//         RemoveService( hWnd, "ChartExcel", sInBlock.indexid );				
	//         int nReqID = RequestService( hWnd, "ChartExcel", &sInBlock );	
	// 
	//
	// ★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	// ★★★★★ 
	// ★★★★★  [4] ~ [5] e종목검색 실시간 데이터 제공
	// ★★★★★
	// ★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//
	//  자료실게시판 내에 "XingAPI_Sample_eSearch" 샘플을 참고하시기 바랍니다.
	//
	//  ※ 조회시 실시간 자동등록을 하면, 실시간 데이터 발생시 XM_RECEIVE_REAL_DATA_SEARCH 로 메시지가 수신
	//	  WPARAM = 해당 없음 
	//	  LPARAM = LPRECV_REAL_PACKET (RECV_REAL_PACKET 의 pszData  = 조회 TR의 outblock1과 동일)
	//
	//
	//  [4] 종목검색 데이터 조회
	//
	//     pszCode		- "t1857" (HTS '[1892] e종목검색' 에서 'API 로 내보내기' 혹은 '전략관리 -> 서버저장' 으로  저장한 조건의 종목을 검색하는 TR)
	//     lpData		- "t1857" TR내 Inblock의 데이터 구조체
	//
	//     ex) 종목검색 
	//         t1857InBlock	pckInBlock;
	//		   char		szTrNo[]	= "t1857";
	//		   char			szNextKey[]	= "";
	//         ...생략.. 
	//		   set_packet_data( pckInBlock.sRealFlag		, sizeof( pckInBlock.sRealFlag		), str_Real	   , DATA_TYPE_STRING );	// 실시간 여부 1:등록 0:조회만
	//		   set_packet_data( pckInBlock.sSearchFlag	, sizeof( pckInBlock.sSearchFlag	), str_Flag	   , DATA_TYPE_STRING );	// 조회구분값 S:서버 F:파일
	//		   set_packet_data( pckInBlock.query_index	, sizeof( pckInBlock.query_index	), str_Index   , DATA_TYPE_STRING );	// 종목검색입력값
	//         int nReqID = RequestService( hWnd, szTrNo, (LPCSTR)&pckInBlock );	
	// 
	// 
	// [5] 종목검색 실시간 데이터 발생 시 XM_RECEIVE_REAL_DATA_SEARCH로 메세지 수신
	//
	//	   WPARAM = 해당 없음 
	//	   LPARAM = LPRECV_REAL_PACKET (RECV_REAL_PACKET 의 pszData  = 조회 TR의 outblock1과 동일)
	//
	//     ex) LPRECV_REAL_PACKET pRealPacket = (LPRECV_REAL_PACKET)lParam;
	//		   LPt1857OutBlock1 pOutBlock = (LPt1857OutBlock1)pRealPacket->pszData;
	// 
	// 	
	int		RequestService(LPCSTR pszCode, LPCSTR pszData, HWND _hWnd = hDefaultWnd);


	//------------------------------------------------------------------------------
	// 부가 서비스 조회 TR 해제
	//     반환값       - 부가서비스에 따라 달라짐
	//     hWnd			- 조회 결과를 받을 윈도우의 핸들
	//
	// ★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	// ★★★★★ 
	// ★★★★★  [1] 종목 검색
	// ★★★★★ 
	// ★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//
	//		해당 없음
	//
	// ★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	// ★★★★★ 
	// ★★★★★  [2] ~ [3] 차트데이터 조회 
	// ★★★★★ 
	// ★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//
	//		지표데이터는 챠트 기초데이터를 수신받아 API내부에서 가공하여 제공하는 것으로
	//      많이 조회할수록 API에 부하가 갈 수 있으니, 사용하지 않는 지표는 해제하는 것이 좋습니다.
	//
	//
	//		※ 조회 시 자동등록한 실시간을 해제할 때도 호출함
	//
	//     pszCode		- "ChartIndex" or "ChartExcel" 
	//     lpData		- 각 TR의  OutBlock의 indexid
	//
	// ★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	// ★★★★★ 
	// ★★★★★  [4] ~ [5] e종목검색 조회 
	// ★★★★★ 
	// ★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//
	//		실시간 종목검색은 등록 갯수에 제한이 있기때문에, 사용하지 않는 경우 해제하는 것이 좋습니다(기본 2개)
	//
	//
	//
	//     pszCode		- "t1857"
	//     lpData		- t1857 TR의 OutBlock의 AlertNum 값
	//
	int		RemoveService(LPCSTR pszCode, LPCSTR pszData, HWND _hWnd = hDefaultWnd);

	//------------------------------------------------------------------------------
	// API 에서 HTS로 연동 (API -> HTS) : 연동을 원할때마다 요청한다
	//     반환값       - 연동 성공 여부
	//     hWnd			- 연동을 요청한 윈도우의 핸들
	//     
	// [1] 종목 연동
	//     pszLinkKey	- &STOCK_CODE				: 주식 종목코드
	//                    &ETF_CODE					: ETF 종목코드
	//                    &ELW_CODE					: ELW 종목코드
	//                    &KONEX_CODE				: 코넥스 종목코드
	//                    &FREEBOARD_CODE			: 프리보드 종목코드
	//                    &KSPI_CODE				: 코스피 업종 코드
	//                    &KSQI_CODE				: 코스닥 업종 코드
	//                    &FUTURE_CODE				: 선물종목코드
	//                    &OPTION_CODE				: 옵션종목코드
	//                    &FUTOPT_CODE				: 선물/옵션 종목코드 
	//                    &FUTSP_CODE				: 선물스프레드 종목코드
	//                    &STOCK_FUTURE_CODE		: 주식 선물 종목코드
	//                    &STOCK_OPTION_CODE		: 주식 옵션 종목코드
	//                    &STOCK_FUTOPT_CODE		: 주식 선물옵션 종목코드 
	//                    &STOCK_FUTSP_CODE			: 주식 선물스프레드 종목코드
	//                    &FUTOPT_STOCK_FUTOPT_CODE : 선물옵션 & 주식 선물옵션 종목코드
	//                    &US_CODE					: 해외종목코드
	//                    &COMMODITY_FUTOPT_CODE	: 상품선물/선물옵션
	//                    &COMMODITY_FUTURE_CODE	: 상품선물
	//                    &COMMODITY_STAR_CODE		: 스타선물
	//                    &CME_FUTURE_CODE			: CME야간선물
	//                    &EUREX_OPTION_CODE		: EUREX야간옵션
	//                    &NIGHT_FUTOPT_CODE		: 야간선물옵션
	//     pszData    	- 상품별 종목코드  
	//
	//     ex) 주식 종목 연동 : pszLinkKey = "&STOCK_CODE", pszData = "078020"
	//         선물 종목 연동 : pszLinkKey = "&FUTURE_CODE", pszData = "101HC000"
	//     
	// [2] HTS 화면 열기 
	//     pszLinkKey	- &OPEN_SCREEN : 화면 열기
	//     pszData    	- 열고 싶은 화면 번호
	//
	//     ex) HTS의 '[6602]선옵원장 미결제잔고' 열기
	//         pszLinkKey = &OPEN_SCREEN, pszData = "6602"	

	int		RequestLinkToHTS(LPCSTR pszLinkKey, LPCSTR pszData, LPCSTR pszFiller = NULL, HWND _hWnd = hDefaultWnd);

	//------------------------------------------------------------------------------
	// HTS 에서 API로 연동 (HTS -> API) : 등록하면 해제할때까지 연동된다
	//     hWnd			- 연동을 원하는 윈도우의 핸들
	// 
	// ※ HTS에서 연동 발생 시, XM_RECEIVE_LINK_DATA 로 메시지가 발생
	//	  WPARAM = LINK_DATA, 
	//	  LPARAM = LINKDATA_RECV_MSG 구조체 데이터 
	//    ★★★  사용방식은 Real 수신과 동일, LPARAM 메모리 수신 후 반드시 해제 필요

	void	AdviseLinkFromHTS(HWND _hWnd = hDefaultWnd) const;
	void	UnAdviseLinkFromHTS(HWND _hWnd = hDefaultWnd) const;

	//------------------------------------------------------------------------------
	// 데이터의 압축을 해제한다 : 틱챠트 데이터 등에서 압축상태로 수신받은 경우 사용
	//     반환값       - 압축을 해제한 데이터(pszDest)의 길이
	//
	//     pszSrc		- 압축상태 데이터
	//     pszDest		- 압축을 해제한 데이터를 저장할 메모리 (Outblock 구조체 사이즈 최대 2000건)
	//	   nSrcLen	    - pszSrc 데이터의 길이
	// 
	// 사용 방법 
	//     ex) t8411 TR 이용시, InBlock의 comp_yn(압축여부) 필드에 "Y" 입력 후 조회
	//          ReceiveData() 에서 Occurs 블럭(t8411OutBlock1)이 압축되어 수신되므로, 해당 블럭 압축을 해제
	// 
	//			LRESULT t8411_Wnd::OnXMReceiveData( WPARAM wParam, LPARAM lParam )
	//			{
	//				//-------------------------------------------------------------------------------------
	//				// Data를 받음
	//				if( wParam == REQUEST_DATA )
	//				{
	//					LPRECV_PACKET pRpData = (LPRECV_PACKET)lParam;
	//
	//					if( strcmp( pRpData->szBlockName, NAME_t8411OutBlock ) == 0 )
	//					{
	//					}
	//					else if( strcmp( pRpData->szBlockName, NAME_t8411OutBlock1 ) == 0 )
	//					{
	//						LPt8411OutBlock1 pOutBlock1 = (LPt8411OutBlock1)pRpData->lpData;
	//
	//						t8411OutBlock1 szOutBlock1[2000];		// 압축 해제시 최대 2000건 수신
	//						int nDestSize = g_iXingAPI.Decompress((char *)pOutBlock1, (char *)&szOutBlock1[0], pRpData->nDataLength);
	//
	//						// Occurs 일 경우
	//						// Header가 'A' 이면 전체길이에서 OutBlock의 길이를 나눠서 갯수를 구한다.
	//						if (nDestSize > 0)
	//						{
	//							int nCount = nDestSize / sizeof( t8411OutBlock1 );
	//
	//							for( int i=0; i<nCount; i++ )
	//							{
	//								데이터 표시 
	//							}
	//						}
	//					}
	//				}
	//			}
	// 
	int		Decompress(LPCSTR pszSrc, LPCSTR pszDest, int nSrcLen) const;

	//------------------------------------------------------------------------------
	inline void req_t2101(std::string_view _pCode)
	{	// 선물/옵션 현재가(시세) 조회, lastmonth 에 만기일이 있음.
		_t2101InBlock t2101InBlock;
		xing::set_packet_data(t2101InBlock.focode, sizeof(t2101InBlock.focode), _pCode.data(), DATA_TYPE_STRING);
		this->RequestWait("t2101", &t2101InBlock, sizeof(_t2101InBlock));
	}
	inline int req_t2301(std::string_view _pYYYYMM, std::string_view _pGubun = "G")	// G: 정규, M: 미니, W: 위클리
	{	// 선물/옵션 현재가(시세) 조회, lastmonth 에 만기일이 있음.
		_t2301InBlock t2301InBlock;
		xing::set_packet_data(t2301InBlock.yyyymm, sizeof(t2301InBlock.yyyymm), _pYYYYMM.data(), DATA_TYPE_STRING);
		xing::set_packet_data(t2301InBlock.gubun, sizeof(t2301InBlock.gubun), _pGubun.data(), DATA_TYPE_STRING);
		return this->RequestWait("t2301", &t2301InBlock, sizeof(_t2301InBlock));
	}
	inline void req_t8432(std::string_view _pGubun = " ")
	{	// 지수선물마스터조회API용, t8432 는 V는 변동성지수선물, S는 섹터지수선물, 그외의 값은 코스피200지수선물
		_t8432InBlock t8432InBlock;
		xing::set_packet_data(t8432InBlock.gubun, sizeof(t8432InBlock.gubun), _pGubun.data(), DATA_TYPE_STRING);
		this->RequestWait("t8432", &t8432InBlock, sizeof(_t8432InBlock));
	}

	inline void req_t8433()
	{	// 지수옵션마스터조회API용, t8433 는 V는 변동성지수옵션, S는 섹터지수옵션, 그외의 값은 코스피200지수옵션
		_t8433InBlock t8433InBlock;
		this->RequestWait("t8433", &t8433InBlock, sizeof(_t8433InBlock));
	}

	inline void req_t8435(std::string_view _pGubun = "WK")
	{	// MF: 미니선물, MO: 미니옵션, WK: 위클리옵션, SF: 코스닥150선물
		_t8435InBlock t8435InBlock{};
		xing::set_packet_data(t8435InBlock.gubun, sizeof(t8435InBlock.gubun), _pGubun.data(), DATA_TYPE_STRING);
		this->RequestWait("t8435", &t8435InBlock, sizeof(_t8435InBlock));
	}
	// 선물옵션 정상주문, 옵션 매도는 손실이 무한대라 사용하지 않는다.
	inline void req_option_open(std::string_view _pCode, std::string_view _pPrice, std::string_view _pQty)
	{	// 계좌번호와 비밀번호가 설정되지 않았으면 주문하지 않는다.
		if (strFuOpAccNo.empty() || strFuOpAccPW.empty()) { return; }
		// BnsTpCode
		// "1" 매도
		// "2" 매수
		// FnoOrdprcPtnCode
		// "00" 지정가
		// "03" 시장가
		// "05" 조건부지정가
		// "06" 최유리지정가
		// "10" 지정가(IOC)
		// "20" 지정가(FOK)
		// "13" 시장가(IOC)
		// "23" 시장가(FOK)
		_CFOAT00100InBlock1	CFOAT00100InBlock;
		dk::init(CFOAT00100InBlock, ' ');

		xing::set_packet_data(CFOAT00100InBlock.AcntNo, sizeof(CFOAT00100InBlock.AcntNo), strFuOpAccNo.c_str(), DATA_TYPE_STRING);			// 계좌번호
		xing::set_packet_data(CFOAT00100InBlock.Pwd, sizeof(CFOAT00100InBlock.Pwd), strFuOpAccPW.c_str(), DATA_TYPE_STRING);				// 비밀번호
		xing::set_packet_data(CFOAT00100InBlock.FnoIsuNo, sizeof(CFOAT00100InBlock.FnoIsuNo), _pCode.data(), DATA_TYPE_STRING);				// 선물옵션종목번호
		xing::set_packet_data(CFOAT00100InBlock.BnsTpCode, sizeof(CFOAT00100InBlock.BnsTpCode), "2", DATA_TYPE_STRING);						// 매매구분
		xing::set_packet_data(CFOAT00100InBlock.FnoOrdprcPtnCode, sizeof(CFOAT00100InBlock.FnoOrdprcPtnCode), "00", DATA_TYPE_STRING);		// 선물옵션호가유형코드
		xing::set_packet_data(CFOAT00100InBlock.FnoOrdPrc, sizeof(CFOAT00100InBlock.FnoOrdPrc), _pPrice.data(), DATA_TYPE_FLOAT_DOT, 8);	// 선물옵션주문가격
		xing::set_packet_data(CFOAT00100InBlock.OrdQty, sizeof(CFOAT00100InBlock.OrdQty), _pQty.data(), DATA_TYPE_LONG);					// 주문수량

		//DBGPRINT("IXingAPI::req_option_open() - AcntNo: %s, Pwd: %s, FnoIsuNo: %s, BnsTpCode: %c, FnoOrdprcPtnCode: %s, FnoOrdPrc: %s, OrdQty: %s",
		//	strFuOpAccNo.c_str()
		//	, strFuOpAccPW.c_str()
		//	, _pCode.data()
		//	, CFOAT00100InBlock.BnsTpCode
		//	, CFOAT00100InBlock.FnoOrdprcPtnCode
		//	, CFOAT00100InBlock.FnoOrdPrc
		//	, _pQty.data()
		//);

		this->RequestWait("CFOAT00100", &CFOAT00100InBlock, sizeof(_CFOAT00100InBlock1));
	}

	// 거부는 신규, 정정, 취소 모두 발생하고, 신규는 거부 나올 시 재처리가 해야함.

	// 선물옵션 정상주문, 옵션 시장가 매도는 반드시 주문 가능 수량이 있을 때 그 수량만큼만 사용해야한다.
	inline void req_option_close(std::string_view _pCode, std::string_view _pQty)
	{
		if (strFuOpAccNo.empty() || strFuOpAccPW.empty()) { return; }
		_CFOAT00100InBlock1 CFOAT00100InBlock;
		dk::init(CFOAT00100InBlock, ' ');

		xing::set_packet_data(CFOAT00100InBlock.AcntNo, sizeof(CFOAT00100InBlock.AcntNo), strFuOpAccNo.c_str(), DATA_TYPE_STRING);			// 계좌번호
		xing::set_packet_data(CFOAT00100InBlock.Pwd, sizeof(CFOAT00100InBlock.Pwd), strFuOpAccPW.c_str(), DATA_TYPE_STRING);					// 비밀번호
		xing::set_packet_data(CFOAT00100InBlock.FnoIsuNo, sizeof(CFOAT00100InBlock.FnoIsuNo), _pCode.data(), DATA_TYPE_STRING);				// 선물옵션종목번호
		xing::set_packet_data(CFOAT00100InBlock.BnsTpCode, sizeof(CFOAT00100InBlock.BnsTpCode), "1", DATA_TYPE_STRING);						// 매매구분
		xing::set_packet_data(CFOAT00100InBlock.FnoOrdprcPtnCode, sizeof(CFOAT00100InBlock.FnoOrdprcPtnCode), "03", DATA_TYPE_STRING);		// 선물옵션호가유형코드
		xing::set_packet_data(CFOAT00100InBlock.FnoOrdPrc, sizeof(CFOAT00100InBlock.FnoOrdPrc), "0", DATA_TYPE_FLOAT_DOT, 8);				// 선물옵션주문가격
		xing::set_packet_data(CFOAT00100InBlock.OrdQty, sizeof(CFOAT00100InBlock.OrdQty), _pQty.data(), DATA_TYPE_LONG);

		this->RequestWait("CFOAT00100", &CFOAT00100InBlock, sizeof(_CFOAT00100InBlock1));
	}
	// 선물옵션 정정주문
	inline void req_option_modify(std::string_view _pCode, std::string_view _pOrdNo, std::string_view _pPrice, std::string_view _pQty)
	{
		if (strFuOpAccNo.empty() || strFuOpAccPW.empty()) { return; }
		_CFOAT00200InBlock1 CFOAT00200InBlock;
		dk::init(CFOAT00200InBlock, ' ');

		xing::set_packet_data(CFOAT00200InBlock.AcntNo, sizeof(CFOAT00200InBlock.AcntNo), strFuOpAccNo.c_str(), DATA_TYPE_STRING);			// 계좌번호
		xing::set_packet_data(CFOAT00200InBlock.Pwd, sizeof(CFOAT00200InBlock.Pwd), strFuOpAccPW.c_str(), DATA_TYPE_STRING);					// 비밀번호
		xing::set_packet_data(CFOAT00200InBlock.FnoIsuNo, sizeof(CFOAT00200InBlock.FnoIsuNo), _pCode.data(), DATA_TYPE_STRING);				// 선물옵션종목번호
		xing::set_packet_data(CFOAT00200InBlock.OrgOrdNo, sizeof(CFOAT00200InBlock.OrgOrdNo), _pOrdNo.data(), DATA_TYPE_LONG);				// 원주문번호
		xing::set_packet_data(CFOAT00200InBlock.FnoOrdprcPtnCode, sizeof(CFOAT00200InBlock.FnoOrdprcPtnCode), "03", DATA_TYPE_STRING);		// 선물옵션호가유형코드
		xing::set_packet_data(CFOAT00200InBlock.FnoOrdPrc, sizeof(CFOAT00200InBlock.FnoOrdPrc), _pPrice.data(), DATA_TYPE_FLOAT_DOT, 8);	// 주문가격
		xing::set_packet_data(CFOAT00200InBlock.MdfyQty, sizeof(CFOAT00200InBlock.MdfyQty), _pQty.data(), DATA_TYPE_LONG);					// 정정수량

		this->RequestWait("CFOAT00200", &CFOAT00200InBlock, sizeof(_CFOAT00200InBlock1));
	}
	// 선물옵션 취소주문
	inline void req_option_cancle(std::string_view _pCode, std::string_view _pOrdNo, std::string_view _pQty)
	{
		if (strFuOpAccNo.empty() || strFuOpAccPW.empty()) { return; }

		_CFOAT00300InBlock1 CFOAT00300InBlock;
		dk::init(CFOAT00300InBlock, ' ');

		xing::set_packet_data(CFOAT00300InBlock.AcntNo, sizeof(CFOAT00300InBlock.AcntNo), strFuOpAccNo.c_str(), DATA_TYPE_STRING);			// 계좌번호
		xing::set_packet_data(CFOAT00300InBlock.Pwd, sizeof(CFOAT00300InBlock.Pwd), strFuOpAccPW.c_str(), DATA_TYPE_STRING);					// 비밀번호
		xing::set_packet_data(CFOAT00300InBlock.FnoIsuNo, sizeof(CFOAT00300InBlock.FnoIsuNo), _pCode.data(), DATA_TYPE_STRING);				// 선물옵션종목번호
		xing::set_packet_data(CFOAT00300InBlock.OrgOrdNo, sizeof(CFOAT00300InBlock.OrgOrdNo), _pOrdNo.data(), DATA_TYPE_LONG);				// 원주문번호
		xing::set_packet_data(CFOAT00300InBlock.CancQty, sizeof(CFOAT00300InBlock.CancQty), _pQty.data(), DATA_TYPE_LONG);					// 취소수량

		this->RequestWait("CFOAT00300", &CFOAT00300InBlock, sizeof(_CFOAT00300InBlock1));
	}
	// 선물/옵션 체결/미체결
	//typedef struct _t0434InBlock
	//{
	//	char    accno[11];    // [string,   11] 계좌번호                        StartPos 0, Length 11
	//	char    passwd[8];    // [string,    8] 비밀번호                        StartPos 11, Length 8
	//	char    expcode[8];    // [string,    8] 종목번호                        StartPos 19, Length 8
	//	char    chegb[1];    // [string,    1] 체결구분                        StartPos 27, Length 1
	//	char    sortgb[1];    // [string,    1] 정렬순서                        StartPos 28, Length 1
	//	char    cts_ordno[7];    // [string,    7] CTS_주문번호                    StartPos 29, Length 7
	//} t0434InBlock, * LPt0434InBlock;
	inline void req_option_chegye(std::string_view _pCode, std::string_view _pChegb = "0"
		, std::string_view _pCtsOrdNo = " ")
	{
		if (strFuOpAccNo.empty() || strFuOpAccPW.empty()) { return; }
		_t0434InBlock t0434InBlock;
		dk::init(t0434InBlock, ' ');
		xing::set_packet_data(t0434InBlock.accno, sizeof(t0434InBlock.accno), strFuOpAccNo.c_str(), DATA_TYPE_STRING);
		xing::set_packet_data(t0434InBlock.passwd, sizeof(t0434InBlock.passwd), strFuOpAccPW.c_str(), DATA_TYPE_STRING);
		xing::set_packet_data(t0434InBlock.expcode, sizeof(t0434InBlock.expcode), _pCode.data(), DATA_TYPE_STRING);
		// 체결구분 0: 전체, 1: 체결, 2: 미체결
		xing::set_packet_data(t0434InBlock.chegb, sizeof(t0434InBlock.chegb), _pChegb.data(), DATA_TYPE_STRING);
		xing::set_packet_data(t0434InBlock.sortgb, sizeof(t0434InBlock.sortgb), "1", DATA_TYPE_STRING);
		// 처음 조회시는 Space, 연속 조회시에 이전 조회한 OutBlock의 cts_ordno 값으로 설정
		xing::set_packet_data(t0434InBlock.cts_ordno, sizeof(t0434InBlock.cts_ordno), _pCtsOrdNo.data(), DATA_TYPE_STRING);
		this->RequestWait("t0434", &t0434InBlock, sizeof(_t0434InBlock));
	}
	
	// 선물/옵션 잔고평가(이동평균)
	//typedef struct _t0441InBlock
	//{
	//	char    accno[11];    char    _accno;    // [string,   11] 계좌번호                        StartPos 0, Length 11
	//	char    passwd[8];    char    _passwd;    // [string,    8] 비밀번호                        StartPos 12, Length 8
	//	char    cts_expcode[8];    char    _cts_expcode;    // [string,    8] CTS_종목번호                    StartPos 21, Length 8
	//	char    cts_medocd[1];    char    _cts_medocd;    // [string,    1] CTS_매매구분                    StartPos 30, Length 1
	//} t0441InBlock, * LPt0441InBlock;
	inline void req_option_balance(std::string_view _pCode)
	{
		if (strFuOpAccNo.empty() || strFuOpAccPW.empty()) { return; }
		_t0441InBlock t0441InBlock;
		dk::init(t0441InBlock, ' ');
		xing::set_packet_data(t0441InBlock.accno, sizeof(t0441InBlock.accno), strFuOpAccNo.c_str(), DATA_TYPE_STRING);
		xing::set_packet_data(t0441InBlock.passwd, sizeof(t0441InBlock.passwd), strFuOpAccPW.c_str(), DATA_TYPE_STRING);
		xing::set_packet_data(t0441InBlock.cts_expcode, sizeof(t0441InBlock.cts_expcode), _pCode.data(), DATA_TYPE_STRING);
		xing::set_packet_data(t0441InBlock.cts_medocd, sizeof(t0441InBlock.cts_medocd), "2", DATA_TYPE_STRING); // 2: 매수평가
		this->RequestWait("t0441", &t0441InBlock, sizeof(_t0441InBlock));
	}

	// 선물옵션 계좌잔고 및 평가현황3	"CFOAQ56000"
	//inline void req_option_balance(std::string_view _pCode, std::string_view _pOrdDate)
	//{
	//	//typedef struct _CFOAQ50600InBlock1
	//	//{
	//	//	char    RecCnt[5];    // [long  ,    5] 레코드갯수                      StartPos 0, Length 5
	//	//	char    AcntNo[20];    // [string,   20] 계좌번호                        StartPos 5, Length 20
	//	//	char    InptPwd[8];    // [string,    8] 입력비밀번호                    StartPos 25, Length 8
	//	//	char    OrdDt[8];    // [string,    8] 주문일                          StartPos 33, Length 8
	//	//	char    BalEvalTp[1];    // [string,    1] 잔고평가구분                    StartPos 41, Length 1
	//	//	char    FutsPrcEvalTp[1];    // [string,    1] 선물가격평가구분                StartPos 42, Length 1
	//	//	char    LqdtQtyQryTp[1];    // [string,    1] 청산수량조회구분                StartPos 43, Length 1
	//	//} CFOAQ50600InBlock1, * LPCFOAQ50600InBlock1;

	//	if (strFuOpAccNo.empty() || strFuOpAccPW.empty()) { return; }
	//	_CFOAQ50600InBlock1 CFOAQ50600InBlock1;
	//	dk::init(CFOAQ50600InBlock1, ' ');

	//	xing::set_packet_data(CFOAQ50600InBlock1.RecCnt, sizeof(CFOAQ50600InBlock1.RecCnt), "1", DATA_TYPE_STRING);
	//	xing::set_packet_data(CFOAQ50600InBlock1.AcntNo, sizeof(CFOAQ50600InBlock1.AcntNo), strFuOpAccNo.c_str(), DATA_TYPE_STRING);
	//	xing::set_packet_data(CFOAQ50600InBlock1.InptPwd, sizeof(CFOAQ50600InBlock1.InptPwd), strFuOpAccPW.c_str(), DATA_TYPE_STRING);
	//	xing::set_packet_data(CFOAQ50600InBlock1.OrdDt, sizeof(CFOAQ50600InBlock1.OrdDt), _pOrdDate.data(), DATA_TYPE_STRING);
	//	// 0 기본설정
	//	// 1 이동평균법
	//	// 2 선입선출법
	//	xing::set_packet_data(CFOAQ50600InBlock1.BalEvalTp, sizeof(CFOAQ50600InBlock1.BalEvalTp), "0", DATA_TYPE_STRING);
	//	// 1 당초가
	//	// 2 전일종가
	//	xing::set_packet_data(CFOAQ50600InBlock1.FutsPrcEvalTp, sizeof(CFOAQ50600InBlock1.FutsPrcEvalTp), "1", DATA_TYPE_STRING);
	//	// 1 청산수량산출
	//	xing::set_packet_data(CFOAQ50600InBlock1.LqdtQtyQryTp, sizeof(CFOAQ50600InBlock1.LqdtQtyQryTp), "1", DATA_TYPE_STRING);

	//	this->RequestWait("CFOAQ56000", &CFOAQ50600InBlock1, sizeof(_CFOAQ50600InBlock1));
	//}
	// 선물옵션 계좌 미결제 약정현황(평균가)	"CFOFQ02400"
	inline void req_option_misung()
	{
		if (strFuOpAccNo.empty() || strFuOpAccPW.empty()) { return; }
		_CFOFQ02400InBlock1 CFOFQ02400InBlock;
		dk::init(CFOFQ02400InBlock, ' ');

		xing::set_packet_data(CFOFQ02400InBlock.RecCnt, sizeof(CFOFQ02400InBlock.RecCnt), "1", DATA_TYPE_STRING);
		xing::set_packet_data(CFOFQ02400InBlock.AcntNo, sizeof(CFOFQ02400InBlock.AcntNo), strFuOpAccNo.c_str(), DATA_TYPE_STRING);
		xing::set_packet_data(CFOFQ02400InBlock.Pwd, sizeof(CFOFQ02400InBlock.Pwd), strFuOpAccPW.c_str(), DATA_TYPE_STRING);
		// 99 전체
		// 40 KOSPI
		// 20 KOSDAQ
		// 10 KSE
		// 50 KOFEX
		xing::set_packet_data(CFOFQ02400InBlock.RegMktCode, sizeof(CFOFQ02400InBlock.RegMktCode), "99", DATA_TYPE_STRING);
		xing::set_packet_data(CFOFQ02400InBlock.BuyDt, sizeof(CFOFQ02400InBlock.BuyDt), "", DATA_TYPE_STRING);

		this->RequestWait("CFOFQ02400", &CFOFQ02400InBlock, sizeof(_CFOFQ02400InBlock1));
	}
	// 선물옵션 계좌예탁금증거금조회 "CFOBQ10500"
	//typedef struct _CFOBQ10500InBlock1
	//{
	//	char    RecCnt[5];    // [long  ,    5] 레코드갯수                      StartPos 0, Length 5
	//	char    AcntNo[20];    // [string,   20] 계좌번호                        StartPos 5, Length 20
	//	char    Pwd[8];    // [string,    8] 비밀번호                        StartPos 25, Length 8
	//} CFOBQ10500InBlock1, * LPCFOBQ10500InBlock1;
	inline void req_option_deposit()
	{
		if (strFuOpAccNo.empty() || strFuOpAccPW.empty()) { return; }
		_CFOBQ10500InBlock1 CFOBQ10500InBlock;
		dk::init(CFOBQ10500InBlock, ' ');
		xing::set_packet_data(CFOBQ10500InBlock.RecCnt, sizeof(CFOBQ10500InBlock.RecCnt), "1", DATA_TYPE_STRING);
		xing::set_packet_data(CFOBQ10500InBlock.AcntNo, sizeof(CFOBQ10500InBlock.AcntNo), strFuOpAccNo.c_str(), DATA_TYPE_STRING);
		xing::set_packet_data(CFOBQ10500InBlock.Pwd, sizeof(CFOBQ10500InBlock.Pwd), strFuOpAccPW.c_str(), DATA_TYPE_STRING);
		this->RequestWait("CFOBQ10500", &CFOBQ10500InBlock, sizeof(_CFOBQ10500InBlock1));
	}
};

#pragma pack( pop )
