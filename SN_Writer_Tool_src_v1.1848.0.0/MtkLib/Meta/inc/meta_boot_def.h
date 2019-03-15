/*******************************************************************************
 *  Copyright Statement:
 *  --------------------
 *  This software is protected by Copyright and the information contained
 *  herein is confidential. The software may not be copied and the information
 *  contained herein may not be used or disclosed except with the written
 *  permission of MediaTek Inc. (C) 2003
 *
 ******************************************************************************/

/*******************************************************************************
 * Filename:
 * ---------
 *  meta_boot_def.h
 *
 * Project:
 * --------
 *  Legacy Boot Define
 *
 * Description:
 * ------------
 *  BootRom define for feature phone.
 *  It's maintain META_Connect_Req, META_Connect_Ex_Req and META_Connect_Report 
 *
 * Author:
 * -------
 *  Victoria Wang
 *
 *******************************************************************************/
#ifndef _BOOT_DEF_H_
#define _BOOT_DEF_H_

#if defined(_WIN32)
#include <windows.h>
#elif defined(__GNUC__)
    typedef int HANDLE;
    #include <stddef.h>
#endif


#include "mtk_mcu.h"
//#include "external_memory_setting.h"
//#include "flashtool_handle.h"

#ifdef	__cplusplus
extern "C" {
#endif

// The magic value to stop boot process
#define BOOT_STOP	9876

// boot infinitely
#define BOOT_INFINITE	0xFFFFFFF

//------------------------------------------------------------------------------

struct FAUTH_HANDLE;
typedef struct FAUTH_HANDLE * FAUTH_HANDLE_T;
struct FSCERT_HANDLE;
typedef struct FSCERT_HANDLE * FSCERT_HANDLE_T;
struct FROOTCERT_HANDLE;
typedef struct FROOTCERT_HANDLE * FROOTCERT_HANDLE_T;

typedef int FBOOT_STATUS;

//------------------------------------------------------------------------------
// prototype of COM initial stage callback
//------------------------------------------------------------------------------
typedef int (__stdcall *FCALLBACK_COM_INIT_STAGE)(void *hCOM, void *usr_arg);

//------------------------------------------------------------------------------
// prototype of BootROM stage callback
//------------------------------------------------------------------------------
typedef int (__stdcall *FCALLBACK_IN_BROM_STAGE)(unsigned int brom_handle, void *hCOM, void *usr_arg);

//------------------------------------------------------------------------------
// prototype of write buf progress callback
//------------------------------------------------------------------------------
typedef int (__stdcall *FCALLBACK_WRITE_BUF_PROGRESS_INIT)(void *usr_arg);
typedef int (__stdcall *FCALLBACK_WRITE_BUF_PROGRESS)(unsigned char finished_percentage, unsigned int finished_bytes, unsigned int total_bytes, void *usr_arg);

//------------------------------------------------------------------------------
// prototype of Serial-Link-Authentication challenge callback
//------------------------------------------------------------------------------
typedef int (__stdcall *FCALLBACK_SLA_CHALLENGE)(void *usr_arg, const unsigned char  *p_challenge_in, unsigned int  challenge_in_len, unsigned char  **pp_challenge_out, unsigned int  *p_challenge_out_len);
typedef int (__stdcall *FCALLBACK_SLA_CHALLENGE_END)(void *usr_arg, unsigned char  *p_challenge_out);


//------------------------------------------------------------------------------
// boot-up result
//------------------------------------------------------------------------------
typedef struct {
	int      		m_bbchip_type;
	char			m_bbchip_name[32];
	unsigned short	m_bbchip_hw_ver;
	unsigned short	m_bbchip_sw_ver;
	unsigned short	m_bbchip_hw_code;
	int		        m_ext_clock;
	unsigned char	m_bbchip_secure_ver;
	unsigned char	m_bbchip_bl_ver;
	unsigned int	m_fw_ver_len;
	char			m_fw_ver[64];

	unsigned char	m_msp_err_code;
    unsigned int    m_security_config;

    unsigned char	m_meid[16];
    bool            m_isTinySystemChip;
} FBOOT_RESULT;

//------------------------------------------------------------------------------
// boot META mode
//------------------------------------------------------------------------------
typedef struct {
	int     		m_bbchip_type;
	int     		m_ext_clock;
	unsigned int	m_ms_boot_timeout;
	unsigned int	m_max_start_cmd_retry_count;
	FCALLBACK_COM_INIT_STAGE		m_cb_com_init_stage;
	void *						m_cb_com_init_stage_arg;
	FCALLBACK_IN_BROM_STAGE		m_cb_in_brom_stage;
	void *						m_cb_in_brom_stage_arg;
	int	m_speedup_brom_baudrate;

#if defined(_WIN32)
	HWND	m_ready_power_on_wnd_handle;
	void *	m_ready_power_on_wparam;
	void *	m_ready_power_on_lparam;
#endif
	FAUTH_HANDLE_T					m_auth_handle;
	FCALLBACK_SLA_CHALLENGE			m_cb_sla_challenge;
	void *							m_cb_sla_challenge_arg;
	FCALLBACK_SLA_CHALLENGE_END		m_cb_sla_challenge_end;
	void *							m_cb_sla_challenge_end_arg;
	FSCERT_HANDLE_T					m_scert_handle;
	int							m_usb_enable;
    int               m_rootcert_force_assign;
    FROOTCERT_HANDLE_T   m_rootcert_handle;
	int               m_enable_without_battery;
    int               m_enable_AP_META_mode;

} FBOOT_META_ARG;

#ifdef	__cplusplus
}
#endif

#endif