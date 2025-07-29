#include "framework.h"
#include "CXingAPI.h"


namespace
{
	thread_local char szResultBuffer[1024]{};
}

HWND IXingAPI::hDefaultWnd = nullptr;
IXingAPI::IXingAPI(HWND _hWnd)
	: m_reqEvent(FALSE)
{
	if (_hWnd) { this->Initialize(_hWnd); }
}

IXingAPI::~IXingAPI()
{
	this->Destroy();
}

bool IXingAPI::init_functions()
{
	BOOL bResult = false;
	int nCurrentLine = __LINE__;
	do
	{
		nCurrentLine = __LINE__; if (!this->hXingAPI)
		{
			DBGPRINT("hXingAPI == 0");
			break;
		}
		this->m_fpConnect = (FP_CONNECT)dk::get_proc_address(this->hXingAPI, "ETK_Connect");
		nCurrentLine = __LINE__; if (!this->m_fpConnect) { break; }
		this->m_fpIsConnected = (FP_ISCONNECTED)dk::get_proc_address(this->hXingAPI, "ETK_IsConnected");
		nCurrentLine = __LINE__; if (!this->m_fpIsConnected) { break; }
		this->m_fpDisconnect = (FP_DISCONNECT)dk::get_proc_address(this->hXingAPI, "ETK_Disconnect");
		nCurrentLine = __LINE__; if (!this->m_fpDisconnect) { break; }
		this->m_fpLogin = (FP_LOGIN)dk::get_proc_address(this->hXingAPI, "ETK_Login");
		nCurrentLine = __LINE__; if (!this->m_fpLogin) { break; }
		this->m_fpLogout = (FP_LOGOUT)dk::get_proc_address(this->hXingAPI, "ETK_Logout");
		nCurrentLine = __LINE__; if (!this->m_fpLogout) { break; }

		this->m_fpGetLastError = (FP_GETLASTERROR)dk::get_proc_address(this->hXingAPI, "ETK_GetLastError");
		nCurrentLine = __LINE__; if (!this->m_fpGetLastError) { break; }
		this->m_fpGetErrorMessage = (FP_GETERRORMESSAGE)dk::get_proc_address(this->hXingAPI, "ETK_GetErrorMessage");
		nCurrentLine = __LINE__; if (!this->m_fpGetErrorMessage) { break; }

		this->m_fpRequest = (FP_REQUEST)dk::get_proc_address(this->hXingAPI, "ETK_Request");
		nCurrentLine = __LINE__; if (!this->m_fpRequest) { break; }
		this->m_fpAdviseRealData = (FP_ADVISEREALDATA)dk::get_proc_address(this->hXingAPI, "ETK_AdviseRealData");
		nCurrentLine = __LINE__; if (!this->m_fpAdviseRealData) { break; }
		this->m_fpUnadviseRealData = (FP_UNADVISEREALDATA)dk::get_proc_address(this->hXingAPI, "ETK_UnadviseRealData");
		nCurrentLine = __LINE__; if (!this->m_fpUnadviseRealData) { break; }
		this->m_fpUnadviseWindow = (FP_UNADVISEWINDOW)dk::get_proc_address(this->hXingAPI, "ETK_UnadviseWindow");
		nCurrentLine = __LINE__; if (!this->m_fpUnadviseWindow) { break; }
		this->m_fpReleaseRequestData = (FP_RELEASEREQUESTDATA)dk::get_proc_address(this->hXingAPI, "ETK_ReleaseRequestData");
		nCurrentLine = __LINE__; if (!this->m_fpReleaseRequestData) { break; }
		this->m_fpReleaseMessageData = (FP_RELEASEMESSAGEDATA)dk::get_proc_address(this->hXingAPI, "ETK_ReleaseMessageData");
		nCurrentLine = __LINE__; if (!this->m_fpReleaseMessageData) { break; }

		this->m_fpGetAccountListCount = (FP_GETACCOUNTLISTCOUNT)dk::get_proc_address(this->hXingAPI, "ETK_GetAccountListCount");
		nCurrentLine = __LINE__; if (!this->m_fpGetAccountListCount) { break; }
		this->m_fpGetAccountList = (FP_GETACCOUNTLIST)dk::get_proc_address(this->hXingAPI, "ETK_GetAccountList");
		nCurrentLine = __LINE__; if (!this->m_fpGetAccountList) { break; }
		this->m_fpGetAccountName = (FP_GETACCOUNTNAME)dk::get_proc_address(this->hXingAPI, "ETK_GetAccountName");
		nCurrentLine = __LINE__; if (!this->m_fpGetAccountName) { break; }
		this->m_fpGetAcctDetailName = (FP_GETACCTDETAILNAME)dk::get_proc_address(this->hXingAPI, "ETK_GetAcctDetailName");
		nCurrentLine = __LINE__; if (!this->m_fpGetAcctDetailName) { break; }
		this->m_fpGetAcctNickname = (FP_GETACCTNICKNAME)dk::get_proc_address(this->hXingAPI, "ETK_GetAcctNickname");
		nCurrentLine = __LINE__; if (!this->m_fpGetAcctNickname) { break; }

		this->m_fpGetCommMedia = (FP_GETCOMMMEDIA)dk::get_proc_address(this->hXingAPI, "ETK_GetCommMedia");
		nCurrentLine = __LINE__; if (!this->m_fpGetCommMedia) { break; }
		this->m_fpGetETKMedia = (FP_GETETKMEDIA)dk::get_proc_address(this->hXingAPI, "ETK_GetETKMedia");
		nCurrentLine = __LINE__; if (!this->m_fpGetETKMedia) { break; }
		this->m_fpGetClientIP = (FP_GETCLIENTIP)dk::get_proc_address(this->hXingAPI, "ETK_GetClientIP");
		nCurrentLine = __LINE__; if (!this->m_fpGetClientIP) { break; }
		this->m_fpGetServerName = (FP_GETSERVERNAME)dk::get_proc_address(this->hXingAPI, "ETK_GetServerName");
		nCurrentLine = __LINE__; if (!this->m_fpGetServerName) { break; }
		this->m_fpGetAPIPath = (FP_GETAPIPATH)dk::get_proc_address(this->hXingAPI, "ETK_GetAPIPath");
		nCurrentLine = __LINE__; if (!this->m_fpGetAPIPath) { break; }

		this->m_fpSetHeaderInfo = (FP_SETHEADERINFO)dk::get_proc_address(this->hXingAPI, "ETK_SetHeaderInfo");
		nCurrentLine = __LINE__; if (!this->m_fpSetHeaderInfo) { break; }
		this->m_fpSetUseAPIVer = (FP_SETUSEAPIVER)dk::get_proc_address(this->hXingAPI, "ETK_SetUseAPIVer");
		nCurrentLine = __LINE__; if (!this->m_fpSetUseAPIVer) { break; }
		this->m_fpSetMode = (FP_SETMODE)dk::get_proc_address(this->hXingAPI, "ETK_SetMode");
		nCurrentLine = __LINE__; if (!this->m_fpSetMode) { break; }

		this->m_fpGetProcBranchNo = (FP_GETPROCBRANCHNO)dk::get_proc_address(this->hXingAPI, "ETK_GetProcBranchNo");
		nCurrentLine = __LINE__; if (!this->m_fpGetProcBranchNo) { break; }
		this->m_fpGetUseOverFuture = (FP_GETUSEOVERFUTURE)dk::get_proc_address(this->hXingAPI, "ETK_GetUseOverFuture");
		nCurrentLine = __LINE__; if (!this->m_fpGetUseOverFuture) { break; }
		this->m_fpGetUseOverStock = (FP_GETUSEOVERSTOCK)dk::get_proc_address(this->hXingAPI, "ETK_GetUseOverStock");
		nCurrentLine = __LINE__; if (!this->m_fpGetUseOverStock) { break; }
		this->m_fpGetUseFX = (FP_GETUSEFX)dk::get_proc_address(this->hXingAPI, "ETK_GetUseFX");
		nCurrentLine = __LINE__; if (!this->m_fpGetUseFX) { break; }

		this->m_fpGetTRCountPerSec = (FP_GETTRCOUNTPERSEC)dk::get_proc_address(this->hXingAPI, "ETK_GetTRCountPerSec");
		nCurrentLine = __LINE__; if (!this->m_fpGetTRCountPerSec) { break; }
		this->m_fpGetTRCountBaseSec = (FP_GETTRCOUNTBASESEC)dk::get_proc_address(this->hXingAPI, "ETK_GetTRCountBaseSec");
		nCurrentLine = __LINE__; if (!this->m_fpGetTRCountBaseSec) { break; }
		this->m_fpGetTRCountRequest = (FP_GETTRCOUNTREQUEST)dk::get_proc_address(this->hXingAPI, "ETK_GetTRCountRequest");
		nCurrentLine = __LINE__; if (!this->m_fpGetTRCountRequest) { break; }
		this->m_fpGetTRCountLimit = (FP_GETTRCOUNTLIMIT)dk::get_proc_address(this->hXingAPI, "ETK_GetTRCountLimit");
		nCurrentLine = __LINE__; if (!this->m_fpGetTRCountLimit) { break; }

		this->m_fpSetNotifyFlag = (FP_SETNOTIFYFLAG)dk::get_proc_address(this->hXingAPI, "ETK_SetNotifyFlag");
		nCurrentLine = __LINE__; if (!this->m_fpSetNotifyFlag) { break; }

		this->m_fpRequestService = (FP_REQUESTSERVICE)dk::get_proc_address(this->hXingAPI, "ETK_RequestService");
		nCurrentLine = __LINE__; if (!this->m_fpRequestService) { break; }
		this->m_fpRemoveService = (FP_REMOVESERVICE)dk::get_proc_address(this->hXingAPI, "ETK_RemoveService");
		nCurrentLine = __LINE__; if (!this->m_fpRemoveService) { break; }

		this->m_fpRequestLinkToHTS = (FP_REQUESTLINKTOHTS)dk::get_proc_address(this->hXingAPI, "ETK_RequestLinkToHTS");
		nCurrentLine = __LINE__; if (!this->m_fpRequestLinkToHTS) { break; }
		this->m_fpAdviseLinkFromHTS = (FP_ADVISELINKFROMHTS)dk::get_proc_address(this->hXingAPI, "ETK_AdviseLinkFromHTS");
		nCurrentLine = __LINE__; if (!this->m_fpAdviseLinkFromHTS) { break; }
		this->m_fpUnAdviseLinkFromHTS = (FP_UNADVISELINKFROMHTS)dk::get_proc_address(this->hXingAPI, "ETK_UnAdviseLinkFromHTS");
		nCurrentLine = __LINE__; if (!this->m_fpUnAdviseLinkFromHTS) { break; }

		this->m_fpDecompress = (FP_DECOMPRESS)dk::get_proc_address(this->hXingAPI, "ETK_Decompress");
		nCurrentLine = __LINE__; if (!this->m_fpDecompress) { break; }
		this->m_fpIsChartLib = (FP_ISCHARTLIB)dk::get_proc_address(this->hXingAPI, "ETK_IsChartLib");
		nCurrentLine = __LINE__; if (!this->m_fpIsChartLib) { break; }

		this->m_fpSetProgramOrder = (FP_SETPROGRAMORDER)dk::get_proc_address(this->hXingAPI, "ETK_SetProgramOrder");
		nCurrentLine = __LINE__; if (!this->m_fpSetProgramOrder) { break; }
		this->m_fpGetProgramOrder = (FP_GETPROGRAMORDER)dk::get_proc_address(this->hXingAPI, "ETK_GetProgramOrder");
		nCurrentLine = __LINE__; if (!this->m_fpGetProgramOrder) { break; }
		bResult = true;
	} while (false);
	return(bResult);
}

