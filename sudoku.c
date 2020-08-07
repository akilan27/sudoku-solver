
 /*============================SUDOKU SOLVER==============================*/
 /*=============================Version 2.1===============================*/
 /* This program is free software; you can redistribute it and/or modify  */
 /* it under the terms of the GNU General Public License as published by  */
 /* the Free Software Foundation; either version 2 of the License,        */
 /* or(at your option) any later version.                                 */
 /*                                                                       */
 /* This program is distributed in the hope that it will be useful,       */
 /* but WITHOUT ANY WARRANTY; without even the implied warranty of        */
 /* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                  */
 /* See the GNU General Public License for more details.                  */
 /*                                                                       */
 /* You should have received a copy of the GNU General Public License     */
 /* along with this program; if not, write to the                         */
 /* Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,          */
 /* Boston, MA 02110-1301 USA                                             */
 /*                                                                       */
 /* Copyright © 2006,2007 Yuvarajaa S and Akilan R.                       */
 /*                 Both are doing B.Tech(Aerospace) at MIT, Chennai.     */
 /*                                                                       */
 /* Suggesstions and bug reports are warmly welcomed.                     */
 /* Please let us know them at akilan27<AT>gmail<DOT>com>             */
 /*=======================================================================*/



#include<ncurses.h>

/*#define MODE 1       /*show result only. Uncomment and compile for step by step solving */
#define MAXVAL 4        /*Number of times callback function is executed */ 
/*more this value more memory space and lesser the value more processing time*/
/*So optimum value is recommended(=4) */


int sudoku[9][9];
int thrash[9][9][9];
int ctrlvalue=0;
int cbackvalue = 0;

void ibug(int,int);

void input(void);
void changealign(int,int);
void output(void);

int usolve(void);
int hard(void);
void calculatethrash(void);
void scanthrash(void);
void editthrash(int,int,int);
void delthrash(int,int,int);
void solverow(int);
void solvecolumn(int);
void solveblock(int,int);

int conv(int);
int rcheck(int,int);
int ccheck(int,int);
int bcheck(int,int,int);
int compcheck(int,int,int);
int zerocount(void);
void zerothrashassign(void);
int repcheck(void);
int parsearch(void);
void msg(int);

int main()       /*------------------------------------MAIN fUNCTION*/
{
 int retval =0;

 initscr();
 start_color();
 noecho();
 
/*****************************************/
/*          Initialize Color            */
/***************************************/

 init_pair(1,COLOR_RED,COLOR_BLACK);
 init_pair(2,COLOR_GREEN,COLOR_BLACK);
 init_pair(3,COLOR_YELLOW,COLOR_BLACK);
 
 msg(-3);
 input();
 calculatethrash();
 do
 {
  retval =  usolve();
  cbackvalue = 0;
 }while(retval==1);
 msg(-1);
 if(repcheck()==-1||retval==-1)
    msg(3);
 printw("\n\n\n\r                 :: Processed Result ::\n\n\r");
 output();
 printw(" Press <SPACE> to see credits or <ESC> to quit...");
 while((retval=getch())!= ' ' && retval != '\x1b');
  if(retval == ' ')
     msg(-2);
 endwin();
 return 0;

}

void input(void) /*-------------------------------------INPUT FUNCTION*/
{

 char num;
 int i = 0,j,error = 0,terminator =0;
 do
{
 do
 {
   nocbreak();
   echo();
   printw("Enter the elements of line(%d)",i+1);
   num = getch();
   for(j=0;j<9;j++)
   {
    if((num<'0'||num>'9')&&num!='\n')
     {
      error = 1;
      break;
     }
     if(num!='\n')
      sudoku[i][j] = num - '0';
     else
      break;
     num = getch();
    }/*End of j for  */

if(j!=9&&error == 0)
 changealign(i,j-1);
if(j==9&&num!='\n' || error)
{
 i--;
 if(num!=EOF)
  flushinp();
 msg(0);
 error=0;
}
else if(terminator==1)
     break;    /*breaks the loop as only desired line must be fed.*/


 ++i; /*increment as a part of loop*/
 }while(i<9);/* End of i while*/

 if(repcheck()==-1)
  {
   terminator = 1;
   clear();
   output();
   msg(1);
   do
   {
    printw("Which line do you wish to change? ");
    flushinp();
    cbreak();
    i=getch()-'0';
    nocbreak();
    printw("\n");
   }while(i<1||i>10);
   --i;
  }/*end of terminator check*/
  else
    terminator = 0;
 }while(terminator==1); /*end of main terminator loop*/

 zerothrashassign();  /*Flush out thrash*/
noecho();
cbreak();
}/* End of input function */



