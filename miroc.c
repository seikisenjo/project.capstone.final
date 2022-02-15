#define sw1		RE0		//switch to initialize maze solving function using alwaysleft
#define sw2		RE1		//switch to initialize maze solving using simplified path
#define motor_ra	RC0		//PWM to right motor backward
#define motor_rb	RC3		//PWM to right motor forward
#define motor_la	RC4		//PWM to left motor backward
#define motor_lb	RC5		//PWM to left motor forward
#define s_left		RB0		//leftmost IR sensor
#define s_mleft		RB1		//middle left IR sensor
#define s_mright	RB2		//middle right IR sensor
#define s_right		RB3		//rightmost IR sensor
#define buzzer 		RE2		//”nothing particular”
#define	rs		RB7		//lcd mode select 1=display 0=command
#define	e		RB6		//send pulse to lcd to confirm data
#define	lcd_data	PORTD		//all port d is used to interface with lcd
#define b_light     	RB5		//lcd backlight
#define SPEEDL		CCPR1L		//TIMER interrupt generate PWM 
#define SPEEDR		CCPR2L		//TIMER interrupt generate PWM
#define RX_PIN 		RC7
#define TX_PIN 		RA2

//Function prototype that have been used in the program.

void init(void);
void delay(unsigned long data);
void send_config(unsigned char data);
void send_char(unsigned char data);
void e_pulse(void);
void lcd_goto(unsigned char data);
void lcd_clr(void);
void send_string(const char *s);
void dis_num(unsigned long data);		//Function prototype to interface with LCD

void line_follow(void);
void maze_solve(void);
void eepwrite(void);
void left3(void);
void left2(void);
void left1(void);
void left0(void);
void straight(void);
void right3(void);
void right2(void);
void right1(void);
void right0(void);
void stop0(void);
void turnback(void);
void alwaysleft(void);			//Function prototype for maze solving

void forward(void);
void stop (void);
void backward (void);
void reverse (void);
void left(void);
void right(void);				//Function prototype for motor positioning/control

//Global variables used in program

unsigned char data[6] = {0};
unsigned int result;
unsigned int To=0,T=0,TH=0;
unsigned char REC;
unsigned char i=0,raw;

unsigned int us_value (unsigned char mode);
unsigned char memory;
int counter = 0;
int temp = 2;
int store[30] = {0};
int arrange = 0;
int rewrite = 0;
int count = 0;
int pa,pb,pc;			
int counter1 = 0;
int done=0;
int flag=0;
int rwt=0;

void main(void)
{
	delay(20000);									
	init();			// initiate cnfiguration and initial condition (custom function)
	buzzer = 1;		// inditcate the circuit is on with beep
	lcd_clr();		// clear the LCD screen
	send_string("Maze Solving");		// display "Maze Solving" on screen
	lcd_goto(20);				// move to 2nd line to display viable option
	send_string("1:start 2:solve");		// display string according to the mode
	buzzer = 0;				// stop beep

	while(1)			// loop while awaiting for command
	{
		if( !sw1)		// if button SW1 is pressed
		{
			while(!sw1);		// wait until button is released
			line_follow();	//function created to solve maze base on always left rule
		}
		
if( !sw2)		// if button SW2 is pressed
		{
			while(!sw2);		// wait untill button is released
			maze_solve();		//solve maze base on the route recorded in eeprom
	
		}	
	}
}

