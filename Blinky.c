/*----------------------------------------------------------------------------
 * Name:    Blinky.c
 * Purpose: LED Flasher
 * Note(s): __USE_LCD   - enable Output on LCD, uncomment #define in code to use
 *  				for demo (NOT for analysis purposes)
 *----------------------------------------------------------------------------
 * Copyright (c) 2008-2011 Keil - An ARM Company.
 * Name: Anita Tino
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdbool.h>
#include "LPC17xx.H"                       
#include "GLCD.h"
#include "LED.h"
#include "ADC.h"
#include <stdlib.h>
// ADDED BY SID
#include "KBD.h"
// ADDED BY SID

#define __FI        1                      /* Font index 16x24               */
#define __USE_LCD   0										/* Uncomment to use the LCD */

//ITM Stimulus Port definitions for printf //////////////////
#define ITM_Port8(n)    (*((volatile unsigned char *)(0xE0000000+4*n)))
#define ITM_Port16(n)   (*((volatile unsigned short*)(0xE0000000+4*n)))
#define ITM_Port32(n)   (*((volatile unsigned long *)(0xE0000000+4*n)))

#define DEMCR           (*((volatile unsigned long *)(0xE000EDFC)))
#define TRCENA          0x01000000

struct __FILE { int handle;  };
FILE __stdout;
FILE __stdin;

int fputc(int ch, FILE *f) {
  if (DEMCR & TRCENA) {
    while (ITM_Port32(0) == 0);
    ITM_Port8(0) = ch;
  }
  return(ch);
}
/////////////////////////////////////////////////////////

char t2[10];

//Use to trace the pot values in Debug
uint16_t ADC_Dbg;
	  uint32_t ad_avg = 0, kbd_val = 0; // ADDED BY SID
  uint16_t ad_val = 0, ad_val_ = 0xFFFF;
/* Import external variables from IRQ.c file                                  */
extern uint8_t  clock_ms;
void applylogic(int operation);
void update1();
int gamelogic[4][4]={{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}};
extern unsigned char Bg_16bpp_b[];
extern unsigned char Bg_16bpp_t[];

void delay() {
    uint32_t count = 0x0;
    int r1, r2, r3;
    while (count < 0xFFFF) {
        count = count + 0x1;
        r1 = 1;
        r2 = 0;
        r3 = 5;

        while (r2 <= 0x18) {
            if ((r1 - r2) > 0) {
                r1 = r1 + 2;
                r2 = r1 + (r3 * 4);
                r3 = r3 / 2;
            } else {
                r2 = r2 + 1;
            }
        }
    }
}


void update(int sel){
	char text[16];
	
	#ifdef __USE_LCD	
	  GLCD_SetBackColor(Black);
  GLCD_SetTextColor(Yellow);
  GLCD_DisplayString(0, 0, __FI, "     COE718 Project    ");
	GLCD_SetTextColor(White);
  GLCD_DisplayString(1, 0, __FI, "      Some subtitle     ");
  GLCD_DisplayString(2, 0, __FI, "  Main Menu ");
	GLCD_SetTextColor(Black);
	if (sel == 1){
		  GLCD_SetBackColor(Blue);	
		sprintf(text, " 1. Gallery");
  GLCD_DisplayString(5, 0, __FI, (unsigned char *)text);		
	  GLCD_SetBackColor(White);	
  GLCD_DisplayString(6, 0, __FI, " 2. Music");
  GLCD_DisplayString(7, 0, __FI, " 3. Game 1");
  GLCD_DisplayString(8, 0, __FI, " 4. Game 2");	}
	else if (sel == 2){
		GLCD_SetBackColor(Blue);
		sprintf(text, " 2. Music");
  GLCD_DisplayString(6, 0, __FI, (unsigned char *)text);		
	  GLCD_SetBackColor(White);
  GLCD_DisplayString(5, 0, __FI, " 1. Gallery     ");
  GLCD_DisplayString(7, 0, __FI, " 3. Game 1");
  GLCD_DisplayString(8, 0, __FI, " 4. Game 2");	}
	else if (sel == 3){
		sprintf(text, " 3. Game 1");
		GLCD_SetBackColor(Blue);		
  GLCD_DisplayString(7, 0, __FI, (unsigned char *)text);		
	  GLCD_SetBackColor(White);
  GLCD_DisplayString(5, 0, __FI, " 1. Gallery     ");
  GLCD_DisplayString(6, 0, __FI, " 2. Music");
  GLCD_DisplayString(8, 0, __FI, " 4. Game 2");	}
	else if (sel == 4){
		GLCD_SetBackColor(Blue);		
		sprintf(text, " 4. Game 2");
  GLCD_DisplayString(8, 0, __FI, (unsigned char *)text);		
	  GLCD_SetBackColor(White);
  GLCD_DisplayString(5, 0, __FI, " 1. Gallery     ");
  GLCD_DisplayString(6, 0, __FI, " 2. Music");
  GLCD_DisplayString(7, 0, __FI, " 3. Game 1");}
	
 
#endif
}