void IXingAPI::Initialize(HWND _hWnd)
{
	hDefaultWnd = _hWnd;
	DBGPRINT(L"IXingAPI::Initialize() hWnd: %x", hDefaultWnd);
	wchar_t szCommLib[_MAX_PATH]{};
	LPCWSTR pCurrentPath = dk::GetCurrentDirectoryW(szCommLib);
	// \\XingAPI 폴더로 세팅.
	::wcscat(szCommLib, L"XingAPI\\");
	::SetCurrentDirectoryW(szCommLib);
	// 파일 경로를 완성하고
	::wcscat(szCommLib, L"XingAPI.dll");
	this->hXingAPI = ::LoadLibraryW(szCommLib);
	//DBGPRINT(L"hWnd: %x, hXingAPI: %x, Path: %s, %d", this->hDefaultWnd, this->hXingAPI, szCommLib, ::wcslen(szCommLib));
	// 현재 디렉토리를 원래대로 되돌림.
	::SetCurrentDirectoryW(pCurrentPath);
	if (this->hXingAPI)
	{
		if (!this->init_functions())
		{
			DBGPRINT("init_functions failed");
			// 예외 발생시킨다
			throw std::runtime_error("Failed to initialize Xing API functions.");
		}
	}
	else
	{
		DBGPRINT("LoadLibraryW failed: %d", ::GetLastError());
	}
}

