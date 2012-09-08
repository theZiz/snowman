typedef struct sbullet *pbullet;
typedef struct sbullet {
  Sint32 x,y;
  Sint32 dx,dy;
  int lifetime;
  char good;
  Uint16 color;
  pbullet next;
} tbullet;

pbullet firstBullet = NULL;

void deleteAllBullets()
{
  while (firstBullet!=NULL)
  {
    pbullet temp=firstBullet->next;
    free(firstBullet);
    firstBullet=temp;
  }
}

void newBullet(Sint32 x,Sint32 y,Sint32 dx,Sint32 dy, int lifetime,char good,Uint16 color)
{
  if (good==1)
  {
    if (ballcount==1 || (ballcount==2 && ballsize[1]<=(1<<(SP_ACCURACY-5))))
      return;
    removesnow(2);
  }
  if (good==1)
    spSoundPlay(shot_chunk,-1,0,0,0);
  pbullet bullet=(pbullet)malloc(sizeof(tbullet));
  bullet->x=x;
  bullet->y=y;
  bullet->dx=dx;
  bullet->dy=dy;
  bullet->lifetime=lifetime;
  bullet->good=good;
  bullet->color=color;
  bullet->next=firstBullet;
  firstBullet=bullet;  
}

void calcBullet()
{
  pbullet before=NULL;
  pbullet bullet=firstBullet;
  while (bullet!=NULL)
  {
    bullet->x+=bullet->dx;
    bullet->y+=bullet->dy;
    bullet->lifetime--;
    if (bullet->lifetime<0)
    {
      if (before==NULL)
        firstBullet=bullet->next;
      else
        before->next=bullet->next;
      newexplosion(PARTICLES,bullet->x,bullet->y,0,1024,bullet->color);
      free(bullet);
      bullet=before;
    }
    before=bullet;
    if (bullet!=NULL)
      bullet=bullet->next;
  }
  
}

void drawBullet(Sint32 x,Sint32 y,Sint32 dx,Sint32 dy)
{
  Sint32 minx=-dx;
  Sint32 maxx=+dx;
  Sint32 miny=-dy;
  Sint32 maxy=+dy;  
  pbullet bullet=firstBullet;
  while (bullet!=NULL)
  {
    if (bullet->x-x < minx ||
        bullet->x-x > maxx ||
        y-bullet->y < miny ||
        y-bullet->y > maxy)
    {
     bullet=bullet->next;
     continue; 
    }    
    spEllipse3D(bullet->x-x,y-bullet->y,0,3<<(SP_ACCURACY-3),3<<(SP_ACCURACY-3),bullet->color);
    bullet=bullet->next;
  }  
}

