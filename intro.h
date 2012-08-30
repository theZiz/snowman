int intro_pos=19000;

void draw_intro(void)
{
  spClearTarget(0);
  
  spFontDrawMiddle( screen->w/2 ,(((intro_pos-9400)*(screen->h>>6))>>7),-1,"There was the big war.",font);
  spFontDrawMiddle( screen->w/2 ,(((intro_pos-8600)*(screen->h>>6))>>7),-1,"The evil flat rotating",font);
  spFontDrawMiddle( screen->w/2 ,(((intro_pos-7800)*(screen->h>>6))>>7),-1,"circles took the power.",font);
  spFontDrawMiddle( screen->w/2 ,(((intro_pos-7000)*(screen->h>>6))>>7),-1,"But the resistance movement",font);
  spFontDrawMiddle( screen->w/2 ,(((intro_pos-6200)*(screen->h>>6))>>7),-1,"fought against the intruders.",font);
  spFontDrawMiddle( screen->w/2 ,(((intro_pos-5400)*(screen->h>>6))>>7),-1,"That was a long time ago.",font);
  spFontDrawMiddle( screen->w/2 ,(((intro_pos-4600)*(screen->h>>6))>>7),-1,"The movement is exhausted,",font);
  spFontDrawMiddle( screen->w/2 ,(((intro_pos-3800)*(screen->h>>6))>>7),-1,"but the enemies are still there.",font);
  spFontDrawMiddle( screen->w/2 ,(((intro_pos-3000)*(screen->h>>6))>>7),-1,"You are the last (snow)man.",font);
  spFontDrawMiddle( screen->w/2 ,(((intro_pos-2200)*(screen->h>>6))>>7),-1,"Bring it to an end.",font);
  #ifdef SCALE_UP
  spScale2XSmooth(screen,real_screen);
  #endif
  spFlip();
}

int calc_intro(Uint32 steps)
{
  PspInput engineInput = spGetInput();
  intro_pos-=steps;
  if (intro_pos<=0)
    return 1;
  if (engineInput->button[SP_BUTTON_LEFT] || engineInput->button[SP_BUTTON_RIGHT] ||
			engineInput->button[SP_BUTTON_DOWN] || engineInput->button[SP_BUTTON_UP] ||
			engineInput->button[SP_BUTTON_START] || engineInput->button[SP_BUTTON_SELECT])
	{
		spResetButtonsState();
    return 1;
  }
  
  return 0;
}

void intro()
{
	spSetZTest(0);
	spSetZSet(0);
  spLoop(draw_intro,calc_intro,10,resize,NULL);
}
