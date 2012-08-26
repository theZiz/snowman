#include "ballbullet.h"

tbullet ballbullet;
Sint32 ballBulletSize;
char ballBulletExists;

void fireBallBullet()
{
  if (ballBulletExists)
    return;
  if (ballcount<3)
    return;
  ballBulletSize=ballsize[0];
  ballsize[0]=0;
  ballBulletExists=1;
  ballbullet.color=getRGB(255,255,255);
  ballbullet.dx=(facedir)?(1<<(ACCURACY-7)):(-1<<(ACCURACY-7));
  ballbullet.dy=0;
  ballbullet.lifetime=2000;
  ballbullet.x=x;
  ballbullet.y=y-ballBulletSize;
  Mix_PlayChannel(-1,ballshot_chunk,0);
}

void drawBallBullet(Sint32 x,Sint32 y)
{
  if (!ballBulletExists)
    return;
  //engineEllipse(bullet->x-x,y-bullet->y,0,3<<(ACCURACY-3),3<<(ACCURACY-3),bullet->color);
  drawMeshXYZS(ballbullet.x-x,y-ballbullet.y,0,ballBulletSize,sphere,ballbullet.color);
}

void calcBallBullet()
{
  if (!ballBulletExists)
    return;
  ballbullet.x+=ballbullet.dx;
  ballbullet.y+=ballbullet.dy;
  ballbullet.lifetime--;
  char collision=0;
  if (ballbullet.lifetime<0)
    collision=1;
  penemy ebefore=NULL;
  penemy enemy=level->firstenemy;
  while (enemy!=NULL)
  {
    //Distance
    if ( ballbullet.x+ballBulletSize >= enemy->x-enemy->symbol->measures[2] &&
         ballbullet.x-ballBulletSize <= enemy->x+enemy->symbol->measures[2] &&
         ballbullet.y+ballBulletSize >= enemy->y-enemy->symbol->measures[3] &&
         ballbullet.y-ballBulletSize <= enemy->y+enemy->symbol->measures[3] ) //Hit
    {
      newexplosion(PARTICLES,ballbullet.x,ballbullet.y,0,1024,getRGB(255,255,255));
      ballBulletSize-=1<<(ACCURACY-5);
      if (ballBulletSize<=0)
        ballBulletExists=0;
      enemy->health--;
      newexplosion(PARTICLES,enemy->x,enemy->y,0,1024,enemy->symbol->color);
      if (enemy->health<=0)
      {
        enemyKilled++;
        if (ebefore==NULL)
          level->firstenemy=enemy->next;
        else
          ebefore->next=enemy->next;
        free(enemy);
        enemy=ebefore;
      }
      break;
    }
    ebefore=enemy;
    if (enemy!=NULL)
      enemy=enemy->next;
  }
  
  int bx =(( ballbullet.x                >>(ACCURACY))+1)>>1;
  int bxl=(((ballbullet.x-ballBulletSize)>>(ACCURACY))+1)>>1;
  int bxr=(((ballbullet.x+ballBulletSize)>>(ACCURACY))+1)>>1;
  int byb=(((ballbullet.y+ballBulletSize)>>(ACCURACY))+1)>>1;
  int byt=(((ballbullet.y-ballBulletSize)>>(ACCURACY))+1)>>1;
  int bym=(( ballbullet.y                >>(ACCURACY))+1)>>1;
  //Solid Block on the left?
  if (bxl>=0 && bxl<level->width)
  {
    if (byb>0 && level->symbollist[level->layer[1][bxl+(byb-1)*level->width]]       != NULL &&
                 level->symbollist[level->layer[1][bxl+(byb-1)*level->width]]->form > 0)
      collision=1;
    if (byt>=0 && level->symbollist[level->layer[1][bxl+(byt)*level->width]]       != NULL &&
                  level->symbollist[level->layer[1][bxl+(byt)*level->width]]->form > 0)
      collision=1;
    if (bym>=0 && level->symbollist[level->layer[1][bxl+(bym)*level->width]]       != NULL &&
                  level->symbollist[level->layer[1][bxl+(bym)*level->width]]->form > 0)
      collision=1;
  }
  //Solid Block on the right?
  if (bxr>=0 && bxr<level->width)
  {
    if (byb>0 && level->symbollist[level->layer[1][bxr+(byb-1)*level->width]]       != NULL &&
                 level->symbollist[level->layer[1][bxr+(byb-1)*level->width]]->form > 0)
      collision=2;
    if (byt>=0 && level->symbollist[level->layer[1][bxr+(byt)*level->width]]       != NULL &&
                  level->symbollist[level->layer[1][bxr+(byt)*level->width]]->form > 0)
      collision=2;
    if (bym>=0 && level->symbollist[level->layer[1][bxr+(bym)*level->width]]       != NULL &&
                  level->symbollist[level->layer[1][bxr+(bym)*level->width]]->form > 0)
      collision=2;
  }
  if (collision)
  {  
    Sint32 c=ballBulletSize>>(ACCURACY-4);
    int i;
    for (i=0;i<c;i++)
      newBullet(ballbullet.x,ballbullet.y,mycos((2*MY_PI*i)/c)>>5,mysin((2*MY_PI*i)/c)>>5,1000,2,ballbullet.color);
    ballBulletExists=0;
  }
}