void bulletEnemyInteraction()
{
  pbullet bbefore=NULL;
  pbullet bullet=firstBullet;
  while (bullet!=NULL)
  {
    if (!bullet->good)
    {
      bbefore=bullet;
      bullet=bullet->next;
      continue;
    }
    penemy ebefore=NULL;
    penemy enemy=level->firstenemy;
    while (enemy!=NULL)
    {
      //Distance
      if ( bullet->x >= enemy->x-enemy->symbol->measures[2] &&
           bullet->x <= enemy->x+enemy->symbol->measures[2] &&
           bullet->y >= enemy->y-enemy->symbol->measures[3] &&
           bullet->y <= enemy->y+enemy->symbol->measures[3] ) //Hit
      {
        if (bbefore==NULL)
          firstBullet=bullet->next;
        else
          bbefore->next=bullet->next;
        newexplosion(PARTICLES,bullet->x,bullet->y,0,1024,bullet->color);
        free(bullet);
        bullet=bbefore;
        newexplosion(PARTICLES,enemy->x,enemy->y,0,1024,enemy->symbol->color);
        enemy->health--;
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
    bbefore=bullet;
    if (bullet!=NULL)
      bullet=bullet->next;
  }
}

void bulletEnvironmentInteraction()
{
  pbullet bbefore=NULL;
  pbullet bullet=firstBullet;
  while (bullet!=NULL)
  {
    Sint32 bx =((bullet->x>>(SP_ACCURACY))+1)>>1;
    Sint32 by =((bullet->y>>(SP_ACCURACY))+1)>>1;
    if (bx>=0 && bx<level->width && by>=0 && by<level->height &&
        level->symbollist[level->layer[1][bx+by*level->width]]!=NULL &&
        level->symbollist[level->layer[1][bx+by*level->width]]->form>0)
    {
      if (bbefore==NULL)
        firstBullet=bullet->next;
      else
        bbefore->next=bullet->next;
      newexplosion(PARTICLES,bullet->x,bullet->y,0,1024,bullet->color);
      free(bullet);
      bullet=bbefore;
    }
    bbefore=bullet;
    if (bullet!=NULL)
      bullet=bullet->next;
  }
}

void bulletEnemy()
{
  int sum=0;
  int i;
  for (i=3-ballcount;i<3;i++)
    sum+=ballsize[i]*2;
  Sint32 dx,dy,dl;
  penemy enemy = level->firstenemy;
  while (enemy!=NULL)
  {
    if (enemy->lastshot>0)
      enemy->lastshot--;
    else
    if (enemy->x >= x-(40<<SP_ACCURACY) && enemy->x <= x+(40<<SP_ACCURACY) &&
        enemy->y >= y-(40<<SP_ACCURACY) && enemy->y <= y+(40<<SP_ACCURACY))
      switch (enemy->weapon)
      {
        case 1: //just like the snowmans weapon
          newBullet(enemy->x,enemy->y,(enemy->dx>=0)?(1<<(SP_ACCURACY-5)):(-1<<(SP_ACCURACY-5)),0,1000,0,spGetRGB(0,0,255));
          enemy->lastshot=enemy->shotfq;
        break;
        case 2: //With targeting
          dx=x-enemy->x;
          dy=y-(sum>>1)-enemy->y;
          dl=spSqrt((dx>>SP_HALF_ACCURACY)*(dx>>SP_HALF_ACCURACY)+(dy>>SP_HALF_ACCURACY)*(dy>>SP_HALF_ACCURACY));
          if (dl!=0)
          {
            dx=((dx<<SP_HALF_ACCURACY)/dl)<<SP_HALF_ACCURACY;
            dy=((dy<<SP_HALF_ACCURACY)/dl)<<SP_HALF_ACCURACY;
            dx=dx>>5;
            dy=dy>>5;
          }
          newBullet(enemy->x,enemy->y,dx,dy,1000,0,spGetRGB(0,0,0));
          enemy->lastshot=enemy->shotfq;
        break;
        
      }
    enemy=enemy->next;
  }
}

void bulletPlayerInteraction()
{
  if (damaged)
    return;  
  int sum=0;
  int i;
  for (i=3-ballcount;i<3;i++)
    sum+=ballsize[i]*2;
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
  pbullet bbefore=NULL;
  pbullet bullet=firstBullet;
  while (bullet!=NULL)
  {
    if (bullet->good)
    {
      bbefore=bullet;
      bullet=bullet->next;
      continue;
    }
    //Distance
    if ( bullet->x >= x-ballsize[biggest] &&
         bullet->x <= x+ballsize[biggest] &&
         bullet->y >= y-sum &&
         bullet->y <= y      ) //Hit
    {
      if (bbefore==NULL)
        firstBullet=bullet->next;
      else
        bbefore->next=bullet->next;
      newexplosion(PARTICLES,bullet->x,bullet->y,0,1024,bullet->color);
      free(bullet);
      bullet=bbefore;
      
      damaged=SP_PI>>(SP_ACCURACY-8);
      if (removesnow(3))
      {
        fade2=1024;
        spSoundPlay(negative_chunk,-1,0,0,0);
        return;
      }
    }
    bbefore=bullet;
    if (bullet!=NULL)
      bullet=bullet->next;
  }
}
