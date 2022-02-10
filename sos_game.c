#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<ctype.h>
#include<ncurses.h>

#define MAX_PLAYERS 10
//Maximum players in a game
#define MAX_WIDTH 14
#define MAX_HEIGHT 14
//Maximum width and height of board. i.e. Max number of columns and rows 
#define MIN_WIDTH 4
#define MIN_HEIGHT 4
//Minimum width and height of board. i.e. Min number of columns and rows
#define VIEW_WIDTH 80 
//width in terms of number of characters to optimize output for

typedef struct{												//Declaring structure for players
	char name[30];											//name of player
	int score;												//score of player
}PLAYER;

PLAYER players[MAX_PLAYERS];								//array of players
int num_of_players;											//maintain count of number of players in a game
char grid[MAX_HEIGHT][MAX_WIDTH];							//matrix for board of game
int columns;												//maintain count of number of columns in a game
int rows;													//maintain count of number of rows in a game
int turn;													//this variable is used to keep track of whose turn it is

void start();												//setup wizard before starting a game,
															//to take number and names of players and board size
void reset();												//sets all grid positions to null character,
															//sets all players score to 0,
															//set turn to 0
void display();												//display name of players along with scores and grid
void print_n_times(char c,int n);							//a utility function to print given character n times
int game_is_not_over();										//checks if there is some place empty,
															//if a place is empty returns 1, if no place empty returns 0
void game();												//the loop in which gameplay happens
void game_move(int pos_i,int pos_j,char letter);					//puts the letter at its position,
															//check if scoring and increment score,
															//if not scored then increment turn

void results();												//compute and display results, also stores records in a file
int pause();												//pause menu, returns 0 if exit is selected, 1 otherwise
void help();												//displays help
void save_game();											//saves variables related to game in a file
void load_game();											//loads variables related to game from file
void statistics();											//display records from file

int generic_menu(WINDOW *win,const char* options[],int num_options);	//accept a list of options and number of options
																		//0th item in list is title of menu
																		//displays options and return number of option selected
																		//index is 1 based


int main()
{
	initscr();
	
	int invalid;
	int ch;
	
	do
	{
		const char *options[]={	"Menu",
							"Start New Game",
							"Continue Saved Game",
							"Help",
							"Records",
							"Exit"				};
		ch=generic_menu(stdscr,options,5);	
		
		switch(ch)
		{
			case 1:
				start();
				break;
			case 2:
				load_game();
				break;
			case 3:
				help();
				break;
			case 4:
				statistics();
				break;
			case 5:
				printw("Exiting...\n");
				refresh();
				break;
		}
	}while(ch!=5);
	
	endwin();
}

void start()
{
	clear();
	int invalid;											//variable used to check validity of input
	
	printw("Enter number of players\t(Min:2,Max:%d)\n",MAX_PLAYERS);								//input number of players
	refresh();
	do
	{
		scanw("%d",&num_of_players);
		invalid=(num_of_players<2 || num_of_players>MAX_PLAYERS);
		if(invalid)
		{
			printw("Number of players must be between 2 and %d\nTry again\n",MAX_PLAYERS);
			refresh();
		}
	}while(invalid);
	
	printw("Enter player names\t\t(It should be one word without spaces)\n");						//input player names
	refresh();
	for(int i=0;i<num_of_players;i++)
	{
		printw("Enter name of player %d:",i+1);
		refresh();
		scanw("%s",players[i].name);
	}
	
	printw("Enter number of rows\t(Min:%d,Max:%d)\n",MIN_HEIGHT,MAX_HEIGHT);						//input number of rows
	refresh();
	do
	{
		scanw("%d",&rows);
		invalid=(rows<MIN_HEIGHT || rows>MAX_HEIGHT);
		if(invalid)
		{
			printw("Number of rows must be between %d and %d\nTry again\n",MIN_HEIGHT,MAX_HEIGHT);
			refresh();
		}
	}while(invalid);
	
	printw("Enter number of columns\t(Min:%d,Max:%d)\n",MIN_WIDTH,MAX_WIDTH);						//input number of columns
	refresh();
	do
	{
		scanw("%d",&columns);
		invalid=(columns<MIN_WIDTH || columns>MAX_WIDTH);
		if(invalid)
		{
			printw("Number of columns must be between %d and %d\nTry again\n",MIN_WIDTH,MAX_WIDTH);
			refresh();
		}
	}while(invalid);
	
	reset();
	game();
}

