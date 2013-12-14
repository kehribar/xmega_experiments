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
#include "pff.h"		
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
	FATFS fatfs;
	DIR dir;
	FILINFO fno;
	FRESULT rc;	
	uint16_t s1;

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

	rc = pf_mount(&fatfs);
	
	if(rc != FR_OK)
	{
		xprintf("> Disk mount problem!\r\n");	
		while(1);
	}
	
	xprintf("> Disk mount ok!\r\n");

	/*-----------------------------------------------------------------------*/

	rc = pf_opendir(&dir,"");
	if (rc) 
	{
		xprintf("> Directory open problem!\r\n");
		while(1);
	}
	xprintf("> Directory open ok!\r\n");

	/*-----------------------------------------------------------------------*/
	
	for (;;) 
	{
		rc = pf_readdir(&dir, &fno);
		if (rc != FR_OK) 
		{
			xprintf("> Directory read problem!\r\n");
			while(1);			
		}
		if (!fno.fname[0])
		{
			break;
		}
		if (fno.fattrib & AM_DIR)
		{
			xprintf("     <DIR>   %s\r\n", fno.fname);
		}
		else
		{
			xprintf("%10lu  %s\r\n", fno.fsize, fno.fname);
		}		
	}
	
	/*-----------------------------------------------------------------------*/
	
	rc = pf_open("LOG01.txt");
	if(rc != FR_OK)
	{
		xprintf("> File open problem!\r\n");
		while(1);		
	}

	rc = pf_read(readBuffer, 128, &s1);	
	if(rc != FR_OK)
	{
		xprintf("> File read problem!\r\n");
		while(1);		
	}

	xprintf("%s\r\n",readBuffer);

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