int gameone (void){
	int gamemath[3][3]={ {0,0,0}, {0,0,0}, {0,0,0}};
		int hilite=0;int x=0;int y=0;
	int turn=0,temp=0,i=0;
  
	GameOneLCD(hilite, x, y, gamemath);
	for (;;){
				kbd_val = KBD_get();
		if (kbd_val == 0x71)	{					//up
			y--;hilite=1;}
		else if (kbd_val == 0x39)  {     // left
			x--;hilite=1;	}
		else if (kbd_val == 0x59)		{	//down
			y++;hilite=1;}
		else if (kbd_val == 0x69) {     // right
			x++;hilite=1;}


		if (y<0)
			hilite=2;
		if (y<-1)
			y=2;
		if (y>2)
			y=0;
		if (x<0)
			x=2;
		if (x>2)
			x=0;
			
		if (kbd_val == 0x78){			//center
			
			if (hilite == 1 || hilite == 0){
				// actual game begins
				
				GameOneLCD(turn+3, x ,y, gamemath);   // xy here
				if (gamemath[x][y] == 0){
					if (turn == 0)
					gamemath[x][y]=turn+1;
					else if (turn ==1)
					gamemath[x][y]=turn+3;
				if (turn == 0) 
					turn=1;
				else 
					turn =0;
			}
			for (i=0; i<3;i++){
				if ((gamemath[i][0]+gamemath[i][1]+gamemath[i][2])==3){
				GameOneLCD(5+i, 1,0,gamemath); //X wins row i
					return(0);
				}
				else if (((gamemath[i][0]+gamemath[i][1]+gamemath[i][2])==16)){
				GameOneLCD(5+i, 0,1,gamemath);//O wins row i
				return(0);
				}
				if ((gamemath[0][i]+gamemath[1][i]+gamemath[2][i])==3)
			  GameOneLCD(8+i, 1,0,gamemath);//X wins col i
				else if ((gamemath[0][i]+gamemath[1][i]+gamemath[2][i])==16)
				GameOneLCD(8+i, 0,1,gamemath);//O wins col i
			}
				if ((gamemath[0][0]+gamemath[1][1]+gamemath[2][2])==3)
				GameOneLCD(11, 1,0,gamemath);//X wins across 1
				else if ((gamemath[0][0]+gamemath[1][1]+gamemath[2][2])==16)
				GameOneLCD(11, 0,1,gamemath);//O wins across 1
				if ((gamemath[0][2]+gamemath[1][1]+gamemath[2][0])==3)
				GameOneLCD(12, 1,0,gamemath);//X wins across 2
				else if ((gamemath[0][2]+gamemath[1][1]+gamemath[2][0])==16)
				GameOneLCD(12, 0,1,gamemath);//O wins across 2
				
				} else {
				LED_On(2);
				#ifdef __USE_LCD
				GLCD_Clear(White); 
				#endif
				return(0);
				}
				//y=2;x=0;
				GameOneLCD(1, x ,y, gamemath);
			} 
		if (kbd_val == 0x79) 
			hilite=0;
		if (hilite == 1 || hilite ==2)
		GameOneLCD(hilite, x ,y, gamemath);
}
 
}

