#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include "lcd4bit.h"

#define DDR_ROW DDRA
#define DDR_ROW2 DDRB
#define DDR_COL DDRC
#define DDR_BUT DDRD
#define DDR_LCD DDRD

#define PORT_ROW PORTA
#define PORT_COL PORTC
#define PORT_ROW2 PORTB
#define PORT_LCD PORTD

#define PIN_BUT PIND

void init(void);
void printSnake( unsigned char rowNumber );
unsigned char directionFind( unsigned char d, unsigned char dnew );
void initGame (void);
void moveSnake(unsigned char r, unsigned char c);
void genFood(void);
void showFood(unsigned char foodRow, unsigned char foodCol,unsigned char rowNumber );
unsigned char FindNextRow(unsigned char d);
unsigned char FindNextCol(unsigned char d);
unsigned char eatFood( unsigned char r, unsigned char c );
void increseLength ( unsigned char newRow, unsigned char newCol );
unsigned char hit( unsigned char r, unsigned char c );
void showInstructions(void);
void userWait(void);
void showScore(unsigned int score); 
void gameOver(void);
void checkHihgscore(unsigned int score);
void difficulty	(void);

unsigned char rowNumber; //current row 
unsigned char snakeRow[104], snakeCol[104]; // snake cordinates
unsigned char snakeLength;  //size
unsigned char foodRow, foodCol;  // food position
unsigned char nextr,nextc; // next column and row
unsigned char direction; //direction of sanke
unsigned char loop ; // add a delay
unsigned char delay; // store the delay from difficulty
unsigned int score; // game score

int main(void){
    
    init();
	showInstructions(); // Display the instructions
    initGame();
    
    while(1){
    
        if ((PIN_BUT &(1<<4)) && (directionFind(direction,1) ))  // go up and prohibit go down
            direction=1;
        if ((PIN_BUT &(1<<5)) && (directionFind(direction,2) ))// go down prohibit go up
            direction=2; 
        if ((PIN_BUT &(1<<6)) && (directionFind(direction,3) ))  // go left prohibit go right
            direction=3;
        if ((PIN_BUT &(1<<7)) && (directionFind(direction,4) ))  // go right prohibit go left
            direction=4;
            
        
        if( loop > delay) {
        
            nextr = FindNextRow( direction );  // find next row
            nextc = FindNextCol( direction );  // find nect column
            
            if( eatFood( nextr, nextc ) ){   // check whether snake hit the target
                
                genFood();
                moveSnake( nextr, nextc );
                increseLength(nextr, nextc);
                
                score++;
                showScore(score);
            }
            if( hit ( nextr, nextc )){  // check whether snake hit the body
                gameOver();
                checkHihgscore(score);
                score = 0;
				initGame();
            
            }
            else 
                moveSnake(nextr,nextc);

            loop = 0;  // loop variabl is used to delay the upper functions to 
        }              // print snake long time, can use to change  snake speed
        
        for( unsigned char i = 0; i < 13; i++ ){     
            PORT_ROW = 0x00; //clear
            PORT_ROW2 = 0x00; // clear
            if(i<8){
                PORT_ROW |= ( 1 << i );
                printSnake( i );
            }
            else{
                PORT_ROW2 |= ( 1 << (i-5) );
                printSnake( i );            
            }
			if (loop > delay/2)
				showFood (foodRow, foodCol, i);
				
            loop++;
            _delay_ms(0.1);
            
        }
    }

return 0;

}

void init (void){ // initialization

    DDR_ROW = 0xff; // row as output
    DDR_COL = 0xff; // column as output
    DDR_BUT &= ~ ((1<<4)|(1<<5)|(1<<6)|(1<<7)); //  button inputs
    DDR_ROW2 = 0xff;
   
    score = 0; // game score
	loop = 0;
    
}

void printSnake( unsigned char rowNumber ){
  
    PORT_COL = 0xff;            //set Column high
    
    for( unsigned char i = 0 ; i < snakeLength; i++ ){                
    
        if( snakeRow[i] == rowNumber ) {    // Check with the row number
       
            PORT_COL &= ~( 1 << snakeCol[i] );
        }
    }

}
unsigned char directionFind( unsigned char d, unsigned char dnew ){
  
	if ( ( d == 1 ) && ( dnew == 2 ) )	 // stop opposit movement
		return 0; 
		
	else if ( ( d == 2 ) && ( dnew == 1 ) )
		return 0;   
		
	else if ( ( d == 3 ) && ( dnew == 4 ) )
		return 0; 
		
	else if ( ( d == 4 ) && ( dnew == 3 ) )
		return 0;
		
	else
		return 1;
		
}

