#ifndef _C2KAGENT_API_DATATYPE_H_
#define _C2KAGENT_API_DATATYPE_H_

#define C2K_BAND_SUPPORT_BC0             (1<<0)
#define C2K_BAND_SUPPORT_BC1             (1<<1)
#define C2K_BAND_SUPPORT_BC2             (1<<2)
#define C2K_BAND_SUPPORT_BC3             (1<<3)
#define C2K_BAND_SUPPORT_BC4             (1<<4)
#define C2K_BAND_SUPPORT_BC5             (1<<5)
#define C2K_BAND_SUPPORT_BC6             (1<<6)
#define C2K_BAND_SUPPORT_BC7             (1<<7)
#define C2K_BAND_SUPPORT_BC8             (1<<8)
#define C2K_BAND_SUPPORT_BC9             (1<<9)
#define C2K_BAND_SUPPORT_BC10            (1<<10)
#define C2K_BAND_SUPPORT_BC11            (1<<11)
#define C2K_BAND_SUPPORT_BC12            (1<<12)
#define C2K_BAND_SUPPORT_BC13            (1<<13)
#define C2K_BAND_SUPPORT_BC14            (1<<14)
#define C2K_BAND_SUPPORT_BC15            (1<<15)

#define C2K_SUPPORT_BAND_COUNT           16

#define C2K_RF_ID_UNKNOWN                0x00000000
#define C2K_RF_ID_MT6158                 0x00000003
#define C2K_RF_ID_MT6176                 0x00000004
#define C2K_RF_ID_MT6179                 0x00000005
#define C2K_RF_ID_MT6177L                0x00000012
#define C2K_RF_ID_MT6177M                0x00000013

#define C2K_LNA_MODE_NUM                 8
#define C2K_NSFT_LIST_MODE_NUM           50
#define C2K_TX_POWER_BACKOFF_NUM         2
#define C2K_FREQ_COMP_NUM                16
#define C2K_TEMP_COMP_NUM                8
#define C2K_PA_SECTION_NUM               8

#define C2K_FHC_MAX_STEP                 2000
#define C2K_FHC_RX_HIGH_POWER_MODE       0x00
#define C2K_FHC_RX_LOW_POWER_MODE        0x08
#define C2K_FHC_RX_MAIN_DIV              0x30
#define C2K_FHC_RX_MAIN_SHDR             0x50
#define C2K_FHC_RX_SHDR                  0x40

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    C2K_COM_SERIAL,
    C2K_COM_USB,
    C2K_COM_USBLTE,
    C2K_COM_VC
}
C2K_COM_TYPE;

typedef enum
{
    C2K_RX_CAL_MAIN_DIV,
    C2K_RX_CAL_MAIN,
    C2K_RX_CAL_DIV
} C2K_RX_CALIBRATION_TYPE;

typedef enum
{
    C2K_AFC_TCXO,
    C2K_AFC_DCXO
} C2K_AFC_CONTROL_MODE;

typedef enum
{
    C2K_TX_SPI,
    C2K_RX_SPI
} C2K_SPI_TYPE;

typedef enum
{
    C2K_RF_MODE_1XRTT,
    C2K_RF_MODE_EVDO,
    C2K_RF_MODE_BOTH
} C2K_RF_MODE;

typedef enum
{
    C2K_CTRL_MODE_AUTO,
    C2K_CTRL_MODE_DISABLE,
    C2K_CTRL_MODE_MANUAL
} C2K_CTRL_MODE;

typedef enum
{
    C2K_LNA_MODE_HIGH,
    C2K_LNA_MODE_MIDDLE,
    C2K_LNA_MODE_LOW
} C2K_LNA_MODE;

// For SIDB access mode
typedef enum
{
    C2K_SIDB_FSI,
    C2K_SIDB_FFS
} C2K_SIDB_ACCESS_MODE;

typedef enum
{
    C2K_NVRAM_RF_PARAM,
    C2K_NVRAM_RF_CAL,
    C2K_NVRAM_MIPI,
    C2K_NVRAM_POC
} C2K_NVRAM_TYPE;

typedef enum
{
    // OrionC DB segments
    C2K_DB_AFC,
    C2K_DB_TEMPERATURE,
    C2K_DB_TXAGC,
    C2K_DB_TXAGC_FREQ_ADJ,
    C2K_DB_TXAGC_TEMP_ADJ,
    C2K_DB_TX_COUPLE_LOSS,
    C2K_DB_TX_COUPLE_LOSS_FREQ_ADJ,
    C2K_DB_RXAGC_FREQ_ADJ,
    C2K_DB_RXAGC_TEMP_ADJ,
    C2K_DB_DIV_RXAGC_FREQ_ADJ,
    C2K_DB_DIV_RXAGC_TEMP_ADJ,
    C2K_DB_RF_VERSION,
    C2K_DB_MEID,
    C2K_DB_CUSTOM_DATA,
    C2K_DB_TAS_DATA,
    //MT6176 LID
    C2K_LID_POC_DATA,
    C2K_LID_AFC,
    C2K_LID_TEMPERATURE,
    C2K_LID_RX_LOSS_HIGH_POWER,
    C2K_LID_RX_DIV_LOSS_HIGH_POWER,
    C2K_LID_RX_SHDR_LOSS_HIGH_POWER,
    C2K_LID_RX_LOSS_LOW_POWER,
    C2K_LID_RX_DIV_LOSS_LOW_POWER,
    C2K_LID_RX_SHDR_LOSS_LOW_POWER,
    C2K_LID_TX_APC,
    C2K_LID_TX_COUPLER_LOSS,
    C2K_LID_TX_PA_GAIN_COMP,
    C2K_LID_TX_COUPLER_LOSS_COMP,
    C2K_LID_TX_PA_GAIN_COMP_1XRTT,
    C2K_LID_TX_COUPLER_LOSS_COMP_1XRTT,
    C2K_LID_AFC_S_CURVE,
    C2K_LID_TX_POWER_BACKOFF,
    C2K_LID_AGPS_GROUP_DELAY,
    C2K_LID_TEMPERATURE_2
} C2K_DB_SEGMENT;

/**
 * \ingroup C2KStruct
 * \details Library initialization parameters
 */
typedef struct
{
    int bLog;            /**< indicate enable(1)/disable(0) to save log in file */
    char* szLogPath;     /**< pointer to the null-terminated log file name string */
    int bScript;         /**< not used, always set to 0 */
    char* szScriptPath;  /**< not used, always set to NULL */
} C2K_LIBCONFIGPARMS, *LPC2K_LIBCONFIGPARMS;

/**
 * \ingroup C2KStruct
 * \details Library initialization parameters(wide character version)
 */
typedef struct
{
    int bLog;                 /**< indicate enable(1)/disable(0) to save log in file */
    const wchar_t* szLogPath; /**< pointer to the null-terminated log file name string(wide-character) */
    int bScript;              /**< not used, always set to 0 */
    char* szScriptPath;       /**< not used, always set to NULL */
} C2K_LIBCONFIGPARMSW;

/**
 * \ingroup C2KStruct
 * \details Not used
 */
typedef struct
{
    C2K_AFC_CONTROL_MODE eAfcControlMode;
    C2K_RF_MODE eRfMode;
    C2K_RX_CALIBRATION_TYPE eRxCalibrationType;
    C2K_SIDB_ACCESS_MODE eSidbAccessMode;
    int bWriteSIDBFlag;
    unsigned int uBarcodeLength;
    unsigned int uPllSettleTime;
    unsigned int uRxCtrlSettleTime;
    unsigned int uRxGainStateSettleTime;
    unsigned int uRxAGCGainTableSelectSettleTime;
    unsigned int uEnableRxTxSpySettleTime;
    unsigned int uTxCtrlSettleTime;
    unsigned int uTxTestRateSettleTime;
    unsigned int uTxAGCConfigSettleTime;
    int bAFCSlopeStepPerPpmWithQ6;
} PHONEATTRIBUTE_PARMS, *LPPHONEATTRIBUTE_PARMS;

/**
 * \ingroup C2KStruct
 * \details Connect target parameters
 */
typedef struct
{
    C2K_COM_TYPE eComType; /**< communication type, always set to C2K_COM_VC */
    unsigned int uComNum;  /**< virtual channel index, always set to 2 */
} C2K_CONNECT_PARAMS, *LPC2K_CONNECT_PARMS;

/**
 * \ingroup C2KStruct
 * \details Target capability information
 */
typedef struct
{
    unsigned int bandSupport;    /**< bitmap of band class supported by target, bit0 ~ bit15 indicates band class 0 ~ 15 */
    unsigned int rxdBandSupport; /**< bitmap of RX diversity band support */
    unsigned int rxdEnabled;     /**< indicate if RX diversity is enabled */
    unsigned int paOctLevel;     /**< indicate PA 8 level control or not, 1: 8 levels, 0: 25 levels */
    unsigned int rfId;           /**< RF chip ID */
    unsigned char cpMajor;       /**< CBP software major version */
    unsigned char cpMinor;       /**< CBP software minor version */
    unsigned char cpRev;         /**< CBP software revision */
} C2K_MS_CAPABILITY;

/**
 * \ingroup C2KStruct
 * \details Not used
 */
