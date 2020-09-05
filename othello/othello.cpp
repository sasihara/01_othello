//
//	Othello(*1).cpp
//  Copyright(C) 2019 T.Sashihara
//	
//  (*1) TM＆© Othello,Co. and MegaHouse
//

#include "framework.h"
#include "othello.hpp"
#include "think.hpp"
#include "externalThinkerHandler.hpp"
#include <stdio.h>

constexpr auto MAX_LOADSTRING = 100;

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Global Variables for this game
Display display;
Board board;
Gaming gaming;
ExternalThinkerHandler externalThinkerHandler[2];	// 0: Black, 1: White

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Dialog1(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_OTHELLO, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OTHELLO));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OTHELLO));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_OTHELLO);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, TEXT("Othello (Please configure game conditions)"), WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
      CW_USEDEFAULT, 0, 800, 800, nullptr, nullptr, hInstance, nullptr);

   // Adjust window size
   RECT rc1, rc2;
   GetWindowRect(hWnd, &rc1);
   GetClientRect(hWnd, &rc2);
   SetWindowPos(hWnd, NULL, 0, 0, 800 + ((rc1.right - rc1.left) - (rc2.right - rc2.left)), 800 + ((rc1.bottom - rc1.top) - (rc2.bottom - rc2.top)), (SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE));

   if (!hWnd)
   {
      return FALSE;
   }

   // Initialize board.
   display.SetParams(hWnd);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int ret;

    switch (message)
    {
    case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_FILE_NEWGAME:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, Dialog1);

			// Redraw the board
			display.UpdateBoard(false);
			break;
		case ID_PASS:
			if (gaming.getGameState() == GAME_STATES::STATE_GAMING && gaming.IsPlayerMustPass() == true) {

				// Update board
				if ((ret = gaming.Pass()) < 0) break;

				// Check gameover and trigger thinker if necessary.
				switchToNextPlayer(hWnd);
			}
			else {
				MessageBox(hWnd, TEXT("You can put at least one place."), TEXT("Warning"), MB_OK | MB_ICONWARNING);
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	}
	case WM_PAINT:
    {
		display.DrawBoard(gaming.getWindowTitle());
		break;
	}
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
	case WM_LBUTTONUP:
	{
		// If left mouse button pushed, check the game state.
		if (gaming.getGameState() == GAME_STATES::STATE_GAMING) {
			// If game is already started, get the clicked position on the board.
			int xPos = (lParam & 0xFFFF) / 100;
			int yPos = ((lParam >> 16) & 0xFFFF) / 100;

			// Update board.
			if ((ret = gaming.PutDisk(xPos, yPos)) < 0) break;

			// Check gameover and trigger thinker if necessary.
			switchToNextPlayer(hWnd);
		}
		else if (gaming.getGameState() == GAME_STATES::STATE_INIT) {
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, Dialog1);

			// Redraw the board
			display.UpdateBoard(false);
		}
		break;
	}
	case WM_USER_TRIGGER_THINKER:
	{
		Thinker thinker;
		DISKCOLORS workBoard[8][8];

		// If player must pass, skip calling thiner and switch to next player
		if (gaming.IsPlayerMustPass() == true) {
			if (gaming.getOpponentPlayerType() == PLAYERTYPE::PLAYERTYPE_USER) {
				MessageBox(hWnd, TEXT("Thinker passes!!"), TEXT("Message from thinker"), MB_OK);
			}
			gaming.Pass();
		}
		else{
			// Get current board from Board object.
			board.CopyBoard(workBoard);

			// Set parameters and then call thinker.
			ret = thinker.SetParams(gaming.getTurn(), workBoard);
			ret = thinker.think();

			// Check if thinker passes or not.
			if (ret >= 0) {
				int xPos = ret / 10;
				int yPos = ret % 10;

				// Update board.
				gaming.PutDisk(xPos, yPos);
			}
			else {
				// If the player is not allowed to pass, initialize the game.
				MessageBox(hWnd, TEXT("Thinker error. Retart the game."), TEXT("Error"), MB_ICONWARNING | MB_OK);
				gaming.InitGame();
				break;
			}
		}

		// Check gameover and trigger thinker if necessary.
		switchToNextPlayer(hWnd);
		break;
	}
	case WM_USER_TRIGGER_EXTERNAL_THINKER:
	{
		// If player must pass, skip calling thiner and switch to next player
		if (gaming.IsPlayerMustPass() == true) {
			if (gaming.getOpponentPlayerType() == PLAYERTYPE::PLAYERTYPE_USER) {
				MessageBox(hWnd, TEXT("Thinker passes!!"), TEXT("Message from thinker"), MB_OK);
			}
			gaming.Pass();

			// Trigger to next player
			switchToNextPlayer(hWnd);
		}
		else{
			DISKCOLORS workBoard[8][8];

			// Get current board from Board object.
			board.CopyBoard(workBoard);

			// Send Think Request to external thinker
			int ret = externalThinkerHandler[TurnToPlayerIndex(gaming.getTurn())].sendThinkRequest(gaming.getTurn(), workBoard, hWnd);

			// Check the result
			if (ret < 0) {
				MessageBox(hWnd, TEXT("Thinker error. Retart the game."), TEXT("Error"), MB_ICONWARNING | MB_OK);
				gaming.InitGame();
				break;
			}
		}
		break;
	}
	case WSOCK_SELECT:
		ret = externalThinkerHandler[TurnToPlayerIndex(gaming.getTurn())].receiveMessages();
		break;
	default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