void IXingAPI::Destroy()
{
	if (this->hXingAPI)
	{
		::FreeLibrary(this->hXingAPI);
	}
}

BOOL IXingAPI::Connect(const BOOL _bMockServer, const int _nStartMsgID)
{
	constexpr char szRealServer[] = "api.ls-sec.co.kr";
	constexpr char szMockServer[] = "demo.ls-sec.co.kr";
	constexpr WORD nRealPort = 20001;
	constexpr WORD nMockPort = 80;

	LPCSTR szServer = (!_bMockServer) ? szRealServer : szMockServer;
	const WORD nPort = (!_bMockServer) ? nRealPort : nMockPort;

	const BOOL bConnect = this->Connect(szServer, nPort, _nStartMsgID);
	if (!bConnect)
	{
		const int nErrorCode = this->GetLastError();
		LPCSTR strMsg = this->GetErrorMessage(nErrorCode);
		DBGPRINT("XING: %s - 접속 실패: %s", szServer, strMsg);
	}
	else
	{
		this->bMockServer = _bMockServer;
		DBGPRINT("XING: %s - 접속 성공", szServer);
	}
	return(bConnect);
}

BOOL IXingAPI::Login(LPCSTR _pID, LPCSTR _pPW, LPCSTR _pCR)
{
	const BOOL bRequestLogin = this->Login(_pID, _pPW
		, (!this->bMockServer) ? _pCR : ""
		, this->bMockServer			// 0 : 실서버, 1 : 모의투자서버
	);
	if (!bRequestLogin)
	{
		const int nErrorCode = this->GetLastError();
		LPCSTR strMsg = this->GetErrorMessage(nErrorCode);
		DBGPRINT("XING: %s - 로그인 요청 실패: %s", _pID, strMsg);
		this->Disconnect();
	}
	//else
	//{
	//	DBGPRINT("XING: %s - 로그인 요청 성공", _pID);
	//}
	// 여기에선 요청을 한거고, 로그인에 성공하면 XM_LOGIN 을 XingAPI 에서 콜백으로 준다.
	return(bRequestLogin);
}

