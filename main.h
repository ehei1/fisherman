#define MAIN

#include <windows.h>
#include <math.h>
#include <limits.h>
#include <assert.h>
#include <vector>
#include "resource.h"
#include "userDigitBitmap.h"
// #include "userTileBitmap.h"
#include "userField.h"
#include "userButton.h"

#define WM_USER_RESET       0x0400
#define RGB_BACKGROUND				RGB( 17 , 39 , 86 )
#define RGB_HIGHLIGHT				RGB( 0 , 133 , 248 )
#define RGB_SHADOW		RGB( 0 , 0 ,0 )
#define BEGIN_X_OF_FIELD 11
#define BEGIN_Y_OF_FIELD 54
#define TILE_SIZE 32
#define ID_TIMER    1
#define TIME_MAX 999

typedef unsigned int UINT;

// fuction
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK CustomProc (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK AboutProc (HWND , UINT , WPARAM , LPARAM ) ;
BOOL SetGame( const HWND ) ;
BOOL DrawBackground ( const HDC , const LPRECT ) ;
bool CheckPoint ( const POINT &lhs , const RECT &rhs  ) ;
void UpdateDisplay ( std::vector < size_t > * , const HWND & ) ;

// global var.
TCHAR szAppName[] = TEXT ("생선찾기");
TCHAR szClassName[] = TEXT ("MineGame");

// learn : no struct , only class ! if not you use only global variable.
int sys_width = 0 , sys_height = 0 ;

struct USERCONFIG {
	size_t difficulty;
	size_t mine;
	size_t width;
	size_t height;
	size_t menu;
	bool isReserved ;
//	int sys_width ;
//	int sys_height ;
} ;

USERCONFIG SET_BASIC = { DIFFICULTY_BASIC , 10 , 9 , 9 , ID_BASIC , true } ;
USERCONFIG SET_INTERMEDIATE = { DIFFICULTY_INTERMEDIATE , 40, 16 , 16 , ID_INTERMEDIATE , true  } ;
USERCONFIG SET_EXPERT = { DIFFICULTY_EXPERT, 99, 30 , 16 , ID_EXPERT , true  } ;
USERCONFIG SET_CUSTOM = { DIFFICULTY_CUSTOM, 40, 16 , 16 , ID_CUSTOM , true  } ;

struct USERPAINTSTRUCT {
	HDC* hdc ;
	HDC* hdcMem ;
	int x ;
	int y ;
	DWORD type ;
} ;


USERCONFIG *uConfig; // indicate current game property