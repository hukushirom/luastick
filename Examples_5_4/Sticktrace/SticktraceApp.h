// SticktraceApp.h : Sticktrace.DLL �̃��C�� �w�b�_�[ �t�@�C��
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH �ɑ΂��Ă��̃t�@�C�����C���N���[�h����O�� 'stdafx.h' ���C���N���[�h���Ă�������"
#endif

#include "resource.h"		// ���C�� �V���{��


// CSticktraceApp
// ���̃N���X�̎����Ɋւ��Ă� SticktraceWindow.cpp ���Q�Ƃ��Ă��������B
//

class CSticktraceApp : public CWinApp
{
public:
	CSticktraceApp();

// �I�[�o�[���C�h
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
