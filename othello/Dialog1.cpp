#include <stdio.h>
#include "framework.h"
#include "think.hpp"
#include "externalThinkerMessages.hpp"
#include "othello.hpp"
#include "externalThinkerHandler.hpp"
#include "history.hpp"

extern Display display;
extern Gaming gaming;
extern ExternalThinkerHandler externalThinkerHandler[2];	// 0: Black, 1: White
extern History history;

//
//	Function Name: Dialog1
//	Summary: Callback function when selecting "New Game" in "File" menu.
//	
INT_PTR CALLBACK Dialog1(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	static DIALOG_STATES dialogState = DIALOG_STATES::STATE_INIT;
	int ret;

	switch (message)
	{
	case WM_INITDIALOG:
	{
		// Check to default for Black and White
		switch (gaming.getPlayerType(PLAYERINDEX::PLAYERINDEX_BLACK)) {
		case PLAYERTYPE::PLAYERTYPE_USER:
			SendMessage(GetDlgItem(hDlg, IDC_RADIO1), BM_SETCHECK, BST_CHECKED, 0);
			break;
		case PLAYERTYPE::PLAYERTYPE_COMPUTER_EMBEDED:
			SendMessage(GetDlgItem(hDlg, IDC_RADIO2), BM_SETCHECK, BST_CHECKED, 0);
			break;
		case PLAYERTYPE::PLAYERTYPE_COMPUTER_EXTERNAL:
			SendMessage(GetDlgItem(hDlg, IDC_RADIO3), BM_SETCHECK, BST_CHECKED, 0);
			break;
		default:
			SendMessage(GetDlgItem(hDlg, IDC_RADIO1), BM_SETCHECK, BST_CHECKED, 0);
			break;
		}

		switch (gaming.getPlayerType(PLAYERINDEX::PLAYERINDEX_WHITE)) {
		case PLAYERTYPE::PLAYERTYPE_USER:
			SendMessage(GetDlgItem(hDlg, IDC_RADIO4), BM_SETCHECK, BST_CHECKED, 0);
			break;
		case PLAYERTYPE::PLAYERTYPE_COMPUTER_EMBEDED:
			SendMessage(GetDlgItem(hDlg, IDC_RADIO5), BM_SETCHECK, BST_CHECKED, 0);
			break;
		case PLAYERTYPE::PLAYERTYPE_COMPUTER_EXTERNAL:
			SendMessage(GetDlgItem(hDlg, IDC_RADIO6), BM_SETCHECK, BST_CHECKED, 0);
			break;
		default:
			SendMessage(GetDlgItem(hDlg, IDC_RADIO4), BM_SETCHECK, BST_CHECKED, 0);
			break;
		}

		// Limit the text length for port edit boxes
		HWND chwnd = GetDlgItem(hDlg, IDC_EDIT2);
		int size = sizeof(char) * 5;
		SendMessage(chwnd, EM_SETLIMITTEXT, (WPARAM)size, 0);

		chwnd = GetDlgItem(hDlg, IDC_EDIT4);
		SendMessage(chwnd, EM_SETLIMITTEXT, (WPARAM)size, 0);

		// Set initial texts
		SetWindowText(GetDlgItem(hDlg, IDC_EDIT1), gaming.playerInfo[(size_t)PLAYERINDEX::PLAYERINDEX_BLACK].sHostname);
		SetWindowText(GetDlgItem(hDlg, IDC_EDIT2), gaming.playerInfo[(size_t)PLAYERINDEX::PLAYERINDEX_BLACK].sPort);
		SetWindowText(GetDlgItem(hDlg, IDC_EDIT3), gaming.playerInfo[(size_t)PLAYERINDEX::PLAYERINDEX_WHITE].sHostname);
		SetWindowText(GetDlgItem(hDlg, IDC_EDIT4), gaming.playerInfo[(size_t)PLAYERINDEX::PLAYERINDEX_WHITE].sPort);

		WCHAR sNumRepeat[16];
		_itow_s(gaming.numRepeat, sNumRepeat, _countof(sNumRepeat), 10);
		SetWindowText(GetDlgItem(hDlg, IDC_EDIT5), sNumRepeat);

		// Set auto repeat checkbox
		if (gaming.autoRepeat == true) {
			SendMessage(GetDlgItem(hDlg, IDC_CHECK1), BM_SETCHECK, BST_CHECKED, 0);
		}
		else {
			SendMessage(GetDlgItem(hDlg, IDC_CHECK1), BM_SETCHECK, BST_UNCHECKED, 0);
		}

		// Set limited repeat
		if (gaming.bLimitedRepeating == true) {
			SendMessage(GetDlgItem(hDlg, IDC_CHECK3), BM_SETCHECK, BST_CHECKED, 0);
		}
		else {
			SendMessage(GetDlgItem(hDlg, IDC_CHECK3), BM_SETCHECK, BST_UNCHECKED, 0);
		}

		// Set timer for waiting the response
		if (gaming.autoRepeat == true) {
			UINT_PTR TimerIdWaitDisablingAutoRepeat;
			TimerIdWaitDisablingAutoRepeat = SetTimer(hDlg, (INT_PTR)TIMERID::WAIT_DISABLING_AUTOREPEAT, WAIT_TIME_DISABLING_AUTOREPEAT * 1000, NULL);
		}

		// Update state
		dialogState = DIALOG_STATES::STATE_INIT;
		return (INT_PTR)TRUE;
	}
	case WM_COMMAND:
	{
		if (LOWORD(wParam) == IDOK) {
			// Disable timer
			KillTimer(hDlg, (INT_PTR)TIMERID::WAIT_DISABLING_AUTOREPEAT);

			// Check radio buttons for black
			if (SendMessage(GetDlgItem(hDlg, IDC_RADIO1), BM_GETCHECK, 0, 0) == BST_CHECKED) {
				gaming.setPlayerType(PLAYERINDEX::PLAYERINDEX_BLACK, PLAYERTYPE::PLAYERTYPE_USER);
			}
			else if (SendMessage(GetDlgItem(hDlg, IDC_RADIO2), BM_GETCHECK, 0, 0) == BST_CHECKED) {
				gaming.setPlayerType(PLAYERINDEX::PLAYERINDEX_BLACK, PLAYERTYPE::PLAYERTYPE_COMPUTER_EMBEDED);
			}
			else if (SendMessage(GetDlgItem(hDlg, IDC_RADIO3), BM_GETCHECK, 0, 0) == BST_CHECKED) {
				gaming.setPlayerType(PLAYERINDEX::PLAYERINDEX_BLACK, PLAYERTYPE::PLAYERTYPE_COMPUTER_EXTERNAL);
			}

			// Check radio buttons for white
			if (SendMessage(GetDlgItem(hDlg, IDC_RADIO4), BM_GETCHECK, 0, 0) == BST_CHECKED) {
				gaming.setPlayerType(PLAYERINDEX::PLAYERINDEX_WHITE, PLAYERTYPE::PLAYERTYPE_USER);
			}
			else if (SendMessage(GetDlgItem(hDlg, IDC_RADIO5), BM_GETCHECK, 0, 0) == BST_CHECKED) {
				gaming.setPlayerType(PLAYERINDEX::PLAYERINDEX_WHITE, PLAYERTYPE::PLAYERTYPE_COMPUTER_EMBEDED);
			}
			else if (SendMessage(GetDlgItem(hDlg, IDC_RADIO6), BM_GETCHECK, 0, 0) == BST_CHECKED) {
				gaming.setPlayerType(PLAYERINDEX::PLAYERINDEX_WHITE, PLAYERTYPE::PLAYERTYPE_COMPUTER_EXTERNAL);
			}

			// Get hostname and port number in the dialog box
			GetDlgItemText(hDlg, IDC_EDIT1, (TCHAR*)gaming.playerInfo[(size_t)PLAYERINDEX::PLAYERINDEX_BLACK].sHostname, sizeof(gaming.playerInfo[(size_t)PLAYERINDEX::PLAYERINDEX_BLACK].sHostname) / sizeof(TCHAR));
			GetDlgItemText(hDlg, IDC_EDIT2, (TCHAR*)gaming.playerInfo[(size_t)PLAYERINDEX::PLAYERINDEX_BLACK].sPort, sizeof(gaming.playerInfo[(size_t)PLAYERINDEX::PLAYERINDEX_BLACK].sPort) / sizeof(TCHAR));
			GetDlgItemText(hDlg, IDC_EDIT3, (TCHAR*)gaming.playerInfo[(size_t)PLAYERINDEX::PLAYERINDEX_WHITE].sHostname, sizeof(gaming.playerInfo[(size_t)PLAYERINDEX::PLAYERINDEX_WHITE].sHostname) / sizeof(TCHAR));
			GetDlgItemText(hDlg, IDC_EDIT4, (TCHAR*)gaming.playerInfo[(size_t)PLAYERINDEX::PLAYERINDEX_WHITE].sPort, sizeof(gaming.playerInfo[(size_t)PLAYERINDEX::PLAYERINDEX_WHITE].sPort) / sizeof(TCHAR));

			// Get auto repeat checkbox
			if (SendMessage(GetDlgItem(hDlg, IDC_CHECK1), BM_GETCHECK, 0, 0) == BST_CHECKED) {
				gaming.autoRepeat = true;
			}
			else {
				gaming.autoRepeat = false;
			}

			// Get limited repeating
			if (SendMessage(GetDlgItem(hDlg, IDC_CHECK3), BM_GETCHECK, 0, 0) == BST_CHECKED) {
				gaming.bLimitedRepeating = true;
			}
			else {
				gaming.bLimitedRepeating = false;
			}

			WCHAR sNumRepeat[16];
			GetDlgItemText(hDlg, IDC_EDIT5, (TCHAR*)sNumRepeat, _countof(sNumRepeat));
			gaming.numRepeat = _wtoi(sNumRepeat);

			// Update auto repeat check on menu
			display.setAutoRepeatOnMenu(gaming.autoRepeat);

			// Check edit boxes to input external thinker for black
			// If valid strings are set, check if valid response is received from external thinker or not.
			if (gaming.getPlayerType(PLAYERINDEX::PLAYERINDEX_BLACK) == PLAYERTYPE::PLAYERTYPE_COMPUTER_EXTERNAL) {
				// Fetch the input data for black and send Information Request
				ret = checkExternalThinker(hDlg, PLAYERINDEX::PLAYERINDEX_BLACK);

				if (ret == 0) {
					// Update state
					dialogState = DIALOG_STATES::STATE_WAITING_INFORESP_BLACK;
				}
			}
			// Check edit boxes to input external thinker for white
			// If valid strings are set, check if valid response is received from external thinker or not.
			else if (gaming.getPlayerType(PLAYERINDEX::PLAYERINDEX_WHITE) == PLAYERTYPE::PLAYERTYPE_COMPUTER_EXTERNAL) {
				// Black is not external thinker, so clear black's external thinker state
				externalThinkerHandler[(int)PLAYERINDEX::PLAYERINDEX_BLACK].init(PLAYERINDEX::PLAYERINDEX_BLACK);

				// Fetch the input data for white and send Information Request
				ret = checkExternalThinker(hDlg, PLAYERINDEX::PLAYERINDEX_WHITE);

				if (ret == 0) {
					// Update state
					dialogState = DIALOG_STATES::STATE_WAITING_INFORESP_WHITE;
				}
			}
			else {
				// Update state
				dialogState = DIALOG_STATES::STATE_END;

				// Clear External Thinker to remove previous game's state
				externalThinkerHandler[0].init(PLAYERINDEX::PLAYERINDEX_BLACK);
				externalThinkerHandler[1].init(PLAYERINDEX::PLAYERINDEX_WHITE);

				// Start the game
				StartGame(hDlg);
			}
		}
		else if (LOWORD(wParam) == IDCANCEL) {
			// Disable timer
			KillTimer(hDlg, (INT_PTR)TIMERID::WAIT_DISABLING_AUTOREPEAT);

			// Set Off to Auto Repeat
			gaming.autoRepeat = false;

			// Update auto repeat check on menu
			display.setAutoRepeatOnMenu(gaming.autoRepeat);
		}

		// Closing the Dialog box
		if ((LOWORD(wParam) == IDOK && dialogState == DIALOG_STATES::STATE_END) || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	case WSOCK_SELECT:
	{
		// Check state
		switch (dialogState) {
		case DIALOG_STATES::STATE_INIT:
			// Abnormal behaviour, do nothing.
			break;
		case DIALOG_STATES::STATE_WAITING_INFORESP_BLACK:
		{
			// Receive the message
			int ret = externalThinkerHandler[(int)PLAYERINDEX::PLAYERINDEX_BLACK].receiveMessages();

			// If successful message is received, go forward
			if (ret == 0) {
				// Cancel the timer
				KillTimer(hDlg, (INT_PTR)TIMERID::WAIT_INFO_RESP);

				// Update window handle from Dialog1 to main window for Black
				externalThinkerHandler[(int)PLAYERINDEX::PLAYERINDEX_BLACK].setWindowHandle(GetWindow(hDlg, GW_OWNER));

				// If white is also external thinker, check whether it is available or not.
				// If white is not external thinker, start gaming.
				if (gaming.getPlayerType(PLAYERINDEX::PLAYERINDEX_WHITE) == PLAYERTYPE::PLAYERTYPE_COMPUTER_EXTERNAL) {
					// Call ExternalThinkerHandler.init() for white
					ret = checkExternalThinker(hDlg, PLAYERINDEX::PLAYERINDEX_WHITE);

					if (ret == 0) {
						// Update state
						dialogState = DIALOG_STATES::STATE_WAITING_INFORESP_WHITE;
					}
				}
				else {
					// White is not external thinker, so clear previous external thinker status for white
					externalThinkerHandler[(int)PLAYERINDEX::PLAYERINDEX_WHITE].init(PLAYERINDEX::PLAYERINDEX_WHITE);

					// Start the game
					StartGame(hDlg);

					// End dialog
					EndDialog(hDlg, LOWORD(wParam));
					return (INT_PTR)TRUE;
				}
			}
			break;
		}
		case DIALOG_STATES::STATE_WAITING_INFORESP_WHITE:
		{
			// Receive the message
			int ret = externalThinkerHandler[(int)PLAYERINDEX::PLAYERINDEX_WHITE].receiveMessages();

			// If successful message is received, go forward
			if (ret == 0) {
				// Cancel the timer
				KillTimer(hDlg, (INT_PTR)TIMERID::WAIT_INFO_RESP);

				// Update window handle from Dialog1 to main window for Black
				externalThinkerHandler[(int)PLAYERINDEX::PLAYERINDEX_WHITE].setWindowHandle(GetWindow(hDlg, GW_OWNER));

				// Start the game
				StartGame(hDlg);

				// End dialog
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}

			break;
		}
		case DIALOG_STATES::STATE_END:
			// Abnormal behaviour, do nothing.
			break;
		}
		break;
	}
	case WM_TIMER:
	{
		// Check Timer ID
		if (wParam == (WPARAM)TIMERID::WAIT_INFO_RESP) {
			switch (dialogState) {
			case DIALOG_STATES::STATE_INIT:
				// Abnormal behaviour, do nothing.
				break;
			case DIALOG_STATES::STATE_WAITING_INFORESP_BLACK:
			{
				KillTimer(hDlg, (INT_PTR)TIMERID::WAIT_INFO_RESP);
				dialogState = DIALOG_STATES::STATE_INIT;
				MessageBox(hDlg, TEXT("External thinker for Black is not ready."), TEXT("Error"), MB_OK | MB_ICONWARNING);

				// Enable "OK" and "Cancel" button
				HWND button = GetDlgItem(hDlg, IDOK);
				EnableWindow(button, TRUE);
				button = GetDlgItem(hDlg, IDCANCEL);
				EnableWindow(button, TRUE);
				break;
			}
			case DIALOG_STATES::STATE_WAITING_INFORESP_WHITE:
			{
				KillTimer(hDlg, (INT_PTR)TIMERID::WAIT_INFO_RESP);
				dialogState = DIALOG_STATES::STATE_INIT;
				MessageBox(hDlg, TEXT("External thinker for White is not ready."), TEXT("Error"), MB_OK | MB_ICONWARNING);

				// Disable "OK" and "Cancel" button
				HWND button = GetDlgItem(hDlg, IDOK);
				EnableWindow(button, TRUE);
				button = GetDlgItem(hDlg, IDCANCEL);
				EnableWindow(button, TRUE);
				break;
			}
			case DIALOG_STATES::STATE_END:
				// Abnormal behaviour, do nothing.
				break;
			}
		}
		else if (wParam == (WPARAM)TIMERID::WAIT_DISABLING_AUTOREPEAT) {
			KillTimer(hDlg, (INT_PTR)TIMERID::WAIT_DISABLING_AUTOREPEAT);

			// Get auto repeat checkbox
			if (SendMessage(GetDlgItem(hDlg, IDC_CHECK1), BM_GETCHECK, 0, 0) == BST_CHECKED) {
				// Push "OK" button automatically
				PostMessage(hDlg, WM_COMMAND, IDOK, 0);
				gaming.autoRepeat = true;

				// Update auto repeat check on menu
				display.setAutoRepeatOnMenu(gaming.autoRepeat);
			}
		}
		else {
			// Abnormal behaviour, do nothing.
		}
	}
	default:
		break;
	}

	return (INT_PTR)FALSE;
}

//
//	Function Name: checkExternalThinker
//	Summary: Fetch the input data in the dialog box and send Information Request message to the external thinker.
//	
//	In:
//		hDlg			Handle for Dialog box
//		IDCHostName		ID for host name in the dialog box
//		IDCPort			ID for port number in the dialog box
//		playerIndex		Player (Black or White)
//
//	Return:
//		0	Success
//		-1	Hostname and/or port number is/are not specified.
//		-2	Hostname and/or port number is/are wrong.
//		-3	Failed to prepare sockets to the hostname and the port number.
//		-4	Faeild to send Information Request.
//
int checkExternalThinker(HWND hDlg, PLAYERINDEX playerIndex)
{
	// Check for Edit boxes if the player type is PLAYERTYPE_COMPUTER_EXTERNAL
	char hostname[256];
	int port;

	// Check if both hostname and port are specified or not
	if (_tcslen(gaming.playerInfo[(size_t)playerIndex].sHostname) == 0 || _tcslen(gaming.playerInfo[(size_t)playerIndex].sPort) == 0) {
		MessageBox(hDlg, TEXT("Hostname and Port must be specified for Black."), TEXT("Error"), MB_OK | MB_ICONWARNING);
		return -1;
	}

	// Convert sHostname to hostname, sPort to port
#ifdef UNICODE
	WideCharToMultiByte(CP_ACP, 0, gaming.playerInfo[(size_t)playerIndex].sHostname, -1, hostname, 256, NULL, NULL);
	port = _wtoi(gaming.playerInfo[(size_t)playerIndex].sPort);
#else
	strcpy_s(hostname, 256, sHostname);
	port = atoi(sPort);
#endif
	// Initialize external thinker
	int ret;
	ret = externalThinkerHandler[(size_t)playerIndex].init(playerIndex);
	ret = externalThinkerHandler[(size_t)playerIndex].setParam(hostname, port, hDlg);

	TCHAR errorMessage[256];
	switch (ret) {
	case -1:
		wsprintf(errorMessage, TEXT("Valid Hostname and Port must be specified for %s."), playerIndex == PLAYERINDEX::PLAYERINDEX_BLACK ? TEXT("Black") : TEXT("White"));
		MessageBox(hDlg, errorMessage, TEXT("Error"), MB_OK | MB_ICONWARNING);
		return -2;
	case -2:
	case -3:
		wsprintf(errorMessage, TEXT("External thinker for %s is not ready."), playerIndex == PLAYERINDEX::PLAYERINDEX_BLACK ? TEXT("Black") : TEXT("White"));
		MessageBox(hDlg, errorMessage, TEXT("Error"), MB_OK | MB_ICONWARNING);
		return -3;
	}

	// Send Information Request to the specified external thinker
	ret = externalThinkerHandler[(size_t)playerIndex].sendInformationRequest();

	if (ret != 0) return -4;

	// Set timer for waiting the response
	UINT_PTR TimerIdWaitInfoResp;
	TimerIdWaitInfoResp = SetTimer(hDlg, (INT_PTR)TIMERID::WAIT_INFO_RESP, WAIT_TIME_INFO_RESP * 1000, NULL);

	// Disable "OK" and "Cancel" button
	HWND button = GetDlgItem(hDlg, IDOK);
	EnableWindow(button, FALSE);
	button = GetDlgItem(hDlg, IDCANCEL);
	EnableWindow(button, FALSE);

	// Wait here if the timer cannot be used
	if (TimerIdWaitInfoResp == 0) {
		// If setting timer failed, wait here but screen blocks
		Sleep(WAIT_TIME_INFO_RESP * 5000);
	}

	return 0;
}

//
//	Function Name: StartGame
//	Summary: Start or trigger the game according to the player's type.
//	
//	In:
//		hDlg			Handle for Dialog box
//
//	Return:
//		None
//
void StartGame(HWND hDlg)
{
	// Initialize variables, board and then start the game
	gaming.InitGame();
	//gaming.StartGame();

	// Initialize History instance
	history.init();

	// Send to WndProc to trigger computer's thinker if black is set to PLAYERTYPE_COMPUTER_EMBEDED
	switch (gaming.getCurrentPlayerType()) {
	case PLAYERTYPE::PLAYERTYPE_USER:
		gaming.setState(GAME_STATES::STATE_GAMING);
		break;
	case PLAYERTYPE::PLAYERTYPE_COMPUTER_EMBEDED:
		gaming.setState(GAME_STATES::STATE_GAMING_WAITING_RESP);
		PostMessage(GetWindow(hDlg, GW_OWNER), WM_USER_TRIGGER_THINKER, 0, 0);
		break;
	case PLAYERTYPE::PLAYERTYPE_COMPUTER_EXTERNAL:
		gaming.setState(GAME_STATES::STATE_GAMING_WAITING_RESP);
		PostMessage(GetWindow(hDlg, GW_OWNER), WM_USER_TRIGGER_EXTERNAL_THINKER, 0, 0);
		break;
	}

	// Decrement num of games
	if (gaming.autoRepeat == true && gaming.bLimitedRepeating == true) {
		gaming.numRepeat--;
		if (gaming.numRepeat <= 0) gaming.autoRepeat = false;
	}
}