typedef struct
{
    unsigned char ucAction;        /**< 0: off, 1: on */
    unsigned char ucRfMode;        /**< 0: 1xRTT, 1: EVDO, 2: SHDR */
    unsigned char ucAntennaBitmap; /**< Bit0: Rx main, Bit1: Rx diversity, Bit2: Tx path; 0: invalid, 1: valid */
    unsigned char ucSampleRate;    /**< 0: 8x, 1: 4x, 2: 2x, 3: 1x */
    unsigned char ucLogMode;       /**< 0: assert mode, 1: period mode */
    unsigned short usLogPeriod;    /**< unit: half slot or half PCG */
    unsigned short usLogOffset1;   /**< unit: half slot or half PCG */
    unsigned short usLogOffset2;   /**< unit: chip */
    unsigned short usLogLength1;   /**< unit: half slot or half PCG */
    unsigned short usLogLength2;   /**< unit: chip */
} LOG_IQ_REQ;

/**
 * \ingroup C2KStruct
 * \details TX AGC configuration parameters
 */
typedef struct
{
    C2K_RF_MODE rfMode;        /**< RF mode, 0: 1xRTT, 1: EVDO */
    C2K_CTRL_MODE ctrlMode;    /**< control mode, 0: auto mode, 2: manual mode */
    unsigned char ucPaMode;    /**< PA mode, 0: high, 1: middle, 2: low */
    double prf;                /**< expected TX power in dBm */
    double paGain;             /**< PA gain in dB */
    double coupleLoss;         /**< coupler loss in dB */
    unsigned char ucVm1;       /**< VM1 of PA control */
    unsigned char ucVm2;       /**< VM2 of PA control */
    unsigned short usVoltCtrl; /**< PA voltage in mV */
    unsigned short ddpcCtrl;   /**< DDPC control, 0: bypass DDPC, 6: enable DDPC loop, 7: enable DDPC measurement */
} TXAGC_CONFIG_REQ;

/**
 * \ingroup C2KStruct
 * \details RSSI measured by target
 */
typedef struct
{
    double dMainPower;    /**< main path power */
    double dMainDigiGain; /**< main path digital gain */
    int nMainBitSel;      /**< not used */
    int nMainDigiGain;    /**< not used */
    int nMainGainState;   /**< not used */
    double dDivPower;     /**< diversity path power */
    double dDivDigiGain;  /**< diversity path digital gain */
    int nDivBitSel;       /**< not used */
    int nDivDigiGain;     /**< not used */
    int nDivGainState;    /**< not used */
} C2K_RSSI_CNF;

/**
 * \ingroup C2KStruct
 * \details FHC common parameters
 */
typedef struct
{
    unsigned short nTxRxDelay;        /**< RX delay offset to TX, unit: us */
    unsigned short nTxStepLength;     /**< TX power step length, unit: us */
    unsigned short nTxRetuneLength;   /**< TX frequency retune length, unit: us */
    unsigned short nRxStepLength;     /**< RX power step length, unit: us */
    unsigned short nRxRetuneLength;   /**< RX frequency retune length, unit: us */
    unsigned char caVm1[22][3];       /**< VM1 of PA control, indexed by [band:22][PA mode:3] */
    unsigned char caVm2[22][3];       /**< VM2 of PA control, indexed by [band:22][PA mode:3] */
    unsigned short naVoltCtrl[22][8]; /**< PA voltage, indexed by [band:22][PA section:8] */
} FHC_COMMON_REQ;

/**
 * \ingroup C2KStruct
 * \details FHC TX parameters of one step
 */
typedef struct
{
    unsigned char cRetuneInd;    /**< retune indicator of the step */
    unsigned char cBand;         /**< band class */
    unsigned short nChannel;     /**< channel number */
    unsigned char cPaMode;       /**< PA mode, 0: high, 1: middle, 2: low */
    unsigned char cSectionIndex; /**< PA section index */
    double prf;                  /**< expected TX power in dBm */
    double paGain;               /**< PA gain in dB */
    double coupleLoss;           /**< coupler loss in dB */
} FHC_TX_FREQ_STEP;

/**
 * \ingroup C2KStruct
 * \details FHC TX step parameters
 */
typedef struct
{
    unsigned short usTxStepNum;                     /**< number of TX steps in array */
    FHC_TX_FREQ_STEP txFreqSteps[C2K_FHC_MAX_STEP]; /**< TX steps array */
} FHC_TX_REQ;

/**
 * \ingroup C2KStruct
 * \details FHC RX parameters of one step
 */
typedef struct
{
    unsigned char cRetuneInd; /**< retune indicator of the step */
    unsigned char cBand;      /**< band class */
    unsigned short nChannel;  /**< channel number */
    /**
     * bit2 ~ bit0: LNA mode index
     * bit3: RX power mode, 0: high power mode, 1: low power mode
     * bit6 ~ bit4: RX path indicator, 1: enabled, 0: disabled
     * bit6: SHDR path, bit5: diversity path, bit4: main path
     */
    unsigned char cLnaMode;
    double cellPower;         /**< input cell power level, not used by target */
} FHC_RX_FREQ_STEP;

/**
 * \ingroup C2KStruct
 * \details FHC RX step parameters
 */
typedef struct
{
    unsigned short usRxStepNum;                     /**< number of RX steps in array */
    FHC_RX_FREQ_STEP rxFreqSteps[C2K_FHC_MAX_STEP]; /**< RX steps array */
} FHC_RX_REQ;

/**
 * \ingroup C2KStruct
 * \details FHC request parameters
 */
typedef struct
{
    unsigned char cReqType;   /**< request type, 0: common, 1: TX, 2: RX */
    unsigned char cReqNum;    /**< number of requests */
    unsigned char cReqIndex;  /**< index of request */
    FHC_COMMON_REQ commonReq; /**< common parameter */
    FHC_TX_REQ txReq;         /**< TX parameter */
    FHC_RX_REQ rxReq;         /**< RX parameter */
} C2K_FHC_REQ;

/**
 * \ingroup C2KStruct
 * \details FHC TX power detector measurement result
 */
typedef struct
{
    unsigned short usCnfNum;           /**< number of results in array */
    double pdResult[C2K_FHC_MAX_STEP]; /**< power detector result */
} FHC_TX_CNF;

/**
 * \ingroup C2KStruct
 * \details FHC RX power measurement result
 */
typedef struct
{
    unsigned short usCnfNum;               /**< number of results in array */
    double mainDigiGain[C2K_FHC_MAX_STEP]; /**< main path power */
    double divDigiGain[C2K_FHC_MAX_STEP];  /**< diversity path power */
    char caTemperature[C2K_FHC_MAX_STEP];  /**< temperature, not used */
} FHC_RX_CNF;

/**
 * \ingroup C2KStruct
 * \details FHC confirm result
 */
typedef struct
{
    unsigned char cCnfId;    /**< confirm ID */
    unsigned char cCnfNum;   /**< number of confirms */
    unsigned char cCnfIndex; /**< index of confirm */
    FHC_TX_CNF txCnf;        /**< TX confirm */
    FHC_RX_CNF rxCnf;        /**< RX confirm */
} C2K_FHC_CNF;

/*********** NVRAM Data Structure definition for OrionC Calibration ***********/
/**
 * \ingroup C2KStruct
 * \details MEID get/set parameters
 */
typedef struct
{
    unsigned int idType; /**< ID type, 0: MEID, 1: ESN */
    char meid[17];       /**< Null-terminated 14 hexadecimal format MEID string */
    char esn[9];         /**< Null-terminated 8 hexadecimal format ESN string */
} MEID_TABLE, *LPMEID_TABLE;

/**
 * \ingroup C2KStruct
 * \details AFC calibration data
 */
typedef struct
{
    unsigned short initDacValue; /**< initial DAC value */
    short afcSlopeInv;           /**< invert of AFC slope */
    unsigned int capId;          /**< CAP ID */
} AFC_TABLE_SEG, *LPAFC_TABLE_SEG;

/**
 * \ingroup C2KStruct
 * \details Temperature and ADC value pair
 */
typedef struct
{
    char tempCelsius;        /**< temperature in degree Celsius */
    unsigned short adcValue; /**< ADC value */
} TEMPERATURE_PAIR;

/**
 * \ingroup C2KStruct
 * \details Temperature calibration data
 */
typedef struct
{
    TEMPERATURE_PAIR tempPair[8]; /**< temperature array */
} TEMPERATURE_TABLE_SEG, *LPTEMPERATURE_TABLE_SEG;

/**
 * \ingroup C2KStruct
 * \details RX compensation data of one channel
 */
typedef struct
{
    unsigned short channel;           /**< channel number */
    double adjData[C2K_LNA_MODE_NUM]; /**< compensation value */
} RXFREQADJ_PAIR;

/**
 * \ingroup C2KStruct
 * \details RX channel compensation data
 */
typedef struct
{
    RXFREQADJ_PAIR freqAdj[16]; /**< RX frequency compensation array */
} RXFREQADJ_TABLE_SEG, *LPRXFREQADJ_TABLE_SEG;

/**
 * \ingroup C2KStruct
 * \details RX channel compensation data of all paths
 */
typedef struct
{
    RXFREQADJ_TABLE_SEG mainTable; /**< table of main path */
    RXFREQADJ_TABLE_SEG divTable;  /**< table of diversity path */
} RXFREQADJ_TABLE_SEG_ALL, *LPRXFREQADJ_TABLE_SEG_ALL;

/**
 * \ingroup C2KStruct
 * \details RX compensation data of one temperature
 */
typedef struct
{
    char tempCelsius; /**< temperature in degree Celsius */
    double adjData;   /**< compensation value */
} RXTEMPADJ_PAIR;

/**
 * \ingroup C2KStruct
 * \details RX temperature compensation data
 */
typedef struct
{
    RXTEMPADJ_PAIR tempAdj[8]; /**< RX temperature compensation array */
} RXTEMPADJ_TABLE_SEG, *LPRXTEMPADJ_TABLE_SEG;

/**
 * \ingroup C2KStruct
 * \details RX temperature compensation data of all paths
 */
