#include <stdio.h>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include "network.h"
#include "X.h"
#include "curses.h"
#undef getch
#include <conio.h>
#include <string.h>
#include <string>
#include <deque>
//x = 85
//y = 27
#define WORLD_X 85
#define WORLD_Y 27
#define BUG_MIN 20

char world[WORLD_Y][WORLD_X];

vector<gridbug> bugs;

int bugviewmode = 0;

int starvingThreshold = 50;
int foodNeededToReplicate = 20;
int offspringNum = 3;

int cursorX = WORLD_X/2;
int cursorY = WORLD_Y/2;

int timestep = 0;
int cbi = -1;

struct spc{
	string name;
	int color = 0;
};


vector<spc> usedColors;

#define EVENTBUFSIZE 5

struct event{
	int eventTime = 0;
	string eventDescription;
};

deque<event> eventlist;

void newEvent(const char* eventDescription)
{
	event e;
	
	e.eventDescription = eventDescription;
	e.eventTime = timestep;
	
	if(eventlist.size() > EVENTBUFSIZE)
	{
		eventlist.pop_back();
	}
	eventlist.push_front(e);
}

struct species{
	int num = 0;
	string name;
	int birthdate = 0;
	int extinctiondate = 0;
	int survivedfor = 0;
	bool extinct = false;
	
	int colorID = 0;
	
};

vector<species> pops;


		


void display()
{
	
	int generation = -1;
	int eaten = -1;
	int hunger = -1;
	int neurons = -1;
	int synapses = -1;
	int timeAlive = -1;
	int activeNeurons = 0;
	string sID;
	int currentColorPair = 0;
	if(bugviewmode == 0)
	{
		for(int i = 0; i<WORLD_Y; ++i)
		{
			for(int x = 0; x<WORLD_X; ++x)
			{
				if(world[i][x] == '@')
				{
					for(int z = 0; z<bugs.size(); ++z)
					{
						if(bugs[z].gridbugY == i && bugs[z].gridbugX == x)
						{
							currentColorPair = bugs[z].colorID;
							break;
						}
					}
					attron(COLOR_PAIR(currentColorPair));
					printw("%c",world[i][x]);
					attroff(COLOR_PAIR(currentColorPair));
				}else{
					printw("%c",world[i][x]);
				}
				
			
				
			}
			printw("\n");
		}
		
		printw("\nPress TAB to toggle between viewmodes.\nEvent log:\n");
		
		for(int i = 0; i<eventlist.size(); ++i)
		{
			
			printw("%d - %s\n",eventlist[i].eventTime,eventlist[i].eventDescription.c_str());
		}
		
	}else if(bugviewmode == 1){
		for(int i = 0; i<WORLD_Y; ++i)
		{
			for(int x = 0; x<WORLD_X; ++x)
			{
				if(i == cursorY && x == cursorX)
				{
					printw("X");
				}else if(world[i][x] == '@')
				{
					for(int z = 0; z<bugs.size(); ++z)
					{
						if(bugs[z].gridbugY == i && bugs[z].gridbugX == x)
						{
							currentColorPair = bugs[z].colorID;
							break;
						}
					}
					attron(COLOR_PAIR(currentColorPair));
					printw("%c",world[i][x]);
					attroff(COLOR_PAIR(currentColorPair));
				}else{
					printw("%c",world[i][x]);
				}
			}
			printw("\n");
		}
		
		printw("\nCurrent viewmode: detailed\nUse arrow keys to move cursor. Move the cursor over a bug to view its info.\n");
		
		
		for(int i = 0; i<bugs.size(); ++i)
		{
			if(bugs[i].gridbugY == cursorY && bugs[i].gridbugX == cursorX)
			{
				cbi = i;
				neurons = bugs[i].brain.neurons.size();
				eaten = bugs[i].eaten;
				hunger = bugs[i].hunger;
				timeAlive = bugs[i].timeAlive;
				generation = bugs[i].generation;
				sID = bugs[i].speciesID;
				
				for(int x = 0; x<bugs[i].brain.neurons.size(); ++x)
				{
					if(bugs[i].brain.neurons[x]->used)
					++activeNeurons; 
				}
				
			}
		}
		
		
		
		printw("\nNeurons: ");
		if(neurons == -1)
		{
			printw("--");
		}else{
			
			printw("%d, (%d active)",neurons,activeNeurons);
			activeNeurons = 0;
		}
		
		printw("\tTime alive: ");
		if(timeAlive == -1)
		{
			printw("--");
		}else{
			printw("%d",timeAlive);
		}
		
		
		printw("\nFood Eaten: ");
		if(eaten == -1)
		{
			printw("--");
		}else{
			printw("%d",eaten);
		}
		
		printw("\tHunger: ");
		if(hunger == -1)
		{
			printw("--");
		}else{
			printw("%d",hunger);
		}
		
		printw("\nGeneration: ");
		if(generation == -1)
		{
			printw("--");
		}else{
			printw("%d",generation);
		}
		
		printw("\tSpecies: ");
		if(sID == "NULL")
		{
			printw("--");
		}else{
			printw("%s",sID.c_str());
		}
		
		
		

	}else if(bugviewmode == 2)
	{
		printw("\n\n\n\tCurrent viewmode: Statistics\n\n");
		
		
		/*vector<species> s;
		bool found = false;
		for(int i = 0; i<bugs.size(); ++i)
		{
			redd:
			found = false;
			for(int x = 0; x<s.size(); ++x)
			{
				if(s[x].name == bugs[i].speciesID)
				{
					found = true;
					++s[x].num;
					break;
				}
			}
			
			if(!found)
			{
				species h;
				h.name = bugs[i].speciesID;
				s.push_back(h);
				goto redd;
			}
		}*/
		int cpi = 0;
		for(int i = 0; i<pops.size(); ++i)
		{
			attron(COLOR_PAIR(pops[i].colorID));
			printw("@");
			attroff(COLOR_PAIR(pops[i].colorID));
			printw(" - Species: %s Census: %d\n",pops[i].name.c_str(),pops[i].num);
		}		

		
	}
	
	if(bugviewmode == 1)
	printw("\n\nPress e to export DNA of selected bug to brain.txt\nPress l to create a new bug using brain.txt\n");
	
	printw("\n\nCycles Elapsed: %d bugs alive: %d\n",timestep,bugs.size());
	
	
	
	
	refresh();
}