void initGame (void) {  // initialize the game

	difficulty(); // Set the difficulty of the game
	userWait(); //wait untill user press any key

    for( unsigned char i = 0 ; i < snakeLength; i++ ) { //clearing position data
    
        snakeRow[i] = 0; 
        snakeCol[i] = 0;
    }
    
    snakeLength = 3;        //initial length 
    snakeRow[0] = 4;    //initialize row 
    snakeRow[1] = 5;
    snakeRow[2] = 6;
 
    snakeCol[0] = 4;    //initialize column 
    snakeCol[1] = 4;
    snakeCol[2] = 4;
    
	genFood();
    direction = 2;
    
    showScore(score);
	
}
void moveSnake(unsigned char r, unsigned char c){

    for( unsigned char i = snakeLength; i > 0; i-- ){ //move 
        
        snakeRow[i] = snakeRow[i-1];
        snakeCol[i] = snakeCol[i-1];
    }
    snakeRow[0] = r;
    snakeCol[0] = c;
	
}
void genFood( void ){
 
    unsigned char find;                                      

    do{
        find = 0;
        
        foodRow = (unsigned char) ( rand() % 13 );   //random num generation for row
        foodCol = (unsigned char) ( rand() % 8 );  //random num generation for column
    
        for( unsigned char i = 0; i < snakeLength; i++ ){
  
            if( ( foodRow == snakeRow[i] ) && ( foodCol == snakeCol[i] ) ){ // check position with the snake
                find = 1;
                break;
            }                                                              
        } 
    } 
    while( find );

}
void showFood(unsigned char foodRow, unsigned char foodCol, unsigned char rowNumber ){
   
    if( foodRow == rowNumber ){   
        PORT_COL &= ~( 1 << foodCol );
    }
    
}
unsigned char FindNextRow ( unsigned char d ){  // create next row
  
    unsigned char r = 0;
    
    r = snakeRow[0];
    
    if( d == 1 ) {  // wehen going up
        if( ( r-1 ) == -1 )                                     
            r = 12; 
        else
            r--;                                                
    }
    
    else if( d == 2 ){  // when going down     
        if( ( r+1 ) == 13 )                                      
            r = 0; 
        else
            r++;                            
    }

    return r;  
  
}

unsigned char FindNextCol ( unsigned char d ){ // create next column

    unsigned char c = 0;
    c = snakeCol[0];
  
    if( d == 3 ){       // when going left
        if( ( c+1 ) == 8 )                                              
            c = 0; 
        else
            c++; 
    }
    
    else if ( d == 4 ){         // when going right
        if( ( c-1 ) == -1 )
            c = 7; 
        else
            c--; 
    }
  
    return c;
	
}
unsigned char eatFood( unsigned char r, unsigned char c ){
  
    if( ( r == foodRow ) && ( c == foodCol ) )    // check with food for overlap
        return 1; 
    else 
        return 0;
		
}
void increseLength ( unsigned char newRow, unsigned char newCol ){
 
    snakeLength++;
 
    for( unsigned char i = snakeLength; i > 0; i-- ){  //shifting and adding
        snakeRow[i] = snakeRow[i-1];
        snakeCol[i] = snakeCol[i-1];
    }
 
    snakeRow[0] = newRow;   //adding new dot
    snakeCol[0] = newCol;
	
}
unsigned char hit( unsigned char r, unsigned char c ){
  
    for( unsigned char i = 0; i < snakeLength; i++ ){
  
        if( ( snakeRow[i] == r ) && ( snakeCol[i] == c ) )  //look for overlap with the body
            return 1;                                           
    
        else
            return 0;
    }
	
}
void checkHihgscore(unsigned int score){
    //eeprom_read_byte((uint8_t*)46);
    
    if (score>eeprom_read_byte((uint8_t*)46)){
        eeprom_write_byte((uint8_t*)46, score);
    
    }

}
void showInstructions(void) {

    LCDInit(0);
    LCDClear();
    
    LCDWriteStringXY(1,0,"Welcome to");  
    LCDWriteStringXY(1,1,"Snake game");
        //LCDWriteInt(val,field_length);
        
    _delay_ms(500);
    LCDClear();
    LCDWriteStringXY(1,0,"Use Keys to ");  
    LCDWriteStringXY(1,1,"move Snake");
        
    _delay_ms(500);
    LCDClear();
    LCDWriteStringXY(1,0,"Hit the targets");  
    LCDWriteStringXY(1,1,"to get Points");
        
    _delay_ms(500);
    LCDClear();
    LCDWriteStringXY(1,0,"Do not hit the");  
    LCDWriteStringXY(1,1,"Snake Body");
    
    _delay_ms(500);
	
}

void difficulty(void){
	LCDClear();
	LCDWriteStringXY(1,0,"Choose the ");
	LCDWriteStringXY(1,1,"difficulty");
	_delay_ms(400);
	LCDClear();
	LCDWriteStringXY(1,0,"UP = Easy");
	LCDWriteStringXY(1,1,"Down = Hard");
	
	while (1){
		if((PIN_BUT & (1<<4))){
			delay = 550;
			break;
		}
		else if((PIN_BUT & (1<<5))){
			delay= 250;
			break;
		}
		else{
			continue;
		}
	}
	_delay_ms(100);
	
}

void userWait(void){ // wait while press any key
    LCDClear();
    
    LCDWriteStringXY(1,0,"Press any key");  
    LCDWriteStringXY(1,1,"to Continue");
    
    while(1){  // wait for user to press a key

        if((PIN_BUT & (1<<4))|(PIN_BUT & (1<<5))|(PIN_BUT & (1<<6))|(PIN_BUT & (1<<7))){ 
            break;
        }else{
            continue; 
        }
    }
	
}
void showScore(unsigned int score){  // show the game score and highscore
    
    LCDClear();
    
    LCDWriteStringXY(1,0,"High Score=");  // high score
    LCDGotoXY(13,0);
    LCDWriteInt(eeprom_read_byte((uint8_t*)46),2);
    LCDGotoXY(1,1);
    LCDWriteStringXY(1,1,"Your Score="); // current score
    LCDGotoXY(13,1);
    LCDWriteInt(score,2);
    
}
void gameOver(void){ // game over
    LCDClear();
	
    LCDWriteStringXY(1,0,"Game Over");
    _delay_ms(500);
	LCDWriteStringXY(1,1,"Your Score="); // current score
	LCDGotoXY(13,1);
	LCDWriteInt(score,2);
	 
}