void reset()
{
	for(int i=0;i<rows;i++)
		for(int j=0;j<columns;j++)
			grid[i][j]='\0';
	
	for(int i=0;i<num_of_players;i++)
		players[i].score=0;
	
	turn=0;
}

void print_n_times(char c,int n)
{
	for(int i=0;i<n;i++)
		addch(c);
}

void display()
{
	clear();
	
	printw("%-30s%s\n","Name of player","Score");						//display player names and scores
	for(int i=0;i<num_of_players;i++)
	{
		printw("%-30s%5d\n",players[i].name,players[i].score);
	}
	print_n_times('=',VIEW_WIDTH);
	
	int hor_pad=round((float)VIEW_WIDTH/(float)(columns+3));			//compute horizontal gap
	int ver_pad=round(hor_pad*0.47);									//compute vertical gap
	int count=1;														//count the position number
	
	print_n_times('\n',ver_pad);										//print newlines for top padding of grid
	printw("%*s",hor_pad,"+");											//print top border
	print_n_times('-',((columns+1)*hor_pad)-1);
	printw("+\n");
	for(int i=0;i<rows;i++)												//loop for printing grid
	{
		for(int k=0;k<ver_pad-1;k++)									//print newlines for vertical gap in table;
		{																//also print parts of side borders
			printw("%*s",hor_pad,"|");
			printw("%*s\n",((columns+1)*hor_pad),"|");
		}
		
		printw("%*s",hor_pad,"|");										//printing line containing row elements
		for(int j=0;j<columns;j++)
		{
			if(grid[i][j]=='\0')										//if element is null character print number in that place
				printw("%*d",hor_pad,count);							//else print the letter
			else
				printw("%*c",hor_pad,grid[i][j]);
			count++;
		}
		printw("%*s\n",hor_pad,"|");
	}
	for(int k=0;k<ver_pad-1;k++)										//print newlines for vertical gap after last row;
	{																	//also print parts of side borders
		printw("%*s",hor_pad,"|");
		printw("%*s\n",((columns+1)*hor_pad),"|");
	}
	printw("%*s",hor_pad,"+");											//print bottom border
	print_n_times('-',((columns+1)*hor_pad)-1);
	printw("+");
	print_n_times('\n',ver_pad);										//print newlines for bottom padding of grid
	print_n_times('=',VIEW_WIDTH);
	printw("\n");
	refresh();
}

int game_is_not_over()
{
	for(int i=0;i<rows;i++)
		for(int j=0;j<columns;j++)
			if(grid[i][j]=='\0')
				return 1;
	return 0;
}

void game()
{
	remove("saved_game");														//delete the file having any previously saved game
	
	int pause_returned=1;														//variable to check if we want to exit via pause
	char letter;																//to store input letter 
	int invalid;																//temprory variable to check if input is valid
	int pos_i,pos_j;															//variables to store position in matrix
	
	while(game_is_not_over() && pause_returned)									//game will run until there is empty place or not exited via pause
	{
		display();
		printw("Turn of %s\n",players[turn%num_of_players].name);
		
		do 																		//repeat until S,O or P is input
		{
			printw("Enter the letter:");
			refresh();
			scanw(" %c",&letter);
			letter=toupper(letter);
			invalid=(letter!='S' && letter!='O' && letter!='P');
			if(invalid)
			{
				printw("Invalid letter...Try again\n");
				refresh();
			}
				
		}while(invalid);
		
		if(letter=='P')															//paused
		{
			pause_returned=pause();
		}
		else 																	//next move
		{
			int position;
			do 																	//input position of move
			{
				printw("Enter the position number:");
				refresh();
				scanw("%d",&position);
				pos_i=(position-1)/columns;
				pos_j=(position-1)%columns;
				invalid=position<1 || position>(rows*columns) || grid[pos_i][pos_j]!='\0';		//invalid if either out of bounds or position is already occupied
				if(invalid)
				{
					printw("Invalid position...Try again\n");
					refresh();
				}
					
			}while(invalid);
			
			game_move(pos_i,pos_j,letter);
		}

	}
	
	if(pause_returned) 															//if exiting loop via game over pause_returned will be 1
	{																			//if loop exits via pause then no results are shown
		results();
		printw("\nPress Enter to continue...\n");								//hold on until user decides to continue
		refresh();
		scanw("%*c%*c");
	}															
	
}

