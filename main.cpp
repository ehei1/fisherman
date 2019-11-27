#include "main.h"

static HINSTANCE GlobalInstance ;


int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
    WNDCLASS wc;
    HWND hwnd;
    MSG msg;

	/*
	// check having a mouse.
	if ( ! GetSystemMetrics ( SM_MOUSEPRESENT ) )
		call ERROR ;
	*/

    wc.style = CS_HREDRAW | CS_VREDRAW;                                   // Class style
    wc.lpfnWndProc = (WNDPROC) WndProc;             // Window procedure address
    wc.cbClsExtra = 0;                              // Class extra bytes
    wc.cbWndExtra = 0;                              // Window extra bytes
    wc.hInstance = hInstance;                       // Instance handle
	wc.hIcon = LoadIcon ( hInstance, MAKEINTRESOURCE( IDI_ICON ) );        // Icon handle
    wc.hCursor = LoadCursor (NULL, IDC_ARROW);      // Cursor handle
    wc.hbrBackground = (HBRUSH) ( COLOR_BACKGROUND ); // Background color
    wc.lpszMenuName = MAKEINTRESOURCE( IDR_MENU );                         // Menu name
    wc.lpszClassName = szClassName;                // WNDCLASS name

    RegisterClass (&wc);
	
	// insert function that read setting from registry in Windows
	// ex) setConfig( &uConfig );

	uConfig = &SET_BASIC;

    hwnd = CreateWindow (
        szClassName,               // WNDCLASS name
        szAppName,          // Window title
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,        // Window style
        CW_USEDEFAULT,              // Horizontal position
        CW_USEDEFAULT,              // Vertical position        
        0 ,              // Initial width
        0 ,              // Initial height
        HWND_DESKTOP,               // Handle of parent window
        NULL,                       // Menu handle
        hInstance,                  // Application's instance handle
        NULL                        // Window-creation data
    ); 

	// int sys_width = 0 , sys_height = 0 ;

	// note : call on the WinMain only. Test this function execution in the place that have HWND info.
	// sys_height += GetSystemMetrics ( SM_CYEDGE ) ; // height of 3D edge
	// sys_width += GetSystemMetrics ( SM_CYEDGE ) ; // width of 3D edge
	sys_width += GetSystemMetrics ( SM_CXFIXEDFRAME ) ; // width of unresizeable window frame
	sys_height += GetSystemMetrics ( SM_CYFIXEDFRAME ) ; // thickness of unresizeable window frame
	sys_width += GetSystemMetrics ( SM_CXBORDER ) ; // width of border
	sys_height += GetSystemMetrics ( SM_CYBORDER ) ; // height of border
	sys_height += GetSystemMetrics ( SM_CYCAPTION ) ; // height of caption bar
	sys_height += GetSystemMetrics ( SM_CYMENU ) ; // height of menu bar

	// uConfig->sys_height = sys_height ;
	// uConfig->sys_width = sys_width ;
    ShowWindow (hwnd, nCmdShow);
    UpdateWindow (hwnd);

	SetGame ( hwnd ) ;

    while (GetMessage (&msg, NULL, 0, 0)) {
        TranslateMessage (&msg);
        DispatchMessage (&msg);
    }

    return msg.wParam;
}


LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HINSTANCE hInstance ;
	static HMENU hMenu ;
	static int iSelection = uConfig->menu ;
	
	static userButton btnSmile ;
	static userDigitBitmap ubDigit ;
	static userField uField ( uConfig->width , uConfig->height , uConfig->mine , BEGIN_X_OF_FIELD , BEGIN_Y_OF_FIELD ) ;
	static size_t time , mine ;
	
	HDC hdcMem ;
	PAINTSTRUCT ps ;
	RECT rect ;
	POINT point ;
	static RECT smileButton = { 69 , 9 , 69 + 37 , 15 + 37 } ; // reload as registry value
	static RECT minePanel = { 17 , 16 , 17 + 39 , 16 + 23 } ;
	static RECT timePanel = { 108 , 17 , 155, 40 } ; // reload as registry value

    switch ( message )
	{
	case WM_CREATE:
		GlobalInstance = hInstance = ((LPCREATESTRUCT) lParam)->hInstance;		
		SetTimer ( hwnd , ID_TIMER , 1000 , NULL ) ;
		
		btnSmile.Set ( hwnd , hInstance , IDB_SMILE , IDB_SMILE_DOWN , IDB_SMILE_CLICK , IDB_SMILE_DEAD , IDB_SMILE_PASS ) ; // set smile button
		ubDigit.set ( hInstance , IDB_DIGIT0 , IDB_DIGIT9 , IDB_MINUS ) ;	// set digit bitmaps
		uField.Set ( hInstance , IDB_TILE , IDB_TILE_8 ) ; // set game fieldIDB_SMILE_DOWN

		SendMessage (hwnd, WM_USER_RESET , 0, 0) ;
        return 0 ;

	case WM_COMMAND:
		// processing menu command
		hMenu = GetMenu (hwnd) ;
		
		switch (LOWORD (wParam))  {
			case ID_NEW_GAME:
				SendMessage (hwnd, WM_USER_RESET, 0, 0) ;
				return 0;

			case ID_MARK:
				{
				uField.SetReserved ( ! uField.IsReserved () ) ;

				if ( ! uField.IsReserved () )
					CheckMenuItem ( hMenu, ID_MARK , MF_CHECKED) ;
				else
					CheckMenuItem ( hMenu, ID_MARK , MF_UNCHECKED) ;
				return 0;
				}
			case ID_COLOR:
				return 0;

			case ID_SOUND:
				return 0;

			case ID_HIGH_SCORE:
				return 0;

			case ID_QUIT:
				SendMessage (hwnd, WM_CLOSE, 0, 0) ;
				return 0 ;		

			case ID_CONTENT:
				return 0;
			case ID_SEARCH:
				return 0;
			case ID_USE:
				return 0;

			case ID_ABOUT:
				DialogBox ( hInstance, MAKEINTRESOURCE (IDD_ABOUT), hwnd, AboutProc);
				return 0;

			case ID_BASIC:
				uConfig = &SET_BASIC;
				// uField.Reset ( uConfig->width , uConfig->height , uConfig->mine ) ;

				SendMessage (hwnd, WM_USER_RESET, 0, 0) ;
				return 0;

			case ID_INTERMEDIATE:
				uConfig = &SET_INTERMEDIATE;
				// uField.Reset ( uConfig->width , uConfig->height , uConfig->mine ) ;

				SendMessage (hwnd, WM_USER_RESET, 0, 0) ;
				return 0;

			case ID_EXPERT:
				uConfig = &SET_EXPERT;
				// uField.Reset ( uConfig->width , uConfig->height , uConfig->mine ) ;

				SendMessage (hwnd, WM_USER_RESET, 0, 0) ;
				return 0;

			case ID_CUSTOM:
				// uConfig = &SET_CUSTOM;
				DialogBox ( hInstance, MAKEINTRESOURCE (IDD_CUSTOM), hwnd, CustomProc);
				// uField.Reset ( uConfig->width , uConfig->height , uConfig->mine ) ;

				SendMessage (hwnd, WM_USER_RESET, 0, 0) ;
				return 0;
		}

		return 0 ;

	case WM_USER_RESET :
		{
		for( int i = ID_BASIC ; i <= ID_CUSTOM ; i++ )
			CheckMenuItem (hMenu, i, MF_UNCHECKED) ;

		CheckMenuItem (hMenu, uConfig->menu, MF_CHECKED) ;
		
		if ( ! uField.IsReserved () )
			CheckMenuItem ( hMenu, ID_MARK , MF_CHECKED) ;
		else
			CheckMenuItem ( hMenu, ID_MARK , MF_UNCHECKED) ;

		SetGame( hwnd );

		// redefine smile button coordinate
		GetClientRect ( hwnd , &rect ) ;
		point.y = 9 ;
		point.x = smileButton.left =  ( rect.right - btnSmile.GetSizeX () ) / 2 ;
		smileButton.right = smileButton.left + btnSmile.GetSizeX () ;

		btnSmile.SetPoint ( point ) ;
		btnSmile.SetStatus ( button_up ) ;

		timePanel.left = rect.right - 53 ;
		timePanel.right = rect.right - 10 ;
		time = 0 ;
		uField.Reset ( uConfig->width , uConfig->height , uConfig->mine ) ;
		mine = uConfig->mine ;
		// uField.Refresh () ;

		PlaySound( MAKEINTRESOURCE ( IDW_START ) , hInstance , SND_RESOURCE | SND_ASYNC); 
		InvalidateRect (hwnd, NULL, FALSE ) ;
		}
		return 0 ;

	case WM_MOUSEMOVE :
		if ( uField.IsOver () )
			return 0 ;

		point.x = LOWORD (lParam) ;
        point.y = HIWORD (lParam) ;

		if ( wParam & MK_LBUTTON )
		{
			uField.OnDownLeft ( point ) ;
			UpdateDisplay ( uField.GetModifiedIndex () , hwnd ) ;
			// InvalidateRect ( hwnd, & rect , FALSE ) ;  // find update area to more speed
		}

		return 0 ;

	case WM_NCLBUTTONUP :
	case WM_LBUTTONUP :
		point.x = LOWORD (lParam) ;
        point.y = HIWORD (lParam) ;

		if ( button_down == btnSmile.GetStatus () )
		{
			if ( uField.IsPlay () )
			{
				SendMessage (hwnd, WM_USER_RESET, 0, 0) ;
				return 0 ;
			}

			btnSmile.SetStatus ( button_up ) ;
			InvalidateRect ( hwnd, &smileButton , FALSE ) ;
			return 0 ;
		}

		if ( uField.OnUpLeft ( point ) )
		{
			if ( uField.IsPass () )
			{
				btnSmile.SetStatus ( game_pass ) ;
				mine = 0 ;
				PlaySound( MAKEINTRESOURCE ( IDW_CLEAR ) , hInstance , SND_RESOURCE | SND_ASYNC ) ;
				InvalidateRect ( hwnd, NULL , FALSE ) ;
				return 0 ;
			}

			if ( uField.IsOver () )
			{
				btnSmile.SetStatus ( game_over ) ;
				PlaySound( MAKEINTRESOURCE ( IDW_DEATH ) , hInstance , SND_RESOURCE | SND_ASYNC ) ;
				InvalidateRect ( hwnd, NULL , FALSE ) ;
				return 0 ;
			}

			PlaySound( MAKEINTRESOURCE ( IDW_CLICK ) , hInstance , SND_RESOURCE | SND_ASYNC ) ;
			// UpdateDisplay ( uField.GetModifiedIndex () , hwnd ) ;
			InvalidateRect ( hwnd, NULL , FALSE ) ;
		}

		if ( ! uField.IsOver () )
		{
			btnSmile.SetStatus ( button_up ) ;
			InvalidateRect ( hwnd, &smileButton , FALSE ) ;
		}

		return 0 ;

	case WM_LBUTTONDOWN:
		point.x = LOWORD (lParam) ;
        point.y = HIWORD (lParam) ;

		if ( button_down == btnSmile.CheckStatus ( point ) )
		{
			InvalidateRect ( hwnd, &smileButton , FALSE ) ;
			return 0 ;
		}

		if ( uField.IsOver () )
			return 0 ;

		// if ( uField.OnDownLeft ( point ) )
		//	UpdateDisplay ( uField.GetModifiedIndex () , hwnd ) ;
		uField.OnDownLeft ( point ) ;
		UpdateDisplay ( uField.GetModifiedIndex () , hwnd ) ;
		/* else
		{
			RECT rect ;
			rect.left = point.x - TILE_SIZE ;
			rect.top = point.y - TILE_SIZE ;
			rect.right = point.x + TILE_SIZE ;
			rect.bottom = point.x + TILE_SIZE ;
			InvalidateRect ( hwnd, & rect , FALSE ) ;
		} */

		btnSmile.SetStatus ( field_check ) ;
		InvalidateRect ( hwnd, &smileButton , FALSE ) ;
		return 0 ;

	case WM_RBUTTONDOWN:
		point.x = LOWORD (lParam) ;
        point.y = HIWORD (lParam) ;

		if ( uField.OnUpRight ( point ) )
		{
			mine = uField.GetSizeFoundMine () ;
			PlaySound( MAKEINTRESOURCE ( IDW_FLAG ) , hInstance , SND_RESOURCE | SND_ASYNC ) ;
			InvalidateRect ( hwnd, & minePanel , FALSE ) ;
			UpdateDisplay ( uField.GetModifiedIndex () , hwnd ) ;
		}

		return 0 ;

	case WM_TIMER :
		if ( uField.IsOver () || ! uField.IsPlay () )
			return 0 ;

		if ( TIME_MAX == time )
			return 0 ;

		++ time ;
		InvalidateRect (hwnd, & timePanel, FALSE ) ; // only update on time display
		// InvalidateRect ( hwnd, NULL, FALSE ) ;
		return 0 ;

    case WM_PAINT:
		// unsigned int iRandomNumber ;
		BeginPaint ( hwnd , &ps ) ; // hdc = BeginPaint( hwnd, &ps ) ;
		hdcMem = CreateCompatibleDC ( ps.hdc ) ;
		GetClientRect ( hwnd , &rect ) ;
		
		DrawBackground ( ps.hdc , &rect ) ;
		btnSmile.Put ( ps , hdcMem ) ;
		// srand ( ( int ) GetCurrentTime () ) ;
		// iRandomNumber = rand () ;
		// ubDigit.put ( ( iRandomNumber = rand() ) , ps , hdcMem , 17 , 16 , SRCCOPY , 3 ) ;
		// ubDigit.put ( ( iRandomNumber = 4014 ) , ps , hdcMem , 17 , 16 , SRCCOPY , 0 ) ;

		ubDigit.put ( mine , ps , hdcMem , 17 , 16 , SRCCOPY , 3 ) ;
		ubDigit.put ( time , ps , hdcMem , rect.right - ubDigit.getX ( 0 ) * 3 - 15 , 16 , SRCCOPY , 3 ) ;
		uField.Paint ( ps , hdcMem ) ;

		// ubTile.put ( IDB_TILE_RESERVED , ps , hdcMem , 10 , 10 ) ;
		// iLength = wsprintf (szBuffer, TEXT ("The sum of %i and %i is %i"), iA, iB, iA + iB) ;
		// TextOut (hdc, x, y, szBuffer, iLength) ;
		// TextOut ( ps.hdc , rect.left + 5  , rect.bottom - 15 , _itoa ( curMouseX , szBuffer , 30 ) , lstrlen ( _itoa ( curMouseX , szBuffer , 30 ) ) ) ;
		// TextOut ( ps.hdc , rect.left + 20  , rect.bottom - 15 , _itoa ( curMouseY , szBuffer , 15 ) , lstrlen ( _itoa ( curMouseY , szBuffer , 15 ) ) ) ;

		DeleteDC ( hdcMem );
        EndPaint ( hwnd, &ps );
        return 0;

    case WM_DESTROY:
		KillTimer ( hwnd, ID_TIMER ) ;
        PostQuitMessage (0);
        return 0;

	/*
	case WM_SYSCOMMAND:
		// not allow to maximized window from double clicking title bar
		if( SC_MAXIMIZE == LOWORD( wParam ) )
			return 0;
	*/
    }

    return DefWindowProc (hwnd, message, wParam, lParam);
}