int GameOneLCD(int hi, int ex, int why, int math[3][3]){
	int i=0,j=0;
#ifdef __USE_LCD
	
	if (hi>=8 && hi<=10){
		GLCD_SetBackColor(Red);GLCD_SetTextColor(Yellow);
	GLCD_DisplayString(((hi-8)*3)+1, 0, __FI, "DING!|DING!|DING!");
		if (ex==1)
	GLCD_DisplayString(5, 6, __FI, "X WINS!");
		else if (why==1)
	GLCD_DisplayString(5, 6, __FI, "O WINS!");
		
		for (i=0;i<100;i++)
			delay();
		gameone();
}
if (hi>=5 && hi<=7){
	
	GLCD_SetBackColor(Red);GLCD_SetTextColor(Yellow);
	GLCD_DisplayString(1, (hi-5)*6+4, __FI, "D");GLCD_DisplayString(5, (hi-5)*6+4, __FI, "D");
	GLCD_DisplayString(2, (hi-5)*6+4, __FI, "I");GLCD_DisplayString(6, (hi-5)*6+4, __FI, "I");
	GLCD_DisplayString(3, (hi-5)*6+4, __FI, "N");GLCD_DisplayString(7, (hi-5)*6+4, __FI, "N");
	GLCD_DisplayString(4, (hi-5)*6+4, __FI, "G");GLCD_DisplayString(8, (hi-5)*6+4, __FI, "G");
	if (ex==1)
	GLCD_DisplayString(5, 6, __FI, "X WINS!");
		else if (why==1)
	GLCD_DisplayString(5, 6, __FI, "O WINS!");
		for (i=0;i<100;i++)
			delay();
		gameone();
}
if (hi==11){
	GLCD_SetBackColor(Red);GLCD_SetTextColor(Yellow);
	GLCD_DisplayString(1, 0, __FI, "DING!");
	GLCD_DisplayString(4, 6, __FI, "DING!");
	GLCD_DisplayString(7, 12, __FI, "DING!");
		if (ex==1)
	GLCD_DisplayString(5, 6, __FI, "X WINS!");
		else if (why==1)
	GLCD_DisplayString(5, 6, __FI, "O WINS!");
		for (i=0;i<100;i++)
			delay();
		gameone();
}
if (hi==12){
	GLCD_SetBackColor(Red);GLCD_SetTextColor(Yellow);
	GLCD_DisplayString(8, 0, __FI, "DING!");
	GLCD_DisplayString(4, 6, __FI, "DING!");
	GLCD_DisplayString(1, 12, __FI, "DING!");
		if (ex==1)
	GLCD_DisplayString(5, 6, __FI, "X WINS!");
		else if (why==1)
	GLCD_DisplayString(5, 6, __FI, "O WINS!");
		for (i=0;i<100;i++)
			delay();
		gameone();
}
	
	if (hi == 3 || hi == 4)
//	if (math[ex][why] != 0)
	for (i=0; i<3; i++){
		for (j=0; j<3; j++){
			if (math[i][j]==1)
					sprintf(t2, "  X  ", math[i][j]);
			else if (math[i][j]==4)
				sprintf(t2, "  O  ", math[i][j]);
			else if (math[i][j]==0)
				sprintf(t2, "     ", math[i][j]);
			if (i==ex && j==why){
			GLCD_SetBackColor(Yellow);GLCD_SetTextColor(Blue);}
			else {
					GLCD_SetBackColor(White);GLCD_SetTextColor(Blue);}
			
				GLCD_DisplayString((j*3)+1, (i*6), __FI, (unsigned char *)t2);
				GLCD_DisplayString((j*3)+2, (i*6), __FI, (unsigned char *)t2);			
		}
	}
	
	if (hi == 0){
	GLCD_SetTextColor(Blue);
		GLCD_SetBackColor(White);
  GLCD_DisplayString(0, 0, __FI, "  TIC-TAK-TOE  EXIT");
	GLCD_DisplayString(1, 0, __FI, "     |     |     ");
	GLCD_DisplayString(2, 0, __FI, "     |     |     ");
	GLCD_DisplayString(3, 0, __FI, "_____|_____|_____");
	GLCD_DisplayString(4, 0, __FI, "     |     |     ");
	GLCD_DisplayString(5, 0, __FI, "     |     |     ");
	GLCD_DisplayString(6, 0, __FI, "_____|_____|_____");
	GLCD_DisplayString(7, 0, __FI, "     |     |     ");
	GLCD_DisplayString(8, 0, __FI, "     |     |     ");
		
		GameOneLCD(3,ex,why,math);
	}
	if (hi == 2){
		GameOneLCD(0,ex,why,math);
		
		GLCD_SetTextColor(Blue);
		GLCD_SetBackColor(Yellow);
			GLCD_DisplayString(0, 15, __FI, "EXIT");
		GameOneLCD(3,ex,why,math);
	} else if (hi == 1){  
		GameOneLCD(0,ex,why,math);
		
		GLCD_SetBackColor(Yellow);
	GLCD_DisplayString((why*3)+1, (ex*6), __FI, "     ");
	GLCD_DisplayString((why*3)+2, (ex*6), __FI, "     ");
		delay();delay();delay();delay();
	GameOneLCD(3,ex,why,math);  

}
#endif
}


