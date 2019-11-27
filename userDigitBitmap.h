#define USERDIGITBITMAP


#ifndef USERBITMAP
#include "userBitmap.h"
#endif


class userDigitBitmap {
public :
	userDigitBitmap::userDigitBitmap () {} ;
	userDigitBitmap ( const HINSTANCE & , const int &resourceBeginNumber , const int &resourceEndNumber , const int &resourceMinus) ;
	~userDigitBitmap () {} ;

	void set ( const HINSTANCE & ,  const int &resourceBeginNumber , const int &resourceEndNumber , const int &resourceMinus ) ;
	void put ( const int &number , const PAINTSTRUCT & , const HDC & , const int &posX , const int & posY , const DWORD &type , const int &size ) ;

	const int getX( const int &i ) { return bitmap [ i ].getX() ; }
	const int getY( const int &i ) { return bitmap [ i ].getY() ; }
	const HBITMAP getHandle ( int &i ) { return bitmap [ i ].getHandle() ; }

private :
	userBitmap bitmap [ 11 ] ;
};


userDigitBitmap::userDigitBitmap ( const HINSTANCE &hInstance,  const int &resourceBeginNumber , const int &resourceEndNumber , const int &resourceMinus )
{
	set ( hInstance , resourceBeginNumber , resourceEndNumber , resourceMinus ) ;
}


void userDigitBitmap::set ( const HINSTANCE &hInstance,  const int &resourceBeginNumber , const int &resourceEndNumber , const int &resourceMinus )
{
	int seq = 0 ;

	for ( int i = resourceBeginNumber ; i <= resourceEndNumber ; i ++ )
		bitmap [ seq ++ ].set ( hInstance , MAKEINTRESOURCE ( i ) ) ;

	bitmap [ seq ].set ( hInstance , MAKEINTRESOURCE ( resourceMinus ) ) ;
}


/*
// prototype: only one numeric print
void userDigitBitmap::put ( const int number , const PAINTSTRUCT &ps , const HDC hdcMem , const int posX , const int posY , const DWORD type )
{
	bitmap [ number ].put ( ps , hdcMem , posX , posY , type ) ;
}
*/


void userDigitBitmap::put ( const int &number , const PAINTSTRUCT &ps , const HDC &hdcMem , const int &posX , const int &posY , const DWORD &type , const int &size )
{
	//	1. display number unlimitly. At first you must to have the "number" variable with string.
	if ( size > 9 || size < 0 || number > INT_MAX )
		return ;

	int printNumber ;

	if ( 0 > number )
	{
		if ( -99 > number )
			printNumber = 99 ;
		else
			printNumber = abs ( number ) ;
	}
	else
		printNumber = number ;

	int dividend = 1 ;
	int figure = 0 ;
	int iQuantityOfZero = 0 ;

	while ( dividend < printNumber ) 
	{
		++ figure ;
		dividend *= 10 ;
	}

	if ( size >= figure ) 
	{
		iQuantityOfZero = size - figure ;
		dividend = ( int ) pow ( ( double ) 10 , ( figure = size ) ) ;
	}

	int digit ;
	int curX = posX ;

	if ( number < 0 )
	{
		bitmap [ 10 ].put ( ps , hdcMem , curX , posY , type ) ; // minus sign
		curX += bitmap [ 10 ].getX () ;

		-- iQuantityOfZero ;
		-- figure ;
		dividend /= 10 ;
	}

	while ( figure -- )
	{
		if ( ( dividend = dividend / 10 ) >= printNumber && ! size && iQuantityOfZero ) 
		{
			-- iQuantityOfZero ;
			continue ;
		}
		
		digit = printNumber / dividend ;
		bitmap [ digit ].put ( ps , hdcMem , curX , posY , type ) ;
		curX += bitmap [ digit ].getX () ;

		printNumber -= digit * dividend ;
	}
}