void changealign(int x,int index)  /*-----------------------CHANGE ALIGN*/
{
 int i;
 for(i=8;i>=0;--index,--i)
  {
   if(index>=0)
    sudoku[x][i]=sudoku[x][index];
   else
    sudoku[x][i]=0;
  }
}/* End of chgealign */

void output(void)  /*----------------------------------------OUTPUT*/
{
 int i,j;
 for(i=0;i<9;i++)
 {
  printw("                ");  /*space provider */
  for(j=0;j<9;j++)
  {
    if(sudoku[i][j]==0)
      printw(" *");
    else
      printw(" %d",sudoku[i][j]);
   if((j+1)%3==0)  printw(" ");
  }
 printw("\n");
 if((i+1)%3==0)  printw("\n");
}
} /*End of output*/

int usolve()  /*---------------------------------------------USOLVE*/
{
 int n1,i,j,retval= 0;
 while(zerocount()!=0)
 {
  n1 = zerocount();
  while(zerocount()!=0)
  {
   n1= zerocount();
   for(i=0;i<9;i++)
   {
    ctrlvalue =1;
    solverow(i);
    ctrlvalue = 2;
    solvecolumn(i);
   }
   for(i=0;i<=6;i+=3)
    for(j=0;j<=6;j+=3)
    {
     ctrlvalue = 3;
     solveblock(i,j);
    }
   if(n1==zerocount())
    break;
  }/*End of inner while*/
  if(zerocount()!=0)
  {
   ctrlvalue = 4;
   scanthrash();
  }
  if(n1==zerocount())
  {
   if(parsearch()==-1||repcheck()==-1)
       return -1; /*error*/
   cbackvalue++;
   if(cbackvalue>(MAXVAL+1))
      return 1;
   else
     retval = hard();
   if(retval==1)
     return 1;
   if(retval==0)
      return 0; /*solved*/
   if(retval==-1)
      return -1; /*error*/
  }
 }/*End of outer while*/
 return 0;
}/*end of usolve*/

int hard()  /*----------------------------------------------------HARD*/
{
 int retval1=0,count =0 ,a1 = 0,a2=0,passkey = 0,lc=9,i,j,k;
 int falsud[9][9];
 int faltray[9][9][9];
 do
 {
  for(i=0;i<9;i++)
   {
    for(j=0;j<9;j++)
    {
     for(k=0;k<9;k++)
      {
       if(thrash[i][j][k]!=0)
	count++;
       falsud[i][j]=sudoku[i][j];
       faltray[i][j][k]=thrash[i][j][k];
      }/*End of k*/
      if(count<lc && count!= 0)
       {
	lc=count;
	a1 =i;a2 =j;
       }
	count =0; /*flush*/
    }/* end of j*/
  }/*end of i*/

 /*processing*/
 while(thrash[a1][a2][passkey]==0)
  {
   passkey++;
   if(passkey>8)
     return -1; /*error*/
  }
 /* assumption*/
  if(retval1==-1)
      ctrlvalue = 6;
  else
      ctrlvalue  = 5;

 sudoku[a1][a2]=thrash[a1][a2][passkey];
 delthrash(sudoku[a1][a2],a1,a2);

    cbackvalue++;
    if(cbackvalue>(MAXVAL+1))
      retval1 = -1;
    else
       retval1 =usolve();
   /*process result */

 if(retval1==-1)
  {
   for(i=0;i<9;i++)
    for(j=0;j<9;j++)
     for(k=0;k<9;k++)
     {
      /* restore */
      sudoku[i][j] = falsud[i][j];
      thrash[i][j][k] = faltray[i][j][k];
     }
      /* take next value */
      passkey++;
      lc=9;
      a1=a2=0; count = 0;

    /*checking callback*/
    if(cbackvalue>(MAXVAL+1))
	return 1;
    else
	cbackvalue--;

  } /* end of -1 */
  if(retval1==1)
	return 1;
  if (retval1==0)
    return 0; /*solved*/
}while(retval1==-1); /*End of outer most while */

}/*end of hard*/

