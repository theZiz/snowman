typedef struct sparticle *pparticle;
typedef struct sparticle {
  Sint32 x,y,z;
  Sint32 dx,dy,dz;
  Sint32 w,h;
  Uint16 color;
  int lifetime;
  char kind;
  pparticle next;
} tparticle;

pparticle firstparticle = NULL;

void newparticle(Sint32 x,Sint32 y,Sint32 z,Sint32 dx,Sint32 dy,Sint32 dz,Sint32 w,Sint32 h,int lifetime,char kind,Uint16 color)
{
  pparticle newparticle = (pparticle)malloc(sizeof(tparticle));
  newparticle->x=x;
  newparticle->y=y;
  newparticle->z=z;
  newparticle->dx=dx;
  newparticle->dy=dy;
  newparticle->dz=dz;
  newparticle->w=w;
  newparticle->h=h;
  newparticle->lifetime=lifetime;
  newparticle->kind=kind;
  newparticle->color=color;
  newparticle->next=firstparticle;
  firstparticle=newparticle;
}

void newexplosion(int count,Sint32 x,Sint32 y,Sint32 z,Sint32 intensity,Uint16 color)
{
  int i;
  for (i=0;i<count;i++)
  {
    newparticle(x,y,z,rand()%intensity-intensity/2,2*(rand()%intensity-intensity/2),rand()%intensity-intensity/2,1<<SP_ACCURACY,1<<SP_ACCURACY,256,0,color);
  }
}

void calcparticle()
{
  pparticle lastparticle=NULL;
  pparticle particle=firstparticle;
  while (particle!=NULL)
  {
    switch (particle->kind)
    {
      case 0:
        particle->dy+=1<<(SP_ACCURACY-14);
        particle->w-=1<<(SP_ACCURACY-8);
        particle->h-=1<<(SP_ACCURACY-8);
      break;
    }
    particle->x+=particle->dx;
    particle->y+=particle->dy;
    particle->z+=particle->dz;
    particle->lifetime--;
    if (particle->lifetime==0)
    {
      if (lastparticle==NULL)
        firstparticle=particle->next;
      else
        lastparticle->next=particle->next;
      free(particle);
      particle=lastparticle;
    }
    lastparticle=particle;
    if (particle!=NULL)
      particle=particle->next;
  }
  
}

void drawparticle(Sint32 x,Sint32 y,Sint32 z,Sint32 dx,Sint32 dy)
{
  Sint32 minx=-dx;
  Sint32 maxx=+dx;
  Sint32 miny=-dy;
  Sint32 maxy=+dy;  
  pparticle particle=firstparticle;
  while (particle!=NULL)
  {
    if (particle->x-x > minx &&
        particle->x-x < maxx &&
        y-particle->y > miny &&
        y-particle->y < maxy)
    {
      switch (particle->kind)
      {
        case 0:
          spEllipse(particle->x-x,y-particle->y,particle->z-z,particle->w>>1,particle->h>>1,particle->color);
        break;
      }
    }
    particle=particle->next;
  } 
}

void resetallparticle()
{
  while (firstparticle!=NULL)
  {
    pparticle temp=firstparticle->next;
    free(firstparticle);
    firstparticle=temp;
  }
}
