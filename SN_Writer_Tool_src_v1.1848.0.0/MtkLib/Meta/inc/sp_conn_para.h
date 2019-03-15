#ifndef __SP_CONN_PARA__
#define __SP_CONN_PARA__

#include "meta.h"
#include "conn_para.h"
#include "ActionFlowPara.h" 

typedef struct
{
	bool                   BootModeFlag;       /**< [META] Decide target will boot meta mode or normal mode. 0: normal mode, need preloader handshake META 1:already meta mode boot */
    bool                   usb_auto_detec;     /**< [META] USB auto detection selection */
    bool                   secure_boot;        /**< [META] Selection of secure boot */
    bool                   send_at_cmd_to_connect_md_meta_mode;
	bool                   con_sp_detect_booting_completed;
	SP_AT_CMD_SETTING_T    at_cmd_setting;
} SP_Conn_Para;
typedef struct
{
	unsigned int           MDloggingType;        /**< [META] MDlogger logging, 0: disable logging. 0x01: USB. 0x02: SD card. DHL meta trace, 0x10: relay to ELT. 0x20: dump to file. */
	unsigned int           APloggingType;        /**< [META] Mobilelog logging, 0: disable logging. 0x01: USB. 0x02: SD card. */
	wchar_t*               MDlogFilePath;
    wchar_t*               APlogFilePath;
} SP_Logging_Para;

typedef struct
{
    Conn_Para input_para;
    SP_Conn_Para sp_input_para;
    SP_BOOT_ARG_S* sp_boot_arg;
    int* boot_stop;
    bool c2k_log_relay_enable;
} SP_Conn_Input;

typedef struct
{
    bool b_shutdown;
    bool b_backup;
	bool b_switchFlag;
	bool b_bootFlag;
    bool b_sendAtCmdFlag;
	char* apdb_pathname;
	SP_Logging_Para disLog_para;
} SP_Disconn_Input;

#endif