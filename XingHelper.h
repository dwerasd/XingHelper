#pragma once


#include <Windows.h>

#include <algorithm>
#include <span>
#include <queue>

#include <DarkCore/DTimer.h>
#include <DarkCore/DString.h>

//#define WMU_SHOW_TR		( WM_USER + 1000 )
//#define WMU_SET_MESSAGE	( WM_USER + 1001 )
//#define WMU_FORCEDQUIT ( WM_USER + 1018 )

enum
{
    DATA_TYPE_STRING = 0,		// 문자열
    DATA_TYPE_LONG,				// 정수
    DATA_TYPE_FLOAT,			// 실수
    DATA_TYPE_FLOAT_DOT,		// 실수( 소숫점을 가지고 있음 )
};
#define DATA_TYPE_DOUBLE	DATA_TYPE_FLOAT
#define DATA_TYPE_DOT		DATA_TYPE_FLOAT_DOT

namespace xing
{
	struct _USER_INFO
	{
		BOOL bMockServer{ FALSE }; // Mock 서버 사용 여부
        std::string strID;
		std::string strPW;
		std::string strCR;
		std::string strAccPW;
		std::string strUserName;
		std::unordered_map<std::string, std::string> umapAccInfo; // 계좌 정보 (key: 계좌번호, value: 계좌명)
	};
	extern _USER_INFO user_info;

    extern bool g_bShowRaw;  // Raw 데이터 표시 여부

    // 메인 함수
    //LPCSTR get_disp_data(LPCSTR psData, const size_t nSize, const BYTE nType, const int nDotPos = 0);

    inline std::string GetDispData(std::string_view sv, const BYTE _bytType = DATA_TYPE_STRING, int nDotPos = 0)
    {
        if (g_bShowRaw) {
            return std::string(sv);
        }

        switch (_bytType) {
        case DATA_TYPE_STRING: {
            auto pos = sv.find_last_not_of(" \t\r\n\v\f");
            if (pos == std::string_view::npos) return "";
			return std::string(sv.substr(0, pos + 1));      // 문자열 끝의 공백 제거
        }
        case DATA_TYPE_LONG: {
            auto pos = sv.find_first_not_of('0');
            if (pos == std::string_view::npos) return "0";
            return std::string(sv.substr(pos));
        }
        case DATA_TYPE_FLOAT: {
            if (sv.length() <= static_cast<size_t>(nDotPos)) return "0.0";
            std::string_view int_part = sv.substr(0, sv.length() - nDotPos);
            std::string_view frac_part = sv.substr(sv.length() - nDotPos);
            auto pos = int_part.find_first_not_of('0');
            if (pos == std::string_view::npos) return "0." + std::string(frac_part);
            return std::string(int_part.substr(pos)) + "." + std::string(frac_part);
        }
        case DATA_TYPE_FLOAT_DOT: {
            const auto dot_pos = sv.find('.');
            if (dot_pos == std::string_view::npos) { // No dot
                auto pos = sv.find_first_not_of('0');
                if (pos == std::string_view::npos) return "0.0";
                return std::string(sv.substr(pos)) + ".0";
            }
            std::string_view int_part = sv.substr(0, dot_pos);
            std::string_view frac_part = sv.substr(dot_pos + 1);
            auto pos = int_part.find_first_not_of('0');
            if (pos == std::string_view::npos || int_part.empty()) return "0." + std::string(frac_part);
            return std::string(int_part.substr(pos)) + "." + std::string(frac_part);
        }
        default:
            return std::string(sv);
        }
    }
    // 기존 API와의 호환성을 위한 최종 어댑터
    inline LPCSTR get_disp_data(LPCSTR psData, size_t nSize, const BYTE _bytType = DATA_TYPE_STRING, int nDotPos = 0)
    {
        thread_local static char resultBuffer[256];
        const std::string result = GetDispData(std::string_view(psData, nSize), _bytType, nDotPos);
        ::strcpy_s(resultBuffer, sizeof(resultBuffer), result.c_str());
        return resultBuffer;
    }