BOOL IXingAPI::Connect(LPCSTR _pszSvrIP, const int _nPort
	, const int _nStartMsgID, const int _nSendMaxPacketSize, const int _nTimeOut, const HWND _hWnd)
{
	BOOL bResult = FALSE;
	if (this->m_fpConnect)
	{
		//DBGPRINT("%s: %s:%d, StartMsgID: %d, TimeOut: %d, MaxPacketSize: %d, hWnd: %x"
		//	, __FUNCTION__
		//	, _pszSvrIP, _nPort, _nStartMsgID, _nTimeOut, _nSendMaxPacketSize, _hWnd
		//);
		bResult = this->m_fpConnect(_hWnd, _pszSvrIP, _nPort, _nStartMsgID, _nTimeOut, _nSendMaxPacketSize);
		if (bResult)
		{	// 접속하면 1 이다.
			this->nStatusXingAPI = 1;
		}
	}
	return(bResult);
}

BOOL IXingAPI::IsConnected()
{
	BOOL bResult = FALSE;
	if (this->m_fpIsConnected)
	{
		bResult = this->m_fpIsConnected();
		if (!bResult)
		{
			this->nStatusXingAPI = 0;
		}
	}
	return(bResult);
}

BOOL IXingAPI::Disconnect()
{
	BOOL bResult = FALSE;
	if (this->m_fpDisconnect)
	{
		if (this->IsConnected())
		{	// vSubscribeInfos 를 모두 해제한다.
			for (auto& itr : this->vSubscribeInfos)
			{
				//const BOOL bUnReg =
					this->UnadviseRealData(itr.first.c_str(), itr.second.szData.c_str(), itr.second.nDataUnitLen);
				//if (bUnReg)
				//{
				//	DBGPRINT("Unsubscribe(%d) - key: %s. data: %s", bUnReg, itr.first.c_str(), itr.second.szData.c_str());
				//}
			}
			bResult = this->m_fpDisconnect();
			if (bResult) { this->nStatusXingAPI = 0; }
		}
		
	}
	return(bResult);
}

BOOL IXingAPI::Login(LPCSTR pszID, LPCSTR pszPwd, LPCSTR pszCertPwd
	, const int _bytType, const BOOL bShowCertErrDlg, const HWND _hWnd)
{
	BOOL bResult = FALSE;
	if (this->m_fpLogin)
	{
		if (1 == this->nStatusXingAPI)
		{	// Connect 가 되면 1 이다.
			//DBGPRINT("%s: ID: %s, Type: %d, ShowCertErrDlg: %d, hWnd: %x"
			//	, __FUNCTION__, pszID, _bytType, bShowCertErrDlg, _hWnd
			//);
			bResult = m_fpLogin(_hWnd, pszID, pszPwd, pszCertPwd, _bytType, bShowCertErrDlg);
			if (bResult)
			{	// 로그인 요청하면 2 가 된다.
				this->nStatusXingAPI = 2;
			}
		}
	}
	return(bResult);
}

BOOL IXingAPI::Logout(HWND _hWnd)
{
	DBGPRINT("%s", __FUNCTION__);
	BOOL bResult = FALSE;
	if (this->m_fpLogout)
	{
		bResult = this->m_fpLogout(_hWnd);
		if (bResult)
		{
			this->nStatusXingAPI = 1;
		}
	}
	return(bResult);
}

int	IXingAPI::GetLastError() const
{
	if (this->m_fpGetLastError) { return this->m_fpGetLastError(); }
	return(0);
}

LPCSTR	IXingAPI::GetErrorMessage(int _nErrorCode) const
{
	szResultBuffer[0] = '\0'; // Clear the buffer before use
	if (this->m_fpGetErrorMessage) { this->m_fpGetErrorMessage(_nErrorCode, szResultBuffer, sizeof(szResultBuffer)); }
	return szResultBuffer;
}

