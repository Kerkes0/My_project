#ifndef _F2MC_8FX_EEPROM_H
#define _F2MC_8FX_EEPROM_H

#define E2P_16B			1			// 0x01
#define E2P_32B			(1<<1)		// 0x02
#define E2P_64B			(1<<2)		// 0x04
#define E2P_128B		(1<<3)		// 0x08
#define E2P_256B		(1<<4)		// 0x10
#define E2P_512B		(1<<5)		// 0x20

#define	OK		0
#define	NG		1
#define	BUSY	2
#define	LOWPOWER	3
#define	PARA_ERROR	4

#define CONSISTENT 0
#define	DEFINE	1
#define	RESTORE	2
#define REDEFINE	3


unsigned char	EEPROM_Check(unsigned char ucSize, unsigned char *ucData);
unsigned char	EEPROM_Restore(void);
unsigned char	EEPROM_Define(unsigned char ucSize);
unsigned char	EEPROM_B_Write(unsigned int uiadr, unsigned char ucData, unsigned char *OverflowF);
unsigned char	EEPROM_B_Read(unsigned int uiAdr, unsigned char *ucData);

#endif /* _F2MC_8FX_EEPROM_H */