// ---------------------------- New functions are defined below ------------------------------------
//
//	Function Name: displayGameOver
//	Summary: Display game over dialog box.
//	
//	In:
//		hWnd:	Window handle
//
//	Return:
//		No return values.
//
void displayGameOver(HWND hWnd)
{
	// Display who won this game
	// Count disks for each color
	int numBlack = 0, numWhite = 0;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (board.GetDisk(i, j) == DISKCOLORS::COLOR_BLACK) numBlack++;
			else if (board.GetDisk(i, j) == DISKCOLORS::COLOR_WHITE) numWhite++;
		}
	}

	// Display the winner
	if (numBlack > numWhite) MessageBox(hWnd, TEXT("Black wins!!"), TEXT("Game Finished"), MB_OK);
	else if (numBlack < numWhite) MessageBox(hWnd, TEXT("White wins!!"), TEXT("Game Finished"), MB_OK);
	else if (numBlack == numWhite) MessageBox(hWnd, TEXT("The game was even."), TEXT("Game Finished"), MB_OK);
}

//
//	Function Name: switchToNextPlayer
//	Summary: Switch to next player
//	
//	In:
//		hWnd:	Window handle
//
//	Return:
//		No return values.
//
void switchToNextPlayer(HWND hWnd)
{
	// Game over decision
	if (gaming.IsGameOver() == true) {
		// Display Gameover
		displayGameOver(hWnd);

		// Reset game
		gaming.InitGame();

		// Redraw the board
		display.UpdateBoard(false);
	}
	else {
		//switch (PlayerInfo[turn & 1].PlayerType) {
		switch (gaming.getCurrentPlayerType()) {
		case PLAYERTYPE::PLAYERTYPE_USER:
			gaming.setState(GAME_STATES::STATE_GAMING);
			break;
		case PLAYERTYPE::PLAYERTYPE_COMPUTER_EMBEDED:
			gaming.setState(GAME_STATES::STATE_GAMING_WAITING_RESP);
			PostMessage(hWnd, WM_USER_TRIGGER_THINKER, 0, 0);
			break;
		case PLAYERTYPE::PLAYERTYPE_COMPUTER_EXTERNAL:
			gaming.setState(GAME_STATES::STATE_GAMING_WAITING_RESP);
			PostMessage(hWnd, WM_USER_TRIGGER_EXTERNAL_THINKER, 0, 0);
			break;
		default:
			break;
		}
	}
}