int IXingAPI::Request(LPCSTR _pTrCode, const LPVOID lpData, const int nDataSize
	, const BOOL bNext, LPCSTR pszNextKey, const int nTimeOut, const HWND _hWnd)
{
	if (this->m_fpRequest)
	{
		// 1. 타이머 체커 가져오기/생성
		xing::C_TR_TIME_CHECKER* pChecker = nullptr;
		auto itr = m_trTimers.find(_pTrCode);
		if (itr != m_trTimers.end())
		{
			pChecker = &itr->second;
			//DBGPRINT("기존 타이머 사용: %s", _pTrCode);
		}
		else
		{
			// 새 타이머 생성
			const int nPerSec = this->GetTRCountPerSec(_pTrCode);
			const int nBaseSec = this->GetTRCountBaseSec(_pTrCode);
			const int nLimit = this->GetTRCountLimit(_pTrCode);

			const auto result = m_trTimers.try_emplace(_pTrCode, nPerSec, nBaseSec, nLimit);
			pChecker = &result.first->second;

			//DBGPRINT("새 타이머 생성: %s - PerSec:%d, BaseSec:%d, Limit:%d", _pTrCode, nPerSec, nBaseSec, nLimit);
		}

		// 2. 간격 체크
		if (!pChecker->checkInterval())
		{
			const unsigned long waitTime = pChecker->GetTimeUntilNextCall();
			DBGPRINT("간격 제한으로 요청 거부: %s (대기시간: %lu ms)", _pTrCode, waitTime);
			return -1;
		}

		// 3. 10분 제한 사전 체크 (가능한 경우)
		if (pChecker->GetLimit() > 0)
		{
			const int currentCount = this->GetTRCountRequest(_pTrCode);
			if (currentCount >= 0 && currentCount >= pChecker->GetLimit())
			{
				DBGPRINT("10분 제한 초과로 요청 거부: %s (현재:%d, 제한:%d)", _pTrCode, currentCount, pChecker->GetLimit());
				return -1;
			}
		}
		// 4. 실제 API 호출
		//DBGPRINT("TR 요청 실행: %s (hWnd: %p, bNext:%d, NextKey:%s, TimeOut:%d)", _pTrCode, _hWnd, bNext, pszNextKey ? pszNextKey : "", nTimeOut);
		const int result = this->m_fpRequest(_hWnd, _pTrCode, lpData, nDataSize, bNext, pszNextKey, nTimeOut);
		//DBGPRINT("inline Request() 완료 - result: %d, hWnd: %p", result, _hWnd);
		// 5. 호출 후 상태 업데이트
		if (result >= 0) // 성공한 경우만
		{
			pChecker->startTimer(); // 타이머 시작

			// 10분 제한 TR인 경우 현재 카운트로 상태 업데이트
			if (pChecker->GetLimit() > 0)
			{
				const int updatedCount = this->GetTRCountRequest(_pTrCode);
				if (updatedCount >= 0)
				{
					pChecker->updateLimitStatus(updatedCount);
					DBGPRINT("10분 제한 상태 업데이트: %s (현재:%d/%d, 제한초과:%s)", _pTrCode, updatedCount, pChecker->GetLimit(), pChecker->IsLimitExceeded() ? "예" : "아니오");
				}
			}
			return(result);
		}
	}
	return(-1);
}

int IXingAPI::RequestWait(LPCSTR _pTrCode, const LPVOID lpData, const int nDataSize, const HWND _hWnd)
{
	if (!this->m_fpRequest || !_pTrCode) return -1;

	// 1. 타이머 체커 가져오기/생성
	xing::C_TR_TIME_CHECKER* pChecker = nullptr;
	auto itr = m_trTimers.find(_pTrCode);

	if (itr != m_trTimers.end())
	{
		pChecker = &itr->second;
		//DBGPRINT("기존 타이머 사용 (Force): %s", _pTrCode);
	}
	else
	{
		// 새 타이머 생성 (GetTRCountRequest는 사용하지 않음)
		const int nPerSec = this->GetTRCountPerSec(_pTrCode);
		const int nBaseSec = this->GetTRCountBaseSec(_pTrCode);
		const int nLimit = this->GetTRCountLimit(_pTrCode);

		auto result = m_trTimers.try_emplace(_pTrCode, nPerSec, nBaseSec, nLimit);
		pChecker = &result.first->second;

		//DBGPRINT("새 타이머 생성 (Force): %s - PerSec:%d, BaseSec:%d, Limit:%d", _pTrCode, nPerSec, nBaseSec, nLimit);
	}
	//DBGPRINT("TR 요청 대기 시작: %s", _pTrCode);
	// 2. 10분 제한 사전 체크 - Force라도 불가능한 경우는 처리
	if (pChecker->GetLimit() > 0)
	{
		const int currentCount = this->GetTRCountRequest(_pTrCode);
		if (currentCount >= 0 && currentCount >= pChecker->GetLimit())
		{
			if (pChecker->IsLimitExceeded())
			{
				// 이미 제한 초과 상태 - 10분 대기는 비현실적이므로 실패
				DBGPRINT("10분 제한 초과로 Force 요청도 실패: %s (현재:%d, 제한:%d)", _pTrCode, currentCount, pChecker->GetLimit());
				return -1;
			}
		}
	}
	//DBGPRINT("10분 제한 범위 내: %s (현재:%d, 제한:%d)", _pTrCode, pChecker->GetLimit() > 0 ? this->GetTRCountRequest(_pTrCode) : 0, pChecker->GetLimit());
	// 3. 간격 체크 및 대기
	while (!pChecker->checkInterval())
	{
		const unsigned long waitTime = pChecker->GetTimeUntilNextCall();
		//DBGPRINT("간격 제한으로 대기: %s (대기시간: %lu ms)", _pTrCode, waitTime);

		if (waitTime > 0)
		{
			m_reqEvent.Reset();
			m_reqEvent.Wait(waitTime);
		}
		else
		{
			break; // 대기시간이 0이면 루프 탈출
		}
	}

	// 4. 실제 API 호출
	//DBGPRINT("TR Force 요청 실행: %s", _pTrCode);

	const int result = this->m_fpRequest(_hWnd, _pTrCode, lpData, nDataSize, FALSE, "", 30);

	// 5. 호출 후 상태 업데이트
	if (result >= 0) // 성공한 경우만
	{
		pChecker->startTimer(); // 타이머 시작

		// 10분 제한 TR인 경우 현재 카운트로 상태 업데이트
		if (pChecker->GetLimit() > 0)
		{
			const int updatedCount = this->GetTRCountRequest(_pTrCode);
			if (updatedCount >= 0)
			{
				pChecker->updateLimitStatus(updatedCount);
				DBGPRINT("10분 제한 상태 업데이트 (Force): %s (현재:%d/%d, 제한초과:%s)",
					_pTrCode, updatedCount, pChecker->GetLimit(),
					pChecker->IsLimitExceeded() ? "예" : "아니오");
			}
		}
	}
	else
	{
		DBGPRINT("TR Force 요청 실패: %s (결과 코드: %d)", _pTrCode, result);
	}
	return result;
}