void calculatethrash() /*--------------------------------CALCULATE THRASH*/
{
  int m,n,l=0,i;
  for(m=0;m<9;m++)
  {
   for(n=0;n<9;n++)
   {
     if(sudoku[m][n]!=0)
	continue;
      for(i=1;i<=9;i++)
      {
       if(compcheck(i,m,n)==0)
	{
	 while(thrash[m][n][l]!=0)
	   l++;
	 thrash[m][n][l]=i;
	  l++;

	}/*end if*/

      }/*end for*/
     l=0;
   }
  }
#ifdef MODE
   ibug(9,9);
#endif

}/*End of calculate thrash */

void scanthrash() /*------------------------------------------SCANTHRASH*/
{
int count =0,m,n,l,note;
 for(m=0;m<9;m++)
 {
  for(n=0;n<9;n++)
   {
    if(sudoku[m][n]!=0) continue;
     for(l=0;l<9;l++)
       if(thrash[m][n][l]!=0)
	{ count++; note =l; }
    if(count ==1) break;
    else count =0;
   }
if(count==1) break;
  } /* End of m for */
if(count==1)
 {
  editthrash(m,n,note);
  scanthrash();
 }
}/*     End of scanthrash              */

void editthrash(int m,int n,int l)
{ int delvar;
 delvar = thrash[m][n][l];
 sudoku[m][n]=thrash[m][n][l];
 delthrash(delvar,m,n);
}

void delthrash(int delvar,int m,int n)
{  int i,j,k;
  for(i=0;i<9;i++)
   thrash[m][n][i]=0;

  for(i=0;i<9;i++)
   for(j=0;j<9;j++)
  { if(thrash[m][i][j]==delvar)
	thrash[m][i][j]=0;
    if(thrash[i][n][j]==delvar)
	thrash[i][n][j]=0;
  }

  for(i=conv(m);i<=(conv(m)+2);i++)
   for(j=conv(n);j<=(conv(n)+2);j++)
     for(k=0;k<9;k++)
      if(thrash[i][j][k]==delvar)
       thrash[i][j][k]=0;
 #ifdef MODE
   ibug(m,n);
 #endif
	
}/*    End of delthrash    */

/**************************PROCESSING ZONE************************/

void solverow(int r)
{
  int note,count=0,i,j;
   for(i=1;i<=9;i++)
   {
    for(j=0;j<9;j++)
     {
      if(sudoku[r][j]!=0)   continue;
       if(compcheck(i,r,j)==0)
	{
	 count++; note = j;
	}
     }/* End of j for */
      if(count==1)
       {
	sudoku[r][note]=i;
	delthrash(i,r,note);
       }
      count = 0 ;

  }
 }/*End of solverow*/

void solvecolumn(int c)
{
  int note,count=0,i,j;
  for(i=1;i<=9;i++)
  {
   for(j=0;j<9;j++)
   {
    if(sudoku[j][c]!=0)  continue;
     if(compcheck(i,j,c)==0)
      {
       count++;note = j;
      }
   }
  if(count==1)
   {
    sudoku[note][c]=i;
    delthrash(i,note,c);
   }
 count =0; /*flushing*/
 }
}/*End of solvecolumn*/

