#include "StdAfx.h"
#include "SnapshotDlg.h"

CSnapshotDlg::CSnapshotDlg(void)
: SHostDialog(UIRES.LAYOUT.XML_DLG_SNAPSHOT)
{
	m_mapColorInfo.insert(std::make_pair(1, RGB(0,0,0)));
	m_mapColorInfo.insert(std::make_pair(2, RGB(127,127,127)));
	m_mapColorInfo.insert(std::make_pair(3, RGB(136,0,21)));
	m_mapColorInfo.insert(std::make_pair(4, RGB(237,28,36)));
	m_mapColorInfo.insert(std::make_pair(5, RGB(255,127,39)));
	m_mapColorInfo.insert(std::make_pair(6, RGB(255,242,0)));
	m_mapColorInfo.insert(std::make_pair(7, RGB(34,177,76)));
	m_mapColorInfo.insert(std::make_pair(8, RGB(0,162,232)));
	m_mapColorInfo.insert(std::make_pair(9, RGB(63,72,204)));
	m_mapColorInfo.insert(std::make_pair(10, RGB(163,73,164)));
	m_mapColorInfo.insert(std::make_pair(11, RGB(255,255,255)));
	m_mapColorInfo.insert(std::make_pair(12, RGB(195,195,195)));
	m_mapColorInfo.insert(std::make_pair(13, RGB(185,122,87)));
	m_mapColorInfo.insert(std::make_pair(14, RGB(255,174,201)));
	m_mapColorInfo.insert(std::make_pair(15, RGB(255,201,14)));
	m_mapColorInfo.insert(std::make_pair(16, RGB(239,228,176)));
	m_mapColorInfo.insert(std::make_pair(17, RGB(181,230,29)));
	m_mapColorInfo.insert(std::make_pair(18, RGB(153,217,234)));
	m_mapColorInfo.insert(std::make_pair(19, RGB(112,146,190)));
	m_mapColorInfo.insert(std::make_pair(20, RGB(200,191,231)));

	m_nSelectSize = 1;
}

CSnapshotDlg::~CSnapshotDlg(void)
{
}

BOOL CSnapshotDlg::OnInitDialog(HWND wnd, LPARAM lInitParam)
{
	SetMsgHandled(FALSE);
	DEVMODE dm;
	ZeroMemory(&dm, sizeof(dm));
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm);
	int nxScreen, nyScreen;
 	nxScreen = dm.dmPelsWidth;
 	nyScreen = dm.dmPelsHeight;
//	nxScreen = 600;
//	nyScreen = 400;


	::SetWindowPos(this->m_hWnd, HWND_TOPMOST, 0,0, nxScreen, nyScreen, SWP_SHOWWINDOW);
	SetForegroundWindow(this->m_hWnd);

	SOUI::CRect rc(0,0, nxScreen, nyScreen);
	CDC       hScrDC, hMemDC; 
	HBITMAP   hBitmap, hOldBitmap; 
	int       nX, nY, nX2, nY2;  
	int       nWidth, nHeight;
	hScrDC = CreateDC(L"DISPLAY", NULL, NULL, NULL);
	hMemDC = CreateCompatibleDC(hScrDC);
	nX = rc.left;
	nY = rc.top;
	nX2 = rc.right;
	nY2 = rc.bottom;
	if (nX < 0)
		nX = 0;
	if (nY < 0)
		nY = 0;
	if (nX2 > dm.dmPelsWidth)
		nX2 = dm.dmPelsWidth;
	if (nY2 > dm.dmPelsHeight)
		nY2 = dm.dmPelsHeight;
	nWidth = nX2 - nX;
	nHeight = nY2 - nY;
	hBitmap = CreateCompatibleBitmap(hScrDC, nWidth, nHeight);
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
	BitBlt(hMemDC, 0, 0, nWidth, nHeight, hScrDC, nX, nY, SRCCOPY);
	hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);


	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	if (pSnapshot)
	{
		pSnapshot->SetBmpResource(new CBitmap(hBitmap));
		pSnapshot->SetScreenSize(nxScreen, nyScreen);

		pSnapshot->GetEventSet()->subscribeEvent(&CSnapshotDlg::OnEventCapturing, this);
		pSnapshot->GetEventSet()->subscribeEvent(&CSnapshotDlg::OnEventRectMoving, this);
		pSnapshot->GetEventSet()->subscribeEvent(&CSnapshotDlg::OnEventRectCaptured, this);
		pSnapshot->GetEventSet()->subscribeEvent(&CSnapshotDlg::OnEventRectDbClk, this);
	}

	return TRUE;
}

void CSnapshotDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_ESCAPE)
		EndDialog(IDOK);

	SetMsgHandled(FALSE);
}

bool CSnapshotDlg::OnEventCapturing(EventCapturing* pEvt)
{
	SStatic* pText = FindChildByName2<SStatic>(L"text_title");
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SWindow* pOperateBar = FindChildByName2<SWindow>(L"operate_bar");
	SASSERT(pText);
	SASSERT(pSnapshot);
	SASSERT(pOperateBar);

	if (!pText->IsVisible())
		pText->SetVisible(TRUE, FALSE);
	if (!pOperateBar->IsVisible())
		pOperateBar->SetVisible(TRUE, FALSE);

	SOUI::CRect rcCap = pSnapshot->GetCapRect();
	SStringW sstrTitle;
	sstrTitle.Format(L"起始位置：%d.%d  区域大小：%d × %d", rcCap.left, rcCap.top, rcCap.Width(), rcCap.Height());
	pText->SetWindowText(sstrTitle);
	
	SOUI::CRect rcText = pText->GetWindowRect();
	SOUI::CRect rcWnd = GetWindowRect();

	int nX = rcCap.left;
	int nY = rcCap.top - rcText.Height() - 1;
	if (nY < 0)
		nY = rcCap.top ;

	if (rcWnd.right - nX < rcText.Width())
		nX = rcWnd.right - rcText.Width() - 1;

	SStringW sstrTitlePos;
	sstrTitlePos.Format(L"%d,%d", nX, nY);
	pText->SetAttribute(L"pos", sstrTitlePos);

	SOUI::CRect rcOperateBar = pOperateBar->GetWindowRect();
	int nOperateBarX = rcCap.right - rcOperateBar.Width();
	if (nOperateBarX < rcWnd.left)
		nOperateBarX = rcWnd.left;

	int nOperateBarY = 0;	
	if ((rcWnd.bottom - rcCap.bottom - 2) > rcOperateBar.Height())			// bottom
		nOperateBarY = rcCap.bottom + 2;
	else if ((rcCap.top - rcWnd.top - 2) > rcOperateBar.Height())				// top  有 空间
		nOperateBarY = rcCap.top - rcOperateBar.Height() - 2;
	else // 右上角 内 显示
		nOperateBarY = rcCap.top + 2;

	SStringW ssOperateBarPos;
	ssOperateBarPos.Format(_T("%d,%d"), nOperateBarX, nOperateBarY);
	pOperateBar->SetAttribute(L"pos", ssOperateBarPos, FALSE);

	return true;
}

bool CSnapshotDlg::OnEventRectMoving(EventRectMoving* pEvt)
{
	SStatic* pText = FindChildByName2<SStatic>(L"text_title");
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SWindow* pOperateBar = FindChildByName2<SWindow>(L"operate_bar");
	SASSERT(pText);
	SASSERT(pSnapshot);
	SASSERT(pOperateBar);

	SOUI::CRect rcText = pText->GetWindowRect();
	SOUI::CRect rcWnd = GetWindowRect();
	SOUI::CRect rcCap = pSnapshot->GetCapRect();

	SStringW sstrTitle;
	sstrTitle.Format(L"起始位置：%d.%d  区域大小：%d × %d", rcCap.left, rcCap.top, rcCap.Width(), rcCap.Height());
	pText->SetWindowText(sstrTitle);

	int nX = rcCap.left;
	int nY = rcCap.top - rcText.Height() - 1;
	if (nY < 0)
		nY = rcCap.top ;

	if (rcWnd.right - nX < rcText.Width())
		nX = rcWnd.right - rcText.Width() - 1;

	SStringW sstrTitlePos;
	sstrTitlePos.Format(L"%d,%d", nX, nY);
	pText->SetAttribute(L"pos", sstrTitlePos);

	SOUI::CRect rcOperateBar = pOperateBar->GetWindowRect();
	int nOperateBarX = rcCap.right - rcOperateBar.Width();
	if (nOperateBarX < rcWnd.left)
		nOperateBarX = rcWnd.left;

	int nOperateBarY = 0;	
	if ((rcWnd.bottom - rcCap.bottom - 2) > rcOperateBar.Height())			// bottom
		nOperateBarY = rcCap.bottom + 2;
	else if ((rcCap.top - rcWnd.top - 2) > rcOperateBar.Height())				// top  有 空间
		nOperateBarY = rcCap.top - rcOperateBar.Height() - 2;
	else // 右上角 内 显示
		nOperateBarY = rcCap.top + 2;

	SStringW ssOperateBarPos;
	ssOperateBarPos.Format(_T("%d,%d"), nOperateBarX, nOperateBarY);
	pOperateBar->SetAttribute(L"pos", ssOperateBarPos, FALSE);

	return true;
}

