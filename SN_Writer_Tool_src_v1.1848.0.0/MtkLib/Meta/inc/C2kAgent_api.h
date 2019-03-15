#ifndef _C2KAGENT_API_H_
#define _C2KAGENT_API_H_

#include "meta.h"
#include "C2kAgent_api_datatype.h"
#ifdef __cplusplus
extern "C" {
#endif

/*************************** Functions Declaration ****************************/
/**
 * \ingroup C2K
 * \details Get DLL version
 * \param [out] major_ver pointer to major version
 * \param [out] minor_ver pointer to minor version
 * \param [out] build_num pointer to build number
 * \param [out] patch_num pointer to patch number
 * \retval 0: successful
 */
int __stdcall META_C2K_GetDLLVer(unsigned int* major_ver, unsigned int* minor_ver, unsigned int* build_num, unsigned int* patch_num);
/**
 * \ingroup C2K
 * \details Get last error string
 * \param [in] metaHandle Context handle to specific DUT
 * \retval pointer to error string
 */
const char* __stdcall META_C2K_GetErrorString(const int metaHandle);
/**
 * \ingroup C2K
 * \details Library initialization(ANSI character version)
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] libConfig pointer to C2K_LIBCONFIGPARMS structure
 * \param [in] phoneAttr pointer to PHONEATTRIBUTE_PARMS structure, not used, set to NULL
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_Init(const int metaHandle, C2K_LIBCONFIGPARMS* libConfig, PHONEATTRIBUTE_PARMS* phoneAttr);
/**
 * \ingroup C2K
 * \details Library initialization(wide character version)
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] libConfig pointer to C2K_LIBCONFIGPARMS structure
 * \param [in] phoneAttr pointer to PHONEATTRIBUTE_PARMS structure, not used, set to NULL
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_InitW(const int metaHandle, C2K_LIBCONFIGPARMSW* libConfig, PHONEATTRIBUTE_PARMS* phoneAttr);
/**
 * \ingroup C2K
 * \details Library deinitialization
 * \param [in] metaHandle Context handle to specific DUT
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_DeInit(const int metaHandle);
/**
 * \ingroup C2K
 * \sa non-reentrant version of META_C2K_ConnectWithTarget_r
 */
META_RESULT __stdcall META_C2K_ConnectWithTarget(unsigned int msTimeout, C2K_CONNECT_PARAMS* connectReq);
/**
 * \ingroup C2K
 * \details Connect to target
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] connectReq pointer to C2K_CONNECT_PARAMS structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_ConnectWithTarget_r(const int metaHandle, unsigned int msTimeout, C2K_CONNECT_PARAMS* connectReq);
/**
 * \ingroup C2K
 * \sa non-reentrant version of META_C2K_DisconnectWithTarget_r
 */
META_RESULT __stdcall META_C2K_DisconnectWithTarget(void);
/**
 * \ingroup C2K
 * \details Disconnect from target
 * \param [in] metaHandle Context handle to specific DUT
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_DisconnectWithTarget_r(const int metaHandle);
/**
 * \ingroup C2K
 * \details Not used
 */
META_RESULT __stdcall META_C2K_LoadMessage(char* PathName);
/**
 * \ingroup C2K
 * \details Not used
 */
META_RESULT __stdcall META_C2K_LoadMessage_r(const int metaHandle, char* PathName);
/**
 * \ingroup C2K
 * \sa non-reentrant version of META_C2K_LoadLogFilter_r
 */
META_RESULT __stdcall META_C2K_LoadLogFilter(char* PathName);
/**
 * \ingroup C2K
 * \details Load log filter from file and set to target(ANSI character version)
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] PathName pointer to the null-terminated file name string
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_LoadLogFilter_r(const int metaHandle, char* PathName);
/**
 * \ingroup C2K
 * \details Load log filter from file and set to target(wide character version)
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] PathName pointer to the null-terminated file name string
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_LoadLogFilter_rW(const int metaHandle, const wchar_t* PathName);
/**
 * \ingroup C2K
 * \details Get RF chip name from ID value
 * \param [in] id RF chip ID value
 * \retval pointer to RF chip name string
 */
const char* __stdcall META_C2K_GetRfChipIdName(unsigned int id);

/**
 * \ingroup C2K
 * \sa non-reentrant version of META_C2K_SetSpi_r
 */
META_RESULT __stdcall META_C2K_SetSpi(unsigned int msTimeout, C2K_SPI_TYPE spiType, unsigned int spiAddress, unsigned int spiData);
/**
 * \ingroup C2K
 * \details Set SPI data to target
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] spiType SPI path type, value of enum type C2K_SPI_TYPE
 * \param [in] spiAddress SPI address
 * \param [in] spiData SPI data
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_SetSpi_r(const int metaHandle, unsigned int msTimeout, C2K_SPI_TYPE spiType, unsigned int spiAddress, unsigned int spiData);
/**
 * \ingroup C2K
 * \sa non-reentrant version of META_C2K_GetSpi_r
 */
META_RESULT __stdcall META_C2K_GetSpi(unsigned int msTimeout, C2K_SPI_TYPE spiType, unsigned int spiAddress, unsigned int* spiData);
/**
 * \ingroup C2K
 * \details Get SPI data from target
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] spiType SPI path type, value of enum type C2K_SPI_TYPE
 * \param [in] spiAddress SPI address
 * \param [out] spiData pointer to SPI data
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_GetSpi_r(const int metaHandle, unsigned int msTimeout, C2K_SPI_TYPE spiType, unsigned int spiAddress, unsigned int* spiData);
/**
 * \ingroup C2K
 * \details Not used
 */
META_RESULT __stdcall META_C2K_LogIq(unsigned int msTimeout, LOG_IQ_REQ* req);
/**
 * \ingroup C2K
 * \details Not used
 */
META_RESULT __stdcall META_C2K_LogIq_r(const int metaHandle, unsigned int msTimeout, LOG_IQ_REQ* req);
/**
 * \ingroup C2K
 * \sa non-reentrant version of META_C2K_GetTemperatureAdc_r
 */
META_RESULT __stdcall META_C2K_GetTemperatureAdc(unsigned int msTimeout, unsigned short* adcValue);
/**
 * \ingroup C2K
 * \details Get temperature sensor ADC value from target
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [out] adcValue pointer to ADC value
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_GetTemperatureAdc_r(const int metaHandle, unsigned int msTimeout, unsigned short* adcValue);
/**
 * \ingroup C2K
 * \sa non-reentrant version of META_C2K_TransmitterCtrl_r
 */
META_RESULT __stdcall META_C2K_TransmitterCtrl(unsigned int msTimeout, bool enable, C2K_RF_MODE rfMode, unsigned int band, unsigned short channel);
/**
 * \ingroup C2K
 * \details Transmitter control
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] enable true: TX on, false: TX off
 * \param [in] rfMode RF mode, 0: 1xRTT, 1: EVDO
 * \param [in] band C2K band class
 * \param [in] channel channel number
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_TransmitterCtrl_r(const int metaHandle, unsigned int msTimeout, bool enable, C2K_RF_MODE rfMode, unsigned int band, unsigned short channel);
/**
 * \ingroup C2K
 * \sa non-reentrant version of META_C2K_ReceiverCtrl_r
 */
META_RESULT __stdcall META_C2K_ReceiverCtrl(unsigned int msTimeout, bool enable, C2K_RF_MODE rfMode, unsigned int rxPathBitmap, unsigned int band, unsigned short channel);
/**
 * \ingroup C2K
 * \details Receiver control
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] enable true: RX on, false: RX off
 * \param [in] rfMode RF mode, 0: 1xRTT, 1: EVDO
 * \param [in] rxPathBitmap RX path bitmap, bit0: main path, bit1: diversity path, bit2: second path
 * \param [in] band C2K band class
 * \param [in] channel channel number
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_ReceiverCtrl_r(const int metaHandle, unsigned int msTimeout, bool enable, C2K_RF_MODE rfMode, unsigned int rxPathBitmap, unsigned int band, unsigned short channel);
/**
 * \ingroup C2K
 * \sa non-reentrant version of META_C2K_AfcConfig_r
 */
META_RESULT __stdcall META_C2K_AfcConfig(unsigned int msTimeout, C2K_RF_MODE rfMode, unsigned int ctrlMode, bool capIdValid, unsigned short capId, bool dacValid, unsigned short initDac);
/**
 * \ingroup C2K
 * \details Configure AFC parameters
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] rfMode RF mode, 0: 1xRTT, 1: EVDO
 * \param [in] ctrlMode 0: auto mode, 1: manual mode
 * \param [in] capIdValid indicate if capId value is used by target
 * \param [in] capId CAP ID
 * \param [in] dacValid indicate if initDac value is used by target
 * \param [in] initDac initial AFC DAC
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_AfcConfig_r(const int metaHandle, unsigned int msTimeout, C2K_RF_MODE rfMode, unsigned int ctrlMode, bool capIdValid, unsigned short capId, bool dacValid, unsigned short initDac);
/**
 * \ingroup C2K
 * \details Not used
 */
META_RESULT __stdcall META_C2K_AfcGetParam(unsigned int msTimeout, C2K_RF_MODE rfMode, unsigned short* capId, unsigned short* initDac);
/**
 * \ingroup C2K
 * \details Not used
 */
META_RESULT __stdcall META_C2K_AfcGetParam_r(const int metaHandle, unsigned int msTimeout, C2K_RF_MODE rfMode, unsigned short* capId, unsigned short* initDac);
/**
 * \ingroup C2K
 * \sa non-reentrant version of META_C2K_TxTone_r
 */
META_RESULT __stdcall META_C2K_TxTone(unsigned int msTimeout, bool enable, unsigned int* frequencyHz);
/**
 * \ingroup C2K
 * \details Transmit continuous wave signal
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] enable true: TX on, false: TX off
 * \param [in,out] frequencyHz TX frequency in Hz
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_TxTone_r(const int metaHandle, unsigned int msTimeout, bool enable, unsigned int* frequencyHz);
/**
 * \ingroup C2K
 * \sa non-reentrant version of META_C2K_TxCdma_r
 */
META_RESULT __stdcall META_C2K_TxCdma(unsigned int msTimeout, bool enable, C2K_RF_MODE rfMode);
/**
 * \ingroup C2K
 * \details Transmit CDMA modulation signal
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] enable true: TX on, false: TX off
 * \param [in] rfMode RF mode, 0: 1xRTT, 1: EVDO
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_TxCdma_r(const int metaHandle, unsigned int  msTimeout, bool enable, C2K_RF_MODE rfMode);
/**
 * \ingroup C2K
 * \sa non-reentrant version of META_C2K_TxAgcConfig_r
 */
META_RESULT __stdcall META_C2K_TxAgcConfig(unsigned int msTimeout, TXAGC_CONFIG_REQ* req);
/**
 * \ingroup C2K
 * \details Configure TX signal parameters
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] req pointer to TXAGC_CONFIG_REQ structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_TxAgcConfig_r(const int metaHandle, unsigned int msTimeout, TXAGC_CONFIG_REQ* req);
/**
 * \ingroup C2K
 * \sa non-reentrant version of META_C2K_ReadCurrentTxPower_r
 */
META_RESULT __stdcall META_C2K_ReadCurrentTxPower(unsigned int msTimeout, short* pdPower);
/**
 * \ingroup C2K
 * \details Read current power detector measurement value
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [out] pdPower pointer to power detector value
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_ReadCurrentTxPower_r(const int metaHandle, unsigned int msTimeout, double* pdPower);
/**
 * \ingroup C2K
 * \sa non-reentrant version of META_C2K_RxAgcSetGain_r
 */
META_RESULT __stdcall META_C2K_RxAgcSetGain(unsigned int msTimeout, unsigned int ctrlMode, C2K_RF_MODE rfMode, unsigned int rxPathBitmap, unsigned int lnaMode, double cellPower);
/**
 * \ingroup C2K
 * \details Set RX AGC gain
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] ctrlMode control mode, 0: auto mode, 1: manual mode
 * \param [in] rfMode RF mode, 0: 1xRTT, 1: EVDO
 * \param [in] rxPathBitmap RX path bitmap, bit0: main path, bit1: diversity path, bit2: second path
 * \param [in] lnaMode LNA mode index
 * \param [in] cellPower input cell power level, not used, ignored by target
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_RxAgcSetGain_r(const int metaHandle, unsigned int msTimeout, unsigned int ctrlMode, C2K_RF_MODE rfMode, unsigned int rxPathBitmap, unsigned int lnaMode, double cellPower);
/**
 * \ingroup C2K
 * \sa non-reentrant version of META_C2K_FhcStart_r
 */
META_RESULT __stdcall META_C2K_FhcStart(unsigned int msTimeout, C2K_FHC_REQ* fhcReq, C2K_FHC_CNF* fhcCnf);
/**
 * \ingroup C2K
 * \details Start FHC command and wait for confirm
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] fhcReq pointer to C2K_FHC_REQ structure
 * \param [out] fhcCnf pointer to C2K_FHC_CNF structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_FhcStart_r(const int metaHandle, unsigned int msTimeout, C2K_FHC_REQ* fhcReq, C2K_FHC_CNF* fhcCnf);
/**
 * \ingroup C2K
 * \sa non-reentrant version of META_C2K_EnableRxTxSpy_r
 */
META_RESULT __stdcall META_C2K_EnableRxTxSpy(unsigned int msTimeout, bool enable);
/**
 * \ingroup C2K
 * \details Enable/disable RX/TX spy message
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] enable true: enabled, false: disabled
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_EnableRxTxSpy_r(const int metaHandle, unsigned int msTimeout, bool enable);
/**
 * \ingroup C2K
 * \sa non-reentrant version of META_C2K_Rssi_r
 */
META_RESULT __stdcall META_C2K_Rssi(unsigned int msTimeout, int rssiCount, C2K_RSSI_CNF* rssiCnf);
/**
 * \ingroup C2K
 * \details Get RSSI value
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] rssiCount RSSI test count
 * \param [out] rssiCnf pointer to C2K_RSSI_CNF structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_Rssi_r(const int metaHandle, unsigned int msTimeout, int rssiCount, C2K_RSSI_CNF* rssiCnf);
/**
 * \ingroup C2K
 * \sa non-reentrant version of META_C2K_QueryTargetCapability_r
 */
META_RESULT __stdcall META_C2K_QueryTargetCapability(unsigned int msTimeout, C2K_MS_CAPABILITY* msCapability);
/**
 * \ingroup C2K
 * \details Query target capability
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [out] msCapability pointer to C2K_MS_CAPABILITY structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_QueryTargetCapability_r(const int metaHandle, unsigned int msTimeout, C2K_MS_CAPABILITY* msCapability);
/**
 * \ingroup C2K
 * \sa non-reentrant version of META_C2K_PsPower_r
 */
META_RESULT __stdcall META_C2K_PsPower(unsigned int msTimeout, bool enable);
/**
 * \ingroup C2K
 * \details Protocol stack power up or down
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] enable true: power up, false: power down
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_PsPower_r(const int metaHandle, unsigned int msTimeout, bool enable);
/**
 * \ingroup C2K
 * \sa non-reentrant version of META_C2K_MPATestRelease_r
 */
META_RESULT __stdcall META_C2K_MPATestRelease(unsigned int msTimeout);
/**
 * \ingroup C2K
 * \details Release MPA
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_MPATestRelease_r(const int metaHandle, unsigned int msTimeout);
/**
 * \ingroup C2K
 * \sa non-reentrant version of META_C2K_NsftSetTestMode_r
 */
META_RESULT __stdcall META_C2K_NsftSetTestMode(unsigned int msTimeout);
/**
 * \ingroup C2K
 * \details Enter NSFT test mode
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_NsftSetTestMode_r(const int metaHandle, unsigned int msTimeout);
/**
 * \ingroup C2K
 * \sa non-reentrant version of META_C2K_NsftPowerUp_r
 */
META_RESULT __stdcall META_C2K_NsftPowerUp(unsigned int msTimeout, unsigned int band, unsigned short channel, unsigned int walshCode, unsigned int rc, unsigned int numFrames, bool afcEnable);
/**
 * \ingroup C2K
 * \details Target search cell and synchronize to instrument
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] band C2K band class
 * \param [in] channel channel number
 * \param [in] walshCode traffic channel walsh code
 * \param [in] rc reverse radio configuration
 * \param [in] numFrames number of frames for sync
 * \param [in] afcEnable AFC enabled or disabled
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_NsftPowerUp_r(const int metaHandle, unsigned int msTimeout, unsigned int band, unsigned short channel, unsigned int walshCode, unsigned int rc, unsigned int numFrames, bool afcEnable);
/**
 * \ingroup C2K
 * \sa non-reentrant version of META_C2K_NsftTxTrafficChannel_r
 */
META_RESULT __stdcall META_C2K_NsftTxTrafficChannel(unsigned int msTimeout, unsigned int rc, unsigned int powerCtrlMode, double txPower);
/**
 * \ingroup C2K
 * \details Set TX power value
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] rc reverse radio configuration
 * \param [in] powerCtrlMode power control mode, not used, ignored by target
 * \param [in] txPower TX power value in dBm
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_NsftTxTrafficChannel_r(const int metaHandle, unsigned int msTimeout, unsigned int rc, unsigned int powerCtrlMode, double txPower);
/**
 * \ingroup C2K
 * \sa non-reentrant version of META_C2K_NsftGetFer_r
 */
META_RESULT __stdcall META_C2K_NsftGetFer(unsigned int msTimeout, unsigned int* badFrames, unsigned int* totalFrames);
/**
 * \ingroup C2K
 * \details Get FER result
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [out] badFrames pointer to bad frames
 * \param [out] totalFrames pointer to total of test frames
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_NsftGetFer_r(const int metaHandle, unsigned int msTimeout, unsigned int* badFrames, unsigned int* totalFrames);
/**
 * \ingroup C2K
 * \sa non-reentrant version of META_C2K_NsftExitTestMode_r
 */
META_RESULT __stdcall META_C2K_NsftExitTestMode(unsigned int msTimeout);
/**
 * \ingroup C2K
 * \details Exit NSFT test mode
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_NsftExitTestMode_r(const int metaHandle, unsigned int msTimeout);
/**
 * \ingroup C2K
 * \sa non-reentrant version of META_C2K_NsftListModeStart_r
 */
META_RESULT __stdcall META_C2K_NsftListModeStart(unsigned int msTimeout, NSTLISTMODE_PARMS* listModeParam);
/**
 * \ingroup C2K
 * \details Start NSFT list mode
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] listModeParam pointer to NSTLISTMODE_PARMS structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_NsftListModeStart_r(const int metaHandle, unsigned int msTimeout, NSTLISTMODE_PARMS* listModeParam);
/**
 * \ingroup C2K
 * \sa non-reentrant version of META_C2K_NsftListModeGetFer_r
 */
META_RESULT __stdcall META_C2K_NsftListModeGetFer(unsigned int msTimeout, unsigned int* count, unsigned int badFrames[], unsigned int totalFrames[]);
/**
 * \ingroup C2K
 * \details Get FER result of NSFT list mode
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [out] count pointer to the count of FER test frames
 * \param [out] badFrames bad frames
 * \param [out] totalFrames total of test frames
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_NsftListModeGetFer_r(const int metaHandle, unsigned int msTimeout, unsigned int* count, unsigned int badFrames[], unsigned int totalFrames[]);

/**
 * \ingroup C2K
 * \sa non-reentrant version of META_C2K_ReadNvram_r
 */
META_RESULT __stdcall META_C2K_ReadNvram(unsigned int msTimeout, unsigned int band, C2K_DB_SEGMENT dbSeg, void* buf, unsigned int len);
/**
 * \ingroup C2K
 * \details Read data from NVRAM
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] band C2K band class
 * \param [in] dbSeg NVRAM database segment ID
 * \param [out] buf pointer to the data buffer
 * \param [in] len the length of data to be read in buffer
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_ReadNvram_r(const int metaHandle, unsigned int msTimeout, unsigned int band, C2K_DB_SEGMENT dbSeg, void* buf, unsigned int len);
/**
 * \ingroup C2K
 * \sa non-reentrant version of META_C2K_WriteNvram_r
 */
META_RESULT __stdcall META_C2K_WriteNvram(unsigned int msTimeout, unsigned int band, C2K_DB_SEGMENT dbSeg, void* buf, unsigned int len);
/**
 * \ingroup C2K
 * \details Write data to NVRAM
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] band C2K band class
 * \param [in] dbSeg NVRAM database segment ID
 * \param [in] buf pointer to the data buffer
 * \param [in] len the length of data to be written in buffer
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_WriteNvram_r(const int metaHandle, unsigned int msTimeout, unsigned int band, C2K_DB_SEGMENT dbSeg, void* buf, unsigned int len);
/**
 * \ingroup C2K
 * \sa non-reentrant version of META_C2K_ClearRfNvram_r
 */
META_RESULT __stdcall META_C2K_ClearRfNvram(unsigned int msTimeout);
/**
 * \ingroup C2K
 * \details Clear calibration data to 0
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_ClearRfNvram_r(const int metaHandle, unsigned int msTimeout);
/**
 * \ingroup C2K
 * \sa non-reentrant version of META_C2K_CalInitNvram_r
 */
META_RESULT __stdcall META_C2K_CalInitNvram(unsigned int msTimeout, unsigned int mode);
/**
 * \ingroup C2K
 * \details Request target to update RF driver data from NVRAM
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] mode value of enum type C2K_CAL_INIT_TYPE
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_CalInitNvram_r(const int metaHandle, unsigned int msTimeout, unsigned int mode);
/**
 * \ingroup C2K
 * \sa non-reentrant version of META_C2K_FlushNvram_r
 */
META_RESULT __stdcall META_C2K_FlushNvram(unsigned int msTimeout);
/**
 * \ingroup C2K
 * \details Flush calibration data to NVRAM
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_FlushNvram_r(const int metaHandle, unsigned int msTimeout);

/**
 * \ingroup C2K
 * \sa non-reentrant version of META_C2K_CheckSIM1Inserted_r
 */
META_RESULT __stdcall META_C2K_CheckSIM1Inserted(unsigned int msTimeout, unsigned char* inserted);
/**
 * \ingroup C2K
 * \details Check if the SIM card 1 is inserted
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [out] inserted pointer to card status, 0: no card, 1: card inserted
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_CheckSIM1Inserted_r(const int metaHandle, unsigned int msTimeout, unsigned char* inserted);
/**
 * \ingroup C2K
 * \details Not used
 */
META_RESULT __stdcall META_C2K_CheckSIM2Inserted(unsigned int msTimeout, unsigned char* inserted);
/**
 * \ingroup C2K
 * \details Not used
 */
META_RESULT __stdcall META_C2K_CheckSIM2Inserted_r(const int metaHandle, unsigned int msTimeout, unsigned char* inserted);

/**
 * \ingroup C2K
 * \details Send customized command to target
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msgId message ID
 * \param [in] buffer pointer to message data buffer
 * \param [in] size the size of data to be sent in buffer
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_SendCommand_r(const int metaHandle, unsigned short msgId, char* buffer, int size);
/**
 * \ingroup C2K
 * \details Wait for response from target
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] rspId response ID
 * \param [out] buffer pointer to buffer to receive response data
 * \param [in] size the size of data to be received in buffer
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_WaitResponse_r(const int metaHandle, unsigned int msTimeout, unsigned short rspId, char* buffer, int size);
/**
 * \ingroup C2K
 * \details Not used
 */
META_RESULT __stdcall META_C2K_ChangeBaudrate_r(const int metaHandle, int baudrate);
/**
 * \ingroup C2K
 * \details Set calibration flag value
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] flag 0: not calibrated, 1: calibrated
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_SetCalibrationFlag_r(const int metaHandle, unsigned int msTimeout, int flag);
/**
 * \ingroup C2K
 * \details Get calibration flag value
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [out] flag pointer to flag, 0: not calibrated, 1: calibrated
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_GetCalibrationFlag_r(const int metaHandle, unsigned int msTimeout, int* flag);
/**
 * \ingroup C2K
 * \details Loop back to test if the target is alive
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] value this value is sent to target and loop back
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_LoopbackTest_r(const int metaHandle, unsigned int msTimeout, unsigned char value);
/**
 * \ingroup C2K
 * \details Read current temperature value
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] txPath TX path
 * \param [out] temperature pointer to the temperature value
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_ReadCurrentTemperature_r(const int metaHandle, unsigned int msTimeout, unsigned char txPath, short* temperature);
/**
 * \ingroup C2K
 * \details Request target to do self calibration
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] calFlag self calibration flag, 1: do self calibration, 0: not do
 * \param [out] pocDataSize pointer to the self calibration data size
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_PocCalibration_r(const int metaHandle, unsigned int msTimeout, unsigned int calFlag, unsigned int* pocDataSize);
/**
 * \ingroup C2K
 * \details Read NVRAM data extension
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] lid pointer to NVRAM LID string
 * \param [in] rid record ID of the NVRAM LID
 * \param [out] buf pointer to the data buffer
 * \param [in] size the size of data to be read in buffer
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_ReadNvramEx_r(const int metaHandle, unsigned int msTimeout, const char* lid, unsigned short rid, void* buf, unsigned int size);
/**
 * \ingroup C2K
 * \details Write NVRAM data extension
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] lid pointer to NVRAM LID string
 * \param [in] rid record ID of the NVRAM LID
 * \param [in] buf pointer to the data buffer
 * \param [in] size the size of data to be written in buffer
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_WriteNvramEx_r(const int metaHandle, unsigned int msTimeout, const char* lid, unsigned short rid, void* buf, unsigned int size);
/**
 * \ingroup C2K
 * \details Request target to update RF driver data from NVRAM
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] mode value of enum type C2K_CAL_INIT_TYPE
 * \param [in] type NVRAM data type of C2K_NVRAM_TYPE
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_CalInitNvramEx_r(const int metaHandle, unsigned int msTimeout, unsigned int mode, C2K_NVRAM_TYPE type);
/**
 * \ingroup C2K
 * \details Read current power detector measurement value extension
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] rfMode RF mode, 0: 1xRTT, 1: EVDO
 * \param [out] pdPower pointer to power detector value
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_ReadCurrentTxPowerEx_r(const int metaHandle, unsigned int msTimeout, C2K_RF_MODE rfMode, double* pdPower);
/**
 * \ingroup C2K
 * \details Configure AFC parameters extension
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] req pointer to AFC_CONFIG_REQ structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_AfcConfigEx_r(const int metaHandle, unsigned int msTimeout, AFC_CONFIG_REQ* req);
/**
 * \ingroup C2K
 * \details Send AT command to target
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] atCmd pointer to the null-terminated AT command string
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_SendATCommand_r(const int metaHandle, unsigned int msTimeout, char* atCmd);
/**
 * \ingroup C2K
 * \details Set logging relay mode
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] enable true: relay data to ETS tool, false: not relay
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_SetLoggingRelay_r(const int metaHandle, bool enable);
/**
 * \ingroup C2K
 * \details Read MIPI code word from target
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] rfIdx RF IC index
 * \param [in] mipiPort MIPI port
 * \param [in] mipiUsid MIPI USID
 * \param [in] mipiAddress MIPI address
 * \param [out] mipiData pointer to MIPI data
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_ReadMipi_r(const int metaHandle, unsigned int msTimeout, unsigned int rfIdx, unsigned char mipiPort, unsigned int mipiUsid, unsigned int mipiAddress, unsigned int* mipiData);
/**
 * \ingroup C2K
 * \details Write MIPI code word to target
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] rfIdx RF IC index
 * \param [in] mipiPort MIPI port
 * \param [in] mipiUsid MIPI USID
 * \param [in] mipiAddress MIPI address
 * \param [in] mipiData MIPI data
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_WriteMipi_r(const int metaHandle, unsigned int msTimeout, unsigned int rfIdx, unsigned char mipiPort, unsigned int mipiUsid, unsigned int mipiAddress, unsigned int mipiData);
/**
 * \ingroup C2K
 * \details Get modem SW version information
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [out] cnf pointer to C2K_VER_INFO structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_GetTargetVerInfo_r(const int metaHandle, unsigned int msTimeout, C2K_VER_INFO* verInfo);
/**
 * \ingroup C2K
 * \details Get RSSI result of single channel
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] band C2K band class
 * \param [in] channel channel number
 * \param [out] cnf pointer to C2K_NSFT_RSSI_CNF structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_NsftRssi_r(const int metaHandle, unsigned int msTimeout, unsigned int band, unsigned short channel, C2K_NSFT_RSSI_CNF* cnf);
/**
 * \ingroup C2K
 * \details Get RSSI result in NSFT list mode
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [out] cnf pointer to C2K_NSFT_LIST_MODE_RSSI_CNF structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_NsftListModeRssi_r(const int metaHandle, unsigned int msTimeout, C2K_NSFT_LIST_MODE_RSSI_CNF* cnf);
/**
 * \ingroup C2K
 * \details Set debug log file path(ANSI character version)
 * \param [in] filename pointer to the null-terminated file name string
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_DebugOn_With_FilePath(const char* filename);
/**
 * \ingroup C2K
 * \details Set debug log file path(wide character version)
 * \param [in] filename pointer to the null-terminated file name string
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_DebugOn_With_FilePathW(const wchar_t* filename);
/**
 * \ingroup C2K
 * \details Get temperature sensor ADC value from target
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] rfIdx RF IC index
 * \param [out] adcValue pointer to ADC value
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_GetTemperatureAdcEx_r(const int metaHandle, unsigned int msTimeout, unsigned int rfIdx, unsigned short* adcValue);
/**
 * \ingroup C2K
 * \details Set TX power to be measured in NSFT list mode
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] req pointer to C2K_NSFT_LIST_MODE_SET_POWER_REQ structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_C2K_NsftListModeSetPower_r(const int metaHandle, unsigned int msTimeout, C2K_NSFT_LIST_MODE_SET_POWER_REQ* req);

/*************************** GEN93 Functions Declaration ****************************/
/**
 * \ingroup C2K
 * \details Set target modem generation type
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] type target modem generation type, 0 is for Gen90/91/92, 1 is for Gen93 and later
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_SetTargetGenType_r(const int metaHandle, unsigned int type);
/**
 * \ingroup C2K
 * \details Get target modem generation type
 * \param [in] metaHandle Context handle to specific DUT
 * \param [out] type pointer to target modem generation type, 0 is for Gen90/91/92, 1 is for Gen93 and later
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_GetTargetGenType_r(const int metaHandle, unsigned int* type);
/**
 * \ingroup C2K
 * \details Register callback handler for data received from target
 * \param [in] metaHandle Context handle to specific DUT
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_RegisterHandler_r(const int metaHandle);
/**
 * \ingroup C2K
 * \details Send command to target and wait for confirm
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] reqId request command ID
 * \param [in] req pointer to the request message buffer
 * \param [in] reqLen length of the request message to be sent in buffer
 * \param [out] cnf pointer to the confirm message buffer
 * \param [in] cnfLen length of the confirm message to be received in buffer
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_SendCommand_r(const int metaHandle, unsigned int msTimeout, unsigned short reqId, const char* req, unsigned int reqLen, char* cnf, unsigned int cnfLen);
/**
 * \ingroup C2K
 * \details Query target capability
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [out] msCapability pointer to target capability C2kMsCapability structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_QueryTargetCapability_r(const int metaHandle, unsigned int msTimeout, C2kMsCapability* msCapability);
/**
 * \ingroup C2K
 * \details Set MIPI code word to target
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] req pointer to C2kTestCmdSetMIPICodeWord structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_SetMIPICodeWord_r(const int metaHandle, unsigned int msTimeout, const C2kTestCmdSetMIPICodeWord* req);
/**
 * \ingroup C2K
 * \details Get MIPI code word from target
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] req pointer to C2kTestCmdGetMIPICodeWord structure
 * \param [out] cnf pointer to C2kTestResultGetMIPICodeWord structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_GetMIPICodeWord_r(const int metaHandle, unsigned int msTimeout, const C2kTestCmdGetMIPICodeWord* req, C2kTestResultGetMIPICodeWord* cnf);
/**
 * \ingroup C2K
 * \details Set BSI data to target
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] req pointer to C2kTestCmdSetBSI structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_SetBSI_r(const int metaHandle, unsigned int msTimeout, const C2kTestCmdSetBSI* req);
/**
 * \ingroup C2K
 * \details Get BSI data from target
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] req pointer to C2kTestCmdGetBSI structure
 * \param [out] cnf pointer to C2kTestResultGetBSI structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_GetBSI_r(const int metaHandle, unsigned int msTimeout, const C2kTestCmdGetBSI* req, C2kTestResultGetBSI* cnf);
/**
 * \ingroup C2K
 * \details Transmitter on/off control
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] req pointer to C2kTestCmdTxControl structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_TxControl_r(const int metaHandle, unsigned int msTimeout, const C2kTestCmdTxControl* req);
/**
 * \ingroup C2K
 * \details Receiver on/off control
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] req pointer to C2kTestCmdRxControl structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_RxControl_r(const int metaHandle, unsigned int msTimeout, const C2kTestCmdRxControl* req);
/**
 * \ingroup C2K
 * \details Configure AFC parameters
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] req pointer to C2kTestCmdAfcConfig structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_AfcConfig_r(const int metaHandle, unsigned int msTimeout, const C2kTestCmdAfcConfig* req);
/**
 * \ingroup C2K
 * \details Configure TX signal parameters
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] req pointer to C2kTestCmdTxAgcConfig structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_TxAgcConfig_r(const int metaHandle, unsigned int msTimeout, const C2kTestCmdTxAgcConfig* req);
/**
 * \ingroup C2K
 * \details Get target's power detector measurement result
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [out] power pointer to power detector measurement result
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_GetPowerDetectorResult_r(const int metaHandle, unsigned int msTimeout, double* power);
/**
 * \ingroup C2K
 * \details Configure RX signal parameters
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] req pointer to C2kTestCmdRxAgcConfig structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_RxAgcConfig_r(const int metaHandle, unsigned int msTimeout, const C2kTestCmdRxAgcConfig* req);
/**
 * \ingroup C2K
 * \details Get RSSI result from target
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] req pointer to C2kTestCmdRssi structure
 * \param [out] cnf pointer to C2kTestResultRssi structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_Rssi_r(const int metaHandle, unsigned int msTimeout, const C2kTestCmdRssi* req, C2kTestResultRssi* cnf);
/**
 * \ingroup C2K
 * \details Set AFC calibration data to target
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] updateNvram whether update AFC data to NVRAM or not, true: update, false: not update
 * \param [in] req pointer to C2kTestCmdGetSetAfcData structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_SetAfcData_r(const int metaHandle, unsigned int msTimeout, bool updateNvram, const C2kTestCmdGetSetAfcData* req);
/**
 * \ingroup C2K
 * \details Get AFC calibration data from target
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [out] cnf pointer to C2kTestCmdGetSetAfcData structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_GetAfcData_r(const int metaHandle, unsigned int msTimeout, C2kTestCmdGetSetAfcData* cnf);
/**
 * \ingroup C2K
 * \details Set TX calibration data to target
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] updateNvram whether update TX data to NVRAM or not, true: update, false: not update
 * \param [in] band C2K band class
 * \param [in] mode RF mode, 0: 1xRTT, 1: EVDO
 * \param [in] req pointer to C2kTestCmdGetSetTxData structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_SetTxData_r(const int metaHandle, unsigned int msTimeout, bool updateNvram, unsigned int band, unsigned int mode, const C2kTestCmdGetSetTxData* req);
/**
 * \ingroup C2K
 * \details Get TX calibration data from target
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] band C2K band class
 * \param [in] mode RF mode, 0: 1xRTT, 1: EVDO
 * \param [out] cnf pointer to C2kTestCmdGetSetTxData structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_GetTxData_r(const int metaHandle, unsigned int msTimeout, unsigned int band, unsigned int mode, C2kTestCmdGetSetTxData* cnf);
/**
 * \ingroup C2K
 * \details Set RX calibration data to target
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] updateNvram whether update RX data to NVRAM or not, true: update, false: not update
 * \param [in] band C2K band class
 * \param [in] path RX path, 0: main path, 1: diversity path, 2: second path
 * \param [in] req pointer to C2kTestCmdGetSetRxData structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_SetRxData_r(const int metaHandle, unsigned int msTimeout, bool updateNvram, unsigned int band, unsigned int path, const C2kTestCmdGetSetRxData* req);
/**
 * \ingroup C2K
 * \details Get RX calibration data from target
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] band C2K band class
 * \param [in] path RX path, 0: main path, 1: diversity path, 2: second path
 * \param [out] cnf pointer to C2kTestCmdGetSetRxData structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_GetRxData_r(const int metaHandle, unsigned int msTimeout, unsigned int band, unsigned int path, C2kTestCmdGetSetRxData* cnf);
/**
 * \ingroup C2K
 * \details Set DPD PA calibration data to target (not used)
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] updateNvram whether update DPD PA data to NVRAM or not, true: update, false: not update
 * \param [in] band C2K band class
 * \param [in] mode RF mode, 0: 1xRTT, 1: EVDO
 * \param [in] req pointer to C2kTestCmdGetSetDpdPaData structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_SetDpdPaData_r(const int metaHandle, unsigned int msTimeout, bool updateNvram, unsigned int band, unsigned int mode, const C2kTestCmdGetSetDpdPaData* req);
/**
 * \ingroup C2K
 * \details Get DPD PA calibration data from target (not used)
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] band C2K band class
 * \param [in] mode RF mode, 0: 1xRTT, 1: EVDO
 * \param [out] cnf pointer to C2kTestCmdGetSetDpdPaData structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_GetDpdPaData_r(const int metaHandle, unsigned int msTimeout, unsigned int band, unsigned int mode, C2kTestCmdGetSetDpdPaData* cnf);
/**
 * \ingroup C2K
 * \details Not used
 */
META_RESULT __stdcall META_MMC2K_SetDpdAmPmData_r(const int metaHandle, unsigned int msTimeout, bool updateNvram, unsigned int band, const C2kTestCmdGetSetDpdAmPmData* req);
/**
 * \ingroup C2K
 * \details Not used
 */
META_RESULT __stdcall META_MMC2K_GetDpdAmPmData_r(const int metaHandle, unsigned int msTimeout, unsigned int band, C2kTestCmdGetSetDpdAmPmData* cnf);
/**
 * \ingroup C2K
 * \details Start FHC command and wait for confirm
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] req pointer to C2kTestCmdFhc structure
 * \param [out] cnf pointer to C2kTestResultFhc structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_StartFhc_r(const int metaHandle, unsigned int msTimeout, const C2kTestCmdFhc* req, C2kTestResultFhc* cnf);
/**
 * \ingroup C2K
 * \details Request target to update RF driver data
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_InitRfDriver_r(const int metaHandle, unsigned int msTimeout);
/**
 * \ingroup C2K
 * \details Setup UBIN mode
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] mode 0: deinitialize, 1: initialize
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_UbinModeSetup_r(const int metaHandle, unsigned int msTimeout, const unsigned char mode);
/**
 * \ingroup C2K
 * \details Set MEID to target
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] req pointer to C2kTestCmdGetSetMeid structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_SetMeid_r(const int metaHandle, unsigned int msTimeout, C2kTestCmdGetSetMeid* req);
/**
 * \ingroup C2K
 * \details Get MEID from target
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [out] cnf pointer to C2kTestCmdGetSetMeid structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_GetMeid_r(const int metaHandle, unsigned int msTimeout, C2kTestCmdGetSetMeid* cnf);
/**
 * \ingroup C2K
 * \details Get RX LNA calibration configuration parameters from target
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] req pointer to C2kTestCmdGetRxLnaConfig structure
 * \param [out] cnf pointer to C2kTestResultRxLnaConfig structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_GetRxLnaConfig_r(const int metaHandle, unsigned int msTimeout, C2kTestCmdGetRxLnaConfig* req, C2kTestResultRxLnaConfig* cnf);

/**
 * \ingroup C2K
 * \details Enter NSFT test mode
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] req pointer to C2kNsftCmdTestMode structure
 * \param [out] cnf pointer to C2kNsftResultStatus structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_NsftEnterTestMode_r(const int metaHandle, unsigned int msTimeout, const C2kNsftCmdTestMode* req, C2kNsftResultStatus* cnf);
/**
 * \ingroup C2K
 * \details Exit NSFT test mode
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] req pointer to C2kNsftCmdTestMode structure
 * \param [out] cnf pointer to C2kNsftResultStatus structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_NsftExitTestMode_r(const int metaHandle, unsigned int msTimeout, const C2kNsftCmdTestMode* req, C2kNsftResultStatus* cnf);
/**
 * \ingroup C2K
 * \details Target search cell and synchronize to instrument
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] req pointer to C2kNsftCmdPowerUp structure
 * \param [out] cnf pointer to C2kNsftResultStatus structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_NsftPowerUp_r(const int metaHandle, unsigned int msTimeout, const C2kNsftCmdPowerUp* req, C2kNsftResultStatus* cnf);
/**
 * \ingroup C2K
 * \details Set TX power value
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] req pointer to C2kNsftCmdSetTxPower structure
 * \param [out] cnf pointer to C2kNsftResultStatus structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_NsftSetTxPower_r(const int metaHandle, unsigned int msTimeout, const C2kNsftCmdSetTxPower* req, C2kNsftResultStatus* cnf);
/**
 * \ingroup C2K
 * \details Get FER test result from target
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] req pointer to C2kNsftCmdFer structure
 * \param [out] cnf pointer to C2kNsftResultFer structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_NsftGetFer_r(const int metaHandle, unsigned int msTimeout, const C2kNsftCmdFer* req, C2kNsftResultFer* cnf);
/**
 * \ingroup C2K
 * \details Get RSSI test result from target
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] req pointer to C2kNsftCmdRssi structure
 * \param [out] cnf pointer to C2kNsftResultRssi structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_NsftGetRssi_r(const int metaHandle, unsigned int msTimeout, const C2kNsftCmdRssi* req, C2kNsftResultRssi* cnf);
/**
 * \ingroup C2K
 * \details Start NSFT list mode and wait for confirm
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] req pointer to C2kNsftCmdListMode structure
 * \param [out] cnf pointer to C2kNsftResultListMode structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_NsftListMode_r(const int metaHandle, unsigned int msTimeout, const C2kNsftCmdListMode* req, C2kNsftResultListMode* cnf);
/**
 * \ingroup C2K
 * \details Set RX antenna mode
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] mode antenna mode, 0: auto mode, 1: diversity only mode
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_SetRxAntennaMode_r(const int metaHandle, unsigned int msTimeout, unsigned int mode);
/**
 * \ingroup C2K
 * \details Get RX gain setting from target
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] req pointer to C2kTestCmdGetRxGainSetting structure
 * \param [out] cnf pointer to C2kTestResultGetRxGainSetting structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_GetRxGainSetting_r(const int metaHandle, unsigned int msTimeout, const C2kTestCmdGetRxGainSetting* req, C2kTestResultGetRxGainSetting* cnf);
/**
 * \ingroup C2K
 * \details Get TX gain setting from target
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] req pointer to C2kTestCmdGetTxGainSetting structure
 * \param [out] cnf pointer to C2kTestResultGetTxGainSetting structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_GetTxGainSetting_r(const int metaHandle, unsigned int msTimeout, const C2kTestCmdGetTxGainSetting* req, C2kTestResultGetTxGainSetting* cnf);
/**
 * \ingroup C2K
 * \details Set BPI data to target
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] req pointer to C2kTestCmdSetBpiData structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_SetBpiData_r(const int metaHandle, unsigned int msTimeout, const C2kTestCmdSetBpiData* req);
/**
 * \ingroup C2K
 * \details Get BPI data from target
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] req pointer to C2kTestCmdGetBpiData structure
 * \param [out] cnf pointer to C2kTestResultGetBpiData structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_GetBpiData_r(const int metaHandle, unsigned int msTimeout, const C2kTestCmdGetBpiData* req, C2kTestResultGetBpiData* cnf);

/**
 * \ingroup C2K
 * \details Configure RX signal parameters Gen97
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] req pointer to C2kTestCmdRxAgcConfigV7 structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_RxAgcConfigV7_r(const int metaHandle, unsigned int msTimeout, const C2kTestCmdRxAgcConfigV7* req);

/**
 * \ingroup C2K
 * \details Set DPD calibration flag to target
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] req pointer to C2kTestCmdSetDpdCalFlag structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_SetDpdCalibrationFlag_r(const int metaHandle, unsigned int msTimeout, const C2kTestCmdSetDpdCalFlag* req);
/**
 * \ingroup C2K
 * \details Get DPD calibration flag from target
 * \param [in] metaHandle Context handle to specific DUT
 * \param [in] msTimeout API transaction timeout value in milliseconds
 * \param [in] req pointer to C2kTestCmdGetDpdCalFlag structure
 * \param [out] cnf pointer to C2kTestResultGetDpdCalFlag structure
 * \retval META_SUCCESS successful
 */
META_RESULT __stdcall META_MMC2K_GetDpdCalibrationFlag_r(const int metaHandle, unsigned int msTimeout, const C2kTestCmdGetDpdCalFlag* req, C2kTestResultGetDpdCalFlag* cnf);

#ifdef __cplusplus
}
#endif

#endif
