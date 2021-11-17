/*Copyright (C) 2020 Telit Communications S.p.A. Italy - All Rights Reserved.*/
/*    See LICENSE file in the project root for full license information.     */

/**
  @file
    at_hash_BSENSDEBUG.c

  @brief
    The file contains the implementation of custom command AT#BSENSDEBUG

  @author


  @date
    13/03/2020
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "m2mb_types.h"
#include "m2mb_os_api.h"
#include "m2mb_atp.h"

#include "azx_log.h"
#include "app_cfg.h"

#include "azx_easy_at.h"

#include "at_common.h"
#include "dbg_utils.h"

extern M2MB_OS_TASK_HANDLE Task_h;


void BSENSDEBUG_AT_Callback( M2MB_ATP_HANDLE atpHandle, UINT16 atpI )
{
  M2MB_ATP_PARAM_T *atpParam;
  char rsp_buf[256];
  char *p_param = NULL;
  UINT32 log;
  AZX_EASY_AT_HANDLES_T hdls;
  m2mb_atp_get_input_data( atpHandle, atpI, &atpParam );
  hdls.handle = atpHandle;
  hdls.atpI = atpI;

  if( NULL == atpHandle )
  {
    AZX_LOG_ERROR( "atpHandle is NULL\r\n" );
  }

  AZX_EASY_AT_CMD_INFO( &hdls );

  switch( atpParam->type )
  {
    /*AT#BSENSDEBUG=<param><CR>*/
    case M2MB_ATP_CMDTYP_SET:
      if( atpParam->itemNum < 1 )
      {
        AZX_LOG_ERROR( "Operation not allowed!\r\n" );
        AZX_EASY_AT_RELEASE_WITH_CMEE( &hdls,  M2MB_ATP_CME_OPERATION_NOT_ALLOWED, NULL );
        return;
      }

      azx_easy_at_TrimAndRemoveQuotes( atpParam->item[0] );
      p_param = atpParam->item[0];
      if(strlen(p_param) > 0)
      {
        if (0 !=azx_easy_at_strToUL(p_param, &log))
        {
          AZX_LOG_ERROR("Cannot parse parameter, not allowed!\r\n" );
          /*Release AT instance with failure*/
          AZX_EASY_AT_RELEASE_WITH_CMEE( &hdls, M2MB_ATP_CME_OPERATION_NOT_ALLOWED, NULL );
          break;
        }

      // AZX_LOG_LEVEL_TRACE    = 1,    /**< Prints every message, adds a "TRACE" at the beginning of the message*/
      // AZX_LOG_LEVEL_DEBUG    = 2,    /**< Prints most of the messages, adds a "DEBUG" at the beginning of the message*/
      // AZX_LOG_LEVEL_INFO     = 3,    /**< print the message only, without any additional info */
      // AZX_LOG_LEVEL_WARN     = 4,    /**< like DEBUG, but adds a "WARNING" at the beginning of the message. */
      // AZX_LOG_LEVEL_ERROR    = 5,    /**< like DEBUG, but adds an "ERROR" at the beginning of the message */
      // AZX_LOG_LEVEL_CRITICAL = 6,    /**< like DEBUG, but adds an "CRITICAL" at the beginning of the message */
      // AZX_LOG_LEVEL_NONE     = 0x7F, /**< Do not print anything */

        if(log > AZX_LOG_LEVEL_CRITICAL)
        {
          AZX_LOG_ERROR("parameter out of range, not allowed!\r\n" );
          /*Release AT instance with failure*/
          AZX_EASY_AT_RELEASE_WITH_CMEE( &hdls, M2MB_ATP_CME_OPERATION_NOT_ALLOWED, NULL );
          break;
        }
        if (log == 0) /*remap 0 to NONE*/
        {
          log = AZX_LOG_LEVEL_NONE;
        }
        AZX_LOG_DEBUG("Setting Logging to %u...\r\n", log);
        azx_log_setLevel((AZX_LOG_LEVEL_E)log);
      }
      if( atpParam->itemNum > 1 )
      {
        azx_easy_at_TrimAndRemoveQuotes( atpParam->item[1] );
        p_param = atpParam->item[1];

        if (0 !=azx_easy_at_strToUL(p_param, &log))
        {
          AZX_LOG_ERROR("Cannot parse parameter, not allowed!\r\n" );
          /*Release AT instance with failure*/
          AZX_EASY_AT_RELEASE_WITH_CMEE( &hdls, M2MB_ATP_CME_OPERATION_NOT_ALLOWED, NULL );
          break;
        }
        else
        {
          AZX_LOG_DEBUG("Setting debug to %u...\r\n", log);
          setDebug(log);
        }
      }
      AZX_EASY_AT_RELEASE_WITH_SUCCESS( &hdls );
      break;

    /*AT#BSENSDEBUG?<CR>*/
    case M2MB_ATP_CMDTYP_READ:
      log = azx_log_getLevel();
      if (log == AZX_LOG_LEVEL_NONE)
      {
        log = 0; /*remap*/
      }
      memset(rsp_buf, 0, sizeof(rsp_buf));
      snprintf( rsp_buf, sizeof(rsp_buf) -1, "%s: %u,%u",
               atpParam->atpCmdString,
               log, getDebug()
               );
      m2mb_atp_msgout( atpHandle, atpI, rsp_buf );
      AZX_EASY_AT_RELEASE_WITH_SUCCESS( &hdls );
      break;

    /*AT#BSENSDEBUG=?<CR>*/
    case M2MB_ATP_CMDTYP_TEST:
      memset(rsp_buf, 0, sizeof(rsp_buf));
      snprintf( rsp_buf, sizeof(rsp_buf) -1, "%s: [0-%d]",
               atpParam->atpCmdString,
               AZX_LOG_LEVEL_CRITICAL
               );
      m2mb_atp_msgout( atpHandle, atpI, rsp_buf );
      AZX_EASY_AT_RELEASE_WITH_SUCCESS( &hdls );
      break;

    /*AT#BSENSDEBUG<CR>*/
    case M2MB_ATP_CMDTYP_EXEC:
    {
      CHAR *getName;
      M2MB_OS_TASK_STATE_E taskState;
      // or simply get the pointer of name, with NULL
      m2mb_os_taskGetItem(Task_h, M2MB_OS_TASK_SEL_CMD_USRNAME, (MEM_W*)&getName, NULL);
      m2mb_os_taskGetItem(Task_h, M2MB_OS_TASK_SEL_CMD_STATE, (MEM_W*)&taskState, NULL);
      memset(rsp_buf, 0, sizeof(rsp_buf));
      snprintf( rsp_buf, sizeof(rsp_buf) -1, "%s: %s: %d",
               atpParam->atpCmdString,
               getName, taskState
               );
      m2mb_atp_msgout( atpHandle, atpI, rsp_buf );
      AZX_EASY_AT_RELEASE_WITH_SUCCESS( &hdls );
    }
      break;

    /*AT#BSENSDEBUG=<CR>*/
    case M2MB_ATP_CMDTYP_NOPARAMS:
      /*Release AT instance with failure*/
      AZX_EASY_AT_RELEASE_WITH_CMEE( &hdls, M2MB_ATP_CME_OPERATION_NOT_ALLOWED, NULL );
      break;

    default:
      /*Release AT instance with failure*/
      AZX_EASY_AT_RELEASE_WITH_CMEE( &hdls, M2MB_ATP_CME_OPERATION_NOT_ALLOWED, NULL );
      break;
  } // end switch
}