void init()
{
	// ADC configuration	
	ADCON1 = 0b10000100;		//set RA0 and RA1 as Analog Input, left justified

	// setup for capture pwm
	RBIE = 1;				// enable interrupt on change of port B

	// motor PWM configuration 
	PR2 = 255;					// set period register 
	T2CON =  	0b00000100;			// 
	CCP1CON =	0b00001100;			// config for RC1 to generate PWM		CCP2CON =	0b00001100;			// config for RC2 to generate PWM

__EEPROM_DATA(0,0,0,0,0,0,0,0); 		//default eeprom config

	// Tris configuration (input or output)
	TRISA = 0b00000011;				//set RA0 and RA2 pin as input,other as output
	TRISB = 0b00011111;				//set RB0-RB4 pin as input, other as output
	TRISC = 0b10000000;				//set PORTC pin as output
	TRISD = 0b00000000;				//set all PORTD pin as output
	TRISE = 0b00000011;				//set RE0-RE1 pin as input, other as output

	// TMR 0 configuation
	T0CS = 0;					//select clock source of Timer 0
	PSA = 0;					//Prescaler Assignment bit to TMR0
	PS2 = 1;						// prescale 1:32
	PS1 = 1;						//
	PS0 = 1;						//set prescaler value to 1:256
	TMR0IE = 1;					// TMR0 Interrupt
	TMR0 = 0;					//initialize current timer value

	// enable all unmasked interrupt	
	GIE = 1;
	PEIE = 1;

//setup UART
	SPBRG = 0x81;				//set baud rate to 9600 for 20Mhz
	BRGH = 1;				//baud rate high speed option
	TXEN = 1;				//enable transmission
	TX9 = 0;
	CREN = 1;				//enable reception
	SPEN = 1;				//enable serial port
	RX9 = 0;
	RCIE = 1;				//enable interrupt on eachdata received

	// LCD configuration
	send_config(0b00000001);		//clear display at lcd
	send_config(0b00000010);		//Lcd Return to home 
	send_config(0b00000110);		//entry mode-cursor increase 1
	send_config(0b00001100);		//diplay on, cursor off and cursor blink off
	send_config(0b00111000);		//function

	TX_PIN = 1;
	b_light = 0;		//disable backlight for LCD screen
	buzzer = 0;		//turn off the buzzer after initialization
	stop();			//stop the motor from moving								
}

void line_follow()							
{

	lcd_clr();					// clear lcd screen
	send_string("Turning");				// display "position" string

	while(1)	//enter the while and breaks only if stop condition detected
	{
		if ((s_left==1)&&(s_mleft==0)&&(s_mright==0)&&(s_right==0))  				// if only sensor left detected black line
		{	
left3();								
// display "left3" mean the robot's position is on the left side of the line and increase the speed of left wheel to steer right
		}
		else if ((s_left==1)&&(s_mleft==1)&&(s_mright==0)&&(s_right==0))			// if only sensors on left detected black line
		{		
			left2();		//steer right with speed of right wheel slower than of left3();
		}
		else if ((s_left==0)&&(s_mleft==1)&&(s_mright==0)&&(s_right==0))			// if only sensor middle left detected black line
		{	
			left1();		//steer right with speed of right wheel slower than of left2();
		}
		else if ((s_left==1)&&(s_mleft==1)&&(s_mright==1)&&(s_right==0))			// if the from all 3 sensor of the left side detected black line which occur when robot is steering
		{	
			left0();		//steer right with speed of right wheel slower than of left1();
		}
		else if ((s_left==0)&&(s_mleft==1)&&(s_mright==1)&&(s_right==0))			// if the middle of the sensor detected black line
		{	
			straight();	//move straight with both wheel on same speed
		}
		else if ((s_left==0)&&(s_mleft==0)&&(s_mright==1)&&(s_right==0))			// if only sensor middle right detected black line
		{	
			right1();	//steer left with speed of right wheel slightly faster of right0();
		}
		else if ((s_left==0)&&(s_mleft==1)&&(s_mright==1)&&(s_right==1))			// if 3 sensors starting from the right detected black line
		{	
			right0();	//steer left with speed of right wheel slightly faster 
		}
		else if ((s_left==0)&&(s_mleft==0)&&(s_mright==1)&&(s_right==1))			// if sensor right and sensor middle right detected black line
		{	
			right2();	//steer left with speed of right wheel slightly faster of right1();
		}
		else if ((s_left==0)&&(s_mleft==0)&&(s_mright==0)&&(s_right==1))			// if only sensor right detected black line 
		{	
			right3();	//steer left with speed of right wheel faster of right2();
		}
		else if ((s_left==1)&&(s_mleft==0)&&(s_mright==0)&&(s_right==1))			// if sensor detected stop pattern
		{	
			stop0();		//stop the motor after it detects the stop pattern designed
			break;		//break the while loop after put the motor to stop
		}
		if ((s_left==0)&&(s_mleft==0)&&(s_mright==0)&&(s_right==0))			
// if all sensor could not detected black line
		{	
			turnback();	
//check the condition of the sensor and steer the robot accordingly 
		}
		if ((s_left==1)&&(s_mleft==1)&&(s_mright==1)&&(s_right==1))			
// if all sensor detected black line
		{
			alwaysleft();	
//the main function of the maze solving robot to solve the maze using always left theorem.
		}
	}

lcd_clr();		// clear lcd screen
	send_string("Writing EEPROM         ");		//display current function it performing

	eepwrite();	
	
//custom function created to write the route recorded into eeprom after comparing and store only the correct route.

	lcd_goto(20);		//command the cursor to go to 2nd line of lcd screen
	send_string("...Done        ");		// display condition after it done writing eeprom
}

