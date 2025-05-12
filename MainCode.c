//**PLAN**//
/*

1. Choose I/O for the LED (PA4)
2. Create systick_init funct
3. Create ISR funct ( GPIOA_Handler)
4. Init the portA in the main funct
5. Dont forget create EnableInterrupts and WaitInterrupts to turn on the enable

*/

#include <stdint.h>

//**DECLARATION**//

void EnableInterrupts(void);
void WaitForInterrupt(void);
void Systick_Init (uint32_t ticks);
void SysTick_Handler (void);






//**ADDRESS**//

//PORT A GPIO
#define SYSCTL_RCGCGPIO_R       (*((volatile uint32_t *)0x400FE608)) //RCGCGPIO Address
#define GPIO_PORTA_BASE					(*((volatile uint32_t *)0x40004000)) //Base address for Port A
#define GPIO_PORTA_DATA_R       (*((volatile uint32_t *)0x400043FC)) //Offset 0x3fc
#define GPIO_PORTA_DIR_R        (*((volatile uint32_t *)0x40004400)) //Offset 0x400
#define GPIO_PORTA_PUR_R        (*((volatile uint32_t *)0x40004510)) //Offset 0x510
#define GPIO_PORTA_DEN_R        (*((volatile uint32_t *)0x4000451C)) //Offset 0x51c
	

//ALL ABOUT NVIC (ctrl, reload, pri, current )
#define NVIC_PRI0_R  						(*((volatile uint32_t *)0xE000E400)) //PRI0 because interrupt 0 ( Port A ) ( OFFSET 0X400)
#define NVIC_STCTRL_R						(*((volatile uint32_t *)0xE000E010)) //Offset 0x010
#define NVIC_STRELOAD_R					(*((volatile uint32_t *)0xE000E014)) //Offset 0x014
#define NVIC_STCURRENT_R					(*((volatile uint32_t *)0xE000E018)) //Offset 0x018





//**FUNCTIONS**//

// Enable global interrupts
void EnableInterrupts(void) {
    __asm("CPSIE I");  // CPSIE I = Clear Interrupt Disable bit, enabling interrupts
}

// Wait for interrupt 
void WaitForInterrupts(void) {
    __asm("WFI");  // WFI = Wait For Interrupt instruction
}

// 16 MHz standard clock speed (16.000.0000 ticks / second )
void Systick_Init (uint32_t ticks)
{
	NVIC_STCTRL_R = 0; // No interrupt first
	NVIC_STRELOAD_R = ticks - 1; 
	NVIC_STCURRENT_R = 0;
	NVIC_PRI0_R = (NVIC_PRI0_R & 0xFFFFFF1F) | 0xA0; //priority 5 ( bit 7-5 is 101 which is 5 in decimal, but based on hex postition its 10 so 0xA0)
	
	NVIC_STCTRL_R = 0x07; //On bit 0-2 so 0x07
	
}

void SysTick_Handler (void)
{
	
	GPIO_PORTA_DATA_R ^= 0x10; //turn on PA4 (toggling)(XOR)

}

int main(void)
{
	SYSCTL_RCGCGPIO_R |= 0x01; // Activate Clock for Port A
	GPIO_PORTA_DIR_R |= 0x10;  // Set PA4 (output)
	GPIO_PORTA_DEN_R |= 0x10; // 0001 0000 PA4 enable digital function
	Systick_Init(800000); // 50 ms
	EnableInterrupts();
	
	while(1)
	{
		WaitForInterrupts();
	
	}
}
