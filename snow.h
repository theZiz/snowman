#define SNOW_COUNT 50

typedef struct snowStruct
{
	Sint32 x,y,size;
	Sint32 counter;
} tSnow;

tSnow snow[SNOW_COUNT];

void initSnow()
{
	int i;
	for (i = 0; i < SNOW_COUNT; i++)
	{
		snow[i].x = spIntToFixed(rand() % (screen->w));
		snow[i].y = spIntToFixed(rand() % (screen->h));
		snow[i].size = spMul(rand() % SP_ONE + SP_ONE,spGetSizeFactor()*2);
		snow[i].counter = rand() % (2*SP_PI);
	}
}

void drawSnow(Sint32 x,Sint32 y)
{
	Uint16 color = level->symbollist['#']->color;
	int i;
	x = -spFixedToInt(x*21);
	y = -spFixedToInt(y*21);
	x = x%(screen->w);
	if (x > screen->w/2)
		x-=screen->w;
	y = y%(screen->h);
	if (y > screen->h/2)
		y-=screen->h;
	for (i = 0; i < SNOW_COUNT; i++)
	{
		int X = spFixedToInt(snow[i].x)+x;
		int Y = spFixedToInt(snow[i].y)+y;
		if (X < 0)
			X += screen->w;
		if (X > screen->w)
			X -= screen->w;
		if (Y < 0)
			Y += screen->h;
		if (Y > screen->h)
			Y -= screen->h;
		spEllipse(X,Y,0,spFixedToInt(snow[i].size),spFixedToInt(snow[i].size),color);
		//spRotozoomSurface(X,Y,0,flake,snow[i].size>>4,snow[i].size>>4,snow[i].counter>>4);
	}
}

void calcSnow(int steps)
{
	int i;
	for (i = 0; i < SNOW_COUNT; i++)
	{
		snow[i].counter += i<<4;		
		snow[i].x += steps*spMul(spGetSizeFactor(),spSin(snow[i].counter)) >> 5;
		snow[i].y += steps*spGetSizeFactor() >> 6;
		if (spFixedToInt(snow[i].y) > screen->h )
			snow[i].y -= spIntToFixed(screen->h);
	}	
}