BOOL CALLBACK CustomProc (HWND hDlg, UINT message, 
                            WPARAM wParam, LPARAM lParam)
{
	char szHeight[10], szWidth[10], szMine[10];
	USERCONFIG *prevConfig ;
	// uConfig = &SET_CUSTOM ;

     switch (message)
     {
     case WM_INITDIALOG :
		 // backup previous user config
		 prevConfig  = uConfig ;
		 uConfig = &SET_CUSTOM ;

		 // replace to define or resource id
		 SetDlgItemText( hDlg, IDC_HEIGHT,_itoa ( uConfig->height , szHeight , 10 ) );
		 SetDlgItemText( hDlg, IDC_WIDTH,_itoa ( uConfig->width , szWidth , 10 ) );
		 SetDlgItemText( hDlg, IDC_MINE, _itoa ( uConfig->mine , szMine , 10 ) );

		 uConfig = prevConfig ;
         return TRUE ;
          
     case WM_COMMAND :
          switch (LOWORD (wParam))
          {
          case IDOK :
			uConfig = &SET_CUSTOM ;

			GetDlgItemText(hDlg, IDC_HEIGHT, szHeight, 10) ; 
			GetDlgItemText(hDlg, IDC_WIDTH, szWidth, 10) ;
			GetDlgItemText(hDlg, IDC_MINE, szMine , 10) ;

			if ( szHeight && szWidth && szMine )
			{
				UINT iHeight, iWidth, iMine;

				iHeight = atoi( szHeight );
				iWidth = atoi( szWidth );
				iMine = atoi( szMine );

				// replace to resource id
				if ( iHeight > 24 )
					iHeight = 24 ;

				if ( iHeight < 1 )
					iHeight = 9 ;

				if ( iWidth > 30 )
					iWidth = 30 ;

				if ( iWidth < 1 )
					iWidth = 9 ;

				if ( iMine > iWidth * iHeight + 1 )
					iMine = iWidth * iHeight + 1 ;

				if ( iMine < ( UINT ) ( iWidth * iHeight * 0.1) )
					iMine = ( UINT ) ( iWidth * iHeight * 0.1) ;

				uConfig->height = iHeight ;
				uConfig->width = iWidth ;
				uConfig->mine = iMine ;
			}

			EndDialog (hDlg, 0);
			// SendMessage (mainHwnd, WM_CLOSE, 0, 0) ;
			return TRUE;

          case IDCANCEL :
			// uConfig = prevConfig ;
			EndDialog (hDlg, 0) ;
			return TRUE ;
          }
     }

     return FALSE ;
}