bool CSnapshotDlg::OnEventRectCaptured(EventRectCaptured* pEvt)
{
	return true;
}

bool CSnapshotDlg::OnEventRectDbClk(EventRectDbClk* pEvt)
{
	EndDialog(IDOK);
	return true;
}

void CSnapshotDlg::OnBnClickRect()
{
	SImageButton* pImgBtnRect = FindChildByName2<SImageButton>(L"btn_rect");
	SImageButton* pImgBtnEllipse = FindChildByName2<SImageButton>(L"btn_ellipse");
	SImageButton* pImgBtnArrow = FindChildByName2<SImageButton>(L"btn_arrow");
	SImageButton* pImgBtnDoodle = FindChildByName2<SImageButton>(L"btn_doodle");
	SImageButton* pImgBtnWord = FindChildByName2<SImageButton>(L"btn_word");
	SImageButton* pImgBtnMask = FindChildByName2<SImageButton>(L"btn_mask");
	SASSERT(pImgBtnRect);
	SASSERT(pImgBtnEllipse);
	SASSERT(pImgBtnArrow);
	SASSERT(pImgBtnDoodle);
	SASSERT(pImgBtnMask);
	SASSERT(pImgBtnWord);

	pImgBtnRect->SetCheck(TRUE);
	pImgBtnEllipse->SetCheck(FALSE);
	pImgBtnArrow->SetCheck(FALSE);
	pImgBtnDoodle->SetCheck(FALSE);
	pImgBtnMask->SetCheck(FALSE);
	pImgBtnWord->SetCheck(FALSE);

	SWindow* pOperateBar = FindChildByName2<SWindow>(L"operate_bar");
	SWindow* pOtherAttr = FindChildByName2<SWindow>(L"other_attrbar");
	SASSERT(pOperateBar);
	SASSERT(pOtherAttr);

	if (!pOtherAttr->IsVisible())
		pOtherAttr->SetVisible(TRUE, FALSE);

	SImageButton* pImgBtnSmallDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_smalldot");
	SImageButton* pImgBtnMiddleDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_middledot");
	SImageButton* pImgBtnBigDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_bigdot");
	SASSERT(pImgBtnSmallDot);
	SASSERT(pImgBtnMiddleDot);
	SASSERT(pImgBtnBigDot);
	switch (m_nSelectSize)
	{
	case 1:
		{
			pImgBtnSmallDot->SetCheck(TRUE);
			pImgBtnMiddleDot->SetCheck(FALSE);
			pImgBtnBigDot->SetCheck(FALSE);
		}
		break;
	case 2:
		{
			pImgBtnSmallDot->SetCheck(FALSE);
			pImgBtnMiddleDot->SetCheck(TRUE);
			pImgBtnBigDot->SetCheck(FALSE);
		}
		break;
	case 5:
		{
			pImgBtnSmallDot->SetCheck(FALSE);
			pImgBtnMiddleDot->SetCheck(FALSE);
			pImgBtnBigDot->SetCheck(TRUE);
		}
		break;
	}

	SOUI::CRect rcOperateBar = pOperateBar->GetWindowRect();
	SOUI::CRect rcOtherAttr = pOtherAttr->GetWindowRect();
	int nOtherAttrX = rcOperateBar.right - rcOtherAttr.Width();
	int nOtherAttrY = rcOperateBar.bottom + 2;

	SStringW ssOtherAttrPos;
	ssOtherAttrPos.Format(_T("%d,%d"), nOtherAttrX, nOtherAttrY);
	pOtherAttr->SetAttribute(L"pos", ssOtherAttrPos, FALSE);

	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetOperateType(1);
}

