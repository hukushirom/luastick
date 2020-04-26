#pragma once

#include <synchapi.h>	// For InitializeConditionVariable.

// 共有ロック、排他ロックについて。
// 複数のスレッドから１つの資源に同時にアクセスする際に使う。
// 資源側にAutoSrwlExオブジェクトを配置し、各スレッド側にAutoReleaseLockExオブジェクトを配置する。
// 共有ロックは、同時に複数設定することができる。
// 排他ロックは、同時に１つだけ設定することができる。他の排他ロック、共有ロックとは同時に設定することはできない。
// 同じスレッドであっても、同じロック変数に排他ロックと共有ロック、排他ロックと排他ロックは同時に設定することはできない。

// AutoReleaseLockExクラス
// 共有から排他、排他から共有へ変更可能なロックを実現する仕組みについて。
//
// 共有から排他へのアップグレードは、以下の場合には失敗する。
// １．Thread1、Thread2がどちらも共有ロック中。
// ２．Thread1、Thread2がどちらも排他ロックへの優先変更フラグを申請。
// ３．先に申請したThread1の優先変更フラグが有効。Thread2の申請は無効。
//     但し、Thread2の共有ロックが解除されるまでThread1は排他ロックに移行できない。
// ここで重要なのは、Thread2の排他アップグレードは失敗しているが、Thread2は共有ロックを解除する必要があるという点。
// 解除しないとデッドロックとなってしまう。
//
//
//                         +-------+
//                         |Thread1|
//                         +---+---+
//                             |
//              Require ShLock-+
//                             |
//              Acquire ShLock-+                    +-------+
//                             |                    |Thread2|
//                             |                    +-------+
//                             | <--------- Require ShLock(A) / ExLock(A') / Upgrade(A'')
//           +-   Set Prefered-+
//           |                 | <--------- Require ShLock(B) / ExLock(B') / Upgrade(B'')
//           |  Release ShLock-+
//           |                 | <--------- Require ShLock(C) / ExLock(C') / Upgrade(C'')
//    Upgrade|  Require ExLock-+
//           |                 | <--------- Require ShLock(D) / ExLock(D') / Upgrade(D'')
//           |  Acquire ExLock-+
//           |                 | <--------- Require ShLock(E) / ExLock(E') / Upgrade(E'')
//           +- Reset Prefered-+
//                             | <--------- Require ShLock(F) / ExLock(F') / Upgrade(F'')
//                             |
//                             :
//                             :
//
//
// 通常の共有/排他ロックの手順
// 手順1. ロックを試みる。ロック失敗の場合は手順4へジャンプ。
// 手順2. 優先ロックフラグをチェック。フラグ＝FALSEの場合は手順6へジャンプ。
// 手順3. ロックを解除。
// 手順4. シグナルを待つ。
// 手順5. 手順1へジャンプ。
// 手順6. ロック完了。
//