typedef struct
{
    RXTEMPADJ_TABLE_SEG mainTable; /**< table of main path */
    RXTEMPADJ_TABLE_SEG divTable;  /**< table of diversity path */
} RXTEMPADJ_TABLE_SEG_ALL, *LPRXTEMPADJ_TABLE_SEG_ALL;

/**
 * \ingroup C2KStruct
 * \details TX setting of one PA section
 */
typedef struct
{
    double paGain;           /**< PA gain in dB */
    double prf;              /**< the power of PA 8 levels in dBm */
    unsigned short paMode;   /**< PA mode, 0: high, 1: middle, 2: low */
    unsigned short vm1;      /**< VM1 of PA control */
    unsigned short vm2;      /**< VM2 of PA control */
    unsigned short voltCtrl; /**< PA voltage in mV */
} PA_SECTION;

/**
 * \ingroup C2KStruct
 * \details TX hysteresis
 */
typedef struct
{
    double start; /**< PA hysteresis start in dBm */
    double end;   /**< PA hysteresis end in dBm */
} TX_HYSTERESIS;

/**
 * \ingroup C2KStruct
 * \details TX AGC calibration data
 */
typedef struct
{
    unsigned char paSectionNum; /**< number of PA sections */
    PA_SECTION paSection[8];    /**< PA section array, index is from 0 to 7, TX power and PA mode order is from high to low */
    TX_HYSTERESIS txHyst[2];    /**< PA hysteresis array, index 0 of high <-> middle, index 1 of middle <-> low */
    short paPhaseComp[3];       /**< PA phase compensation, index 0 ~ 2 to PA high, middle, low mode */
} TXAGC_TABLE_SEG, *LPTXAGC_TABLE_SEG;

/**
 * \ingroup C2KStruct
 * \details TX compensation data of one channel
 */
typedef struct
{
    unsigned short channel; /**< channel number */
    double adjData[3];      /**< compensation value */
} TXFREQADJ_PAIR;

/**
 * \ingroup C2KStruct
 * \details TX channel compensation data
 */
typedef struct
{
    TXFREQADJ_PAIR freqAdj[16]; /**< TX frequency compensation array */
} TXFREQADJ_TABLE_SEG, *LPTXFREQADJ_TABLE_SEG;

/**
 * \ingroup C2KStruct
 * \details TX compensation data of one temperature
 */
typedef struct
{
    char tempCelsius;  /**< temperature in degree Celsius */
    double adjData[3]; /**< compensation value */
} TXTEMPADJ_PAIR;

/**
 * \ingroup C2KStruct
 * \details TX temperature compensation data
 */
typedef struct
{
    TXTEMPADJ_PAIR tempAdj[8]; /**< TX temperature compensation array */
} TXTEMPADJ_TABLE_SEG, *LPTXTEMPADJ_TABLE_SEG;

/**
 * \ingroup C2KStruct
 * \details TX coupler loss calibration data
 */
typedef struct
{
    double coupleLoss[3]; /**< coupler loss in dB, index 0 ~ 2 to PA high, middle, low mode */
} TXCOUPLELOSS_TABLE_SEG, *LPTXCOUPLELOSS_TABLE_SEG;

/**
 * \ingroup C2KStruct
 * \details Coupler loss compensation data of one channel
 */
typedef struct
{
    unsigned short channel; /**< channel number */
    double adjData;         /**< compensation value */
} TXCOUPLELOSSFREQADJ_PAIR;

/**
 * \ingroup C2KStruct
 * \details Coupler loss channel compensation data
 */
typedef struct
{
    TXCOUPLELOSSFREQADJ_PAIR freqAdj[16]; /**< coupler loss frequency compensation array */
} TXCOUPLELOSSFREQADJ_TABLE_SEG, *LPTXCOUPLELOSSFREQADJ_TABLE_SEG;

/**
 * \ingroup C2KStruct
 * \details Not used
 */
typedef struct
{
    unsigned char sysBandClass;
    unsigned char supported;
    unsigned int subClass;
} SUPPORTED_BAND_CLASS_T;

/**
 * \ingroup C2KStruct
 * \details Not used
 */
typedef struct
{
    /* BAND A BPI mask */
    unsigned int MASK_BAND_A_PR1;
    unsigned int MASK_BAND_A_PR2;
    unsigned int MASK_BAND_A_PR2B;
    unsigned int MASK_BAND_A_PR3;
    unsigned int MASK_BAND_A_PR3A;
    unsigned int MASK_BAND_A_PT1;
    unsigned int MASK_BAND_A_PT2;
    unsigned int MASK_BAND_A_PT2B;
    unsigned int MASK_BAND_A_PT3;
    unsigned int MASK_BAND_A_PT3A;
    unsigned int MASK_BAND_A_RXD_PR1;
    unsigned int MASK_BAND_A_RXD_PR2;
    unsigned int MASK_BAND_A_RXD_PR2B;
    unsigned int MASK_BAND_A_RXD_PR3;
    unsigned int MASK_BAND_A_RXD_PR3A;

    /* BAND B BPI mask */
    unsigned int MASK_BAND_B_PR1;
    unsigned int MASK_BAND_B_PR2;
    unsigned int MASK_BAND_B_PR2B;
    unsigned int MASK_BAND_B_PR3;
    unsigned int MASK_BAND_B_PR3A;
    unsigned int MASK_BAND_B_PT1;
    unsigned int MASK_BAND_B_PT2;
    unsigned int MASK_BAND_B_PT2B;
    unsigned int MASK_BAND_B_PT3;
    unsigned int MASK_BAND_B_PT3A;
    unsigned int MASK_BAND_B_RXD_PR1;
    unsigned int MASK_BAND_B_RXD_PR2;
    unsigned int MASK_BAND_B_RXD_PR2B;
    unsigned int MASK_BAND_B_RXD_PR3;
    unsigned int MASK_BAND_B_RXD_PR3A;

    /* BAND C BPI mask */
    unsigned int MASK_BAND_C_PR1;
    unsigned int MASK_BAND_C_PR2;
    unsigned int MASK_BAND_C_PR2B;
    unsigned int MASK_BAND_C_PR3;
    unsigned int MASK_BAND_C_PR3A;
    unsigned int MASK_BAND_C_PT1;
    unsigned int MASK_BAND_C_PT2;
    unsigned int MASK_BAND_C_PT2B;
    unsigned int MASK_BAND_C_PT3;
    unsigned int MASK_BAND_C_PT3A;
    unsigned int MASK_BAND_C_RXD_PR1;
    unsigned int MASK_BAND_C_RXD_PR2;
    unsigned int MASK_BAND_C_RXD_PR2B;
    unsigned int MASK_BAND_C_RXD_PR3;
    unsigned int MASK_BAND_C_RXD_PR3A;

    /* Band A Gate BPI mask */
    unsigned int MASK_BAND_A_PRG1;       /* Main Rx On */
    unsigned int MASK_BAND_A_PRG2;
    unsigned int MASK_BAND_A_PRG2B;
    unsigned int MASK_BAND_A_PRG3;       /* Main Rx Off */
    unsigned int MASK_BAND_A_PRG3A;
    unsigned int MASK_BAND_A_PTG1;       /* Tx On */
    unsigned int MASK_BAND_A_PTG2;
    unsigned int MASK_BAND_A_PTG2B;
    unsigned int MASK_BAND_A_PTG3;       /* Tx Off */
    unsigned int MASK_BAND_A_PTG3A;
    unsigned int MASK_BAND_A_RXD_PRG1;   /* Diversity Rx On */
    unsigned int MASK_BAND_A_RXD_PRG2;
    unsigned int MASK_BAND_A_RXD_PRG2B;
    unsigned int MASK_BAND_A_RXD_PRG3;   /* Diversity Rx Off */
    unsigned int MASK_BAND_A_RXD_PRG3A;

    /* Band B Gate BPI mask */
    unsigned int MASK_BAND_B_PRG1;       /* Main Rx On */
    unsigned int MASK_BAND_B_PRG2;
    unsigned int MASK_BAND_B_PRG2B;
    unsigned int MASK_BAND_B_PRG3;       /* Main Rx Off */
    unsigned int MASK_BAND_B_PRG3A;
    unsigned int MASK_BAND_B_PTG1;       /* Tx On */
    unsigned int MASK_BAND_B_PTG2;
    unsigned int MASK_BAND_B_PTG2B;
    unsigned int MASK_BAND_B_PTG3;       /* Tx Off */
    unsigned int MASK_BAND_B_PTG3A;
    unsigned int MASK_BAND_B_RXD_PRG1;   /* Diversity Rx On */
    unsigned int MASK_BAND_B_RXD_PRG2;
    unsigned int MASK_BAND_B_RXD_PRG2B;
    unsigned int MASK_BAND_B_RXD_PRG3;   /* Diversity Rx Off */
    unsigned int MASK_BAND_B_RXD_PRG3A;

    /* Band C Gate BPI mask */
    unsigned int MASK_BAND_C_PRG1;       /* Main Rx On */
    unsigned int MASK_BAND_C_PRG2;
    unsigned int MASK_BAND_C_PRG2B;
    unsigned int MASK_BAND_C_PRG3;       /* Main Rx Off */
    unsigned int MASK_BAND_C_PRG3A;
    unsigned int MASK_BAND_C_PTG1;       /* Tx On */
    unsigned int MASK_BAND_C_PTG2;
    unsigned int MASK_BAND_C_PTG2B;
    unsigned int MASK_BAND_C_PTG3;       /* Tx Off */
    unsigned int MASK_BAND_C_PTG3A;
    unsigned int MASK_BAND_C_RXD_PRG1;   /* Diversity Rx On */
    unsigned int MASK_BAND_C_RXD_PRG2;
    unsigned int MASK_BAND_C_RXD_PRG2B;
    unsigned int MASK_BAND_C_RXD_PRG3;   /* Diversity Rx Off */
    unsigned int MASK_BAND_C_RXD_PRG3A;

    /* Power Control mask */
    unsigned int MASK_PRPC1;       /* Main Rx On */
    unsigned int MASK_PRPC2;
    unsigned int MASK_PRPC2B;
    unsigned int MASK_PRPC3;       /* Main Rx Off */
    unsigned int MASK_PRPC3A;
    unsigned int MASK_PTPC1;       /* Tx On */
    unsigned int MASK_PTPC2;
    unsigned int MASK_PTPC2B;
    unsigned int MASK_PTPC3;       /* Tx Off */
    unsigned int MASK_PTPC3A;
    unsigned int MASK_RXD_PRPC1;   /* Diversity Rx On */
    unsigned int MASK_RXD_PRPC2;
    unsigned int MASK_RXD_PRPC2B;
    unsigned int MASK_RXD_PRPC3;   /* Diversity Rx Off */
    unsigned int MASK_RXD_PRPC3A;
} BPI_MASK_T;

