/*Copyright (C) 2020 Telit Communications S.p.A. Italy - All Rights Reserved.*/
/*    See LICENSE file in the project root for full license information.     */

/**
  @file
    at_hash_MYCMD.c

  @brief
    The file contains the implementation of custom command AT#MYCMD

  @author


  @date
    13/03/2020
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "m2mb_types.h"
#include "m2mb_os_api.h"

#include "m2mb_socket.h"


#include "m2mb_atp.h"

#include "azx_log.h"
#include "app_cfg.h"

#include "azx_easy_at.h"

#include "at_common.h"

#include "sensors_demo.h"

void BSENS_AT_Callback( M2MB_ATP_HANDLE atpHandle, UINT16 atpI )
{
  M2MB_ATP_PARAM_T *atpParam;
  char rsp_buf[128];
  char *p_param = NULL;
  AZX_EASY_AT_HANDLES_T hdls;
  m2mb_atp_get_input_data( atpHandle, atpI, &atpParam );
  hdls.handle = atpHandle;
  hdls.atpI = atpI;

  void * pdata = NULL;

  if( NULL == atpHandle )
  {
    AZX_LOG_ERROR( "atpHandle is NULL\r\n" );
  }

  AZX_EASY_AT_CMD_INFO( &hdls );

  switch( atpParam->type )
  {
    /*AT#MYCMD=<param><CR>*/
    case M2MB_ATP_CMDTYP_SET:
      if( atpParam->itemNum < 1 )
      {
        AZX_LOG_ERROR( "Operation not allowed!\r\n" );
        AZX_EASY_AT_RELEASE_WITH_CMEE( &hdls,  M2MB_ATP_CME_OPERATION_NOT_ALLOWED, NULL );
        return;
      }

      UINT32 sens_id;
      azx_easy_at_TrimAndRemoveQuotes( atpParam->item[0] );
      p_param = atpParam->item[0];

      if (0 !=azx_easy_at_strToUL(p_param, &sens_id))
      {
        AZX_LOG_ERROR("Cannot parse parameter not allowed!\r\n" );
        /*Release AT instance with failure*/
        AZX_EASY_AT_RELEASE_WITH_CMEE( &hdls, M2MB_ATP_CME_OPERATION_NOT_ALLOWED, NULL );
        break;
      }

      AZX_LOG_DEBUG("reading id %u...\r\n", sens_id);
//TODO check range
      read_sensor((BSENS_SENSOR_ID_E)sens_id, (void**) &pdata);

      switch(sens_id)
      {
      case BSENS_SENSOR_ENVIRONM_ID:
      {
        BSENS_ENV_T  *pENVIRON_data = (BSENS_ENV_T *) pdata;
        sprintf( rsp_buf, "%s: %d,%.02f,%.02f,%.02f,%d",
                               atpParam->atpCmdString,
                               sens_id,
                               pENVIRON_data->temperature,
                               pENVIRON_data->humidity,
                               pENVIRON_data->pressure,
                               pENVIRON_data->airQ
                );
      }
        break;

      case BSENS_SENSOR_3D_VECT_ID:
      {
        BSENS_3DVECT_T *p3DVECT_data = (BSENS_3DVECT_T *) pdata;
        sprintf( rsp_buf, "%s: %d,%.04f,%.04f,%.04f,%.04f,%d",
                       atpParam->atpCmdString,
                       sens_id,
                       p3DVECT_data->w,
                       p3DVECT_data->x,
                       p3DVECT_data->y,
                       p3DVECT_data->z,
                       p3DVECT_data->accuracy

        );
      }
        break;

      case BSENS_SENSOR_TAMPER_ID:
      {
        BSENS_TAMPER_T *pTAMPER_data = (BSENS_TAMPER_T *) pdata;
        read_sensor((BSENS_SENSOR_ID_E)sens_id, (void**) &pTAMPER_data);
        sprintf( rsp_buf, "%s: %d,%d",
                               atpParam->atpCmdString,
                               sens_id,
                               pTAMPER_data->status

                );
      }
        break;

      default:
        break;
      }


      m2mb_atp_msgout( atpHandle, atpI, rsp_buf );
      AZX_EASY_AT_RELEASE_WITH_SUCCESS( &hdls );
      break;

    /*AT#MYCMD?<CR>*/
    case M2MB_ATP_CMDTYP_READ:
      /*Release AT instance with failure*/
      AZX_EASY_AT_RELEASE_WITH_CMEE( &hdls, M2MB_ATP_CME_OPERATION_NOT_ALLOWED, NULL );
      break;

    /*AT#MYCMD=?<CR>*/
    case M2MB_ATP_CMDTYP_TEST:
      sprintf( rsp_buf, "%s: %d-%d",
               atpParam->atpCmdString,
               BSENS_SENSOR_ENVIRONM_ID,
               BSENS_SENSOR_TAMPER_ID
               );
      m2mb_atp_msgout( atpHandle, atpI, rsp_buf );
      AZX_EASY_AT_RELEASE_WITH_SUCCESS( &hdls );
      break;

    /*AT#MYCMD<CR>*/
    case M2MB_ATP_CMDTYP_EXEC:
      /*Release AT instance with failure*/
      AZX_EASY_AT_RELEASE_WITH_CMEE( &hdls, M2MB_ATP_CME_OPERATION_NOT_ALLOWED, NULL );
      break;

    /*AT#MYCMD=<CR>*/
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
