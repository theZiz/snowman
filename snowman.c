#include <string.h>
#include <sparrow3d.h>
#include "splashscreen.h"
//#define SCALE_UP
spFontPointer font = NULL;
spFontPointer font_red = NULL;
spFontPointer font_green = NULL;
#ifdef SCALE_UP
SDL_Surface* real_screen;
#endif
SDL_Surface* screen = NULL;
#define CLOUD_COUNT 16
SDL_Surface* cloud[CLOUD_COUNT];

#define TIME_BETWEEN_TWO_JUMPS 20

int gameMode = 0; //easy

Uint16* mapPixel;
int mapLine;

void addBorder( spFontPointer font, Uint16 fontColor,Uint16 backgroundColor)
{
	int i;
	for (i = 0; i < (spGetSizeFactor() >> SP_ACCURACY)-1; i++)
		spFontAddBorder( font, fontColor );
	for (i = 0; i < (spGetSizeFactor()*3/2 >> SP_ACCURACY); i++)
		spFontAddBorder( font, backgroundColor );
	printf("%i %i\n",spGetSizeFactor(),spGetSizeFactor() >> SP_ACCURACY);
}

//#define FONT "./data/LondrinaOutline-Regular.ttf"
#define FONT "./data/Pompiere-Regular.ttf"
//#define FONT "./data/Lemon-Regular.ttf"
//#define FONT "./data/FugazOne-Regular.ttf"
//#define FONT "./data/ChelaOne-Regular.ttf"
//#define FONT "./data/BubblegumSans-Regular.ttf"
#define FONT_SIZE 13


void resize( Uint16 w, Uint16 h )
{
	#ifdef SCALE_UP
	if (screen)
		spDeleteSurface(screen);
	screen = spCreateSurface(real_screen->w/2,real_screen->h/2);
	#endif
	spSelectRenderTarget(screen);
	//Setup of the new/resized window
	spSetPerspective( 50.0, ( float )screen->w / ( float )screen->h, 1.0, 100 );

	int scale = 0;
	#ifdef SCALE_UP
	scale++;
	#endif
	//Font Loading
	if ( font )
		spFontDelete( font );
	font = spFontLoad( FONT, FONT_SIZE * spGetSizeFactor() >> SP_ACCURACY+scale );
	spFontAdd( font, SP_FONT_GROUP_ASCII, 65535 ); //whole ASCII
	spFontAddButton( font, 'R', SP_BUTTON_START_NAME, 65535, SP_ALPHA_COLOR ); //Return == START
	spFontAddButton( font, 'B', SP_BUTTON_SELECT_NAME, 65535, SP_ALPHA_COLOR ); //Backspace == SELECT
	spFontAddButton( font, 'q', SP_BUTTON_L_NAME, 65535, SP_ALPHA_COLOR ); // q == L
	spFontAddButton( font, 'e', SP_BUTTON_R_NAME, 65535, SP_ALPHA_COLOR ); // e == R
	spFontAddButton( font, 'a', SP_BUTTON_LEFT_NAME, 65535, SP_ALPHA_COLOR ); //a == left button
	spFontAddButton( font, 'd', SP_BUTTON_RIGHT_NAME, 65535, SP_ALPHA_COLOR ); // d == right button
	spFontAddButton( font, 'w', SP_BUTTON_UP_NAME, 65535, SP_ALPHA_COLOR ); // w == up button
	spFontAddButton( font, 's', SP_BUTTON_DOWN_NAME, 65535, SP_ALPHA_COLOR ); // s == down button
	
	addBorder( font, 65535, spGetRGB(128,128,128) );


	if ( font_red )
		spFontDelete( font_red );
	font_red = spFontLoad( FONT, FONT_SIZE * spGetSizeFactor() >> SP_ACCURACY+scale );
	spFontAdd( font_red, SP_FONT_GROUP_ASCII, spGetRGB(255,128,128) ); //whole ASCII
	addBorder( font_red, spGetRGB(255,128,128), spGetRGB(128,64,64) );
	
	if ( font_green )
		spFontDelete( font_green );
	font_green = spFontLoad( FONT, FONT_SIZE * spGetSizeFactor() >> SP_ACCURACY+scale );
	spFontAdd( font_green, SP_FONT_GROUP_ASCII, spGetRGB(128,255,128) ); //whole ASCII
	addBorder( font_green, spGetRGB(128,255,128), spGetRGB(64,128,64) );

	//Creating Clouds
	int i;
	for (i = 0; i < CLOUD_COUNT; i++)
	{
		if (cloud[i])
			spDeleteSurface(cloud[i]);
		cloud[i] = spCreateSurface((96+rand()%96)*spGetSizeFactor() >> SP_ACCURACY,(48+rand()%48)*spGetSizeFactor() >> SP_ACCURACY);
		spSelectRenderTarget(cloud[i]);
		spClearTarget(SP_ALPHA_COLOR);
		int step = 16*spGetSizeFactor() >> SP_ACCURACY;
		int x;
		spSetZSet(0);
		spSetZTest(0);
		for (x = step+2; x < cloud[i]->w-step-2; x+=rand()%step)
		{
			int y_from = (x-cloud[i]->w/2)*(x-cloud[i]->w/2)*(cloud[i]->h/2)/((cloud[i]->w/2)*(cloud[i]->w/2));
			int y_to = cloud[i]->h - y_from;
			int y;
			for (y = y_from+step+2; y <= y_to-step-2; y+=rand()%step)
			{
				int c = rand()%16;
				spEllipse(x,y,-1,step,step,spGetFastRGB(190+c,190+c,190+c));
			}
		}
		spAddBorder(cloud[i],spGetFastRGB(150,150,150),SP_ALPHA_COLOR);
		spAddBorder(cloud[i],spGetFastRGB(100,100,100),SP_ALPHA_COLOR);
	}
	spSelectRenderTarget(screen);
	initSnow();
}
#include "intro.h"
#include "level.h"
#include "particle.h"

