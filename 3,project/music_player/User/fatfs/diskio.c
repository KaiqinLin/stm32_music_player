/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"    /* FatFs lower layer API */
#include "ff.h"
#include "./sdio/bsp_sdio.h"
#include "string.h"

/* Definitions of physical drive number for each drive */
#define ATA          0  /* SD card to physical drive 0 */
#define SPI_FLASH    1  /* SPI flash to physical drive 1 */

#define SD_BLOCKSIZE  512

extern  SD_CardInfo SDCardInfo;

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
  BYTE pdrv    /* Physical drive nmuber to identify the drive */
)
{
  DSTATUS stat = STA_NOINIT;

  switch (pdrv) {
  case ATA :

    // translate the reslut code here
        stat &= ~STA_NOINIT;

    return stat;

  case SPI_FLASH :

    // translate the reslut code here

    return stat;

  }
  return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
  BYTE pdrv        /* Physical drive nmuber to identify the drive */
)
{
  DSTATUS stat = STA_NOINIT;
  int result;

  switch (pdrv) {
  case ATA :
    result = SD_Init();
      if (result == SD_OK)
      {
        stat &= ~STA_NOINIT;
      }
      else 
      {
        stat = STA_NOINIT;
      }
    // translate the reslut code here

    return stat;

  case SPI_FLASH :

    // translate the reslut code here

    return stat;
  }
  return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
  BYTE pdrv,    /* Physical drive nmuber to identify the drive */
  BYTE *buff,    /* Data buffer to store read data */
  DWORD sector,  /* Start sector in LBA */
  UINT count    /* Number of sectors to read */
)
{
  DRESULT  status = RES_PARERR;
  SD_Error SD_state = SD_OK;

  switch (pdrv) {
    case ATA :
    // translate the arguments here

      if((DWORD)buff&3)
      {
        DRESULT res = RES_OK;
        DWORD scratch[SD_BLOCKSIZE / 4];

        while (count--) 
        {
          res = disk_read(ATA,(void *)scratch, sector++, 1);

          if (res != RES_OK) 
          {
            break;
          }
          memcpy(buff, scratch, SD_BLOCKSIZE);
          buff += SD_BLOCKSIZE;
        }
        return res;
      }
      
      SD_state=SD_ReadMultiBlocks(buff,sector*SD_BLOCKSIZE,SD_BLOCKSIZE,count);
      if(SD_state==SD_OK)
      {
        /* Check if the Transfer is finished */
        SD_state=SD_WaitReadOperation();
        while(SD_GetStatus() != SD_TRANSFER_OK);
      }
      if(SD_state!=SD_OK)
        status = RES_PARERR;
      else
        status = RES_OK;

    // translate the reslut code here

      return status;

    case SPI_FLASH :
    // translate the arguments here

    // translate the reslut code here

      return status;
  }

  return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
  BYTE pdrv,      /* Physical drive nmuber to identify the drive */
  const BYTE *buff,  /* Data to be written */
  DWORD sector,    /* Start sector in LBA */
  UINT count      /* Number of sectors to write */
)
{
  DRESULT status;
  SD_Error SD_state = SD_OK;

  if (!count) {
    return RES_PARERR;    /* Check parameter */
  }

  switch (pdrv) {
  case ATA :
    // translate the arguments here

      if((DWORD)buff&3)
      {
        DRESULT res = RES_OK;
        DWORD scratch[SD_BLOCKSIZE / 4];

        while (count--) 
        {
          memcpy( scratch,buff,SD_BLOCKSIZE);
          res = disk_write(ATA,(void *)scratch, sector++, 1);
          if (res != RES_OK) 
          {
            break;
          }          
          buff += SD_BLOCKSIZE;
        }
        return res;
      }
    
      SD_state=SD_WriteMultiBlocks((uint8_t *)buff,sector*SD_BLOCKSIZE,SD_BLOCKSIZE,count);
      if(SD_state==SD_OK)
      {
        /* Check if the Transfer is finished */
        SD_state=SD_WaitWriteOperation();

        /* Wait until end of DMA transfer */
        while(SD_GetStatus() != SD_TRANSFER_OK);
      }
      if(SD_state!=SD_OK)
        status = RES_PARERR;
      else
        status = RES_OK;

    // translate the reslut code here

    return status;

  case SPI_FLASH :
    // translate the arguments here


    // translate the reslut code here

    return status;
  }

  return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
  BYTE pdrv,    /* Physical drive nmuber (0..) */
  BYTE cmd,    /* Control code */
  void *buff    /* Buffer to send/receive control data */
)
{
  DRESULT res;

  switch (pdrv) {
  case ATA :

    // Process of the command for the RAM drive
      switch (cmd) 
      {
        // Get R/W sector size (WORD) 
        case GET_SECTOR_SIZE :    
          *(WORD * )buff = SD_BLOCKSIZE;
        break;
        // Get erase block size in unit of sector (DWORD)
        case GET_BLOCK_SIZE :      
          *(DWORD * )buff = 1;//SDCardInfo.CardBlockSize;
        break;

        case GET_SECTOR_COUNT:
          *(DWORD * )buff = SDCardInfo.CardCapacity/SDCardInfo.CardBlockSize;
          break;
        case CTRL_SYNC :
        break;
      }
      res = RES_OK;

    return res;

  case SPI_FLASH :

    // Process of the command for the MMC/SD card

    return res;
  }

  return RES_PARERR;
}

__weak DWORD get_fattime(void) {
  /* return current timestamp */
  return    ((DWORD)(2015 - 1980) << 25)  /* Year 2015 */
      | ((DWORD)1 << 21)        /* Month 1 */
      | ((DWORD)1 << 16)        /* Mday 1 */
      | ((DWORD)0 << 11)        /* Hour 0 */
      | ((DWORD)0 << 5)          /* Min 0 */
      | ((DWORD)0 >> 1);         /* Sec 0 */
}

