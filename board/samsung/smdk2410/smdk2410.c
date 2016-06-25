/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002, 2010
 * David Mueller, ELSOFT AG, <d.mueller@elsoft.ch>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <netdev.h>
#include <asm/io.h>
#include <asm/arch/s3c24x0_cpu.h>

DECLARE_GLOBAL_DATA_PTR;

#define FCLK_SPEED 1

#if FCLK_SPEED==0		/* Fout = 203MHz, Fin = 12MHz for Audio */
#define M_MDIV	0xC3
#define M_PDIV	0x4
#define M_SDIV	0x1
#elif FCLK_SPEED==1		/* Fout = 202.8MHz */
#define M_MDIV	0xA1
#define M_PDIV	0x3
#define M_SDIV	0x1
#endif

#define USB_CLOCK 1

#if USB_CLOCK==0
#define U_M_MDIV	0xA1
#define U_M_PDIV	0x3
#define U_M_SDIV	0x1
#elif USB_CLOCK==1
#define U_M_MDIV	0x5c
#define U_M_PDIV	0x1
#define U_M_SDIV	0x1
#endif

static inline void pll_delay(unsigned long loops)
{
	__asm__ volatile ("1:\n"
	  "subs %0, %1, #1\n"
	  "bne 1b":"=r" (loops):"0" (loops));
}

/*
 * Miscellaneous platform dependent initialisations
 */

int board_early_init_f(void)
{
	struct s3c24x0_clock_power * const clk_power =
					s3c24x0_get_base_clock_power();
	struct s3c24x0_gpio * const gpio = s3c24x0_get_base_gpio();

	writel(0x5,&clk_power->clkdivn);

	/* to reduce PLL lock time, adjust the LOCKTIME register */
	writel(0xFFFFFF, &clk_power->locktime);

	/* configure MPLL */
	writel((M_MDIV << 12) + (M_PDIV << 4) + M_SDIV,
	       &clk_power->mpllcon);

	/* some delay between MPLL and UPLL */
	pll_delay(4000);

	/* configure UPLL */
	writel((U_M_MDIV << 12) + (U_M_PDIV << 4) + U_M_SDIV,
	       &clk_power->upllcon);

	/* some delay between MPLL and UPLL */
	pll_delay(8000);

	/* set up the I/O ports */
	writel(0x007FFFFF, &gpio->gpacon);
	writel(0x00044555, &gpio->gpbcon);
	writel(0x000007FF, &gpio->gpbup);
	writel(0xAAAAAAAA, &gpio->gpccon);
	writel(0x0000FFFF, &gpio->gpcup);
	writel(0xAAAAAAAA, &gpio->gpdcon);
	writel(0x0000FFFF, &gpio->gpdup);
	writel(0xAAAAAAAA, &gpio->gpecon);
	writel(0x0000FFFF, &gpio->gpeup);
	writel(0x000055AA, &gpio->gpfcon);
	writel(0x000000FF, &gpio->gpfup);
	writel(0xFF95FFBA, &gpio->gpgcon);
	writel(0x0000FFFF, &gpio->gpgup);
	writel(0x002AFAAA, &gpio->gphcon);
	writel(0x000007FF, &gpio->gphup);

	return 0;
}

int board_init(void)
{
	/* arch number of SMDK2410-Board */
	gd->bd->bi_arch_number = MACH_TYPE_SMDK2410;

	/* adress of boot parameters */
	gd->bd->bi_boot_params = 0x30000100;

	icache_enable();
	dcache_enable();

	return 0;
}

int dram_init(void)
{
	/* dram_init must store complete ramsize in gd->ram_size */
	gd->ram_size = PHYS_SDRAM_1_SIZE;
	return 0;
}

#ifdef CONFIG_CMD_NET
int board_eth_init(bd_t *bis)
{
	int rc = 0;
#ifdef CONFIG_CS8900
	rc = cs8900_initialize(0, CONFIG_CS8900_BASE);
#endif
	return rc;
}
#endif

/*
 * Hardcoded flash setup:
 * Flash 0 is a non-CFI AMD AM29LV800BB flash.
 */
ulong board_flash_get_legacy(ulong base, int banknum, flash_info_t *info)
{
	info->portwidth = FLASH_CFI_16BIT;
	info->chipwidth = FLASH_CFI_BY16;
	info->interface = FLASH_CFI_X16;
	return 1;
}

/*****************************************************************************
 * Define section
 * add all #define here
 *****************************************************************************/
/* LED memory map */
#define	GPFCON		(*(volatile unsigned long *)0x56000050)
#define	GPFDAT		(*(volatile unsigned long *)0x56000054)
/* LED Mode Define */
#define LED_INPUT_MODE                                    0
#define LED_OUTPUT_MODE                                   1
#define LED_EXINT_MODE                                    2
/* LED PIN Define */
#define LED1                                              4
#define LED2                                              5
#define LED3                                              6
/* LED port define */
#define LED_CON_REG                                  GPFCON
#define LED_DATA_REG                                 GPFDAT

/*****************************************************************************
 * Function:
 *      LED_Init
 * Description:
 *		This is used to initialize LED
 * Parameters:
 *		ulLED_PinNum: The corresponding LED pin
 *      ulGPIO_Mode:  You need to set this mode with LED pin
 * Return:
 *		None
 *****************************************************************************/
void LED_Init( unsigned long ulLED_PinNum, unsigned long ulGPIO_Mode )
{
    LED_CON_REG |= (ulGPIO_Mode << (ulLED_PinNum*2));
}
/*****************************************************************************
 * Function:
 *      LED_On
 * Description:
 *		Turn on the LED
 * Parameters:
 *		ulLED_PinNum: The corresponding LED pin
 * Return:
 *		None
 *****************************************************************************/
void LED_On( unsigned long ulLED_PinNum )
{
	LED_DATA_REG &= ~( 1 << ulLED_PinNum );
}
/*****************************************************************************
 * Function:
 *      LED_Off
 * Description:
 *		Turn off the LED
 * Parameters:
 *		ulLED_PinNum: The corresponding LED pin
 * Return:
 *		None
 *****************************************************************************/
void LED_Off( unsigned long ulLED_PinNum )
{
	LED_DATA_REG |= ( 1 << ulLED_PinNum );
}

/*****************************************************************************
 * Function:
 *      show_boot_process
 * Description:
 *		Use LED debug u-boot
 * Parameters:
 *		status: The status of u-boot's step
 * Return:
 *		None
 *****************************************************************************/
void show_boot_process(int status)
{
	LED_Init(LED1,LED_OUTPUT_MODE);
	LED_Init(LED2,LED_OUTPUT_MODE);
	LED_Init(LED3,LED_OUTPUT_MODE);

	switch(status)
	{
		case 1: LED_On(LED1) ;LED_Off(LED2);LED_Off(LED3); break;
		case 2: LED_Off(LED1);LED_On(LED2) ;LED_Off(LED3); break;
		case 3: LED_On(LED1) ;LED_On(LED2) ;LED_Off(LED3); break;
		case 4: LED_Off(LED1);LED_Off(LED2);LED_On(LED3) ; break;
		case 5: LED_On(LED1) ;LED_Off(LED2);LED_On(LED3) ; break;
		case 6: LED_Off(LED1);LED_On(LED2) ;LED_On(LED3) ; break;
		case 7: LED_On(LED1) ;LED_On(LED2) ;LED_On(LED3) ; break;
		default : break;
	}
}
