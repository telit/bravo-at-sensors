/*Copyright (C) 2021 Telit Communications S.p.A. Italy - All Rights Reserved.*/
/*    See LICENSE file in the project root for full license information.     */

/**
  @file
    dbg_utils.c

  @brief
    The file contains the common functions for debugging

  @author


  @date
    16/11/2021
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "m2mb_types.h"
#include "m2mb_os_types.h"
#include "m2mb_os.h"

#include "azx_log.h"

static volatile UINT32 gDEBUG = 0;


UINT32 getDebug(void)
{
  return gDEBUG;
}

void setDebug(UINT32 dbg)
{
  gDEBUG = dbg;
}