// アップグレードロックの手順
// 手順1. 優先フラグ設定を試みる。設定成功の場合は手順4へジャンプ。
// 手順2. ロックを解除する。
// 手順3. エラーを返す。
// 手順4. ロックを解除する。
// 手順5. ロックを試みる。ロック成功の場合は手順8へジャンプ。
// 手順6. シグナルを待つ。
// 手順7. 手順5へジャンプ。
// 手順8. 優先フラグを解除。
// 手順9. ロック完了。
//
// (A)の場合
// Thread1はまだ共有ロック中。以下の流れとなる。
// 手順1 > 手順2 > 手順6
// (B)の場合
// Thread1はまだ共有ロック中だが優先フラグ＝TRUE。以下の流れとなる。
// 手順1 > 手順2 > 手順3 > 手順4
// (C)の場合
// Thread1はロック解除中だが優先フラグ＝TRUE。以下の流れとなる。
// 手順1 > 手順2 > 手順3 > 手順4
// (D)の場合
// Thread1は排他ロック申請中。以下の流れとなる。
// 手順1 > 手順4 （Thread1のロックが優先される）
// (E)の場合
// Thread1は排他ロック済み。以下の流れとなる。
// 手順1 > 手順4
// (F)の場合
// Thread1は排他ロック済み、優先フラグ＝FALSE。以下の流れとなる。
// 手順1 > 手順4
//
// (A')の場合
// Thread1はまだ共有ロック中。以下の流れとなる。
// 手順1 > 手順4 > 手順5 > 手順1 > 手順2 > 手順3 > 手順4 …
// (B')の場合
// Thread1はまだ共有ロック中だが優先フラグ＝TRUE。以下の流れとなる。
// 手順1 > 手順4 > 手順5 > 手順1 > 手順2 > 手順3 > 手順4 …
// (C')の場合
// Thread1はロック解除中だが優先フラグ＝TRUE。以下の流れとなる。
// 手順1 > 手順2 > 手順3 > 手順4 …
// (D')の場合
// Thread1は排他ロック申請中。以下の流れとなる。
// 手順1 > 手順4 > 手順5 > 手順1 > 手順2 > 手順3 > 手順4 …
// (E')の場合
// Thread1は排他ロック済み。以下の流れとなる。
// 手順1 > 手順4
// (F')の場合
// Thread1は排他ロック済み、優先フラグ＝FALSE。以下の流れとなる。
// 手順1 > 手順4
//
// (A'')の場合
// Thread1はまだ共有ロック中。以下の流れとなる。
// 手順1 > 手順4 > 手順5 > 手順8 > 手順9
// (B'')の場合
// Thread1はまだ共有ロック中だが優先フラグ＝TRUE。以下の流れとなる。
// 手順1 > 手順2 > 手順3
// (C'')の場合
// Thread1はロック解除中だが優先フラグ＝TRUE。以下の流れとなる。
// 手順1 > 手順2 > 手順3
// (D'')の場合
// Thread1は排他ロック申請中。以下の流れとなる。
// 手順1 > 手順2 > 手順3
// (E'')の場合
// Thread1は排他ロック済み。以下の流れとなる。
// Thread1が排他ロック済みということは、Thread2のロックは解除されているはず。
// (F'')の場合
// Thread1は排他ロック済み、優先フラグ＝FALSE。以下の流れとなる。
// Thread1が排他ロック済みということは、Thread2のロックは解除されているはず。
//
//

/// <summary>
/// CONDITION_VARIABLEを自動的に初期化する。
/// </summary>
class AutoCV
{
public:
	AutoCV ()
	{
		InitializeConditionVariable(&cdv);
	}	

	/// <summary>
	/// コピーコンストラクター。CONDITION_VARIABLEをコピーしないよう気を付ける。
	/// </summary>
	/// <param name="acv">The acv.</param>
	AutoCV(const AutoCV & acv) :AutoCV() {}

	/// <summary>
	/// SleepConditionVariableでスリープ状態にあるスレッドを稼働させる。
	/// </summary>
	void Wake()
	{
		::WakeConditionVariable(&cdv);
	} // WakeConditionVariable

	CONDITION_VARIABLE cdv;
}; // class AutoCV.

/// <summary>
/// CriticalSection の初期化と抹消処理を自動で行う。
/// </summary>
struct AutoCS
{
	AutoCS ()
	{
		InitializeCriticalSection(&cs);
	}	

	/// <summary>
	/// コピーコンストラクター。CRITICAL_SECTIONをコピーしないよう気を付ける。
	/// </summary>
	/// <param name="acs">The acs.</param>
	AutoCS(const AutoCS & acs) :AutoCS() {}

	~AutoCS ()
	{
		DeleteCriticalSection(&cs);
	}
	CRITICAL_SECTION cs;
}; // struct AutoCS.

/*************************************************************************
 * <クラス>	AutoLeaveCS
 *
 * <機能>	CriticalSection の初期化と抹消処理を自動で行う。
 *
 * <履歴>	04.08.19 Fukushiro M. 作成
 *************************************************************************/