//
//	Function Name: SetParams
//	Summary: Set parameters for Display object
//	
//	In:
//		hWnd:	Window handle
//
//	Return:
//		No return values.
//
int Display::SetParams(HWND _hWnd)
{
	hWnd = _hWnd;
	hMenu = GetMenu(hWnd);
	return 0;
}

//
//	Function Name: UpdateBoard
//	Summary: Trigger drawing the board
//	
//	In:
//		No parameters.
//
//	Return:
//		No return values.
//
int Display::UpdateBoard(bool playerMustPass)
{
	// Draw board
	InvalidateRect(hWnd, NULL, NULL);
	UpdateWindow(hWnd);

	// Set "PASS" menu to gray if player need not to pass.
	menuInfo.cbSize = sizeof(MENUITEMINFO);
	menuInfo.fMask = MIIM_STATE;

	if (playerMustPass == true) menuInfo.fState = MFS_ENABLED;
	else menuInfo.fState = MFS_GRAYED;
		
	SetMenuItemInfo(hMenu, ID_PASS, FALSE, &menuInfo);
	DrawMenuBar(hWnd);

	return 0;
}

//
//	Function Name: DrawBoard
//	Summary: Draw the board and dice
//	
//	In:
//		windowTitle:	Text for the window title.
//
//	Return:
//		0:	Succeed
//
int Display::DrawBoard(LPCTSTR windowTitle)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);
	// TODO: Add any drawing code that uses hdc here...
	// Draw background.
	RECT rc;
	rc.left = rc.top = 0;
	rc.right = rc.bottom = 800;
	HBRUSH hbr = CreateSolidBrush(RGB(0, 128, 0));
	FillRect(hdc, &rc, hbr);
	DeleteObject(hbr);

	// Draw lines.
	int i, j;
	HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
	for (i = 100; i < 800; i += 100) {
		MoveToEx(hdc, 0, i, NULL);
		LineTo(hdc, 800, i);
		MoveToEx(hdc, i, 0, NULL);
		LineTo(hdc, i, 800);
	}
	SelectObject(hdc, hOldPen);
	DeleteObject(hPen);

	// Draw circles on board.
	hOldPen = (HPEN)SelectObject(hdc, GetStockObject(NULL_PEN));
	hbr = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hbr);

	Ellipse(hdc, 195, 195, 205, 205);
	Ellipse(hdc, 595, 595, 605, 605);
	Ellipse(hdc, 195, 595, 205, 605);
	Ellipse(hdc, 595, 195, 605, 205);

	SelectObject(hdc, hOldPen);
	SelectObject(hdc, hOldBrush);
	DeleteObject(hbr);

	// Draw disks
	hOldPen = (HPEN)SelectObject(hdc, GetStockObject(NULL_PEN));

	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			int cx = i * 100 + 50;
			int cy = j * 100 + 50;

			switch (board.GetDisk(i, j)) {
			case DISKCOLORS::COLOR_BLACK:
				hbr = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
				hOldBrush = (HBRUSH)SelectObject(hdc, hbr);
				Ellipse(hdc, cx - 40, cy - 40, cx + 40, cy + 40);
				break;
			case DISKCOLORS::COLOR_WHITE:
				hbr = (HBRUSH)CreateSolidBrush(RGB(255, 255, 255));
				hOldBrush = (HBRUSH)SelectObject(hdc, hbr);
				Ellipse(hdc, cx - 40, cy - 40, cx + 40, cy + 40);
				break;
			default:
				break;
			}
		}
	}

	SelectObject(hdc, hOldPen);
	SelectObject(hdc, hOldBrush);
	DeleteObject(hbr);

	// Finish drawings
	EndPaint(hWnd, &ps);

	// Update Window's title
	SetWindowText(hWnd, windowTitle);

	return 0;
}