BOOL CALLBACK AboutProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static userBitmap logo ;
	HDC hdc ;
	PAINTSTRUCT ps ;
	static HINSTANCE aboutInstance ;

     switch (message)
     {
     case WM_INITDIALOG :
		 // aboutInstance = ((LPCREATESTRUCT) lParam)->hInstance ;
		 aboutInstance = GlobalInstance ;

		 logo.set ( aboutInstance , MAKEINTRESOURCE ( IDB_LOGO ) ) ;
         return TRUE ;
          
     case WM_COMMAND :
          switch (LOWORD (wParam))
          {
          case IDOK :
			EndDialog (hDlg, 0);
			return TRUE;

          case IDCANCEL :
			// uConfig = prevConfig ;
			EndDialog (hDlg, 0) ;
			return TRUE ;
          }

		  return TRUE ;

	 case WM_PAINT :
		 {
			BeginPaint ( hDlg , &ps ) ; // hdc = BeginPaint( hwnd, &ps ) ;
			hdc = CreateCompatibleDC ( ps.hdc ) ;
			logo.put ( ps , hdc , 0 , 0 ) ;
			
			DeleteDC ( hdc );
			EndPaint ( hDlg , &ps );
			return TRUE;
		 }
	 case WM_DESTROY:
		 EndDialog (hDlg, 0);
		 return TRUE ;
     }

     return FALSE ;
}


