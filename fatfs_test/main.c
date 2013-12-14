/*-----------------------------------------------------------------------------
/
/
/
/
/
/
/
/
/----------------------------------------------------------------------------*/
#include "ff.h"		
#include <avr/io.h>	
#include <util/delay.h>
#include "digital_xmega.h"
#include "xprintf.h"
/*---------------------------------------------------------------------------*/
uint8_t readBuffer[128];
/*---------------------------------------------------------------------------*/
void sendch(uint8_t ch)
{
	while(!(USARTD0.STATUS & USART_DREIF_bm));
	USARTD0.DATA = ch;		
}
/*---------------------------------------------------------------------------*/
int main (void)
{
	UINT bw;
	FATFS FatFs;		/* FatFs work area needed for each volume */
	FIL Fil;			/* File object needed for each open file */

	/*-----------------------------------------------------------------------*/

	OSC.PLLCTRL = OSC_PLLSRC_RC2M_gc | 16 ;
	OSC.CTRL |= OSC_PLLEN_bm ; // enable the PLL...
	while( (OSC.STATUS & OSC_PLLRDY_bm) == 0 ){} // wait until it's stable

	// And now we can (finally) switch to the PLL as a clocksource.
	CCP = CCP_IOREG_gc;				// protected write follows	 
	CLK.CTRL = CLK_SCLKSEL_PLL_gc;	// The System clock is now the PLL output (2MhzRC * 16)

	/*-----------------------------------------------------------------------*/

	/* set UART pins */
	pinMode(D,2,INPUT);
	pinMode(D,3,OUTPUT);
	
	/* 9600 baud rate UART with 32 Mhz clock */
	USARTD0.CTRLB = USART_RXEN_bm|USART_TXEN_bm;
	USARTD0.CTRLC = USART_CMODE_ASYNCHRONOUS_gc|USART_PMODE_DISABLED_gc|USART_CHSIZE_8BIT_gc;
	USARTD0.BAUDCTRLA = 0xF5; 
	USARTD0.BAUDCTRLB = (0XC<<USART_BSCALE_gp)|0X0C; 

	/*-----------------------------------------------------------------------*/

	xdev_out(sendch);
	
	xprintf("\r\n");	
	xprintf("> Hello World!\r\n");
	
	pinMode(B,2,OUTPUT);

	/*-----------------------------------------------------------------------*/

	f_mount(&FatFs, "", 0);		/* Give a work area to the default drive */

	/* Create a file */
	if(f_open(&Fil, "newfile.txt", FA_WRITE | FA_CREATE_ALWAYS) == FR_OK) 
	{
		f_write(&Fil, "It works!\r\n", 11, &bw);	/* Write data to the file */

		f_close(&Fil);								/* Close the file */

		if (bw == 11) 
		{
			xprintf("Ok!\r\n");
		}
	}

	/*-----------------------------------------------------------------------*/

	while(1);
	
	/*-----------------------------------------------------------------------*/
}
/*---------------------------------------------------------------------------*/
DWORD get_fattime (void)
{
	return 0;
}
/*---------------------------------------------------------------------------*/