#define PARTICLES 16
#define ENEMY_COUNT 7

int show_snow = 1;

int jump_min_time;
SDL_Surface* sphere;
SDL_Surface* sphere_left;
SDL_Surface* sphere_right;
SDL_Surface* flake;
SDL_Surface* door_open;
SDL_Surface* door_closed;
SDL_Surface* door_boss_open;
SDL_Surface* door_boss_closed;
SDL_Surface* enemySur[ENEMY_COUNT];
spModelPointer broom;

Sint32 w=0;
Sint32 x,y;
Sint32 camerax,cameray;
int ballcount;
Sint32 ballsize[3];
Sint32 speedup;
Sint32 facedir;
int cloudcount;
Sint32 cloudx[16];
Sint32 cloudy[16];
Sint32 cloudz[16];
Sint32 clouds[16];
int gotchasmall;
int gotchabig;
int valuesmall;
int valuebig;

int fade;
int fade2;
int fader,fadeg,fadeb;
int damaged;

int bx;
int bxl;
int bxr;
int by;
int byb;
int sum;
int byt;
int bym;
int biggest;
int in_hit;

int enemyKilled;

plevel level = NULL;

spSound* shot_chunk;
spSound* jump_chunk;
spSound* ballshot_chunk;
spSound* positive_chunk;
spSound* negative_chunk;
spSound* hu_chunk;
char broom_exist;

int volume;
int volumefactor;
Sint32 angle;

int levelcount;

char pausemode;

int getBiggest()
{
	if (ballsize[0]>=ballsize[2] && ballsize[0]>=ballsize[1])
		return 0;
	if (ballsize[1]>=ballsize[2] && ballsize[1]>=ballsize[0])
		return 1;
  return 2;
}

#include "snow.h"
#include "enemy.h"
#include "drawlevel.h"
#include "drawcharacter.h"
#include "bullet.h"
#include "ballbullet.h"

void init_game(plevel level,char complete)
{
	jump_min_time = 0;
	angle=0;
	resetallparticle();
	in_hit = 0;
	broom_exist=0;
	ballBulletExists=0;
	enemyKilled=0;
	damaged=0;
	deleteAllBullets();
	gotchasmall=0;
	gotchabig=0;
	x=level->startx<<(SP_ACCURACY+1);
	camerax=x;
	y=level->starty<<(SP_ACCURACY+1);
	cameray=y;
	y+=1<<SP_ACCURACY;
	speedup=0;
	if (complete)
	{
		ballcount=1;
		ballsize[0]=0;//13<<(SP_ACCURACY-4);
		ballsize[1]=0;//9<<(SP_ACCURACY-4);
		ballsize[2]=7<<(SP_ACCURACY-4);
	}
	cloudcount=level->width*level->height/50+1;
	if (cloudcount>16)
		cloudcount=16;
	int i;
	for (i=0;i<cloudcount;i++)
	{
		cloudx[i]=(rand()%(2*level->width+40)-20)<<SP_ACCURACY;
		cloudy[i]=(rand()%(2*level->height+40)-20)<<SP_ACCURACY;
		cloudz[i]=-(rand()%20+10)<<SP_ACCURACY;
		clouds[i]=rand()%CLOUD_COUNT;
	}
	facedir=1;
}