class AutoLeaveCS
{
public:	
	/// <summary>
	/// Initializes a new instance of the <see cref="AutoLeaveCS"/> class.
	/// </summary>
	/// <param name="cs">Critical Section</param>
	/// <param name="cdv">Condition Variable. It is used for SleepConditionVariable. If there is no necessity you can reset nullptr.</param>
	/// <param name="isAcquireInit">true:初期化時にAcquire状態 / false:初期化時にRelease状態</param>
	AutoLeaveCS(CRITICAL_SECTION* cs, CONDITION_VARIABLE* cdv, bool isEnterinit)
		: m_cs(cs)
		, m_cdv(cdv)
		, m_isEnter(false)
	{
		if (isEnterinit) Enter();
	}
	/// <summary>
	/// Initializes a new instance of the <see cref="AutoLeaveCS"/> class.
	/// </summary>
	/// <param name="cs">Critical Section</param>
	/// <param name="isAcquireInit">true:初期化時にAcquire状態 / false:初期化時にRelease状態</param>
	AutoLeaveCS (CRITICAL_SECTION* cs, bool isEnterinit)
		: AutoLeaveCS(cs, nullptr, isEnterinit)
	{
	}
	/// <summary>
	/// Initializes a new instance of the <see cref="AutoLeaveCS"/> class. 初期化時にAcquire状態。
	/// </summary>
	/// <param name="cs">Critical Section</param>
	/// <param name="cdv">Condition Variable. It is used for SleepConditionVariable. If there is no necessity you can reset nullptr.</param>
	AutoLeaveCS (CRITICAL_SECTION* cs, CONDITION_VARIABLE* cdv)
		: AutoLeaveCS(cs, cdv, true)
	{
	}
	/// <summary>
	/// Initializes a new instance of the <see cref="AutoLeaveCS"/> class. 初期化時にAcquire状態。
	/// </summary>
	/// <param name="cs">Critical Section</param>
	AutoLeaveCS (CRITICAL_SECTION* cs)
		: AutoLeaveCS(cs, nullptr, true)
	{
	}
	/// <summary>
	/// Initializes a new instance of the <see cref="AutoLeaveCS"/> class.
	/// </summary>
	/// <param name="acs">Critical Section</param>
	/// <param name="acv">Condition Variable. It is used for SleepConditionVariable.</param>
	/// <param name="isAcquireInit">true:初期化時にAcquire状態 / false:初期化時にRelease状態</param>
	AutoLeaveCS (AutoCS& acs, AutoCV& acv, bool isEnterinit)
		: AutoLeaveCS(&acs.cs, &acv.cdv, isEnterinit)
	{
	}
	/// <summary>
	/// Initializes a new instance of the <see cref="AutoLeaveCS"/> class. 初期化時にAcquire状態。
	/// </summary>
	/// <param name="acs">Critical Section</param>
	/// <param name="acv">Condition Variable. It is used for SleepConditionVariable.</param>
	AutoLeaveCS (AutoCS& acs, AutoCV& acv)
		: AutoLeaveCS(&acs.cs, &acv.cdv, true)
	{
	}
	/// <summary>
	/// Initializes a new instance of the <see cref="AutoLeaveCS"/> class.
	/// </summary>
	/// <param name="acs">Critical Section</param>
	/// <param name="isAcquireInit">true:初期化時にAcquire状態 / false:初期化時にRelease状態</param>
	AutoLeaveCS (AutoCS& acs, bool isEnterinit)
		: AutoLeaveCS(&acs.cs, nullptr, isEnterinit)
	{
	}
	/// <summary>
	/// Initializes a new instance of the <see cref="AutoLeaveCS"/> class. 初期化時にAcquire状態。
	/// </summary>
	/// <param name="acs">Critical Section</param>
	AutoLeaveCS (AutoCS& acs)
		: AutoLeaveCS(&acs.cs, nullptr, true)
	{
	}
	/// <summary>
	/// Initializes a new instance of the <see cref="AutoLeaveCS"/> class. 初期化時にAcquire状態。
	/// </summary>
	/// <param name="acs">Critical Section</param>
	AutoLeaveCS (const AutoCS& acs)
		: AutoLeaveCS((CRITICAL_SECTION*)&acs.cs, nullptr, true)
	{
	}

	~AutoLeaveCS ()
	{
		Leave();
		m_cs = nullptr;
	}
	void Enter()
	{
		if (!m_isEnter)
		{
			EnterCriticalSection(m_cs);
			m_isEnter = true;
		}
	}
	void Leave()
	{
		if (m_isEnter)
		{
			LeaveCriticalSection(m_cs);
			m_isEnter = false;
		}
	}
	bool IsEnter() const
	{
		return m_isEnter;
	}
	
