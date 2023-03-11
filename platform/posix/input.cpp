#include "input.h"
#include "image.h"

using namespace vgui;

class CursorBitmap : public Bitmap
{
private:
	int _hotspot[2];
public:
	CursorBitmap( Cursor::DefaultCursor dc )
	{

	}

	virtual void getHotspot( int &x, int &y )
	{
		x = _hotspot[0];
		y = _hotspot[1];
	}
};

Cursor::Cursor( DefaultCursor dc ) :
	_dc( dc )
{
	CursorBitmap *bmp = new CursorBitmap( dc );

	int x, y;
	bmp->getHotspot( x, y );

	privateInit( bmp, x, y );
}