BOOL SetGame( const HWND hwnd )
{
	SetWindowPos( hwnd, NULL, 0, 0, uConfig->width * TILE_SIZE + 20 + sys_width , uConfig->height * TILE_SIZE + 63 + sys_height , SWP_NOZORDER | SWP_NOMOVE ) ;

	return TRUE;
}


BOOL DrawBackground ( const HDC hdc , const LPRECT lpRect )
{
	FillRect ( hdc , lpRect , CreateSolidBrush ( RGB_BACKGROUND ) ) ;

	SelectObject ( hdc , CreatePen ( PS_SOLID , 1 , RGB_HIGHLIGHT ) ) ;

	MoveToEx ( hdc , 0 , 0 , NULL ) ;		// begin drawing white border
	LineTo ( hdc , lpRect->right , 0 ) ;
	MoveToEx ( hdc , 0 , 1 , NULL ) ;
	LineTo ( hdc , lpRect->right , 1 ) ;
	MoveToEx ( hdc , 0 , 0 , NULL ) ;
	LineTo ( hdc , 0 , lpRect->bottom ) ;
	MoveToEx ( hdc , 1 , 1 , NULL ) ;
	LineTo ( hdc , 1 , lpRect->bottom ) ;	// end drawing white border
	MoveToEx ( hdc , 11 , 44 , NULL ) ;		// begin drawing white line in the top box
	LineTo ( hdc , lpRect->right - 7, 44 ) ;
	MoveToEx ( hdc , 10 , 45 , NULL ) ;
	LineTo ( hdc , lpRect->right - 6 , 45 ) ;
	MoveToEx ( hdc , lpRect->right - 7 , 11 , NULL ) ;
	LineTo ( hdc , lpRect->right - 7 , 45 ) ;
	MoveToEx ( hdc , lpRect->right - 6 , 10 , NULL ) ;
	LineTo ( hdc , lpRect->right - 6 , 46 ) ;	// end drawing white line in the top box
	MoveToEx ( hdc , lpRect->right - 8 , 55 , NULL ) ;	// begin drawing white line in the bottom box
	LineTo ( hdc , lpRect->right - 8 , lpRect->bottom - 8  ) ;
	MoveToEx ( hdc , lpRect->right - 7 , 54 , NULL ) ;
	LineTo ( hdc , lpRect->right - 7 , lpRect->bottom - 7 ) ;
	MoveToEx ( hdc , lpRect->right - 6 , 53 , NULL ) ;
	LineTo ( hdc , lpRect->right - 6 , lpRect->bottom - 6 ) ;
	MoveToEx ( hdc , 12 , lpRect->bottom - 8 , NULL ) ;
	LineTo ( hdc , lpRect->right - 7 , lpRect->bottom - 8 ) ;
	MoveToEx ( hdc , 11 , lpRect->bottom - 7 , NULL ) ;
	LineTo ( hdc , lpRect->right - 6 , lpRect->bottom - 7 ) ;
	MoveToEx ( hdc , 10 , lpRect->bottom - 6 , NULL ) ;
	LineTo ( hdc , lpRect->right - 5 , lpRect->bottom - 6 ) ;	// end drawing white line in the bottom box

	SelectObject ( hdc , CreatePen ( PS_SOLID , 1 , RGB_SHADOW ) ) ;		
	MoveToEx ( hdc , 9 , 9 , NULL ) ;	// begin drawing darkgray line in the top box
	LineTo ( hdc , lpRect->right - 6 , 9 ) ;
	MoveToEx ( hdc , 10 , 10 , NULL ) ;
	LineTo ( hdc , lpRect->right - 7 , 10 ) ;
	MoveToEx ( hdc , 9 , 9 , NULL ) ;
	LineTo ( hdc , 9 , 44 ) ;
	MoveToEx ( hdc , 10 , 10 , NULL ) ;
	LineTo ( hdc , 10 , 43 ) ;	// end drawing darkgray line in the top box
	MoveToEx ( hdc , 9 , 52 , NULL ) ;	// begin drawing darkgray line in the bottom box
	LineTo ( hdc , 9 , lpRect->bottom - 6 ) ;
	MoveToEx ( hdc , 10 , 53 , NULL ) ;
	LineTo ( hdc , 10 , lpRect->bottom - 7 ) ;
	MoveToEx ( hdc , 11 , 54 , NULL ) ;
	LineTo ( hdc , 11 , lpRect->bottom - 8 ) ;
	MoveToEx ( hdc , 9 , 52 , NULL ) ;
	LineTo ( hdc , lpRect->right - 7 , 52 ) ;
	MoveToEx ( hdc , 10 , 53 , NULL ) ;
	LineTo ( hdc , lpRect->right - 8 , 53 ) ;
	MoveToEx ( hdc , 11 , 54 , NULL ) ;
	LineTo ( hdc , lpRect->right - 9 , 54 ) ;	// end drawing darkgray line in the bottom box} */

	return TRUE ;
}


bool CheckPoint ( const POINT &lhs , const RECT &rhs )
{
	if ( lhs.x >= rhs.left && lhs.x <= rhs.right )
		if ( lhs.y >= rhs.top && lhs.y <= rhs.bottom )
			return true ;

	return false ;
}


void UpdateDisplay ( std::vector < size_t > * vec , const HWND & hWnd )
{
	size_t index ;
	RECT rect ;
	rect.left = BEGIN_X_OF_FIELD ;  ; //coord x of tiles
	rect.top = BEGIN_Y_OF_FIELD ; //         y 
	rect.right = 0 ;
	rect.bottom = 0 ;
	size_t test = vec->size () ;

	while ( ! vec->empty () )
	{
		index = vec->back () ;

		rect.left += index % uConfig->width * TILE_SIZE ;
		rect.top += index / uConfig->width * TILE_SIZE ;
		rect.right = rect.left + TILE_SIZE ;
		rect.bottom = rect.top + TILE_SIZE ;

		InvalidateRect ( hWnd , & rect , FALSE ) ;
		vec->pop_back () ;
	}
}