int IXingAPI::RequestForce(LPCSTR _pTrCode, const LPVOID lpData, const int nDataSize, const HWND _hWnd)
{
	if (!this->m_fpRequest || !_pTrCode) return -1;

	// 1. 타이머 체커 가져오기/생성
	xing::C_TR_TIME_CHECKER* pChecker = nullptr;
	auto itr = m_trTimers.find(_pTrCode);

	if (itr != m_trTimers.end())
	{
		pChecker = &itr->second;
		DBGPRINT("기존 타이머 사용 (ForceWithLimitWait): %s", _pTrCode);
	}
	else
	{
		// 새 타이머 생성 (GetTRCountRequest는 사용하지 않음)
		const int nPerSec = this->GetTRCountPerSec(_pTrCode);
		const int nBaseSec = this->GetTRCountBaseSec(_pTrCode);
		const int nLimit = this->GetTRCountLimit(_pTrCode);

		auto result = m_trTimers.try_emplace(_pTrCode, nPerSec, nBaseSec, nLimit);
		pChecker = &result.first->second;

		DBGPRINT("새 타이머 생성 (ForceWithLimitWait): %s - PerSec:%d, BaseSec:%d, Limit:%d",
			_pTrCode, nPerSec, nBaseSec, nLimit);
	}

	// 2. 10분 제한 대기 로직
	if (pChecker->GetLimit() > 0)
	{
		while (true)
		{
			const int currentCount = this->GetTRCountRequest(_pTrCode);
			if (currentCount < 0 || currentCount < pChecker->GetLimit())
			{
				break; // 제한 범위 내
			}

			// 10분 제한 초과 - 대기
			const unsigned long limitWaitTime = 600000; // 10분 = 600초
			DBGPRINT("10분 제한 초과로 대기: %s (대기시간: %lu ms)", _pTrCode, limitWaitTime);

			m_reqEvent.Reset();
			m_reqEvent.Wait(limitWaitTime);
		}
	}

	// 3. 간격 체크 및 대기
	while (!pChecker->checkInterval())
	{
		const unsigned long waitTime = pChecker->GetTimeUntilNextCall();
		if (waitTime > 0)
		{
			//DBGPRINT("간격 제한으로 대기: %s (대기시간: %lu ms)", _pTrCode, waitTime);

			m_reqEvent.Reset();
			m_reqEvent.Wait(waitTime);
		}
		else
		{
			break;
		}
	}

	// 4. 실제 API 호출
	//DBGPRINT("TR ForceWithLimitWait 요청 실행: %s", _pTrCode);

	const int result = this->m_fpRequest(_hWnd, _pTrCode, lpData, nDataSize, FALSE, "", 30);

	// 5. 호출 후 상태 업데이트
	if (result >= 0) // 성공한 경우만
	{
		pChecker->startTimer(); // 타이머 시작

		// 10분 제한 TR인 경우 현재 카운트로 상태 업데이트
		if (pChecker->GetLimit() > 0)
		{
			const int updatedCount = this->GetTRCountRequest(_pTrCode);
			if (updatedCount >= 0)
			{
				pChecker->updateLimitStatus(updatedCount);
				DBGPRINT("10분 제한 상태 업데이트 (ForceWithLimitWait): %s (현재:%d/%d, 제한초과:%s)",
					_pTrCode, updatedCount, pChecker->GetLimit(),
					pChecker->IsLimitExceeded() ? "예" : "아니오");
			}
		}
	}
	return result;
}

