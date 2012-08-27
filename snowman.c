#include <sparrow3d.h>

spFontPointer font = NULL;
spFontPointer font_red = NULL;
spFontPointer font_green = NULL;
SDL_Surface* screen;

void resize( Uint16 w, Uint16 h )
{
	//Setup of the new/resized window
	spSetPerspective( 50.0, ( float )spGetWindowSurface()->w / ( float )spGetWindowSurface()->h, 0.1, 100 );

	//Font Loading
	if ( font )
		spFontDelete( font );
	font = spFontLoad( "./data/LondrinaOutline-Regular.ttf", 17 * spGetSizeFactor() >> SP_ACCURACY );
	spFontAdd( font, SP_FONT_GROUP_ASCII, 65535 ); //whole ASCII
	spFontAddBorder( font, spGetRGB(192,192,192) );
	spFontAddBorder( font, spGetRGB(128,128,128) );
	if ( font_red )
		spFontDelete( font_red );
	font_red = spFontLoad( "./data/LondrinaOutline-Regular.ttf", 17 * spGetSizeFactor() >> SP_ACCURACY );
	spFontAdd( font_red, SP_FONT_GROUP_ASCII, spGetRGB(255,128,128) ); //whole ASCII
	spFontAddBorder( font_red, spGetRGB(192,96,96) );
	spFontAddBorder( font_red, spGetRGB(128,64,64) );
	if ( font_green )
		spFontDelete( font_green );
	font_green = spFontLoad( "./data/LondrinaOutline-Regular.ttf", 17 * spGetSizeFactor() >> SP_ACCURACY );
	spFontAdd( font_green, SP_FONT_GROUP_ASCII, spGetRGB(128,255,128) ); //whole ASCII
	spFontAddBorder( font_green, spGetRGB(96,192,96) );
	spFontAddBorder( font_green, spGetRGB(64,128,64) );
}

#include "intro.h"
#include "level.h"
#include "particle.h"

#define PARTICLES 16

spModelPointer sphere;
spModelPointer sphere_nose_left;
spModelPointer sphere_nose_right;
spModelPointer cloud;
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

#include "enemy.h"
#include "drawlevel.h"
#include "drawcharacter.h"
#include "bullet.h"
#include "ballbullet.h"