void CSnapshotDlg::OnBnClickEllipse()
{
	SImageButton* pImgBtnRect = FindChildByName2<SImageButton>(L"btn_rect");
	SImageButton* pImgBtnEllipse = FindChildByName2<SImageButton>(L"btn_ellipse");
	SImageButton* pImgBtnArrow = FindChildByName2<SImageButton>(L"btn_arrow");
	SImageButton* pImgBtnDoodle = FindChildByName2<SImageButton>(L"btn_doodle");
	SImageButton* pImgBtnWord = FindChildByName2<SImageButton>(L"btn_word");
	SImageButton* pImgBtnMask = FindChildByName2<SImageButton>(L"btn_mask");
	SASSERT(pImgBtnRect);
	SASSERT(pImgBtnEllipse);
	SASSERT(pImgBtnArrow);
	SASSERT(pImgBtnDoodle);
	SASSERT(pImgBtnMask);
	SASSERT(pImgBtnWord);

	pImgBtnRect->SetCheck(FALSE);
	pImgBtnEllipse->SetCheck(TRUE);
	pImgBtnArrow->SetCheck(FALSE);
	pImgBtnDoodle->SetCheck(FALSE);
	pImgBtnMask->SetCheck(FALSE);
	pImgBtnWord->SetCheck(FALSE);

	SWindow* pOperateBar = FindChildByName2<SWindow>(L"operate_bar");
	SWindow* pOtherAttr = FindChildByName2<SWindow>(L"other_attrbar");
	SASSERT(pOperateBar);
	SASSERT(pOtherAttr);

	if (!pOtherAttr->IsVisible())
		pOtherAttr->SetVisible(TRUE, FALSE);

	SImageButton* pImgBtnSmallDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_smalldot");
	SImageButton* pImgBtnMiddleDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_middledot");
	SImageButton* pImgBtnBigDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_bigdot");
	SASSERT(pImgBtnSmallDot);
	SASSERT(pImgBtnMiddleDot);
	SASSERT(pImgBtnBigDot);
	switch (m_nSelectSize)
	{
	case 1:
		{
			pImgBtnSmallDot->SetCheck(TRUE);
			pImgBtnMiddleDot->SetCheck(FALSE);
			pImgBtnBigDot->SetCheck(FALSE);
		}
		break;
	case 2:
		{
			pImgBtnSmallDot->SetCheck(FALSE);
			pImgBtnMiddleDot->SetCheck(TRUE);
			pImgBtnBigDot->SetCheck(FALSE);
		}
		break;
	case 5:
		{
			pImgBtnSmallDot->SetCheck(FALSE);
			pImgBtnMiddleDot->SetCheck(FALSE);
			pImgBtnBigDot->SetCheck(TRUE);
		}
		break;
	}

	SOUI::CRect rcOperateBar = pOperateBar->GetWindowRect();
	SOUI::CRect rcOtherAttr = pOtherAttr->GetWindowRect();
	int nOtherAttrX = rcOperateBar.right - rcOtherAttr.Width();
	int nOtherAttrY = rcOperateBar.bottom + 2;

	SStringW ssOtherAttrPos;
	ssOtherAttrPos.Format(_T("%d,%d"), nOtherAttrX, nOtherAttrY);
	pOtherAttr->SetAttribute(L"pos", ssOtherAttrPos, FALSE);

	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetOperateType(2);
}