/**
 * \ingroup C2KStruct
 * \details Not used
 */
typedef struct
{
    /* BAND A BPI data */
    unsigned int DATA_BAND_A_PR1;
    unsigned int DATA_BAND_A_PR2;
    unsigned int DATA_BAND_A_PR2B;
    unsigned int DATA_BAND_A_PR3;
    unsigned int DATA_BAND_A_PR3A;
    unsigned int DATA_BAND_A_PT1;
    unsigned int DATA_BAND_A_PT2;
    unsigned int DATA_BAND_A_PT2B;
    unsigned int DATA_BAND_A_PT3;
    unsigned int DATA_BAND_A_PT3A;
    unsigned int DATA_BAND_A_RXD_PR1;
    unsigned int DATA_BAND_A_RXD_PR2;
    unsigned int DATA_BAND_A_RXD_PR2B;
    unsigned int DATA_BAND_A_RXD_PR3;
    unsigned int DATA_BAND_A_RXD_PR3A;

    /* BAND B BPI data */
    unsigned int DATA_BAND_B_PR1;
    unsigned int DATA_BAND_B_PR2;
    unsigned int DATA_BAND_B_PR2B;
    unsigned int DATA_BAND_B_PR3;
    unsigned int DATA_BAND_B_PR3A;
    unsigned int DATA_BAND_B_PT1;
    unsigned int DATA_BAND_B_PT2;
    unsigned int DATA_BAND_B_PT2B;
    unsigned int DATA_BAND_B_PT3;
    unsigned int DATA_BAND_B_PT3A;
    unsigned int DATA_BAND_B_RXD_PR1;
    unsigned int DATA_BAND_B_RXD_PR2;
    unsigned int DATA_BAND_B_RXD_PR2B;
    unsigned int DATA_BAND_B_RXD_PR3;
    unsigned int DATA_BAND_B_RXD_PR3A;

    /* BAND C BPI data */
    unsigned int DATA_BAND_C_PR1;
    unsigned int DATA_BAND_C_PR2;
    unsigned int DATA_BAND_C_PR2B;
    unsigned int DATA_BAND_C_PR3;
    unsigned int DATA_BAND_C_PR3A;
    unsigned int DATA_BAND_C_PT1;
    unsigned int DATA_BAND_C_PT2;
    unsigned int DATA_BAND_C_PT2B;
    unsigned int DATA_BAND_C_PT3;
    unsigned int DATA_BAND_C_PT3A;
    unsigned int DATA_BAND_C_RXD_PR1;
    unsigned int DATA_BAND_C_RXD_PR2;
    unsigned int DATA_BAND_C_RXD_PR2B;
    unsigned int DATA_BAND_C_RXD_PR3;
    unsigned int DATA_BAND_C_RXD_PR3A;

    /* Band A TXGATE data */
    unsigned int DATA_BAND_A_PRG1;       /* Main Rx On */
    unsigned int DATA_BAND_A_PRG2;
    unsigned int DATA_BAND_A_PRG2B;
    unsigned int DATA_BAND_A_PRG3;       /* Main Rx Off */
    unsigned int DATA_BAND_A_PRG3A;
    unsigned int DATA_BAND_A_PTG1;       /* Tx On */
    unsigned int DATA_BAND_A_PTG2;
    unsigned int DATA_BAND_A_PTG2B;
    unsigned int DATA_BAND_A_PTG3;       /* Tx Off */
    unsigned int DATA_BAND_A_PTG3A;
    unsigned int DATA_BAND_A_RXD_PRG1;   /* Diversity Rx On */
    unsigned int DATA_BAND_A_RXD_PRG2;
    unsigned int DATA_BAND_A_RXD_PRG2B;
    unsigned int DATA_BAND_A_RXD_PRG3;   /* Diversity Rx Off */
    unsigned int DATA_BAND_A_RXD_PRG3A;

    /* Band B TXGATE data */
    unsigned int DATA_BAND_B_PRG1;       /* Main Rx On */
    unsigned int DATA_BAND_B_PRG2;
    unsigned int DATA_BAND_B_PRG2B;
    unsigned int DATA_BAND_B_PRG3;       /* Main Rx Off */
    unsigned int DATA_BAND_B_PRG3A;
    unsigned int DATA_BAND_B_PTG1;       /* Tx On */
    unsigned int DATA_BAND_B_PTG2;
    unsigned int DATA_BAND_B_PTG2B;
    unsigned int DATA_BAND_B_PTG3;       /* Tx Off */
    unsigned int DATA_BAND_B_PTG3A;
    unsigned int DATA_BAND_B_RXD_PRG1;   /* Diversity Rx On */
    unsigned int DATA_BAND_B_RXD_PRG2;
    unsigned int DATA_BAND_B_RXD_PRG2B;
    unsigned int DATA_BAND_B_RXD_PRG3;   /* Diversity Rx Off */
    unsigned int DATA_BAND_B_RXD_PRG3A;

    /* Band C TXGATE data */
    unsigned int DATA_BAND_C_PRG1;       /* Main Rx On */
    unsigned int DATA_BAND_C_PRG2;
    unsigned int DATA_BAND_C_PRG2B;
    unsigned int DATA_BAND_C_PRG3;       /* Main Rx Off */
    unsigned int DATA_BAND_C_PRG3A;
    unsigned int DATA_BAND_C_PTG1;       /* Tx On */
    unsigned int DATA_BAND_C_PTG2;
    unsigned int DATA_BAND_C_PTG2B;
    unsigned int DATA_BAND_C_PTG3;       /* Tx Off */
    unsigned int DATA_BAND_C_PTG3A;
    unsigned int DATA_BAND_C_RXD_PRG1;   /* Diversity Rx On */
    unsigned int DATA_BAND_C_RXD_PRG2;
    unsigned int DATA_BAND_C_RXD_PRG2B;
    unsigned int DATA_BAND_C_RXD_PRG3;   /* Diversity Rx Off */
    unsigned int DATA_BAND_C_RXD_PRG3A;

    /* TX Power Control data */
    unsigned int DATA_PRPC1;       /* Main Rx On */
    unsigned int DATA_PRPC2;
    unsigned int DATA_PRPC2B;
    unsigned int DATA_PRPC3;       /* Main Rx Off */
    unsigned int DATA_PRPC3A;
    unsigned int DATA_PTPC1;       /* Tx On */
    unsigned int DATA_PTPC2;
    unsigned int DATA_PTPC2B;
    unsigned int DATA_PTPC3;       /* Tx Off */
    unsigned int DATA_PTPC3A;
    unsigned int DATA_RXD_PRPC1;   /* Diversity Rx On */
    unsigned int DATA_RXD_PRPC2;
    unsigned int DATA_RXD_PRPC2B;
    unsigned int DATA_RXD_PRPC3;   /* Diversity Rx Off */
    unsigned int DATA_RXD_PRPC3A;
} BPI_DATA_T;

/**
 * \ingroup C2KStruct
 * \details Not used
 */
typedef struct
{
    // RF Window timing
    unsigned short TC_PR1;
    unsigned short TC_PR2;
    unsigned short TC_PR2B;

    unsigned short TC_PR3;
    unsigned short TC_PR3A;

    unsigned short TC_RXD_PR1;
    unsigned short TC_RXD_PR2;
    unsigned short TC_RXD_PR2B;

    unsigned short TC_RXD_PR3;
    unsigned short TC_RXD_PR3A;

    unsigned short TC_PT1;
    unsigned short TC_PT2;
    unsigned short TC_PT2B;

    unsigned short TC_PT3;
    unsigned short TC_PT3A;

    // RF Gate timing
    unsigned short TC_PRG1;
    unsigned short TC_PRG2;
    unsigned short TC_PRG2B;

    unsigned short TC_PRG3;
    unsigned short TC_PRG3A;

    unsigned short TC_RXD_PRG1;
    unsigned short TC_RXD_PRG2;
    unsigned short TC_RXD_PRG2B;

    unsigned short TC_RXD_PRG3;
    unsigned short TC_RXD_PRG3A;

    unsigned short TC_PTG1;
    unsigned short TC_PTG2;
    unsigned short TC_PTG2B;

    unsigned short TC_PTG3;
    unsigned short TC_PTG3A;

    // RF TX Power Control timing
    short TC_PRPC1;
    short TC_PRPC2;
    short TC_PRPC2B;

    short TC_PRPC3;
    short TC_PRPC3A;

    short TC_RXD_PRPC1;
    short TC_RXD_PRPC2;
    short TC_RXD_PRPC2B;

    short TC_RXD_PRPC3;
    short TC_RXD_PRPC3A;

    short TC_PTPC1;
    short TC_PTPC2;
    short TC_PTPC2B;

    short TC_PTPC3;
    short TC_PTPC3A;
} BPI_TIMING_T;