	/// <summary>
	/// WakeConditionVariableが実行されるまでスリープする。Critical SectionはRelease状態となる。
	/// WakeConditionVariableが実行されると、再びCritical SectionはAcquire状態となる。
	/// </summary>
	/// <param name="_msec">待ち時間（ミリ秒） / INFINITE:無限時間</param>
	/// <returns>true:成功 / false:失敗（タイムアウト等）</returns>
	bool SleepConditionVariable(DWORD _msec = INFINITE)
	{
		return SleepConditionVariableCS(m_cdv, m_cs, _msec) ? true : false;
	} // SleepConditionVariable
	
	/// <summary>
	/// SleepConditionVariableでスリープ状態にあるスレッドを稼働させる。
	/// </summary>
	void WakeConditionVariable()
	{
		::WakeConditionVariable(m_cdv);
	} // WakeConditionVariable

protected:
	CRITICAL_SECTION*	m_cs;
	CONDITION_VARIABLE*	m_cdv;
	bool				m_isEnter;
}; // class AutoLeaveCS.



#if 0


/// <summary>
/// SRWLOCKの自動初期化クラス。AutoReleaseLockExクラス専用。
/// </summary>
class AutoSrwlEx
{
public:
	AutoSrwlEx ()
		: m_priority1(0)
		, m_priority2(0)
	{
		InitializeSRWLock(&m_srw);
		m_event = ::CreateEvent(nullptr, false, false, nullptr);
	}
	~AutoSrwlEx ()
	{
		::CloseHandle(m_event);
		m_event = INVALID_HANDLE_VALUE;
	}
	SRWLOCK m_srw;		// ロック変数
	HANDLE m_event;		// ロック解除シグナルを通知するイベント。
	short m_priority1;	// 第１優先フラグ：1:優先ロック設定 / 0:優先ロック未設定
	short m_priority2;	// 第２優先フラグ：1:優先ロック設定 / 0:優先ロック未設定
}; // class AutoSrwlEx.

/// <summary>
/// 共有ロック、排他ロックを制御するクラス。ロックを自動的に解放する。
/// 他のロック要求よりも優先させる排他ロックを設定できる。
/// 共有から排他、排他から共有へロックを切り替えることができる。
/// </summary>
class AutoReleaseLockEx
{
public:	
	/// <summary>
	/// ロックのタイプ。
	/// </summary>
	enum class LockType
	{
		None,
		Shared,		// 共有ロック
		Exclusive,	// 排他ロック
	};
	
	/// <summary>
	/// エラーコード。
	/// </summary>
	enum class ErrorCode : __int32
	{
		None = 0,
		Succeeded = 1,			// 成功
		NotAcquired = 2,		// ロックされていない
		Timeout = 3,			// タイムアウト
		AlreadyPrefered = 4,	// 他から優先申請されているロック変数に優先を申請した
	};
protected:
	static constexpr DWORD TRYINTERVAL = 100;
public:	
	/// <summary>
	/// コンストラクタ。作成時にロックさせることができる。
	/// 排他ロックの場合、優先ロックされる。
	/// </summary>
	/// <param name="asrw">ロック変数</param>
	/// <param name="isInitAcquire">true:作成時にロックする / false:作成時にロックしない</param>
	/// <param name="lockType">Shared:共有ロック / Exclusive:排他ロック</param>
	/// <param name="dwMilliseconds">ロックできるまでの最大待ち時間。ミリ秒</param>
	AutoReleaseLockEx(AutoSrwlEx& asrw, bool isInitAcquire, LockType lockType, DWORD dwMilliseconds)
		: m_asrwEx(asrw)
		, m_lockType(LockType::None)
	{
		if (isInitAcquire)
		{
			/// 排他ロックの場合、優先ロックされる。
			const auto errorCode = (lockType == LockType::Shared) ?
				Acquire(LockType::Shared, dwMilliseconds)
				:
				AcquirePriorExclusive(dwMilliseconds);
			if (errorCode != ErrorCode::Succeeded)
				ThrowLeException(
					LeError::RESOURCE_LOCK_FAILED,		// リソースのロックに失敗しました。
					(__int32)errorCode
					);
		}
	}
	