void CSnapshotDlg::OnBnClickArrow()
{
	SImageButton* pImgBtnRect = FindChildByName2<SImageButton>(L"btn_rect");
	SImageButton* pImgBtnEllipse = FindChildByName2<SImageButton>(L"btn_ellipse");
	SImageButton* pImgBtnArrow = FindChildByName2<SImageButton>(L"btn_arrow");
	SImageButton* pImgBtnDoodle = FindChildByName2<SImageButton>(L"btn_doodle");
	SImageButton* pImgBtnWord = FindChildByName2<SImageButton>(L"btn_word");
	SImageButton* pImgBtnMask = FindChildByName2<SImageButton>(L"btn_mask");
	SASSERT(pImgBtnRect);
	SASSERT(pImgBtnEllipse);
	SASSERT(pImgBtnArrow);
	SASSERT(pImgBtnDoodle);
	SASSERT(pImgBtnMask);
	SASSERT(pImgBtnWord);

	pImgBtnRect->SetCheck(FALSE);
	pImgBtnEllipse->SetCheck(FALSE);
	pImgBtnArrow->SetCheck(TRUE);
	pImgBtnDoodle->SetCheck(FALSE);
	pImgBtnMask->SetCheck(FALSE);
	pImgBtnWord->SetCheck(FALSE);

	SWindow* pOperateBar = FindChildByName2<SWindow>(L"operate_bar");
	SWindow* pOtherAttr = FindChildByName2<SWindow>(L"other_attrbar");
	SASSERT(pOperateBar);
	SASSERT(pOtherAttr);

	if (!pOtherAttr->IsVisible())
		pOtherAttr->SetVisible(TRUE, FALSE);

	SImageButton* pImgBtnSmallDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_smalldot");
	SImageButton* pImgBtnMiddleDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_middledot");
	SImageButton* pImgBtnBigDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_bigdot");
	SASSERT(pImgBtnSmallDot);
	SASSERT(pImgBtnMiddleDot);
	SASSERT(pImgBtnBigDot);
	switch (m_nSelectSize)
	{
	case 1:
		{
			pImgBtnSmallDot->SetCheck(TRUE);
			pImgBtnMiddleDot->SetCheck(FALSE);
			pImgBtnBigDot->SetCheck(FALSE);
		}
		break;
	case 2:
		{
			pImgBtnSmallDot->SetCheck(FALSE);
			pImgBtnMiddleDot->SetCheck(TRUE);
			pImgBtnBigDot->SetCheck(FALSE);
		}
		break;
	case 5:
		{
			pImgBtnSmallDot->SetCheck(FALSE);
			pImgBtnMiddleDot->SetCheck(FALSE);
			pImgBtnBigDot->SetCheck(TRUE);
		}
		break;
	}

	SOUI::CRect rcOperateBar = pOperateBar->GetWindowRect();
	SOUI::CRect rcOtherAttr = pOtherAttr->GetWindowRect();
	int nOtherAttrX = rcOperateBar.right - rcOtherAttr.Width();
	int nOtherAttrY = rcOperateBar.bottom + 2;

	SStringW ssOtherAttrPos;
	ssOtherAttrPos.Format(_T("%d,%d"), nOtherAttrX, nOtherAttrY);
	pOtherAttr->SetAttribute(L"pos", ssOtherAttrPos, FALSE);

	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetOperateType(3);
}

void CSnapshotDlg::OnBnClickDoodle()
{
	SImageButton* pImgBtnRect = FindChildByName2<SImageButton>(L"btn_rect");
	SImageButton* pImgBtnEllipse = FindChildByName2<SImageButton>(L"btn_ellipse");
	SImageButton* pImgBtnArrow = FindChildByName2<SImageButton>(L"btn_arrow");
	SImageButton* pImgBtnDoodle = FindChildByName2<SImageButton>(L"btn_doodle");
	SImageButton* pImgBtnWord = FindChildByName2<SImageButton>(L"btn_word");
	SImageButton* pImgBtnMask = FindChildByName2<SImageButton>(L"btn_mask");
	SASSERT(pImgBtnRect);
	SASSERT(pImgBtnEllipse);
	SASSERT(pImgBtnArrow);
	SASSERT(pImgBtnDoodle);
	SASSERT(pImgBtnMask);
	SASSERT(pImgBtnWord);

	pImgBtnRect->SetCheck(FALSE);
	pImgBtnEllipse->SetCheck(FALSE);
	pImgBtnArrow->SetCheck(FALSE);
	pImgBtnDoodle->SetCheck(TRUE);
	pImgBtnMask->SetCheck(FALSE);
	pImgBtnWord->SetCheck(FALSE);

	SWindow* pOperateBar = FindChildByName2<SWindow>(L"operate_bar");
	SWindow* pOtherAttr = FindChildByName2<SWindow>(L"other_attrbar");
	SASSERT(pOperateBar);
	SASSERT(pOtherAttr);

	if (!pOtherAttr->IsVisible())
		pOtherAttr->SetVisible(TRUE, FALSE);

	SImageButton* pImgBtnSmallDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_smalldot");
	SImageButton* pImgBtnMiddleDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_middledot");
	SImageButton* pImgBtnBigDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_bigdot");
	SASSERT(pImgBtnSmallDot);
	SASSERT(pImgBtnMiddleDot);
	SASSERT(pImgBtnBigDot);
	switch (m_nSelectSize)
	{
	case 1:
		{
			pImgBtnSmallDot->SetCheck(TRUE);
			pImgBtnMiddleDot->SetCheck(FALSE);
			pImgBtnBigDot->SetCheck(FALSE);
		}
		break;
	case 2:
		{
			pImgBtnSmallDot->SetCheck(FALSE);
			pImgBtnMiddleDot->SetCheck(TRUE);
			pImgBtnBigDot->SetCheck(FALSE);
		}
		break;
	case 5:
		{
			pImgBtnSmallDot->SetCheck(FALSE);
			pImgBtnMiddleDot->SetCheck(FALSE);
			pImgBtnBigDot->SetCheck(TRUE);
		}
		break;
	}

	SOUI::CRect rcOperateBar = pOperateBar->GetWindowRect();
	SOUI::CRect rcOtherAttr = pOtherAttr->GetWindowRect();
	int nOtherAttrX = rcOperateBar.right - rcOtherAttr.Width();
	int nOtherAttrY = rcOperateBar.bottom + 2;

	SStringW ssOtherAttrPos;
	ssOtherAttrPos.Format(_T("%d,%d"), nOtherAttrX, nOtherAttrY);
	pOtherAttr->SetAttribute(L"pos", ssOtherAttrPos, FALSE);

	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetOperateType(4);
}