/**
 * \ingroup C2KStruct
 * \details Not used
 */
typedef struct
{
    short dc2DcSettlingTime;
} PA_TIMING_T;

/**
 * \ingroup C2KStruct
 * \details Not used
 */
typedef struct
{
    unsigned char BAND_A_CHANNEL_SEL;
    unsigned char BAND_B_CHANNEL_SEL;
    unsigned char BAND_C_CHANNEL_SEL;
} RX_LNA_PORT_SEL_T;

/**
 * \ingroup C2KStruct
 * \details Not used
 */
typedef struct
{
    unsigned char BAND_A_OUTPUT_SEL;
    unsigned char BAND_B_OUTPUT_SEL;
    unsigned char BAND_C_OUTPUT_SEL;
} TX_PATH_SEL_T;

/**
 * \ingroup C2KStruct
 * \details Not used
 */
typedef struct
{
    unsigned char structVersion; //reserved
    unsigned char isDataUpdate; //reserved
    SUPPORTED_BAND_CLASS_T supportedBand[3];
    BPI_MASK_T bpiMask;
    BPI_DATA_T bpiData;
    BPI_TIMING_T bpiTiming;
    PA_TIMING_T paTiming;
    RX_LNA_PORT_SEL_T rxLnaPortSel;
    RX_LNA_PORT_SEL_T rxDivLnaPortSel;
    TX_PATH_SEL_T txPathSel;
    unsigned char rxDiversityEnable;
    unsigned char rxDiversityTestEnable;
    unsigned char paVddPmuControlEnable;
    unsigned char paVddBattControlEnable;
    unsigned char paVddDc2DcControlEnable;
    unsigned char customTmEnable;
} CUSTOMDATA_TABLE, *LPCUSTOMDATA_TABLE;

/**
 * \ingroup C2KStruct
 * \details Not used
 */
typedef struct
{
    char rfOption[21];
    char rfDriverRev[11];
} RFVERSION_TABLE, *LPRFVERSION_TABLE;

/**
 * \ingroup C2KStruct
 * \details NSFT list mode steps parameter
 */
typedef struct
{
    unsigned int uCount;                                 /**< count of list steps */
    bool bAFCEnable;                                     /**< AFC enabled, true: enabled, false: disabled */
    unsigned int uIndex[C2K_NSFT_LIST_MODE_NUM];         /**< index of step */
    unsigned int uProtocol[C2K_NSFT_LIST_MODE_NUM];      /**< number of RSSI test levels */
    unsigned int uOffset[C2K_NSFT_LIST_MODE_NUM];        /**< occupied frames of the step */
    unsigned int uBand[C2K_NSFT_LIST_MODE_NUM];          /**< band class of the step */
    unsigned int uChannel[C2K_NSFT_LIST_MODE_NUM];       /**< channel number of the step */
    unsigned int uWashCode[C2K_NSFT_LIST_MODE_NUM];      /**< traffic channel walsh code of the step */
    unsigned int uRadioConfig[C2K_NSFT_LIST_MODE_NUM];   /**< reverse radio configuration of the step */
    unsigned int uNumFrames[C2K_NSFT_LIST_MODE_NUM];     /**< number of FER test frames of the step */
    unsigned int uPowerCtrlMode[C2K_NSFT_LIST_MODE_NUM]; /**< power control mode, not used */
    float fTxPower[C2K_NSFT_LIST_MODE_NUM];              /**< expected max TX power of the step */
} NSTLISTMODE_PARMS, *LPNSTLISTMODE_PARMS;

/**
 * \ingroup C2KStruct
 * \details Not used
 */
typedef struct
{
    unsigned char tasEnable;
    unsigned char tasInitAntIndex;
    unsigned char forceTxAntEnable;
    unsigned char forceTxAntIndex;
    unsigned int tasMask;
    unsigned int tasData[3][7];
} TASDATA_TABLE, *LPTASDATA_TABLE;

/*********** MT6176 data structure ***********/
typedef enum
{
    C2K_CAL_INIT_NVRAM,
    C2K_CAL_INIT_DEFAULT
} C2K_CAL_INIT_TYPE;

typedef enum
{
    C2K_NVRAM_DATA_RF,
    C2K_NVRAM_DATA_POC
} C2K_NVRAM_DATA_TYPE;

/**
 * \ingroup C2KStruct
 * \details RX channel and temperature compensation data
 */
typedef struct
{
    unsigned short channel[16]; /**< channel number */
    char temp[8];               /**< temperature */
    double comp[8][8][16];      /**< compensation value, indexed by [temp celsius][LNA mode][channel] */
} RX_LOSS_TABLE, *LPRX_LOSS_TABLE;

/**
 * \ingroup C2KStruct
 * \details TX PA gain channel and temperature compensation data
 */
typedef struct
{
    unsigned short channel[16]; /**< channel number */
    char temp[8];               /**< temperature */
    double comp[3][8][16];      /**< compensation value, indexed by [PA mode][temp celsius][channel] */
} PA_GAIN_COMP_TABLE, *LPPA_GAIN_COMP_TABLE;

/**
 * \ingroup C2KStruct
 * \details Coupler loss channel and temperature compensation data
 */
typedef struct
{
    unsigned short channel[16]; /**< channel number */
    char temp[8];               /**< temperature */
    double comp[3][8][16];      /**< compensation value, indexed by [PA mode][temp celsius][channel] */
} COUPLER_LOSS_COMP_TABLE, *LPCOUPLER_LOSS_COMP_TABLE;

/**
 * \ingroup C2KStruct
 * \details AFC configuration parameters
 */
typedef struct
{
    C2K_RF_MODE rfMode;     /**< RF mode, 0: 1xRTT, 1: EVDO */
    unsigned int ctrlMode;  /**< control mode, 0: auto mode, 1: manual mode */
    bool capIdValid;        /**< indicate if capId value is used by target */
    unsigned short capId;   /**< CAP ID */
    bool dacValid;          /**< indicate if initDac value is used by target */
    unsigned short initDac; /**< initial DAC value */
    bool foeValid;          /**< indicate if foePpb value is used by target */
    short foePpb;           /**< frequency error in PPB */
} AFC_CONFIG_REQ;

/**
 * \ingroup C2KStruct
 * \details AFC S-Curve coefficients
 */
typedef struct
{
    double c0;  /**< constant coefficient */
    double c1;  /**< 1 order coefficient */
    double c2;  /**< 2 order coefficient */
    double c3;  /**< 3 order coefficient */
} S_CURVE_TABLE, *LPS_CURVE_TABLE;

/**
 * \ingroup C2KStruct
 * \details Modem SW version information
 */
typedef struct
{
    char    BB_CHIP[64];   /**< BaseBand chip version */
    char    SW_VER[100];   /**< S/W version */
    char    SW_BRANCH[64]; /**< Build branch information */
    char    SW_FLAVOR[64]; /**< Build flavor information */
} C2K_VER_INFO;

/**
 * \ingroup C2KStruct
 * \details TX max power back off hysteresis
 */
typedef struct
{
    short temp;          /**< temperature */
    double powerBackoff; /**< max power back off value */
} TX_POWER_BACKOFF;

/**
 * \ingroup C2KStruct
 * \details TX max power back off data
 */
typedef struct
{
    TX_POWER_BACKOFF txPowerBackoff[C2K_TX_POWER_BACKOFF_NUM]; /**< max power back off hysteresis */
} TX_POWER_BACKOFF_TABLE, *LPTX_POWER_BACKOFF_TABLE;

/**
 * \ingroup C2KStruct
 * \details Single RSSI measurement result
 */
typedef struct
{
    unsigned char ok;        /**< status */
    double rssiMain;         /**< main path RSSI */
    double rssiRxd;          /**< diversity path RSSI */
    unsigned short pnOffset; /**< PN offset */
    short strength;          /**< signal strength */
} C2K_NSFT_RSSI_CNF;

/**
 * \ingroup C2KStruct
 * \details NSFT list mode RSSI measurement result
 */
typedef struct
{
    unsigned short listCount;                    /**< count of list steps */
    unsigned short rssiCount;                    /**< count of RSSI levels */
    double rssiMain[C2K_NSFT_LIST_MODE_NUM][10]; /**< main path RSSI levels result */
    double rssiRxd[C2K_NSFT_LIST_MODE_NUM][10];  /**< diversity path RSSI levels result */
} C2K_NSFT_LIST_MODE_RSSI_CNF;

/**
 * \ingroup C2KStruct
 * \details NSFT list mode TX power test levels
 */
typedef struct
{
    unsigned char powerCount; /**< count of TX power levels */
    double power[10];         /**< TX power levels array */
} C2K_NSFT_LIST_MODE_SET_POWER_REQ;


/*************************** GEN93 data structure ****************************/
#define C2K_FHC_MAX_TX_STEP                 1000
#define C2K_FHC_MAX_RX_STEP                 1000
#define C2K_NSFT_LIST_MODE_TX_PWR_COUNT     20
#define C2K_NSFT_LIST_MODE_RSSI_COUNT       20

typedef enum
{
    C2K_TX_SIG_RF_TONE,
    C2K_TX_SIG_BB_TONE,
    C2K_TX_SIG_1xRTT,
    C2K_TX_SIG_EVDO_PILOT,
    C2K_TX_SIG_EVDO_ST2
} C2K_TX_SIG_TYPE;

