#ifndef MAIN
#include "main.h"
#endif


enum button_status {
	button_up , button_down , field_check , game_over , game_pass
};


class userButton {
public :
	userButton () ;
	void Set ( const HWND & , const HINSTANCE & , const int &upImage , const int &downImage , const int &clickImage , const int &endImage , const int &passImage , const int &x = 0 , const int &y = 0 ) ;
	void Put ( const PAINTSTRUCT & , const HDC & ) ;
	const int GetSizeX () { return _upImage.getX () ; }
	const int GetSizeY () { return _upImage.getY () ; }
	void SetPoint ( const POINT &point ) { _posX = point.x ; _posY = point.y ; }
	void SetStatus ( button_status status ) { _status = status ; }
	const enum button_status &CheckStatus ( const POINT & ) ;
	const enum button_status &GetStatus () const { return _status ; }

private :
	HWND _hwnd ;
	userBitmap _upImage ;
	userBitmap _downImage ;
	userBitmap _endImage ;
	userBitmap _clickImage ;
	userBitmap _passImage ;

	long _posX ;
	long _posY ;
	long _sizeX ;
	long _sizeY ;

	enum button_status _status ;
} ;


userButton::userButton () :
 _posX ( 0 ) , _posY ( 0 ) , _status ( button_up ) , _hwnd ( NULL )
{}


void userButton::Set ( const HWND &hwnd , const HINSTANCE &hInstance , 
					  const int &upImage , const int &downImage , const int &clickImage , 
					  const int &endImage , const int &passImage ,
					  const int &x , const int &y )
{
	_hwnd = hwnd ;
	_upImage.set ( hInstance , MAKEINTRESOURCE ( upImage ) ) ;
	_downImage.set ( hInstance , MAKEINTRESOURCE ( downImage ) ) ;
	_clickImage.set ( hInstance , MAKEINTRESOURCE ( clickImage ) ) ;
	_endImage.set ( hInstance , MAKEINTRESOURCE ( endImage ) ) ;
	_passImage.set ( hInstance , MAKEINTRESOURCE ( passImage ) ) ;
	
	_posX = x ;
	_posY = y ;
	_sizeX = _upImage.getX () ;
	_sizeY = _upImage.getY () ;
}

			
void userButton::Put ( const PAINTSTRUCT &ps , const HDC &hdc )
{
	RECT rect = { _posX , _posY , _posX + _upImage.getX () , _posY + _upImage.getY () } ;

	switch ( _status )
	{
	case button_up :
		_upImage.put ( ps , hdc , _posX , _posY  ) ;
		break ;
	case button_down :
		_downImage.put ( ps , hdc , _posX , _posY ) ;
		break ;
	case field_check :
		_clickImage.put ( ps , hdc , _posX , _posY ) ;
		break ;
	case game_over :
		_endImage.put ( ps , hdc , _posX , _posY ) ;
		break ;
	case game_pass :
		_passImage.put ( ps , hdc , _posX , _posY ) ;
		break ;
	}

	GetClientRect ( _hwnd , &rect ) ;
}

const enum button_status &userButton::CheckStatus ( const POINT &point )
{
	// if clicked location is self, change status
	if ( point.x >= _posX && point.y >= _posY && point.x <= _posX + _sizeX && point.y <= _posY + _sizeY )
		_status = button_down ;

	return _status ;
}