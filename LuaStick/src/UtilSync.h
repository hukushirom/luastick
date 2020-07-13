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