//
//	Function Name: Board
//	Summary: Constructor for Board object
//	
//	In:
//		No parameters.
//
//	Return:
//		No return values.
//
Board::Board()
{
	InitBoard();
}

//
//	Function Name: PutDisk 
//	Summary: Put a disk on the board
//	
//	In:
//		x, y :	Position for the disk
//		color:	The disk color
//
//	Return:
//		0: Succeed
//
int Board::PutDisk(int x, int y, DISKCOLORS color)
{
	board[x][y] = color;
	return 0;
}

//
//	Function Name: GetDisk
//	Summary: Get disk color on the board
//	
//	In:
//		x, y : Position for the disk to get the color.
//
//	Return:
//		Disk color.
//
DISKCOLORS Board::GetDisk(int x, int y) {
	return board[x][y];
}

//
//	Function Name: InitBoard
//	Summary: Initialize the board.
//	
//	In:
//		No parameters.
//
//	Return:
//		0: Succeed
//
int Board::InitBoard()
{
	memset(board, (int)DISKCOLORS::COLOR_NONE, BOARDSIZE_IN_BYTE);
	board[3][3] = board[4][4] = DISKCOLORS::COLOR_WHITE;
	board[3][4] = board[4][3] = DISKCOLORS::COLOR_BLACK;
	return 0;
}

//
//	Function Name: CopyBoard
//	Summary: Copy the board to the specified memory area.
//	
//	In:
//		board:	The address to copy the board data
//
//	Return:
//		0: Succeed
//
int Board::CopyBoard(DISKCOLORS _board[8][8])
{
	memcpy(_board, board, BOARDSIZE_IN_BYTE);
	return 0;
}

//
//	Function Name: Gaming
//	Summary: Constructor for Gaming object
//	
//	In:
//		No parameters.
//
//	Return:
//		No return value.
//
Gaming::Gaming()
{
	InitGame();
}

//
//	Function Name: check
//	Summary: Check if the specified color's disk can put the specified place or not.
//	
//	In:
//		board:		The pointer to the board array.
//		xPos, yPos:	The position to put the disk.
//		color:		The disk color to put.
//
//	Return:
//		The flag which direction's disks will turn by putting the specified disk at the specified place.
//			bit 1:	Upper.
//			bit 2:	Upper right.
//			bit 3:	Right.
//			bit 4:	Lower right.
//			bit 5:	Lower.
//			bit 6:	Lower left.
//			bit 7:	Left.
//			bit 8:	Upper Left.

int Gaming::check(int xPos, int yPos, DISKCOLORS color)
{
	int ret = 0;

	if (checkOneDir(xPos, yPos, color, 0, -1) > 0) ret = ret | 1;
	if (checkOneDir(xPos, yPos, color, 1, -1) > 0) ret = ret | 2;
	if (checkOneDir(xPos, yPos, color, 1, 0) > 0) ret = ret | 4;
	if (checkOneDir(xPos, yPos, color, 1, 1) > 0) ret = ret | 8;
	if (checkOneDir(xPos, yPos, color, 0, 1) > 0) ret = ret | 16;
	if (checkOneDir(xPos, yPos, color, -1, 1) > 0) ret = ret | 32;
	if (checkOneDir(xPos, yPos, color, -1, 0) > 0) ret = ret | 64;
	if (checkOneDir(xPos, yPos, color, -1, -1) > 0) ret = ret | 128;

	return ret;
}