void game_move(int pos_i,int pos_j,char letter)
{
	int scored=0;
	
	grid[pos_i][pos_j]=letter;
	
	if(letter=='S')														//for letter S
	{
		if(pos_j>1)
		{
			if(grid[pos_i][pos_j-2]=='S' && grid[pos_i][pos_j-1]=='O')
			{
				players[turn%num_of_players].score++;
				scored=1;
			}
		}
		if(pos_j>1 && pos_i>1)
		{
			if(grid[pos_i-2][pos_j-2]=='S' && grid[pos_i-1][pos_j-1]=='O')
			{
				players[turn%num_of_players].score++;
				scored=1;
			}
		}
		if(pos_i>1)
		{
			if(grid[pos_i-2][pos_j]=='S' && grid[pos_i-1][pos_j]=='O')
			{
				players[turn%num_of_players].score++;
				scored=1;
			}
		}
		if(pos_j<(columns-2) && pos_i>1)
		{
			if(grid[pos_i-2][pos_j+2]=='S' && grid[pos_i-1][pos_j+1]=='O')
			{
				players[turn%num_of_players].score++;
				scored=1;
			}
		}
		if(pos_j<(columns-2))
		{
			if(grid[pos_i][pos_j+2]=='S' && grid[pos_i][pos_j+1]=='O')
			{
				players[turn%num_of_players].score++;
				scored=1;
			}
		}
		if(pos_j<(columns-2) && pos_i<(rows-2))
		{
			if(grid[pos_i+2][pos_j+2]=='S' && grid[pos_i+1][pos_j+1]=='O')
			{
				players[turn%num_of_players].score++;
				scored=1;
			}
		}
		if(pos_i<(rows-2))
		{
			if(grid[pos_i+2][pos_j]=='S' && grid[pos_i+1][pos_j]=='O')
			{
				players[turn%num_of_players].score++;
				scored=1;
			}
		}
		if(pos_j>1 && pos_i<(rows-2))
		{
			if(grid[pos_i+2][pos_j-2]=='S' && grid[pos_i+1][pos_j-1]=='O')
			{
				players[turn%num_of_players].score++;
				scored=1;
			}
		}
	}
	else 																//for letter O
	{
		if(pos_i>0 && pos_i<(rows-1) && pos_j>0 && pos_j<(columns-1))
		{
			if(grid[pos_i-1][pos_j-1]=='S' && grid[pos_i+1][pos_j+1]=='S')
			{
				players[turn%num_of_players].score++;
				scored=1;
			}
			if(grid[pos_i+1][pos_j-1]=='S' && grid[pos_i-1][pos_j+1]=='S')
			{
				players[turn%num_of_players].score++;
				scored=1;
			}
		}
		if(pos_i>0 && pos_i<(rows-1))
		{
			if(grid[pos_i-1][pos_j]=='S' && grid[pos_i+1][pos_j]=='S')
			{
				players[turn%num_of_players].score++;
				scored=1;
			}
		}
		if(pos_j>0 && pos_j<(columns-1))
		{
			if(grid[pos_i][pos_j-1]=='S' && grid[pos_i][pos_j+1]=='S')
			{
				players[turn%num_of_players].score++;
				scored=1;
			}
		}
	}
	
	if(!scored)
		turn++;
}