	/// <summary>
	/// コンストラクタ。作成時に無限待ち時間でロックする。
	/// 排他ロックの場合、優先ロックされる。
	/// </summary>
	/// <param name="asrw">ロック変数</param>
	/// <param name="lockType">Shared:共有ロック / Exclusive:排他ロック</param>
	AutoReleaseLockEx(AutoSrwlEx& asrw, LockType lockType)
		: AutoReleaseLockEx(asrw, true, lockType, INFINITE)
	{
	}
	
	/// <summary>
	/// コンストラクタ。作成時にロックしない。
	/// </summary>
	/// <param name="asrw">ロック変数</param>
	AutoReleaseLockEx(AutoSrwlEx& asrw)
		: AutoReleaseLockEx(asrw, false, LockType::None, INFINITE)
	{
	}
	
	/// <summary>
	/// デストラクタ。自動的にロックを解除する。
	/// </summary>
	~AutoReleaseLockEx()
	{
		Release();
	}
		
	/// <summary>
	/// 通常の（優先の無い）ロックを試みる。
	/// </summary>
	/// <param name="lockType">Shared:共有ロック / Exclusive:排他ロック</param>
	/// <returns>エラーコード</returns>
	ErrorCode Acquire(LockType lockType)
	{
		return Acquire(lockType, INFINITE);
	}
	
	/// <summary>
	/// 通常の（優先の無い）ロックを試みる。
	/// </summary>
	/// <param name="lockType">Shared:共有ロック / Exclusive:排他ロック</param>
	/// <param name="dwMilliseconds">ロックできるまでの最大待ち時間。ミリ秒</param>
	/// <returns>エラーコード</returns>
	ErrorCode Acquire(LockType lockType, DWORD dwMilliseconds)
	{
		using TryLockFunc = BOOLEAN (WINAPI *)(_Inout_ PSRWLOCK SRWLock);
		using ReleaseLockFunc = VOID (WINAPI *)(_Inout_ PSRWLOCK SRWLock);
		// ロック済みの場合は復帰。
		if (IsAcquired())
			ThrowLeSystemError();
		// ロック関数と解除関数。
		TryLockFunc tryLockFunc = (lockType == LockType::Shared) ? TryAcquireSRWLockShared : TryAcquireSRWLockExclusive;
		ReleaseLockFunc releaseLockFunc = (lockType == LockType::Shared) ? ReleaseSRWLockShared : ReleaseSRWLockExclusive;
		for (;;)
		{
			// ロックを試みる。
			if (tryLockFunc(&m_asrwEx.m_srw))
			{
				//----- ロックできた場合 -----
				if (::InterlockedCompareExchange16(&m_asrwEx.m_priority1, 0, 0) == 0 &&
					::InterlockedCompareExchange16(&m_asrwEx.m_priority2, 0, 0) == 0)
				{	//----- 優先フラグが未設定の場合はロック成功 -----
					m_lockType = lockType;
					break;
				}
				//----- ロックできたが、優先ロックが指定されていた場合 -----
				// ロックを解除し、イベント通知を待つ。
				releaseLockFunc(&m_asrwEx.m_srw);
			}
			if (dwMilliseconds != INFINITE)
			{	//----- 待ち時間が指定されている場合はチェック -----
				if (dwMilliseconds < TRYINTERVAL) break;
				dwMilliseconds -= TRYINTERVAL;
			}
			::WaitForSingleObject(m_asrwEx.m_event, TRYINTERVAL);
		}
		return IsAcquired() ? ErrorCode::Succeeded : ErrorCode::Timeout;
	}

