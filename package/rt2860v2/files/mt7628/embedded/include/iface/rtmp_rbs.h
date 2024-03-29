/****************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 * (c) Copyright 2002, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ****************************************************************************

    Module Name:
	rtmp_rbs.h

    Abstract:
 	Ralink SoC Internal Bus related definitions and data dtructures

    Revision History:
    Who          When          What
    ---------    ----------    ----------------------------------------------
 */

#ifndef __RTMP_RBUS_H__
#define __RTMP_RBUS_H__

#include "iface/rtmp_inf_pcirbs.h"

/*************************************************************************
  *
  *	Device hardware/ Interface related definitions.
  *
  ************************************************************************/

#define RTMP_MAC_IRQ_NUM		6 //SURFBOARDINT_WLAN


/*************************************************************************
  *
  *	EEPROM Related definitions
  *
  ************************************************************************/








#if defined (CONFIG_RT2880_FLASH_32M)
#define MTD_NUM_FACTORY 5
#else
#define MTD_NUM_FACTORY 2
#endif


#ifndef RALINK_SYSCTL_BASE
#define RALINK_SYSCTL_BASE			0xb0000000
#endif


#ifdef LINUX
/*************************************************************************
  *
  *	Device Tx/Rx related definitions.
  *
  ************************************************************************/
#ifdef DFS_SUPPORT
/* TODO: Check these functions. */
#ifdef RTMP_RBUS_SUPPORT
extern void unregister_tmr_service(void);
extern void request_tmr_service(int, void *, void *);
#endif /* RTMP_RBUS_SUPPORT */

#endif /* DFS_SUPPORT */

#endif /* LINUX */

#endif /* __RTMP_RBUS_H__ */