void CSnapshotDlg::OnBnClickMask()
{
	SImageButton* pImgBtnRect = FindChildByName2<SImageButton>(L"btn_rect");
	SImageButton* pImgBtnEllipse = FindChildByName2<SImageButton>(L"btn_ellipse");
	SImageButton* pImgBtnArrow = FindChildByName2<SImageButton>(L"btn_arrow");
	SImageButton* pImgBtnDoodle = FindChildByName2<SImageButton>(L"btn_doodle");
	SImageButton* pImgBtnWord = FindChildByName2<SImageButton>(L"btn_word");
	SImageButton* pImgBtnMask = FindChildByName2<SImageButton>(L"btn_mask");
	SASSERT(pImgBtnRect);
	SASSERT(pImgBtnEllipse);
	SASSERT(pImgBtnArrow);
	SASSERT(pImgBtnDoodle);
	SASSERT(pImgBtnMask);
	SASSERT(pImgBtnWord);

	pImgBtnRect->SetCheck(FALSE);
	pImgBtnEllipse->SetCheck(FALSE);
	pImgBtnArrow->SetCheck(FALSE);
	pImgBtnDoodle->SetCheck(FALSE);
	pImgBtnMask->SetCheck(TRUE);
	pImgBtnWord->SetCheck(FALSE);

	SWindow* pOperateBar = FindChildByName2<SWindow>(L"operate_bar");
	SWindow* pOtherAttr = FindChildByName2<SWindow>(L"other_attrbar");
	SASSERT(pOperateBar);
	SASSERT(pOtherAttr);

	if (!pOtherAttr->IsVisible())
		pOtherAttr->SetVisible(TRUE, FALSE);

	SImageButton* pImgBtnSmallDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_smalldot");
	SImageButton* pImgBtnMiddleDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_middledot");
	SImageButton* pImgBtnBigDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_bigdot");
	SASSERT(pImgBtnSmallDot);
	SASSERT(pImgBtnMiddleDot);
	SASSERT(pImgBtnBigDot);
	switch (m_nSelectSize)
	{
	case 1:
		{
			pImgBtnSmallDot->SetCheck(TRUE);
			pImgBtnMiddleDot->SetCheck(FALSE);
			pImgBtnBigDot->SetCheck(FALSE);
		}
		break;
	case 2:
		{
			pImgBtnSmallDot->SetCheck(FALSE);
			pImgBtnMiddleDot->SetCheck(TRUE);
			pImgBtnBigDot->SetCheck(FALSE);
		}
		break;
	case 5:
		{
			pImgBtnSmallDot->SetCheck(FALSE);
			pImgBtnMiddleDot->SetCheck(FALSE);
			pImgBtnBigDot->SetCheck(TRUE);
		}
		break;
	}

	SOUI::CRect rcOperateBar = pOperateBar->GetWindowRect();
	SOUI::CRect rcOtherAttr = pOtherAttr->GetWindowRect();
	int nOtherAttrX = rcOperateBar.right - rcOtherAttr.Width();
	int nOtherAttrY = rcOperateBar.bottom + 2;

	SStringW ssOtherAttrPos;
	ssOtherAttrPos.Format(_T("%d,%d"), nOtherAttrX, nOtherAttrY);
	pOtherAttr->SetAttribute(L"pos", ssOtherAttrPos, FALSE);

	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetOperateType(5);
}

