typedef struct ssymbol *psymbol;
typedef struct ssymbol {
	char symbol;
	char objectfile[256];
	spModelPointer mesh;
	int meshmask; //1 badcoin, 2 text, 4 door, 8 super door, 16 enemy (was badcoin)
	Uint16 color;
	char form; //0 none, 1 quad
	Sint32 measures[4];
	int functionmask; //1 snow, 2 teleport, 4 left, 8 right, 256 waywalker
	char function[512];
	int needed_level;
	int enemy_kind;
	psymbol next;
} tsymbol;

typedef struct senemy *penemy;
typedef struct senemy {
	Sint32 x;
	Sint32 y;
	Sint32 dx;	//Vector
	Sint32 was_change;
	psymbol symbol;
	int health;
	int maxhealth;
	int weapon; //0 noweapon, 1 weapon1, 2 weapon2 ...
	int lastshot;
	int shotfq;
	penemy next;
} tenemy;

typedef struct slevel *plevel;
typedef struct slevel {
	int width,height,startx,starty;
//	Sint32 startzoom;
	psymbol symbollist[256];
	char* layer[3];
	Uint16 backgroundcolor;
	penemy firstenemy;
	int enemycount;
	int havetokill;
	char failback[256];
} tlevel;

int levelerrorline;
char levelreadbytes;

char* readnextline(SDL_RWops *file,char* buffer,int bufferlen)
{
	if (bufferlen<=1)
		return NULL;
	int pos=0;
	char sign;
	while ((levelreadbytes=SDL_RWread(file,&sign,1,1))>0 && pos<bufferlen-1 && sign!='\n')
	{
		buffer[pos]=sign;
		pos++;
	}
	buffer[pos]=0;
	levelerrorline++;
	return buffer;
}

char firstsign(char* string)
{
	int pos=0;
	while (string[pos]==' ')
		pos++;
	return string[pos];
}

int strcmp_firstsign(char *first,char* second)
{
	while (first[0]==' ')
		first=&(first[1]);
	while (second[0]==' ')
		second=&(second[1]);
	return strcmp(first,second);	
}

int getNextWord(int ipos,char* input,char* output,int outputlen,char cbegin,char cend)
{
	if (outputlen<=1)
		return ipos;	
	int opos=0;
	while (input[ipos]==' ' || input[ipos]==cbegin)
		ipos++;
	while (input[ipos]!=cend && opos<outputlen-1 && input[ipos]!=0)
	{
		output[opos]=input[ipos];
		opos++;
		ipos++;
	}
	output[opos]=0;
	return ipos;
}

void freeLevel(plevel level)
{
	int i;
	for (i=0;i<255;i++)
		while (level->symbollist[i]!=NULL)
			{
				psymbol temp=level->symbollist[i]->next;
				if (level->symbollist[i]->mesh!=NULL)
					spMeshDelete(level->symbollist[i]->mesh);
				free(level->symbollist[i]);
				level->symbollist[i]=temp;
			}
	for (i=0;i<3;i++)
		free(level->layer[i]);
	while (level->firstenemy!=NULL)
	{
		penemy temp=level->firstenemy->next;
		free(level->firstenemy);
		level->firstenemy=temp;
	}
	free(level);
}