void initmap()
{
	for(int i = 0; i<WORLD_Y; ++i)
	{
		for(int x = 0; x<WORLD_X; ++x)
		{
			world[i][x] = '#';
		}
	}
	int t,tx,ty;
	
	int n = t = tx = ty = 0;
	
	
	for(int x = 0; x<300; ++x)
	{
		tx = rand()%WORLD_X;
		ty = rand()%WORLD_Y;
		
		world[ty][tx] = ' ';
		n = rand()%30;
		for(int i = 0; i<n; ++i)
		{
			t = rand()%4;
			switch(t)
			{
				case 0:
					if(ty > 0)
					{
						world[--ty][tx] = ' ';
					}
				break;
				
				case 1:
					if(ty < WORLD_Y-1)
					{
						world[++ty][tx] = ' ';
					}
				break;
				
				case 2:
					if(tx > 0)
					{
						world[ty][--tx] = ' ';
					}
				break;
				
				case 3:
					if(tx < WORLD_X-1)
					{
						world[ty][++tx] = ' ';
					}
				break;
			}
		}
	
	}
	
	init_pair(1,COLOR_YELLOW,COLOR_BLACK);
	init_pair(2,COLOR_GREEN,COLOR_BLACK);
	init_pair(3,COLOR_CYAN,COLOR_BLACK);
	init_pair(4,COLOR_WHITE,COLOR_BLACK);
	init_pair(5,COLOR_RED,COLOR_BLACK);
	init_pair(6,COLOR_BLACK,COLOR_WHITE);
	init_pair(7,COLOR_YELLOW,COLOR_GREEN);
	init_pair(8,COLOR_RED,COLOR_YELLOW);
	init_pair(9,COLOR_BLACK,COLOR_GREEN);
	init_pair(10,COLOR_WHITE,COLOR_YELLOW);
	init_pair(11,COLOR_CYAN,COLOR_RED);
	init_pair(12,COLOR_YELLOW,COLOR_RED);
	init_pair(13,COLOR_WHITE,COLOR_RED);
	init_pair(14,COLOR_BLACK,COLOR_CYAN);
	init_pair(15,COLOR_YELLOW,COLOR_CYAN);
	init_pair(16,COLOR_WHITE,COLOR_CYAN);
	init_pair(17,COLOR_BLACK,COLOR_RED);
	init_pair(18,COLOR_RED,COLOR_WHITE);
	init_pair(19,COLOR_GREEN,COLOR_WHITE);
	init_pair(20,COLOR_BLACK,COLOR_YELLOW);
	init_pair(21,COLOR_MAGENTA,COLOR_BLACK);
	
}