	/// <summary>
	/// 通常ロックに対して優先のある排他ロックを試みる。優先権はロックタイプの変更（アップグレード、ダウングレード）が最優先。
	/// このロックはその次に優先権を持つ。
	/// </summary>
	/// <param name="dwMilliseconds">ロックできるまでの最大待ち時間。ミリ秒</param>
	/// <returns>エラーコード</returns>
	ErrorCode AcquirePriorExclusive(DWORD dwMilliseconds)
	{
		// ロック済みの場合は復帰。
		if (IsAcquired())
			ThrowLeSystemError();
		// 第２優先フラグの設定を試みる。
		while (::InterlockedCompareExchange16(&m_asrwEx.m_priority2, 1, 0) != 0)
		{
			if (dwMilliseconds != INFINITE)
			{	//----- 待ち時間が指定されている場合はチェック -----
				// 時間切れの場合はエラー。
				if (dwMilliseconds < TRYINTERVAL) return ErrorCode::Timeout;
				dwMilliseconds -= TRYINTERVAL;
			}
			::WaitForSingleObject(m_asrwEx.m_event, TRYINTERVAL);
		}
		//----- 第２優先フラグが設定できた場合 -----
		for (;;)
		{
			// ロックを試みる。
			if (TryAcquireSRWLockExclusive(&m_asrwEx.m_srw))
			{
				//----- ロックできた場合 -----
				if (::InterlockedCompareExchange16(&m_asrwEx.m_priority1, 0, 0) == 0)
				{	//----- 第１優先フラグが未設定の場合はロック成功 -----
					m_lockType = LockType::Exclusive;
					break;
				}
				//----- ロックできたが、第１優先ロックが指定されていた場合 -----
				// ロックを解除し、イベント通知を待つ。
				ReleaseSRWLockExclusive(&m_asrwEx.m_srw);
			}
			if (dwMilliseconds != INFINITE)
			{	//----- 待ち時間が指定されている場合はチェック -----
				if (dwMilliseconds < TRYINTERVAL) break;
				dwMilliseconds -= TRYINTERVAL;
			}
			::WaitForSingleObject(m_asrwEx.m_event, TRYINTERVAL);
		}
		// 優先ロックを解除。
		if (::InterlockedCompareExchange16(&m_asrwEx.m_priority2, 0, 1) != 1)
			ThrowLeSystemError();
		return IsAcquired() ? ErrorCode::Succeeded : ErrorCode::Timeout;
	}

	/// <summary>
	/// 共有ロックを排他ロックにアップグレードする。処理途中にエラーが発生した場合はロックが解除される。
	/// 共有ロックから排他ロックへの変更は、他のスレッドから同時に変更が要求された場合に失敗する。
	/// この場合、先に変更要求を出した他のスレッドも、このスレッドの共有ロックが解除されるまで排他ロックできない（デッドロック）。
	/// デッドロックを防ぐため、エラー時は必ずロックを解除するようにする。
	/// </summary>
	/// <param name="dwMilliseconds">ロックできるまでの最大待ち時間。ミリ秒</param>
	/// <returns>
	/// ErrorCode::Succeeded ロック成功 / エラーコード
	/// なお以下のエラーコードは正常時でも発生する場合がある。
	/// ErrorCode::AlreadyPrefered 他の共有ロックが先に排他ロックの優先要求を出していた場合
	/// ErrorCode::Timeout タイムアウトした場合
	/// 排他ロックから共有ロックへの変更は、ロックタイプ等を間違えない限りエラーは発生しない。
	/// </returns>
	ErrorCode UpgradeToExclusive(DWORD dwMilliseconds)
	{
		// ロック済みでない場合はエラー。
		if (m_lockType != LockType::Shared)
			ThrowLeSystemError();
		// 第１優先フラグの設定を試みる。ロック種類変更はロックが継続することが必須条件。
		// そのため、第１優先フラグが設定できない場合はロック変更を断念する。ロックを解除し終了する。
		if (::InterlockedCompareExchange16(&m_asrwEx.m_priority1, 1, 0) != 0)
		{	//----- 第１優先フラグが設定できない場合 -----
			// 既に他のスレッドで第１優先フラグが指定されている。ロックを解除し、エラーを返す。
			// ロックを解除する。
			(void)Release();
			return ErrorCode::AlreadyPrefered;
		}
		// 第１優先フラグが設定できれば、このスレッド以外がロックすることはない。
		// ロックを解除する。
		ReleaseSRWLockShared(&m_asrwEx.m_srw);
		m_lockType = LockType::None;
		for (;;)
		{
			// ロックを試みる。
			if (TryAcquireSRWLockExclusive(&m_asrwEx.m_srw))
			{
				m_lockType = LockType::Exclusive;
				break;
			}
			// 共有ロック->排他ロックの場合、他のスレッドで排他ロックがかかっている場合があるのでロックできるまでに時間がかかる場合がある。
			// そのためタイムアウトする可能性がある。
			// 排他ロック->共有ロックの場合、他のスレッドではロックできない。微妙なタイミングでロックに失敗する可能性はあるが、何度かトライすれば必ず成功する。
			if (dwMilliseconds != INFINITE)
			{	//----- 待ち時間が指定されている場合はチェック -----
				if (dwMilliseconds < TRYINTERVAL) break;
				dwMilliseconds -= TRYINTERVAL;
			}
			::WaitForSingleObject(m_asrwEx.m_event, TRYINTERVAL);
		}
		// 優先ロックを解除。
		if (::InterlockedCompareExchange16(&m_asrwEx.m_priority1, 0, 1) != 1)
			ThrowLeSystemError();
		return IsAcquired() ? ErrorCode::Succeeded : ErrorCode::Timeout;
	}

