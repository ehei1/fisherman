#include <vector>


#define MINE -1
#define NON_MINE 0
#define MAX_WIDTH	500
#define MAX_HEIGHT	500


enum face { clear , flaged , reserved , opened } ;
enum status { 
	wrong_mine = -3 ,
	exploded_mine ,
	mine ,
	no_mine ,
	around_1_mine , around_2_mine ,	around_3_mine , around_4_mine ,
	around_5_mine , around_6_mine , around_7_mine , around_8_mine
} ;

enum statusFlag {
	increase , decrease , reserve
} ;


class cMine
{
public :
	cMine() : _status ( no_mine ) , _zone_id ( 0 ) /*, _isReserved ( false ) */ {}  ;
	~cMine () {} 
	void SetStatus ( const enum status value ) { _status = value ; }
	void SetZoneId ( const size_t& value ) { _zone_id = value ; }
	void SetOpen () { _face = opened ; }
	void SetClose () { _face = clear ; }
	void SetFace ( const enum face value ) { _face = value ; }
	const enum status& GetStatus () { return _status ; }
	const size_t& GetZoneId () const { return _zone_id ; }
	const enum face& GetFace () const { return _face ; }
	// void SetReserved ( const bool &type ) { _isReserved = type ; }
	// size_t& GetResourceID () { return _resourceID ; }
	const bool IsClose () const { return opened != _face ; }
	const bool IsMine () const { return mine == _status ; }
	const bool IsReserved () const { return reserved == _face ; }
	const bool IsFlaged () const { return flaged == _face ; }
        
private :
	size_t _zone_id ;
	enum face _face ;
	enum status _status ;
	// bool _close ;
	// bool _isReserved ;
} ;


class cField 
{
public:
	cField ( const size_t &width , const size_t &height , const size_t &mine ) ;
	~cField () {} ;
	void Reset ( const size_t &width , const size_t &height , const size_t &mine ) ;
	void Restart () ;
	const bool RestoreRecentTile ( size_t & ) ;
	const bool RestoreRecentTile () ;
	const bool OnDownLeft ( const size_t & ) ;
	const bool OnUpLeft ( const size_t & ) ;
	const bool OnUpRight ( const size_t & ) ;
	const size_t GetSize () { return _field.size () ; }
	void SetReserved ( const bool & flag ) { _isReserved = flag ; }
	const bool& IsReserved () const { return _isReserved ; }
	const bool IsReservedAsIndex ( const size_t & ) const ;
	const bool IsFlagedAsIndex ( const size_t & ) const ;
	const bool & IsOver () const { return _isOver ; }
	const bool & IsPass () ;
	const int & GetSizeFoundMine () const { return _foundMine ; }
	std::vector < cMine > * GetField () { return &_field ; } // the method for test
	std::vector < size_t > * GetModifiedIndex ( void ) { return & _modifiedIndex ; }
	// void SetModifiedIndex ( std::vector < size_t > * vec ) { _modifiedIndex = vec ; }

private:
	void SetOpenArea () ;
	void PutMine () ;
	void CheckMine () ;
	void ClearField () ;
	// const bool CertificateIndex ( const size_t &index ) const { return ( _field.size () >= index ) ; }
	// void SetOver ( const bool type ) { _isOver = type ; }
	void OpenTilesAroundIndex ( const size_t & ) ;
	void OpenTileAsIndex ( const int & , const int & , const size_t & ) ;
	const bool CheckMineAsIndex ( const size_t & , const size_t & ) ;
	const bool CheckMineAroundIndex ( const size_t & , size_t & ) ;
	
	size_t _width ;
	size_t _height ;
	size_t _mine ;
	int _foundMine ;
	bool _isOver ;
	bool _isPass ;
	bool _isReserved ;
	int _recentTouchIndex ;
	enum status _recentTouchStatus ;
	enum face _recentTouchFace ;
	std::vector < cMine > _field ;
	std::vector < size_t > _modifiedIndex ;
} ;


