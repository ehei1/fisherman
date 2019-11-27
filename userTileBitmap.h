#define USERTILEBITMAP


#ifndef USERBITMAP
#include "userBitmap.h"
#endif

#include <stdlib.h>
#include <map>
#include <vector>


#ifndef USERBITMAP
#include "userBitmap.h"
#endif


class userTileBitmap
{
public :
	userTileBitmap () {} 
	userTileBitmap ( const HINSTANCE & ,  const int &resourceBeginNumber , const int &resourceEndNumber ) ;
	~userTileBitmap () ;
	void set ( const HINSTANCE & ,  const int &resourceBeginNumber , const int &resourceEndNumber ) ;
	void put ( const int &resourceID , const PAINTSTRUCT &ps , const HDC &hdcMem , const int &posX , const int &posY ) ;
	userBitmap * Select ( const int &resourceID ) ; 
	const int& GetSize () { return _size ; }
	
private :
	std::map < int , userBitmap * > _bitmap ; // dictionary bitmap as resource_id 
	std::vector < userBitmap * > pt_bitmap ; // backup pointer list for delete
	int _size ;
} ;


userBitmap * userTileBitmap::Select ( const int &resourceID ) 
{
	// begin of test
	userBitmap *test ;

	for ( int i = IDB_TILE + 0 ; i < IDB_TILE + _size ; i++ )
		test = _bitmap [ i ] ;
	// end of test

	return _bitmap [ resourceID ] ;
}


userTileBitmap::userTileBitmap ( const HINSTANCE & hInstance ,  const int &resourceBeginNumber , const int &resourceEndNumber ) :
_size ( 0 )
{
	set ( hInstance , resourceBeginNumber , resourceEndNumber ) ;
}


userTileBitmap::~userTileBitmap ()
{
	userBitmap *bitmap ;

	while ( ! pt_bitmap.empty () )
	{
		bitmap = pt_bitmap.back () ;
		pt_bitmap.pop_back () ;

		delete bitmap ;
	}
}


void userTileBitmap::set ( const HINSTANCE &hInstance,  const int &resourceBeginNumber , const int &resourceEndNumber )
{
	for ( int i = resourceBeginNumber ; i <= resourceEndNumber ; i ++ )
	{
		userBitmap *bitmap = new userBitmap ;

		if ( NULL == bitmap )
			return ; // ERROR ! insert assert or try~catch statement

		bitmap->set ( hInstance , MAKEINTRESOURCE ( i ) ) ;

		_bitmap [ i ] = bitmap ;
		
		int test = _bitmap [ i ]->getX () ; // for test

		pt_bitmap.push_back ( bitmap ) ;

		++ _size ;
	}

	/* using copy creator in userBitmap class. but it not run properly
	userBitmap bitmap ;

	for ( int i = resourceBeginNumber ; i <= resourceEndNumber ; i ++ )
	{
		bitmap.set ( hInstance , MAKEINTRESOURCE ( i ) ) ;
		_bitmap [ i ] = bitmap ;
	}
	*/
}


void userTileBitmap::put ( const int &resourceID , const PAINTSTRUCT &ps , const HDC &hdcMem , const int &posX , const int &posY )
{
	// if count of resourceID is 0, then there isn't it in the map of _bitmap
	if ( _bitmap.count ( resourceID ) )
		_bitmap [ resourceID ]->put ( ps , hdcMem , posX , posY ) ;
}