typedef enum
{
    C2K_PA_TYPE_APT_MODE = 0,
    C2K_PA_TYPE_DPD_MODE
} C2K_PA_TYPE;

typedef enum
{
    C2K_POWER_CTRL_MODE_FIX = 0,
    C2K_POWER_CTRL_MODE_MANUAL
} C2K_POWER_CTRL_MODE;

/**
 * \ingroup C2KStruct
 * \details Target capability information
 */
typedef struct
{
    unsigned short rfId;               /**< RF chip ID */
    unsigned short xoType;             /**< crystal type, 0: VCTCXO, 1: TSX, 2: TCXO */
    unsigned int bandSupport;          /**< bitmap of band class supported by target, bit0 ~ bit15 indicates band class 0 ~ 15 */
    unsigned int rxdBandSupport;       /**< bitmap of RXD supported band */
    unsigned int mipiBandSupport;      /**< bitmap of MIPI supported band */
    unsigned int dpdBandSupport;       /**< bitmap of DPD PA supported band */
    unsigned int rxdPathNum;           /**< number of RXD paths */
    unsigned int nsftListModeType;     /**< 0: NSFT list mode TX frame = 2, 1: NSFT list mode TX frame = 5 */
    /**
     * bit0 indicates using modulation or CW signal for RX calibration, 0: modulation signal, 1: CW signal
     * bit1 indicates if target supports Gen97 new APIs, 0: not support, 1: support
     * bit2 indicates if target supports read/write DPD calibration flag, 0: not support, 1: support
     */
    unsigned int rxCalCW;
} C2kMsCapability;

/**
 * \ingroup C2KStruct
 * \details MIPI code word parameters for set command
 */
typedef struct
{
    unsigned short mipiPort; /**< MIPI port */
    unsigned short reserved; /**< reserved */
    unsigned int mipiUsid;   /**< MIPI USID */
    unsigned int mipiAddr;   /**< MIPI address */
    unsigned int mipiData;   /**< MIPI data */
} C2kTestCmdSetMIPICodeWord;

/**
 * \ingroup C2KStruct
 * \details MIPI code word parameters for get command
 */
typedef struct
{
    unsigned short mipiPort; /**< MIPI port */
    unsigned short reserved; /**< reserved */
    unsigned int mipiUsid;   /**< MIPI USID */
    unsigned int mipiAddr;   /**< MIPI address */
} C2kTestCmdGetMIPICodeWord;

/**
 * \ingroup C2KStruct
 * \details MIPI code word data of get command
 */
typedef struct
{
    unsigned int mipiData;   /**< MIPI data */
} C2kTestResultGetMIPICodeWord;

/**
 * \ingroup C2KStruct
 * \details BSI parameters for set command
 */
typedef struct
{
    unsigned short bsiId;    /**< BSI ID */
    unsigned short reserved; /**< reserved */
    unsigned int bsiAddr;    /**< BSI address */
    unsigned int bsiData;    /**< BSI data */
} C2kTestCmdSetBSI;

/**
 * \ingroup C2KStruct
 * \details BSI parameters for get command
 */
typedef struct
{
    unsigned short bsiId;    /**< BSI ID */
    unsigned short reserved; /**< reserved */
    unsigned int bsiAddr;    /**< BSI address */
} C2kTestCmdGetBSI;

/**
 * \ingroup C2KStruct
 * \details BSI data of get command
 */
typedef struct
{
    unsigned int bsiData;    /**< BSI data */
} C2kTestResultGetBSI;

/**
 * \ingroup C2KStruct
 * \details 1xRTT TX channel parameters
 */
typedef struct
{
    unsigned char rc;              /**< reverse radio config, 0: RC1, 1: RC2, 2: RC3, 3: RC4, 4: RC5 */
    unsigned char chType;          /**< channel type, 0: access, 1: FCH, 2: FCH + SCH */
    unsigned char rate;            /**< data rate, 0: full rate, 1: 1/2 rate, 2: 1/4 rate, 3: 1/8 rate */
    unsigned short txPreamble;     /**< TX preamble, 0: pilot + data, 1: pilot only */
} C2k1xRTTChannelParam;

/**
 * \ingroup C2KStruct
 * \details TX control parameters
 */
typedef struct
{
    unsigned char rfMode;          /**< RF mode, 0: 1xRTT, 1: EVDO */
    unsigned char action;          /**< 0: switch off, 1: switch on */
    unsigned char sigType;         /**< signal type, value of enum C2K_TX_SIG_TYPE */
    unsigned char band;            /**< band class 0 ~ 21 */
    unsigned short channel;        /**< channel number */
    unsigned short reserved;       /**< reserved */
    unsigned int frequency;        /**< unit: Hz, for singal tone use */
    unsigned int bbTone;           /**< 0: RF tone, 1: use baseband to generate CW tone singal */
    C2k1xRTTChannelParam chParam;  /**< only for 1xRTT use */
} C2kTestCmdTxControl;

/**
 * \ingroup C2KStruct
 * \details RX control parameters
 */
typedef struct
{
    unsigned char rfMode;      /**< RF mode, 0: 1xRTT, 1: EVDO */
    unsigned char action;      /**< 0: switch off, 1: switch on */
    unsigned char rxPath;      /**< bitmap of RX path, bit0: main path, bit1: diversity path, bit2: second path */
    unsigned char band;        /**< band class 0 ~ 21 */
    unsigned short channel;    /**< channel number */
    unsigned short ctrlMode;   /**< control mode, 0: RX AGC auto loop, 1: manual */
} C2kTestCmdRxControl;

/**
 * \ingroup C2KStruct
 * \details AFC control parameters
 */
typedef struct
{
    unsigned char ctrlMode;    /**< control mode, 1: manual */
    unsigned char capIdValid;  /**< 0: capId not used, 1: capId used */
    unsigned short capId;      /**< CAP ID */
    unsigned char reserved;    /**< reserved */
    unsigned char dacValid;    /**< 0: dacValue not used, 1: dacValue used */
    unsigned short dacValue;   /**< DAC value */
} C2kTestCmdAfcConfig;

/**
 * \ingroup C2KStruct
 * \details TX AGC configuration parameters
 */
typedef struct
{
    unsigned char rfMode;      /**< RF mode, 0: 1xRTT, 1: EVDO */
    unsigned char paType;      /**< 0: APT mode, 1: DPD mode */
    unsigned char ctrlMode;    /**< 0: fix power, 1: manual */
    unsigned char ilpcEnable;  /**< DDPC control, 0: disable, 1: enable */
    unsigned char paTblIndex;  /**< 8 levels PA table index */
    unsigned char paMode;      /**< PA mode, 0: high, 1: middle, 2: low */
    double antennaPower;       /**< the power want to be transmitted at antenna port in dBm*/
    double txPower;            /**< the power of PA 8 levels in dBm */
    double paGain;             /**< PA gain in dB */
    double couplerLoss;        /**< coupler loss in dB */
    unsigned short am;         /**< not used, set to 0 */
    unsigned short pm;         /**< not used, set to 0 */
    unsigned char vm1;         /**< VM1 of PA control */
    unsigned char vm2;         /**< VM2 of PA control */
    unsigned short dc2dcLevel; /**< PA voltage in mV */
} C2kTestCmdTxAgcConfig;

/**
 * \ingroup C2KStruct
 * \details RX AGC configuration parameters
 */
typedef struct
{
    unsigned char rfMode;      /**< RF mode, 0: 1xRTT, 1: EVDO */
    unsigned char rxPath;      /**< bitmap of RX path, bit0: main path, bit1: diversity path, bit2: second path */
    unsigned char ctrlMode;    /**< control mode, 0: RX AGC auto loop, 1: manual */
    unsigned char powerMode;   /**< power mode, 0: high power mode, 1: low power mode */
    unsigned char lnaMode;     /**< LNA mode index 0 ~ 7 */
    unsigned char reserved[3];
} C2kTestCmdRxAgcConfig;

/**
 * \ingroup C2KStruct
 * \details Get RSSI command parameters
 */
typedef struct
{
    unsigned char rfMode;      /**< RF mode, 0: 1xRTT, 1: EVDO */
    unsigned char rxPath;      /**< bitmap of RX path, bit0: main path, bit1: diversity path, bit2: second path */
    unsigned short reserved;
} C2kTestCmdRssi;

/**
 * \ingroup C2KStruct
 * \details RSSI measurement result
 */
typedef struct
{
    double mainPower;          /**< main path power(dBm) */
    double divPower;           /**< diversity/second path power(dBm) */
} C2kTestResultRssi;

/**
 * \ingroup C2KStruct
 * \details AFC calibration data
 */
typedef struct
{
    unsigned short initDacValue; /**< initial DAC value */
    short afcSlopeInv;           /**< invert of AFC slope */
    unsigned short capId;        /**< CAP ID */
    unsigned short reserved;
} C2kTestCmdGetSetAfcData;

/**
 * \ingroup C2KStruct
 * \details TX setting of one PA section
 */
typedef struct
{
    unsigned char paMode;      /**< PA mode, 0: high, 1: middle, 2: low */
    short prf;                 /**< the power of PA 8 levels in dBm */
    double paGain;             /**< PA gain in dB */
    unsigned char vm1;         /**< VM1 of PA control */
    unsigned char vm2;         /**< VM2 of PA control */
    unsigned short dc2dcLevel; /**< PA voltage in mV */
} C2kTestPaSection;

/**
 * \ingroup C2KStruct
 * \details TX calibration data
 */