void CSnapshotDlg::OnBnClickWord()
{
	SImageButton* pImgBtnRect = FindChildByName2<SImageButton>(L"btn_rect");
	SImageButton* pImgBtnEllipse = FindChildByName2<SImageButton>(L"btn_ellipse");
	SImageButton* pImgBtnArrow = FindChildByName2<SImageButton>(L"btn_arrow");
	SImageButton* pImgBtnDoodle = FindChildByName2<SImageButton>(L"btn_doodle");
	SImageButton* pImgBtnWord = FindChildByName2<SImageButton>(L"btn_word");
	SImageButton* pImgBtnMask = FindChildByName2<SImageButton>(L"btn_mask");
	SASSERT(pImgBtnRect);
	SASSERT(pImgBtnEllipse);
	SASSERT(pImgBtnArrow);
	SASSERT(pImgBtnDoodle);
	SASSERT(pImgBtnMask);
	SASSERT(pImgBtnWord);

	pImgBtnRect->SetCheck(FALSE);
	pImgBtnEllipse->SetCheck(FALSE);
	pImgBtnArrow->SetCheck(FALSE);
	pImgBtnDoodle->SetCheck(FALSE);
	pImgBtnMask->SetCheck(FALSE);
	pImgBtnWord->SetCheck(TRUE);

	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetOperateType(6);
}

void CSnapshotDlg::OnBnClickRevoke()
{
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->RevokeOperate();
}

void CSnapshotDlg::OnBnClickCopy()
{
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SaveCapBmpToClipboard();
}

void CSnapshotDlg::OnBnClickSave()
{
	//
}

void CSnapshotDlg::OnBnClickCancel()
{
	EndDialog(IDOK);
}

void CSnapshotDlg::OnBnClickFinish()
{
	//
}

void CSnapshotDlg::OnBnClickSmallDot()
{
	SWindow* pOtherAttr = FindChildByName2<SWindow>(L"other_attrbar");
	SASSERT(pOtherAttr);
	SImageButton* pImgBtnSmallDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_smalldot");
	SImageButton* pImgBtnMiddleDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_middledot");
	SImageButton* pImgBtnBigDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_bigdot");
	SASSERT(pImgBtnSmallDot);
	SASSERT(pImgBtnMiddleDot);
	SASSERT(pImgBtnBigDot);
	m_nSelectSize = 1;
	pImgBtnSmallDot->SetCheck(TRUE);
	pImgBtnMiddleDot->SetCheck(FALSE);
	pImgBtnBigDot->SetCheck(FALSE);

	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenSize(m_nSelectSize);
}

void CSnapshotDlg::OnBnClickMiddleDot()
{
	SWindow* pOtherAttr = FindChildByName2<SWindow>(L"other_attrbar");
	SASSERT(pOtherAttr);
	SImageButton* pImgBtnSmallDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_smalldot");
	SImageButton* pImgBtnMiddleDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_middledot");
	SImageButton* pImgBtnBigDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_bigdot");
	SASSERT(pImgBtnSmallDot);
	SASSERT(pImgBtnMiddleDot);
	SASSERT(pImgBtnBigDot);
	m_nSelectSize = 2;
	pImgBtnSmallDot->SetCheck(FALSE);
	pImgBtnMiddleDot->SetCheck(TRUE);
	pImgBtnBigDot->SetCheck(FALSE);

	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenSize(m_nSelectSize);
}

void CSnapshotDlg::OnBnClickBigDot()
{
	SWindow* pOtherAttr = FindChildByName2<SWindow>(L"other_attrbar");
	SASSERT(pOtherAttr);
	SImageButton* pImgBtnSmallDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_smalldot");
	SImageButton* pImgBtnMiddleDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_middledot");
	SImageButton* pImgBtnBigDot = pOtherAttr->FindChildByName2<SImageButton>(L"btn_bigdot");
	SASSERT(pImgBtnSmallDot);
	SASSERT(pImgBtnMiddleDot);
	SASSERT(pImgBtnBigDot);
	m_nSelectSize = 5;
	pImgBtnSmallDot->SetCheck(FALSE);
	pImgBtnMiddleDot->SetCheck(FALSE);
	pImgBtnBigDot->SetCheck(TRUE);

	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenSize(m_nSelectSize);
}