void results()
{
	int max=players[0].score;													//assume first player has highest score
	int max_index=0;
	int count=1;																//maintains count of players having highest score to determine win or draw
	for(int i=1;i<num_of_players;i++)
	{
		if(players[i].score > max)												//if higher score is found set that to max
		{
			max=players[i].score;
			max_index=i;
			count=1;
		}
		else if(players[i].score == max)										//if another player having max score is found increment count
		{
			count++;
		}
	}
	
	FILE *fp=fopen("records.txt","a");											//open file for appending records
	fprintf(fp,"%d %d*%d ",num_of_players,rows,columns);						//write number of players, rows and columns to file
	
	display();
	if(count==1)																//there is only one winner
	{
		printw("\n%s won with %d score!!\n",players[max_index].name,max);		//display on screen
		fprintf(fp,"%s won with %d score!!\n",players[max_index].name,max);		//write to records
	}
	else 																		//draw
	{
		printw("\nIt is a draw between %s",players[max_index].name)	;			//display first player name
		fprintf(fp,"It is a draw between %s",players[max_index].name);			//write to records
		for(int i=max_index+1,j=1 ;j<count ;i++)
		{
			if(players[i].score==max)											//print names of players having high score
			{
				if(j==count-1)													//if last name put "and" else put ","
				{
					printw(" and ");
					fprintf(fp," and ");
				}													
				else
				{
					printw(", ");
					fprintf(fp,", ");
				}
				printw("%s",players[i].name);
				fprintf(fp,"%s",players[i].name);
				j++;
			}
		}
		printw(" with %d score\n",max);
		fprintf(fp," with %d score\n",max);
	}
	refresh();
	fclose(fp);
}

int pause()
{
	int invalid;
	int ch;
	char yes_or_no;
	
	clear();
	printw("\n\t\tGame paused\n\n");
	printw("\t1: Resume\n");
	printw("\t2: Restart\n");
	printw("\t3: Exit\n");
	printw("\nEnter your choice:");
	refresh();
	do
	{
		scanw("%d",&ch);
		invalid=(ch<1 || ch>3);
		if(invalid)
		{
			printf("Invalid option...Try again\n");
			refresh();			
		}
	}while(invalid);
	
	switch(ch)
	{
		case 2:
			reset();
		case 1:
			return 1;
		case 3:
			printw("Do you wish to save game for later?\n(y/N)");		//prompt for asking for saving to file
			refresh();
			scanw("%*c%c",&yes_or_no);
			if(tolower(yes_or_no)=='y')
				save_game();				
			return 0;
	}
}

void help()
{
	clear();
	
	printw("\tIntroduction and Rules\n");
	printw("SOS is a game usually played on paper. \
It conststs of a grid into which players enter either letter S or O to form SOS in a line. \
The SOS pattern can be vertical, horizontal and diagonal.\n");
	printw("Players take turns to enter S or O, if they make an SOS pattern they score a point. \
If they score a point they get another chance to enter, \
this rule applies to subsequent move as well. \
i.e. they retain the turn until they are forming SOS pattern.");
	printw("The game continues till all positions are filled. The player with highest score is the winner.\n\n");
	
	printw("\tNavigation\n");
	printw("The game displays a menu on startup. It has Start new game,Continue saved game, Help, Records and Exit options.\n");
	printw("To select an option enter the number of the option.\n");
	printw("Start New Game displays new game wizard which leads through setup to start a game.\n");
	printw("Continue saved game continues previously saved game. If no game is saved it will inform the same\n");
	printw("Help opens this help page.\n");
	printw("Records display the records of previously played games such as number of players, rows and columns and the result of game\n");
	printw("Exit exits the game.\n\n");
	
	printw("\tStart Game Wizard\n");
	printw("This sets up the configuration of game.\n");
	printw("It first asks to enter number of players. Minimum numberof players is 2 and Game allows maximum of %d players.\n",MAX_PLAYERS);
	printw("It then asks for names of the players one by one.\n");
	printw("Next, the number of rows are asked, minimum is %d and maximum is %d.\n",MIN_HEIGHT,MAX_HEIGHT);
	printw("Next, the number of columns are asked, minimum is %d and maximum is %d.\n",MIN_WIDTH,MAX_WIDTH);
	printw("The game setup is done.\n\n");
	
	printw("\tGameplay and Controls\n");
	printw("The names of players and their scores are displayed at top. \
The grid is displayed below it. Below this, a prompt to enter S or O is given. \
After selecting the letter its position has to be selected by entering number corresponding to the position.\n\n");

	printw("\tPause Menu\n");
	printw("Pause menu is invoked by entering P at the prompt for selecting the letter. \
It displays options to Resume, Restart and Exit.\n");
	printw("Resume just continues the game.\n");
	printw("Restart restarts game by resetting the scores and board, \
but retains the configuration of player names and size of board.\n");
	printw("Exit is used to quit game and take user to main menu. A prompt is given asking if user wants to save the game.\
If yes is selected then the state of game is saved and it can be continued later using continue saved game in main menu\n\n");
	
	printw("\nPress Enter to continue...\n");								//hold on until user decides to continue
	refresh();
	scanw("%*c%*c");
}