void draw_game(void)
{
	Sint32* modellViewMatrix=spGetMatrix();

	spClearTarget(level->backgroundcolor);
	spResetZBuffer();
	spIdentity();

	modellViewMatrix[14]=-25<<SP_ACCURACY;
	SDL_LockSurface(level->mini_map);
	mapPixel = level->mini_map->pixels;
	int i;
	mapLine = level->mini_map->pitch/level->mini_map->format->BytesPerPixel;
	for (i = 0; i < mapLine*level->mini_map->h; i++)
		mapPixel[i] = SP_ALPHA_COLOR;

	//#ifdef PANDORA
	//	Sint32 dx=25<<SP_ACCURACY;
	//	Sint32 dy=15<<SP_ACCURACY;
	//#else
		Sint32 dy=16<<SP_ACCURACY;
		Sint32 dx=dy*screen->w/screen->h;
	//#endif
	spSetZSet(0);
	spSetZTest(0);
	drawclouds(camerax,cameray-(4<<SP_ACCURACY),dx,dy);
	spSetZSet(1);
	spSetAlphaTest(1);
	drawSnow(camerax,cameray);
	spSetZSet(1);
	spSetZTest(1);
	spSetAlphaTest(0);
	drawlevel(level,camerax,cameray-(4<<SP_ACCURACY),dx,dy);
	spSetZSet(0);
	spSetAlphaTest(1);
	//updating mini map
	int mx = (spFixedToInt(x)+1)/2;
	int my = (spFixedToInt(y))/2;
	if (mx >= 0 && mx < mapLine && my >= 0 && my<=level->mini_map->h)
		mapPixel[mx+my*mapLine] = spGetRGB(255,127,127);
	if (ballsize[0]>0)
	{
		my--;
		if (mx >= 0 && mx < mapLine && my >= 0 && my<=level->mini_map->h)
			mapPixel[mx+my*mapLine] = spGetRGB(255,127,127);		
	}
	drawcharacter(x-camerax,cameray-y-(4<<SP_ACCURACY),0,facedir);
	spSetZSet(1);
	spSetZTest(1);
	drawenemies(camerax,cameray-(4<<SP_ACCURACY),dx,dy);
	spSetZSet(0);
	spSetZTest(0);
	drawBullet(camerax,cameray-(4<<SP_ACCURACY),dx,dy);
	drawBallBullet(camerax,cameray-(4<<SP_ACCURACY));
	drawparticle(camerax,cameray-(4<<SP_ACCURACY),0,dx,dy);
	SDL_UnlockSurface(level->mini_map);
	spRotozoomSurface(screen->w-spFixedToInt(level->mini_map->w*spGetSizeFactor()),spFixedToInt(level->mini_map->h*spGetSizeFactor()),0,level->mini_map,spGetSizeFactor()*2,spGetSizeFactor()*2,0);
	
	char buffer[64];
	sprintf(buffer,"Killed %i/%i (Objective: %i)",enemyKilled,level->enemycount,level->havetokill);
	if (enemyKilled<level->havetokill)
		spFontDraw(1,1,0,buffer,font_red);
	else
		spFontDraw(1,1,0,buffer,font_green);


	sprintf(buffer,"%i",spGetFPS());
	spFontDrawRight(screen->w-1,screen->h-font->maxheight,0,buffer,font);
	sprintf(buffer,"Small Belly: %i/18		 Big Belly: %i/26",ballsize[1]>>(SP_ACCURACY-5),ballsize[0]>>(SP_ACCURACY-5));
	if (ballsize[1]<=0)
		spFontDraw(1,screen->h-font_red->maxheight,0,buffer,font_red);
	else
		spFontDraw(1,screen->h-font_green->maxheight,0,buffer,font_green);
	int whole_text_length = spFontWidth("Small Belly: %i/18		 Big Belly: %i/26",font);
		
	if (gotchasmall)
	{
		if (valuesmall>=0)
			sprintf(buffer,"+%i",valuesmall);
		else
			sprintf(buffer,"%i",valuesmall);
		int part_text_length = spFontWidth("Small Belly: ",font);
		spFontDraw((screen->w - whole_text_length>>1)+part_text_length,screen->h-font->maxheight*2,0,buffer,font);
	}
	if (gotchabig)
	{
		if (valuebig>=0)
			sprintf(buffer,"+%i",valuebig);
		else
			sprintf(buffer,"%i",valuebig);
		int part_text_length = spFontWidth("Small Belly: 18/18		 Big Belly: ",font);
		spFontDraw((screen->w - whole_text_length>>1)+part_text_length,screen->h-font->maxheight*2,0,buffer,font);
	}
	if (fade)
	{
		if (fade>512)
			spAddWhiteLayer((1024-fade)>>1);
		else
			spAddWhiteLayer(			fade >>1);
	}
	if (fade2)
	{
		if (fade2>512)
			spAddBlackLayer((1024-fade2)>>1);
		else
			spAddBlackLayer(			fade2 >>1);
	}
	if (pausemode)
	{
		spAddBlackLayer(128);
		spFontDrawMiddle(screen->w>>1,(screen->h>>1)-font->maxheight*7/2,-1,"Press [R] to unpause",font);

		if (gameMode)
			spFontDrawMiddle(screen->w>>1,(screen->h>>1)-font->maxheight*4/2,-1,"Mode: Hard[w]",font);
		else
			spFontDrawMiddle(screen->w>>1,(screen->h>>1)-font->maxheight*4/2,-1,"Mode: Easy[w]",font);

		sprintf(buffer,"Total volume: [q]%i %%[e]",volume*100/2048);
		spFontDrawMiddle(screen->w>>1,(screen->h>>1)-font->maxheight*1/2,-1,buffer,font);
		sprintf(buffer,"Music volume: [a]%i %%[d] of total volume",volumefactor*100/2048);
		spFontDrawMiddle(screen->w>>1,(screen->h>>1)+font->maxheight*1/2,-1,buffer,font);
		
		spFontDrawMiddle(screen->w>>1,(screen->h>>1)+font->maxheight*4/2,-1,"Press [s] to return to sublevel",font);
		spFontDrawMiddle(screen->w>>1,(screen->h>>1)+font->maxheight*6/2,-1,"Press [B] to quit",font);
	}
	#ifdef SCALE_UP
	spScale2XSmooth(screen,real_screen);
	#endif
	spFlip();
}