void solveblock(int x,int y)
{
 int i,j,m,count=0,n1,n2;
 for(m=1;m<=9;m++)
 {
  for(i=x;i<=(x+2);i++)
   {
    for(j=y;j<=(y+2);j++)
     {
       if(sudoku[i][j]!=0)  continue;
       if(compcheck(m,i,j)==0)
	{
	 count++; n1=i;n2=j;
	}
      }
   }
    if(count==1)
     {
      sudoku[n1][n2]=m;delthrash(m,n1,n2);
     }
  count = 0; /*flushing*/
 }
}/*  End of solveblock   */

/*******************************ACCESSORIES*****************************/

int conv(int x)
{
 if(x<=2) return 0;
 if(x>=3&&x<=5) return 3;
 if(x>=6&&x<=8) return 6;
}

int rcheck(int n,int r)
{
 int i,x=0;
 for(i=0;i<9;i++)
  if(sudoku[r][i]==n)
    x++;
 if(x==0) return 0;
 if(x==1) return 1;
 if(x>1) return -1; /*ERROR*/
}/*end of rcheck*/

int ccheck(int n,int c)
{
 int i,x=0;
 for(i=0;i<9;i++)
  if(sudoku[i][c]==n)
   x++;
 if(x==0) return 0;
 if(x==1) return 1;
 if(x>1) return -1;
}/*End of ccheck  */

int bcheck(int n,int x,int y)
{
 int i,j; int count=0;
 for(i=x;i<=(x+2);i++)
  for(j=y;j<=(y+2);j++)
   if(sudoku[i][j]==n)
     count++;
 if(count==0) return 0;
 if(count==1) return 1;
 if(count>1) return -1;
}/*end of bcheck*/

int compcheck(int num,int x,int y)
{
  if(rcheck(num,x)==0 && ccheck(num,y)==0 && bcheck(num,conv(x),conv(y))==0)
      return 0;
  else if(rcheck(num,x)==-1 || ccheck(num,y)==-1 || bcheck(num,conv(x),conv(y))==-1)
      return -1;
   return 1;
}/*End of compcheck*/

int zerocount()
{
 int i,j,count= 0;
  for(i=0;i<=8;i++)
   for(j=0;j<=8;j++)
     if(sudoku[i][j]==0)
       count++;
  return  count;
}

void zerothrashassign()
{
  int m,n,l;
  for(m=0;m<9;m++)
   for(n=0;n<9;n++)
      for(l=0;l<9;l++)
	thrash[m][n][l]=0;
}

int repcheck()
{
 int i,j,k;
 for(i=0;i<9;i++)
  for(j=0;j<9;j++)
   for(k=1;k<10;k++)
     if(compcheck(k,i,j)==-1)
       return -1;
 return 0;


}/*End of repcheck */

int parsearch()  /*---------------------------------------------parsearch()*/
{
int count= 0,i,j,k;
 for(i=0;i<9;i++)
 {
  for(j=0;j<9;j++)
   {
     for(k=0;k<9;k++)
      {
	if(thrash[i][j][k]!=0)
	 count++;
      }/*End k for*/
   if(count==0 && sudoku[i][j]==0)
    return -1; /* Error*/

    count =0;/*flush*/
  } /*end of j*/
 }  /*  end of i*/
 return 0;

}/*End of parallelsearch() */

