void drawlevel(plevel level,Sint32 mx,Sint32 my,Sint32 dx,Sint32 dy)
{
  int l,x,y;
  Sint32 minx=-dx;
  Sint32 maxx=+dx;
  Sint32 miny=-dy;
  Sint32 maxy=+dy;
  for (x=0;x<level->width;x++)
  {
    if ((( 2*x)<<ACCURACY)-mx < minx)
      continue;
    if ((( 2*x)<<ACCURACY)-mx > maxx)
      continue;
    for (y=0;y<level->height;y++)
    {
      if (((-2*y)<<ACCURACY)+my < miny)
        continue;
      if (((-2*y)<<ACCURACY)+my > maxy)
        continue;
      for (l=0;l<3;l++)
      {
        psymbol now=level->symbollist[level->layer[l][x+y*level->width]];
        while (now!=NULL)
        {
          if (now->mesh!=NULL)
          {
            if ((now->functionmask & 2)!=2 || now->needed_level<=levelcount)
              drawMeshXYZ(((2*x)<<ACCURACY)-mx,((-2*y)<<ACCURACY)+my,(l-1)<<ACCURACY,now->mesh,now->color);
          }
          else
          if ((now->functionmask & 1) == 1)
          {
            int c=abs(mysin(w)>>(ACCURACY-7))+127;
            if (c>255)
              c=255;
            engineEllipse(((2*x)<<ACCURACY)-mx,((-2*y)<<ACCURACY)+my,(l-1)<<(ACCURACY+1),
                          mysin(w)*3/4,/*3<<(ACCURACY-2),*/3<<(ACCURACY-2),
                          getRGB(c,c,c)/*level->symbollist[level->layer[l][x+y*level->width]]->color*/);
          }
          else
          if ((now->meshmask & 2) == 2)
            engineDrawTextMXMY(((2*x)<<ACCURACY)-mx,((-2*y)<<ACCURACY)+my,(l-1)<<(ACCURACY+1),now->function);
          now=now->next;
        }
          /*engineEllipse(((2*x)<<ACCURACY)-mx,((-2*y)<<ACCURACY)+my,(l-1)<<(ACCURACY+1),
                        1<<ACCURACY,1<<ACCURACY,
                        level->symbollist[level->layer[l][x+y*level->width]]->color);*/
      }
    }
  }
}

void drawclouds(Sint32 mx,Sint32 my,Sint32 dx,Sint32 dy)
{
  int i;
  Sint32 minx=-dx-(20<<ACCURACY);
  Sint32 maxx=+dx+(20<<ACCURACY);
  Sint32 miny=-dy-(20<<ACCURACY);
  Sint32 maxy=+dy+(20<<ACCURACY);
  for (i=0;i<cloudcount;i++)
  {
    if (cloudx[i]-mx < minx)
      continue;
    if (cloudx[i]-mx > maxx)
      continue;
    if (-cloudy[i]+my < miny)
      continue;
    if (-cloudy[i]+my > maxy)
      continue;
    drawMeshXYZ(cloudx[i]-mx,-cloudy[i]+my,cloudz[i],cloud,65535);
  }
}
