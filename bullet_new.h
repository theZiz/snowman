typedef struct sbullet *pbullet;
typedef struct sbullet {
  Sint32 x,y;
  Sint32 dx,dy;
  int lifetime;
  char good;
  Uint16 color;
  pbullet next;
} tbullet;