void msg(int x) /*-------------------------------------------MSG*/
{
 int i;
 printw("\n");
 if(x<0)
  color_set(2,NULL);
 else
  color_set(1,NULL);  /*ERROR msgs in red*/
 switch(x)
 {
  case 0:
     printw(" Error 0: Invalid Input Set\n\r");
     break;
  case 1:
    printw(" Error 1: Value repeated in Sudoku\n\r");
    break;
  case 2:
    printw(" Error 2: Inadequate Code\n\r");
    break;
  case 3:
    printw(" Error 3: Error in sudoku framing\n\r");
    break;
  case -1:
    clear();
    move(1,25);
    printw("    ___________________");
    move(2,25);
    printw("    SUDOKU SOLVER   2.1    ");
    move(3,25);
    printw("    ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯\n\n\r");
    break;
  case -2:
    clear();
    move(0,25);
    printw("        ___________");
    move(1,25);
    printw("          CREDITS          ");
    move(2,25);
    printw("        ¯¯¯¯¯¯¯¯¯¯¯\n\n");
    color_set(3,NULL);
    attron(A_BOLD);
    move(4,4);
    printw("One fine evening Nimbus came up with an idea of writing a C code to solve");
    printw("\nsudoku and discussed it with me. We had a debate whether it could be done so ");
    printw("\neasily. We decided to use the Common Human Logic. He started from scratch as we were not");
    printw(" aware of other algorithms to solve it at that time. He accomplished it within weeks");
    printw(" while I studied the mathematical aspects of np-complete problems.");
    printw("\nWe released it as V 1.0. Then we did the debugging and alterations and  released");
    printw("it as version 2.0 which ran on Win32 and MSDOS platforms only. Since I`m an ");
    printw("\nardent supporter of FSF, I decided to port it to GNU/Linux. I encountered a");
    printw("\nproblem in getting input at runtime. This was solved by using ncurses library   which I mastered and admired.(Thanks to ncurses guys!)");
    printw("\nThis is named as Version 2.1 as it`s derived from V2.0 directly and runs on all Unix clones with ncurses library support.");
    printw("\nWe wish to thank Srini for his moral and technical support.");
    printw("\n\n -Aki27");
    printw("\n\n Yuvarajaa S[Nimbus] & Akilan R are doing B.Tech(Aero) @ MIT, Chennai");
    printw("\n Srinivasan N is on the faculty of IT @ MIT, Chennai ");
    /*printw("           Algorithm & Code: Yuvarajaa S  B.Tech(Aerospace)  MIT, Chennai.");
    printw("   Un*x porting & debugging: Akilan R  B.Tech(Aerospace)  MIT, Chennai.\n");
    printw("                                   (Thanks to ncurses Guys!!)");*/
    attroff(A_BOLD);
    getch();
    break;
  case -3:
    msg(-1);
    color_set(0,NULL);
    printw("\n\rInstructions:");
    printw("\n\n\r 1: Input the numbers of every row without spaces.");
    printw("\n\r 2:Fill the unknown digits with zeroes.");
    printw("\n\r 3:Leading zeroes may be truncated.");
    printw("\n\n\r Eg: 0 0 2 8 0 0 0 9 4 can be given as ");
    printw("\n\n\r        002800094 or 2800094\n\n\n\rStrike any key to continue...");
    getch();
    msg(-1);
    break;
 }/*End of switch*/
 color_set(0,NULL);
}

/*									*/
/*	IBUG: This fn can be used in debugging this program.            */
/*	This fn is executed if (n 1ly if:-)) MODE is defined            */
/*									*/
// 
#ifdef MODE
void ibug(int x,int y)
{
 int i,j,k;
 clear();
 move(1,35);
 printw("Line no:%d, %d ",x+1,y+1);
 move(3,1);
  for(i=0;i<=8;i++)
  {
   for(j=0;j<=8;j++)
   {
    for(k=0;k<=8;k++)
     {
      if(sudoku[i][j]!=0)
      {
       printw("*");
       break;
      }
      if(thrash[i][j][k]!=0)
	printw("%d",thrash[i][j][k]);
     }
    printw(" ");
   }
   printw("\n");
  }
  output();

 switch (ctrlvalue)
 {
  case 1:
    printw("Solved by row...");
    break;
  case 2:
    printw("Solved by column...");
    break;
  case 3:
    printw("Solved by block...");
    break;
  case 4:
    printw("Solved by scanthrash...");
    break;
  case 5:
    printw("Assuming the value...");
    break;
  case 6:
    printw("Changing the assumed Value...");
    break;
  default:
    printw("Processing Calculate thrash...");
 }

 getch();
}

#endif