int calc_game(Uint32 steps)
{
	PspInput engineInput = spGetInput();
	if (engineInput->button[SP_BUTTON_L])
	{
		volume-=steps;
		if (volume<0)
			volume=0;
		spSoundSetVolume(volume>>4);
		spSoundSetMusicVolume(((volumefactor*volume)/(128<<4))>>5);
		savelevelcount();
	}
	if (engineInput->button[SP_BUTTON_R])
	{
		volume+=steps;
		if (volume>(128<<4))
			volume=128<<4;
		spSoundSetVolume(volume>>4);
		spSoundSetMusicVolume(((volumefactor*volume)/(128<<4))>>5);
		savelevelcount();
	}
	/*if (engineGetMuteKey())
	{
		printf("teenage MUTEnt ninja turtles\n");
		if (volumefactor>0)
			volumefactor=0;
		else
			volumefactor=128<<4;
		spSoundSetMusicVolume(((volumefactor*volume)/(128<<4))>>5);
		savelevelcount();
		engineSetMuteKey(0);
	}*/
	if (engineInput->button[SP_BUTTON_START])
	{
		engineInput->button[SP_BUTTON_START]=0;
		pausemode=1-pausemode;
		jump_min_time = 0;
	}
	if (engineInput->button[SP_BUTTON_SELECT])
		return 1;
	if (pausemode)
	{
		if (engineInput->button[SP_BUTTON_DOWN])
		{
			engineInput->button[SP_BUTTON_DOWN]=0;
			fade2=1024;
			pausemode=0;
		}
		if (engineInput->button[SP_BUTTON_LEFT] && engineInput->button[SP_BUTTON_RIGHT] && engineInput->button[SP_BUTTON_DOWN] && engineInput->button[SP_BUTTON_UP])
		{
			engineInput->button[SP_BUTTON_LEFT]=0;
			engineInput->button[SP_BUTTON_RIGHT]=0;
			engineInput->button[SP_BUTTON_DOWN]=0;
			engineInput->button[SP_BUTTON_UP]=0;
			return 1;
		}
		if (engineInput->button[SP_BUTTON_LEFT])
		{
			volumefactor-=steps;
			if (volumefactor<0)
				volumefactor=0;
			spSoundSetMusicVolume(((volumefactor*volume)/(128<<4))>>5);
			savelevelcount();
		}
		if (engineInput->button[SP_BUTTON_RIGHT])
		{
			volumefactor+=steps;
			if (volumefactor>(128<<4))
				volumefactor=128<<4;
			spSoundSetMusicVolume(((volumefactor*volume)/(128<<4))>>5);
			savelevelcount();
		}
		if (engineInput->button[SP_BUTTON_UP])
		{
			engineInput->button[SP_BUTTON_UP] = 0;
			gameMode = 1-gameMode;
			savelevelcount();
		}
		return 0;
	}
	if (fade)
	{
		int i;
		for (i=0;i<steps && fade>0;i++)
		{
			fade--;
			if (fade==511)
			{
				if (level,(level->symbollist[level->layer[1][bx+(byb-1)*level->width]]->functionmask & 64) == 64)
					return 1;
				printf("Sprich Freund und tritt ein!\n");
				char buffer[256];
				sprintf(buffer,"%s",level->symbollist[level->layer[1][bx+(byb-1)*level->width]]->function);
				char reset=(level->symbollist[level->layer[1][bx+(byb-1)*level->width]]->functionmask & 32) == 32;
				freeLevel(level);
				level=loadlevel(buffer);
				init_game(level,reset);
				ballsize[0] = 0;
				if (gameMode != 0)
					ballsize[1] = 0;
			}
		}
		return 0;
	}
	if (fade2)
	{
		int i;
		for (i=0;i<steps && fade2>0;i++)
		{
			fade2--;
			if (fade2==511)
			{
				printf("Sprich Freund und tritt ein!\n");
				char buffer[256];
				sprintf(buffer,"%s",level->failback);
				freeLevel(level);
				level=loadlevel(buffer);
				init_game(level,1);
			}
		}
		return 0;
	}
	if (gotchasmall)
	{
		gotchasmall-=steps;
		if (gotchasmall<0)
			gotchasmall=0;
	}
	if (gotchabig)
	{
		gotchabig-=steps;
		if (gotchabig<0)
			gotchabig=0;
	}

	w+=(steps*256)%(2*SP_PI);
	//Time based movement
	int step;
	for (step=0;step<steps;step++)
	{
		if (jump_min_time>0)
			jump_min_time--;
		//Camera
		Sint32 dx=(x-camerax)>>7;
		Sint32 dy=((y-cameray)*3)>>7;
		camerax+=dx;
		cameray+=dy;

		Sint32 ox=x;
		Sint32 oy=y;

		calcparticle();

		//Enemys
		moveenemies();
		bulletEnemy();

		//Bullets
		calcBullet();
		calcBallBullet();
		bulletEnemyInteraction();
		bulletPlayerInteraction();
		bulletEnvironmentInteraction();

		if (broom_exist && in_hit<=0 && engineInput->button[SP_BUTTON_RIGHT])
			in_hit=864;
		if (in_hit==768)
			broomEnemyInteraction(facedir);
		if (in_hit>0)
			in_hit--;

		//Setting some values
		biggest = getBiggest();
		bx =((x>>(SP_ACCURACY))+1)>>1;
		bxl=(((x-ballsize[biggest])>>(SP_ACCURACY))+1)>>1;
		bxr=(((x+ballsize[biggest])>>(SP_ACCURACY))+1)>>1;
		by =((y>>(SP_ACCURACY))+1)>>1;
		byb=by;
		if (((2*by-1)<<SP_ACCURACY)!=y)
			byb+=1;
		int sum=0;
		int i;
		for (i=3-ballcount;i<3;i++)
			sum+=ballsize[i]*2;
		byt =((((y-sum)>>(SP_ACCURACY))+1)>>1);
		bym =(byt + byb)>>1;
		char tofat=0;
		//Snow left?
		if (bxl>=0 && bxl<level->width)
		{
			 if (byb>0 && byb<=level->height && level->symbollist[level->layer[1][bxl+(byb-1)*level->width]]!= NULL)
			 {
				 if ((level->symbollist[level->layer[1][bxl+(byb-1)*level->width]]->functionmask & 1) == 1)
				 {
					 if (addonesnow())
					 {
						 level->layer[1][bxl+(byb-1)*level->width]=' ';
						 tofat=fattest(&x,ox);
					 }
				 }
				 else
				 if ((level->symbollist[level->layer[1][bxl+(byb-1)*level->width]]->functionmask & 16) == 16)
				 {
					 level->layer[1][bxl+(byb-1)*level->width]=' ';
					 broom_exist=1;
					 spSoundPlay(positive_chunk,-1,0,0,0);
				 }
			 }
			 if (byt>=0 && byt<level->height && level->symbollist[level->layer[1][bxl+(byt)*level->width]]!= NULL)
			 {
				 if ((level->symbollist[level->layer[1][bxl+(byt)*level->width]]->functionmask & 1) == 1)
				 {
					 if (addonesnow())
					 {
						 level->layer[1][bxl+(byt)*level->width]=' ';
						 tofat=fattest(&x,ox);
					 }
				 }
				 else
				 if ((level->symbollist[level->layer[1][bxl+(byt)*level->width]]->functionmask & 16) == 16)
				 {
					 level->layer[1][bxl+(byt)*level->width]=' ';
					 broom_exist=1;
					 spSoundPlay(positive_chunk,-1,0,0,0);
				 }
			 }
			 if (bym>=0 && bym<level->height && level->symbollist[level->layer[1][bxl+(bym)*level->width]]!= NULL)
			 {
				 if ((level->symbollist[level->layer[1][bxl+(bym)*level->width]]->functionmask & 1) == 1)
				 {
					 if (addonesnow())
					 {
						 level->layer[1][bxl+(bym)*level->width]=' ';
						 tofat=fattest(&x,ox);
					 }
				 }
				 else
				 if ((level->symbollist[level->layer[1][bxl+(bym)*level->width]]->functionmask & 16) == 16)
				 {
					 level->layer[1][bxl+(bym)*level->width]=' ';
					 broom_exist=1;
					 spSoundPlay(positive_chunk,-1,0,0,0);
				 }
			 }
		}
		//Snow right?
		if (bxr>=0 && bxr<level->width)
		{
			 if (byb>0 && byb<=level->height && level->symbollist[level->layer[1][bxr+(byb-1)*level->width]]			 != NULL)
			 {
				 if ((level->symbollist[level->layer[1][bxr+(byb-1)*level->width]]->functionmask & 1) == 1)
				 {
					 if (addonesnow())
					 {
						 level->layer[1][bxr+(byb-1)*level->width]=' ';
						 tofat=fattest(&x,ox);
					 }
				 }
				 else
				 if ((level->symbollist[level->layer[1][bxr+(byb-1)*level->width]]->functionmask & 16) == 16)
				 {
					 level->layer[1][bxr+(byb-1)*level->width]=' ';
					 broom_exist=1;
					 spSoundPlay(positive_chunk,-1,0,0,0);
				 }
			 }
			 if (byt>=0 && byt<level->height && level->symbollist[level->layer[1][bxr+(byt)*level->width]]			 != NULL)
			 {
				 if ((level->symbollist[level->layer[1][bxr+(byt)*level->width]]->functionmask & 1) == 1)
				 {
					 if (addonesnow())
					 {
						 level->layer[1][bxr+(byt)*level->width]=' ';
						 tofat=fattest(&x,ox);
					 }
				 }
				 else
				 if ((level->symbollist[level->layer[1][bxr+(byt)*level->width]]->functionmask & 16) == 16)
				 {
					 level->layer[1][bxr+(byt)*level->width]=' ';
					 broom_exist=1;
					 spSoundPlay(positive_chunk,-1,0,0,0);
				 }
			 }
			 if (bym>=0 && bym<level->height && level->symbollist[level->layer[1][bxr+(bym)*level->width]]			 != NULL)
			 {
				 if ((level->symbollist[level->layer[1][bxr+(bym)*level->width]]->functionmask & 1) == 1)
				 {
					 if (addonesnow())
					 {
						 level->layer[1][bxr+(bym)*level->width]=' ';
						 tofat=fattest(&x,ox);
					 }
				 }
				 else
				 if ((level->symbollist[level->layer[1][bxr+(bym)*level->width]]->functionmask & 16) == 16)
				 {
					 level->layer[1][bxr+(bym)*level->width]=' ';
					 broom_exist=1;
					 spSoundPlay(positive_chunk,-1,0,0,0);
				 }
			 }
		}

		//	Player
		if (!tofat)
		{
			if (engineInput->axis[0]==-1)
			{
				x-=2<<(SP_ACCURACY-7);
				angle+=2<<(SP_ACCURACY-8);
				if (angle>=2*SP_PI)
					angle-=2*SP_PI;
				facedir=0;
			}
			if (engineInput->axis[0]== 1)
			{
				x+=2<<(SP_ACCURACY-7);
				angle-=2<<(SP_ACCURACY-8);
				if (angle<0)
					angle+=2*SP_PI;
				facedir=1;
			}
			//Hm, where am I?

			if (testX(x,ox))
				x=ox;
		}

		//new calculation after the X-Check
		biggest = getBiggest();
		bx =((x>>(SP_ACCURACY))+1)>>1;
		bxl=(((x-ballsize[biggest])>>(SP_ACCURACY))+1)>>1;
		bxr=(((x+ballsize[biggest])>>(SP_ACCURACY))+1)>>1;

		if (by>level->height) //Fuck, I am dead...
		{
			fade2=1024;
			spSoundPlay(negative_chunk,-1,0,0,0);
			return 0;
		}
		//printf("bx: %i by-1: %i byt: %i \"%c\"	 \"%c\"\n",bx,by-1,byt,level->layer[1][bx+byt*level->width],level->layer[1][bx+(by-1)*level->width]);
		//negative gravitation?
		if (speedup<0 &&
				(bxr<0 || bxl>=level->width || 1))
		{
			Sint32 sum=0;
			int i;
			for (i=3-ballcount;i<3;i++)
				sum+=ballsize[i]*2;
			byt =((((y-sum)>>(SP_ACCURACY))+1)>>1);
			if (!(bxr<0 || bxl>=level->width || byt<0 ||
					((level->symbollist[level->layer[1][bxl+byt*level->width]]			 == NULL ||
						level->symbollist[level->layer[1][bxl+byt*level->width]]->form <= 0) &&
					 (level->symbollist[level->layer[1][bxr+byt*level->width]]			 == NULL ||
						level->symbollist[level->layer[1][bxr+byt*level->width]]->form <= 0))))
				speedup=0;
			else
			{
				speedup+=1<<(SP_ACCURACY-13);
				y+=speedup;
				by=((y>>(SP_ACCURACY))+1)>>1;
			}
		}
		else //gravitation?
		if (bxr<0 || bxl>=level->width || by<0 || by>=level->height ||
			 ((bxl<0						 || (level->symbollist[level->layer[1][bxl+by*level->width]]			 == NULL ||
															 level->symbollist[level->layer[1][bxl+by*level->width]]->form <= 0))	&&
				(bxr>=level->width || (level->symbollist[level->layer[1][bxr+by*level->width]]			 == NULL ||
															 level->symbollist[level->layer[1][bxr+by*level->width]]->form <= 0))	))
		{
			speedup+=1<<(SP_ACCURACY-13);
			y+=speedup;
			by=((y>>(SP_ACCURACY))+1)>>1;
		}
		else
		{
			speedup=0;
			y=(by-1)<<(SP_ACCURACY+1);
			y+=1<<SP_ACCURACY;
		}
		if (damaged>0)
			damaged--;
		playerEnemyInteraction();
	}
	//Jump
	if (engineInput->button[SP_BUTTON_UP] && jump_min_time <= 0)
	{
		jump_min_time = TIME_BETWEEN_TWO_JUMPS;
		int biggest=getBiggest();
		int bxl=(((x-ballsize[biggest])>>(SP_ACCURACY))+1)>>1;
		int bxr=(((x+ballsize[biggest])>>(SP_ACCURACY))+1)>>1;
		int by =((y>>(SP_ACCURACY))+1)>>1;
		if (by>=0 && by<level->height && !(bxr<0 || bxl>=level->width ||
				((level->symbollist[level->layer[1][bxl+by*level->width]]			 == NULL ||
					level->symbollist[level->layer[1][bxl+by*level->width]]->form <= 0) &&
				 (level->symbollist[level->layer[1][bxr+by*level->width]]			 == NULL ||
					level->symbollist[level->layer[1][bxr+by*level->width]]->form <= 0)))) //opposite of gravitaion conditions
		if (ballsize[1]>(0<<(SP_ACCURACY-5)))
		{
			removesnow(1);
			newexplosion(PARTICLES,x,y,0,1024,spGetRGB(255,255,255));
			speedup=-23<<(SP_ACCURACY-9);
			spSoundPlay(jump_chunk,-1,0,0,0);
		}
	}

	//Shooting
	if (engineInput->button[SP_BUTTON_LEFT])
	{
		engineInput->button[SP_BUTTON_LEFT]=0;
		int sum=0;
		int i;
		for (i=3-ballcount;i<3;i++)
			sum+=ballsize[i]*2;
		newBullet(x,y-(sum>>1),(facedir)?(1<<(SP_ACCURACY-5)):(-1<<(SP_ACCURACY-5)),0,1000,1,spGetRGB(255,255,255));
	}
	if (engineInput->button[SP_BUTTON_DOWN])
	{
		engineInput->button[SP_BUTTON_DOWN]=0;
		fireBallBullet();
	}

	//clouds
	int i;
	for (i=0;i<cloudcount;i++)
	{
		cloudx[i]-=steps*(1<<(SP_ACCURACY-9));
		if (cloudx[i]<(-50<<SP_ACCURACY))
		{
			cloudx[i]+=(2*level->width+100)<<SP_ACCURACY;
			cloudy[i]=(rand()%(2*level->height+40)-20)<<SP_ACCURACY;
			cloudz[i]=-(rand()%20+10)<<SP_ACCURACY;
		}
	}

	//Door
	if (engineInput->axis[1]==-1)
		if (bx>=0 && bx<level->width)
			if (byb>0 && byb<=level->height &&
			    level->symbollist[level->layer[1][bx+(byb-1)*level->width]]!= NULL &&
					(level->symbollist[level->layer[1][bx+(byb-1)*level->width]]->functionmask & 2) == 2)
				if (enemyKilled>=level->havetokill && level->symbollist[level->layer[1][bx+(byb-1)*level->width]]->needed_level<=levelcount)
				{
					if (level->symbollist[level->layer[1][bx+(byb-1)*level->width]]->needed_level<0)
					{
						if (levelcount<-level->symbollist[level->layer[1][bx+(byb-1)*level->width]]->needed_level)
							levelcount=-level->symbollist[level->layer[1][bx+(byb-1)*level->width]]->needed_level;
						savelevelcount();
					}
					fade=1024;
					return 0;
				}

	sum=0;
	for (i=3-ballcount;i<3;i++)
		sum+=ballsize[i]*2;
	calcSnow(steps);
	return 0;
}