plevel loadlevel(char* filename)
{
	SDL_RWops *file=SDL_RWFromFile(filename,"rb");
	if (file==NULL)
	{
		printf("Fatal Error: Could not load %s\n",filename);
		return NULL;
	}
	levelerrorline=0;
	char buffer[8192];
	//head
	do
		readnextline(file,buffer,1024);
	while (firstsign(buffer)=='#' || firstsign(buffer)==0); //comments and free lines
	if (strcmp_firstsign("[head]",buffer)==0)
		printf("--- Found head, loading head...\n");
	else
	{
		printf("(%i)Fatal Error: Cannot find head...\n",levelerrorline);
		SDL_RWclose(file);
		return NULL;
	}
	//Reading head until another [Statement] is detected
	plevel level = (plevel)malloc(sizeof(tlevel));
	level->firstenemy=NULL;
	level->width=0;
	level->height=0;
	level->startx=0;
	level->starty=0;
	level->havetokill=0;
	level->backgroundcolor=0;
	sprintf(level->failback,"./levels/menu.slvl");
//	level->startzoom=1<<SP_ACCURACY;
	while (firstsign(readnextline(file,buffer,1024))!='[')
	{
		if (firstsign(buffer)=='#' || firstsign(buffer)==0) //comments and free lines
			continue;
		char word[1024];
		char value[1024];
		int pos = getNextWord(0,buffer,word,1024,' ',' ');
				pos = getNextWord(pos,buffer,value,1024,' ',' ');
		if (strcmp_firstsign(value,"=")!=0) //there have to be a =
		{
			printf("(%i)Fatal Error: Expected \"=\" after %s\n",levelerrorline,word);
			free(level);
			SDL_RWclose(file);
			return NULL;
		}
		pos = getNextWord(pos,buffer,value,1024,' ','\n');
		if (strcmp_firstsign(word,"width")==0)
			level->width=atoi(value);
		if (strcmp_firstsign(word,"height")==0)
			level->height=atoi(value);
		if (strcmp_firstsign(word,"startx")==0)
			level->startx=atoi(value);
		if (strcmp_firstsign(word,"starty")==0)
			level->starty=atoi(value);
		if (strcmp_firstsign(word,"havetokill")==0)
			level->havetokill=atoi(value);
		if (strcmp_firstsign(word,"failback")==0)
			sprintf(level->failback,"%s",value);
/*	if (strcmp_firstsign(word,"startzoom")==0)
			level->startzoom=getFixedPoint(value);*/
		if (strcmp_firstsign(word,"backgroundcolor")==0)
		{
			char word[1024];
			int pos2 = getNextWord(0,value,word,1024,'(',',');
			int r = atoi(word);
			pos2 = getNextWord(pos2,value,word,1024,',',',');
			int g = atoi(word);
			pos2 = getNextWord(pos2,value,word,1024,',',',');
			int b = atoi(word);
			level->backgroundcolor=spGetRGB(r,g,b);
		}
	}
	printf("					width: %i\n",level->width);
	printf("				 height: %i\n",level->height);
	printf("				 startx: %i\n",level->startx);
	printf("				 starty: %i\n",level->starty);
	printf("backgroundcolor: %i\n",level->backgroundcolor);
	printf("		 havetokill: %i\n",level->havetokill);
	printf("			 failback: %s\n",level->failback);
//	printf("startzoom: %i+%i/%i\n",level->startzoom>>SP_ACCURACY,level->startzoom-((level->startzoom>>SP_ACCURACY)<<SP_ACCURACY),1<<SP_ACCURACY);
	psymbol lastsymbol=NULL;
	//symbols
	if (strcmp_firstsign("[symbols]",buffer)==0)
		printf("--- Found symbols, loading symbols...\n");
	else
	{
		printf("(%i)Fatal Error: Cannot find symbols...\n",levelerrorline);
		SDL_RWclose(file);
		return NULL;
	}
	//Reading symbols until another [Statement] is detected
	psymbol firstsymbol=NULL;
	while (firstsign(readnextline(file,buffer,1024))!='[')
	{
		if (firstsign(buffer)=='#' || firstsign(buffer)==0) //comments and free lines
			continue;
		//Reading Symbol
		if (firstsign(buffer)!='\'')
		{
			printf("(%i)Fatal Error: Expected \"\'\"\n",levelerrorline);
			while (firstsymbol!=NULL)
			{
				psymbol temp=firstsymbol->next;
				free(firstsymbol);
				firstsymbol=temp;
			}
			free(level);
			SDL_RWclose(file);
			return NULL;
		}
		char word[1024];
		int pos = getNextWord(0,buffer,word,1024,' ',' ');
		if (strlen(word)!=3 || word[2]!='\'')
		{
			printf("(%i)Fatal Error: Expected \'X\', where X is a sign\n",levelerrorline);
			while (firstsymbol!=NULL)
			{
				psymbol temp=firstsymbol->next;
				free(firstsymbol);
				firstsymbol=temp;
			}
			free(level);
			SDL_RWclose(file);
			return NULL;
		}
		psymbol newsymbol=(psymbol)malloc(sizeof(tsymbol));
		newsymbol->next=NULL;
		if (lastsymbol==NULL)
			firstsymbol=newsymbol;
		else
			lastsymbol->next=newsymbol;
		lastsymbol=newsymbol;
		newsymbol->symbol=word[1];
		printf("New Symbol: %c\n",newsymbol->symbol);
		
		newsymbol->meshmask=0;
		//Reading Objectfile
		pos = getNextWord(pos,buffer,word,1024,' ',' ');
		sprintf(newsymbol->objectfile,"%s",word);
		
		//Reading Color
		pos = getNextWord(pos,buffer,word,1024,'(',')');
		char value[1024];
		int pos2 = getNextWord(0,word,value,1024,' ',',');
		int r = atoi(value);
				pos2 = getNextWord(pos2,word,value,1024,',',',');
		int g = atoi(value);
				pos2 = getNextWord(pos2,word,value,1024,',',0);
		int b = atoi(value);
		newsymbol->color=spGetFastRGB(r,g,b);
		printf("	Color: r:%i g:%i b:%i\n",r,g,b);
		
		int object_is_door = 0;
		//Loading object file with the color
		if (strcmp("none",newsymbol->objectfile)==0)
		{
			printf("	No Objectfille\n");
			newsymbol->mesh=NULL;
		}
		else
		if (strcmp("badcoin",newsymbol->objectfile)==0)
		{
			printf("\tNo Objectfille\n");
			newsymbol->mesh=NULL;
			newsymbol->meshmask|=1;
		}
		else
		if (strstr(newsymbol->objectfile,"enemy")==newsymbol->objectfile)
		{
			printf("\tEnemy with sprite\n");
			newsymbol->mesh=NULL;
			newsymbol->enemy_kind = newsymbol->objectfile[5]-'0';
			newsymbol->meshmask|=16;
		}
		else
		if (strcmp("text",newsymbol->objectfile)==0)
		{
			printf("\tNo Objectfille\n");
			newsymbol->mesh=NULL;
			newsymbol->meshmask|=2;
		}
		else
		if (strcmp("./data/door.obj",newsymbol->objectfile)==0 || strcmp("door",newsymbol->objectfile)==0)
		{
			printf("	No Objectfille\n");
			newsymbol->mesh=NULL;
			object_is_door = 1;
		}
		else
		{
			printf("	Loading Mesh %s...",newsymbol->objectfile);
			if (strcmp(newsymbol->objectfile,"./data/broom.obj") == 0)
				newsymbol->mesh=spMeshLoadObj(newsymbol->objectfile,spLoadSurface("./data/broom.png"),spGetRGB(255,255,255));
			else
				newsymbol->mesh=spMeshLoadObj(newsymbol->objectfile,NULL,newsymbol->color);
			printf(" done\n");
		}

		//Reading Form
		newsymbol->form=0;
		pos = getNextWord(pos,buffer,word,1024,')',' ');
		if (strcmp_firstsign(word,"quad")==0)
			newsymbol->form=1;
		printf("	Form: %i (%s)\n",newsymbol->form,word);

		//Reading Measures
		pos = getNextWord(pos,buffer,word,1024,'(',')');
				pos2 = getNextWord(0,word,value,1024,' ',',');
		newsymbol->measures[0] = spAtof(value);
				pos2 = getNextWord(pos2,word,value,1024,',',',');
		newsymbol->measures[1] = spAtof(value);
				pos2 = getNextWord(pos2,word,value,1024,',',',');
		newsymbol->measures[2] = spAtof(value);
				pos2 = getNextWord(pos2,word,value,1024,',',0);
		newsymbol->measures[3] = spAtof(value);
		printf("	Measures: (%i+%i/%i, %i+%i/%i, %i+%i/%i, %i+%i/%i)\n",newsymbol->measures[0]>>SP_ACCURACY,newsymbol->measures[0]-((newsymbol->measures[0]>>SP_ACCURACY)<<SP_ACCURACY),1<<SP_ACCURACY
																																	 ,newsymbol->measures[1]>>SP_ACCURACY,newsymbol->measures[1]-((newsymbol->measures[1]>>SP_ACCURACY)<<SP_ACCURACY),1<<SP_ACCURACY
																																	 ,newsymbol->measures[2]>>SP_ACCURACY,newsymbol->measures[2]-((newsymbol->measures[2]>>SP_ACCURACY)<<SP_ACCURACY),1<<SP_ACCURACY
																																	 ,newsymbol->measures[3]>>SP_ACCURACY,newsymbol->measures[3]-((newsymbol->measures[3]>>SP_ACCURACY)<<SP_ACCURACY),1<<SP_ACCURACY);
		//Reading function
		if ((newsymbol->meshmask & 2)==2)
		{
			newsymbol->functionmask=0;
			pos = getNextWord(pos,buffer,word,1024,')','"');
			pos = getNextWord(pos,buffer,word,1024,'"','"');
			sprintf(newsymbol->function,"%s",word);
			printf("	Text: %s\n",newsymbol->function);
		}
		else
		{
			newsymbol->functionmask=0;
			pos = getNextWord(pos,buffer,word,1024,')',' ');
			sprintf(newsymbol->function,"%s",word);
			printf("	Function: %s\n",newsymbol->function);
			if (strcmp(newsymbol->function,"snow")==0)
				newsymbol->functionmask|=1;
			char* meow=strstr(newsymbol->function,"load");
			if (meow)
			{
				newsymbol->functionmask|=2;
				char newlevel[256];				
				int pos=4;
				if (newsymbol->function[pos]=='r')
				{
					newsymbol->functionmask|=32; 
					pos++;
				} 
				int pos2=getNextWord(0,&(meow[pos]),newlevel,256,'(',')');
				sprintf(newsymbol->function,"%s",newlevel);
				pos2++;
				if (getNextWord(pos2,&(meow[pos]),newlevel,256,',',' ')!=pos2)
				{
					newsymbol->needed_level=atoi(newlevel);
				}
				else
				newsymbol->needed_level=0;
			}
			if (strcmp(newsymbol->function,"left")==0)
				newsymbol->functionmask|=4;
			if (strcmp(newsymbol->function,"right")==0)
				newsymbol->functionmask|=8;
			if (strcmp(newsymbol->function,"broom")==0)
				newsymbol->functionmask|=16;
			if (strcmp(newsymbol->function,"quit")==0)
			{
				newsymbol->functionmask|=66;
				newsymbol->needed_level=0;
			}

			//enemies
			if (strstr(newsymbol->function,"waywalker")!=NULL)
				newsymbol->functionmask|=256;
			if (strstr(newsymbol->function,"standing")!=NULL)
				newsymbol->functionmask|=512;
		}
		if (object_is_door)
		{
			if (strstr(newsymbol->function,"_4") == NULL)
				newsymbol->meshmask|=4;
			else
				newsymbol->meshmask|=8;
		}
	}
	//Creating Symbollist
	int i;
	for (i=0;i<256;i++)
		level->symbollist[i]=NULL;
	while (firstsymbol!=NULL)
	{
		int i = firstsymbol->symbol;
		if (level->symbollist[i]==NULL)
		{
			level->symbollist[i]=firstsymbol;
			firstsymbol=firstsymbol->next;
			level->symbollist[i]->next=NULL;
		}
		else
		{
			psymbol lastsymbol=level->symbollist[i];
			while (lastsymbol->next!=NULL)
				lastsymbol=lastsymbol->next;
			lastsymbol->next=firstsymbol;
			firstsymbol=firstsymbol->next;
			lastsymbol->next->next=NULL;
		}
	}
	printf("Avaible symbol trees:\n");
	for (i=0;i<256;i++)
	{
		if (level->symbollist[i]==NULL)
			continue;
		psymbol now=level->symbollist[i];
		printf("	%c: ",now->symbol);
		while (now!=NULL)
		{
			printf("(%s) ",now->objectfile);
			if (now->next!=NULL)
				printf("-> ");
			now=now->next;
		}
		printf("\n");
	}
	printf("Allocating memory for the three layers...");
	for (i=0;i<3;i++)
	{
		level->layer[i]=(char*)malloc(sizeof(char)*level->width*level->height);
		memset(level->layer[i],' ',sizeof(char)*level->width*level->height);
	}
	printf(" done\n");

	while (levelreadbytes)
	{
		//Reading layerX until another [Statement] is detected
		int layernr=0;
		if (strcmp_firstsign("[layer0]",buffer)==0 || strcmp_firstsign("[layer1]",buffer)==0 || strcmp_firstsign("[layer2]",buffer)==0)
		{
			layernr=buffer[6]-'0';
			printf("--- Found layer %i, loading layer %i...\n",layernr,layernr);
		}
		else
		{
			printf("Finished loading.\n",levelerrorline,buffer);
			break;
		}
		int linenr=0;
		while (firstsign(readnextline(file,buffer,1024))!='[')
		{
			if (firstsign(buffer)=='#' || firstsign(buffer)==0) //comments and free lines
				continue;
			//Reading line
			if (firstsign(buffer)!='\'')
			{
				printf("(%i)Fatal Error: Expected \"\'\"\n",levelerrorline);
				freeLevel(level);
				SDL_RWclose(file);
				return NULL;
			}
			char word[8192];
			int pos = getNextWord(0,buffer,word,1024,' ',0);
			if (strlen(word)!=2+level->width || word[strlen(word)-1]!='\'')
			{
				printf("(%i)Fatal Error: Expected \'XXX\', where XXX is a layer line\n",levelerrorline);
				freeLevel(level);
				SDL_RWclose(file);
				return NULL;
			}
			for (i=0;i<level->width;i++)
				level->layer[layernr][linenr*level->width+i]=word[i+1];
			linenr++;
		}
		if (linenr!=level->height)
		{
			printf("(%i)Fatal Error: Layer has to much or too few lines\n",levelerrorline);
			freeLevel(level);
			SDL_RWclose(file);
			return NULL;
		}
		printf("Layer %i:\n",layernr);
		for (linenr=0;linenr<3 && linenr<level->height;linenr++)
		{ 
			for (i=0;i<3 && i<level->width;i++)
				printf("%c",level->layer[layernr][linenr*level->width+i]);
			printf(" ... ");
			for (i=level->width-3;i<level->width;i++)
				printf("%c",level->layer[layernr][linenr*level->width+i]);
			printf("\n");
		}
		printf("...		 ...\n");
		for (linenr=level->height-3;linenr<level->height;linenr++)
		{ 
			for (i=0;i<3 && i<level->width;i++)
				printf("%c",level->layer[layernr][linenr*level->width+i]);
			printf(" ... ");
			for (i=level->width-3;i<level->width;i++)
				printf("%c",level->layer[layernr][linenr*level->width+i]);
			printf("\n");
		}
	}		
	
	//Searching Enemies, Cutting from the Layer1 and putting in the enemy-List
	level->enemycount=0;
	penemy lastenemy=NULL;
	for (i=0;i<level->width*level->height;i++)
		if (level->symbollist[level->layer[1][i]]!=NULL && level->symbollist[level->layer[1][i]]->functionmask>255)
		{
			printf("Found Enemy \"%c\" at position %i:%i\n",level->symbollist[level->layer[1][i]]->symbol,i%level->width,i/level->width);
			level->enemycount++;
			penemy newenemy = (penemy)malloc(sizeof(tenemy));
			newenemy->next = NULL;
			newenemy->health = 1;
			newenemy->maxhealth=newenemy->health;
			newenemy->weapon = 0;
			newenemy->shotfq = 1000;
			newenemy->lastshot = 0;
			//Parsing symbol->function
			if (lastenemy==NULL)
				level->firstenemy=newenemy;
			else
				lastenemy->next=newenemy;
			newenemy->symbol=level->symbollist[level->layer[1][i]];
			char word[256];
			int pos;		
			for (pos=getNextWord(0,newenemy->symbol->function,word,256,',',',');strlen(word)>0;pos = getNextWord(pos,newenemy->symbol->function,word,256,',',','))
			{
				char value[256];
				int pos2 = getNextWord(0,word,value,256,'=','=');
				if (strcmp(value,"health")==0)
				{
					pos2 = getNextWord(pos2,word,value,256,'=','=');
					newenemy->health=atoi(value);
					newenemy->maxhealth=newenemy->health;
					printf("Setting health to %i (%s)\n",newenemy->health,value);
				}				
				if (strcmp(value,"weapon")==0)
				{
					pos2 = getNextWord(pos2,word,value,256,'=','=');
					newenemy->weapon=atoi(value);
					printf("Setting weapon to %i (%s)\n",newenemy->weapon,value);
				}				
				if (strcmp(value,"shotfq")==0)
				{
					pos2 = getNextWord(pos2,word,value,256,'=','=');
					newenemy->shotfq=atoi(value);
					printf("Setting shot frequency to %i (%s)\n",newenemy->shotfq,value);
				}				
			}
			newenemy->dx = (rand()%2 ? -1:1)<<(SP_ACCURACY-7);
			newenemy-> x = (i%level->width	)<<(SP_ACCURACY+1);
			newenemy-> y = ((i/level->width*2+1)<<(SP_ACCURACY))-newenemy->symbol->measures[3];
			level->layer[1][i]=' ';
			lastenemy=newenemy;
		}
	SDL_RWclose(file);
	return level;
}
