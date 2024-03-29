
/*=========================================================================*/
/*   Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved.  */
/**
  @file
    i2c.c

  @brief
    The file contains i2c utilities

  @details


  @author WhiteBeard
  @author FabioPi

  @date
    2020-02-15
*/
/* Include files ================================================================================*/
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include "m2mb_types.h"
#include "m2mb_os_api.h"
#include "m2mb_fs_stdio.h"
#include "m2mb_i2c.h"
#include "m2mb_gpio.h"
#include "m2mb_lwm2m.h"
#include "m2mb_power.h"
#include "azx_log.h"
#include "azx_utils.h"
#include "app_cfg.h"

#include "i2c.h"
#include "dbg_utils.h"

/* Local defines ================================================================================*/

/* Local typedefs ===============================================================================*/
/* Local statics ================================================================================*/
static INT32 i2c_fd = -1;

/* Local function prototypes ====================================================================*/
/* Static functions =============================================================================*/
/* Global functions =============================================================================*/

/*-----------------------------------------------------------------------------------------------*/



int open_I2C( void )
{
  INT32 res;
  CHAR dev_ID[64];
  M2MB_I2C_CFG_T config;


  /**************
      Configuring the IIC device.
   **************/
  AZX_LOG_INFO( "\r\nConfiguring the Bosch BHI160 I2C channel...\r\n" );
  //Create device name using device address in decimal base, left shifted by 1 bit
  sprintf( dev_ID, "/dev/I2C-%d",
           ( UINT16 ) BHY_I2C_160B_ADDR1 << 1 ); //I2C API require an already left-shifted device address
  AZX_LOG_INFO( "Opening channel %s\r\n", dev_ID );
  i2c_fd = m2mb_i2c_open( dev_ID, 0 );

  if( -1 == i2c_fd )
  {
    AZX_LOG_ERROR( "cannot open I2C channel!\r\n" );
    return 1;
  }
  else
  {
    AZX_LOG_DEBUG("i2d_fd: %d\r\n", i2c_fd);
  }
  config.sclPin = I2C_SCL;
  config.sdaPin = I2C_SDA;
  config.registerId = 0x00; //dummy register
  res = m2mb_i2c_ioctl( i2c_fd, M2MB_I2C_IOCTL_SET_CFG, ( void * )&config );

  if( res != 0 )
  {
    AZX_LOG_ERROR( "cannot configure I2C channel\r\n" );
    return 1;
  }

  return 0;
}

/*-----------------------------------------------------------------------------------------------*/
int8_t sensor_i2c_write( uint8_t addr, uint8_t reg, uint8_t *p_buf, uint16_t size )
{
  ( void )addr;
  INT32 i2c_res;
  M2MB_I2C_CFG_T config;

  if( i2c_fd == -1 )
  {
    return 1;
  }


  i2c_res = m2mb_i2c_ioctl( i2c_fd, M2MB_I2C_IOCTL_GET_CFG, ( void * )&config );

  if( i2c_res != 0 )
  {
    AZX_LOG_ERROR( "cannot get I2C channel configuration on %d\r\n", i2c_fd );
    return 1;
  }

  config.registerId = reg;
  i2c_res = m2mb_i2c_ioctl( i2c_fd, M2MB_I2C_IOCTL_SET_CFG, ( void * )&config );

  if( i2c_res != 0 )
  {
    AZX_LOG_ERROR( "cannot set I2C channel configuration on %d\r\n", i2c_fd );
    return 1;
  }

  AZX_LOG_TRACE( "Configuring I2C Registers - Writing %d bytes into 0x%02X register...\r\n", size,
                 reg );
  if(getDebug() >= DBG_I2C_EVENTS)
  {
    AZX_LOG_INFO("before i2c_write\r\n");
  }
  i2c_res = m2mb_i2c_write( i2c_fd, p_buf, size );

  if(getDebug() >= DBG_I2C_EVENTS)
  {
    AZX_LOG_INFO("after i2c_write\r\n");
  }
  if( i2c_res != size )
  {
    AZX_LOG_ERROR( "cannot write data on %d! error: %d\r\n", i2c_fd, i2c_res );
    return 1;
  }
  else
  {
    AZX_LOG_TRACE( "Write: success\r\n" );
    return 0;
  }
}