void spawnfood()
{
	for(int i = 0; i<WORLD_Y; ++i)
	{
		for(int x = 0; x<WORLD_X; ++x)
		{
			if(rand()%300 == 1 && world[i][x] == ' ')
			{
				world[i][x] = ',';	
			}
		}
	}
	
}


void spawnBugs()
{
		int bu = BUG_MIN - bugs.size();
		
		int ry = rand()%WORLD_Y;
		int rx = rand()%WORLD_X;
		while(bugs.size() < BUG_MIN)
		{
			ry = rand()%WORLD_Y;
			rx = rand()%WORLD_X;
			
			if(world[ry][rx] == ' ')
			{
				gridbug g = newBug(rand()%50 + 12,12,4,0);
				
				g.gridbugX = rx;
				g.gridbugY = ry;
				
				g.speciesID = to_string(timestep) + "_" + to_string(g.brain.neurons.size()) + "_" + to_string(rand()%1000 + 1);
				species s;
				//s.num = 1;
				s.name = g.speciesID;
				s.birthdate = timestep;
						
				int mcnu = 0;
				
				choosecolor:
								
				mcnu = rand() % 21 + 1;
				for(int k = 0; k<pops.size(); ++k)
				{
					if(pops[k].colorID == mcnu)
					{
						goto choosecolor;
					}
				}	
				
				s.colorID = mcnu;
				pops.push_back(s);
				g.colorID = mcnu;				
				string ts = "Created a new species: " + g.speciesID;
				newEvent(ts.c_str());
				bugs.push_back(g);
			}
			
		}
}

void tallySpecies()
{
//	vector<species> s;

	for(int i = 0; i<pops.size(); ++i)
	pops[i].num = 0;
	
	for(int i = 0; i<bugs.size(); ++i)
	{
		rdo:
		bool found = false;
		for(int x = 0; x<pops.size(); ++x)
		{
			if(bugs[i].speciesID == pops[x].name)
			{
				found = true;
				//pops[x].colorID = bugs[i].colorID;
				++pops[x].num;
				break;
			}
		}
		
		/*if(!found)
		{
		//	exit(0);
			species k;
			k.name = bugs[i].speciesID;
			pops.push_back(k);
			goto rdo;
		}*/
	}
	
/*	vector<int> ages;
	for(int i = 0; i<pops.size(); ++i)
	{
		ages.push_back(pops[i].birthdate);
	}
	vector<species> ts = pops;
	pops.clear();
	
	for(int i = 0; i<s.size(); ++i)
	{
		pops.push_back(s[i]);
	}*/
	
}

