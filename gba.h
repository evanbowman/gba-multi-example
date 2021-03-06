
#define REG_BASE 0x04000000


typedef unsigned short u16;


//---------------------------------------------------------------------------------
// SIO mode control bits used with REG_SIOCNT
//---------------------------------------------------------------------------------
#define SIO_8BIT		0x0000	// Normal 8-bit communication mode
#define SIO_32BIT		0x1000	// Normal 32-bit communication mode
#define SIO_MULTI		0x2000	// Multi-play communication mode
#define SIO_UART		0x3000	// UART communication mode
#define SIO_IRQ			0x4000	// Enable serial irq


//---------------------------------------------------------------------------------
// baud rate settings
//---------------------------------------------------------------------------------
#define SIO_9600		0x0000
#define SIO_38400		0x0001
#define SIO_57600		0x0002
#define SIO_115200		0x0003

#define SIO_CLK_INT		(1<<0)	// Select internal clock
#define SIO_2MHZ_CLK	(1<<1)	// Select 2MHz clock
#define SIO_RDY			(1<<2)	// Opponent SO state
#define SIO_SO_HIGH		(1<<3)	// Our SO state
#define SIO_START		(1<<7)


//---------------------------------------------------------------------------------
// SIO modes set with REG_RCNT
//---------------------------------------------------------------------------------
#define R_NORMAL		0x0000
#define R_MULTI			0x0000
#define R_UART			0x0000
#define R_GPIO			0x8000
#define R_JOYBUS		0xC000

//---------------------------------------------------------------------------------
// General purpose mode control bits used with REG_RCNT
//---------------------------------------------------------------------------------
#define	GPIO_SC			0x0001	// Data
#define	GPIO_SD			0x0002
#define	GPIO_SI			0x0004
#define	GPIO_SO			0x0008
#define	GPIO_SC_IO		0x0010	// Select I/O
#define	GPIO_SD_IO		0x0020
#define	GPIO_SI_IO		0x0040
#define	GPIO_SO_IO		0x0080
#define	GPIO_SC_INPUT	0x0000	// Input setting
#define GPIO_SD_INPUT	0x0000
#define	GPIO_SI_INPUT	0x0000
#define	GPIO_SO_INPUT	0x0000
#define	GPIO_SC_OUTPUT	0x0010	// Output setting
#define	GPIO_SD_OUTPUT	0x0020
#define	GPIO_SI_OUTPUT	0x0040
#define	GPIO_SO_OUTPUT	0x0080


#define REG_SIOCNT		*(volatile u16*)(REG_BASE + 0x128)	// Serial Communication Control
#define REG_SIODATA8	*(volatile u16*)(REG_BASE + 0x12a)	// 8bit Serial Communication Data
#define REG_SIODATA32	*(volatile u32*)(REG_BASE + 0x120)	// 32bit Serial Communication Data
#define REG_SIOMLT_SEND	*(volatile u16*)(REG_BASE + 0x12a)	// Multi-play SIO Send Data
#define REG_SIOMLT_RECV	*(volatile u16*)(REG_BASE + 0x120)	// Multi-play SIO Receive Data
#define REG_SIOMULTI0	*(volatile u16*)(REG_BASE + 0x120)	// Master data
#define REG_SIOMULTI1	*(volatile u16*)(REG_BASE + 0x122)	// Slave 1 data
#define REG_SIOMULTI2	*(volatile u16*)(REG_BASE + 0x124)	// Slave 2 data
#define REG_SIOMULTI3	*(volatile u16*)(REG_BASE + 0x126)	// Slave 3 data

#define REG_RCNT		*(volatile u16*)(REG_BASE + 0x134)	// SIO Mode Select/General Purpose Data

#define REG_TM2CNT *(volatile u32*)(0x04000000 + 0x108)
#define REG_TM2CNT_L *(volatile u16*)(REG_BASE + 0x108)
#define REG_TM2CNT_H *(volatile u16*)(REG_BASE + 0x10a)