void eepwrite()
{
//this function prototype provide simplification based on the route travelled and recorded. It include a list of conditions then simplify the route based on a series of combination matched. 	
	void eepwrite()
{	
	do{
		do
		{	
			pa=store[counter];  	
			pb=store[counter+1];							
			pc=store[counter+2];
		
			if ((pa==1)&&(pb==4)&&(pc==3))  				
			{	
				store[rwt]=4;								
				flag=1;	
			}
			else if ((pa==1)&&(pb==4)&&(pc==2))			
			{		
				store[rwt]=3;
				flag=1;	
			}
			else if ((pa==3)&&(pb==4)&&(pc==1))			
			{	
				store[rwt]=4;
				flag=1;	
			}
			else if ((pa==2)&&(pb==4)&&(pc==1))			
			{	
				store[rwt]=3;
				flag=1;	
			}
			else if ((pa==2)&&(pb==4)&&(pc==2))			
			{	
				store[rwt]=4;
				flag=1;	
			}
			else if ((pa==1)&&(pb==4)&&(pc==1))			
			{	
				store[rwt]=2;
				flag=1;	
			}
			else if ((pa==3)&&(pb==4)&&(pc==3))			
			{	
				store[rwt]=2;
				flag=1;	
			}
			else if ((pa==3)&&(pb==4)&&(pc==2))			
			{	
				store[rwt]=1;
				flag=1;	
			}
			else if ((pa==2)&&(pb==4)&&(pc==3))			
			{	
				store[rwt]=1;
				flag=1;	
			}
			if (flag==1)
			{
				counter=counter+2;
			}	
			else if (flag==0)
			{
				store[rwt]=pa;
				store[rwt+1]=pb;
				store[rwt+2]=pc;
			}
//Full list of condition is listed above and each value stored in array when the robot is solving the maze is retrieved and compared to check for possible simplification
		
		flag=0;
		rwt++;	
		counter++;
	
		}while(counter<30)	;
			
		counter=0;
		rwt=0;
		arrange++;

	}while(arrange<5);		

	arrange = 0;

	do
	{
		eeprom_write(rewrite,store[rewrite]);	
		rewrite++;

	}while(rewrite<30);		//Writing to EEPROM after the simplification completed

	rewrite = 0;	

}
		
//the function perform sorting and simplification using do while loop instead of for loop is because we determined that while using for loop, we are unable to jump out of for loop, causing the robot to hang inside the program. It might be due to hardware limitation as we using PIC16F877A as our robot MCU to solve the maze.

void maze_solve()							
{
	lcd_clr();					// clear lcd screen
	send_string("Solving");				// display "Solving" string
		
		while(1)		
		{

			if ((s_left==1)&&(s_mleft==1)&&(s_mright==0)&&(s_right==0))			// if left sensor detected black line
			{		
				left2();		
			}
			else if ((s_left==0)&&(s_mleft==0)&&(s_mright==1)&&(s_right==1))			// if right sensor detected black line
			{		
				right2();	
			}	
			else if ((s_left==1)&&(s_mleft==0)&&(s_mright==0)&&(s_right==0))  			// if only sensor leftmost detected black line
			{	
				left3();									
			}
			else if ((s_left==0)&&(s_mleft==0)&&(s_mright==0)&&(s_right==1))			// if only sensor rightmost detected black line
			{	
				right3();															// right motor speed is 0
			}
			else if ((s_left==0)&&(s_mleft==1)&&(s_mright==1)&&(s_right==0))			// if sensor middle left and sensor middle right detected black line (straight path)
			{	
				straight();
			}
			else if ((s_left==0)&&(s_mleft==1)&&(s_mright==0)&&(s_right==0))			// if only sensor middle left detected black line
			{	
				left1();
			}
			else if ((s_left==0)&&(s_mleft==0)&&(s_mright==1)&&(s_right==0))			// if only sensor middle right detected black line
			{	
				right1();
			}
			else if  ((s_left==1)&&(s_mleft==0)&&(s_mright==0)&&(s_right==1))
	// if sensors detected stop pattern
			{
				stop0();
			}		
//function above used to steer the robot so that it remain on path 

			if (((s_left==1)&&(s_mleft==1)&&(s_mright==1)&&(s_right==1))||((s_left==0)&&(s_mleft==0)&&(s_mright==0)&&(s_right==0)))

//This is the function that reads the data from EEPROM when it reaches a point where it require input to decide the direction the robot steer based on the path simplified
			{
			
			if((eeprom_read(counter1)==1)&&(done==0))	
			{
	//run this function if the value store in eeprom currently is 1
				while(1)
				{
													
				for(;;) 								
				{		
					left3();		//steer robot left			
					if((s_left==0)&&(s_mleft==1)&&(s_mright==0)&&(s_right==0))
					{
						break;
					}	//stop turning robot left after it detects next path
				}

				if ((s_left==1)&&(s_mleft==1)&&(s_mright==0)&&(s_right==0))				// if left sensor detected black line
				{		
					left2();								
				}
				else if ((s_left==0)&&(s_mleft==0)&&(s_mright==1)&&(s_right==1))			// if right sensor detected black line
				{		
					right2();							
				}		
				else if ((s_left==0)&&(s_mleft==1)&&(s_mright==0)&&(s_right==0))			// if only sensor middle left detected black line
				{	
					left1();
				}
				else if ((s_left==0)&&(s_mleft==0)&&(s_mright==1)&&(s_right==0))			// if only sensor middle right detected black line
				{	
					right1();
				}
						if(((s_left==0)&&(s_mleft==1)&&(s_mright==1)&&(s_right==0))||((s_left==0)&&(s_mleft==1)&&(s_mright==0)&&(s_right==0)))
				{
					done=1;
					break;	
				}	
				}	//jump out of this function after it align properly on the path	
			}
			else if((eeprom_read(counter1)==3)&&(done==0))
			{	
//run this function if the value store in eeprom currently is 1
				while(1)
				{
													
				for(;;) 									
				{		
					right3();	//steer robot right				
					if((s_left==0)&&(s_mleft==0)&&(s_mright==1)&&(s_right==0))
					{
						break;
					}	//stop turning robot right after it detects next path
				}

				if ((s_left==1)&&(s_mleft==1)&&(s_mright==0)&&(s_right==0))				// if left sensor detected black line
				{		
					left2();								
				}
				else if ((s_left==0)&&(s_mleft==0)&&(s_mright==1)&&(s_right==1))			// if right sensor detected black line
				{		
					right2();							
				}	
				else if ((s_left==0)&&(s_mleft==1)&&(s_mright==0)&&(s_right==0))			// if only sensor middle left detected black line
				{	
					left1();
				}
				else if ((s_left==0)&&(s_mleft==0)&&(s_mright==1)&&(s_right==0))			// if only sensor middle right detected black line
				{	
					right1();
				}						
				if(((s_left==0)&&(s_mleft==1)&&(s_mright==1)&&(s_right==0))||((s_left==0)&&(s_mleft==0)&&(s_mright==1)&&(s_right==0)))
				{
					done=1;
					break;	
				}	
				}	//jump out of this function after it align properly on the path	
			}
			else if((eeprom_read(counter1)==2)&&(done==0))
			{
				while(1)
				{
				straight();								
				if(!((s_left==1)&&(s_mleft==1)&&(s_mright==1)&&(s_right==1)))
				{
					done=1;
					break;	
				}	//jump out of this function after it align properly on the path
				}	
}	//ignore the junction as straight basically just move forward until it cross the junction
			else if((eeprom_read(counter1)==4)&&(done==0))
			{
				while(1)
				{
				left();	//turn back using the spot turn left function			
				if(((s_left==0)&&(s_mleft==1)&&(s_mright==1)&&(s_right==0))||((s_left==0)&&(s_mleft==1)&&(s_mright==0)&&(s_right==0)))
				{
					done=1;
					break;	
				}	
				}			
}	
//this function is basically not used as the path combination consist of “Back” can be further simplified
			else if((eeprom_read(counter1)==0)&&(done==0))
			{
				while(1)
				{
				stop0();										
				}						
			}	
			}
		if(done==1)
		{
			done=0;
			counter1++;
			if(counter1==30)
			{
				counter1=0;
			}
		}
		}			
}

void left3()
{
	forward();		// motor forward
	SPEEDL = 0;		// left motor speed is 0
	SPEEDR = 195;		// right motor speed is 195
	memory = PORTB&0b00001111;	
//store the pattern of current sensor to compare and steer in turnback() function when it encounter no reading on both sensor
	lcd_goto(20);		// lcd go to 2nd line 1st character 
	send_string ("Left 3             ");								
// display "left 3" mean the robot is currently executing left3() function
}

void left2()
{		
	forward();		// motor forward
	SPEEDL = 140;		// left motor speed is 140
	SPEEDR = 195;		// right motor speed is 195
	memory = PORTB&0b00001111;
///store the pattern of current sensor to compare and steer in turnback() function when it encounter no reading on both sensor

	lcd_goto(20);		// lcd go to 2nd line 1st character 
	send_string ("Left 2            ");		
// display "left 2" mean the robot is currently executing left2() function
}

void left1()
{	
	forward();		// motor forward
	SPEEDL = 160;		// left motor speed is 160
	SPEEDR = 195;		// right motor speed is 195
	memory = PORTB&0b00001111;
//store the pattern of current sensor to compare and steer in turnback() function when it encounter no reading on both sensor

	lcd_goto(20);		// lcd go to 2nd line 1st character
	send_string ("Left 1             ");
// display "left 1" mean the robot is currently executing left1() function
}

void left0()
{	
	forward();		// motor forward
	SPEEDL = 160;		// left motor speed is 160
	SPEEDR = 195;		// right motor speed is 195
	memory = PORTB&0b00001111;
//store the pattern of current sensor to compare and steer in turnback() function when it encounter no reading on both sensor
	lcd_goto(20);		// lcd go to 2nd line 1st character
	send_string ("Left           ");		
// display "left" mean the robot is currently executing left0() function
}

void straight()
{
	forward();		// motor forward
	SPEEDL = 195;		// left motor speed is 195
	SPEEDR = 195;		// right motor speed is 195
	temp = 2;
//variable created as a flag and enable record every time after it return from steering
	memory = PORTB&0b00001111;
//store the pattern of current sensor to compare and steer in turnback() function when it encounter no reading on both sensor	
	lcd_goto(20);		//lcd go to 2nd line 1st character
	send_string ("Straight          ");
// display "straight" mean the robot is currently executing current straight() function
}

void right1()
{	
	forward();		// motor forward
	SPEEDL = 195;		// left motor speed is 195
	SPEEDR = 160;		// right motor speed is 160
	memory = PORTB&0b00001111;
//store the pattern of current sensor to compare and steer in turnback() function when it encounter no reading on both sensor	
	lcd_goto(20);		//lcd go to 2nd line 1st character
	send_string ("Right 1            ");
// display "Right 1" mean the robot is currently executing right1() function
}

void right0()
{	
	forward();		// motor forward
	SPEEDL = 195;		// left motor speed is 195
	SPEEDR = 160;		// right motor speed is 160
	memory = PORTB&0b00001111;
//store the pattern of current sensor to compare and steer in turnback() function when it encounter no reading on both sensor	
	lcd_goto(20);		//lcd go to 2nd line 1st character
	send_string ("Right          ");
// display "Right " mean the robot is currently executing right0() function
}

void right2()
{	
	forward();		// motor forward
	SPEEDL = 195;		// left motor speed is 195
	SPEEDR = 140;		// right motor speed is 140
	memory = PORTB&0b00001111;
//store the pattern of current sensor to compare and steer in turnback() function when it encounter no reading on both sensor	
	lcd_goto(20);		//lcd go to 2nd line 1st character
	send_string ("Right 2          ");
// display "Right 2 " mean the robot is currently executing right2() function
}

void right3()
{	
	forward();		// motor forward
	SPEEDL = 195;		// left motor speed is 195
	SPEEDR = 0;		// right motor speed is 0
	memory = PORTB&0b00001111;
//store the pattern of current sensor to compare and steer in turnback() function when it encounter no reading on both sensor		
	lcd_goto(20);		//lcd go to 2nd line 1st character
	send_string ("Right 3           ");
// display "Right 3 " mean the robot is currently executing right3() function
}

void stop0()
{	
stop();			//stop the motor								memory = PORTB&0b00001111;
//store the pattern of current sensor to compare and steer in turnback() function when it encounter no reading on both sensor
	if(temp == 2)		
//write the path value when the flag is set to ensure it will not record more than once
	{
		store[count] = 0;	//store the path travelled in the form of integer in array
		temp = 0;		//reset the flag
		count++;	//increment the counter value manually to store another value in array 
	}		//store the path the robot travelled if condition fulfills
	lcd_goto(20);		//lcd go to 2nd line 1st character
	send_string ("End          ");	
// display "End " to indicate the robot has done writing eeprom
}

void turnback()		//major function in maze solving to determine robot steering direction based on the previous path travelled
{
	if ((memory == 0b00000001)||(memory == 0b00000010)||(memory == 0b00000011)||(memory == 0b00000111))	
//spot turn left if the recorded path travelled match with statement above, usually used to turn left on inversed ‘r’ junction
	{
		while(1)
		{
			left();		//function used to steer robot right (spot turn)
			if 
(((s_left==0)&&(s_mleft==1)&&(s_mright==0)&&(s_right==0))||((s_left==1)&&(s_mleft==0)&&(s_mright==0)&&(s_right==0)))			
				{
					if (temp == 2)
					{
						store[count] = 1; 
						temp = 1;
						count++;
					}
					break;
				
				}
				lcd_goto(20);
				send_string ("Spot Left         ");
				}
			}
//jump out of the loop when sensor pattern above is detected and record the path in array while display the function It currently execute (Turning left).

else if ((memory == 0b00001000)||(memory == 0b00001100)||(memory == 0b00000100)||(memory == 0b00001110))
//spot turn right if the recorded path travelled match with statement above, usually used to turn right on ‘r’ junction

			{
				while(1)
				{
				right();
				if (((s_left==0)&&(s_mleft==0)&&(s_mright==1)&&(s_right==0))||((s_left==0)&&(s_mleft==0)&&(s_mright==0)&&(s_right==1)))
		{
			if (temp == 2)
			{
				store[count] = 3; 
				temp = 3;
				count++;
			}
			break;
		}
		lcd_goto(20);
		send_string ("Spot Right         ");
		}
//jump out of the loop when sensor pattern above is detected and record the path in array while display the function It currently execute (Turning Right).
	}
	else if (memory == 0b00000110)
//spot turn left if the recorded path travelled match with statement above, used to turn around on the dead end of the junction junction
	{
		while(1)
		{
		left();
		if (((s_left==0)&&(s_mleft==1)&&(s_mright==0)&&(s_right==0))||((s_left==1)&&(s_mleft==0)&&(s_mright==0)&&(s_right==0)))
		{
			if (temp == 2)
			{
				store[count] = 4;
				temp = 4;
				count++;
			}
			break;
		}
		lcd_goto(20);
		send_string ("Back...        ");
		}
//jump out of the loop when sensor pattern above is detected and record the path in array while display the function It currently execute (Turning back).
	}
	if (memory == 0b00001111)
	{
		while(1)
		{
		left();
		if ((s_left==0)&&(s_mleft==0)&&(s_mright==0)&&(s_right==1))
		{
			break;
		}
		lcd_goto(20);
		send_string ("Search Path         ");
		}
	}
}

void alwaysleft()
{
//main function of the maze solving robot algorithm, used to steer robot leftwards every time it detects a “+” junction
	while(1)
	{
	left();	
	memory = PORTB&0b00001111;
	lcd_goto(20);
	send_string ("Search Path         ");										
	//jump out of the left spot turn after it detects the first corner of the new path.			
	if ((s_left==0)&&(s_mleft==0)&&(s_mright==0)&&(s_right==1))
	{
		if (temp == 2)
		{
		store[count] = 1; 
		temp = 1;
		count++;
		}
		break;
	}
	} 
}

//Below are the custom function that is used to control the PWM feed into motor which control speed and direction. This configuration is already initialized in void_init() function

void forward ()
{
	motor_ra = 0;		
	motor_rb = 1;
	motor_la = 0;
	motor_lb = 1;
}

void backward ()
{
	motor_ra = 1;
	motor_rb = 0;
	motor_la = 1;
	motor_lb = 0;
}

void left()
{
	motor_la = 1;
	motor_lb = 0;
	motor_ra = 0;
	motor_rb = 1;
}
void right()
{
	motor_la = 0;
	motor_lb = 1;
	motor_ra = 1;
	motor_rb = 0;
}

void stop()
{
	motor_la = 1;
	motor_lb = 1;
	motor_ra = 1;
	motor_rb = 1;
}