void updateBugs()
{
	
	recheck:
		
	bool dar = false;	
	
	int g = bugs.size();
	string ds;	
	for(int i = 0; i<g; ++i)
	{
		if(bugs[i].hunger > starvingThreshold)
		{
			world[bugs[i].gridbugY][bugs[i].gridbugX] = ' ';
			networkDelete(bugs[i].brain);
			ds = bugs[i].speciesID;
			
			/*bool f1ound = false;
			for(int x = 0; x<bugs.size(); ++x)
			{
				if(bugs[x].speciesID == ds)
				{
					f1ound = true;
					break;
				}
			}
			
			if(!f1ound)
			{
				for(int x = 0; x<pops.size(); ++x)
				{
					if(pops[x].name == ds)
					{
						pops[x].num = 0;
						break;
					}
				}
			}*/
			
			bugs.erase(bugs.begin()+i);
			
			goto recheck;
		}
	}
	
	
	tallySpecies();
	
	
	int c = pops.size();
	
	
	
	vector<gridbug> offspring;
	
	int rx = 0;
	int ry = 0;	
	
	for(int i = 0; i<bugs.size(); ++i)
	{
		if(bugs[i].eaten > foodNeededToReplicate)
		{
			offspring = mutateBug(bugs[i],offspringNum);
			for(int x = 0; x<offspring.size(); ++x)
			{
				d1:
				dar = true;
				rx = rand()%WORLD_X;
				ry = rand()%WORLD_Y;
				
				if(world[ry][rx] != '#')
				{
					offspring[x].gridbugX = rx;
					offspring[x].gridbugY = ry;
					offspring[x].speciesID = bugs[i].speciesID;
					offspring[x].colorID = bugs[i].colorID;
					
				}else{
					goto d1;
				}	
				
			}
			
			for(int x = 0; x<offspring.size(); ++x)
			{
				bugs.push_back(offspring[x]);
			}
			
			bugs[i].eaten = -1;
			bugs[i].hunger = starvingThreshold+9999;
			
		}	
	}
	
	for(int i = 0; i<pops.size(); ++i)
	{
		if(pops[i].num < 1)
		{
			dar = true;
			pops[i].extinctiondate = timestep;
			pops[i].survivedfor = pops[i].extinctiondate - pops[i].birthdate;
			string s = "Species " + pops[i].name + " has gone extinct after " + to_string(pops[i].survivedfor) + " cycles.";
			pops[i].extinct = true;
			newEvent(s.c_str());
			
		}
	}
	
	eraseagain:
	for(int i = 0; i<pops.size(); ++i)
	{
		if(pops[i].extinct)
		{
			pops.erase(pops.begin() + i);
			goto eraseagain;
		}
	}
	
	
	
	for(int i = 0; i<bugs.size(); ++i)
	{
		
		++bugs[i].hunger;
		++bugs[i].timeAlive;
		
		if(world[bugs[i].gridbugY-1][bugs[i].gridbugX] == '#')
		{
			bugs[i].brain.neurons[0]->activated = bugs[i].brain.neurons[0]->activation = true;
		}else{
			bugs[i].brain.neurons[0]->activated = bugs[i].brain.neurons[0]->activation = false;
		}
		
		if(world[bugs[i].gridbugY][bugs[i].gridbugX+1] == '#')
		{
			bugs[i].brain.neurons[1]->activated = bugs[i].brain.neurons[1]->activation = true;
		}else{
			bugs[i].brain.neurons[1]->activated = bugs[i].brain.neurons[1]->activation = false;
		}
		
		if(world[bugs[i].gridbugY+1][bugs[i].gridbugX] == '#')
		{
			bugs[i].brain.neurons[2]->activated = bugs[i].brain.neurons[2]->activation = true;
		}else{
			bugs[i].brain.neurons[2]->activated = bugs[i].brain.neurons[2]->activation = false;
		}
		
		if(world[bugs[i].gridbugY][bugs[i].gridbugX-1] == '#')
		{
			bugs[i].brain.neurons[3]->activated = bugs[i].brain.neurons[3]->activation = true;
		}else{
			bugs[i].brain.neurons[3]->activated = bugs[i].brain.neurons[3]->activation = false;
		}
		
		if(world[bugs[i].gridbugY-1][bugs[i].gridbugX] == ',')
		{
			bugs[i].brain.neurons[4]->activated = bugs[i].brain.neurons[4]->activation = true;
		}else{
			bugs[i].brain.neurons[4]->activated = bugs[i].brain.neurons[4]->activation = false;
		}
		
		if(world[bugs[i].gridbugY][bugs[i].gridbugX+1] == ',')
		{
			bugs[i].brain.neurons[5]->activated = bugs[i].brain.neurons[5]->activation = true;
		}else{
			bugs[i].brain.neurons[5]->activated = bugs[i].brain.neurons[5]->activation = false;
		}
		
		if(world[bugs[i].gridbugY+1][bugs[i].gridbugX] == ',')
		{
			bugs[i].brain.neurons[6]->activated = bugs[i].brain.neurons[6]->activation = true;
		}else{
			bugs[i].brain.neurons[6]->activated = bugs[i].brain.neurons[6]->activation = false;
		}
		
		if(world[bugs[i].gridbugY][bugs[i].gridbugX-1] == ',')
		{
			bugs[i].brain.neurons[7]->activated = bugs[i].brain.neurons[7]->activation = true;
		}else{
			bugs[i].brain.neurons[7]->activated = bugs[i].brain.neurons[7]->activation = false;
		}
		
		if(world[bugs[i].gridbugY-1][bugs[i].gridbugX] == ' ')
		{
			bugs[i].brain.neurons[8]->activated = bugs[i].brain.neurons[8]->activation = true;
		}else{
			bugs[i].brain.neurons[8]->activated = bugs[i].brain.neurons[8]->activation = false;
		}
		
		if(world[bugs[i].gridbugY][bugs[i].gridbugX+1] == ' ')
		{
			bugs[i].brain.neurons[9]->activated = bugs[i].brain.neurons[9]->activation = true;
		}else{
			bugs[i].brain.neurons[9]->activated = bugs[i].brain.neurons[9]->activation = false;
		}
		
		if(world[bugs[i].gridbugY+1][bugs[i].gridbugX] == ' ')
		{
			bugs[i].brain.neurons[10]->activated = bugs[i].brain.neurons[10]->activation = true;
		}else{
			bugs[i].brain.neurons[10]->activated = bugs[i].brain.neurons[10]->activation = false;
		}
		
		if(world[bugs[i].gridbugY][bugs[i].gridbugX-1] == ' ')
		{
			bugs[i].brain.neurons[11]->activated = bugs[i].brain.neurons[11]->activation = true;
		}else{
			bugs[i].brain.neurons[11]->activated = bugs[i].brain.neurons[11]->activation = false;
		}
		
		process(bugs[i].brain);
		

		world[bugs[i].gridbugY][bugs[i].gridbugX] = ' ';
			
		if(bugs[i].brain.outputNeurons[0]->activated)
		{
			if(bugs[i].gridbugY > 0 && world[bugs[i].gridbugY-1][bugs[i].gridbugX] != '#')
			bugs[i].gridbugY--;
		}
		
		
		if(bugs[i].brain.outputNeurons[1]->activated)
		{
			if(bugs[i].gridbugX < WORLD_X-1 && world[bugs[i].gridbugY][bugs[i].gridbugX+1] != '#')
			bugs[i].gridbugX++;
		}
		if(bugs[i].brain.outputNeurons[2]->activated)
		{
			if(bugs[i].gridbugY < WORLD_Y-1 && world[bugs[i].gridbugY+1][bugs[i].gridbugX] != '#')
			bugs[i].gridbugY++;
		}
		if(bugs[i].brain.outputNeurons[3]->activated)
		{
			if(bugs[i].gridbugX > 0 && world[bugs[i].gridbugY][bugs[i].gridbugX-1] != '#')
			bugs[i].gridbugX--;
		}
	
		if(world[bugs[i].gridbugY][bugs[i].gridbugX] == ',')
		{
			++bugs[i].eaten;
			bugs[i].hunger -= 20;
			if(bugs[i].hunger < 0)
			bugs[i].hunger = 0;
		}
		world[bugs[i].gridbugY][bugs[i].gridbugX] = '@';
		
		
	}
	
}