cField::cField ( const size_t &width , const size_t &height , const size_t &mine )
:	_width ( width ) , _height ( height ) , _mine ( mine ) , _isReserved ( true )
{
	if ( _width > MAX_WIDTH )
		_width = MAX_WIDTH ;

	if ( _height > MAX_HEIGHT )
		_height = MAX_HEIGHT ;

	// problem : _field ( quantityX + 1 ... ) is invaild. if it's value is quantityX then unexpected error will occur when game quit.
	// _field.resize ( ( _width + 1 ) * ( _height + 1 ) ) ; // temporary ...
	_field.resize ( _width * _height ) ;
	
	Restart () ;
}


void cField::Reset ( const size_t &width , const size_t &height , const size_t &mine )
{
	_width = width ;
	_height = height ;
	_mine = mine ;
	_field.resize ( _width * _height ) ;

	Restart () ;
}

void cField::Restart()
{
	this->_isOver = false ;
	this->_foundMine = static_cast < int > ( this->_mine ) ;
	this->_recentTouchIndex = -1 ;
	this->_isOver = this->_isPass = false ;

	ClearField () ;
	PutMine () ;
	CheckMine () ;
	SetOpenArea () ;
}


void cField::ClearField ()
{
	std::vector < cMine >::iterator it ;

	for ( it = _field.begin () ; it != _field.end () ; ++ it )
	{
		it->SetStatus ( no_mine ) ;
		it->SetZoneId ( 0 ) ;
		it->SetClose () ;
		it->SetFace ( clear ) ;
	}
}


void cField::PutMine ()
{
	enum status selector = mine ;
    size_t max_mine , min_mine ;
    size_t quantity , loop_num ;
    size_t step , rest_step ;
    size_t locator = 0 ;
    
	max_mine =( size_t ) (  _field.size() * 0.8 ) ;
	min_mine = ( size_t ) ( _field.size() * 0.1 ) ;

    // mine number is bigger than 80% of field, modify it.
    if ( _mine >= max_mine )
        quantity = max_mine ;
    else
	{
        if ( _mine < min_mine )
            quantity = min_mine ;
        else
            quantity = _mine ;
	}
    
    // select smaller whether mine or non mine place.
    if ( quantity > ( _field.size() / 2 ) ) 
    {
        selector = no_mine ; // now selector is non-mine place
        quantity = _field.size() - quantity ;
        
		std::vector< cMine >::iterator it = _field.begin() ;
        
        while ( ++it != _field.end() ) 
            it->SetStatus ( mine ) ;
    }
    
    // define step
    step = _field.size() / quantity ;
    
    // define skip step
    if ( rest_step = ( _field.size() - step * quantity ) ) 
    {
        srand ( ( int ) GetCurrentTime () ) ;
        locator = ( int ) ( rand() ) % rest_step ;
    }

    loop_num = quantity ;
    
    srand ( ( int ) GetCurrentTime () ) ;
    
	cMine * mine ;

    do
    {
        /* _field [ locator + ( rand() % step ) ].SetStatus ( selector ) ;
        locator += step */ ;
		size_t index = locator + ( rand() % step ) ;
		mine = & this->_field [ index ] ;
		mine->SetStatus ( selector ) ;
		locator += step ;
    }
    while ( -- loop_num ) ;
}


void cField::CheckMine()
{
    // size_t *status ;
    size_t mine ;
	cMine *cur_tile ;

	for ( size_t i = 0 ; i < _field.size () ; ++ i )
		if ( CheckMineAroundIndex ( i , mine ) )
		{
			cur_tile = & _field [ i ] ;

			switch ( mine )
			{
				case 0 :
					cur_tile->SetStatus ( no_mine ) ;
					break ;
				case 1 :
					cur_tile->SetStatus ( around_1_mine ) ;
					break ;
				case 2 :
					cur_tile->SetStatus ( around_2_mine ) ;
					break ;
				case 3 :
					cur_tile->SetStatus ( around_3_mine ) ;
					break ;
				case 4 :
					cur_tile->SetStatus ( around_4_mine ) ;
					break ;
				case 5 :
					cur_tile->SetStatus ( around_5_mine ) ;
					break ;
				case 6 :
					cur_tile->SetStatus ( around_6_mine ) ;
					break ;
				case 7 :
					cur_tile->SetStatus ( around_7_mine ) ;
					break ;
				case 8 :
					cur_tile->SetStatus ( around_8_mine ) ;
					break ;
				default :
					break ;
			}
		}
}