BOOL IXingAPI::AdviseRealData(LPCSTR pszTrNo, LPCSTR pszData, const int nDataUnitLen, const HWND _hWnd)
{
	BOOL bReg = FALSE;
	if (m_fpAdviseRealData)
	{
		//DBGPRINT("AdviseRealData - key: %s, data: %s, nDataUnitLen: %d, hWnd: %x", pszTrNo, pszData ? pszData : "", nDataUnitLen, _hWnd);
		if (!is_reg_sub_info(pszTrNo, pszData))
		{
			bReg = m_fpAdviseRealData(_hWnd, pszTrNo, pszData, nDataUnitLen);
			if (bReg)
			{
				//DBGPRINT("AdviseRealData success - key: %s, data: %s, nDataUnitLen: %d, hWnd: %x", pszTrNo, pszData ? pszData : "", nDataUnitLen, _hWnd);
				this->reg_sub_info(pszTrNo, pszData, nDataUnitLen);
				DBGPRINT("Subscribe(%d) - key: %s, data: %s, %d, %p, %p", bReg, pszTrNo, pszData ? pszData : "", _hWnd, nDataUnitLen, hDefaultWnd);
			}
			
		}
		//else
		//{
		//	DBGPRINT("AdviseRealData failed - key: %s, data: %s, nDataUnitLen: %d, hWnd: %x", pszTrNo, pszData ? pszData : "", nDataUnitLen, _hWnd);
		//}
	}
	return(bReg);
}

BOOL IXingAPI::UnadviseRealData(LPCSTR pszTrNo, LPCSTR pszData, const int nDataUnitLen, const HWND _hWnd) const
{
	DBGPRINT("UnadviseRealData - key: %s, data: %s, nDataUnitLen: %d, hWnd: %x", pszTrNo, pszData ? pszData : "", nDataUnitLen, _hWnd);
	if (m_fpUnadviseRealData) { return m_fpUnadviseRealData(_hWnd, pszTrNo, pszData, nDataUnitLen); }
	return(FALSE);
}

BOOL IXingAPI::UnadviseWindow(const HWND _hWnd) const
{
	DBGPRINT("UnadviseWindow - hWnd: %x", _hWnd);
	if (m_fpUnadviseWindow) { return m_fpUnadviseWindow(_hWnd); }
	return(FALSE);
}

BOOL IXingAPI::ReleaseRequestData(int nRequestID) const
{
	BOOL bResult = FALSE;
	if (m_fpReleaseRequestData)
	{
		m_fpReleaseRequestData(nRequestID);
		bResult = TRUE;
	}
	return(bResult);
}

BOOL IXingAPI::ReleaseMessageData(LPARAM lp) const
{
	BOOL bResult = FALSE;
	if (m_fpReleaseMessageData) { m_fpReleaseMessageData(lp); bResult = TRUE; }
	return(bResult);
}

int	IXingAPI::GetAccountListCount() const
{
	if (m_fpGetAccountListCount) { return m_fpGetAccountListCount(); }
	return 0;
}

BOOL IXingAPI::GetAccountList(int nIndex, LPSTR pszAcc, int nAccSize) const
{
	if (m_fpGetAccountList) { return m_fpGetAccountList(nIndex, pszAcc, nAccSize); }
	return(FALSE);
}

BOOL IXingAPI::GetAccountName(LPCSTR pszAcc, LPSTR pszAccName, int nAccNameSize) const
{
	if (m_fpGetAccountName) { return m_fpGetAccountName(pszAcc, pszAccName, nAccNameSize); }
	return(FALSE);
}

BOOL IXingAPI::GetAcctDetailName(LPCSTR pszAcc, LPSTR pszAcctDetailName, int nAcctDetailNameSize) const
{
	if (m_fpGetAcctDetailName) { return m_fpGetAcctDetailName(pszAcc, pszAcctDetailName, nAcctDetailNameSize); }
	return(FALSE);
}

BOOL IXingAPI::GetAcctNickname(LPCSTR pszAcc, LPSTR pszAcctNickname, int nAcctNicknameSize) const
{
	if (m_fpGetAcctNickname) { return m_fpGetAcctNickname(pszAcc, pszAcctNickname, nAcctNicknameSize); }
	return(FALSE);
}

LPCSTR	IXingAPI::GetCommMedia() const
{
	szResultBuffer[0] = '\0'; // Clear the buffer before use
	if (m_fpGetCommMedia) { m_fpGetCommMedia(szResultBuffer); }
	return szResultBuffer;
}

LPCSTR	IXingAPI::GetETKMedia() const
{
	szResultBuffer[0] = '\0'; // Clear the buffer before use
	if (m_fpGetETKMedia) { m_fpGetETKMedia(szResultBuffer); }
	return szResultBuffer;
}

LPCSTR	IXingAPI::GetClientIP() const
{
	szResultBuffer[0] = '\0'; // Clear the buffer before use
	if (m_fpGetClientIP) { m_fpGetClientIP(szResultBuffer); }
	return szResultBuffer;
}

LPCSTR	IXingAPI::GetServerName() const
{
	szResultBuffer[0] = '\0'; // Clear the buffer before use
	if (m_fpGetServerName) { m_fpGetServerName(szResultBuffer); }
	return szResultBuffer;
}

