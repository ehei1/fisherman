#define USERFIELD

#ifndef MAIN
#include "main.h"
#endif

#include "mine.h"
#include "userTileBitmap.h"


class userField 
{
public :
	userField ( const int &quantityX , const int &quantityY , const int &mine , const int &beginX , const int &beginY ) ;
	// userField ( const HINSTANCE & , const int &resourceBeginNumber , const int &resourceEndNumber ) ;
	// ~userField () ;

	void Set ( const HINSTANCE & , const int &resourceBeginNumber , const int &resourceEndNumber ) ;
	void Restart () ;
	const bool OnDownLeft ( const POINT & ) ;
	const bool OnUpLeft ( const POINT & ) ;
	const bool OnUpRight ( const POINT & ) ;
	void RefreshAll () ;
	void RefreshAsVector ( std::vector < size_t > * vec ) ;
	void Refresh ( const size_t &index ) ;
	void Disclosure ( const size_t & = 0 ) ;
	void Reset ( const int &width , const int &height , const int &mine ) ;
	const bool &IsOver () { return _isOver ; }
	const bool & IsPlay ( void ) { return _isPlay ; }
	void Paint ( const PAINTSTRUCT & , const HDC & ) ;
	void Clear () ;
	// const bool IsPass () { return _field.IsPass () ; }
	const bool IsPass () { return _isPass ; }
	const bool &IsReserved () const { return _field.IsReserved () ; }
	void SetReserved ( const bool &flag ) { _field.SetReserved ( flag ) ; }
	const int &GetSizeFoundMine () const { return _field.GetSizeFoundMine () ; }
	const bool RestorRecentTile ( void ) ;
	std::vector < size_t > * GetModifiedIndex ( void ) { return & _modifiedIndex ; }
	// void ClearModifiedIndex ( void ) { _modifiedIndex.clear () ; }

private :
	const bool CertificatePoint ( const POINT & , size_t &index  ) const ;
	std::vector < size_t > _modifiedIndex ;
	cField _field ;
	userTileBitmap _tile ;
	std::vector < userBitmap * > _paint ;
	size_t _quantityX ;
	size_t _quantityY ;
	size_t _mine ;
	bool _isOver ;
	bool _isPass ;
	bool _isPlay ;
	const size_t _beginX ; 
	const size_t _beginY ;
	size_t _resourceBeginNumber ;
	size_t _resourceEndNumber ;
	HINSTANCE _hInstance ;
} ;


// problem 1 : when this class had used USERSTRUCT type or static variable *uConfig , compiler occured unexpected error of type unknown
// problem 2 : hinstance value had got when WM_CREATE , creator using is very difficult. more think.
userField::userField ( const int &quantityX , const int &quantityY , const int &mine , const int &beginX , const int &beginY ) :
_quantityX ( quantityX ) , _quantityY ( quantityY ) , _mine ( mine ) , 
_field ( quantityX , quantityY , mine ) , _resourceBeginNumber ( 0 ) , _resourceEndNumber ( 0 ) ,
_beginX ( beginX ) , _beginY ( beginY ) ,
_hInstance ( NULL ), _isOver ( false )
{
	// this->_field.SetModifiedIndex ( & this->_modifiedIndex ) ;
}


void userField::Set ( const HINSTANCE &hInstance , const int &resourceBeginNumber , const int &resourceEndNumber )
{
	_hInstance = hInstance ;
	_resourceBeginNumber = resourceBeginNumber ;
	_resourceEndNumber = resourceEndNumber ;
	_tile.set ( _hInstance , _resourceBeginNumber , _resourceEndNumber ) ;
	
	Clear () ;
}


void userField::Clear ()
{
	int count = _quantityX * _quantityY ;
	userBitmap *init_value = _tile.Select ( IDB_TILE ) ;

	_paint.clear () ;

	// initialize with covered tile on field
	while ( count -- ) 
		_paint.push_back(  init_value ) ;
}


void userField::RefreshAll ()
{
	if ( this->_isOver )
		return ;

	for ( size_t i = 0 ; i < this->_field.GetSize () ; ++ i )
		this->Refresh ( i ) ;
}


void userField::RefreshAsVector ( std::vector < size_t > * vec )
{
	size_t index ;
	size_t test = vec->size () ;

	while ( ! vec->empty () )
	{
		index = vec->back () ;

		this->Refresh ( index ) ;
		this->_modifiedIndex.push_back ( index ) ;

		vec->pop_back () ;
	}

	test = this->_modifiedIndex.size () ;
}


void userField::Refresh ( const size_t &index )
{
	// WARNING: CRITICAL RESOURCE NEAK
	std::vector < cMine > * field = _field.GetField () ;
	cMine *cur_tile = & ( * field ) [ index ] ;

	if ( cur_tile->IsClose () )
		_paint [ index ] = _tile.Select ( IDB_TILE + cur_tile->GetFace () ) ;
	else
		_paint [ index ] = _tile.Select ( IDB_TILE_0 + cur_tile->GetStatus () ) ;

	// this->_modifiedIndex.push_back ( index ) ;
}