typedef struct
{
    char hysteresisStart0;        /**< hysteresis start of high <-> middle */
    char hysteresisEnd0;          /**< hysteresis end of high <-> middle */
    char hysteresisStart1;        /**< hysteresis start of middle <-> low */
    char hysteresisEnd1;          /**< hysteresis end of middle <-> low */
    unsigned char paSectionNum;   /**< number of PA sections */
    C2kTestPaSection paSection[C2K_PA_SECTION_NUM]; /**< PA section array, index is from 0 to 7, TX power and PA mode order is from low to high */
    double couplerLoss[3];        /**< coupler loss of PA high, middle, low mode */
    char temperature[C2K_TEMP_COMP_NUM];       /**< temperature */
    unsigned short channel[C2K_FREQ_COMP_NUM]; /**< channel number */
    double paGainComp[C2K_PA_SECTION_NUM][C2K_TEMP_COMP_NUM][C2K_FREQ_COMP_NUM]; /**< PA gain compensation, indexed by [PA section:8][temperature:8][channel:16] */
    double couplerLossComp[3][C2K_TEMP_COMP_NUM][C2K_FREQ_COMP_NUM]; /**< coupler loss compensation, indexed by [PA mode:3][temperature:8][channel:16] */
    short paPhaseComp[3]; /**< PA phase compensation, index 0 ~ 2 to PA high, middle, low mode */
} C2kTestCmdGetSetTxData;

/**
 * \ingroup C2KStruct
 * \details RX calibration data
 */
typedef struct
{
    char temperature[C2K_TEMP_COMP_NUM];       /**< temperature */
    unsigned short channel[C2K_FREQ_COMP_NUM]; /**< channel number */
    double lossHpm[C2K_LNA_MODE_NUM][C2K_TEMP_COMP_NUM][C2K_FREQ_COMP_NUM]; /**< high power mode loss, indexed by [LNA mode:8][temperature:8][channel:16] */
    double lossLpm[C2K_LNA_MODE_NUM][C2K_TEMP_COMP_NUM][C2K_FREQ_COMP_NUM]; /**< low power mode loss, indexed by [LNA mode:8][temperature:8][channel:16] */
} C2kTestCmdGetSetRxData;

/**
 * \ingroup C2KStruct
 * \details Not used
 */
typedef struct
{
    unsigned char paControlLevel;
    unsigned char roomTempIndex;
    char hysteresisStart0;
    char hysteresisEnd0;
    char hysteresisStart1;
    char hysteresisEnd1;
    unsigned char paMode[C2K_PA_SECTION_NUM];
    short prf[C2K_PA_SECTION_NUM];
    short paGain[C2K_PA_SECTION_NUM];
    unsigned short dc2dcLevel[C2K_PA_SECTION_NUM];
    unsigned char vm1[C2K_PA_SECTION_NUM];
    unsigned char vm2[C2K_PA_SECTION_NUM];
    unsigned short channel[C2K_FREQ_COMP_NUM];
    short paGainComp[C2K_PA_SECTION_NUM][C2K_TEMP_COMP_NUM][C2K_FREQ_COMP_NUM]; //[PA section:8][temperature:8][channel:16]
    short couplerLoss[3];
    short couplerLossComp[3][C2K_TEMP_COMP_NUM][C2K_FREQ_COMP_NUM]; //[PA mode:3][temperature:8][channel:16]
    unsigned short delay[15];
    unsigned short dpdPaArray[512];
} C2kTestCmdGetSetDpdPaData;

/**
 * \ingroup C2KStruct
 * \details Not used
 */
typedef struct
{
    unsigned short channel[C2K_FREQ_COMP_NUM];
    unsigned char lutAm[C2K_PA_SECTION_NUM][C2K_FREQ_COMP_NUM][16];
    char lutPm[C2K_PA_SECTION_NUM][C2K_FREQ_COMP_NUM][16];
    unsigned short dpdPaArray[512];
} C2kTestCmdGetSetDpdAmPmData;

/**
 * \ingroup C2KStruct
 * \details FHC TX one step
 */
typedef struct
{
    unsigned char rfMode;              /**< RF mode, 0: 1xRTT, 1: EVDO */
    /**
     * 0: next step is normal step
     * 1: next step is retune step
     * 2: current step is the last step
     * 3: next step is RF mode switch step
     */
    unsigned char stepInd;
    unsigned char band;                /**< band class */
    unsigned short channel;            /**< channel number */
    double txPower;                    /**< TX power in dBm */
} FhcTxStep;

/**
 * \ingroup C2KStruct
 * \details FHC RX one step
 */
typedef struct
{
    /**
     * 0: next step is normal step
     * 1: next step is retune step
     * 2: current step is the last step
     */
    unsigned char stepInd;
    unsigned char band;                /**< band class */
    unsigned short channel;            /**< channel number */
    unsigned char path;                /**< bitmap of RX path, bit0: main path, bit1: diversity path, bit2: second path */
    unsigned char powerMode;           /**< power mode, 0: high power mode, 1: low power mode */
    unsigned char lnaMode;             /**< LNA mode index 0 ~ 7 */
} FhcRxStep;

/**
 * \ingroup C2KStruct
 * \details FHC command parameters
 */
typedef struct
{
    unsigned short txRxDelay;               /**< RX delay offset to TX, unit: us */
    unsigned short txStepLength;            /**< TX power step length, unit: us, 1000 ~ 20000 */
    unsigned short txRetuneLength;          /**< TX frequency retune length, unit: us, >=500 */
    unsigned short rfModeSwitchLength;      /**< RF mode switch length, unit: us, >=1000 */
    unsigned short rxStepLength;            /**< RX power step length, unit: us, 1000 ~ 20000 */
    unsigned short rxRetuneLength;          /**< RX frequency retune length, unit: us, >=500 */
    unsigned short txStepNum;               /**< number of TX steps */
    FhcTxStep txSteps[C2K_FHC_MAX_TX_STEP]; /**< TX steps */
    unsigned short rxStepNum;               /**< number of RX steps */
    FhcRxStep rxSteps[C2K_FHC_MAX_RX_STEP]; /**< RX steps */
} C2kTestCmdFhc;

/**
 * \ingroup C2KStruct
 * \details FHC result
 */
typedef struct
{
    unsigned short pdNum;                  /**< number of power detector results */
    unsigned short rssiNum;                /**< number of RSSI results */
    double pdResult[C2K_FHC_MAX_TX_STEP];  /**< power detector results in dBm */
    double rssiMain[C2K_FHC_MAX_RX_STEP];  /**< main path RSSI results in dBm */
    double rssiDiv[C2K_FHC_MAX_RX_STEP];   /**< diversity path RSSI results in dBm */
} C2kTestResultFhc;

/**
 * \ingroup C2KStruct
 * \details NSFT power up parameters
 */
typedef struct
{
    unsigned short band;       /**< band class */
    unsigned short channel;    /**< channel number */
    unsigned char walshCode;   /**< traffic channel walsh code */
    unsigned char radioConfig; /**< reverse radio config */
} C2kNsftCmdPowerUp;

/**
 * \ingroup C2KStruct
 * \details NSFT status of command execution
 */
typedef struct
{
    unsigned int status;       /**< NSFT status */
} C2kNsftResultStatus;

/**
 * \ingroup C2KStruct
 * \details NSFT get FER command parameters
 */
typedef struct
{
    unsigned short band;      /**< band class */
    unsigned short channel;   /**< channel number */
    unsigned short numFrames; /**< number of FER test frames */
    unsigned char enableAfc;  /**< enable AFC, 0: disable, 1: enable */
} C2kNsftCmdFer;

/**
 * \ingroup C2KStruct
 * \details NSFT get FER result
 */
typedef struct
{
    unsigned int status;        /**< NSFT status */
    unsigned short badFrames;   /**< number of bad frames */
    unsigned short totalFrames; /**< number of total test frames */
} C2kNsftResultFer;

/**
 * \ingroup C2KStruct
 * \details NSFT set TX power command parameters
 */
typedef struct
{
    unsigned short band;        /**< band class */
    unsigned short channel;     /**< channel number */
    double txPower;             /**< TX power in dBm */
} C2kNsftCmdSetTxPower;

/**
 * \ingroup C2KStruct
 * \details NSFT get RSSI command parameters
 */
typedef struct
{
    unsigned short band;    /**< band class */
    unsigned short channel; /**< channel number */
} C2kNsftCmdRssi;

/**
 * \ingroup C2KStruct
 * \details NSFT get RSSI result
 */
typedef struct
{
    unsigned int status;     /**< NSFT status */
    unsigned short pnOffset; /**< PN offset */
    unsigned short strength; /**< signal strength */
    double mainRssi;         /**< main path RSSI */
    double divRssi;          /**< diversity path RSSI */
} C2kNsftResultRssi;

/**
 * \ingroup C2KStruct
 * \details NSFT list mode command parameters
 */
typedef struct
{
    unsigned char count;                                                          /**< count of list steps */
    unsigned char offset[C2K_NSFT_LIST_MODE_NUM];                                 /**< occupied frames of the step */
    unsigned short band[C2K_NSFT_LIST_MODE_NUM];                                  /**< band class of the step */
    unsigned short channel[C2K_NSFT_LIST_MODE_NUM];                               /**< channel number of the step */
    unsigned char walshCode[C2K_NSFT_LIST_MODE_NUM];                              /**< traffic channel walsh code of the step */
    unsigned char radioConfig[C2K_NSFT_LIST_MODE_NUM];                            /**< reverse radio configuration of the step */
    unsigned short numFrames[C2K_NSFT_LIST_MODE_NUM];                             /**< number of FER test frames of the step */
    unsigned char txPowerCount[C2K_NSFT_LIST_MODE_NUM];                           /**< count of TX power levels of the step */
    double txPowerLevel[C2K_NSFT_LIST_MODE_NUM][C2K_NSFT_LIST_MODE_TX_PWR_COUNT]; /**< TX power levels array in dBm of the step */
    unsigned char rssiCount[C2K_NSFT_LIST_MODE_NUM];                              /**< count of RSSI levels of the step */
} C2kNsftCmdListMode;