int gametwo(void){
int operand=0,i=0;
	applylogic(0);
	GameTwoLCD(operand);
//gamelogic=applylogic(1, gamelogic);
	GameTwoLCD(1);
		for (i=0;i<50;i++)
			delay();
	for (;;){
		
		kbd_val = KBD_get();
		if (kbd_val == 0x71)	{					//up
			applylogic(4);
			GameTwoLCD(1);
		}
		else if (kbd_val == 0x39)  {     // left
				applylogic(3);
			GameTwoLCD(1);
		}
		else if (kbd_val == 0x59)		{	//down
				applylogic(2);
			GameTwoLCD(1);
		}
		else if (kbd_val == 0x69) {     // right
				applylogic(1);
			GameTwoLCD(1);
		} 
		else if (kbd_val == 0x78) {     // center
				gametwo();
		}
			
		for (i=0;i<10;i++)
			delay();
		if (gamelogic==NULL)
			return(0);
		} 
	
}

void applylogic(int operation){

	int r=0,i=0,j=0,a=0,k=0,c=0,queue[4]={0,0,0,0},track=0;
	bool helper[4]; 
	int faker[4][4];
	for(i=0;i<4;i++)
		for(j=0;j<4;j++)
			faker[i][j]=gamelogic[i][j];


	
if (operation ==0){
	for (i=0;i<4;i++)
		for (j=0;j<4;j++)
			gamelogic[i][j]=0;	
}
	
	
	if (operation==1){
	
	for (i=0;i<4;i++){
	j=0;track=0;
		for (j=3;j>-1;j--){
		queue[j]=0;
			if (gamelogic[j][i]==0)
				helper[j]=false;
			else helper[j]=true;
		}
		for (j=3;j>-1;j--){
			printf("%d\n", gamelogic[j][i]);
			if (helper[j])
				printf("true %d\n",j);
			else 
				printf("false %d\n",j);
		}	
		
		// START
		for (k=3;k>-1;k--){
			if (helper[k]){
				printf("k %d is true\n", k);
				if (k == 0){
					for (j=1;j<4;j++)
						if (helper[j]){
							queue[j-1]=gamelogic[k][i];
						} else if (j==3) {
							queue[j]=gamelogic[k][i];
						}
				}
				for (c=k-1;c>-1;c--){
					if (helper[c]){
						printf("c %d is true\n", c);
						if (gamelogic[k][i]==gamelogic[c][i]){
							if (k!=3) {
							for (j=k+1;j<4;j++)
								if (helper[j]){
									queue[j-1]=gamelogic[k][i]+gamelogic[c][i];
									helper[c]=false;
									if (j==3)
										helper[k]=false;
									k=c+1;
									
								}
							} else {
								queue[k]=gamelogic[k][i]+gamelogic[c][i];
								helper[c]=false;k=c;c=3;							
							}
							
						} else {
							if (k!=3){
								for (j=k+1;j<4;j++)
									if (helper[j]){
										queue[k]=gamelogic[k][i];
										queue[k-1]=gamelogic[c][i];
										helper[c]=false;
									} else if (j==3) {
										queue[j]=gamelogic[k][i];
										queue[j-1]=gamelogic[c][i];
										helper[c]=false;helper[k]=false;k=c;
									}	
							} else {
								queue[k]=gamelogic[k][i];
								queue[k-1]=gamelogic[c][i];
								if (!(c==k-1))
									helper[c]=false;
								k=c;c=k-1;
								
							}
						}
					} else {
						if (c==0){
							queue[track]=0;	track++;
							//for(j=k;j>-1;j--)								
							queue[track]=gamelogic[k][i];track++;
						} else {
							printf("track is %d value %d\n", track,0);
							queue[track]=0;	track++;
						}
					
					}
				}
			} else {
				if (k!=0)
					queue[track]=0;	track++;
							
			}
		}
			for (j=0;j<4;j++){
				printf("queue at %d = %d\n", j, queue[j]);
				gamelogic[j][i]=queue[j];
			}
	
	}
 	for (i=0;i<10000;i++){
 	r=rand()%4;a=rand()%4;
 	if (gamelogic[r][a]==0){
 		gamelogic[r][a]=2;
 		i=10000;
 	}}
	
	
	
	}
	
 	
 	else if (operation == 2) {
 		for (i=0;i<4;i++)
 			for (j=0;j<4;j++)
 				gamelogic[3-j][i]=faker[j][i];

 		
 		
 		applylogic(1);
		for (i=0;i<4;i++)
 			for (j=0;j<4;j++)
 				faker[j][i]=gamelogic[j][i];
 		for (i=0;i<4;i++)
 			for (j=0;j<4;j++)
 				gamelogic[3-j][i]=faker[j][i];
 			
 			
 	} else if (operation == 3) {
 		for (i=0;i<4;i++)
 			for (j=0;j<4;j++)
 				gamelogic[3-j][i]=faker[i][j];
		

 		applylogic(1);
		for (i=0;i<4;i++)
 			for (j=0;j<4;j++)
 				faker[j][i]=gamelogic[j][i];
 		for (i=0;i<4;i++)
 			for (j=0;j<4;j++)
 				gamelogic[j][3-i]=faker[i][j];
 				
 				
 	} else if (operation == 4) {
 		 for (i=0;i<4;i++)
 			for (j=0;j<4;j++)
 				gamelogic[j][3-i]=faker[i][j];
 
 		applylogic(1);
		for (i=0;i<4;i++)
 			for (j=0;j<4;j++)
 				faker[j][i]=gamelogic[j][i];
 		for (i=0;i<4;i++)
 			for (j=0;j<4;j++)
 				gamelogic[3-j][i]=faker[i][j];
 				
 					
 		
 	}
 	
}