    //void set_packet_data(LPSTR _pOutput, const size_t _nOutputBufferSize, LPCSTR _pSrc, const BYTE _bytType, const WORD _bytDotPos = 0);
    inline void SetPacketData(std::span<char> dest, std::string_view src, const BYTE _bytType = DATA_TYPE_STRING, const WORD _bytDotPos = 0)
    {
        switch (_bytType)
        {
        case DATA_TYPE_STRING:
            {
                // 왼쪽 정렬, ' '로 패딩
                std::fill(dest.begin(), dest.end(), ' ');
                const auto copy_len = min(dest.size(), src.length());
                std::copy(src.begin(), src.begin() + copy_len, dest.begin());
                break;
            }
        case DATA_TYPE_LONG:
            {
                // 오른쪽 정렬, '0'으로 패딩
                std::fill(dest.begin(), dest.end(), '0');
                if (src.length() >= dest.size()) {
                    // 원본이 더 길면, 원본의 뒷부분을 잘라 채움 (오른쪽 정렬이므로)
                    std::copy(src.end() - dest.size(), src.end(), dest.begin());
                }
                else {
                    // 원본이 더 짧으면, 버퍼의 뒷부분에 맞추어 복사
                    std::copy(src.begin(), src.end(), dest.end() - src.length());
                }
                break;
            }
        case DATA_TYPE_FLOAT:
        case DATA_TYPE_FLOAT_DOT:
            {
                // 실수 처리: 정수부와 소수부를 분리하여 각각 정렬
                std::fill(dest.begin(), dest.end(), '0');

                const auto dot_pos_src = src.find('.');
                const std::string_view src_int = (dot_pos_src == std::string_view::npos) ? src : src.substr(0, dot_pos_src);
                const std::string_view src_frac = (dot_pos_src == std::string_view::npos) ? "" : src.substr(dot_pos_src + 1);

                // 목적지 버퍼에서 정수부와 소수부가 차지할 공간 계산
                const size_t dest_frac_len = _bytDotPos;
                const size_t dest_dot_len = (_bytType == DATA_TYPE_FLOAT) ? 1 : 0;
                if (dest.size() < dest_frac_len + dest_dot_len) break; // 공간 부족
                const size_t dest_int_len = dest.size() - dest_frac_len - dest_dot_len;

                // 정수부 복사 (오른쪽 정렬)
                if (src_int.length() >= dest_int_len) {
                    std::copy(src_int.end() - dest_int_len, src_int.end(), dest.begin());
                }
                else {
                    std::copy(src_int.begin(), src_int.end(), dest.begin() + dest_int_len - src_int.length());
                }

                // 소수점 복사 (필요 시)
                if (dest_dot_len > 0) {
                    dest[dest_int_len] = '.';
                }

                // 소수부 복사 (왼쪽 정렬, 뒷부분 잘림)
                const auto frac_copy_len = min(dest_frac_len, src_frac.length());
                std::copy(src_frac.begin(), src_frac.begin() + frac_copy_len, dest.begin() + dest_int_len + dest_dot_len);

                break;
            }
        }
    }

    inline void set_packet_data(char* psData, int nSize, LPCSTR pszSrc, const BYTE _bytType = DATA_TYPE_STRING, const WORD _bytDotPos = 0)
    {
        // LPCTSTR은 std::string_view로 암시적 변환이 가능
        SetPacketData(std::span(psData, nSize), pszSrc, _bytType, _bytDotPos);
    }

    class C_TR_TIME_CHECKER
    {
    private:
        int nTRCountPerSec{ 0 };          // 초당 전송 가능 횟수
        int nTrCountBaseSec{ 0 };         // 기준 초
        int nTrCountLimit{ 0 };           // 10분당 전송 가능 횟수 제한
        long nBaseInterval{ 0 };          // 기본 호출 간격 (밀리초)
        long nPenaltyInterval{ 0 };       // 제한 초과 시 간격 (5배)
        bool bExceededLimit{ false };     // 10분 제한 초과 여부

        dk::C_TIMER_QP Timer;           // 간격 체크용 타이머

    public:
        C_TR_TIME_CHECKER(const int _nPerSec = 0, const int _nBaseSec = 0, const int _nCountLimit = 0)
        {
            Init(_nPerSec, _nBaseSec, _nCountLimit);
        }

        virtual ~C_TR_TIME_CHECKER() = default;