//
//	Function Name: checkOneDir
//	Summary: Return the number of disks will be turned in the specified direction if the specified disk put to the specified place.
//	
//	In:
//		board:			The pointer to the board array.
//		xPos, yPos:		The position to put the disk.
//		color:			The disk color to put.
//		xStep, yStep:	The direction to be checked.	
//
//	Return:	The number of disks to be turned.
//
int Gaming::checkOneDir(int xPos, int yPos, DISKCOLORS color, int xStep, int yStep)
{
	// Check parameters
	if (xPos < 0 || xPos > 7 || yPos < 0 || yPos > 7) return 0;
	if (color != DISKCOLORS::COLOR_BLACK && color != DISKCOLORS::COLOR_WHITE) return 0;
	if (xStep < -1 || xStep > 1 || yStep < -1 || yStep > 1) return 0;

	// Check if the position is empty or not
	if (board.GetDisk(xPos, yPos) != DISKCOLORS::COLOR_NONE) return 0;

	// Check if the next place is on the board or not
	if (xPos + xStep < 0 || xPos + xStep > 7 || yPos + yStep < 0 || yPos + yStep > 7) return 0;

	// Count the number of disks to be turned
	int x, y, NumTurned = 0;
	for (x = xPos + xStep, y = yPos + yStep; 0 <= x && x <= 7 && 0 <= y && y <= 7; x += xStep, y += yStep) {
		if (board.GetDisk(x, y) == color) return NumTurned;				// meeting player's disk
		else if (board.GetDisk(x, y) == DISKCOLORS::COLOR_NONE) return 0;				// reached to empty without meeting player's disk
		else if (board.GetDisk(x, y) == OPPONENT(color)) NumTurned++;		// meeting opponent's disk
		else return 0;												// Illegal case
	}

	// In this case, reached to the edge without meeting player's disk
	return 0;
}

//
//	Function Name: turnDisk
//	Summary: Turn disks by putting a disk at the specified place of the specified color.
//	
//	In:
//		board:			The pointer to the board array.
//		xPos, yPos:		The position to put the disk.
//		color:			The disk color to put.
//		flag:			The result of check() function.	
//
//	Return:	The number of disks to be turned.
//
int Gaming::turnDisk(int xPos, int yPos, int flag)
{
	if (flag & 1) turnDiskOneDir(xPos, yPos, 0, -1);
	if (flag & 2) turnDiskOneDir(xPos, yPos, 1, -1);
	if (flag & 4) turnDiskOneDir(xPos, yPos, 1, 0);
	if (flag & 8) turnDiskOneDir(xPos, yPos, 1, 1);
	if (flag & 16) turnDiskOneDir(xPos, yPos, 0, 1);
	if (flag & 32) turnDiskOneDir(xPos, yPos, -1, 1);
	if (flag & 64) turnDiskOneDir(xPos, yPos, -1, 0);
	if (flag & 128) turnDiskOneDir(xPos, yPos, -1, -1);

	return 0;
}

//
//	Function Name: turnDiskOneDir
//	Summary: Turn disks by putting a disk at the specified place of the specified color in the specified direction.
//	
//	In:
//		board:			The pointer to the board array.
//		xPos, yPos:		The position to put the disk.
//		color:			The disk color to put.
//		xStep, yStep:	The direction to be turned.
//
//	Return:	The number of disks to be turned.
//
int Gaming::turnDiskOneDir(int xPos, int yPos, int xStep, int yStep)
{
	// Get current player's color
	DISKCOLORS color = getCurrentColor();

	// Place player's disk on the board
	board.PutDisk(xPos, yPos, color);

	// Turn disks
	int x, y;
	for (x = xPos + xStep, y = yPos + yStep; 0 <= x && x <= 7 && 0 <= y && y <= 7; x += xStep, y += yStep) {
		if (board.GetDisk(x, y) == color) break;
		board.PutDisk(x, y, color);
	}

	return 0;
}

//
//	Function Name: PutDisk
//	Summary: Put a disk and then turn dice if exist
//	
//	In:
//		x, y: Position to put a disk for the current player
//
//	Return:
//		0: Succeed
//		-1:	Specified position is out of the board.
//		-2:	Player cannot place the disk at the specified place.
//
int Gaming::PutDisk(int x, int y)
{
	int dirFlag = 0;

	// Check if specified position is on the board or not
	if (x < 0 || x > 7 || y < 0 || y > 7) return -1;

	// Check if specified position can be put for the disk
	if ((dirFlag = check(x, y, getCurrentColor())) == 0) {
		return -2;
	}

	// Turn disks 
	turnDisk(x, y, dirFlag);

	// Switch to another player
	turn++;

	// Redraw the board
	display.UpdateBoard(IsPlayerMustPass());

	return 0;
}