void save_game()
{
	FILE *fp=fopen("saved_game","wb");
	fwrite(&num_of_players,sizeof(int),1,fp);
	fwrite(players,sizeof(PLAYER),num_of_players,fp);
	fwrite(&rows,sizeof(int),1,fp);
	fwrite(&columns,sizeof(int),1,fp);
	for(int i=0;i<rows;i++)
		fwrite(grid[i],sizeof(char),columns,fp);
	fwrite(&turn,sizeof(int),1,fp);
	fclose(fp);
}

void load_game()
{
	FILE *fp=fopen("saved_game","rb");
	clear();
	if(fp)
	{
		fread(&num_of_players,sizeof(int),1,fp);
		fread(players,sizeof(PLAYER),num_of_players,fp);
		fread(&rows,sizeof(int),1,fp);
		fread(&columns,sizeof(int),1,fp);
		for(int i=0;i<rows;i++)
			fread(grid[i],sizeof(char),columns,fp);
		fread(&turn,sizeof(int),1,fp);
		fclose(fp);
		game();
	}
	else
	{
		printw("\n\tNo saved game found!");
		printw("\n\nPress Enter to continue...\n");						//hold on until user decides to continue
		refresh();
		scanw("%*c%*c");
	}
}

void statistics()
{
	FILE *fp=fopen("records.txt","r");
	clear();
	if(fp)
	{
		int num,r,c;
		char buf;
		printw("No. of players     Rows * Columns     Result\n");		//heading of table
		while(1)														//repeat until break is executed
		{
			fscanf(fp,"%d %d*%d",&num,&r,&c);							//read and print num of players, rows and columns
			printw("%14d%9d * %-7d   ",num,r,c);
			do 															//read and print characters until newline is found
			{
				buf=fgetc(fp);
				putchar(buf);
			}while(buf!='\n');
			
			buf=fgetc(fp);												//read a character and check if it is EOF
			if(buf==EOF)												//if EOF break from loop
				break;
			ungetc(buf,fp);												//if its not EOF then push character back to stream
		}
		fclose(fp);
	}
	else
	{
		printw("\n\tNo records found!");
	}
	printw("\n\nPress Enter to continue...\n");						//hold on until user decides to continue
	refresh();
	scanw("%*c%*c");
}

int generic_menu(WINDOW *win,const char* options[],int num_options)
{
	noecho();
	keypad(win,TRUE);
	wclear(win);
	mvwprintw(win,1,30,"%s",options[0]);
	int selected=1;
	for(int i=1;i<=num_options;i++)
	{
		mvwprintw(win,2+i,15,"%s",options[i]);
	}
	mvwchgat(win,2+selected,0,-1,A_REVERSE,0,NULL);
	wrefresh(win);
	while(1)
	{
		int ch=wgetch(win);
		mvwchgat(win,2+selected,0,-1,A_NORMAL,0,NULL);
		switch(ch)
		{
			case KEY_UP:
				if(selected==1)
					selected=num_options;
				else
					selected--;
				break;
			case KEY_DOWN:
				if(selected==num_options)
					selected=1;
				else
					selected++;
				break;
			case 10:
				echo();
				return selected;
		}
		mvwchgat(win,2+selected,0,-1,A_REVERSE,0,NULL);
		wrefresh(win);
	}
}