void init_snowman()
{
	levelcount=10;
	pausemode=0;
	volume=128<<4;
	volumefactor=128<<4;
	loadlevelcount();
	sphere=spLoadSurface("./data/sphere.png");
	sphere_left=spLoadSurface("./data/sphere_left.png");
	sphere_right=spLoadSurface("./data/sphere_right.png");
	flake=spLoadSurface("./data/snowflake.png");
	door_open=spLoadSurface("./data/door.png");
	door_closed=spLoadSurface("./data/door_closed.png");
	door_boss_open=spLoadSurface("./data/door_boss.png");
	door_boss_closed=spLoadSurface("./data/door_boss_closed.png");
	int i;
	for (i = 0; i < ENEMY_COUNT; i++)
	{
		char buffer[256];
		sprintf(buffer,"./data/enemy%i.png",i);
		enemySur[i]=spLoadSurface(buffer);
	}
	broom=spMeshLoadObj("./data/broom.obj",spLoadSurface("./data/broom.png"),spGetRGB(255,255,255));
	fade=0;
	fade2=0;
	spSoundInit();
	spSoundSetChannels(32);
	spSoundSetVolume(volume>>4);
	spSoundSetMusicVolume(((volumefactor*volume)/(128<<4))>>5);
	spSoundSetMusic("./sounds/Cold Funk.ogg");
	spSoundPlayMusic(0, -1);
	shot_chunk=spSoundLoad("./sounds/plop.wav");
	ballshot_chunk=spSoundLoad("./sounds/plop2.wav");
	jump_chunk=spSoundLoad("./sounds/shot.wav");
	positive_chunk=spSoundLoad("./sounds/positive.wav");
	hu_chunk=spSoundLoad("./sounds/hu.wav");
	negative_chunk=spSoundLoad("./sounds/negative.wav");
	spSetLight(1);
	spSetAmbientLightColor(1 << SP_ACCURACY-1,1 << SP_ACCURACY-1,1 << SP_ACCURACY-1);
	spSetLightColor(1,1 << SP_ACCURACY,1 << SP_ACCURACY,1 << SP_ACCURACY);
	spUsePrecalculatedNormals(1);
}