const bool cField::CheckMineAroundIndex ( const size_t & index , size_t & mine )
{
	if ( _field [ index ].IsMine () )
		return false ;

	mine = 0 ;
	size_t x = index % this->_width ;
	size_t y = index / this->_width ;

	for ( int locY = -1 ; locY < 2 ; ++ locY )
		for ( int locX = -1 ; locX < 2 ; ++ locX )
			if ( this->CheckMineAsIndex ( x + locX , y + locY ) )
				++ mine ;

	return true ;
}


const bool cField::CheckMineAsIndex ( const size_t & x , const size_t & y )
{
	if ( 0 > x || 0 > y || x >= this->_width || y >= this->_height )
		return false ;

	size_t index = x + y * this->_width ;

	if ( this->_field [ index ].IsMine () )
		return true ;

	return false ;
}

void cField::SetOpenArea()
{
    size_t left_id , up_id , cur_id ;
    size_t max_id = 1 ;
    
    // 반복자가 벡터 끝일때까지 순환 
    for ( size_t i = 0 ; i < _height ; ++ i )
        for ( size_t j = 0 ; j < _width ; ++ j )
            // 현재 타일 포함 주위 8타일에 지뢰가 없으면 
            if ( NON_MINE == _field [ j + i * _width ].GetStatus() )
            {
                left_id = up_id = cur_id = 0 ;
                
                //  체크하라 - x 값이 0이 아니고 왼쪽 타일의 zone_id가 있는지 
                if ( 0 != j )
                {
                    left_id =_field [ ( j - 1 ) + i * _width ].GetZoneId() ;
                    
                    if ( left_id )
                        cur_id = left_id ;
                }

                // 체크하라 - y 값이 0이 아니고 위쪽 타일의 zone_id가 있는지
                if ( 0 != i )
                {
                    up_id = _field [ j + ( i - 1 ) * _width ].GetZoneId() ;
                    
                    if ( up_id )
                        if ( left_id && left_id != up_id )
                        {
                            // 다르면 타일의 모든 기존 zone_id 값을 cur_id로 바꾼다. 
                            // (범위: 처음부터 ~ 현재 이전 반복자까지) 
                            for ( size_t k = 0 ; k < ( ( j - 1 ) + i * _width ) ; k ++ )
                                if ( _field [ k ].GetZoneId() == up_id )
                                    _field [ k ].SetZoneId( left_id ) ;
                        }
                        else
                            cur_id = up_id ;
                }
                    
                if ( ! cur_id )
                {
                    cur_id = max_id ;
                    ++ max_id ;
                }
                
                _field [ j + i *_width ].SetZoneId ( cur_id ) ;
            }
}


const bool cField::OnDownLeft ( const size_t &index )
{
	this->RestoreRecentTile () ;

	cMine *cur_tile = &_field [ index ] ;

	if ( ! cur_tile->IsClose () )
		return false ;

	this->_recentTouchIndex = static_cast < int > ( index ) ;
	this->_recentTouchStatus = cur_tile->GetStatus () ;
	this->_recentTouchFace = cur_tile->GetFace () ;

	cur_tile->SetOpen () ;
	cur_tile->SetStatus ( no_mine ) ;
	this->_modifiedIndex.push_back ( index ) ;
	return true ;
}


const bool cField::OnUpLeft ( const size_t &index )
{
	this->RestoreRecentTile () ;

	cMine *cur_tile = &_field [ index ] ;

	if ( ! cur_tile->IsClose () )
		return false ;

	std::vector < cMine >::iterator it ;

	switch ( cur_tile->GetStatus () )
	{
	case mine :
		_isOver = true ;
		break ;
	case no_mine :
		{
			size_t zone_id = cur_tile->GetZoneId () ;
		
			for ( size_t i = 0 ; i < _field.size () ; ++ i )
			{
				cMine * mine = & ( this->_field [ i ] ) ;
			
				if ( zone_id == mine->GetZoneId () )
					this->OpenTilesAroundIndex ( i ) ;
			}
		}
		break ;
	case around_1_mine : // there is around mine between 1 from 8
	case around_2_mine :
	case around_3_mine :
	case around_4_mine :
	case around_5_mine :
	case around_6_mine :
	case around_7_mine :
	case around_8_mine :
		cur_tile->SetOpen () ;
		this->_modifiedIndex.push_back ( index ) ;
		break ;
	}

	return true ;
}