void init_game(plevel level,char complete)
{
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
  cloudcount=level->width*level->height/200;
  if (cloudcount>16)
    cloudcount=16;
  int i;
  for (i=0;i<cloudcount;i++)
  {
    cloudx[i]=(rand()%(2*level->width+40)-20)<<SP_ACCURACY;
    cloudy[i]=(rand()%(2*level->height+40)-20)<<SP_ACCURACY;
    cloudz[i]=-(rand()%20+10)<<SP_ACCURACY;
    clouds[i]=1<<SP_ACCURACY;
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

  //#ifdef PANDORA
  //  Sint32 dx=25<<SP_ACCURACY;
  //  Sint32 dy=15<<SP_ACCURACY;
  //#else
    Sint32 dy=16<<SP_ACCURACY;
    Sint32 dx=dy*screen->w/screen->h;
  //#endif
  drawclouds(camerax,cameray-(4<<SP_ACCURACY),dx,dy);
  drawlevel(level,camerax,cameray-(4<<SP_ACCURACY),dx,dy);
  drawcharacter(x-camerax,cameray-y-(4<<SP_ACCURACY),0,facedir);
  drawenemies(camerax,cameray-(4<<SP_ACCURACY),dx,dy);
  drawBullet(camerax,cameray-(4<<SP_ACCURACY),dx,dy);
  drawBallBullet(camerax,cameray-(4<<SP_ACCURACY));
  drawparticle(camerax,cameray-(4<<SP_ACCURACY),0,dx,dy);


  char buffer[64];
  sprintf(buffer,"Killed %i/%i (Objective: %i)",enemyKilled,level->enemycount,level->havetokill);
  if (enemyKilled<level->havetokill)
    spFontDrawMiddle(screen->w>>1,1,-1,buffer,font_red);
  else
    spFontDrawMiddle(screen->w>>1,1,-1,buffer,font_green);


  sprintf(buffer,"%i",spGetFPS());
  spFontDraw(0,0,-1,buffer,font);
  sprintf(buffer,"Small Belly: %i/18     Big Belly: %i/26",ballsize[1]>>(SP_ACCURACY-5),ballsize[0]>>(SP_ACCURACY-5));
  if (ballsize[1]<=0)
    spFontDrawMiddle(screen->w>>1,screen->h-font_red->maxheight,-1,buffer,font_red);
  else
    spFontDrawMiddle(screen->w>>1,screen->h-font_green->maxheight,-1,buffer,font_green);
  /*if (gotchasmall)
  {
    if (valuesmall>=0)
      sprintf(buffer,"            +%i                       ",valuesmall);
    else
      sprintf(buffer,"            %i                       ",valuesmall);
    spFontDrawMiddle(screen->w>>1,screen->h-5*(engineGetSurface(SURFACE_KEYMAP)->h>>5),buffer,engineGetSurface(SURFACE_KEYMAP));
  }
  if (gotchabig)
  {
    if (valuebig>=0)
      sprintf(buffer,"                                +%i   ",valuebig);
    else
      sprintf(buffer,"                                %i   ",valuebig);
    spFontDrawMiddle(screen->w>>1,screen->h-5*(engineGetSurface(SURFACE_KEYMAP)->h>>5),buffer,engineGetSurface(SURFACE_KEYMAP));
  }*/
  /*if (fade)
  {
    if (fade>512)
      engineAddWhiteLayer((1024-fade)>>1);
    else
      engineAddWhiteLayer(      fade >>1);
  }
  if (fade2)
  {
    if (fade2>512)
      engineAddBlackLayer((1024-fade2)>>1);
    else
      engineAddBlackLayer(      fade2 >>1);
  }*/
  if (pausemode)
  {
    //engineAddBlackLayer(192);
    spFontDrawMiddle(screen->w>>1,(screen->h>>1)-font->maxheight,-1,"Press "SP_BUTTON_START_NAME" to unpause",font);
    spFontDrawMiddle(screen->w>>1,(screen->h>>1)                ,-1,"Press "SP_BUTTON_SELECT_NAME" to return to submenu",font);
    spFontDrawMiddle(screen->w>>1,(screen->h>>1)+font->maxheight,-1,"Press "SP_BUTTON_A_NAME","SP_BUTTON_B_NAME","SP_BUTTON_X_NAME" and "SP_BUTTON_Y_NAME" to quit",font);
  }
  spFlip();
}

int calc_game(Uint32 steps)
{
  PspInput engineInput = spGetInput();
  if (engineInput->button[SP_BUTTON_VOLMINUS])
  {
    volume-=steps;
    if (volume<0)
      volume=0;
    spSoundSetVolume(volume>>4);
    spSoundSetMusicVolume(((volumefactor*volume)/(128<<4))>>5);
    savelevelcount();
  }
  if (engineInput->button[SP_BUTTON_VOLPLUS])
  {
    volume+=steps;
    if (volume>(128<<4))
      volume=128<<4;
    spSoundSetVolume(volume>>4);
    spSoundSetMusicVolume(((volumefactor*volume)/(128<<4))>>5);
    savelevelcount();
  }
  if (engineInput->button[SP_BUTTON_L])
  {
    volumefactor-=steps;
    if (volumefactor<0)
      volumefactor=0;
    spSoundSetMusicVolume(((volumefactor*volume)/(128<<4))>>5);
    savelevelcount();
  }
  if (engineInput->button[SP_BUTTON_R])
  {
    volumefactor+=steps;
    if (volumefactor>(128<<4))
      volumefactor=128<<4;
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
    Mix_VolumeMusic(((volumefactor*volume)/(128<<4))>>5);
    savelevelcount();
    engineSetMuteKey(0);
  }*/
  if (engineInput->button[SP_BUTTON_START])
  {
    engineInput->button[SP_BUTTON_START]=0;
    pausemode=1-pausemode;
  }
  if (pausemode)
  {
    if (engineInput->button[SP_BUTTON_SELECT])
    {
      engineInput->button[SP_BUTTON_SELECT]=0;
      fade2=1024;
      pausemode=0;
    }
    if (engineInput->button[SP_BUTTON_A] && engineInput->button[SP_BUTTON_B] && engineInput->button[SP_BUTTON_X] && engineInput->button[SP_BUTTON_Y])
    {
      engineInput->button[SP_BUTTON_A]=0;
      engineInput->button[SP_BUTTON_B]=0;
      engineInput->button[SP_BUTTON_X]=0;
      engineInput->button[SP_BUTTON_Y]=0;
      return 1;
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
    if (engineInput->button[SP_BUTTON_A])
    {
      engineInput->button[SP_BUTTON_A]=0;
      int sum=0;
      int i;
      for (i=3-ballcount;i<3;i++)
        sum+=ballsize[i]*2;
      newBullet(x,y-(sum>>1),(facedir)?(1<<(SP_ACCURACY-5)):(-1<<(SP_ACCURACY-5)),0,1000,1,spGetRGB(255,255,255));
    }
    calcBallBullet();
    if (engineInput->button[SP_BUTTON_X])
    {
      engineInput->button[SP_BUTTON_X]=0;
      fireBallBullet();
    }
    bulletEnemyInteraction();
    bulletPlayerInteraction();
    bulletEnvironmentInteraction();

    if (broom_exist && in_hit<=0 && engineInput->button[SP_BUTTON_B])
    {
      //engineInput->button[SP_BUTTON_B]=0;
      in_hit=288;
    }
    if (in_hit==192)
      broomEnemyInteraction(facedir);
    if (in_hit>0)
      in_hit-=1;
    testX(x,ox);
        
    char tofat=0;
    //Snow left?
    if (bxl>=0 && bxl<level->width)
    {
       if (byb>0 && level->symbollist[level->layer[1][bxl+(byb-1)*level->width]]       != NULL)
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
       if (byt>=0 && level->symbollist[level->layer[1][bxl+(byt)*level->width]]       != NULL)
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
       if (bym>=0 && level->symbollist[level->layer[1][bxl+(bym)*level->width]]       != NULL)
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
       if (byb>0 && level->symbollist[level->layer[1][bxr+(byb-1)*level->width]]       != NULL)
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
       if (byt>=0 && level->symbollist[level->layer[1][bxr+(byt)*level->width]]       != NULL)
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
       if (bym>=0 && level->symbollist[level->layer[1][bxr+(bym)*level->width]]       != NULL)
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
        
    //  Player
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
    biggest=2;
    if (ballcount>2)
    {
      if (ballsize[0]>ballsize[2] && ballsize[0]>ballsize[1])
        biggest=0;
      if (ballsize[1]>ballsize[2] && ballsize[1]>ballsize[0])
        biggest=1;
    }
    else
    if (ballcount>1)
    {
      if (ballsize[1]>ballsize[2])
        biggest=1;
    }
    bx =((x>>(SP_ACCURACY))+1)>>1;
    bxl=(((x-ballsize[biggest])>>(SP_ACCURACY))+1)>>1;
    bxr=(((x+ballsize[biggest])>>(SP_ACCURACY))+1)>>1;

    if (by>level->height) //Fuck, I am dead...
    {
      fade2=1024;
      spSoundPlay(negative_chunk,-1,0,0,0);
      return 0;
    }
    //printf("bx: %i by-1: %i byt: %i \"%c\"   \"%c\"\n",bx,by-1,byt,level->layer[1][bx+byt*level->width],level->layer[1][bx+(by-1)*level->width]);
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
          ((level->symbollist[level->layer[1][bxl+byt*level->width]]       == NULL ||
            level->symbollist[level->layer[1][bxl+byt*level->width]]->form <= 0) &&
           (level->symbollist[level->layer[1][bxr+byt*level->width]]       == NULL ||
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
       ((bxl<0             || (level->symbollist[level->layer[1][bxl+by*level->width]]       == NULL ||
                               level->symbollist[level->layer[1][bxl+by*level->width]]->form <= 0))  &&
        (bxr>=level->width || (level->symbollist[level->layer[1][bxr+by*level->width]]       == NULL ||
                               level->symbollist[level->layer[1][bxr+by*level->width]]->form <= 0))  ))
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
  if (engineInput->button[SP_BUTTON_Y])
  {
    int biggest=2;
    if (ballcount>2)
    {
      if (ballsize[0]>ballsize[2] && ballsize[0]>ballsize[1])
        biggest=0;
      if (ballsize[1]>ballsize[2] && ballsize[1]>ballsize[0])
        biggest=1;
    }
    else
    if (ballcount>1)
    {
      if (ballsize[1]>ballsize[2])
        biggest=1;
    }
    int bxl=(((x-ballsize[biggest])>>(SP_ACCURACY))+1)>>1;
    int bxr=(((x+ballsize[biggest])>>(SP_ACCURACY))+1)>>1;
    int by =((y>>(SP_ACCURACY))+1)>>1;
    if (by>=0 && by<level->height && !(bxr<0 || bxl>=level->width ||
        ((level->symbollist[level->layer[1][bxl+by*level->width]]       == NULL ||
          level->symbollist[level->layer[1][bxl+by*level->width]]->form <= 0) &&
         (level->symbollist[level->layer[1][bxr+by*level->width]]       == NULL ||
          level->symbollist[level->layer[1][bxr+by*level->width]]->form <= 0)))) //opposite of gravitaion conditions
    if (ballsize[1]>(0<<(SP_ACCURACY-5)))
    {
      removesnow(1);
      newexplosion(PARTICLES,x,y,0,1024,spGetRGB(255,255,255));      
      speedup=-23<<(SP_ACCURACY-9);
      spSoundPlay(jump_chunk,-1,0,0,0);
    }
    //engineInput->button[SP_BUTTON_Y]=0;
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
      if (byb>0 && level->symbollist[level->layer[1][bx+(byb-1)*level->width]]       != NULL &&
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
  return 0; 
}

void init_snowman()
{
  levelcount=10;
  pausemode=0;
  volume=128<<4;
  volumefactor=128<<4;
  loadlevelcount();
  sphere=spMeshLoadObj("./data/sphere.obj",NULL,65535);
  sphere_nose_left=spMeshLoadObjSize("./data/sphere_head.obj",NULL,spGetRGB(255,200,0),1<<SP_ACCURACY-1);
  //mirror X
  int i;
  for (i = 0; i < sphere_nose_left->pointCount; i++)
		sphere_nose_left->point[i].x = -sphere_nose_left->point[i].x;
  sphere_nose_right=spMeshLoadObjSize("./data/sphere_head.obj",NULL,spGetRGB(255,200,0),1<<SP_ACCURACY-1);
  cloud=spMeshLoadObj("./data/cloud.obj",NULL,65535);
  broom=spMeshLoadObj("./data/broom.obj",NULL,spGetRGB(86,22,0));
  fade=0;
  fade2=0;
	spSoundInit();
  spSoundSetVolume(volume>>4);  
  spSoundSetVolume(((volumefactor*volume)/(128<<4))>>5);
  spSoundSetMusic("./sounds/Cold Funk.ogg");
  spSoundPlayMusic(0, -1);
  shot_chunk=spSoundLoad("./sounds/plop.wav");
  ballshot_chunk=spSoundLoad("./sounds/plop2.wav");
  jump_chunk=spSoundLoad("./sounds/shot.wav");
  positive_chunk=spSoundLoad("./sounds/positive.wav");
  hu_chunk=spSoundLoad("./sounds/hu.wav");
  negative_chunk=spSoundLoad("./sounds/negative.wav");
  spSetZTest(1);
  spSetZSet(1);
  spSetLight(1);
}

void quit_snowman()
{
  spSoundDelete(shot_chunk);
  spSoundDelete(ballshot_chunk);
  spSoundDelete(jump_chunk);
  spSoundDelete(negative_chunk);
  spSoundDelete(hu_chunk);
  spSoundQuit();
  spMeshDelete(sphere);
  spMeshDelete(sphere_nose_left);
  spMeshDelete(sphere_nose_right);
  spMeshDelete(cloud);
  spMeshDelete(broom);
}

int main(int argc, char **argv)
{
	//spSetDefaultWindowSize( 640, 480 ); //Creates a 640x480 window at PC instead of 320x240
	spInitCore();

	//Setup
	screen = spCreateDefaultWindow();
	spSelectRenderTarget(screen);
	resize( screen->w, screen->h );  
  intro();
  init_snowman();
  level=loadlevel("./levels/menu.slvl");
  init_game(level,1);
  spLoop(draw_game,calc_game,10,resize,NULL);
  freeLevel(level);
  quit_snowman();
  spQuitCore();
  return 0;
}