void CSnapshotDlg::OnBnClickC1()
{
	COLORREF& color = m_mapColorInfo[1];
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenColor(color);

	SetSelectedColor(1);
}

void CSnapshotDlg::OnBnClickC2()
{
	COLORREF& color = m_mapColorInfo[2];
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenColor(color);

	SetSelectedColor(2);
}

void CSnapshotDlg::OnBnClickC3()
{
	COLORREF& color = m_mapColorInfo[3];
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenColor(color);

	SetSelectedColor(3);
}

void CSnapshotDlg::OnBnClickC4()
{
	COLORREF& color = m_mapColorInfo[4];
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenColor(color);

	SetSelectedColor(4);
}

void CSnapshotDlg::OnBnClickC5()
{
	COLORREF& color = m_mapColorInfo[5];
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenColor(color);

	SetSelectedColor(5);
}

void CSnapshotDlg::OnBnClickC6()
{
	COLORREF& color = m_mapColorInfo[6];
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenColor(color);

	SetSelectedColor(6);
}

void CSnapshotDlg::OnBnClickC7()
{
	COLORREF& color = m_mapColorInfo[7];
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenColor(color);

	SetSelectedColor(7);
}

void CSnapshotDlg::OnBnClickC8()
{
	COLORREF& color = m_mapColorInfo[8];
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenColor(color);

	SetSelectedColor(8);
}

void CSnapshotDlg::OnBnClickC9()
{
	COLORREF& color = m_mapColorInfo[9];
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenColor(color);

	SetSelectedColor(9);
}

void CSnapshotDlg::OnBnClickC10()
{
	COLORREF& color = m_mapColorInfo[10];
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenColor(color);

	SetSelectedColor(10);
}

void CSnapshotDlg::OnBnClickC11()
{
	COLORREF& color = m_mapColorInfo[11];
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenColor(color);

	SetSelectedColor(11);
}

void CSnapshotDlg::OnBnClickC12()
{
	COLORREF& color = m_mapColorInfo[12];
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenColor(color);

	SetSelectedColor(12);
}

void CSnapshotDlg::OnBnClickC13()
{
	COLORREF& color = m_mapColorInfo[13];
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenColor(color);

	SetSelectedColor(13);
}

void CSnapshotDlg::OnBnClickC14()
{
	COLORREF& color = m_mapColorInfo[14];
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenColor(color);

	SetSelectedColor(14);
}

void CSnapshotDlg::OnBnClickC15()
{
	COLORREF& color = m_mapColorInfo[15];
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenColor(color);

	SetSelectedColor(15);
}

void CSnapshotDlg::OnBnClickC16()
{
	COLORREF& color = m_mapColorInfo[16];
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenColor(color);

	SetSelectedColor(16);
}

void CSnapshotDlg::OnBnClickC17()
{
	COLORREF& color = m_mapColorInfo[17];
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenColor(color);

	SetSelectedColor(17);
}

void CSnapshotDlg::OnBnClickC18()
{
	COLORREF& color = m_mapColorInfo[18];
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenColor(color);

	SetSelectedColor(18);
}

void CSnapshotDlg::OnBnClickC19()
{
	COLORREF& color = m_mapColorInfo[19];
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenColor(color);

	SetSelectedColor(19);
}

void CSnapshotDlg::OnBnClickC20()
{
	COLORREF& color = m_mapColorInfo[20];
	SSnapshotCtrl* pSnapshot = FindChildByName2<SSnapshotCtrl>(L"snapshot");
	SASSERT(pSnapshot);
	pSnapshot->SetPenColor(color);
	SetSelectedColor(20);
}

void CSnapshotDlg::SetSelectedColor(int nIndex)
{
	SWindow* pOtherAttr = FindChildByName2<SWindow>(L"other_attrbar");
	SASSERT(pOtherAttr);
	SWindow* pWindow = FindChildByName2<SWindow>(L"selected_window");
	SASSERT(pWindow);
	SWindow* pSelectColor = pWindow->FindChildByName2<SWindow>(L"selected_color");
	SASSERT(pSelectColor);
	SStringW sstrColor;
	sstrColor.Format(L"RGB(%d,%d,%d)", GetRValue(m_mapColorInfo[nIndex]), GetGValue(m_mapColorInfo[nIndex]), GetBValue(m_mapColorInfo[nIndex]));
	pSelectColor->SetAttribute(L"colorBkgnd", sstrColor, FALSE);
}