const bool cField::OnUpRight ( const size_t &index )
{
	cMine *cur_tile = &_field [ index ] ;
	
	switch ( cur_tile->GetFace ()  )
	{
	case clear :
		cur_tile->SetFace ( flaged ) ;
		-- this->_foundMine ;

		break ;
	case flaged :
		if ( _isReserved )
			cur_tile->SetFace ( reserved ) ;
		else
			cur_tile->SetFace ( clear ) ;

		++ this->_foundMine ;
		break ;
	case reserved :
		cur_tile->SetFace ( clear ) ;
		break ;
	case opened :
		return false ;
	default :
		break ;
	}

	this->_modifiedIndex.push_back ( index ) ;
	return true ;
}


const bool cField::IsReservedAsIndex ( const size_t &index ) const 
{
	if ( 0 > index || ( int ) _field.size () < index )
		return false ; // later, changing to try~catch statement

	const cMine *cur_tile = &_field [ index ] ;

	// return cur_tile->IsReseved () ? true : false ;
	return cur_tile->IsReserved () ? true : false ;
}


const bool cField::IsFlagedAsIndex ( const size_t &index ) const
{
	if ( 0 > index || ( int ) _field.size () < index )
		return false ; // later, changing to try~catch statement

	const cMine *cur_tile = &_field [ index ] ;

	return cur_tile->IsFlaged () ? true : false ;
}


// open tiles around index
void cField::OpenTilesAroundIndex ( const size_t & index )
{
	// this->_modifiedIndex.push_back ( index ) ;
	// extract x , y value from index
	size_t x = index % this->_width ;
	size_t y = index / this->_width ;

	for ( int locY = -1 ; locY < 2 ; ++ locY )
		for ( int locX = -1 ; locX < 2 ; ++ locX )
			this->OpenTileAsIndex ( x + locX , y + locY , index ) ;
}


void cField::OpenTileAsIndex ( const int & x , const int & y , const size_t & indexSource )
{
	if ( 0 > x || 0 > y || x >= this->_width || y >= this->_height )
		return ;

	size_t index = x + y * this->_width ;

	if ( index > this->_field.size () )
		return ;

	const size_t * zoneId = & this->_field [ indexSource ].GetZoneId () ;

	if ( this->_field [ index ].GetZoneId () != * zoneId && this->_field [ index ].GetStatus () == no_mine )
		return ;

	if ( ! this->_field [ index ].IsClose () || this->_field [ index ].IsMine () || this->_field [ index ].IsFlaged () )
		return ;

	this->_field [ index ].SetOpen () ;
	this->_modifiedIndex.push_back ( index ) ;
}

const bool & cField::IsPass ( void )
{
	if ( this->_isOver )
		return this->_isPass ;

	size_t remainTile = 0 ;

	for ( size_t i = 0 ; i < this->_field.size () ; ++ i )
	{
		if ( this->_field [ i ].IsClose () )
			++ remainTile ;

		if ( remainTile > this->_mine )
			return ( this->_isPass = false ) ;
	}

	for ( size_t i = 0 ; i < this->_field.size () && 0 <= remainTile ; ++ i , -- remainTile )
		if ( this->_field [ i ].IsClose () && this->_field [ i ].IsMine () )
			this->_field [ i ].SetFace ( flaged ) ;

	return ( this->_isPass = this->_isOver = true ) ;
}


const bool cField::RestoreRecentTile ( void )
{
	if ( this->_recentTouchIndex < 0 )
		return false ;

	this->_modifiedIndex.push_back ( this->_recentTouchIndex ) ;

	cMine * recentTile = & this->_field [ this->_recentTouchIndex ] ;
	recentTile->SetClose () ;
	recentTile->SetStatus ( this->_recentTouchStatus ) ;
	recentTile->SetFace ( this->_recentTouchFace ) ;
	this->_recentTouchIndex = -1 ;
	return true ;
}


const bool cField::RestoreRecentTile ( size_t & index )
{
	index = this->_recentTouchIndex ;

	if ( this->RestoreRecentTile () )
		return true ;

	return false ;
}