int GameTwoLCD(int op){
	int i=0,j=0;
	#ifdef __USE_LCD
		if (op==0){
	GLCD_SetTextColor(Black);
	GLCD_SetBackColor(White);
  GLCD_DisplayString(0, 0, __FI, "2048");
	GLCD_DisplayString(1, 0, __FI, "(center key to exit)");
	GLCD_DisplayString(2, 0, __FI, "    |    |    |    ");
	GLCD_DisplayString(3, 0, __FI, "____|____|____|____");
	GLCD_DisplayString(4, 0, __FI, "    |    |    |    ");
	GLCD_DisplayString(5, 0, __FI, "____|____|____|____");
	GLCD_DisplayString(6, 0, __FI, "    |    |    |    ");
	GLCD_DisplayString(7, 0, __FI, "____|____|____|____");
	GLCD_DisplayString(8, 0, __FI, "    |    |    |    ");
			for (i=0;i<10;i++)
			delay();
	}
	
	if (op==1){		
		for (i=0;i<4;i++)
		for (j=0;j<4;j++){
			sprintf(t2, "%d", gamelogic[i][j]);
				GLCD_SetTextColor(Black);
			GLCD_SetBackColor(Yellow);
			GLCD_DisplayString(10-(j+1)*2, (i*5)+2, __FI, (unsigned char *)t2);
			
		//return(0);
		}
	}
}
	#endif


//IMAGE AND UPDATE1