void quit_snowman()
{
	spSoundDelete(shot_chunk);
	spSoundDelete(ballshot_chunk);
	spSoundDelete(jump_chunk);
	spSoundDelete(negative_chunk);
	spSoundDelete(hu_chunk);
	printf("Deleted Sounds\n");
	spSoundQuit();
	printf("Quit Sound\n");
	spDeleteSurface(sphere);
	spDeleteSurface(sphere_left);
	spDeleteSurface(sphere_right);
	spDeleteSurface(flake);
	spDeleteSurface(door_closed);
	spDeleteSurface(door_open);
	spDeleteSurface(door_boss_closed);
	spDeleteSurface(door_boss_open);
	int i;
	for (i = 0; i < CLOUD_COUNT; i++)
		spDeleteSurface(cloud[i]);
	for (i = 0; i < ENEMY_COUNT; i++)
		spDeleteSurface(enemySur[i]);
	printf("Deleted Surfaces\n");
	spMeshDelete(broom);
	printf("Deleted Meshs\n");
}

int main(int argc, char **argv)
{
	int i;
	for (i = 0; i < CLOUD_COUNT; i++)
		cloud[i] = NULL;
	//spSetDefaultWindowSize( 800, 480 );
	spInitCore();
	//Setup
	#ifdef SCALE_UP
	real_screen = spCreateDefaultWindow();
	resize( real_screen->w, real_screen->h );
	#else
	screen = spCreateDefaultWindow();
	resize( screen->w, screen->h );
	#endif
	run_splashscreen(resize);
	intro();
	init_snowman();
	if (argc < 2)
		level=loadlevel("./levels/menu.slvl");
	else
	{
		char buffer[256];
		sprintf(buffer,"./levels/%s.slvl",argv[1]);
		level=loadlevel(buffer);
	}
	init_game(level,1);
	spLoop(draw_game,calc_game,10,resize,NULL);
	freeLevel(level);
	#ifdef SCALE_UP
	spDeleteSurface(screen);
	#endif
	quit_snowman();
	spQuitCore();
	return 0;
}