int cyclestorunbeforemanual = 10000;
int gv = 0;
int main()
{
	srand(time(0));
	initscr();
	start_color();
	initmap();
	resize_term(40,86);
	
	printw("LOADING...");
	refresh();
	
	for(;;)
	{
		if(!bugviewmode)
		{
			
			tallySpecies();
			updateBugs();
			spawnBugs();
			spawnfood();
			++timestep;
		}
		
		
		clear();
		display();
		
		
		gv = getch();
		
		
		switch(gv)
		{
			case 9:
				++bugviewmode;
				if(bugviewmode == 3)
				bugviewmode = 0;
			break;
			
			case 72://UP
				if(bugviewmode)
				if(cursorY > 0)
				--cursorY;
			break;
			
			case 77://RIGHT
				if(bugviewmode)
				if(cursorX < WORLD_X-1)
				++cursorX;
			break;
			
			case 80://DOWN
				if(bugviewmode)	
				if(cursorY < WORLD_Y-1)
				++cursorY;
			break;
			
			case 75://LEFT
				if(bugviewmode)
				if(cursorX > 0)
				--cursorX;
			break;
			
			case 101://e E
			case 69:
				if(cbi != -1)
				{
					saveNetwork(bugs[cbi].brain,"brain.txt");
				}
			break;
			
			case 108://l L
			case 76:
				gridbug g = newBug(loadNetwork("brain.txt"));
				g.gridbugX = cursorX;
				g.gridbugY = cursorY;
				bugs.push_back(g);
			break;
		}
	}
	
	
	
	endwin();
	
}
