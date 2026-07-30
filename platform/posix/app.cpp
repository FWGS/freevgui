#include <time.h>
#include "app.h"

using namespace vgui;

class SurfacePlat
{
public:
	int bitmapSize[2];
	int restoreInfo[4];
	bool isFullscreen;
	int fullscreenInfo[3];
};

void App::internalSetMouseArena(int, int, int, int, bool)
{
	// stub
}

long int App::getTimeMillis()
{
	struct timespec ts;

	clock_gettime( CLOCK_MONOTONIC, &ts );

	return ts.tv_sec * 1000000 + ts.tv_nsec / 1000.0f;
}

void App::platTick()
{

}