//
//	Function Name: Pass
//	Summary: Pass for the current turn.
//	
//	In:
//		No parameters.
//
//	Return:
//		0: Succeed
//		-1:	The player cannot pass for the current turn.
//
int Gaming::Pass()
{
	if (IsPlayerMustPass() == true) {
		// Switch to another player
		turn++;
		return 0;
	}
	else {
		return -1;
	}
}

//
//	Function Name: InitGame
//	Summary: Initialize the game and refresh the board.
//	
//	In:
//		No parameters.
//
//	Return:
//		0: Succeed.
//
int Gaming::InitGame()
{
	// Initialize game parameters
	state = GAME_STATES::STATE_INIT;
	turn = 0;

	// Initialize board
	board.InitBoard();

	// Redraw the board
	display.UpdateBoard(false);

	return 0;
}

////
////	Function Name: StartGame
////	Summary: Start the game.
////	
////	In:
////		No parameters.
////
////	Return:
////		0: Succeed.
////
//int Gaming::StartGame()
//{
//	// Change game state
//	state = GAME_STATES::STATE_GAMING;
//	return 0;
//}

//
//	Function Name: SetState
//	Summary: Change the state.
//	
//	In:
//		newState : The new state.
//
//	Return:
//		0: Succeed.
//
int Gaming::setState(GAME_STATES newState)
{
	// Change game state
	state = newState;
	return 0;
}

//
//	Function Name: IsPlayerShouldPass
//	Summary: Check if the player should pass or not.
//	
//	In:
//		_turn:			The number of turn.
//
//	Return:
//		false:	The player should not pass.
//		true:	The player should pass.
//
bool Gaming::IsPlayerMustPass()
{
	int x, y;
	DISKCOLORS color;

	color = getCurrentColor();

	// Check if current player can put a disk or not
	for (x = 0; x < 8; x++) {
		for (y = 0; y < 8; y++) {
			if (check(x, y, color) > 0) return false;
		}
	}

	return true;
}

//
//	Function Name: IsNextPlayerMustPass
//	Summary: Check if the next player must pass or not.
//	
//	In:
//		No parameters.
//
//	Return:
//		true: Next player must pass.
//		false: Next player must not pass.
//
bool Gaming::IsNextPlayerMustPass()
{
	int x, y;
	DISKCOLORS color;

	color = OPPONENT(getCurrentColor());

	// Check if current player can put a disk or not
	for (x = 0; x < 8; x++) {
		for (y = 0; y < 8; y++) {
			if (check(x, y, color) > 0) return false;
		}
	}

	return true;
}

//
//	Function Name: IsGameOver
//	Summary: Check if the game is finished or not.
//	
//	In:
//		_turn:			The number of turn.
//
//	Return:
//		false:	The game is not finished.
//		true:	The game is finished.
//
bool Gaming::IsGameOver()
{
	if (IsPlayerMustPass() == true && IsNextPlayerMustPass() == true) return true;
	else return false;
}

//
//	Function Name: getGameState
//	Summary: Get current game state.
//	
//	In:
//		No parameters.
//
//	Return:
//		Game state.
//
GAME_STATES Gaming::getGameState()
{
	return state;
}

//
//	Function Name: getPlayerType
//	Summary: Get the player type for the specified player in index.
//	
//	In:
//		player: Player to get player type in index (0: Black, 1: White)
//
//	Return:
//		Player type.
//
PLAYERTYPE Gaming::getPlayerType(PLAYERINDEX player)
{
	if (player < PLAYERINDEX::PLAYERINDEX_LIMIT)
		return playerInfo[(int)player].PlayerType;
	else
		return PLAYERTYPE::PLAYERTYPE_ERROR;
}

