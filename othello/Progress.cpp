#include "framework.h"
#include "commctrl.h"
#include "othello.hpp"

extern Display display;
extern Gaming gaming;

//
//	Function Name: Progress
//	
INT_PTR CALLBACK Progress(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	WCHAR strW[100];
	UNREFERENCED_PARAMETER(lParam);
	static HWND hProgressBar1 = NULL;
	static HWND hProgressBar2 = NULL;
	static HWND hProgressBar3 = NULL;
	static HWND hProgressBar4 = NULL;
	int ret;

	switch (message)
	{
	case WM_INITDIALOG:
	{
		hProgressBar1 = GetDlgItem(hDlg, IDC_PROGRESS1);
		if (hProgressBar1){
			SendMessage(hProgressBar1, PBM_SETRANGE32, 0, gaming.numRepeatTotal);
			SendMessage(hProgressBar1, PBM_SETPOS, 0, 0);
		}

		hProgressBar2 = GetDlgItem(hDlg, IDC_PROGRESS2);
		if (hProgressBar2) {
			SendMessage(hProgressBar2, PBM_SETRANGE32, 0, 1000);
			SendMessage(hProgressBar2, PBM_SETPOS, 0, 0);
		}

		hProgressBar3 = GetDlgItem(hDlg, IDC_PROGRESS3);
		if (hProgressBar3) {
			SendMessage(hProgressBar3, PBM_SETRANGE32, 0, 1000);
			SendMessage(hProgressBar3, PBM_SETPOS, 0, 0);
		}

		hProgressBar4 = GetDlgItem(hDlg, IDC_PROGRESS4);
		if (hProgressBar4) {
			SendMessage(hProgressBar4, PBM_SETRANGE32, 0, 1000);
			SendMessage(hProgressBar4, PBM_SETPOS, 0, 0);
		}
	}
	case WM_COMMAND:
	{
		if (LOWORD(wParam) == IDOK) {
			EndDialog(hDlg, IDOK);
			display.updateShowProgressOnMenu();
		}
		break;
	}
	case WM_PAINT:
	{
		// テキスト更新
		swprintf_s(strW, _countof(strW), L"%d/%d", gaming.getNumGames(), gaming.numRepeatTotal);
		SetDlgItemText(hDlg, IDC_STATIC5, strW);

		double winRate = (double)gaming.calcWinRate(DISKCOLORS::COLOR_BLACK) / 10.0;
		swprintf_s(strW, _countof(strW), L"%.1f", winRate);
		SetDlgItemText(hDlg, IDC_STATIC6, strW);

		winRate = (double)gaming.calcWinRate(DISKCOLORS::COLOR_WHITE) / 10.0;
		swprintf_s(strW, _countof(strW), L"%.1f", winRate);
		SetDlgItemText(hDlg, IDC_STATIC7, strW);

		winRate = (double)gaming.calcWinRate(DISKCOLORS::COLOR_NONE) / 10.0;
		swprintf_s(strW, _countof(strW), L"%.1f", winRate);
		SetDlgItemText(hDlg, IDC_STATIC8, strW);

		// プログレスバー更新
		if (hProgressBar1) SendMessage(hProgressBar1, PBM_SETPOS, gaming.getNumGames(), 0);
		if (hProgressBar2) SendMessage(hProgressBar2, PBM_SETPOS, gaming.calcWinRate(DISKCOLORS::COLOR_BLACK), 0);
		if (hProgressBar3) SendMessage(hProgressBar3, PBM_SETPOS, gaming.calcWinRate(DISKCOLORS::COLOR_WHITE), 0);
		if (hProgressBar4) SendMessage(hProgressBar4, PBM_SETPOS, gaming.calcWinRate(DISKCOLORS::COLOR_NONE), 0);

		break;
	}
	case WM_CLOSE:
		display.hProgressDialog = NULL;
		EndDialog(hDlg, IDOK);
		display.updateShowProgressOnMenu();
		break;
	default:
		break;
	}

	return (INT_PTR)FALSE;
}