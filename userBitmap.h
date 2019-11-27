#define USERBITMAP


class userBitmap {
public :
	userBitmap () ;
	userBitmap ( const HINSTANCE &, const LPCTSTR fileName ) ;
	// userBitmap ( userBitmap & ) ; // TEST: copy creator
	~userBitmap () ;

	void set ( const HINSTANCE & , const LPCTSTR fileName ) ;
	void put ( const PAINTSTRUCT & , const HDC &, const int &posX , const int &posY , const DWORD &type ) ;
	const long& getX() { return _x ; }
	const long& getY() { return _y ; }
	const HBITMAP getHandle() { return hBitmap ; }
	void Test () ;

private :
	HBITMAP hBitmap ;
	long _x ;
	long _y ;
};


userBitmap::userBitmap ()
: hBitmap( NULL ) , _x ( 0 ) , _y ( 0 )
{}


// ERROR ... more study copy creator 
/*
userBitmap::userBitmap ( userBitmap &rhs )
{
	hBitmap = rhs.getHandle () ;
	x = rhs.getX () ;
	y = rhs.getY () ;
}
*/


userBitmap::userBitmap ( const HINSTANCE &hInstance , const LPCTSTR fileName )
{
	set ( hInstance , fileName ) ;
}


void userBitmap::set ( const HINSTANCE &hInstance , const LPCTSTR fileName )
{
	if ( hBitmap != NULL )
		DeleteObject ( hBitmap );

	/*
	BITMAP bitmap ;

	hBitmap = LoadBitmap ( hInstance, fileName ) ;
	GetObject ( hBitmap, sizeof ( BITMAP ), &bitmap ) ;	

    _x = bitmap.bmWidth ;
    _y = bitmap.bmHeight ;
	*/

	// BITMAP *bitmap = new BITMAP ;
	BITMAP bitmap ;

	hBitmap = LoadBitmap ( hInstance, fileName ) ;
	GetObject ( hBitmap, sizeof ( BITMAP ), & bitmap ) ;	

    // _x = bitmap->bmWidth ;
    // _y = bitmap->bmHeight ;
	_x = bitmap.bmWidth ;
	_y = bitmap.bmHeight ;

	// delete bitmap ;
}


void userBitmap::put ( const PAINTSTRUCT &ps , const HDC &hdcMem , const int &posX , const int &posY , const DWORD &type = SRCCOPY )
{

	if ( ps.hdc == NULL || hdcMem == NULL )
		return ;

	SelectObject ( hdcMem , getHandle () ) ;

	// int i = _x , j = _y ; // for test
	// int k = getX () , m = getY () ;
	// test = getHandle () ;

    BitBlt ( ps.hdc , posX  , posY , _x , _y , hdcMem, 0 , 0 , type ) ;
}


userBitmap::~userBitmap ()
{
	DeleteObject ( hBitmap );
}


void userBitmap::Test ()
{
	long i , j ;

	i = _x ;
	j = _y ;
}