void userField::Disclosure ( const size_t & index ) 
{	
	if ( this->IsOver () )
		return ;

	// need upgrade for encapsulization !
	// for view mine field
	std::vector < cMine > * field ;
	field = _field.GetField () ;

	std::vector < cMine >::iterator it = field->begin () ;

	for ( int i = 0 ; it != field->end () ; ++ it , ++ i )
	{
		if ( it->IsMine () && it->IsFlaged () )
			continue ;

		if ( ! it->IsMine () && it->IsFlaged ()  ) 
		{
			_paint [ i ] = _tile.Select ( IDB_TILE_WRONG ) ;
			continue ;
		}

		if ( it->IsMine () && it->IsClose ()  )
			_paint [ i ] = _tile.Select ( IDB_TILE_MINE ) ;
	}

	_paint [ index ] = _tile.Select ( IDB_TILE_EXPLODED ) ;
	_isOver = true ;

	this->RefreshAll () ;
}


void userField::Paint ( const PAINTSTRUCT &ps , const HDC &hdc )
{
	size_t sizeX = _paint [ 0 ]->getX () , sizeY = _paint [ 0 ]->getY () ;
	size_t endX = sizeX * _quantityX - _beginX ;
	size_t curX = _beginX , curY = _beginY ;
	size_t tileOfLine = _quantityX ;

	for ( size_t i = 0 ; i < _paint.size () ; i ++ )
	{
		_paint [ i ]->put ( ps , hdc , curX , curY ) ;
		 
		if ( ( endX  - sizeX ) < curX )
		{
			curY += sizeY ;
			curX = _beginX ;
			tileOfLine = _quantityX ;
		}
		else
			curX += sizeX ;
	} 
}



void userField::Reset ( const int &width , const int &height , const int &mine )
{
	_quantityX = width ;
	_quantityY = height ;
	_mine = mine ;
	_isPass = _isPlay = _isOver = false ;
	
	_field.Reset ( _quantityX , _quantityY , _mine ) ;
	
	Clear () ;
}


const bool userField::OnDownLeft ( const POINT & point )
{
	if ( this->_isOver )
		return false ;

	size_t index ;

	if ( ! CertificatePoint ( point , index ) || _field.IsFlagedAsIndex ( index ) )
	{
		if ( this->_field.RestoreRecentTile () )
			this->RefreshAsVector ( this->_field.GetModifiedIndex () ) ;

		return false ;
	}

	if ( this->_field.OnDownLeft ( index ) )
	{
		this->RefreshAsVector ( this->_field.GetModifiedIndex () ) ;
		return true ;
	}

	this->RefreshAsVector ( this->_field.GetModifiedIndex () ) ;
	return false ;
}


const bool userField::OnUpLeft ( const POINT &point )
{
	if ( this->_isOver )
		return false ;


	/* if ( this->_field.RestoreRecentTile () )
		this->RefreshAsVector ( this->_field.GetModifiedIndex () ) ; */
	size_t restoreIndex ;

	if ( this->_field.RestoreRecentTile ( restoreIndex ) )
		this->Refresh ( restoreIndex ) ;
	
	size_t index ;

	if ( ! CertificatePoint ( point , index ) )
		return false ;

	if ( _field.IsFlagedAsIndex ( index ) )
		return false ;

	if ( _field.OnUpLeft ( index ) ) 
	{ 
		this->_isPlay = true ;

		if ( _field.IsOver () )
		{
			this->Disclosure ( index ) ;
			return true ;
		}
			
		if ( this->_field.IsPass () )
		{
			this->RefreshAll () ;
			this->_isPass = this->_isOver = true ;
			return true ;
		}

		this->RefreshAll () ;
		// size_t test = _field.GetModifiedIndex ()->size () ; // not available : cause not find
		// this->RefreshAsVector ( this->_field.GetModifiedIndex () ) ; // not available : cause not find
		return true ;
	}

	return false ;
}


const bool userField::OnUpRight ( const POINT &point )
{
	if ( this->_isOver )
		return false ;

	size_t index ;

	if ( ! CertificatePoint ( point , index ) )
		return false ;
	
	if ( _field.OnUpRight ( index ) ) 
	{
		// this->RefreshAsVector ( this->_field.GetModifiedIndex () ) ; // found bug
		Refresh ( index ) ;
		this->_modifiedIndex.push_back ( index ) ;
		return true ;
	}
	
	return false ;
}


const bool userField::CertificatePoint ( const POINT &point , size_t &index ) const
{
	size_t tileSizeX = _paint [ 0 ]->getX () ;
	size_t tileSizeY = _paint [ 0 ]->getY () ;

	if ( _beginX < point.x &&  _beginY < point.y && _beginX + _quantityX * tileSizeX > point.x && _beginY + _quantityY * tileSizeY > point.y )
	{
		size_t x = point.x - _beginX ;
		size_t y = point.y - _beginY ;

		index = x / tileSizeX + y / tileSizeY * _quantityX ;
		return true ;
	}
	
	return false ;
}


const bool userField::RestorRecentTile ( void )
{
	size_t restoreIndex ;

	if ( this->_field.RestoreRecentTile ( restoreIndex ) )
	{
		this->Refresh ( restoreIndex ) ;
		return true ;
	}

	return false ;
}