LPCSTR	IXingAPI::GetAPIPath() const
{
	szResultBuffer[0] = '\0'; // Clear the buffer before use
	if (m_fpGetAPIPath) { m_fpGetAPIPath(szResultBuffer); }
	return szResultBuffer;
}

void IXingAPI::SetHeaderInfo(LPCSTR szType, LPCSTR szValue)
{
	if (m_fpSetHeaderInfo) { m_fpSetHeaderInfo(szType, szValue); }
}

void IXingAPI::SetUseAPIVer(LPCSTR szUserAPIVer)
{
	if (m_fpSetUseAPIVer) { m_fpSetUseAPIVer(szUserAPIVer); }
}

BOOL IXingAPI::SetMode(LPCSTR pszMode, LPCSTR pszValue)
{
	BOOL bResult = FALSE;
	if (m_fpSetMode)
	{
		m_fpSetMode(pszMode, pszValue);
		bResult = TRUE;
	}
	return(bResult);
}

LPCSTR	IXingAPI::GetProcBranchNo() const
{
	szResultBuffer[0] = '\0'; // Clear the buffer before use
	if (m_fpGetProcBranchNo) { m_fpGetProcBranchNo(szResultBuffer); }
	return szResultBuffer;
}

BOOL IXingAPI::GetUseOverFuture() const
{
	if (m_fpGetUseOverFuture) { return m_fpGetUseOverFuture(); }
	return(FALSE);
}

BOOL IXingAPI::GetUseOverStock() const
{
	return m_fpGetUseOverStock();
}

BOOL IXingAPI::GetUseFX() const
{
	if (m_fpGetUseFX) { return m_fpGetUseFX(); }
	return(FALSE);
}

int IXingAPI::GetTRCountPerSec(LPCSTR pszCode) const
{
	if (m_fpGetTRCountPerSec) { return m_fpGetTRCountPerSec(pszCode); }
	return(-1);
}

int IXingAPI::GetTRCountBaseSec(LPCSTR pszCode) const
{
	if (m_fpGetTRCountBaseSec) { return m_fpGetTRCountBaseSec(pszCode); }
	return(-1);
}

int IXingAPI::GetTRCountRequest(LPCSTR pszCode) const
{
	if (m_fpGetTRCountRequest)
	{
		const int nCount = m_fpGetTRCountRequest(pszCode);
		//DBGPRINT("GetTRCountRequest: %s, Count: %d", pszCode, nCount);
		return(nCount);
	}
	return(-1);
}

int IXingAPI::GetTRCountLimit(LPCSTR pszCode) const
{
	if (m_fpGetTRCountLimit) { return m_fpGetTRCountLimit(pszCode); }
	return(-1);
}

void IXingAPI::SetNotifyFlag(BOOL bNotifyFlag)
{
	if (m_fpSetNotifyFlag) { m_fpSetNotifyFlag(bNotifyFlag); }
}

int IXingAPI::RequestService(LPCSTR pszCode, LPCSTR pszData, HWND _hWnd)
{
	if (m_fpRequestService) { return m_fpRequestService(_hWnd, pszCode, pszData); }
	return(-1);
}

int IXingAPI::RemoveService(LPCSTR pszCode, LPCSTR pszData, HWND _hWnd)
{
	if (m_fpRemoveService) { return m_fpRemoveService(_hWnd, pszCode, pszData); }
	return(-1);
}

int IXingAPI::RequestLinkToHTS(LPCSTR pszLinkKey, LPCSTR pszData, LPCSTR pszFiller, HWND _hWnd)
{
	if (m_fpRequestLinkToHTS) { return m_fpRequestLinkToHTS(_hWnd, pszLinkKey, pszData, pszFiller); }
	return(-1);
}

void IXingAPI::AdviseLinkFromHTS(HWND _hWnd) const
{
	if (m_fpAdviseLinkFromHTS) { m_fpAdviseLinkFromHTS(_hWnd); }
}

void IXingAPI::UnAdviseLinkFromHTS(HWND _hWnd) const
{
	if (m_fpUnAdviseLinkFromHTS) { m_fpUnAdviseLinkFromHTS(_hWnd); }
}

int IXingAPI::Decompress(LPCSTR _pSrc, LPCSTR pszDest, int nSrcLen) const
{
	if (m_fpDecompress) { return m_fpDecompress(_pSrc, pszDest, nSrcLen); }
	return(-1);
}

BOOL IXingAPI::IsChartLib() const
{
	if (m_fpIsChartLib) { return m_fpIsChartLib(); }
	return(FALSE);
}

void IXingAPI::SetProgramOrder(BOOL bProgramOrder) const
{
	if (m_fpSetProgramOrder) { m_fpSetProgramOrder(bProgramOrder); }
}

BOOL IXingAPI::GetProgramOrder() const
{
	if (m_fpGetProgramOrder) { return m_fpGetProgramOrder(); }
	return(FALSE);
}