//
//	Function Name: getCurrentPlayerType
//	Summary: Get current player type.
//	
//	In:
//		No parameters.
//
//	Return:
//		Player type.
//
PLAYERTYPE Gaming::getCurrentPlayerType()
{
	switch (getCurrentColor()) {
	case DISKCOLORS::COLOR_BLACK:
		return playerInfo[(int)PLAYERINDEX::PLAYERINDEX_BLACK].PlayerType;
		break;
	case DISKCOLORS::COLOR_WHITE:
		return playerInfo[(int)PLAYERINDEX::PLAYERINDEX_WHITE].PlayerType;
		break;
	}
	return PLAYERTYPE::PLAYERTYPE_ERROR;
}

//
//	Function Name: getOpponentPlayerType
//	Summary: Get opponent player type.
//	
//	In:
//		No parameters.
//
//	Return:
//		Player type.
//
PLAYERTYPE Gaming::getOpponentPlayerType()
{
	switch (getCurrentColor()) {
	case DISKCOLORS::COLOR_BLACK:
		return playerInfo[(int)PLAYERINDEX::PLAYERINDEX_WHITE].PlayerType;
		break;
	case DISKCOLORS::COLOR_WHITE:
		return playerInfo[(int)PLAYERINDEX::PLAYERINDEX_BLACK].PlayerType;
		break;
	}
	return PLAYERTYPE::PLAYERTYPE_ERROR;
}

//
//	Function Name: setPlayerType
//	Summary: Set player type for the specified player in index.
//	
//	In:
//		player:	Player in index.
//		playerType: Player type to set.
//
//	Return:
//		0: Suceess
//		-1: Fail.
//
int Gaming::setPlayerType(PLAYERINDEX player, PLAYERTYPE playerType)
{
	if (player < PLAYERINDEX::PLAYERINDEX_LIMIT){
		playerInfo[(int)player].PlayerType = playerType;
		return  0;
	}
	else
		return -1;
}

//
//	Function Name: getWindowTitle
//	Summary: Get window title text for the current.
//	
//	In:
//		No parameters.
//
//	Return:
//		Text for the window title.
//
LPCTSTR Gaming::getWindowTitle()
{
	if (state == GAME_STATES::STATE_GAMING || state == GAME_STATES::STATE_GAMING_WAITING_RESP) {
		switch (getCurrentColor()) {
		case DISKCOLORS::COLOR_BLACK:
			if (getCurrentPlayerType() == PLAYERTYPE::PLAYERTYPE_COMPUTER_EMBEDED || getCurrentPlayerType() == PLAYERTYPE::PLAYERTYPE_COMPUTER_EXTERNAL) {
				return TEXT("Othello (Black's turn, Computer is now thinking...)");
			}
			else {
				return TEXT("Othello (Black's turn)");
			}
			return TEXT("Othello (Black's turn)");
			break;
		case DISKCOLORS::COLOR_WHITE:
			if (getCurrentPlayerType() == PLAYERTYPE::PLAYERTYPE_COMPUTER_EMBEDED || getCurrentPlayerType() == PLAYERTYPE::PLAYERTYPE_COMPUTER_EXTERNAL) {
				return TEXT("Othello (White's turn, Computer is now thinking...)");
			}
			else {
				return TEXT("Othello (White's turn)");
			}
			break;
		default:
			return TEXT("Othello");
			break;
		}
	}
	else {
		return TEXT("Othello (Please start to setup by clicking left mouse button or from File menu.)");
	}
}

//
//	Function Name: getTurn
//	Summary: Get the turn value.
//	
//	In:
//		No parameters.
//
//	Return:
//		The turn value.
//
int Gaming::getTurn()
{
	return turn;
}