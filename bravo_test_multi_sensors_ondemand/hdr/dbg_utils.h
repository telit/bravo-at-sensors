/*Copyright (C) 2021 Telit Communications S.p.A. Italy - All Rights Reserved.*/
/*    See LICENSE file in the project root for full license information.     */

/**
  @file
    dbg_utils.h

  @brief
    The file contains common functions for debugging

  @author


  @date
    16/11/2021
*/

#ifndef HDR_DBG_UTILS_H_
#define HDR_DBG_UTILS_H_

enum
{
  DBG_NONE = 0,
  DBG_STATE_MACHINE = 1,
  DBG_I2C_EVENTS = 2
};

UINT32 getDebug(void);
void setDebug(UINT32 dbg);

#endif /* HDR_DBG_UTILS_H_ */
