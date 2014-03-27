#include <stdio.h>
#include <thread.h>
#include <kernel.h>
#include <msg.h>
//#include <gpioint.h>
//#include "include/buttons.h"
#include "include/board.h"
//#include "drivers/include/display.h"

char stack[KERNEL_CONF_STACKSIZE_MAIN];
char stack_sim[KERNEL_CONF_STACKSIZE_MAIN];

void board_mng_thread(void);
void board_sim_thread(void);

int board_sim_thread_pid;
int board_mng_thread_pid;
extern int pidLCD;

void initBoard(void)
{
/*	gpioint_init();
	BUTTONS_DIR &= ~(ALL_BUTTONS | AS_INT_PIN);
	// Enable PD
	BUTTONS_OUT &= ~(ALL_BUTTONS | AS_INT_PIN);
	BUTTONS_REN |= (ALL_BUTTONS | AS_INT_PIN);
	gpioint_set(2, 0x0, GPIOINT_RISING_EDGE | GPIOINT_DEBOUNCE, port2IRQ_ISR);
//	gpio_set(2, GPIO_RISING_EDGE | GPIO_DEBOUNCE, accelIRQ_ISR);
*/	
	// Create Board management thread
	board_mng_thread_pid = thread_create(stack, 
		KERNEL_CONF_STACKSIZE_MAIN,
		PRIORITY_MAIN-1,
		CREATE_WOUT_YIELD | CREATE_STACKTEST,
		board_mng_thread, "BRD mng");
	// Create Buttons simulation thread
	board_sim_thread_pid = thread_create(stack_sim,
		KERNEL_CONF_STACKSIZE_MAIN,
		PRIORITY_MAIN,
		/*CREATE_WOUT_YIELD |*/ CREATE_STACKTEST,
		board_sim_thread, "Buttons Sim");
}

/*
void port2IRQ_ISR(void)
{
	uint8_t int_flag = BUTTONS_IFG & BUTTONS_IE;
//	uint8_t int_cma3000;
	uint8_t buzzer = 0;

	if(IRQ_TRIGGERED(int_flag, BUTTON_STAR_PIN))
	{
		button.flag.star = 1;
		//buzzer = 1;
	}
	if(IRQ_TRIGGERED(int_flag, BUTTON_NUM_PIN))
	{
		button.flag.num = 1;
		//buzzer = 1;
	}
	if(IRQ_TRIGGERED(int_flag, BUTTON_UP_PIN))
	{
		button.flag.up = 1;
		//buzzer = 1;
	}
	if(IRQ_TRIGGERED(int_flag, BUTTON_DOWN_PIN))
	{
		button.flag.down = 1;
		//buzzer = 1;
	}
	if(IRQ_TRIGGERED(int_flag, BUTTON_BACKLIGHT_PIN))
	{
		button.flag.backlight = 1;
		//buzzer = 1;
	}
	if (buzzer)
	{
		
	}
	#ifdef NATIVE
	char ch;
	printf(" - > ");
	getch(&ch);
	button.allflag = ch;
	#endif
	
	thread_wakeup(board_mng_thread_pid);
}
*/

void board_mng_thread(void)
{
	msg_t message;
	printf("Board Management Thread started\n");
	while (true)
	{
		msg_receive(&message);
		msg_send(&message, pidLCD, true);
		thread_sleep();
	}
	/*
	while (true)
	{
		if(button.flag.star)
		{
			display_chars(LCD_SEG_L1_3_0, "StAr", SEG_ON);
			button.flag.star = 0;
		}
		if(button.flag.num)
		{
			display_chars(LCD_SEG_L1_3_0, "NUM", SEG_ON);
			button.flag.num = 0;
		}
		if(button.flag.up)
		{
			display_chars(LCD_SEG_L1_3_0, "UP", SEG_ON);
			button.flag.up = 0;
		}
		if(button.flag.down)
		{
			display_chars(LCD_SEG_L1_3_0, "doWn", SEG_ON);
			button.flag.down = 0;
		}
		if(button.flag.backlight)
		{
			display_chars(LCD_SEG_L1_3_0, "bAcL", SEG_ON);
			button.flag.backlight = 0;
		}
		#ifdef NATIVE
		printf("Read -> %c\n", (char)buttons.allflags);
		button.allflags = 0;
		#endif
		thread_sleep();
	}
	 * */
}

void board_sim_thread(void)
{
	msg_t message;
	printf ("Entering sim thread loop\n");	
	while(true)
	{		
		char ch = getchar();
		//getch(&ch);
		thread_wakeup(board_mng_thread_pid);
		//message.content.value = (uint32_t)ch;		
		const char str[10];
		memcpy (str, "\0\0\0\0\0\0\0\0\0\0", 10);
		switch(ch)
		{
			case 's': memcpy(str, "StAr\0",5);				
				break;
			case 'n': memcpy(str,"nUM\0", 4);
				break;
			case 'u': memcpy(str, "UP\0", 3);
				break;
			case 'd': memcpy(str, "dOWn\0", 5);
				break;
			case 'b': memcpy(str, "BAcK\0", 5);
				break;				
		}
		message.content.ptr = str;		
		msg_send(&message, board_mng_thread_pid, true);		
	}
}