int image(){
	int selected1 = 1;
	
#ifdef __USE_LCD
GLCD_Clear(White); 
#endif	
	for (;;){
    update1(selected1);
		delay();delay();		
		kbd_val = KBD_get();

		
	if (kbd_val == 0x71){
			LED_On(1);selected1--;}
		else if (kbd_val == 0x39){
			LED_On(2);
}
		else if (kbd_val == 0x59){
			LED_On(3);selected1++; }
		else if (kbd_val == 0x69){
			
			LED_On(4);  
      show(selected1);						
  }
 
  else if (kbd_val == 0x78){
			LED_On(5);
	    update(1);
      break;		
 }
		else if (kbd_val == 0x79){
		LED_Out(0);}
		
		if (selected1 > 2){
			selected1 = 1;}
		else if (selected1 < 1){
			selected1 = 2;}
		
		
		}	
	
}
int show(int x){
   int num     = -1; 
   int dir     =  1;

while( 1 ){
	kbd_val = KBD_get();
  if(x==1){
	GLCD_Bitmap (  0, 0, 320,  240, Bg_16bpp_t+10);
	if(kbd_val == 0x59 || kbd_val == 0x71){
		x=2;
	}
else if(kbd_val==0x78){
	LED_On(2);
	GLCD_Clear(White); 
	  update1(1);
		break;
					
	}	
}
	else if(x==2){
	GLCD_Bitmap (  0, 0, 320,  240, Bg_16bpp_b+10);
	if(kbd_val == 0x59 || kbd_val == 0x71){
		x=1;
	}
	else if(kbd_val==0x78){
	LED_On(2);
		GLCD_Clear(White); 
	  update1(2);
		break;
					
    	}
   }

  }
return(0);   }


void update1(int sel){
	char text[16];
	
			
#ifdef __USE_LCD
                              /* Initialize graphical LCD (if enabled */

                        /* Clear graphical LCD display   */
  GLCD_SetBackColor(Black);
  GLCD_SetTextColor(Yellow);
  GLCD_DisplayString(0, 0, __FI, "     COE718 Project    ");
	GLCD_SetTextColor(White);
  GLCD_DisplayString(1, 0, __FI, "     Photo Gallery     ");
  GLCD_DisplayString(2, 0, __FI, "     List of Images    ");
	
  GLCD_SetBackColor(Blue);
  GLCD_SetTextColor(Black);

#endif
	#ifdef __USE_LCD	
	if (sel == 1){
		  GLCD_SetBackColor(Blue);	
		sprintf(text, " 1. Image 01");
  GLCD_DisplayString(5, 0, __FI, (unsigned char *)text);		
	  GLCD_SetBackColor(White);	
  GLCD_DisplayString(6, 0, __FI, " 2. Image 02");
  }
	else if (sel == 2){
		GLCD_SetBackColor(Blue);
		sprintf(text, " 2. Image 02");
  GLCD_DisplayString(6, 0, __FI, (unsigned char *)text);		
	  GLCD_SetBackColor(White);
  GLCD_DisplayString(5, 0, __FI, " 1. Image 01     ");
}
	
 
#endif

}







/*----------------------------------------------------------------------------
  Main Program
 *----------------------------------------------------------------------------*/
int main (void) {
	  uint32_t ad_avg = 0, kbd_val = 0; // ADDED BY SID
  uint16_t ad_val = 0, ad_val_ = 0xFFFF;

	int selected = 1;
			KBD_Init();
	LED_Init();


		
		
#ifdef __USE_LCD
  GLCD_Init();                               /* Initialize graphical LCD (if enabled */

  GLCD_Clear(White);                         /* Clear graphical LCD display   */
  GLCD_SetBackColor(Black);
  GLCD_SetTextColor(Yellow);
  GLCD_DisplayString(0, 0, __FI, "     COE718 Project    ");
	GLCD_SetTextColor(White);
  GLCD_DisplayString(1, 0, __FI, "      Some subtitle     ");
  GLCD_DisplayString(2, 0, __FI, "  Main Menu ");
	
  GLCD_SetBackColor(Blue);
  GLCD_SetTextColor(Black);

#endif
	
	for (;;){
		update(selected);
		delay();
					kbd_val = KBD_get();

		sprintf(t2, "0x%04X", kbd_val);
	if (kbd_val == 0x71){
			LED_On(1);selected--;}
		else if (kbd_val == 0x39)
			LED_On(2);
		else if (kbd_val == 0x59){
			LED_On(3);selected++; }
		else if (kbd_val == 0x69){
			LED_On(4);
			if(selected==1){
//				image();
				LED_On(4);
} else if (selected == 3){
		GLCD_Clear(White); 
 	gameone();
} else if (selected == 4){
	GLCD_Clear(White); 
	gametwo();
}
}
		else if (kbd_val == 0x78)
			LED_On(5);
		else if (kbd_val == 0x79)
			LED_Out(0);
		if (selected > 4)
			selected = 1;
		else if (selected < 1)
			selected = 4;
// 		update(selected);
// 		delay();
		
	}
	

	
	
	
	
	
	
}