/*-----------------------------------------------------------------------------------------------*/
int8_t sensor_i2c_read_16( uint8_t addr, uint8_t reg, uint8_t *p_buf, uint16_t size,
                           M2MB_I2C_CFG_T *pConfig )
{
  ( void )addr;

  for( int i = 0; size > 0; i++ )
  {
    pConfig->registerId = reg;
    INT32 i2c_res = m2mb_i2c_ioctl( i2c_fd, M2MB_I2C_IOCTL_SET_CFG, ( void * )pConfig );

    if( i2c_res != 0 )
    {
      AZX_LOG_ERROR( "Cannot set I2C channel configuration on %d\r\n", i2c_fd );
      return 1;
    }

    uint16_t tmpsize;

    if( size >= 16 )
    {
      tmpsize = 16;
    }
    else
    {
      tmpsize = size;
    }

    i2c_res = m2mb_i2c_read( i2c_fd, p_buf, tmpsize ); //reading 16 bytes max at a time

    if( i2c_res == tmpsize )
    {
      size = size - tmpsize;
      reg = reg + tmpsize;

      AZX_LOG_TRACE( "Reading Success.\r\n" );
      AZX_LOG_TRACE( "i2c->" );
      for( int i = 0; i < i2c_res; i++ )
      {
        AZX_LOG_TRACE( " %02x", p_buf[i] );
      }
      AZX_LOG_TRACE( "\r\n" );

      p_buf = p_buf + tmpsize;
    }
    else
    {
      AZX_LOG_ERROR( "Reading FAIL for %d bytes on register 0x%02X! - Exit Value: %d.\r\n", size, reg,
                     i2c_res );
      return 1;
    }
  }

  return 0;
}


/*-----------------------------------------------------------------------------------------------*/
int8_t sensor_i2c_read( uint8_t addr, uint8_t reg, uint8_t *p_buf, uint16_t size )
{
  INT32 i2c_res;
  M2MB_I2C_CFG_T config;
  if( i2c_fd == -1 )
  {
    return 1;
  }

  AZX_LOG_TRACE( "I2C read register..." );

  i2c_res = m2mb_i2c_ioctl( i2c_fd, M2MB_I2C_IOCTL_GET_CFG, ( void * )&config );

  if( i2c_res != 0 )
  {
    AZX_LOG_ERROR( "Cannot get I2C channel configuration on %d\r\n", i2c_fd );
    return 1;
  }

  if(getDebug() >= DBG_I2C_EVENTS)
  {
    AZX_LOG_INFO("before i2c_read\r\n");
  }
  if( reg == 0 ) // if read FIFO double buffer
  {
    for( ; size > 50; )
    {
      i2c_res = sensor_i2c_read_16( addr, reg, p_buf, 50, &config );
      size = size - 50;
      p_buf = p_buf + 50;
    }

    i2c_res += sensor_i2c_read_16( addr, reg, p_buf, size, &config );
  }
  else
  {
    i2c_res = sensor_i2c_read_16( addr, reg, p_buf, size, &config );
  }

  if(getDebug() >= DBG_I2C_EVENTS)
  {
    AZX_LOG_INFO("after i2c_read\r\n");
  }
  return i2c_res;
}


int close_I2C( void )
{
  INT32 res;

  AZX_LOG_INFO( "\r\nClosing the Bosch BHI160 I2C channel...\r\n" );

  res = m2mb_i2c_close( i2c_fd);

  if( -1 == res )
  {
    AZX_LOG_ERROR( "cannot close I2C channel!\r\n" );
    return 1;
  }
  else
  {
    i2c_fd = -1;
    return 0;
  }
}