/**
 * \ingroup C2KStruct
 * \details NSFT list mode result
 */
typedef struct
{
    unsigned int status;                                                    /**< NSFT status */
    unsigned char count;                                                    /**< count of list steps */
    unsigned char index[C2K_NSFT_LIST_MODE_NUM];                            /**< index of the step */
    unsigned short band[C2K_NSFT_LIST_MODE_NUM];                            /**< band class of the step */
    unsigned short channel[C2K_NSFT_LIST_MODE_NUM];                         /**< channel number of the step */
    unsigned char badFrames[C2K_NSFT_LIST_MODE_NUM];                        /**< number of bad frames of the step */
    unsigned char totalFrames[C2K_NSFT_LIST_MODE_NUM];                      /**< number of total frames of the step */
    double mainRssi[C2K_NSFT_LIST_MODE_NUM][C2K_NSFT_LIST_MODE_RSSI_COUNT]; /**< main path RSSI levels of the step */
    double divRssi[C2K_NSFT_LIST_MODE_NUM][C2K_NSFT_LIST_MODE_RSSI_COUNT];  /**< diversity path RSSI levels of the step */
} C2kNsftResultListMode;

/**
 * \ingroup C2KStruct
 * \details NSFT test mode command
 */
typedef struct
{
    unsigned int reserved;
} C2kNsftCmdTestMode;

/**
 * \ingroup C2KStruct
 * \details MEID get/set parameters
 */
typedef struct
{
    char meid[17]; /**< Null-terminated 14 hexadecimal format MEID string */
    char esn[9];   /**< Null-terminated 8 hexadecimal format ESN string, ignored in set command */
} C2kTestCmdGetSetMeid;

/**
 * \ingroup C2KStruct
 * \details Specify which bands to get the LNA configuration
 */
typedef struct
{
    unsigned char bandNum;      /**< number of band classes in array */
    unsigned char bandClass[5]; /**< band class */
} C2kTestCmdGetRxLnaConfig;

/**
 * \ingroup C2KStruct
 * \details How many LNA modes are supported and calibration power of each
 */
typedef struct
{
    unsigned char lnaNum;          /**< number of LNA modes */
    short value[C2K_LNA_MODE_NUM]; /**< calibration power value */
} C2kLnaCalibrationPower;

/**
 * \ingroup C2KStruct
 * \details High/low power mode
 */
typedef struct
{
    C2kLnaCalibrationPower hpm; /**< high power mode parameter */
    C2kLnaCalibrationPower lpm; /**< low power mode parameter */
} C2kRxLnaCalibrationPower;

/**
 * \ingroup C2KStruct
 * \details Limit value of calibration power
 */
typedef struct
{
    short min; /**< min limit */
    short max; /**< max limit */
} C2kRxPowerRange;

/**
 * \ingroup C2KStruct
 * \details High/low power mode
 */
typedef struct
{
    C2kRxPowerRange hpm[C2K_LNA_MODE_NUM]; /**< high power mode limit */
    C2kRxPowerRange lpm[C2K_LNA_MODE_NUM]; /**< high power mode limit */
} C2kRxLnaPowerRange;

/**
 * \ingroup C2KStruct
 * \details LNA configuration of one band
 */
typedef struct
{
    C2kRxLnaCalibrationPower power[3]; /**< calibration power indexed by 3 RX paths */
    C2kRxLnaPowerRange range[3];       /**< limit range indexed by 3 RX paths */
} C2kRxLnaConfig;

/**
 * \ingroup C2KStruct
 * \details LNA configurations
 */
typedef struct
{
    C2kRxLnaConfig lnaConfig[5];      /**< LNA config indexed by 5 bands */
} C2kTestResultRxLnaConfig;

/**
 * \ingroup C2KStruct
 * \details Get TX gain setting command
 */
typedef struct
{
    unsigned int rfMode;              /**< RF mode, 0: 1xRTT, 1: EVDO */
} C2kTestCmdGetTxGainSetting;

/**
 * \ingroup C2KStruct
 * \details TX gain setting result
 */
typedef struct
{
    unsigned char rfMode;             /**< RF mode, 0: 1xRTT, 1: EVDO */
    unsigned char opMode;             /**< PA operating mode, 0: ET mode, 1: DPD mode, 2: APT mode */
    unsigned char pgaIndex;           /**< PGA index */
    unsigned char paIndex;            /**< PA index */
    unsigned char powerDetectorIndex; /**< power detector index */
    unsigned char pgaMode;            /**< PGA A/B table mode */
    int desiredTxGain;                /**< desiredTxGain = pgaGain + digitalGain */
    int targetGain;                   /**< targetGain = paGain + pgaGain + digitalGain - compensation */
    int paGain;                       /**< PA gain in Q5 */
    int pgaGain;                      /**< PGA gain in Q5 */
    int digitalGain;                  /**< digital gain in Q5 */
    int powerDetectorGain;            /**< power detector gain in Q5 */
    int couplerLossGain;              /**< coupler loss gain in Q5 */
    int backOffGain;                  /**< back off gain in Q5 */
    int compensation;                 /**< compensation in Q5 */
    int powerDetectorValue;           /**< power detector value in Q5 */
} C2kTestResultGetTxGainSetting;

/**
 * \ingroup C2KStruct
 * \details Get RX gain setting command
 */
typedef struct
{
    unsigned int rfMode;              /**< RF mode, 0: 1xRTT, 1: EVDO */
    unsigned int path;                /**< path index, 0: main path, 1: diversity path, 2: second path */
} C2kTestCmdGetRxGainSetting;

/**
 * \ingroup C2KStruct
 * \details RX gain setting result
 */
typedef struct
{
    unsigned char rfMode;             /**< RF mode, 0: 1xRTT, 1: EVDO */
    unsigned char pathBitmap;         /**< bitmap of RX path, bit0: main path, bit1: diversity path, bit2: second path */
    unsigned char pgaIndex;           /**< PGA index */
    unsigned char lnaIndex;           /**< LNA index */
    unsigned char eLnaIndex;          /**< eLNA index */
    unsigned char mLnaIndex;          /**< mLNA index */
    int digitalGain;                  /**< digital gain in Q5 */
    int rfGain;                       /**< RF gain in Q5 */
    int pathLoss;                     /**< path loss in Q5 */
    int wideBandPower;                /**< wide-band power in Q5 */
    int inBandPower;                  /**< in-band power in Q5 */
    int rssi;                         /**< RSSI in Q5 */
    int dcEstI;                       /**< DC of I path */
    int dcEstQ;                       /**< DC of Q path */
} C2kTestResultGetRxGainSetting;

/**
 * \ingroup C2KStruct
 * \details Set BPI data command
 */
typedef struct
{
    unsigned int rfMode;              /**< RF mode, 0: 1xRTT, 1: EVDO */
    unsigned int bpiData;             /**< BPI data */
} C2kTestCmdSetBpiData;

/**
 * \ingroup C2KStruct
 * \details Get BPI data command
 */
typedef struct
{
    unsigned int rfMode;              /**< RF mode, 0: 1xRTT, 1: EVDO */
} C2kTestCmdGetBpiData;

/**
 * \ingroup C2KStruct
 * \details Get BPI data result
 */
typedef struct
{
    unsigned int bpiData;             /**< BPI data */
} C2kTestResultGetBpiData;

/**
 * \ingroup C2KStruct
 * \details RX AGC configuration parameters Gen97
 */
typedef struct
{
    unsigned char rfMode;             /**< RF mode, 0: 1xRTT, 1: EVDO */
    unsigned char pathBitmap;         /**< bitmap of RX path, bit0: main path, bit1: diversity path, bit2: second path */
    unsigned char testMode;           /**< test mode, 0: fixed gain mode, 1: manual mode */
    unsigned char agcFsm;             /**< AGC Finite State Machine, 0: fast, 1: steady, 2: ICS, 3: CAL, 4: fix gain */
    unsigned char powerMode;          /**< power mode, 0: high power mode, 1: low power mode */
    unsigned char lnaMode;            /**< LNA mode index 0 ~ 7 */
    unsigned char pgaIndex;           /**< PGA index */
    unsigned char agcDcFixBmp;        /**< indicate which parameter to be fixed, bit3: dig_dc, bit2: dig_gain, bit1: rf_dc, bit0: rf_gain */
    int digitalGain;                  /**< digital gain in Q5 */
} C2kTestCmdRxAgcConfigV7;

/**
 * \ingroup C2KStruct
 * \details Set DPD calibration flag command structure
 */
typedef struct
{
    unsigned char updateNvram;        /**< whether update flag to NVRAM or not, 0: not update, 1: update */
    unsigned char reserved[3];        /**< reserved */
    unsigned int dpdFlag;             /**< DPD calibration flag, 0: fail, 1: pass */
} C2kTestCmdSetDpdCalFlag;

/**
 * \ingroup C2KStruct
 * \details Get DPD calibration flag command structure
 */
typedef struct
{
    unsigned int reserved;            /**< reserved */
} C2kTestCmdGetDpdCalFlag;

/**
 * \ingroup C2KStruct
 * \details Get DPD calibration flag result structure
 */
typedef struct
{
    unsigned int dpdFlag;             /**< DPD calibration flag, 0: fail, 1: pass */
} C2kTestResultGetDpdCalFlag;

#ifdef __cplusplus
}
#endif

#endif