//   uint32_t ad_avg = 0, kbd_val = 0; // ADDED BY SID
//   uint16_t ad_val = 0, ad_val_ = 0xFFFF;
// 	

//   LED_Init();                                /* LED Initialization            */
//   ADC_Init();                                /* ADC Initialization            */

// 	// ADDED BY SID
// 	KBD_Init();
// 	// ADDED BY SID

// #ifdef __USE_LCD
//   GLCD_Init();                               /* Initialize graphical LCD (if enabled */

//   GLCD_Clear(White);                         /* Clear graphical LCD display   */
//   GLCD_SetBackColor(Blue);
//   GLCD_SetTextColor(Yellow);
//   GLCD_DisplayString(0, 0, __FI, "     COE718 Demo    ");
// 	GLCD_SetTextColor(White);
//   GLCD_DisplayString(1, 0, __FI, "       Blinky.c     ");
//   GLCD_DisplayString(2, 0, __FI, "  Turn pot for LEDs ");
//   GLCD_SetBackColor(White);
//   GLCD_SetTextColor(Blue);
//   GLCD_DisplayString(6, 0, __FI, "AD value:            ");
// 	GLCD_DisplayString(8, 0, __FI, "Joystick:            ");
// #endif

//   SysTick_Config(SystemCoreClock/100);       /* Generate interrupt each 10 ms */

//   while (1) {		/* Loop forever                  */

// 		// ADDED BY SID
// 		kbd_val = KBD_get();
// 		sprintf(t2, "0x%04X", kbd_val);
// 		if (kbd_val == 0x71)
// 			LED_On(1);
// 		else if (kbd_val == 0x39)
// 			LED_On(2);
// 		else if (kbd_val == 0x59)
// 			LED_On(3);
// 		else if (kbd_val == 0x69)
// 			LED_On(4);
// 		else if (kbd_val == 0x78)
// 			LED_On(5);
// 		else if (kbd_val == 0x79)
// 			LED_Out(0);
// 		
// 		// ADDED BY SID

//     /* AD converter input                                                     */
//     if (AD_done) {                           /* If conversion has finished    */
//       AD_done = 0;

//       ad_avg += AD_last << 8;                /* Add AD value to averaging     */
//       ad_avg ++;
//       if ((ad_avg & 0xFF) == 0x10) {         /* average over 16 values        */
//         ad_val = (ad_avg >> 8) >> 4;         /* average devided by 16         */
//         ad_avg = 0;
//       }
//     }
//     
// 		ADC_Dbg = ad_val;
// 		
//     if (ad_val ^ ad_val_) {                  /* AD value changed              */
//       ad_val_ = ad_val;
// 			
//       sprintf(text, "0x%04X", ad_val);       /* format text for print out     */
// 			
// #ifdef __USE_LCD
//       GLCD_SetTextColor(Red);
//       GLCD_DisplayString(6,  9, __FI,  (unsigned char *)text);
// 			GLCD_SetTextColor(Green);
//       GLCD_Bargraph (144, 7*24, 176, 20, (ad_val >> 2)); /* max bargraph is 10 bit */
// 			GLCD_SetTextColor(Red);
// 			GLCD_DisplayString(8,  9, __FI,  (unsigned char *)t2);
// #endif
//     }

//     /* Print message with AD value every 10 ms                               */
//     if (clock_ms) {
//       clock_ms = 0;

//       printf("AD value: %s\r\n", text);
//     }
//   }
// }
