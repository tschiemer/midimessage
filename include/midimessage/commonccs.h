/**
 * \file
 */

#ifndef MIDIMESSAGE_COMMONCCS_H
#define MIDIMESSAGE_COMMONCCS_H


namespace MidiMessage {

    typedef enum {
        CcBankSelect                                = 0x00, // 0  LSB
        CcModulationWheel                           = 0x01, // 1
        CcBreathController                          = 0x02, // 2

        CcFootPedal                                 = 0x04, // 4  MSB
        CcPortamentoTime                            = 0x05, // 5  MSB
        CcDataEntryMSB                              = 0x06, // 6  MSB
        CcVolume                                    = 0x07, // 7  MSB
        CcBalance                                   = 0x08, // 8  MSB

        CcPan                                       = 0x0A, // 10 MSB
        CcExpressionPedal                           = 0x0B, // 11
        CcEffectControl1                            = 0x0C, // 12
        CcEffectControl2                            = 0x0D, // 13

        CcGeneralPurposeController1                 = 0x10, // 16
        CcGeneralPurposeController2                 = 0x11, // 17
        CcGeneralPurposeController3                 = 0x12, // 18
        CcGeneralPurposeController4                 = 0x13, // 19

        // 0x20 - 0x3F = LSB for Controllers 0-31 (rarely implemented)

        CcDataEntryLSB                              = 0x26, // 38

        CcHoldPedal                                 = 0x40, // 64
        CcSustain                                   = CcHoldPedal,

        CcPortamento                                = 0x41, // 65
        CcSostenutoPedal                            = 0x42, // 66
        CcSoftPedal                                 = 0x43, // 67
        CcLegatoPedal                               = 0x44, // 68
        CcHoldPedal2                                = 0x45, // 69

        CcSoundController1                          = 0x46, // 70
        CcSoundVariation                            = CcSoundController1,
        CcSoundController2                          = 0x47, // 71
        CcTimbreContent                             = CcSoundController2,
        CcResonance                                 = CcSoundController2,
        CcSoundController3                          = 0x48, // 72
        CcReleaseTime                               = CcSoundController3,
        CcSoundController4                          = 0x49, // 73
        CcAttackTime                                = CcSoundController4,
        CcSoundController5                          = 0x4A, // 74
        CcBrightness                                = CcSoundController5,
        CcFrequencyCutoff                           = CcSoundController5,
        CcSoundController6                          = 0x4B, // 75
        CcSoundController7                          = 0x4C, // 76
        CcSoundController8                          = 0x4D, // 77
        CcSoundController9                          = 0x4E, // 78
        CcSoundController10                         = 0x4F, // 79
        CcGeneralPurposeController5                 = 0x50, // 80
        CcDecay                                     = CcGeneralPurposeController5,
        CcGeneralPurposeController6                 = 0x51, // 81
        CcHighPassFilter                            = CcGeneralPurposeController6,
        CcGeneralPurposeController7                 = 0x52, // 82
        CcGeneralPurposeController8                 = 0x53, // 83
        CcPortamentoControl                         = 0x54, // 84

        CcEffectDepth1                              = 0x5B, // 91
        CcExternalEffectDepth                       = CcEffectDepth1,
        CcReverbDepth                               = CcEffectDepth1,
        CcEffectDepth2                              = 0x5C, // 92
        CcTremoloDepth                              = CcEffectDepth2,
        CcEffectDepth3                              = 0x5D, // 93
        CcChorusDepth                               = CcEffectDepth3,
        CcEffectDepth4                              = 0x5E, // 94
        CcDetuneDepth                               = CcEffectDepth4,
        CcEffectDepth5                              = 0x5F, // 95
        CcPhaserDepth                               = CcEffectDepth5,
        CcDataIncrement                             = 0x60, // 96
        CcDataDecrement                             = 0x61, // 97

        CcNonRegisteredParameterLSB                 = 0x62, // 98 LSB
        CcNonRegisteredParameterMSB                 = 0x63, // 99 MSB
        CcRegisteredParameterLSB                    = 0x64, // 100 LSB
        CcRegisteredParameterMSB                    = 0x65, // 101 MSB

        CcAllSoundOff                               = 0x78, // 120
        CcResetAllControllers                       = 0x79, // 121
        CcLocalControl                              = 0x7A, // 122
        CcAllNotesOff                               = 0x7B, // 123
        CcOmniOff                                   = 0x7C, // 124
        CcOmniOn                                    = 0x7D, // 125
        CcMonoMode                                  = 0x7E, // 126
        CcPolyMode                                  = 0x7F, // 127
    } Cc_t;

}
#endif //MIDIMESSAGE_COMMONCCS_H
