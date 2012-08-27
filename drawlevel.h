void drawlevel(plevel level,Sint32 mx,Sint32 my,Sint32 dx,Sint32 dy)
{
  int l,x,y;
  Sint32 minx=-dx;
  Sint32 maxx=+dx;
  Sint32 miny=-dy;
  Sint32 maxy=+dy;
  for (x=0;x<level->width;x++)
  {
    if ((( 2*x)<<SP_ACCURACY)-mx < minx)
      continue;
    if ((( 2*x)<<SP_ACCURACY)-mx > maxx)
      continue;
    for (y=0;y<level->height;y++)
    {
      if (((-2*y)<<SP_ACCURACY)+my < miny)
        continue;
      if (((-2*y)<<SP_ACCURACY)+my > maxy)
        continue;
      for (l=0;l<3;l++)
      {
        psymbol now=level->symbollist[level->layer[l][x+y*level->width]];
        while (now!=NULL)
        {
          if (now->mesh!=NULL)
          {
            if ((now->functionmask & 2)!=2 || now->needed_level<=levelcount)
              spMesh3DwithPos(((2*x)<<SP_ACCURACY)-mx,((-2*y)<<SP_ACCURACY)+my,(l-1)<<SP_ACCURACY,now->mesh,0);
          }
          else
          if ((now->functionmask & 1) == 1)
          {
            int c=abs(spSin(w)>>(SP_ACCURACY-7))+127;
            if (c>255)
              c=255;
            spEllipse3D(((2*x)<<SP_ACCURACY)-mx,((-2*y)<<SP_ACCURACY)+my,(l-1)<<(SP_ACCURACY+1),
                          spSin(w)*3/4,/*3<<(SP_ACCURACY-2),*/3<<(SP_ACCURACY-2),
                          spGetRGB(c,c,c)/*level->symbollist[level->layer[l][x+y*level->width]]->color*/);
          }
          else
          if ((now->meshmask & 2) == 2)
          {
						Sint32 tx,ty,tz;
						spProjectPoint3D(((2*x)<<SP_ACCURACY)-mx,((-2*y)<<SP_ACCURACY)+my,(l-1)<<(SP_ACCURACY+1),&tx,&ty,&tz,1);
            spFontDrawMiddle(tx,ty,tz,now->function,font);
          }
          now=now->next;
        }
          /*engineEllipse(((2*x)<<SP_ACCURACY)-mx,((-2*y)<<SP_ACCURACY)+my,(l-1)<<(SP_ACCURACY+1),
                        1<<SP_ACCURACY,1<<SP_ACCURACY,
                        level->symbollist[level->layer[l][x+y*level->width]]->color);*/
      }
    }
  }
}

void drawclouds(Sint32 mx,Sint32 my,Sint32 dx,Sint32 dy)
{
  int i;
  Sint32 minx=-dx-(20<<SP_ACCURACY);
  Sint32 maxx=+dx+(20<<SP_ACCURACY);
  Sint32 miny=-dy-(20<<SP_ACCURACY);
  Sint32 maxy=+dy+(20<<SP_ACCURACY);
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
    spMesh3DwithPos(cloudx[i]-mx,-cloudy[i]+my,cloudz[i],cloud,0);
  }
}