        inline void Init(const int _nPerSec, const int _nBaseSec, const int _nCountLimit)
        {
            nTRCountPerSec = _nPerSec;
            nTrCountBaseSec = _nBaseSec;
            nTrCountLimit = _nCountLimit;
            bExceededLimit = false;

            // 타이머 리셋 - 생성자에서 설정된 시작시간 제거
            Timer.Stop();

            // 간격 계산
            if (nTrCountLimit > 0)
            {
                // 종량제 TR: 10분을 제한횟수로 나눈 간격
                nBaseInterval = (10 * 60 * 1000) / nTrCountLimit; // 예: 600초/200건 = 3초
                nPenaltyInterval = nBaseInterval * 5; // 제한 초과 시 5배
            }
            else if (nTRCountPerSec > 0 && nTrCountBaseSec > 0)
            {
                // 일반 TR: 기존 방식
                nBaseInterval = (nTrCountBaseSec * 1000) / nTRCountPerSec;
                nPenaltyInterval = nBaseInterval;
            }
            else
            {
                nBaseInterval = 100; // 기본값
                nPenaltyInterval = nBaseInterval;
            }
        }

        inline void Reset()
        {
            Timer.Stop();
            bExceededLimit = false;
        }

        // 간격 체크 - Request 호출 전에 체크
        inline bool checkInterval()
        {
            if (nBaseInterval <= 0) return false;

            if (!Timer.Running()) return true; // 첫 호출

            double elapsedMs = Timer.TimeLeft() * 1000;
            long currentInterval = bExceededLimit ? nPenaltyInterval : nBaseInterval;

            return elapsedMs >= currentInterval;
        }

        // Request 호출 후 타이머 시작
        inline void startTimer()
        {
            Timer.Start();
        }

        // 10분 제한 상태 업데이트 - Request 호출 후에 호출
        inline void updateLimitStatus(int currentCount)
        {
            if (nTrCountLimit <= 0) return; // 제한 없는 TR

            bool wasExceeded = bExceededLimit;
            bExceededLimit = (currentCount >= nTrCountLimit);

            if (!wasExceeded && bExceededLimit)
            {
                // 제한 초과 시작
                DBGPRINT("10분 제한 초과 - 간격 5배 증가: %d -> %d ms", nBaseInterval, nPenaltyInterval);
            }
            else if (wasExceeded && !bExceededLimit)
            {
                // 제한 해제
                DBGPRINT("10분 제한 해제 - 간격 복원: %d ms", nBaseInterval);
            }
        }

        // 다음 호출까지 남은 시간 (밀리초)
        inline unsigned long GetTimeUntilNextCall()
        {
            if (!Timer.Running() || nBaseInterval <= 0) return 0;

            const double elapsed = Timer.TimeLeft() * 1000;
            const long currentInterval = bExceededLimit ? nPenaltyInterval : nBaseInterval;

            return static_cast<unsigned long>(max(0.0, currentInterval - elapsed));
        }

        // 상태 조회
        inline bool IsLimitExceeded() const { return bExceededLimit; }
        inline int GetLimit() const { return nTrCountLimit; }
        inline long GetCurrentInterval() const { return bExceededLimit ? nPenaltyInterval : nBaseInterval; }
    };
	typedef std::unordered_map<std::string, C_TR_TIME_CHECKER> UMAP_TR_TIMERS;

	struct _CODE_INFO
	{
		BYTE nMarketType{ 0 };					// [0]: 코스피, [1]: 코스닥, [10~]: 국내선물, [20~]: 해외거래소, [30~]: 해외선물
		std::string szCodeStock;
	};

	struct _REGISTER_INFO
	{
		BYTE nMarketType{ 0 };						// [0]: 코스피, [1]: 코스닥, [10~]: 국내선물, [20~]: 해외거래소, [30~]: 해외선물
		//BYTE bytSubscribe{ _SUBSCRIBE_NONE_ };				// [1] : JC0, [2]: JH0, [3]: OVC, [4]: OVH
		bool bSubscribeC{ false };					// 국내 종목 실시간 구독 여부
		bool bSubscribeH{ false };					// 해외 종목 실시간 구독 여부
		dk::CHART<8> strC, strH;						// 
	};
	typedef std::unordered_map<std::string, _REGISTER_INFO*> UMAP_ITEM_INFOS;

	struct _SUBSCRIBE
		: public _CODE_INFO
	{
		bool bRegister{ false };

		void init() { ::memset(this, 0, sizeof(_SUBSCRIBE)); }
	};
	typedef std::vector<_SUBSCRIBE> VEC_SUBSCRIBE;	// 

	// 구독중인 종목리스트, first: 구독키, second: info
	struct _SUBSCRIBE_INFO
	{
        //dk::CHART<4> szKey;
		dk::CHART<9> szData;
		int nDataUnitLen{ 0 };
	};
	typedef std::vector<std::pair<std::string, _SUBSCRIBE_INFO>> VEC_SUBSCRIBE_INFOS;
}