	/// <summary>
	/// 排他ロックを共有ロックにダウングレードする。ロック状態等を間違えない限り必ず成功する。
	/// </summary>
	void DowngradeToShared()
	{
		// ロック済みでない場合はエラー。
		if (m_lockType != LockType::Exclusive)
			ThrowLeSystemError();
		// 第１優先フラグの設定を試みる。
		// 排他ロック->共有ロックの場合、他のスレッドで同時にロックしていることはあり得ない。
		// 第１優先フラグは既にロック済みのオブジェクトからしか設定できないので、ここは必ず成功する。
		if (::InterlockedCompareExchange16(&m_asrwEx.m_priority1, 1, 0) != 0)
			ThrowLeSystemError();
		// 第１優先フラグが設定できれば、このスレッド以外がロックすることはない。
		// ロックを解除する。
		ReleaseSRWLockExclusive(&m_asrwEx.m_srw);
		m_lockType = LockType::None;
		// 排他ロック->共有ロックの場合、他のスレッドで同時にロックしていることはあり得ない。微妙なタイミングでロックに失敗する可能性はあるが、何度かトライすれば必ず成功する。
		// ロックを試みる。
		while (!TryAcquireSRWLockShared(&m_asrwEx.m_srw))
			::WaitForSingleObject(m_asrwEx.m_event, TRYINTERVAL);
		m_lockType = LockType::Shared;
		// 優先ロックを解除。
		if (::InterlockedCompareExchange16(&m_asrwEx.m_priority1, 0, 1) != 1)
			ThrowLeSystemError();
	}

	/// <summary>
	/// ロックを解除する。
	/// </summary>
	/// <returns>エラーコード</returns>
	ErrorCode Release()
	{
		// ロック解除済みの場合は復帰。
		if (!IsAcquired()) return ErrorCode::NotAcquired;
		// ロックを解除する。
		if (m_lockType == LockType::Shared)
			ReleaseSRWLockShared(&m_asrwEx.m_srw);
		else
			ReleaseSRWLockExclusive(&m_asrwEx.m_srw);
		m_lockType = LockType::None;
		// シグナルを送信。ロック申請中のオブジェクトへ通知。
		::SetEvent(m_asrwEx.m_event);
		return ErrorCode::Succeeded;
	}

	/// <summary>
	/// ロック中かどうか調べる。
	/// </summary>
	/// <returns>true:ロック中 / false:ロック解除中</returns>
	bool IsAcquired() const
	{
		return (m_lockType != LockType::None);
	}
	
	/// <summary>
	/// ロックタイプを返す。
	/// </summary>
	/// <returns>None:ロック無し / Shared:共有ロック / Exclusive:排他ロック</returns>
	LockType GetLockType() const
	{
		return m_lockType;
	}
private:
	AutoSrwlEx&	m_asrwEx;
	LockType	m_lockType;
}; // class AutoReleaseLockEx.

#endif
