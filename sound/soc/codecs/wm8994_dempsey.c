/*
 * wm8994_aries.c  --  WM8994 ALSA Soc Audio driver related Aries
 *
 *  Copyright (C) 2010 Samsung Electronics.
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 */
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <asm/gpio.h> 
#include <plat/gpio-cfg.h> 
#include <plat/map-base.h>
#include <mach/regs-clock.h> 
#include <mach/gpio.h> 
#include "wm8994.h"

#include "A1026_regs_dempsey.h" //hdlnc_ldj_0415_A1026
#include "A1026_dev.h"  //hdlnc_ldj_0415_A1026
#include "A1026_i2c_drv.h"  //hdlnc_ldj_0415_A1026

#ifdef CONFIG_SND_VOODOO
#include "wm8994_voodoo.h"
#endif


//------------------------------------------------
//		Debug Feature
//------------------------------------------------
#define SUBJECT "wm8994_aries.c"

//------------------------------------------------
// Definitions of tunning volumes for wm8994
//------------------------------------------------

//------------------------------------------------
// Common

// DAC
#define TUNING_DAC1L_VOL		0xC0		// 610h
#define TUNING_DAC1R_VOL		0xC0		// 611h
#define TUNING_DAC2L_VOL		0xC0		// 612h
#define TUNING_DAC2R_VOL		0xC0		// 613h

// DAC QIK
#define TUNING_QIK_DAC1L_VOL		0xC0		// 610h
#define TUNING_QIK_DAC1R_VOL		0xC0		// 611h
#define TUNING_QIK_DAC2L_VOL		0xC0		// 612h
#define TUNING_QIK_DAC2R_VOL		0xC0		// 613h

// Speaker
#define TUNING_SPKMIXL_ATTEN		0x0		// 22h
#define TUNING_SPKMIXR_ATTEN		0x0		// 23h

// Headset
#define TUNING_EAR_OUTMIX5_VOL		0x0		// 31h
#define TUNING_EAR_OUTMIX6_VOL		0x0		// 32h

//------------------------------------------------
// Normal
// Speaker
#define TUNING_MP3_SPKL_VOL		0x3D		// 26h
#define TUNING_MP3_SPKR_VOL		0x3E		// 27h
#define TUNING_MP3_CLASSD_VOL		0x6		// 25h

// Headset
#define TUNING_MP3_OUTPUTL_VOL		0x35		// 1Ch
#define TUNING_MP3_OUTPUTR_VOL		0x35		// 1Dh
#define TUNING_MP3_OPGAL_VOL		0x39		// 20h
#define TUNING_MP3_OPGAR_VOL		0x39		// 21h

// Dual
#define TUNING_MP3_DUAL_OUTPUTL_VOL		0x21		// 1Ch
#define TUNING_MP3_DUAL_OUTPUTR_VOL		0x21		// 1Dh
// Extra_Dock_speaker
#define TUNING_MP3_EXTRA_DOCK_SPK_OPGAL_VOL		0x39		// 20h
#define TUNING_MP3_EXTRA_DOCK_SPK_OPGAR_VOL		0x39		// 21h
#define TUNING_EXTRA_DOCK_SPK_OUTMIX5_VOL		0x1		// 31h
#define TUNING_EXTRA_DOCK_SPK_OUTMIX6_VOL		0x1		// 32h
#define TUNING_MP3_EXTRA_DOCK_SPK_VOL	0x0		//1Eh

//------------------------------------------------
// Ringtone
// Speaker
#define TUNING_RING_SPKL_VOL		0x3D		// 26h
#define TUNING_RING_SPKR_VOL		0x3E		// 27h
#define TUNING_RING_CLASSD_VOL		0x6		// 25h


// Headset
#define TUNING_RING_OUTPUTL_VOL		0x34		// 1Ch
#define TUNING_RING_OUTPUTR_VOL		0x34		// 1Dh
#define TUNING_RING_OPGAL_VOL		0x39		// 20h
#define TUNING_RING_OPGAR_VOL		0x39		// 21h

// Headset QIK
#define TUNING_QIK_OUTPUTL_VOL		0x34		// 1Ch
#define TUNING_QIK_OUTPUTR_VOL		0x34		// 1Dh
#define TUNING_QIK_OPGAL_VOL		0x39		// 20h
#define TUNING_QIK_OPGAR_VOL		0x39		// 21h

// Dual
#define TUNING_RING_DUAL_OUTPUTL_VOL		0x1E		// 1Ch
#define TUNING_RING_DUAL_OUTPUTR_VOL		0x1E		// 1Dh

//------------------------------------------------
// Call
// Speaker
#define TUNING_CALL_SPKL_VOL		0x3F		// 26h
#define TUNING_CALL_CLASSD_VOL		0x7		// 25h

// Headset
#define TUNING_CALL_OUTPUTL_VOL		0x30		// 1Ch
#define TUNING_CALL_OUTPUTR_VOL		0x30		// 1Dh
#define TUNING_CALL_OPGAL_VOL		0x3B		// 20h
#define TUNING_CALL_OPGAR_VOL		0x3B		// 21h

// Receiver
#define TUNING_RCV_OUTMIX5_VOL		0x0		// 31h
#define TUNING_RCV_OUTMIX6_VOL 		0x0		// 32h
#define TUNING_RCV_OPGAL_VOL		0x3F	// 20h
#define TUNING_RCV_OPGAR_VOL		0x3F	// 21h
#define TUNING_HPOUT2_VOL		0x0		// 1Fh

#define TUNING_RCV_LOOP_OUTMIX5_VOL		0x0		// 31h
#define TUNING_RCV_LOOP_OUTMIX6_VOL 		0x0		// 32h
#define TUNING_RCV_LOOP_OPGAL_VOL		0x28	// 20h
#define TUNING_RCV_LOOP_OPGAR_VOL		0x28	// 21h
#define TUNING_HPOUT2_LOOP_VOL		0x0		// 1Fh

// Call Main MIC
#define TUNING_CALL_RCV_INPUTMIX_VOL	0x0D// 18h
#define TUNING_CALL_RCV_MIXER_VOL	0x00	// 29h 0dB
#define TUNING_CALL_RCV_LOOP_INPUTMIX_VOL	0x0D// 18h
#define TUNING_CALL_RCV_LOOP_MIXER_VOL	0x00	// 29h 0dB
// Call Sub MIC
#define TUNING_CALL_RCV_INPUTMIX_SUB_VOL	0xD		// 1bh
#define TUNING_CALL_RCV_MIXER_SUB_VOL	0x00	// 2ah 0dB
#define TUNING_CALL_RCV_LOOP_INPUTMIX_SUB_VOL	0xD		// 1bh
#define TUNING_CALL_RCV_LOOP_MIXER_SUB_VOL	0x00	// 2ah 0dB



#define TUNING_CALL_SPK_INPUTMIX_VOL	0x7		// 18h
#define TUNING_CALL_SPK_MIXER_VOL	0x0		// 29h 0dB
// Call Ear MIC
#define TUNING_CALL_EAR_INPUTMIX_VOL	0x1a		// 1Ah
//Call Sub MIC
#define TUNING_CALL_SPKMIC_INPUTMIX_VOL	0x8	// 1bh
//[JKB 2010.12.06 add
//------------------------------------------------
// VoIP Call
#if (defined FEATURE_VOIP)
// Receiver
#define TUNING_VOIP_RCV_AIF1DAC_BOOST           0x01 // 301h // [DJ02-1720]
#define TUNING_VOIP_RCV_OPGAL_VOL               0x39 // 20h // [DJ05-2239] VoIP 0x34->0x39 (0dB)
#define TUNING_VOIP_RCV_OPGAR_VOL               0x39 // 21h // [DJ05-2239] VoIP 0x34->0x39 (0dB)

// Headset(3pole/4pole)
#define TUNING_VOIP_EAR_OUTPUTL_VOL             0x35 // 1Ch // [DJ05-2239] VoIP 0x38->0x35 (-4dB)
#define TUNING_VOIP_EAR_OUTPUTR_VOL             0x35 // 1Dh // [DJ05-2239] VoIP 0x38->0x35 (-4dB)
#define TUNING_VOIP_EAR_OPGAL_VOL               0x3F // 20h // [DI25-1834] VoIP 0x3F (+6dB)
#define TUNING_VOIP_EAR_OPGAR_VOL               0x3F // 21h // [DI25-1834] VoIP 0x3F (+ddB)
#define TUNING_VOIP_EAR_AIF1DAC_BOOST           0x01 // 301h // [DJ05-2239] VoIP 0x01 (+6dB)

// Speaker
#define TUNING_VOIP_SPKL_VOL                    0x39 // 26h // [DJ05-2239] 0x3C->0x39
#define TUNING_VOIP_CLASSD_VOL                  0x07 // 25h
#define TUNING_VOIP_SPK_AIF1DAC_BOOST           0x02 // 301h // [DJ05-2239] 0x01->0x02

// VoIP Call Main MIC
#define TUNING_VOIP_RCV_INPUTMIX_VOL            0x17 // 18h // [DJ05-2239] VoIP 0x17(+18.0dB)
#define TUNING_VOIP_SPK_INPUTMIX_VOL            0x1A // 18h // [DJ05-2239] VoIP 0x1A(+22.5dB)
#define TUNING_VOIP_3P_INPUTMIX_VOL             0x1A // 18h // [DJ05-2239] VoIP 0x1A(+22.5dB)
#define TUNING_VOIP_MAIN_AIF1ADCL_VOL           0xEF // 400h // [DI28-2336] VoIP 0xEF(max, +17.625dB)
#define TUNING_VOIP_MAIN_AIF1ADCR_VOL           0xEF // 401h

// VoIP Call Ear MIC
#define TUNING_VOIP_EAR_INPUTMIX_VOL            0x17 // 1Ah // [DJ05-2239] VoIP 0x17(+22.5dB)
#define TUNING_VOIP_EAR_AIF1ADCL_VOL            0xEF // 400h
#define TUNING_VOIP_EAR_AIF1ADCR_VOL            0xEF // 401h // [DI28-2336] VoIP 0xEF(max, +17.625dB)
#endif
//JKB]

//------------------------------------------------
// FM Radio
// Speaker
#define TUNING_FMRADIO_SPKL_VOL		0x3E		// 26h
#define TUNING_FMRADIO_CLASSD_VOL	0x5		// 25h

// Headset
#define TUNING_FMRADIO_OUTPUTL_VOL	0x3C		// 1Ch
#define TUNING_FMRADIO_OUTPUTR_VOL	0x3C		// 1Dh
#define TUNING_FMRADIO_OPGAL_VOL	0x39		// 20h
#define TUNING_FMRADIO_OPGAR_VOL	0x39		// 21h

//Input
#define TUNING_DAC1L_RADIO_VOL		0xA8		// 402h
#define TUNING_DAC1R_RADIO_VOL		0xA8		// 403h

//Dual
#define TUNING_DUAL_DAC1L_RADIO_VOL		0x70		// 402h
#define TUNING_DUAL_DAC1R_RADIO_VOL		0x70		// 403h

// FM Radio Input
#define TUNING_FMRADIO_EAR_INPUTMIXL_VOL	0x0B		// 19h
#define TUNING_FMRADIO_EAR_INPUTMIXR_VOL	0x0B		// 1Bh

#define TUNING_FMRADIO_SPK_INPUTMIXL_VOL	0x0F		// 19h
#define TUNING_FMRADIO_SPK_INPUTMIXR_VOL	0x0F		// 1Bh

// ear loopback
#define TUNING_LOOPBACK_EAR_INPUTMIX_VOL 0x1D // 1Ah

#define TUNING_LOOPBACK_OUTPUTL_VOL 0x37 // 1Ch
#define TUNING_LOOPBACK_OUTPUTR_VOL 0x37 // 1Dh
#define TUNING_LOOPBACK_OPGAL_VOL 0x39 // 20h
#define TUNING_LOOPBACK_OPGAR_VOL 0x39 // 21h

//------------------------------------------------
// Recording
// Main MIC
#define TUNING_RECORD_MAIN_INPUTLINE_VOL	0x13		// 18h
#define TUNING_RECORD_MAIN_AIF1ADCL_VOL	0xcd		// 400h
#define TUNING_RECORD_MAIN_AIF1ADCR_VOL	0xcd		// 401h
#define TUNING_RECOGNITION_MAIN_INPUTLINE_VOL	0x1B		// 18h
#define TUNING_RECOGNITION_MAIN_AIF1ADCL_VOL	0xDC		// 400h
#define TUNING_RECOGNITION_MAIN_AIF1ADCR_VOL	0xD8		// 401h
// Ear MIC
#define TUNING_RECORD_SUB_INPUTMIX_VOL	0x0b		// 1Ah
#define TUNING_RECORD_SUB_AIF1ADCL_VOL	0xC0		// 400h
#define TUNING_RECORD_SUB_AIF1ADCR_VOL	0xC0		// 401h
#define TUNING_RECOGNITION_SUB_INPUTMIX_VOL	0x0F		// 1Ah
#define TUNING_RECOGNITION_SUB_AIF1ADCL_VOL	0xC0		// 400h
#define TUNING_RECOGNITION_SUB_AIF1ADCR_VOL	0xC0		// 401

//Sub Mic
#define TUNING_RECORD_SUB_INPUTLINE_VOL	0x16		// 18h
#define TUNING_RECORD_SPKSUB_AIF1ADCL_VOL	0xcd		// 400h
#define TUNING_RECORD_SPKSUB_AIF1ADCR_VOL	0xcd		// 401h
//------------------------------------------------

// tty
#define TUNING_TTY_RCV_INPUTMIX_VOL	0x19		// 18h
#define TUNING_TTY_OUTMIX5_VOL		0x0		// 31h
#define TUNING_TTY_OUTMIX6_VOL		0x0		// 32h
#define TUNING_TTY_OUTPUTL_VOL		0x30		// 1Ch
#define TUNING_TTY_OUTPUTR_VOL		0x30		// 1Dh
#define TUNING_TTY_DAC1L_VOL		0xC0		// 610h
#define TUNING_TTY_DAC1R_VOL		0xC0		// 611h

//-------------------------------------------------
// audience
#define TUNING_AUDIENCE_RCV_OUTMIX5_VOL		0x0		// 31h
#define TUNING_AUDIENCE_RCV_OUTMIX6_VOL 		0x0		// 32h

#define TUNING_AUDIENCE_RCV_OPGAL_VOL		0x3F	// 20h
#define TUNING_AUDIENCE_RCV_OPGAR_VOL		0x3F	// 21h

#define TUNING_AUDIENCE_RCV_OPGAL_VOL_REV		0x3B	// 20h
#define TUNING_AUDIENCE_RCV_OPGAR_VOL_REV		0x3B	// 21h

#define TUNING_AUDIENCE_HPOUT2_VOL		0x0		// 1Fh


// audience Call Main MIC
#define TUNING_CALL_AUDIENCE_RCV_INPUTMIX_VOL	0xD	// 18h

#define TUNING_CALL_AUDIENCE_RCV_INPUTMIX_VOL_REV	0xB	// 18h

#define TUNING_CALL_AUDIENCE_RCV_MIXER_VOL	0x00	// 29h 0dB
// audience Call Sub MIC
#define TUNING_CALL_AUDIENCE_RCV_INPUTMIX_SUB_VOL	0xD	// 1bh

#define TUNING_CALL_AUDIENCE_RCV_INPUTMIX_SUB_VOL_REV	0xB	// 1bh

#define TUNING_CALL_AUDIENCE_RCV_MIXER_SUB_VOL	0x00	// 2ah 0dB

// audience
#define TUNING_AUDIENCE_RCV_LOOP_OUTMIX5_VOL		0x0		// 31h
#define TUNING_AUDIENCE_RCV_LOOP_OUTMIX6_VOL 		0x0		// 32h

#define TUNING_AUDIENCE_RCV_LOOP_OPGAL_VOL		0x28	// 20h
#define TUNING_AUDIENCE_RCV_LOOP_OPGAR_VOL		0x28	// 21h

#define TUNING_AUDIENCE_RCV_LOOP_OPGAL_VOL_REV		0x28	// 20h
#define TUNING_AUDIENCE_RCV_LOOP_OPGAR_VOL_REV		0x28	// 21h

#define TUNING_AUDIENCE_LOOP_HPOUT2_VOL		0x0		// 1Fh


// audience Call Main MIC
#define TUNING_CALL_AUDIENCE_RCV_LOOP_INPUTMIX_VOL	0xD	// 18h

#define TUNING_CALL_AUDIENCE_RCV_LOOP_INPUTMIX_VOL_REV	0xB	// 18h

#define TUNING_CALL_AUDIENCE_RCV_LOOP_MIXER_VOL	0x00	// 29h 0dB
// audience Call Sub MIC
#define TUNING_CALL_AUDIENCE_RCV_LOOP_INPUTMIX_SUB_VOL	0xD	// 1bh

#define TUNING_CALL_AUDIENCE_RCV_LOOP_INPUTMIX_SUB_VOL_REV	0xB	// 1bh

#define TUNING_CALL_AUDIENCE_RCV_LOOP_MIXER_SUB_VOL	0x00	// 2ah 0dB


//-------------------------------------------------
// audience
#define TUNING_AUDIENCE_SPK_OUTMIX5_VOL		0x0		// 31h
#define TUNING_AUDIENCE_SPK_OUTMIX6_VOL 		0x0		// 32h
#define TUNING_AUDIENCE_SPK_OPGAL_VOL		0x3F	// 20h
#define TUNING_AUDIENCE_SPK_OPGAR_VOL		0x3F	// 21h
#define TUNING_AUDIENCE_SPK_VOL		0x0		// 1Fh


// audience Call Main MIC
#define TUNING_CALL_AUDIENCE_SPK_INPUTMIX_VOL	0xB	// 18h
#define TUNING_CALL_AUDIENCE_SPK_MIXER_VOL	0x10	// 29h 0dB
// audience Call Sub MIC
#define TUNING_CALL_AUDIENCE_SPK_INPUTMIX_SUB_VOL	0x8	// 1bh
#define TUNING_CALL_AUDIENCE_SPK_MIXER_SUB_VOL	0x80	// 2ah 0dB

//--------------------------------------------------
extern int hw_version_check(void);
extern short int get_headset_status(void);		// For ear-jack control(MIC-Bias)
extern void set_recording_status(int value);	// For preventing MIC Bias off on using MIC.
extern unsigned int HWREV;  //hdlnc_ldj_0416_A1026

int a1026_mode_status = -1;

int audio_init(void)
{

	//CODEC LDO SETTING
	if (gpio_is_valid(GPIO_CODEC_LDO_EN))
	{
		if (gpio_request(GPIO_CODEC_LDO_EN, "CODEC_LDO_EN"))
			DEBUG_LOG_ERR("Failed to request CODEC_LDO_EN! \n");
		gpio_direction_output(GPIO_CODEC_LDO_EN, 0);
	}

	s3c_gpio_setpull(GPIO_CODEC_LDO_EN, S3C_GPIO_PULL_NONE);
	
	// For preserving output of codec related pins.
	s3c_gpio_slp_cfgpin(GPIO_CODEC_LDO_EN, S3C_GPIO_SLP_PREV);

	//CODEC XTAL CLK SETTING
	//b4 : AP Gpio emul, B5 : CODEC_XTAL_EN 

	s3c_gpio_slp_setpull_updown(GPIO_CODEC_LDO_EN, S3C_GPIO_PULL_NONE);

	// FM radio reset pin control
	//s3c_gpio_slp_cfgpin(GPIO_FM_RST, S3C_GPIO_SLP_PREV);

	//hdlnc_ysyim_2010-05-04
	gpio_direction_output(GPIO_MICBIAS_EN,1);
	s3c_gpio_slp_cfgpin(GPIO_MICBIAS_EN, S3C_GPIO_SLP_PREV);
	//gpio_set_value(GPIO_MICBIAS_EN, 0);
	return 0;

}

int audio_power(int en)
{
	u32 val;

	if(en)
	{
		// Forbid to turn off MCLK in sleep mode.
		val = __raw_readl(S5P_SLEEP_CFG);
		val |= (S5P_SLEEP_CFG_USBOSC_EN);
		__raw_writel(val , S5P_SLEEP_CFG);

		// Turn on LDO for codec.
		gpio_set_value(GPIO_CODEC_LDO_EN, 1);

		msleep(10);	// Wait for warming up.

		// Turn on master clock.
#if (defined CONFIG_SND_ARIES_WM8994_MASTER)
		if(hw_version_check())
		{
			__raw_writel(__raw_readl(S5P_OTHERS) | (3<<8) , S5P_OTHERS); 
			__raw_writel(__raw_readl(S5P_CLK_OUT) | (0x1) , S5P_CLK_OUT);	
		}
#endif
	}
	else
	{
		// Turn off LDO for codec.
		gpio_set_value(GPIO_CODEC_LDO_EN, 0);

		msleep(125);	// Wait to turn off codec entirely.

		// Turn on master clock.
#if (defined CONFIG_SND_ARIES_WM8994_MASTER)
		__raw_writel(__raw_readl(S5P_OTHERS) & (~(0x3<<8)) , S5P_OTHERS);
		__raw_writel(__raw_readl(S5P_CLK_OUT) & (0xFFFFFFFE) , S5P_CLK_OUT);
#endif
		// Allow to turn off MCLK in sleep mode.
		val = __raw_readl(S5P_SLEEP_CFG);
		val &= ~(S5P_SLEEP_CFG_USBOSC_EN);
		__raw_writel(val , S5P_SLEEP_CFG);
	}
	a1026_mode_status = -1;
	DEBUG_LOG("AUDIO POWER COMPLETED : %d", en);

	return 0;
}

void audio_ctrl_mic_bias_gpio(int enable)
{
	int headset_state;
	
	if(enable)
	{
		DEBUG_LOG("enable = [%d]", enable);
		set_recording_status(1);
		gpio_set_value(GPIO_MICBIAS_EN, 1);
	}
	else
	{
		set_recording_status(0);
		headset_state = get_headset_status();


		if((HWREV == 0x04) || (HWREV == 0x08) || (HWREV == 0x0C) || (HWREV == 0x02) || (HWREV == 0x0A)) //0x08:00, 0x04:01, 0x0C:02, 0x02:03, 0x0A:04
		{
			if( headset_state == 0 ||headset_state == 0x1 << 5 )	// TV out or no headset
			{
				DEBUG_LOG("enable = [%d]", enable);
				gpio_set_value(GPIO_MICBIAS_EN, 0);
			}
		}
		else // from 05 board (0x06: 05, 0x0E: 06)
		{
			gpio_set_value(GPIO_MICBIAS_EN, 0); //hdlnc_ysyim_2010-05-04
		}		
		
	}
}

/*Audio Routing routines for the universal board..wm8994 codec*/
void wm8994_disable_playback_path(struct snd_soc_codec *codec, enum playback_path path)
{
	u16 val;
	struct wm8994_priv *wm8994 = codec->private_data;

	DEBUG_LOG("Path = [%d]", path);

	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);	
	
	switch(path)
	{
		case RCV:		
			//Disbale the HPOUT2
			val &= ~(WM8994_HPOUT2_ENA_MASK);
			wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);
			
			// Disable left MIXOUT
			val = wm8994_read(codec,WM8994_OUTPUT_MIXER_1);
			val &= ~(WM8994_DAC1L_TO_MIXOUTL_MASK);
			wm8994_write(codec,WM8994_OUTPUT_MIXER_1,val);

			// Disable right MIXOUT
			val = wm8994_read(codec,WM8994_OUTPUT_MIXER_2);
			val &= ~(WM8994_DAC1R_TO_MIXOUTR_MASK);
			wm8994_write(codec,WM8994_OUTPUT_MIXER_2,val);

			// Disable HPOUT Mixer
			val = wm8994_read(codec,WM8994_HPOUT2_MIXER);
			val &= ~(WM8994_MIXOUTLVOL_TO_HPOUT2_MASK | WM8994_MIXOUTRVOL_TO_HPOUT2_MASK);
			wm8994_write(codec,WM8994_HPOUT2_MIXER,val);

			// Disable mixout volume control
			val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_3);
			val &= ~(WM8994_MIXOUTLVOL_ENA_MASK | WM8994_MIXOUTRVOL_ENA_MASK | WM8994_MIXOUTL_ENA_MASK | WM8994_MIXOUTR_ENA_MASK);
			wm8994_write(codec,WM8994_POWER_MANAGEMENT_3,val);			
			break;

		case SPK:
			//Disbale the SPKOUTL
			val &= ~(WM8994_SPKOUTL_ENA_MASK); 
			wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

			// Disable SPKLVOL
			val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_3);
			val &= ~(WM8994_SPKLVOL_ENA_MASK);
			wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, val);

			// Disable SPKOUT mixer
			val = wm8994_read(codec,WM8994_SPKOUT_MIXERS);
			val &= ~(WM8994_SPKMIXL_TO_SPKOUTL_MASK | 
				WM8994_SPKMIXR_TO_SPKOUTL_MASK | WM8994_SPKMIXR_TO_SPKOUTR_MASK);
			wm8994_write(codec, WM8994_SPKOUT_MIXERS, val);
			
			//Mute Speaker mixer
			val = wm8994_read(codec,WM8994_SPEAKER_MIXER);
			val &= ~(WM8994_DAC1L_TO_SPKMIXL_MASK);
			wm8994_write(codec, WM8994_SPEAKER_MIXER ,val);
			break;

		case HP:
			if(wm8994->codec_state & CALL_ACTIVE)
			{
			val = wm8994_read(codec, WM8994_DAC1_LEFT_VOLUME);
			val &= ~(0x02C0);
			val |= 0x02C0;
			wm8994_write(codec, WM8994_DAC1_LEFT_VOLUME, 0x02C0);

			val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME);
			val &= ~(0x02C0);
			val |= 0x02C0;
			wm8994_write(codec, WM8994_DAC1_RIGHT_VOLUME, 0x02C0);

			val = wm8994_read(codec, WM8994_ANALOGUE_HP_1);
			val &= ~(0x0022);
			val |= 0x0022;
			wm8994_write(codec, WM8994_ANALOGUE_HP_1, 0x0022);

			val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);
			val &= ~(0x0);
			val |= 0x0;
			wm8994_write(codec, WM8994_OUTPUT_MIXER_1, 0x0);

			val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);
			val &= ~(0x0);
			val |= 0x0;
			wm8994_write(codec, WM8994_OUTPUT_MIXER_2, 0x0);

			val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_5);
			val &= ~(0x0300);
			val |= 0x0300;
			wm8994_write(codec, WM8994_POWER_MANAGEMENT_5, 0x0300);

			val = wm8994_read(codec, WM8994_CHARGE_PUMP_1);
			val &= ~(0x1F25);
			val |= 0x1F25;
			wm8994_write(codec, WM8994_CHARGE_PUMP_1, 0x1F25);
			}
			break;

		case BT :
			//R6(6h) - 0x0000
			val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_6);
			val &= ~(WM8994_AIF3_ADCDAT_SRC_MASK | WM8994_AIF2_ADCDAT_SRC_MASK | WM8994_AIF2_DACDAT_SRC_MASK | WM8994_AIF1_DACDAT_SRC_MASK);
			wm8994_write(codec, WM8994_POWER_MANAGEMENT_6, val);

			//R1056(420h) - 0x0200
			val = wm8994_read(codec, WM8994_AIF1_DAC1_FILTERS_1);
			val &= ~(WM8994_AIF1DAC1_MUTE_MASK | WM8994_AIF1DAC1_MONO_MASK);
			val |= (WM8994_AIF1DAC1_MUTE);
			wm8994_write(codec, WM8994_AIF1_DAC1_FILTERS_1, val); 
			break;

		case DUAL :
			if(wm8994->codec_state & CALL_ACTIVE)
			{
			val &= ~(WM8994_HPOUT1L_ENA_MASK |WM8994_HPOUT1R_ENA_MASK |WM8994_SPKOUTL_ENA_MASK);
			wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

			// ------------------ Ear path setting ------------------
			// Disable DAC1L to HPOUT1L path
			val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);
			val &= ~(WM8994_DAC1L_TO_HPOUT1L_MASK | WM8994_DAC1L_TO_MIXOUTL_MASK);
			wm8994_write(codec,WM8994_OUTPUT_MIXER_1, val);
			
			// Disable DAC1R to HPOUT1R path
			val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);
			val &= ~(WM8994_DAC1R_TO_HPOUT1R_MASK | WM8994_DAC1R_TO_MIXOUTR_MASK);
			wm8994_write(codec, WM8994_OUTPUT_MIXER_2, val);
			
			//Disable Charge Pump	
			val = wm8994_read(codec, WM8994_CHARGE_PUMP_1);
			val &= ~WM8994_CP_ENA_MASK ;
			val |=  WM8994_CP_ENA_DEFAULT ; // this is from wolfson	
			wm8994_write(codec, WM8994_CHARGE_PUMP_1, val);
			
			// Intermediate HP settings
			val = wm8994_read(codec, WM8994_ANALOGUE_HP_1); 	
			val &= ~(WM8994_HPOUT1R_DLY_MASK |WM8994_HPOUT1R_OUTP_MASK |WM8994_HPOUT1R_RMV_SHORT_MASK |
				WM8994_HPOUT1L_DLY_MASK |WM8994_HPOUT1L_OUTP_MASK | WM8994_HPOUT1L_RMV_SHORT_MASK);
			wm8994_write(codec, WM8994_ANALOGUE_HP_1, val);

			// ------------------ Spk path setting ------------------		
			// Disable SPKLVOL
			val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_3);
			val &= ~(WM8994_SPKLVOL_ENA_MASK);
			wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, val);

			// Disable SPKOUT mixer
			val = wm8994_read(codec,WM8994_SPKOUT_MIXERS);
			val &= ~(WM8994_SPKMIXL_TO_SPKOUTL_MASK | 
				WM8994_SPKMIXR_TO_SPKOUTL_MASK | WM8994_SPKMIXR_TO_SPKOUTR_MASK);
			wm8994_write(codec, WM8994_SPKOUT_MIXERS, val);
			
			//Mute Speaker mixer
			val = wm8994_read(codec,WM8994_SPEAKER_MIXER);
			val &= ~(WM8994_DAC1L_TO_SPKMIXL_MASK);
			wm8994_write(codec, WM8994_SPEAKER_MIXER ,val);
			}
			break;			

		default:
			DEBUG_LOG_ERR("Path[%d] is not invaild!\n", path);
			return;
			break;
	}
}

void wm8994_disable_rec_path(struct snd_soc_codec *codec, enum mic_path rec_path) 
{
	struct wm8994_priv *wm8994 = codec->private_data;

	u16 val;

	wm8994->rec_path = MIC_OFF;
	if(!(wm8994->codec_state & CALL_ACTIVE))	// Normal case
	audio_ctrl_mic_bias_gpio(0);	// Disable MIC bias
	
        switch(rec_path)
        {
                case MAIN:
			DEBUG_LOG("Disabling MAIN Mic Path..\n");
			
			if(!(wm8994->codec_state & CALL_ACTIVE))	// Normal case
			{
			val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_2  );
			val &= ~( WM8994_IN1L_ENA_MASK | WM8994_MIXINL_ENA_MASK |WM8994_IN2R_ENA_MASK | WM8994_MIXINR_ENA_MASK );
			wm8994_write(codec,WM8994_POWER_MANAGEMENT_2,val);
			
				if(wm8994->testmode_config_flag == SEC_NORMAL)
			{	
				// Mute IN1L PGA, update volume
				val = wm8994_read(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME);	
				val &= ~(WM8994_IN1L_MUTE_MASK | WM8994_IN1L_VOL_MASK);
				val |= (WM8994_IN1L_VU |WM8994_IN1L_MUTE); //volume
				wm8994_write(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME, val);
			
				//Mute the PGA
				val = wm8994_read(codec, WM8994_INPUT_MIXER_3);
				val&= ~(WM8994_IN1L_TO_MIXINL_MASK | WM8994_IN1L_MIXINL_VOL_MASK | WM8994_MIXOUTL_MIXINL_VOL_MASK);
				wm8994_write(codec, WM8994_INPUT_MIXER_3, val); 
				
				val = wm8994_read(codec, WM8994_RIGHT_LINE_INPUT_3_4_VOLUME);	
				val &= ~(WM8994_IN2R_MUTE_MASK | WM8994_IN2R_VOL_MASK);
				val |= (WM8994_IN2R_VU |WM8994_IN2R_MUTE); //volume
				wm8994_write(codec, WM8994_RIGHT_LINE_INPUT_3_4_VOLUME, val);
	
				//Unmute the PGA
				val = wm8994_read(codec, WM8994_INPUT_MIXER_4);
				val&= ~(WM8994_IN2R_TO_MIXINR_MASK | WM8994_IN2R_MIXINR_VOL_MASK | WM8994_MIXOUTR_MIXINR_VOL_MASK);
				wm8994_write(codec, WM8994_INPUT_MIXER_4, val); 	
				
			}
			
			//Disconnect IN1LN ans IN1LP to the inputs
			val = wm8994_read(codec,WM8994_INPUT_MIXER_2 ); 
			val &= (WM8994_IN1LN_TO_IN1L_MASK | WM8994_IN1LP_TO_IN1L_MASK | WM8994_IN1LN_TO_IN1L_MASK | WM8994_IN1LP_TO_IN1L_MASK | WM8994_IN2RN_TO_IN2R_MASK| WM8994_IN2RP_TO_IN2R_MASK );
			wm8994_write(codec, WM8994_INPUT_MIXER_2, val);
			
			//Digital Paths
			
			val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_4 );
			val &= ~(WM8994_ADCL_ENA_MASK |WM8994_AIF1ADC1L_ENA_MASK  | WM8994_ADCR_ENA_MASK |WM8994_AIF1ADC1R_ENA_MASK);
			wm8994_write(codec,WM8994_POWER_MANAGEMENT_4 ,val);
			}
			else
			{				
				//Digital Paths 	
				val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_4);
				val &= ~(WM8994_AIF1ADC1L_ENA_MASK);
				wm8994_write(codec,WM8994_POWER_MANAGEMENT_4, val);
			}

			//Disable timeslots
			val = wm8994_read(codec,WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING );
			val &= ~(WM8994_ADC1L_TO_AIF1ADC1L);
			wm8994_write(codec,WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING ,val);			

			val = wm8994_read(codec,WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING);
			val &= ~( WM8994_ADC1R_TO_AIF1ADC1R_MASK);
			wm8994_write(codec,WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING,val);

			val = wm8994_read(codec,WM8994_AIF1_ADC2_LEFT_MIXER_ROUTING );
			val &= ~(WM8994_ADC2L_TO_AIF1ADC2L);
			wm8994_write(codec,WM8994_AIF1_ADC2_LEFT_MIXER_ROUTING ,val);			

			val = wm8994_read(codec,WM8994_AIF1_ADC2_RIGHT_MIXER_ROUTING);
			val &= ~( WM8994_ADC2R_TO_AIF1ADC2R_MASK);
			wm8994_write(codec,WM8994_AIF1_ADC2_RIGHT_MIXER_ROUTING,val);
			
			break;

                case SUB:
			DEBUG_LOG("Disbaling SUB Mic path..\n");

			if(!(wm8994->codec_state & CALL_ACTIVE))	// Normal case
			{
			val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_2 );
			val &= ~(WM8994_IN1R_ENA_MASK |WM8994_MIXINR_ENA_MASK  );
			wm8994_write(codec,WM8994_POWER_MANAGEMENT_2,val);
			
				if(wm8994->testmode_config_flag == SEC_NORMAL)
			{	
				// Disable volume,unmute Right Line	
				val = wm8994_read(codec,WM8994_RIGHT_LINE_INPUT_1_2_VOLUME);	
				val &= ~(WM8994_IN1R_MUTE_MASK | WM8994_IN1R_VOL_MASK);	// Unmute IN1R
				val |= (WM8994_IN1R_VU | WM8994_IN1R_MUTE);
				wm8994_write(codec,WM8994_RIGHT_LINE_INPUT_1_2_VOLUME,val);
				
				// Mute right pga, set volume 
				val = wm8994_read(codec,WM8994_INPUT_MIXER_4 );
				val&= ~(WM8994_IN1R_TO_MIXINR_MASK | WM8994_IN1R_MIXINR_VOL_MASK | WM8994_MIXOUTR_MIXINR_VOL_MASK);
				wm8994_write(codec,WM8994_INPUT_MIXER_4 ,val);
			}
			
			//Disconnect in1rn to inr1 and in1rp to inrp
			val = wm8994_read(codec,WM8994_INPUT_MIXER_2);
			val &= ~( WM8994_IN1RN_TO_IN1R_MASK | WM8994_IN1RP_TO_IN1R_MASK);
			wm8994_write(codec,WM8994_INPUT_MIXER_2,val);
			
			//Digital Paths 
			//Disable right ADC and time slot
			val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_4);
			val &= ~(WM8994_ADCR_ENA_MASK |WM8994_AIF1ADC1R_ENA_MASK );
			wm8994_write(codec,WM8994_POWER_MANAGEMENT_4 ,val);
			}
			else
			{				
				//Digital Paths 
				//Disable right ADC and time slot
				val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_4);
				val &= ~(WM8994_AIF1ADC1R_ENA_MASK);
				wm8994_write(codec,WM8994_POWER_MANAGEMENT_4, val);
	
				val = wm8994_read(codec, WM8994_AIF1_CONTROL_1);	//605H : 0x0010
				val &= ~(WM8994_AIF1ADCL_SRC_MASK);
				wm8994_write(codec, WM8994_AIF1_CONTROL_1, val);
			}
			
			//ADC Right mixer routing
			val = wm8994_read(codec,WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING);
			val &= ~( WM8994_ADC1R_TO_AIF1ADC1R_MASK);
			wm8994_write(codec,WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING,val);
			break;
	
                case BT_REC:
			if(!(wm8994->codec_state & CALL_ACTIVE))	// Normal case
			{
			//R1542(606h) - 0x0000
			val = wm8994_read(codec, WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING);
			val &= ~(WM8994_AIF2DACL_TO_AIF1ADC1L_MASK | WM8994_ADC1L_TO_AIF1ADC1L_MASK);
			wm8994_write(codec, WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING, val);

			//R1543(607h) - 0x0000
			val = wm8994_read(codec, WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING);
			val &= ~(WM8994_AIF2DACR_TO_AIF1ADC1R_MASK | WM8994_ADC1R_TO_AIF1ADC1R_MASK);
			wm8994_write(codec, WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING, val);

			//R1312(520h) - 0x0200
			val = wm8994_read(codec, WM8994_AIF2_DAC_FILTERS_1);
			val &= ~(WM8994_AIF2DAC_MUTE_MASK);
			val |= (WM8994_AIF2DAC_MUTE);
			wm8994_write(codec, WM8994_AIF2_DAC_FILTERS_1, val); 
			}
			break;

		default:
                    if((wm8994->codec_state & CALL_ACTIVE) &&  (wm8994->call_record_ch == CH_OFF))
                    {
                            if(wm8994->call_record_path == CALL_RECORDING_MAIN)
                            {
                                    DEBUG_LOG("Disabling MAIN Mic Path in call");
                                    //Digital Paths 	
                                    val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_4);
                                    val &= ~(WM8994_AIF1ADC1L_ENA_MASK);
                                    wm8994_write(codec,WM8994_POWER_MANAGEMENT_4, val);

                                    //Disable timeslots
                                    val = wm8994_read(codec,WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING);
                                    val &= ~(WM8994_ADC1L_TO_AIF1ADC1L);
                                    wm8994_write(codec,WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING, val);	
                            }
                            else if(wm8994->call_record_path == CALL_RECORDING_SUB)
                            {
                                    DEBUG_LOG("Disbaling SUB Mic path in call");
                                    //Digital Paths 
                                    //Disable right ADC and time slot
                                    val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_4);
                                    val &= ~(WM8994_AIF1ADC1R_ENA_MASK);
                                    wm8994_write(codec,WM8994_POWER_MANAGEMENT_4, val);

                                    val = wm8994_read(codec, WM8994_AIF1_CONTROL_1);	//605H : 0x0010
                                    val &= ~(WM8994_AIF1ADCL_SRC_MASK);
                                    wm8994_write(codec, WM8994_AIF1_CONTROL_1, val);

                                    //ADC Right mixer routing
                                    val = wm8994_read(codec,WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING);
                                    val &= ~(WM8994_ADC1R_TO_AIF1ADC1R_MASK);
                                    wm8994_write(codec, WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING, val);
                            }

                            wm8994->call_record_path = CALL_RECORDING_OFF;
                    }
                    else
                    {
                            DEBUG_LOG_ERR("Path[%d] is not invaild!", rec_path);
                    }
                	break;
        }
}

void wm8994_disable_fmradio_path(struct snd_soc_codec *codec, enum fmradio_path path)
{
	struct wm8994_priv *wm8994 = codec->private_data;

	u16 val;

	DEBUG_LOG("Turn off fmradio_path = [%d]", path);

	switch(path)
	{
		case FMR_OFF :
			wm8994->fmradio_path = FMR_OFF;

			//---------------------------------------------------
			// Disable speaker setting for FM radio
			if(wm8994->codec_state & CALL_ACTIVE)
			{

			//disbale the SPKOUTL
			val =wm8994_read(codec,WM8994_POWER_MANAGEMENT_1);
			val &= ~(WM8994_SPKOUTL_ENA_MASK); 
			wm8994_write(codec,WM8994_POWER_MANAGEMENT_1 ,val);

			// Disable SPK Volume.
			val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_3);
			val &= ~(WM8994_SPKRVOL_ENA_MASK | WM8994_SPKLVOL_ENA_MASK);
			wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, val);

			if(wm8994->testmode_config_flag == SEC_NORMAL)
			{	
				// Mute the SPKMIXVOLUME
				val = wm8994_read(codec, WM8994_SPKMIXL_ATTENUATION);
				val &= ~(WM8994_SPKMIXL_VOL_MASK);
				wm8994_write(codec, WM8994_SPKMIXL_ATTENUATION, val);
					
				val = wm8994_read(codec, WM8994_SPKMIXR_ATTENUATION);
				val &= ~(WM8994_SPKMIXR_VOL_MASK);
				wm8994_write(codec, WM8994_SPKMIXR_ATTENUATION, val);
			
				val = wm8994_read(codec, WM8994_SPEAKER_VOLUME_LEFT);
				val &= ~(WM8994_SPKOUTL_MUTE_N_MASK | WM8994_SPKOUTL_VOL_MASK);
				wm8994_write(codec, WM8994_SPEAKER_VOLUME_LEFT, val);
			
				val = wm8994_read(codec, WM8994_SPEAKER_VOLUME_RIGHT);
				val &= ~(WM8994_SPKOUTR_MUTE_N_MASK | WM8994_SPKOUTR_VOL_MASK);
				wm8994_write(codec, WM8994_SPEAKER_VOLUME_RIGHT, val);
			
				val = wm8994_read(codec, WM8994_CLASSD);
				val &= ~(WM8994_SPKOUTL_BOOST_MASK);
				wm8994_write(codec, WM8994_CLASSD, val);
			}
			
			/*Output MIxer-Output PGA*/
			val = wm8994_read(codec,WM8994_SPKOUT_MIXERS );
			val &= ~(WM8994_SPKMIXL_TO_SPKOUTL_MASK | 
				WM8994_SPKMIXR_TO_SPKOUTL_MASK | WM8994_SPKMIXR_TO_SPKOUTR_MASK);
			wm8994_write(codec, WM8994_SPKOUT_MIXERS, val);
							
			// Output mixer setting
			val = wm8994_read(codec, WM8994_SPEAKER_MIXER);
			val &= ~(WM8994_MIXINL_TO_SPKMIXL_MASK | WM8994_MIXINR_TO_SPKMIXR_MASK);
			wm8994_write(codec, WM8994_SPEAKER_MIXER, val);

			//---------------------------------------------------
			// Disable earpath setting for FM radio

			//Disable end point for preventing pop up noise.
			val =wm8994_read(codec,WM8994_POWER_MANAGEMENT_1);
			val &= ~(WM8994_HPOUT1L_ENA_MASK |WM8994_HPOUT1R_ENA_MASK);
			wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

			// Disable MIXOUT
			val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_3);
			val &= ~(WM8994_MIXOUTLVOL_ENA_MASK | WM8994_MIXOUTRVOL_ENA_MASK | WM8994_MIXOUTL_ENA_MASK | WM8994_MIXOUTR_ENA_MASK);
			wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, val);

			if(wm8994->testmode_config_flag == SEC_NORMAL)
			{
				// Output setting
				val = wm8994_read(codec, WM8994_LEFT_OUTPUT_VOLUME);
				val &= ~(WM8994_HPOUT1L_MUTE_N_MASK | WM8994_HPOUT1L_VOL_MASK);
				wm8994_write(codec, WM8994_LEFT_OUTPUT_VOLUME, val);
			
				val = wm8994_read(codec, WM8994_RIGHT_OUTPUT_VOLUME);
				val &= ~(WM8994_HPOUT1R_MUTE_N_MASK | WM8994_HPOUT1R_VOL_MASK);
				wm8994_write(codec, WM8994_RIGHT_OUTPUT_VOLUME, val);
			}

			//Disable Charge Pump	
			val = wm8994_read(codec, WM8994_CHARGE_PUMP_1);
			val &= ~WM8994_CP_ENA_MASK ;
			val |= WM8994_CP_ENA_DEFAULT ; // this is from wolfson		
			wm8994_write(codec, WM8994_CHARGE_PUMP_1, val);
			
			// Intermediate HP settings
			val = wm8994_read(codec, WM8994_ANALOGUE_HP_1); 	
			val &= ~(WM8994_HPOUT1R_DLY_MASK |WM8994_HPOUT1R_OUTP_MASK |WM8994_HPOUT1R_RMV_SHORT_MASK |
				WM8994_HPOUT1L_DLY_MASK |WM8994_HPOUT1L_OUTP_MASK | WM8994_HPOUT1L_RMV_SHORT_MASK);
			wm8994_write(codec, WM8994_ANALOGUE_HP_1, val);
												
			// Disable Output mixer setting
			val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);
			val &= ~(WM8994_DAC1L_TO_HPOUT1L_MASK | WM8994_MIXINL_TO_MIXOUTL_MASK);
			wm8994_write(codec, WM8994_OUTPUT_MIXER_1, val); 
			
			val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);
			val &= ~(WM8994_DAC1R_TO_HPOUT1R_MASK | WM8994_MIXINR_TO_MIXOUTR_MASK);
			wm8994_write(codec, WM8994_OUTPUT_MIXER_2, val); 

			//---------------------------------------------------
			// Disable common setting for FM radio
			
			// Disable IN2 and MIXIN
			val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_2);
			val &= ~(WM8994_TSHUT_ENA_MASK | WM8994_TSHUT_OPDIS_MASK | WM8994_OPCLK_ENA_MASK | 
					WM8994_MIXINL_ENA_MASK | WM8994_MIXINR_ENA_MASK | WM8994_IN2L_ENA_MASK | WM8994_IN2R_ENA_MASK);
			wm8994_write(codec, WM8994_POWER_MANAGEMENT_2, val);
			
			// Disable Input mixer setting
			val = wm8994_read(codec, WM8994_INPUT_MIXER_2);
			val &= ~(WM8994_IN2LN_TO_IN2L_MASK | WM8994_IN2RN_TO_IN2R_MASK);
			wm8994_write(codec, WM8994_INPUT_MIXER_2, val); 	
			
			if(wm8994->testmode_config_flag == SEC_NORMAL)
			{
				// Disable IN2L to MIXINL
				val = wm8994_read(codec, WM8994_INPUT_MIXER_3);
				val &= ~(WM8994_IN2L_TO_MIXINL_MASK);
				wm8994_write(codec, WM8994_INPUT_MIXER_3, val);
				
				//Disable IN2R to MIXINR
				val = wm8994_read(codec, WM8994_INPUT_MIXER_4);
				val &= ~(WM8994_IN2R_TO_MIXINR_MASK);
				wm8994_write(codec, WM8994_INPUT_MIXER_4, val); 	
			}
							
			// Mute IN2L PGA volume
			val = wm8994_read(codec, WM8994_LEFT_LINE_INPUT_3_4_VOLUME);
			val &= ~(WM8994_IN2L_VU_MASK | WM8994_IN2L_MUTE_MASK | WM8994_IN2L_VOL_MASK);
			val |= (WM8994_IN2L_VU | WM8994_IN2L_MUTE);
			wm8994_write(codec, WM8994_LEFT_LINE_INPUT_3_4_VOLUME, val); 
			
			val = wm8994_read(codec, WM8994_RIGHT_LINE_INPUT_3_4_VOLUME);
			val &= ~(WM8994_IN2R_VU_MASK | WM8994_IN2R_MUTE_MASK | WM8994_IN2R_VOL_MASK);
			val |= (WM8994_IN2R_VU |WM8994_IN2R_MUTE);	
			wm8994_write(codec, WM8994_RIGHT_LINE_INPUT_3_4_VOLUME, val); 

			//---------------------------------------------------
			// Disable path setting for mixing
			val = wm8994_read(codec,WM8994_SPEAKER_MIXER);
			val &= ~(WM8994_DAC1L_TO_SPKMIXL_MASK);
			wm8994_write(codec, WM8994_SPEAKER_MIXER, val);			

			// Disable DAC1L to HPOUT1L path
			val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);
			val &= ~(WM8994_DAC1L_TO_HPOUT1L_MASK | WM8994_DAC1L_TO_MIXOUTL_MASK);
			wm8994_write(codec,WM8994_OUTPUT_MIXER_1, val);
			
			// Disable DAC1R to HPOUT1R path
			val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);
			val &= ~(WM8994_DAC1R_TO_HPOUT1R_MASK | WM8994_DAC1R_TO_MIXOUTR_MASK);
			wm8994_write(codec, WM8994_OUTPUT_MIXER_2, val);				
			}
			break;
		
		case FMR_SPK :
			//disbale the SPKOUTL
			val =wm8994_read(codec,WM8994_POWER_MANAGEMENT_1);
			val &= ~(WM8994_SPKOUTL_ENA_MASK); 
//			wm8994_write(codec,WM8994_POWER_MANAGEMENT_1 ,val);

			// Disable SPK Volume.
			val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_3);
			val &= ~(WM8994_SPKRVOL_ENA_MASK | WM8994_SPKLVOL_ENA_MASK);
//			wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, val);

			if(wm8994->testmode_config_flag == SEC_NORMAL)
			{	
				// Mute the SPKMIXVOLUME
				val = wm8994_read(codec, WM8994_SPKMIXL_ATTENUATION);
				val &= ~(WM8994_SPKMIXL_VOL_MASK);
				wm8994_write(codec, WM8994_SPKMIXL_ATTENUATION, val);
					
				val = wm8994_read(codec, WM8994_SPKMIXR_ATTENUATION);
				val &= ~(WM8994_SPKMIXR_VOL_MASK);
				wm8994_write(codec, WM8994_SPKMIXR_ATTENUATION, val);
			
				val = wm8994_read(codec, WM8994_SPEAKER_VOLUME_LEFT);
				val &= ~(WM8994_SPKOUTL_MUTE_N_MASK | WM8994_SPKOUTL_VOL_MASK);
				wm8994_write(codec, WM8994_SPEAKER_VOLUME_LEFT, val);
			
				val = wm8994_read(codec, WM8994_SPEAKER_VOLUME_RIGHT);
				val &= ~(WM8994_SPKOUTR_MUTE_N_MASK | WM8994_SPKOUTR_VOL_MASK);
				wm8994_write(codec, WM8994_SPEAKER_VOLUME_RIGHT, val);
			
				val = wm8994_read(codec, WM8994_CLASSD);
				val &= ~(WM8994_SPKOUTL_BOOST_MASK);
				wm8994_write(codec, WM8994_CLASSD, val);
			}
			
			/*Output MIxer-Output PGA*/
			val = wm8994_read(codec,WM8994_SPKOUT_MIXERS );
			val &= ~(WM8994_SPKMIXL_TO_SPKOUTL_MASK | 
				WM8994_SPKMIXR_TO_SPKOUTL_MASK | WM8994_SPKMIXR_TO_SPKOUTR_MASK);
			wm8994_write(codec, WM8994_SPKOUT_MIXERS, val);
							
			// Output mixer setting
			val = wm8994_read(codec, WM8994_SPEAKER_MIXER);
			val &= ~(WM8994_MIXINL_TO_SPKMIXL_MASK | WM8994_MIXINR_TO_SPKMIXR_MASK);
			wm8994_write(codec, WM8994_SPEAKER_MIXER, val);
			break;

		case FMR_HP :
			//Disable end point for preventing pop up noise.
			val =wm8994_read(codec,WM8994_POWER_MANAGEMENT_1);
			val &= ~(WM8994_HPOUT1L_ENA_MASK |WM8994_HPOUT1R_ENA_MASK);
			wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

			// Disable MIXOUT
			val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_3);
			val &= ~(WM8994_MIXOUTLVOL_ENA_MASK | WM8994_MIXOUTRVOL_ENA_MASK | WM8994_MIXOUTL_ENA_MASK | WM8994_MIXOUTR_ENA_MASK);
			wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, val);

			if(wm8994->testmode_config_flag == SEC_NORMAL)
			{
				// Output setting
				val = wm8994_read(codec, WM8994_LEFT_OUTPUT_VOLUME);
				val &= ~(WM8994_HPOUT1L_MUTE_N_MASK | WM8994_HPOUT1L_VOL_MASK);
				wm8994_write(codec, WM8994_LEFT_OUTPUT_VOLUME, val);
			
				val = wm8994_read(codec, WM8994_RIGHT_OUTPUT_VOLUME);
				val &= ~(WM8994_HPOUT1R_MUTE_N_MASK | WM8994_HPOUT1R_VOL_MASK);
				wm8994_write(codec, WM8994_RIGHT_OUTPUT_VOLUME, val);
			}

			//Disable Charge Pump	
			val = wm8994_read(codec, WM8994_CHARGE_PUMP_1);
			val &= ~WM8994_CP_ENA_MASK ;
			val |= WM8994_CP_ENA_DEFAULT ; // this is from wolfson		
			wm8994_write(codec, WM8994_CHARGE_PUMP_1, val);
			
			// Intermediate HP settings
			val = wm8994_read(codec, WM8994_ANALOGUE_HP_1); 	
			val &= ~(WM8994_HPOUT1R_DLY_MASK |WM8994_HPOUT1R_OUTP_MASK |WM8994_HPOUT1R_RMV_SHORT_MASK |
				WM8994_HPOUT1L_DLY_MASK |WM8994_HPOUT1L_OUTP_MASK | WM8994_HPOUT1L_RMV_SHORT_MASK);
			wm8994_write(codec, WM8994_ANALOGUE_HP_1, val);
												
			// Disable Output mixer setting
			val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);
			val &= ~(WM8994_DAC1L_TO_HPOUT1L_MASK | WM8994_MIXINL_TO_MIXOUTL_MASK);
			wm8994_write(codec, WM8994_OUTPUT_MIXER_1, val); 
			
			val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);
			val &= ~(WM8994_DAC1R_TO_HPOUT1R_MASK | WM8994_MIXINR_TO_MIXOUTR_MASK);
			wm8994_write(codec, WM8994_OUTPUT_MIXER_2, val); 
			break;

		case FMR_SPK_MIX :
			//Mute the DAC path
			val = wm8994_read(codec,WM8994_SPEAKER_MIXER);
			val &= ~(WM8994_DAC1L_TO_SPKMIXL_MASK);
			wm8994_write(codec, WM8994_SPEAKER_MIXER, val);			
			break;

		case FMR_HP_MIX :					
			// Disable DAC1L to HPOUT1L path
			val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);
			val &= ~(WM8994_DAC1L_TO_HPOUT1L_MASK | WM8994_DAC1L_TO_MIXOUTL_MASK);
			wm8994_write(codec,WM8994_OUTPUT_MIXER_1, val);
			
			// Disable DAC1R to HPOUT1R path
			val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);
			val &= ~(WM8994_DAC1R_TO_HPOUT1R_MASK | WM8994_DAC1R_TO_MIXOUTR_MASK);
			wm8994_write(codec, WM8994_OUTPUT_MIXER_2, val);	
			break;

		case FMR_DUAL_MIX :		
			//Mute the DAC path
			val = wm8994_read(codec,WM8994_SPEAKER_MIXER);
			val &= ~(WM8994_DAC1L_TO_SPKMIXL_MASK);
			wm8994_write(codec, WM8994_SPEAKER_MIXER, val);
			
			// Disable DAC1L to HPOUT1L path
			val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);
			val &= ~(WM8994_DAC1L_TO_HPOUT1L_MASK | WM8994_DAC1L_TO_MIXOUTL_MASK);
			wm8994_write(codec,WM8994_OUTPUT_MIXER_1, val);
			
			// Disable DAC1R to HPOUT1R path
			val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);
			val &= ~(WM8994_DAC1R_TO_HPOUT1R_MASK | WM8994_DAC1R_TO_MIXOUTR_MASK);
			wm8994_write(codec, WM8994_OUTPUT_MIXER_2, val);	
			break;

		default:
			DEBUG_LOG_ERR("fmradio path[%d] is not invaild!\n", path);
			return;
			break;
		}
}

void wm8994_record_headset_mic(struct snd_soc_codec *codec) 
{
	struct wm8994_priv *wm8994 = codec->private_data;
	
	u16 val;

	audio_ctrl_mic_bias_gpio(1);
	
	DEBUG_LOG("Recording through Headset Mic\n");

	if(!(wm8994->codec_state & CALL_ACTIVE))	// Normal case
	{
	// Disable FM radio path
	val = wm8994_read(codec, WM8994_SPEAKER_MIXER);
	val &= ~WM8994_MIXINL_TO_SPKMIXL_MASK;
	wm8994_write(codec, WM8994_SPEAKER_MIXER, val);

	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);
	val &= ~WM8994_MIXINL_TO_MIXOUTL_MASK;
	wm8994_write(codec, WM8994_OUTPUT_MIXER_1, val); 
	
	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);
	val &= ~WM8994_MIXINR_TO_MIXOUTR_MASK;
	wm8994_write(codec, WM8994_OUTPUT_MIXER_2, val);
	
	val = wm8994_read(codec, WM8994_DAC2_LEFT_MIXER_ROUTING);	//604H : 0x0010
	val &= ~(WM8994_ADC1_TO_DAC2L_MASK);
	wm8994_write(codec, WM8994_DAC2_LEFT_MIXER_ROUTING, val);

	val = wm8994_read(codec, WM8994_DAC2_RIGHT_MIXER_ROUTING);	//605H : 0x0010
	val &= ~(WM8994_ADC1_TO_DAC2R_MASK);
	wm8994_write(codec, WM8994_DAC2_RIGHT_MIXER_ROUTING, val);

	// Mixing left channel output to right channel.
	val = wm8994_read(codec, WM8994_AIF1_CONTROL_1);	//605H : 0x0010
	val &= ~(WM8994_AIF1ADCL_SRC_MASK | WM8994_AIF1ADCR_SRC_MASK);
	val |= (WM8994_AIF1ADCL_SRC | WM8994_AIF1ADCR_SRC);
	wm8994_write(codec, WM8994_AIF1_CONTROL_1, val);

	wm8994_write(codec,WM8994_ANTIPOP_2,0x68);	//Ear  mic volume issue fix

	//Enable mic bias, vmid, bias generator
	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_1 );
	val &= ~(WM8994_BIAS_ENA_MASK | WM8994_VMID_SEL_MASK);
	val |= (WM8994_BIAS_ENA | WM8994_VMID_SEL_NORMAL);
	wm8994_write(codec,WM8994_POWER_MANAGEMENT_1,val);

	//Enable Right Input Mixer,Enable IN1R PGA - Temporary inserted for blocking MIC and FM radio mixing - DW Shim 2010.03.04
//	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_2 );
//	val &= ~(WM8994_IN1R_ENA_MASK |WM8994_MIXINR_ENA_MASK  );
//	val |= (WM8994_MIXINR_ENA | WM8994_IN1R_ENA );
	val = (WM8994_MIXINR_ENA | WM8994_IN1R_ENA );
	wm8994_write(codec,WM8994_POWER_MANAGEMENT_2,val);

		if(wm8994->testmode_config_flag == SEC_NORMAL)
	{	
		// Enable volume,unmute Right Line	
		val = wm8994_read(codec,WM8994_RIGHT_LINE_INPUT_1_2_VOLUME);	
		val &= ~(WM8994_IN1R_MUTE_MASK | WM8994_IN1R_VOL_MASK);	// Unmute IN1R
		if(wm8994->recognition_active == REC_ON)
			val |= (WM8994_IN1R_VU | TUNING_RECOGNITION_SUB_INPUTMIX_VOL);
		else
		val |= (WM8994_IN1R_VU | TUNING_RECORD_SUB_INPUTMIX_VOL);
		wm8994_write(codec,WM8994_RIGHT_LINE_INPUT_1_2_VOLUME,val);
		
		// unmute right pga, set volume 
		val = wm8994_read(codec,WM8994_INPUT_MIXER_4 );
		val&= ~(WM8994_IN1R_TO_MIXINR_MASK | WM8994_IN1R_MIXINR_VOL_MASK | WM8994_MIXOUTR_MIXINR_VOL_MASK);
		if(wm8994->recognition_active == REC_ON)
			val |= (WM8994_IN1R_TO_MIXINR | WM8994_IN1R_MIXINR_VOL);//30db
		else
		val |= (WM8994_IN1R_TO_MIXINR | WM8994_IN1R_MIXINR_VOL);//30db
		wm8994_write(codec,WM8994_INPUT_MIXER_4 ,val);
	}

	val = wm8994_read(codec, WM8994_AIF1_ADC1_LEFT_VOLUME);
	val &= ~(WM8994_AIF1ADC1L_VOL_MASK);
	if(wm8994->recognition_active == REC_ON)		
		val |= (WM8994_AIF1ADC1_VU | TUNING_RECOGNITION_SUB_AIF1ADCL_VOL); 
	else
		val |= (WM8994_AIF1ADC1_VU | TUNING_RECORD_SUB_AIF1ADCL_VOL); // 0db
	wm8994_write(codec, WM8994_AIF1_ADC1_LEFT_VOLUME, val);

	val = wm8994_read(codec, WM8994_AIF1_ADC1_RIGHT_VOLUME);
	val &= ~(WM8994_AIF1ADC1R_VOL_MASK);
	if(wm8994->recognition_active == REC_ON)		
		val |= (WM8994_AIF1ADC1_VU | TUNING_RECOGNITION_SUB_AIF1ADCR_VOL);
	else
		val |= (WM8994_AIF1ADC1_VU | TUNING_RECORD_SUB_AIF1ADCR_VOL); // 0db
	wm8994_write(codec, WM8994_AIF1_ADC1_RIGHT_VOLUME, val);
	
	//connect in1rn to inr1 and in1rp to inrp - Temporary inserted for blocking MIC and FM radio mixing - DW Shim 2010.03.04
//	val = wm8994_read(codec,WM8994_INPUT_MIXER_2);
//	val &= ~( WM8994_IN1RN_TO_IN1R_MASK | WM8994_IN1RP_TO_IN1R_MASK);
//	val |= (WM8994_IN1RN_TO_IN1R | WM8994_IN1RP_TO_IN1R)  ;	
	val = (WM8994_IN1RN_TO_IN1R | WM8994_IN1RP_TO_IN1R)  ; 
	wm8994_write(codec,WM8994_INPUT_MIXER_2,val);
	}
	else
	{
		// Mixing left channel output to right channel.
		val = wm8994_read(codec, WM8994_AIF1_CONTROL_1);	//605H : 0x0010
		val &= ~(WM8994_AIF1ADCL_SRC_MASK | WM8994_AIF1ADCR_SRC_MASK);
		val |= (WM8994_AIF1ADCL_SRC | WM8994_AIF1ADCR_SRC);
		wm8994_write(codec, WM8994_AIF1_CONTROL_1, val);
	}
	
	//Digital Paths	
	//Enable right ADC and time slot
	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_4);
	val &= ~(WM8994_ADCR_ENA_MASK |WM8994_AIF1ADC1R_ENA_MASK );
	val |= (WM8994_AIF1ADC1R_ENA | WM8994_ADCR_ENA  );
	wm8994_write(codec,WM8994_POWER_MANAGEMENT_4 ,val);

	wm8994_write(codec,WM8994_AIF1_ADC1_FILTERS ,0x0000);

	//ADC Right mixer routing
	val = wm8994_read(codec,WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING);
	val &= ~( WM8994_ADC1R_TO_AIF1ADC1R_MASK);
	val |= WM8994_ADC1R_TO_AIF1ADC1R;
	wm8994_write(codec,WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING,val);

	val = wm8994_read(codec, WM8994_AIF1_MASTER_SLAVE);
	val |= (WM8994_AIF1_MSTR | WM8994_AIF1_CLK_FRC | WM8994_AIF1_LRCLK_FRC);	// Master mode
	wm8994_write(codec, WM8994_AIF1_MASTER_SLAVE, val);

	wm8994_write( codec, WM8994_GPIO_1, 0xA101 );   // GPIO1 is Input Enable
}

void wm8994_record_main_mic(struct snd_soc_codec *codec) 
{
	struct wm8994_priv *wm8994 = codec->private_data;

	u16 val;

	DEBUG_LOG("Recording through Main Mic\n");
	audio_ctrl_mic_bias_gpio(1);

	if(!(wm8994->codec_state & CALL_ACTIVE))	// Normal case
	{
	// Disable FM radio path
	val = wm8994_read(codec, WM8994_SPEAKER_MIXER);
	val &= ~WM8994_MIXINL_TO_SPKMIXL_MASK;
	wm8994_write(codec, WM8994_SPEAKER_MIXER, val);

	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);
	val &= ~WM8994_MIXINL_TO_MIXOUTL_MASK;
	wm8994_write(codec, WM8994_OUTPUT_MIXER_1, val); 
	
	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);
	val &= ~WM8994_MIXINR_TO_MIXOUTR_MASK;
	wm8994_write(codec, WM8994_OUTPUT_MIXER_2, val); 
	
	val = wm8994_read(codec, WM8994_DAC2_LEFT_MIXER_ROUTING);	//604H : 0x0010
	val &= ~(WM8994_ADC1_TO_DAC2L_MASK);
	wm8994_write(codec, WM8994_DAC2_LEFT_MIXER_ROUTING, val);

	val = wm8994_read(codec, WM8994_DAC2_RIGHT_MIXER_ROUTING);	//605H : 0x0010
	val &= ~(WM8994_ADC1_TO_DAC2R_MASK);
	wm8994_write(codec, WM8994_DAC2_RIGHT_MIXER_ROUTING, val);

	// Mixing left channel output to right channel.
	val = wm8994_read(codec, WM8994_AIF1_CONTROL_1);	//605H : 0x0010
	val &= ~(WM8994_AIF1ADCL_SRC_MASK | WM8994_AIF1ADCR_SRC_MASK);
	if((wm8994->QIK_state == QIK_ON)||(wm8994->recognition_active != REC_ON))
		val |= (WM8994_AIF1ADCL_SRC | WM8994_AIF1ADCR_SRC);
	wm8994_write(codec, WM8994_AIF1_CONTROL_1, val);

	wm8994_write(codec,WM8994_ANTIPOP_2,0x68);	//Main mic volume issue fix: requested H/W

	//Enable micbias,vmid,mic1
	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_1 );
	val &= ~(WM8994_BIAS_ENA_MASK | WM8994_VMID_SEL_MASK);
	val |= (WM8994_BIAS_ENA | WM8994_VMID_SEL_NORMAL);  
	wm8994_write(codec,WM8994_POWER_MANAGEMENT_1,val);

	//Enable left input mixer and IN1L PGA	- Temporary inserted for blocking MIC and FM radio mixing - DW Shim 2010.03.04
//	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_2  );
//	val &= ~( WM8994_IN1L_ENA_MASK | WM8994_MIXINL_ENA_MASK );
//	val |= (WM8994_MIXINL_ENA |WM8994_IN1L_ENA );
	if((wm8994->QIK_state == QIK_ON)||(wm8994->recognition_active != REC_ON))
	{
		val = (WM8994_MIXINR_ENA |WM8994_IN2R_ENA );
		wm8994_write(codec,WM8994_POWER_MANAGEMENT_2,val);	
	}	
	else
	{		
		val = (WM8994_MIXINL_ENA |WM8994_IN1L_ENA );
		wm8994_write(codec,WM8994_POWER_MANAGEMENT_2,val);
	}

		//Enable HPF Filter for google voice search
		if(wm8994->recognition_active == REC_ON){
			val = wm8994_read(codec,WM8994_AIF1_ADC1_FILTERS );
			val &= ~(WM8994_AIF1ADC1L_HPF_MASK | WM8994_AIF1ADC1R_HPF_MASK);
			val |= (WM8994_AIF1ADC1L_HPF | WM8994_AIF1ADC1R_HPF);  	
			wm8994_write(codec,WM8994_AIF1_ADC1_FILTERS ,val);
		}
		
		if(wm8994->testmode_config_flag == SEC_NORMAL)
	{	
		// Unmute IN1L PGA, update volume
		if((wm8994->QIK_state == QIK_ON)||(wm8994->recognition_active != REC_ON))
		{
			val = wm8994_read(codec, WM8994_RIGHT_LINE_INPUT_3_4_VOLUME);	
			val &= ~(WM8994_IN2R_MUTE_MASK | WM8994_IN2R_VOL_MASK);
			if(wm8994->QIK_state == QIK_ON)
				val |= (WM8994_IN2R_VU |TUNING_RECORD_SUB_INPUTLINE_VOL); //volume
			else
				val |= (WM8994_IN2R_VU |TUNING_RECORD_MAIN_INPUTLINE_VOL); //volume
			wm8994_write(codec, WM8994_RIGHT_LINE_INPUT_3_4_VOLUME, val);
	
			//Unmute the PGA
			val = wm8994_read(codec, WM8994_INPUT_MIXER_4);
			val&= ~(WM8994_IN2R_TO_MIXINR_MASK | WM8994_IN2R_MIXINR_VOL_MASK | WM8994_MIXOUTR_MIXINR_VOL_MASK);
			if(wm8994->QIK_state == QIK_ON)
				val |= (WM8994_IN2R_TO_MIXINR | WM8994_IN2R_MIXINR_VOL);	//30db
			else
				val |= (WM8994_IN2R_TO_MIXINR | WM8994_IN2R_MIXINR_VOL);	//30db	
			wm8994_write(codec, WM8994_INPUT_MIXER_4, val); 			
		}
		else
		{		
		val = wm8994_read(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME);	
		val &= ~(WM8994_IN1L_MUTE_MASK | WM8994_IN1L_VOL_MASK);
		if(wm8994->recognition_active == REC_ON)
			val |= (WM8994_IN1L_VU |TUNING_RECOGNITION_MAIN_INPUTLINE_VOL); //volume
		else
		val |= (WM8994_IN1L_VU |TUNING_RECORD_MAIN_INPUTLINE_VOL); //volume
		wm8994_write(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME, val);
	
		//Unmute the PGA
		val = wm8994_read(codec, WM8994_INPUT_MIXER_3);
		val&= ~(WM8994_IN1L_TO_MIXINL_MASK | WM8994_IN1L_MIXINL_VOL_MASK | WM8994_MIXOUTL_MIXINL_VOL_MASK);
		if(wm8994->recognition_active == REC_ON)		
			val |= (WM8994_IN1L_TO_MIXINL);	//30db
		else
		val |= (WM8994_IN1L_MIXINL_VOL | WM8994_IN1L_TO_MIXINL);	//30db
		wm8994_write(codec, WM8994_INPUT_MIXER_3, val); 
	}
	}
	
	//Connect IN1LN ans IN1LP to the inputs - Temporary inserted for blocking MIC and FM radio mixing - DW Shim 2010.03.04
//	val = wm8994_read(codec,WM8994_INPUT_MIXER_2 );	
//	val &= (WM8994_IN1LN_TO_IN1L_MASK | WM8994_IN1LP_TO_IN1L_MASK );
//	val |= ( WM8994_IN1LP_TO_IN1L | WM8994_IN1LN_TO_IN1L );
	if((wm8994->QIK_state == QIK_ON)||(wm8994->recognition_active != REC_ON))
	{
		val = ( WM8994_IN2RN_TO_IN2R | WM8994_IN2RP_TO_IN2R);
		wm8994_write(codec, WM8994_INPUT_MIXER_2, val);		
	}	
	else
	{		
	val = ( WM8994_IN1LP_TO_IN1L | WM8994_IN1LN_TO_IN1L);
	wm8994_write(codec, WM8994_INPUT_MIXER_2, val);
	}

	{
		//Digital Paths
		val = wm8994_read(codec, WM8994_AIF1_ADC1_LEFT_VOLUME);
		val &= ~(WM8994_AIF1ADC1L_VOL_MASK);
		if(wm8994->QIK_state == QIK_ON)
			val |= (WM8994_AIF1ADC1_VU | TUNING_RECORD_SPKSUB_AIF1ADCL_VOL);
		else if(wm8994->recognition_active == REC_ON)		
			val |= (WM8994_AIF1ADC1_VU | TUNING_RECOGNITION_MAIN_AIF1ADCL_VOL);
		else
			val |= (WM8994_AIF1ADC1_VU | TUNING_RECORD_MAIN_AIF1ADCL_VOL); // 0db
		wm8994_write(codec, WM8994_AIF1_ADC1_LEFT_VOLUME, val);

		val = wm8994_read(codec, WM8994_AIF1_ADC1_RIGHT_VOLUME);
		val &= ~(WM8994_AIF1ADC1R_VOL_MASK);
		if(wm8994->QIK_state == QIK_ON)
			val |= (WM8994_AIF1ADC1_VU | TUNING_RECORD_SPKSUB_AIF1ADCR_VOL);
		else if(wm8994->recognition_active == REC_ON)		
			val |= (WM8994_AIF1ADC1_VU | TUNING_RECOGNITION_MAIN_AIF1ADCR_VOL);
		else
			val |= (WM8994_AIF1ADC1_VU | TUNING_RECORD_MAIN_AIF1ADCR_VOL); // 0db
		wm8994_write(codec, WM8994_AIF1_ADC1_RIGHT_VOLUME, val);
	}
	}
	
	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_4 );
	val &= ~(WM8994_ADCL_ENA_MASK |WM8994_AIF1ADC1L_ENA_MASK | WM8994_ADCR_ENA_MASK |WM8994_AIF1ADC1R_ENA_MASK );
	if((wm8994->QIK_state == QIK_ON)||(wm8994->recognition_active != REC_ON))
	{
		val |= (WM8994_AIF1ADC1R_ENA | WM8994_ADCR_ENA  );
	}
	else
	{
		val |= ( WM8994_AIF1ADC1L_ENA | WM8994_ADCL_ENA);
	}
	wm8994_write(codec,WM8994_POWER_MANAGEMENT_4 ,val);
	//Enable timeslots

		//ADC Right mixer routing
	val = wm8994_read(codec,WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING);
	val &= ~( WM8994_ADC1R_TO_AIF1ADC1R_MASK);
	val |= WM8994_ADC1R_TO_AIF1ADC1R;
	wm8994_write(codec,WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING,val);	

	val = wm8994_read(codec,WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING );
	val |=WM8994_ADC1L_TO_AIF1ADC1L ;  
	wm8994_write(codec,WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING ,val);
	
	if(wm8994->recognition_active == REC_ON)	
		wm8994_write(codec,WM8994_AIF1_ADC1_FILTERS ,0x3000);
	else	
		wm8994_write(codec,WM8994_AIF1_ADC1_FILTERS ,0x2800);
	
	val = wm8994_read(codec, WM8994_AIF1_MASTER_SLAVE);
	val |= (WM8994_AIF1_MSTR | WM8994_AIF1_CLK_FRC | WM8994_AIF1_LRCLK_FRC);	// Master mode
	wm8994_write(codec, WM8994_AIF1_MASTER_SLAVE, val);

	wm8994_write( codec, WM8994_GPIO_1, 0xA101 );   // GPIO1 is Input Enable

#ifdef CONFIG_SND_VOODOO_RECORD_PRESETS
	voodoo_hook_record_main_mic();
#endif
	
	if(wm8994->recognition_active == REC_ON)	// for avoiding pop noise when start google voice search
		msleep(60);
	
}

void wm8994_record_bluetooth(struct snd_soc_codec *codec)
{
	u16 val;

	struct wm8994_priv *wm8994 = codec->private_data;

	DEBUG_LOG("BT Record Path for Voice Command\n");

//wm8994_set_voicecall_common_setting(codec);
	/*GPIO Configuration*/
	wm8994_write(codec, WM8994_GPIO_1, 0xA101);
	wm8994_write(codec, WM8994_GPIO_2, 0x8100);
	wm8994_write(codec, WM8994_GPIO_3, 0x0100);
	wm8994_write(codec, WM8994_GPIO_4, 0x0100);
	wm8994_write(codec, WM8994_GPIO_5, 0x8100);
	wm8994_write(codec, WM8994_GPIO_6, 0xA101);
	wm8994_write(codec, WM8994_GPIO_7, 0x0100);
	wm8994_write(codec, WM8994_GPIO_8, 0xA101);
	wm8994_write(codec, WM8994_GPIO_9, 0xA101);
	wm8994_write(codec, WM8994_GPIO_10, 0xA101);
	wm8994_write(codec, WM8994_GPIO_11, 0xA101);

	/*FLL2 Setting*/
	wm8994_write(codec, WM8994_FLL2_CONTROL_2, 0x2F00);
	wm8994_write(codec, WM8994_FLL2_CONTROL_3, 0x3126);
	wm8994_write(codec, WM8994_FLL2_CONTROL_4, 0x0100);
	wm8994_write(codec, WM8994_FLL2_CONTROL_5, 0x0C88);
//	wm8994_write(codec, WM8994_FLL2_CONTROL_5, 0x0C89); // CP 24MHz
	wm8994_write(codec, WM8994_FLL2_CONTROL_1, WM8994_FLL2_FRACN_ENA | WM8994_FLL2_ENA);

	val = wm8994_read(codec, WM8994_AIF2_CLOCKING_1);
	if(!(val & WM8994_AIF2CLK_ENA))
		wm8994_write(codec, WM8994_AIF2_CLOCKING_1, 0x0018);

	wm8994_write(codec, WM8994_AIF2_RATE, 0x3 << WM8994_AIF2CLK_RATE_SHIFT);
	
	if(wm8994->hw_version == 3) // H/W Rev D
	{	
	// AIF2 Interface - PCM Stereo mode
		wm8994_write(codec, WM8994_AIF2_CONTROL_1,	//Left Justified, BCLK invert, LRCLK Invert
		WM8994_AIF2ADCR_SRC | WM8994_AIF2_BCLK_INV |0x18);
		wm8994_write(codec, WM8994_AIF2_BCLK, 0x70);
	}
	else // H/W Rev B
	{
		wm8994_write(codec, WM8994_AIF2_CONTROL_1,	//Left Justified, BCLK invert, LRCLK Invert
		WM8994_AIF2ADCR_SRC | WM8994_AIF2_BCLK_INV |WM8994_AIF2_LRCLK_INV | 0x1 << WM8994_AIF2_FMT_SHIFT);
	}		

	wm8994_write(codec, WM8994_AIF2_CONTROL_2, 0x0000);
	wm8994_write(codec, WM8994_AIF2_MASTER_SLAVE, WM8994_AIF2_MSTR | WM8994_AIF2_CLK_FRC | WM8994_AIF2_LRCLK_FRC);	//Master

	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_5);	//05 : 3303
	val &= ~(WM8994_AIF2DACL_ENA_MASK | WM8994_AIF2DACR_ENA_MASK | WM8994_AIF1DAC1L_ENA_MASK | WM8994_AIF1DAC1R_ENA_MASK
		 | WM8994_DAC1L_ENA_MASK | WM8994_DAC1R_ENA_MASK);
	val |= (WM8994_AIF2DACL_ENA | WM8994_AIF2DACR_ENA | WM8994_AIF1DAC1L_ENA | WM8994_AIF1DAC1R_ENA
		 | WM8994_DAC1L_ENA | WM8994_DAC1R_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_5, val);

	/*Clocking*/	
	val = wm8994_read(codec, WM8994_CLOCKING_1);
	val |= (WM8994_DSP_FS2CLK_ENA);
	wm8994_write(codec, WM8994_CLOCKING_1, val);

	wm8994_write(codec, WM8994_POWER_MANAGEMENT_6, 0x0);

	// AIF1 & AIF2 Output is connected to DAC1	
	val = wm8994_read(codec,WM8994_DAC1_LEFT_MIXER_ROUTING);	
	val &= ~(WM8994_AIF1DAC1L_TO_DAC1L_MASK | WM8994_AIF2DACL_TO_DAC1L_MASK);	
	val |= (WM8994_AIF1DAC1L_TO_DAC1L);
	wm8994_write(codec,WM8994_DAC1_LEFT_MIXER_ROUTING, val );  

	val = wm8994_read(codec,WM8994_DAC1_RIGHT_MIXER_ROUTING);	
	val &= ~(WM8994_AIF1DAC1R_TO_DAC1R_MASK | WM8994_AIF2DACR_TO_DAC1R_MASK);	
	val |= (WM8994_AIF1DAC1R_TO_DAC1R);
	wm8994_write(codec,WM8994_DAC1_RIGHT_MIXER_ROUTING, val);
	
	val = wm8994_read(codec, WM8994_AIF1_DAC1_LEFT_VOLUME);
	val &= ~(WM8994_AIF1DAC1L_VOL_MASK);
	val |= (WM8994_AIF1DAC1_VU | TUNING_DAC1L_VOL);
	wm8994_write(codec, WM8994_AIF1_DAC1_LEFT_VOLUME, val);
	
	val = wm8994_read(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME);
	val &= ~(WM8994_AIF1DAC1R_VOL_MASK);
	val |= (WM8994_AIF1DAC1_VU |TUNING_DAC1R_VOL);
	wm8994_write(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME, val);

	wm8994_write(codec, 0x6, 0x0);
//[mook
	//wm8994_write(codec	,	0x01	,	0x0003	)	; // Power Management (1)(01H): 0003  SPKOUTR_ENA=0, SPKOUTL_ENA=0, HPOUT2_ENA=0, HPOUT1L_ENA=0, HPOUT1R_ENA=0, MICB2_ENA=0, MICB1_ENA=0, VMID_SEL=01, BIAS_ENA=1														
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);	//05 : 3303
	val &= ~(0x0007);	
	val |= (0x0003);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);
//]mook

	wm8994_write(codec	,	0x310	,	0x4118	)	; // AIF2 Control (1)(310H):  4010	AIF2ADCL_SRC=0, AIF2ADCR_SRC=1, AIF2ADC_TDM=0, AIF2ADC_TDM_CHAN=0, AIF2_BCLK_INV=1, AIF2_LRCLK_INV=0, AIF2_WL=00, AIF2_FMT=11
		wm8994_write(codec, WM8994_AIF2_CLOCKING_1, 0x0019);	
	wm8994_write(codec	,	0x312	,	0x4000	)	; // AIF2 Master/Slave(312H): 4000	AIF2_TRI=0, AIF2_MSTR=1, AIF2_CLK_FRC=0, AIF2_LRCLK_FRC=0																															
	wm8994_write(codec	,	0x420	,	0x0000	)	; // AIF1 DAC1 Filters (1)(420H): 0000	AIF1DAC1_MUTE=0, AIF1DAC1_MONO=0, AIF1DAC1_MUTERATE=0, AIF1DAC1_UNMUTE_RAMP=0, AIF1DAC1_DEEMP=00																				
	wm8994_write(codec	,	0x422	,	0x0000	)	; // AIF1 DAC2 Filters (1)(422H): 0000	AIF1DAC2_MUTE=0, AIF1DAC2_MONO=0, AIF1DAC2_MUTERATE=0, AIF1DAC2_UNMUTE_RAMP=0, AIF1DAC2_DEEMP=00																				
	wm8994_write(codec	,	0x423	,	0x0010	)	; // AIF1 DAC2 Filters (2)(423H): 0010	AIF1DAC2_3D_GAIN=0_0000, AIF1DAC2_3D_ENA=0																																		
	wm8994_write(codec	,	0x520	,	0x0000	)	; // AIF2 DAC Filters (1)(520H): 0000  AIF2DAC_MUTE=0, AIF2DAC_MONO=0, AIF2DAC_MUTERATE=0, AIF2DAC_UNMUTE_RAMP=0, AIF2DAC_DEEMP=00																						

//[mook do commant
//	wm8994_write(codec	,	0x601	,	0x0004	)	; // DAC1 Left Mixer Routing(601H): 0004  ADCR_TO_DAC1L=0, ADCL_TO_DAC1L=0, AIF2DACL_TO_DAC1L=1, AIF1DAC2L_TO_DAC1L=0, AIF1DAC1L_TO_DAC1L=0 																			
//	wm8994_write(codec	,	0x602	,	0x0004	)	; // DAC1 Right Mixer Routing(602H): 0004  ADCR_TO_DAC1R=0, ADCL_TO_DAC1R=0, AIF2DACR_TO_DAC1R=1, AIF1DAC2R_TO_DAC1R=0, AIF1DAC1R_TO_DAC1R=0																			

//	wm8994_write(codec	,	0x604	,	0x0001	)	; // DAC2 Left Mixer Routing(604H): 0001  ADCR_TO_DAC2L=0, ADCL_TO_DAC2L=0, AIF2DACL_TO_DAC2L=0, AIF1DAC2L_TO_DAC2L=0, AIF1DAC1L_TO_DAC2L=1 																			
//	wm8994_write(codec	,	0x605	,	0x0001	)	; // DAC2 Right Mixer Routing(605H): 0001  ADCR_TO_DAC2R=0, ADCL_TO_DAC2R=0, AIF2DACR_TO_DAC2R=0, AIF1DAC2R_TO_DAC2R=0, AIF1DAC1R_TO_DAC2R=1																			
//]mook do commant
	wm8994_write(codec	,	0x606	,	0x0001	)	; // AIF1 ADC1 Left Mixer Routing(606H): 0001  ADC1L_TO_AIF1ADC1L=0, AIF2DACL_TO_AIF1ADC1L=1																															
	wm8994_write(codec	,	0x607	,	0x0001	)	; // AIF1 ADC1 Right Mixer Routing(607H): 0001	ADC1R_TO_AIF1ADC1R=0, AIF2DACR_TO_AIF1ADC1R=1																															

//[mook do commant
//	wm8994_write(codec	,	0x610	,	0x01C0	)	; // DAC1 Left Volume(610H):  01C0	DAC1L_MUTE=0, DAC1_VU=1, DAC1L_VOL=1100_0000																																		
//	wm8994_write(codec	,	0x611	,	0x01C0	)	; // DAC1 Right Volume(611H): 01C0	DAC1R_MUTE=0, DAC1_VU=1, DAC1R_VOL=1100_0000																																		
//	wm8994_write(codec	,	0x612	,	0x01C0	)	; // DAC2 Left Volume(612H):  01C0	DAC2L_MUTE=0, DAC2_VU=1, DAC2L_VOL=1100_0000																																		
//	wm8994_write(codec	,	0x613	,	0x01C0	)	; // DAC2 Right Volume(613H): 01C0	DAC2R_MUTE=0, DAC2_VU=1, DAC2R_VOL=1100_0000																																		
//]mook do commant
	wm8994_write(codec	,	0x620	,	0x0000	)	; // Oversampling(620H):	  0000	ADC_OSR128=0, DAC_OSR128=0																																							
	//wm8994_write(codec	,	0x621	,	0x01C0	)	; // Sidetone(621H):		  01C0	ST_HPF_CUT=011, ST_HPF=1, STR_SEL=0, STL_SEL=0																																		
	wm8994_write(codec, WM8994_GPIO_8, WM8994_GP8_DIR | WM8994_GP8_DB);
	wm8994_write(codec, WM8994_GPIO_9, WM8994_GP9_DB);
	wm8994_write(codec, WM8994_GPIO_10, WM8994_GP10_DB);
	wm8994_write(codec, WM8994_GPIO_11, WM8994_GP11_DB);																			

//[mook	
//	wm8994_write(codec	,	0x04	,	0x3300	)	; // Power Management (4)(04H): 3300  AIF2ADCL_ENA=1, AIF2ADCR_ENA=1, AIF1ADC2L_ENA=0, AIF1ADC2R_ENA=0, AIF1ADC1L_ENA=1, AIF1ADC1R_ENA=1, DMIC2L_ENA=0, DMIC2R_ENA=0, DMIC1L_ENA=0, DMIC1R_ENA=0, ADCL_ENA=0, ADCR_ENA=0
	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_4 );
	val &= ~(WM8994_AIF1ADC1L_ENA_MASK|WM8994_AIF1ADC1R_ENA_MASK);
	val |= ( WM8994_AIF1ADC1L_ENA | WM8994_AIF1ADC1R_ENA );
	wm8994_write(codec,WM8994_POWER_MANAGEMENT_4 ,val);

//	wm8994_write(codec	,	0x05	,	0x3300	)	; // Power Management (5)(05H): 3300  AIF2DACL_ENA=1, AIF2DACR_ENA=1, AIF1DAC2L_ENA=0, AIF1DAC2R_ENA=0, AIF1DAC1L_ENA=1, AIF1DAC1R_ENA=1, DAC2L_ENA=0, DAC2R_ENA=0, DAC1L_ENA=0, DAC1R_ENA=0							
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_5);	//05 : 3303
	val &= ~(WM8994_AIF2DACL_ENA_MASK | WM8994_AIF2DACR_ENA_MASK);
	val |= (WM8994_AIF2DACL_ENA | WM8994_AIF2DACR_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_5, val);
//]mook
	
	wm8994_write(codec	,	0x06	,	0x000A	)	; // Power Management (6)(06H): 000A  AIF3_TRI=0, AIF3_ADCDAT_SRC=01, AIF2_ADCDAT_SRC=0, AIF2_DACDAT_SRC=1, AIF1_DACDAT_SRC=0																							
}


void wm8994_set_playback_receiver(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->private_data;

	u16 val;

	DEBUG_LOG("");

	//Disable spk output.
	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_1);
	val &= ~(WM8994_SPKOUTL_ENA_MASK);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);
	
	if(wm8994->testmode_config_flag == SEC_NORMAL)
	{
	        val = wm8994_read(codec, WM8994_OUTPUT_MIXER_5);
		val &= ~(WM8994_DACL_MIXOUTL_VOL_MASK);
	        val |= TUNING_RCV_OUTMIX5_VOL << WM8994_DACL_MIXOUTL_VOL_SHIFT;
	        wm8994_write(codec,WM8994_OUTPUT_MIXER_5, val );
	
		val = wm8994_read(codec, WM8994_OUTPUT_MIXER_6);
		val &= ~(WM8994_DACR_MIXOUTR_VOL_MASK);
		val |= TUNING_RCV_OUTMIX6_VOL << WM8994_DACR_MIXOUTR_VOL_SHIFT;
		wm8994_write(codec,WM8994_OUTPUT_MIXER_6, val );

		val = wm8994_read(codec, WM8994_LEFT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTL_MUTE_N_MASK | WM8994_MIXOUTL_VOL_MASK);
		val |= (WM8994_MIXOUT_VU |WM8994_MIXOUTL_MUTE_N | TUNING_RCV_OPGAL_VOL);
		wm8994_write(codec,WM8994_LEFT_OPGA_VOLUME, val );

		val = wm8994_read(codec, WM8994_RIGHT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTR_MUTE_N_MASK | WM8994_MIXOUTR_VOL_MASK);
		val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTR_MUTE_N | TUNING_RCV_OPGAR_VOL);
		wm8994_write(codec,WM8994_RIGHT_OPGA_VOLUME, val );
	
	        val = wm8994_read(codec, WM8994_HPOUT2_VOLUME);
		val &= ~(WM8994_HPOUT2_MUTE_MASK | WM8994_HPOUT2_VOL_MASK);
	        val |= TUNING_HPOUT2_VOL << WM8994_HPOUT2_VOL_SHIFT;
	        wm8994_write(codec,WM8994_HPOUT2_VOLUME, val );

		//Unmute DAC1 left
		val = wm8994_read(codec,WM8994_DAC1_LEFT_VOLUME );
		val &= ~(WM8994_DAC1L_MUTE_MASK | WM8994_DAC1L_VOL_MASK);
		val |= TUNING_DAC1L_VOL; 
		wm8994_write(codec,WM8994_DAC1_LEFT_VOLUME ,val);
	
		//Unmute and volume ctrl RightDAC
		val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME ); 
		val &= ~(WM8994_DAC1R_MUTE_MASK | WM8994_DAC1R_VOL_MASK);
		val |= TUNING_DAC1R_VOL;
		wm8994_write(codec,WM8994_DAC1_RIGHT_VOLUME,val);
		
		val = wm8994_read(codec, WM8994_AIF1_DAC1_LEFT_VOLUME);
		val &= ~(WM8994_AIF1DAC1L_VOL_MASK);
		val |= (WM8994_AIF1DAC1_VU | TUNING_DAC1L_VOL);
		wm8994_write(codec, WM8994_AIF1_DAC1_LEFT_VOLUME, val);

		val = wm8994_read(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME);
		val &= ~(WM8994_AIF1DAC1R_VOL_MASK);
		val |= (WM8994_AIF1DAC1_VU |TUNING_DAC1R_VOL);
		wm8994_write(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME, val);
	}

	val = wm8994_read(codec,WM8994_OUTPUT_MIXER_1);
        val &= ~(WM8994_DAC1L_TO_MIXOUTL_MASK);
        val |= (WM8994_DAC1L_TO_MIXOUTL);
        wm8994_write(codec,WM8994_OUTPUT_MIXER_1,val);

        val = wm8994_read(codec,WM8994_OUTPUT_MIXER_2);
        val &= ~(WM8994_DAC1R_TO_MIXOUTR_MASK);
        val |= (WM8994_DAC1R_TO_MIXOUTR);
        wm8994_write(codec,WM8994_OUTPUT_MIXER_2,val);

	val = wm8994_read(codec,WM8994_HPOUT2_MIXER);
        val &= ~(WM8994_MIXOUTLVOL_TO_HPOUT2_MASK | WM8994_MIXOUTRVOL_TO_HPOUT2_MASK);
        val |= (WM8994_MIXOUTRVOL_TO_HPOUT2 | WM8994_MIXOUTLVOL_TO_HPOUT2);
        wm8994_write(codec,WM8994_HPOUT2_MIXER,val);

        val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_5);
        val &= ~(WM8994_DAC1R_ENA_MASK | WM8994_DAC1L_ENA_MASK | WM8994_AIF1DAC1R_ENA_MASK | WM8994_AIF1DAC1L_ENA_MASK);
        val |= (WM8994_AIF1DAC1L_ENA | WM8994_AIF1DAC1R_ENA | WM8994_DAC1L_ENA | WM8994_DAC1R_ENA);
        wm8994_write(codec,WM8994_POWER_MANAGEMENT_5,val);

        val = wm8994_read(codec,WM8994_AIF1_DAC1_FILTERS_1);
	val &= ~(WM8994_AIF1DAC1_MUTE_MASK |WM8994_AIF1DAC1_MONO_MASK);
	val |= (WM8994_AIF1DAC1_UNMUTE | WM8994_AIF1DAC1_MONO);
        wm8994_write(codec,WM8994_AIF1_DAC1_FILTERS_1,val);

	val = wm8994_read(codec,WM8994_DAC1_LEFT_MIXER_ROUTING);
        val &= ~(WM8994_AIF1DAC1L_TO_DAC1L_MASK);
        val |= (WM8994_AIF1DAC1L_TO_DAC1L);
        wm8994_write(codec,WM8994_DAC1_LEFT_MIXER_ROUTING,val);

        val = wm8994_read(codec,WM8994_DAC1_RIGHT_MIXER_ROUTING);
        val &= ~(WM8994_AIF1DAC1R_TO_DAC1R_MASK);
        val |= (WM8994_AIF1DAC1R_TO_DAC1R);
        wm8994_write(codec,WM8994_DAC1_RIGHT_MIXER_ROUTING,val);

	val = wm8994_read(codec, WM8994_CLOCKING_1);
        val &= ~(WM8994_DSP_FS1CLK_ENA_MASK | WM8994_DSP_FSINTCLK_ENA_MASK);
        val |= (WM8994_DSP_FS1CLK_ENA | WM8994_DSP_FSINTCLK_ENA);
        wm8994_write(codec, WM8994_CLOCKING_1, val);

	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_3);
        val &= ~(WM8994_MIXOUTLVOL_ENA_MASK | WM8994_MIXOUTRVOL_ENA_MASK | WM8994_MIXOUTL_ENA_MASK | WM8994_MIXOUTR_ENA_MASK);
        val |= (WM8994_MIXOUTL_ENA | WM8994_MIXOUTR_ENA | WM8994_MIXOUTRVOL_ENA | WM8994_MIXOUTLVOL_ENA);
        wm8994_write(codec,WM8994_POWER_MANAGEMENT_3,val);
		
	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_1 );
	val &= ~(WM8994_BIAS_ENA_MASK | WM8994_VMID_SEL_MASK | WM8994_HPOUT2_ENA_MASK );
	val |= (WM8994_BIAS_ENA | WM8994_VMID_SEL_NORMAL | WM8994_HPOUT2_ENA );
	wm8994_write(codec,WM8994_POWER_MANAGEMENT_1,val);
}


void wm8994_set_playback_headset(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->private_data;

	u16 val;
	
	u16 TestReturn1=0;
	u16 TestReturn2=0;
	u16 TestLow1=0;
	u16 TestHigh1=0;
	u8 TestLow=0;
	u8 TestHigh=0;
	
	DEBUG_LOG("");

	//Configuring the Digital Paths


	// Enable the Timeslot0 to DAC1L
	val = wm8994_read(codec, WM8994_DAC1_LEFT_MIXER_ROUTING  ); 	
	val &= ~(WM8994_AIF1DAC1L_TO_DAC1L_MASK);
	val |= WM8994_AIF1DAC1L_TO_DAC1L;
	wm8994_write(codec,WM8994_DAC1_LEFT_MIXER_ROUTING ,val);
	

	//Enable the Timeslot0 to DAC1R
	val = wm8994_read(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING  ); 	
	val &= ~(WM8994_AIF1DAC1R_TO_DAC1R_MASK);
	val |= WM8994_AIF1DAC1R_TO_DAC1R;
	wm8994_write(codec,WM8994_DAC1_RIGHT_MIXER_ROUTING ,val);

		val = wm8994_read(codec, 0x102  ); 	
		val &= ~(0x0003);
		val = 0x0003;
		wm8994_write(codec,0x102,val);
	
		val = wm8994_read(codec, 0x56  ); 	
		val &= ~(0x0003);
		val = 0x0003;
		wm8994_write(codec,0x56,val);
	
		val = wm8994_read(codec, 0x102  ); 	
		val &= ~(0x0000);
		val = 0x0000;
		wm8994_write(codec,0x102,val);

		val = wm8994_read(codec, WM8994_CLASS_W_1  ); 	
		val &= ~(0x0005);
		val |= 0x0005;
		wm8994_write(codec,WM8994_CLASS_W_1,val);

	// Headset Control
	if(wm8994->testmode_config_flag == SEC_NORMAL)
	{

		val = wm8994_read(codec,WM8994_LEFT_OUTPUT_VOLUME);
		val &= ~(WM8994_HPOUT1L_MUTE_N_MASK | WM8994_HPOUT1L_VOL_MASK);
		if(wm8994->QIK_state == QIK_ON)
			val |= (WM8994_HPOUT1_VU | WM8994_HPOUT1L_MUTE_N | TUNING_QIK_OUTPUTL_VOL);
		else if(wm8994->ringtone_active)
			val |= (WM8994_HPOUT1_VU | WM8994_HPOUT1L_MUTE_N | TUNING_RING_OUTPUTL_VOL);
		else
			val |= (WM8994_HPOUT1_VU | WM8994_HPOUT1L_MUTE_N | TUNING_MP3_OUTPUTL_VOL);
		wm8994_write(codec, WM8994_LEFT_OUTPUT_VOLUME, val);
	

		val = wm8994_read(codec,WM8994_RIGHT_OUTPUT_VOLUME);
		val &= ~(WM8994_HPOUT1R_MUTE_N_MASK | WM8994_HPOUT1R_VOL_MASK);
		if(wm8994->QIK_state == QIK_ON)
			val |= (WM8994_HPOUT1_VU | WM8994_HPOUT1R_MUTE_N | TUNING_QIK_OUTPUTR_VOL);
		else if(wm8994->ringtone_active)
			val |= (WM8994_HPOUT1_VU | WM8994_HPOUT1R_MUTE_N | TUNING_RING_OUTPUTR_VOL);
		else
			val |= (WM8994_HPOUT1_VU | WM8994_HPOUT1R_MUTE_N | TUNING_MP3_OUTPUTR_VOL);
		wm8994_write(codec, WM8994_RIGHT_OUTPUT_VOLUME, val);
	

		val = wm8994_read(codec, WM8994_LEFT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTL_MUTE_N_MASK | WM8994_MIXOUTL_VOL_MASK);
		if(wm8994->QIK_state == QIK_ON)
			val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTL_MUTE_N | TUNING_QIK_OPGAL_VOL);
		else if(wm8994->ringtone_active)
			val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTL_MUTE_N | TUNING_RING_OPGAL_VOL);
		else
			val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTL_MUTE_N | TUNING_MP3_OPGAL_VOL);
		wm8994_write(codec,WM8994_LEFT_OPGA_VOLUME, val );
	
		val = wm8994_read(codec, WM8994_RIGHT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTR_MUTE_N_MASK | WM8994_MIXOUTR_VOL_MASK);
		if(wm8994->QIK_state == QIK_ON)
			val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTR_MUTE_N | TUNING_QIK_OPGAR_VOL);
		else if(wm8994->ringtone_active)
			val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTR_MUTE_N | TUNING_RING_OPGAR_VOL);
		else
			val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTR_MUTE_N | TUNING_MP3_OPGAR_VOL);
		wm8994_write(codec,WM8994_RIGHT_OPGA_VOLUME, val);
		
		val = wm8994_read(codec, WM8994_AIF1_DAC1_LEFT_VOLUME);
		val &= ~(WM8994_AIF1DAC1L_VOL_MASK);
		if(wm8994->QIK_state == QIK_ON)
			val |= (WM8994_AIF1DAC1_VU | TUNING_QIK_DAC1L_VOL);
		else
			val |= (WM8994_AIF1DAC1_VU | TUNING_DAC1L_VOL);
		wm8994_write(codec, WM8994_AIF1_DAC1_LEFT_VOLUME, val);

		val = wm8994_read(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME);
		val &= ~(WM8994_AIF1DAC1R_VOL_MASK);
		if(wm8994->QIK_state == QIK_ON)
			val |= (WM8994_AIF1DAC1_VU |TUNING_QIK_DAC1R_VOL);
		else
			val |= (WM8994_AIF1DAC1_VU |TUNING_DAC1R_VOL);
		wm8994_write(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME, val);
	}
	
		val = wm8994_read(codec, WM8994_DC_SERVO_2  ); 	
		val &= ~(0x03E0);
		val = 0x03E0;
		wm8994_write(codec,WM8994_DC_SERVO_2,val);

	//Enable vmid,bias, hp left and right
	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_1 );
	val &= ~(WM8994_BIAS_ENA_MASK | WM8994_VMID_SEL_MASK |WM8994_HPOUT1L_ENA_MASK |WM8994_HPOUT1R_ENA_MASK | 
		WM8994_SPKOUTR_ENA_MASK | WM8994_SPKOUTL_ENA_MASK);
	val |= (WM8994_BIAS_ENA | WM8994_VMID_SEL_NORMAL | WM8994_HPOUT1R_ENA | WM8994_HPOUT1L_ENA);  
	wm8994_write(codec,WM8994_POWER_MANAGEMENT_1,val);

		val = wm8994_read(codec, WM8994_ANALOGUE_HP_1  ); 	
		val &= ~(0x0022);
		val = 0x0022;
		wm8994_write(codec,WM8994_ANALOGUE_HP_1,val);

		//Enable Charge Pump	
		val = wm8994_read(codec, WM8994_CHARGE_PUMP_1);
		val &= ~WM8994_CP_ENA_MASK ;
		val |= WM8994_CP_ENA | WM8994_CP_ENA_DEFAULT ; // this is from wolfson  	
		wm8994_write(codec, WM8994_CHARGE_PUMP_1, val);

		msleep(5);	// 5ms delay

	//Enable Dac1 and DAC2 and the Timeslot0 for AIF1	
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_5 ); 	
	val &= ~(WM8994_DAC1R_ENA_MASK | WM8994_DAC1L_ENA_MASK|WM8994_AIF1DAC1R_ENA_MASK |  WM8994_AIF1DAC1L_ENA_MASK );
	val |= (WM8994_AIF1DAC1L_ENA | WM8994_AIF1DAC1R_ENA  | WM8994_DAC1L_ENA |WM8994_DAC1R_ENA );
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_5 ,val);


	// Enable DAC1L to HPOUT1L path
	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);
	val &=  ~(WM8994_DAC1L_TO_HPOUT1L_MASK | WM8994_DAC1L_TO_MIXOUTL_MASK);
	val |= WM8994_DAC1L_TO_MIXOUTL ;  	
	wm8994_write(codec,WM8994_OUTPUT_MIXER_1, val);	// Enable MIXOUT
	
	// Enable DAC1R to HPOUT1R path
	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);
	val &= ~(WM8994_DAC1R_TO_HPOUT1R_MASK | WM8994_DAC1R_TO_MIXOUTR_MASK);
	val |= WM8994_DAC1R_TO_MIXOUTR;
	wm8994_write(codec, WM8994_OUTPUT_MIXER_2, val);
	
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_3);
	val &= ~(WM8994_MIXOUTLVOL_ENA_MASK | WM8994_MIXOUTRVOL_ENA_MASK | WM8994_MIXOUTL_ENA_MASK | WM8994_MIXOUTR_ENA_MASK |
		WM8994_SPKRVOL_ENA_MASK | WM8994_SPKLVOL_ENA_MASK);
	val |= (WM8994_MIXOUTLVOL_ENA | WM8994_MIXOUTRVOL_ENA | WM8994_MIXOUTL_ENA | WM8994_MIXOUTR_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, 0x0030);
	
		val = wm8994_read(codec, WM8994_DC_SERVO_1  ); 	
		val &= ~(0x0303);
		val = 0x0303;
		wm8994_write(codec,WM8994_DC_SERVO_1,val);
	
		msleep(160);	// 160ms delay

		TestReturn1=wm8994_read(codec,WM8994_DC_SERVO_4);
	
		TestLow=(signed char)(TestReturn1 & 0xff);
		TestHigh=(signed char)((TestReturn1>>8) & 0xff);

		TestLow1=((signed short)(TestLow-5))&0x00ff;
		TestHigh1=(((signed short)(TestHigh-5)<<8)&0xff00);
		TestReturn2=TestLow1|TestHigh1;
		wm8994_write(codec,WM8994_DC_SERVO_4, TestReturn2);

		val = wm8994_read(codec, WM8994_DC_SERVO_1  ); 	
		val &= ~(0x000F);
		val = 0x000F;
		wm8994_write(codec,WM8994_DC_SERVO_1,val);

		msleep(20);

		// Intermediate HP settings
		val = wm8994_read(codec, WM8994_ANALOGUE_HP_1); 	
		val &= ~(WM8994_HPOUT1R_DLY_MASK |WM8994_HPOUT1R_OUTP_MASK |WM8994_HPOUT1R_RMV_SHORT_MASK |
			WM8994_HPOUT1L_DLY_MASK |WM8994_HPOUT1L_OUTP_MASK | WM8994_HPOUT1L_RMV_SHORT_MASK);
		val = (WM8994_HPOUT1L_RMV_SHORT | WM8994_HPOUT1L_OUTP|WM8994_HPOUT1L_DLY |WM8994_HPOUT1R_RMV_SHORT | 
			WM8994_HPOUT1R_OUTP | WM8994_HPOUT1R_DLY);
		wm8994_write(codec, WM8994_ANALOGUE_HP_1, val);

	//Unmute DAC1 left
	val = wm8994_read(codec,WM8994_DAC1_LEFT_VOLUME );
	val &= ~(WM8994_DAC1L_MUTE_MASK | WM8994_DAC1L_VOL_MASK);
	if(wm8994->QIK_state == QIK_ON)
		val |= TUNING_QIK_DAC1L_VOL; 
	else
		val |= TUNING_DAC1L_VOL; 
	wm8994_write(codec,WM8994_DAC1_LEFT_VOLUME ,val);
	
	//Unmute and volume ctrl RightDAC
	val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME ); 
	val &= ~(WM8994_DAC1R_MUTE_MASK | WM8994_DAC1R_VOL_MASK);
	if(wm8994->QIK_state == QIK_ON)
		val |= TUNING_QIK_DAC1R_VOL; 
	else
		val |= TUNING_DAC1R_VOL; //0 db volume	
	wm8994_write(codec,WM8994_DAC1_RIGHT_VOLUME,val);	
	
	// Unmute the AF1DAC1	
	val = wm8994_read(codec, WM8994_AIF1_DAC1_FILTERS_1 ); 	
	val &= ~(WM8994_AIF1DAC1_MUTE_MASK | WM8994_AIF1DAC1_MONO_MASK);
	val |= WM8994_AIF1DAC1_UNMUTE;
	wm8994_write(codec,WM8994_AIF1_DAC1_FILTERS_1 ,val);
	
//	wm8994->codec_HPAmp_state =HPAMP_PLAYBACK;
	
}

void wm8994_set_playback_speaker(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->private_data;

	u16 val;

	DEBUG_LOG("");

	//Disable end point for preventing pop up noise.
	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_1);
	val &= ~(WM8994_SPKOUTL_ENA_MASK);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_3);
	val &= ~(WM8994_MIXOUTLVOL_ENA_MASK | WM8994_MIXOUTRVOL_ENA_MASK | WM8994_MIXOUTL_ENA_MASK | WM8994_MIXOUTR_ENA_MASK | 
		WM8994_SPKRVOL_ENA_MASK | WM8994_SPKLVOL_ENA_MASK);	
	val |= WM8994_SPKLVOL_ENA;
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, val);

	// Speaker Volume Control
	if(wm8994->testmode_config_flag  == SEC_NORMAL)
	{
		// Unmute the SPKMIXVOLUME
		val = wm8994_read(codec, WM8994_SPKMIXL_ATTENUATION);
		val &= ~(WM8994_SPKMIXL_VOL_MASK);
		val |= TUNING_SPKMIXL_ATTEN;	
		wm8994_write(codec, WM8994_SPKMIXL_ATTENUATION, val);
		
		val = wm8994_read(codec,WM8994_SPKMIXR_ATTENUATION);
		val &= ~(WM8994_SPKMIXR_VOL_MASK);
		wm8994_write(codec, WM8994_SPKMIXR_ATTENUATION, val);

		val = wm8994_read(codec,WM8994_SPEAKER_VOLUME_LEFT);
		val &= ~(WM8994_SPKOUTL_MUTE_N_MASK | WM8994_SPKOUTL_VOL_MASK);
		if(wm8994->ringtone_active)
			val |= (WM8994_SPKOUT_VU | WM8994_SPKOUTL_MUTE_N | TUNING_RING_SPKL_VOL);
		else
			val |= (WM8994_SPKOUT_VU | WM8994_SPKOUTL_MUTE_N | TUNING_MP3_SPKL_VOL);
		wm8994_write(codec, WM8994_SPEAKER_VOLUME_LEFT, val);

		val = wm8994_read(codec,WM8994_SPEAKER_VOLUME_RIGHT);
		val &= ~(WM8994_SPKOUTR_MUTE_N_MASK | WM8994_SPKOUTR_VOL_MASK);
		if(wm8994->ringtone_active)
			val |= (WM8994_SPKOUT_VU | WM8994_SPKOUTR_MUTE_N | TUNING_RING_SPKR_VOL);
		else
			val |= (WM8994_SPKOUT_VU | WM8994_SPKOUTR_MUTE_N | TUNING_MP3_SPKR_VOL);		
		wm8994_write(codec, WM8994_SPEAKER_VOLUME_RIGHT, val);

		val = wm8994_read(codec,WM8994_CLASSD);
		val &= ~(WM8994_SPKOUTL_BOOST_MASK);
		if(wm8994->ringtone_active)
			val |= TUNING_RING_CLASSD_VOL << WM8994_SPKOUTL_BOOST_SHIFT;
		else
			val |= TUNING_MP3_CLASSD_VOL << WM8994_SPKOUTL_BOOST_SHIFT;
		wm8994_write(codec, WM8994_CLASSD, val);
	
		//Unmute DAC1 left
		val = wm8994_read(codec,WM8994_DAC1_LEFT_VOLUME );
		val &= ~(WM8994_DAC1L_MUTE_MASK | WM8994_DAC1L_VOL_MASK);
		val |= TUNING_DAC1L_VOL; 
		wm8994_write(codec,WM8994_DAC1_LEFT_VOLUME ,val);
	
		//Unmute and volume ctrl RightDAC
		val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME ); 
		val &= ~(WM8994_DAC1R_MUTE_MASK | WM8994_DAC1R_VOL_MASK);
		val |= TUNING_DAC1R_VOL; //0 db volume	
		wm8994_write(codec,WM8994_DAC1_RIGHT_VOLUME,val);
		
		val = wm8994_read(codec, WM8994_AIF1_DAC1_LEFT_VOLUME);
		val &= ~(WM8994_AIF1DAC1L_VOL_MASK);
		val |= (WM8994_AIF1DAC1_VU | TUNING_DAC1L_VOL);
		wm8994_write(codec, WM8994_AIF1_DAC1_LEFT_VOLUME, val);

		val = wm8994_read(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME);
		val &= ~(WM8994_AIF1DAC1R_VOL_MASK);
		val |= (WM8994_AIF1DAC1_VU |TUNING_DAC1R_VOL);
		wm8994_write(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME, val);
	}

	val = wm8994_read(codec,WM8994_SPKOUT_MIXERS);
	val &= ~(WM8994_SPKMIXL_TO_SPKOUTL_MASK | 
		WM8994_SPKMIXR_TO_SPKOUTL_MASK | WM8994_SPKMIXR_TO_SPKOUTR_MASK);
	val |= WM8994_SPKMIXL_TO_SPKOUTL;
	wm8994_write(codec, WM8994_SPKOUT_MIXERS, val);

	//Unmute the DAC path
	val = wm8994_read(codec,WM8994_SPEAKER_MIXER);
	val &= ~(WM8994_DAC1L_TO_SPKMIXL_MASK);
	val |= WM8994_DAC1L_TO_SPKMIXL;
	wm8994_write(codec, WM8994_SPEAKER_MIXER, val);

	// Eable DAC1 Left and timeslot left
	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_5);	
	val &= ~( WM8994_DAC1L_ENA_MASK | WM8994_AIF1DAC1L_ENA_MASK);
	val |= (WM8994_AIF1DAC1L_ENA | WM8994_DAC1L_ENA|WM8994_AIF1DAC1R_ENA | WM8994_DAC1R_ENA); //hdlnc_ldj_0416_A1026
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_5, val);   

	//Unmute
	val = wm8994_read(codec, WM8994_AIF1_DAC1_FILTERS_1);
	val &= ~(WM8994_AIF1DAC1_MUTE_MASK | WM8994_AIF1DAC1_MONO_MASK);
	val |= (WM8994_AIF1DAC1_UNMUTE | WM8994_AIF1DAC1_MONO);
	wm8994_write(codec, WM8994_AIF1_DAC1_FILTERS_1, val);

	//enable timeslot0 to left dac
	val = wm8994_read(codec, WM8994_DAC1_LEFT_MIXER_ROUTING);
	val &= ~(WM8994_AIF1DAC1L_TO_DAC1L_MASK);
	val |= WM8994_AIF1DAC1L_TO_DAC1L;
	wm8994_write(codec, WM8994_DAC1_LEFT_MIXER_ROUTING, val);
	
	//Enbale bias,vmid and Left speaker
	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_1);
	val &= ~(WM8994_BIAS_ENA_MASK | WM8994_VMID_SEL_MASK |WM8994_HPOUT1L_ENA_MASK |WM8994_HPOUT1R_ENA_MASK | 
		WM8994_SPKOUTR_ENA_MASK | WM8994_SPKOUTL_ENA_MASK |WM8994_HPOUT2_ENA_MASK);
	val |= (WM8994_BIAS_ENA | WM8994_VMID_SEL_NORMAL | WM8994_SPKOUTL_ENA);  
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);
}

void wm8994_set_playback_speaker_headset(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->private_data;

	u16 val;

	u16 nReadServo4Val = 0;
	u16 ncompensationResult = 0;
	u16 nCompensationResultLow=0;
	u16 nCompensationResultHigh=0;
	u8  nServo4Low = 0;
	u8  nServo4High = 0;

	//------------------  Common Settings ------------------
	// Enable the Timeslot0 to DAC1L
	val = wm8994_read(codec, WM8994_DAC1_LEFT_MIXER_ROUTING  ); 	
	val &= ~(WM8994_AIF1DAC1L_TO_DAC1L_MASK);
	val |= WM8994_AIF1DAC1L_TO_DAC1L;
	wm8994_write(codec,WM8994_DAC1_LEFT_MIXER_ROUTING ,val);
	
	//Enable the Timeslot0 to DAC1R
	val = wm8994_read(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING  ); 	
	val &= ~(WM8994_AIF1DAC1R_TO_DAC1R_MASK);
	val |= WM8994_AIF1DAC1R_TO_DAC1R;
	wm8994_write(codec,WM8994_DAC1_RIGHT_MIXER_ROUTING ,val);
	
	val = wm8994_read(codec, WM8994_AIF1_DAC1_LEFT_VOLUME);
	val &= ~(WM8994_AIF1DAC1L_VOL_MASK);
	val |= (WM8994_AIF1DAC1_VU | TUNING_DAC1L_VOL);
	wm8994_write(codec, WM8994_AIF1_DAC1_LEFT_VOLUME, val);
	
	val = wm8994_read(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME);
	val &= ~(WM8994_AIF1DAC1R_VOL_MASK);
	val |= (WM8994_AIF1DAC1_VU |TUNING_DAC1R_VOL);
	wm8994_write(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME, val);

		//------------------  Speaker Path Settings ------------------

	// Speaker Volume Control
	if(wm8994->testmode_config_flag == SEC_NORMAL)
	{
		val = wm8994_read(codec,WM8994_SPEAKER_VOLUME_LEFT);
		val &= ~(WM8994_SPKOUTL_MUTE_N_MASK | WM8994_SPKOUTL_VOL_MASK);
		if(wm8994->ringtone_active)
			val |= (WM8994_SPKOUT_VU | WM8994_SPKOUTL_MUTE_N | TUNING_RING_SPKL_VOL);
		else
			val |= (WM8994_SPKOUT_VU | WM8994_SPKOUTL_MUTE_N | TUNING_MP3_SPKL_VOL);
		wm8994_write(codec, WM8994_SPEAKER_VOLUME_LEFT, val);

		val = wm8994_read(codec,WM8994_SPEAKER_VOLUME_RIGHT);
		val &= ~(WM8994_SPKOUTR_MUTE_N_MASK | WM8994_SPKOUTR_VOL_MASK);
		wm8994_write(codec, WM8994_SPEAKER_VOLUME_RIGHT, val);

		val = wm8994_read(codec,WM8994_CLASSD);
		val &= ~(WM8994_SPKOUTL_BOOST_MASK);
		if(wm8994->ringtone_active)
			val |= TUNING_RING_CLASSD_VOL << WM8994_SPKOUTL_BOOST_SHIFT;
		else
			val |= TUNING_MP3_CLASSD_VOL << WM8994_SPKOUTL_BOOST_SHIFT;
		wm8994_write(codec, WM8994_CLASSD, val);
	}

	val = wm8994_read(codec,WM8994_SPKOUT_MIXERS );
	val &= ~(WM8994_SPKMIXL_TO_SPKOUTL_MASK | 
		WM8994_SPKMIXR_TO_SPKOUTL_MASK | WM8994_SPKMIXR_TO_SPKOUTR_MASK);
	val |= WM8994_SPKMIXL_TO_SPKOUTL;
	wm8994_write(codec,WM8994_SPKOUT_MIXERS,val );

	//Unmute the DAC path
	val = wm8994_read(codec,WM8994_SPEAKER_MIXER );
	val &= ~(WM8994_DAC1L_TO_SPKMIXL_MASK );
	val |= WM8994_DAC1L_TO_SPKMIXL ;
	wm8994_write(codec,WM8994_SPEAKER_MIXER ,val);

	//------------------  Ear Path Settings ------------------
	//Configuring the Digital Paths
		val = wm8994_read(codec, 0x102);
		val &= ~(0x0003);
		val = 0x0003;
		wm8994_write(codec,0x102,val);
	
		val = wm8994_read(codec, 0x56  ); 	
		val &= ~(0x0003);
		val = 0x0003;
		wm8994_write(codec,0x56,val);
	
		val = wm8994_read(codec, 0x102  ); 	
		val &= ~(0x0000);
		val = 0x0000;
		wm8994_write(codec,0x102,val);

		val = wm8994_read(codec, WM8994_CLASS_W_1  ); 	
		val &= ~(0x0005);
		val = 0x0005;
		wm8994_write(codec,WM8994_CLASS_W_1,val);

	if(wm8994->testmode_config_flag == SEC_NORMAL)
	{
		val = wm8994_read(codec,WM8994_LEFT_OUTPUT_VOLUME);
		val &= ~(WM8994_HPOUT1L_MUTE_N_MASK | WM8994_HPOUT1L_VOL_MASK);
		if(wm8994->ringtone_active)
			val |= (WM8994_HPOUT1_VU | WM8994_HPOUT1L_MUTE_N | TUNING_RING_DUAL_OUTPUTL_VOL);
		else
			val |= (WM8994_HPOUT1_VU | WM8994_HPOUT1L_MUTE_N | TUNING_MP3_DUAL_OUTPUTL_VOL);
		wm8994_write(codec, WM8994_LEFT_OUTPUT_VOLUME, val);

		val = wm8994_read(codec,WM8994_RIGHT_OUTPUT_VOLUME);
		val &= ~(WM8994_HPOUT1R_MUTE_N_MASK | WM8994_HPOUT1R_VOL_MASK);
		if(wm8994->ringtone_active)
			val |= (WM8994_HPOUT1_VU | WM8994_HPOUT1R_MUTE_N | TUNING_RING_DUAL_OUTPUTR_VOL);
		else
			val |= (WM8994_HPOUT1_VU | WM8994_HPOUT1L_MUTE_N | TUNING_MP3_DUAL_OUTPUTR_VOL);
		wm8994_write(codec, WM8994_RIGHT_OUTPUT_VOLUME, val);
	}

		//* DC Servo Series Count
		val = 0x03E0;
		wm8994_write(codec, WM8994_DC_SERVO_2, val);

	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);
	val &= ~(WM8994_BIAS_ENA_MASK | WM8994_VMID_SEL_MASK |
		WM8994_HPOUT1L_ENA_MASK |WM8994_HPOUT1R_ENA_MASK|WM8994_SPKOUTL_ENA_MASK);
	val |= (WM8994_BIAS_ENA | WM8994_VMID_SEL_NORMAL |
		WM8994_HPOUT1R_ENA	 |WM8994_HPOUT1L_ENA |WM8994_SPKOUTL_ENA);
		wm8994_write(codec,WM8994_POWER_MANAGEMENT_1,val);

		val = (WM8994_HPOUT1L_DLY | WM8994_HPOUT1R_DLY);
		wm8994_write(codec, WM8994_ANALOGUE_HP_1, val);

		//Enable Charge Pump	
		val = wm8994_read(codec, WM8994_CHARGE_PUMP_1);
		val &= ~WM8994_CP_ENA_MASK ;
		val |= WM8994_CP_ENA | WM8994_CP_ENA_DEFAULT ; // this is from wolfson  	
		wm8994_write(codec, WM8994_CHARGE_PUMP_1, val);

		msleep(5);

	//Enable DAC1 and DAC2 and the Timeslot0 for AIF1	
	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_5 );	
	val &= ~(WM8994_DAC1R_ENA_MASK | WM8994_DAC1L_ENA_MASK|WM8994_AIF1DAC1R_ENA_MASK |  WM8994_AIF1DAC1L_ENA_MASK );
	val |= (WM8994_AIF1DAC1L_ENA | WM8994_AIF1DAC1R_ENA  | WM8994_DAC1L_ENA |WM8994_DAC1R_ENA );
	wm8994_write(codec,WM8994_POWER_MANAGEMENT_5,val);   

	// Enbale DAC1L to HPOUT1L path
	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);
	val &=  ~(WM8994_DAC1L_TO_HPOUT1L_MASK | WM8994_DAC1L_TO_MIXOUTL_MASK);
	val |=  WM8994_DAC1L_TO_MIXOUTL;
	wm8994_write(codec,WM8994_OUTPUT_MIXER_1, val);

	// Enbale DAC1R to HPOUT1R path
	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);
	val &= ~(WM8994_DAC1R_TO_HPOUT1R_MASK | WM8994_DAC1R_TO_MIXOUTR_MASK);
	val |= WM8994_DAC1R_TO_MIXOUTR;
	wm8994_write(codec, WM8994_OUTPUT_MIXER_2, val);

	//Enbale bias,vmid, hp left and right and Left speaker
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_3);
	val &= ~(WM8994_MIXOUTLVOL_ENA_MASK | WM8994_MIXOUTRVOL_ENA_MASK | WM8994_MIXOUTL_ENA_MASK | WM8994_MIXOUTR_ENA_MASK | WM8994_SPKLVOL_ENA_MASK);
	val |= (  WM8994_MIXOUTL_ENA | WM8994_MIXOUTR_ENA | WM8994_SPKLVOL_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_3 ,val);

		//* DC Servo 
		val = (WM8994_DCS_TRIG_SERIES_1 | WM8994_DCS_TRIG_SERIES_0 | WM8994_DCS_ENA_CHAN_1 | WM8994_DCS_ENA_CHAN_0);
		wm8994_write(codec,WM8994_DC_SERVO_1, 0x0303 );

		msleep(160);

		nReadServo4Val=wm8994_read(codec,WM8994_DC_SERVO_4);
		nServo4Low=(signed char)(nReadServo4Val & 0xff);
		nServo4High=(signed char)((nReadServo4Val>>8) & 0xff);
		
		nCompensationResultLow=((signed short)nServo4Low -5)&0x00ff;
		nCompensationResultHigh=((signed short)(nServo4High -5)<<8)&0xff00;
		ncompensationResult=nCompensationResultLow|nCompensationResultHigh;
		wm8994_write(codec,WM8994_DC_SERVO_4, ncompensationResult);

		val = (WM8994_DCS_TRIG_DAC_WR_1 | WM8994_DCS_TRIG_DAC_WR_0 | WM8994_DCS_ENA_CHAN_1 | WM8994_DCS_ENA_CHAN_0);
		wm8994_write(codec,WM8994_DC_SERVO_1, val );

		msleep(15);	

		// Intermediate HP settings
		val = wm8994_read(codec, WM8994_ANALOGUE_HP_1); 	
		val &= ~(WM8994_HPOUT1R_DLY_MASK |WM8994_HPOUT1R_OUTP_MASK |WM8994_HPOUT1R_RMV_SHORT_MASK |
			WM8994_HPOUT1L_DLY_MASK |WM8994_HPOUT1L_OUTP_MASK | WM8994_HPOUT1L_RMV_SHORT_MASK);
		val |= (WM8994_HPOUT1L_RMV_SHORT | WM8994_HPOUT1L_OUTP|WM8994_HPOUT1L_DLY |WM8994_HPOUT1R_RMV_SHORT | 
			WM8994_HPOUT1R_OUTP | WM8994_HPOUT1R_DLY);
		wm8994_write(codec, WM8994_ANALOGUE_HP_1, val);

	if(wm8994->testmode_config_flag == SEC_NORMAL)
	{
		//Unmute DAC1 left
		val = wm8994_read(codec,WM8994_DAC1_LEFT_VOLUME );
		val &= ~(WM8994_DAC1L_MUTE_MASK | WM8994_DAC1L_VOL_MASK);
		val |= TUNING_DAC1L_VOL; 
		wm8994_write(codec,WM8994_DAC1_LEFT_VOLUME ,val);

		//Unmute and volume ctrl RightDAC
		val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME ); 
		val &= ~(WM8994_DAC1R_MUTE_MASK | WM8994_DAC1R_VOL_MASK);
		val |= TUNING_DAC1R_VOL; //0 db volume	
		wm8994_write(codec,WM8994_DAC1_RIGHT_VOLUME,val);
	}
//------------------  Common Settings ------------------
	// Unmute the AIF1DAC1	
	val = wm8994_read(codec, WM8994_AIF1_DAC1_FILTERS_1 ); 	
	val &= ~(WM8994_AIF1DAC1_MUTE_MASK | WM8994_AIF1DAC1_MONO_MASK);
	val |= (WM8994_AIF1DAC1_UNMUTE | WM8994_AIF1DAC1_MONO);
	wm8994_write(codec,WM8994_AIF1_DAC1_FILTERS_1 ,val);


	if(wm8994->testmode_config_flag == SEC_NORMAL)
	{
		// Unmute the SPKMIXVOLUME
		val = wm8994_read(codec, WM8994_SPKMIXL_ATTENUATION);
		val &= ~(WM8994_SPKMIXL_VOL_MASK);
		val |= TUNING_SPKMIXL_ATTEN;	
		wm8994_write(codec, WM8994_SPKMIXL_ATTENUATION, val);

		val = wm8994_read(codec,WM8994_SPKMIXR_ATTENUATION);
		val &= ~(WM8994_SPKMIXR_VOL_MASK);
		wm8994_write(codec, WM8994_SPKMIXR_ATTENUATION, val);
	}
	
}                                                      

void wm8994_set_playback_bluetooth(struct snd_soc_codec *codec)
{
	u16 val;

	DEBUG_LOG("BT Playback Path for Voice Command\n");
	
	struct wm8994_priv *wm8994 = codec->private_data;
	//wm8994_set_voicecall_common_setting(codec);
	
	//wm8994_set_voicecall_common_setting(codec);
	/*GPIO Configuration*/
	wm8994_write(codec, WM8994_GPIO_1, 0xA101);
	wm8994_write(codec, WM8994_GPIO_2, 0x8100);
	wm8994_write(codec, WM8994_GPIO_3, 0x0100);
	wm8994_write(codec, WM8994_GPIO_4, 0x0100);
	wm8994_write(codec, WM8994_GPIO_5, 0x8100);
	wm8994_write(codec, WM8994_GPIO_6, 0xA101);
	wm8994_write(codec, WM8994_GPIO_7, 0x0100);
	wm8994_write(codec, WM8994_GPIO_8, 0xA101);
	wm8994_write(codec, WM8994_GPIO_9, 0xA101);
	wm8994_write(codec, WM8994_GPIO_10, 0xA101);
	wm8994_write(codec, WM8994_GPIO_11, 0xA101);

	/*FLL2 Setting*/
	wm8994_write(codec, WM8994_FLL2_CONTROL_2, 0x2F00);
	wm8994_write(codec, WM8994_FLL2_CONTROL_3, 0x3126);
	wm8994_write(codec, WM8994_FLL2_CONTROL_4, 0x0100);
	wm8994_write(codec, WM8994_FLL2_CONTROL_5, 0x0C88);
//	wm8994_write(codec, WM8994_FLL2_CONTROL_5, 0x0C89); // CP 24MHz
	wm8994_write(codec, WM8994_FLL2_CONTROL_1, WM8994_FLL2_FRACN_ENA | WM8994_FLL2_ENA);

	val = wm8994_read(codec, WM8994_AIF2_CLOCKING_1);
	if(!(val & WM8994_AIF2CLK_ENA))
		wm8994_write(codec, WM8994_AIF2_CLOCKING_1, 0x0018);

	wm8994_write(codec, WM8994_AIF2_RATE, 0x3 << WM8994_AIF2CLK_RATE_SHIFT);
	
	if(wm8994->hw_version == 3) // H/W Rev D
	{	
	// AIF2 Interface - PCM Stereo mode
		wm8994_write(codec, WM8994_AIF2_CONTROL_1,	//Left Justified, BCLK invert, LRCLK Invert
		WM8994_AIF2ADCR_SRC | WM8994_AIF2_BCLK_INV |0x18);
		wm8994_write(codec, WM8994_AIF2_BCLK, 0x70);
	}
	else // H/W Rev B
	{
		wm8994_write(codec, WM8994_AIF2_CONTROL_1,	//Left Justified, BCLK invert, LRCLK Invert
		WM8994_AIF2ADCR_SRC | WM8994_AIF2_BCLK_INV |WM8994_AIF2_LRCLK_INV | 0x1 << WM8994_AIF2_FMT_SHIFT);
	}		

	wm8994_write(codec, WM8994_AIF2_CONTROL_2, 0x0000);
	wm8994_write(codec, WM8994_AIF2_MASTER_SLAVE, WM8994_AIF2_MSTR | WM8994_AIF2_CLK_FRC | WM8994_AIF2_LRCLK_FRC);	//Master

	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_5);	//05 : 3303
	val &= ~(WM8994_AIF2DACL_ENA_MASK | WM8994_AIF2DACR_ENA_MASK | WM8994_AIF1DAC1L_ENA_MASK | WM8994_AIF1DAC1R_ENA_MASK
		 | WM8994_DAC1L_ENA_MASK | WM8994_DAC1R_ENA_MASK);
	val |= (WM8994_AIF2DACL_ENA | WM8994_AIF2DACR_ENA | WM8994_AIF1DAC1L_ENA | WM8994_AIF1DAC1R_ENA
		 | WM8994_DAC1L_ENA | WM8994_DAC1R_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_5, val);

	/*Clocking*/	
	val = wm8994_read(codec, WM8994_CLOCKING_1);
	val |= (WM8994_DSP_FS2CLK_ENA);
	wm8994_write(codec, WM8994_CLOCKING_1, val);

	wm8994_write(codec, WM8994_POWER_MANAGEMENT_6, 0x0);

	// AIF1 & AIF2 Output is connected to DAC1	
	val = wm8994_read(codec,WM8994_DAC1_LEFT_MIXER_ROUTING);	
	val &= ~(WM8994_AIF1DAC1L_TO_DAC1L_MASK | WM8994_AIF2DACL_TO_DAC1L_MASK);	
	val |= (WM8994_AIF1DAC1L_TO_DAC1L);
	wm8994_write(codec,WM8994_DAC1_LEFT_MIXER_ROUTING, val );  

	val = wm8994_read(codec,WM8994_DAC1_RIGHT_MIXER_ROUTING);	
	val &= ~(WM8994_AIF1DAC1R_TO_DAC1R_MASK | WM8994_AIF2DACR_TO_DAC1R_MASK);	
	val |= (WM8994_AIF1DAC1R_TO_DAC1R);
	wm8994_write(codec,WM8994_DAC1_RIGHT_MIXER_ROUTING, val);
	
	val = wm8994_read(codec, WM8994_AIF1_DAC1_LEFT_VOLUME);
	val &= ~(WM8994_AIF1DAC1L_VOL_MASK);
	val |= (WM8994_AIF1DAC1_VU | TUNING_DAC1L_VOL);
	wm8994_write(codec, WM8994_AIF1_DAC1_LEFT_VOLUME, val);
	
	val = wm8994_read(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME);
	val &= ~(WM8994_AIF1DAC1R_VOL_MASK);
	val |= (WM8994_AIF1DAC1_VU |TUNING_DAC1R_VOL);
	wm8994_write(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME, val);

	wm8994_write(codec, 0x6, 0x0);
	/*GPIO Configuration*/		
	wm8994_write(codec, WM8994_GPIO_8, WM8994_GP8_DIR | WM8994_GP8_DB);
	wm8994_write(codec, WM8994_GPIO_9, WM8994_GP9_DB);
	wm8994_write(codec, WM8994_GPIO_10, WM8994_GP10_DB);
	wm8994_write(codec, WM8994_GPIO_11, WM8994_GP11_DB);

	/*Digital Path Enables and Unmutes*/		 
	val  = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);
	val &= ~(WM8994_SPKOUTL_ENA_MASK | WM8994_HPOUT2_ENA_MASK | WM8994_HPOUT1L_ENA_MASK | WM8994_HPOUT1R_ENA_MASK);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

//	wm8994_write(codec, WM8994_POWER_MANAGEMENT_4, WM8994_AIF2ADCL_ENA | WM8994_ADCL_ENA);
		wm8994_write(codec, WM8994_POWER_MANAGEMENT_4, 0x3303);
	// If Input MIC is enabled, bluetooth Rx is muted.
	wm8994_write(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME, WM8994_IN1L_MUTE);
	wm8994_write(codec, WM8994_RIGHT_LINE_INPUT_1_2_VOLUME, WM8994_IN1R_MUTE);
	wm8994_write(codec, WM8994_INPUT_MIXER_2, 0x00);
	wm8994_write(codec, WM8994_INPUT_MIXER_3, 0x00);
	wm8994_write(codec, WM8994_INPUT_MIXER_4, 0x00);
	
	//for BT DTMF Play
	//Rx Path: AIF2ADCDAT2 select
	//CP(CALL) Path:GPIO5/DACDAT2 select
	//AP(DTMF) Path: DACDAT1 select
	//Tx Path: GPIO8/DACDAT3 select
	wm8994_write(codec	,	0x310	,	0x4118	)	;
//	wm8994_write(codec, WM8994_POWER_MANAGEMENT_6, 0x000C);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_6, 0x000a);
	// AIF1 & AIF2 Output is connected to DAC1	
	wm8994_write(codec, WM8994_DAC2_LEFT_MIXER_ROUTING,/* WM8994_AIF2DACL_TO_DAC2L |*/ WM8994_AIF1DAC1L_TO_DAC2L);
	wm8994_write(codec, WM8994_DAC2_RIGHT_MIXER_ROUTING,/* WM8994_AIF2DACR_TO_DAC2R |*/ WM8994_AIF1DAC1R_TO_DAC2R);

	wm8994_write(codec, WM8994_AIF2_CLOCKING_1, 0x0019);

	wm8994_write(codec, WM8994_DAC2_MIXER_VOLUMES, 0x000C); 

	wm8994_write(codec, WM8994_DAC2_LEFT_VOLUME, 0x01C0);
	wm8994_write(codec, WM8994_DAC2_RIGHT_VOLUME, 0x01C0); 

	wm8994_write(codec, WM8994_OVERSAMPLING, 0X0000);

	//Unmute DAC1 left
	val = wm8994_read(codec,WM8994_DAC1_LEFT_VOLUME );
	val &= ~(WM8994_DAC1L_MUTE_MASK | WM8994_DAC1L_VOL_MASK);
	val |= TUNING_DAC1L_VOL; 
	wm8994_write(codec,WM8994_DAC1_LEFT_VOLUME ,val);
	
	//Unmute and volume ctrl RightDAC
	val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME ); 
	val &= ~(WM8994_DAC1R_MUTE_MASK | WM8994_DAC1R_VOL_MASK);
	val |= TUNING_DAC1R_VOL; //0 db volume	
	wm8994_write(codec,WM8994_DAC1_RIGHT_VOLUME,val);
	
	wm8994_write(codec, WM8994_AIF1_DAC1_FILTERS_1, 0x0000);  
	wm8994_write(codec, WM8994_AIF2_DAC_FILTERS_1, 0x0000);

}

void wm8994_set_playback_extra_dock_speaker(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->private_data;

	u16 val;

	DEBUG_LOG("");

	//OUTPUT mute
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_3);
	val &= ~(WM8994_LINEOUT2N_ENA_MASK | WM8994_LINEOUT2P_ENA_MASK);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, val);

	
	if(wm8994->testmode_config_flag == SEC_NORMAL)
	{
        val = wm8994_read(codec, WM8994_OUTPUT_MIXER_5);
		val &= ~(WM8994_DACL_MIXOUTL_VOL_MASK);
        val |= TUNING_EXTRA_DOCK_SPK_OUTMIX5_VOL << WM8994_DACL_MIXOUTL_VOL_SHIFT;
        wm8994_write(codec,WM8994_OUTPUT_MIXER_5, val );
	
		val = wm8994_read(codec, WM8994_OUTPUT_MIXER_6);
		val &= ~(WM8994_DACR_MIXOUTR_VOL_MASK);
		val |= TUNING_EXTRA_DOCK_SPK_OUTMIX6_VOL << WM8994_DACR_MIXOUTR_VOL_SHIFT;
		wm8994_write(codec,WM8994_OUTPUT_MIXER_6, val );

		val = wm8994_read(codec, WM8994_LEFT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTL_MUTE_N_MASK | WM8994_MIXOUTL_VOL_MASK);
		val |= (WM8994_MIXOUT_VU |WM8994_MIXOUTL_MUTE_N | TUNING_MP3_EXTRA_DOCK_SPK_OPGAL_VOL);
		wm8994_write(codec,WM8994_LEFT_OPGA_VOLUME, val );

		val = wm8994_read(codec, WM8994_RIGHT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTR_MUTE_N_MASK | WM8994_MIXOUTR_VOL_MASK);
		val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTR_MUTE_N | TUNING_MP3_EXTRA_DOCK_SPK_OPGAR_VOL);
		wm8994_write(codec,WM8994_RIGHT_OPGA_VOLUME, val );

		val = wm8994_read(codec,WM8994_LINE_OUTPUTS_VOLUME);
		val &= ~(WM8994_LINEOUT2_VOL_MASK);
		val |= (TUNING_MP3_EXTRA_DOCK_SPK_VOL << WM8994_LINEOUT2_VOL_SHIFT);		
		wm8994_write(codec,WM8994_LINE_OUTPUTS_VOLUME,val);	
		//Unmute DAC1 left
		val = wm8994_read(codec,WM8994_DAC1_LEFT_VOLUME );
		val &= ~(WM8994_DAC1L_MUTE_MASK | WM8994_DAC1L_VOL_MASK);
		val |= TUNING_DAC1L_VOL; 
		wm8994_write(codec,WM8994_DAC1_LEFT_VOLUME ,val);
	
		//Unmute and volume ctrl RightDAC
		val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME ); 
		val &= ~(WM8994_DAC1R_MUTE_MASK | WM8994_DAC1R_VOL_MASK);
		val |= TUNING_DAC1R_VOL;
		wm8994_write(codec,WM8994_DAC1_RIGHT_VOLUME,val);
		
		val = wm8994_read(codec, WM8994_AIF1_DAC1_LEFT_VOLUME);
		val &= ~(WM8994_AIF1DAC1L_VOL_MASK);
		val |= (WM8994_AIF1DAC1_VU | TUNING_DAC1L_VOL);
		wm8994_write(codec, WM8994_AIF1_DAC1_LEFT_VOLUME, val);

		val = wm8994_read(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME);
		val &= ~(WM8994_AIF1DAC1R_VOL_MASK);
		val |= (WM8994_AIF1DAC1_VU |TUNING_DAC1R_VOL);
		wm8994_write(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME, val);
	}

	// For X-talk of VPS's L/R line. It's requested by H/W team.

	wm8994_write(codec, WM8994_ADDITIONAL_CONTROL, 0x00);
	wm8994_write(codec, WM8994_ANTIPOP_1, 0x80);

	val = wm8994_read(codec,WM8994_OUTPUT_MIXER_1);
	val &= ~(WM8994_DAC1L_TO_MIXOUTL_MASK);
	val |= (WM8994_DAC1L_TO_MIXOUTL);
	wm8994_write(codec,WM8994_OUTPUT_MIXER_1,val);

	val = wm8994_read(codec,WM8994_OUTPUT_MIXER_2);
	val &= ~(WM8994_DAC1R_TO_MIXOUTR_MASK);
	val |= (WM8994_DAC1R_TO_MIXOUTR);
	wm8994_write(codec,WM8994_OUTPUT_MIXER_2,val);


	val = wm8994_read(codec,WM8994_LINE_MIXER_2);
	val &= ~(WM8994_MIXOUTR_TO_LINEOUT2N_MASK | WM8994_MIXOUTL_TO_LINEOUT2N_MASK | WM8994_LINEOUT2_MODE_MASK | WM8994_MIXOUTR_TO_LINEOUT2P_MASK);
	val |= (WM8994_MIXOUTL_TO_LINEOUT2N | WM8994_LINEOUT2_MODE | WM8994_MIXOUTR_TO_LINEOUT2P);
	wm8994_write(codec,WM8994_LINE_MIXER_2,val);

	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_5);
	val &= ~(WM8994_DAC1R_ENA_MASK | WM8994_DAC1L_ENA_MASK | WM8994_AIF1DAC1R_ENA_MASK | WM8994_AIF1DAC1L_ENA_MASK);
	val |= (WM8994_AIF1DAC1L_ENA | WM8994_AIF1DAC1R_ENA | WM8994_DAC1L_ENA | WM8994_DAC1R_ENA);
	wm8994_write(codec,WM8994_POWER_MANAGEMENT_5,val);

	val = wm8994_read(codec,WM8994_AIF1_DAC1_FILTERS_1);
	val &= ~(WM8994_AIF1DAC1_MUTE_MASK |WM8994_AIF1DAC1_MONO_MASK);
	val |= (WM8994_AIF1DAC1_UNMUTE);
	wm8994_write(codec,WM8994_AIF1_DAC1_FILTERS_1,val);

	val = wm8994_read(codec,WM8994_LINE_OUTPUTS_VOLUME);
	val &= ~(WM8994_LINEOUT2N_MUTE_MASK | WM8994_LINEOUT2P_MUTE_MASK);
	wm8994_write(codec,WM8994_LINE_OUTPUTS_VOLUME,val);

	val = wm8994_read(codec,WM8994_DAC1_LEFT_MIXER_ROUTING);
	val &= ~(WM8994_AIF1DAC1L_TO_DAC1L_MASK);
	val |= (WM8994_AIF1DAC1L_TO_DAC1L);
	wm8994_write(codec,WM8994_DAC1_LEFT_MIXER_ROUTING,val);

	val = wm8994_read(codec,WM8994_DAC1_RIGHT_MIXER_ROUTING);
	val &= ~(WM8994_AIF1DAC1R_TO_DAC1R_MASK);
	val |= (WM8994_AIF1DAC1R_TO_DAC1R);
	wm8994_write(codec,WM8994_DAC1_RIGHT_MIXER_ROUTING,val);

	val = wm8994_read(codec, WM8994_CLOCKING_1);
	val &= ~(WM8994_DSP_FS1CLK_ENA_MASK | WM8994_DSP_FSINTCLK_ENA_MASK);
	val |= (WM8994_DSP_FS1CLK_ENA | WM8994_DSP_FSINTCLK_ENA);
	wm8994_write(codec, WM8994_CLOCKING_1, val);

	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_3);
	val &= ~(WM8994_LINEOUT2N_ENA_MASK | WM8994_LINEOUT1P_ENA_MASK | WM8994_MIXOUTLVOL_ENA_MASK | WM8994_MIXOUTRVOL_ENA_MASK | WM8994_MIXOUTL_ENA_MASK | WM8994_MIXOUTR_ENA_MASK);
	val |= (WM8994_LINEOUT2N_ENA | WM8994_LINEOUT2P_ENA | WM8994_MIXOUTL_ENA | WM8994_MIXOUTR_ENA | WM8994_MIXOUTRVOL_ENA | WM8994_MIXOUTLVOL_ENA);
	wm8994_write(codec,WM8994_POWER_MANAGEMENT_3,val);
		
	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_1 );
	val &= ~(WM8994_BIAS_ENA_MASK | WM8994_VMID_SEL_MASK | WM8994_HPOUT2_ENA_MASK );
	val |= (WM8994_BIAS_ENA | WM8994_VMID_SEL_NORMAL );
	wm8994_write(codec,WM8994_POWER_MANAGEMENT_1,val);

}

void wm8994_set_playback_hdmi_tvout(struct snd_soc_codec *codec)
{

	DEBUG_LOG("wm8994_set_playback_hdmi_tvout");
	
	wm8994_write(codec, WM8994_AIF1_BCLK, 0x70);
				
}


void wm8994_set_playback_speaker_hdmitvout(struct snd_soc_codec *codec)
{
	DEBUG_LOG("wm8994_set_playback_speaker_hdmitvout");
	
	wm8994_write(codec, WM8994_AIF1_BCLK, 0x70);
	
	wm8994_set_playback_speaker(codec);
}


void wm8994_set_playback_speakerheadset_hdmitvout(struct snd_soc_codec *codec)
{
	DEBUG_LOG("wm8994_set_playback_speakerheadset_hdmitvout");
	
	wm8994_write(codec, WM8994_AIF1_BCLK, 0x70);	

	wm8994_set_playback_speaker_headset(codec);
}


void wm8994_set_voicecall_common_setting(struct snd_soc_codec *codec)
{
	int val;

	/*GPIO Configuration*/
	wm8994_write(codec, WM8994_GPIO_1, 0xA101);
	wm8994_write(codec, WM8994_GPIO_2, 0x8100);
#if 0 	
	wm8994_write(codec, WM8994_GPIO_3, 0x0100);
	wm8994_write(codec, WM8994_GPIO_4, 0x0100);
#else
	wm8994_write(codec, WM8994_GPIO_3, 0x8100);
	wm8994_write(codec, WM8994_GPIO_4, 0x8100);
#endif		
	wm8994_write(codec, WM8994_GPIO_5, 0x8100);
	wm8994_write(codec, WM8994_GPIO_6, 0xA101);
	wm8994_write(codec, WM8994_GPIO_7, 0x0100);
	wm8994_write(codec, WM8994_GPIO_8, 0xA101);
	wm8994_write(codec, WM8994_GPIO_9, 0xA101);
	wm8994_write(codec, WM8994_GPIO_10, 0xA101);
	wm8994_write(codec, WM8994_GPIO_11, 0xA101);
#if 0
	/*FLL2 Setting*/
	wm8994_write(codec, WM8994_FLL2_CONTROL_2, 0x2F00);
	wm8994_write(codec, WM8994_FLL2_CONTROL_3, 0x3126);
	wm8994_write(codec, WM8994_FLL2_CONTROL_4, 0x0100);
	wm8994_write(codec, WM8994_FLL2_CONTROL_5, 0x0C88);
//	wm8994_write(codec, WM8994_FLL2_CONTROL_5, 0x0C89);	// CP 24MHz
	wm8994_write(codec, WM8994_FLL2_CONTROL_1, WM8994_FLL2_FRACN_ENA | WM8994_FLL2_ENA);
#else
//wm8994_write(codec	,	0x240	,	0x0004	)	;	//	FLL2	Control	(1)(240H):	FLL2_FRAC=1,	FLL2_OSC_ENA=0,	FLL2_ENA=1	jieun_jung_codec_lock_issue_100805	
wm8994_write(codec	,	0x241	,	0x0700	)	;	//	FLL2	Control	(2)(241H):	FLL2_OUTDIV=00_0111,	FLL2_CTRL_RATE=000,	FLL2_FRATIO=000		
wm8994_write(codec	,	0x242	,	0x8FD5	)	;	//	FLL2	Control	(3)(242H):	FLL2_K=1000_1111_1101_0101				
wm8994_write(codec	,	0x243	,	0x00E0	)	;	//	FLL2	Control	(4)(243H):	FLL2_N=00_0000_0111,	FLL2_LOOP_GAIN=0000			
wm8994_write(codec	,	0x244	,	0x0C89	)	;	//	FLL2	Control	(5)(244H):	FLL2_FRC_NCO_VAL=01_1001,	FLL2_FRC_NCO=0,	FLL2_REFCLK_DIV=01,	FLL2_REFCLK_SRC=01	
wm8994_write(codec	,	0x240	,	0x0005	)	;	//	FLL2	Control	(1)(240H):	FLL2_FRAC=1,	FLL2_OSC_ENA=0,	FLL2_ENA=1		
//wm8994_write(codec	,	0x208	,	0x000A	)	;	//	Clocking	(1)(208H):	TOCLK_ENA=0,	AIF1DSPCLK_ENA=1,	AIF2DSPCLK_ENA=1,	SYSDSPCLK_ENA=1,	SYSCLK_SRC=0	
wm8994_write(codec	,	WM8994_AIF2_RATE	,	0x0009	)	;	//	AIF2	Rate(211H):	AIF2_SR=0000,	AIF2CLK_RATE=1001				
wm8994_write(codec	,	WM8994_AIF2_CLOCKING_1	,	0x0018	)	;	//	AIF2	Clocking	(1)(204H):	AIF2CLK_SRC=11,	AIF2CLK_INV=0,	AIF2CLK_DIV=0,	AIF2CLK_ENA=1	
#endif

	wm8994_write(codec,WM8994_AIF2_BCLK, 0x0070 ); //hdlnc_ldj_0415_A1026
	wm8994_write(codec,WM8994_AIF2DAC_LRCLK, 0x0020 ); //hdlnc_ldj_0415_A1026
#if 0
	val = wm8994_read(codec, WM8994_AIF2_CLOCKING_1);
	if(!(val & WM8994_AIF2CLK_ENA))
		wm8994_write(codec, WM8994_AIF2_CLOCKING_1, 0x0018);

	wm8994_write(codec, WM8994_AIF2_RATE, 0x3 << WM8994_AIF2CLK_RATE_SHIFT);
#endif	
	// AIF2 Interface
//	wm8994_write(codec, WM8994_AIF2_CONTROL_1, 	//Left Justified, BCLK invert, LRCLK Invert
//		WM8994_AIF2ADCR_SRC | WM8994_AIF2_BCLK_INV |WM8994_AIF2_LRCLK_INV | 0x1 << WM8994_AIF2_FMT_SHIFT);
//	wm8994_write(codec,WM8994_AIF2_CONTROL_2, WM8994_AIF2DACR_SRC | 0x0000);  //hdlnc_ldj_0416_A1026
//		wm8994_write(codec, WM8994_AIF2_MASTER_SLAVE, WM8994_AIF2_MSTR | WM8994_AIF2_CLK_FRC | WM8994_AIF2_LRCLK_FRC);	//Master

	wm8994_write(codec, WM8994_AIF2_CONTROL_1,	0x8118);//PCM mode, BCLK invert, LRCLK Invert
	wm8994_write(codec, WM8994_AIF2_CONTROL_2, 0x0000);
	wm8994_write(codec, WM8994_AIF2_MASTER_SLAVE, 0x0000);	//slave

	wm8994_write(codec,WM8994_AIF2DAC_DATA, 0x0003 ); //hdlnc_ldj_0416_A1026

	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_5);	//05 : 3303
	val &= ~(WM8994_AIF2DACL_ENA_MASK | WM8994_AIF2DACR_ENA_MASK | WM8994_AIF1DAC1L_ENA_MASK | WM8994_AIF1DAC1R_ENA_MASK
		 | WM8994_DAC1L_ENA_MASK | WM8994_DAC1R_ENA_MASK);
	val |= (WM8994_AIF2DACL_ENA | WM8994_AIF2DACR_ENA | WM8994_AIF1DAC1L_ENA | WM8994_AIF1DAC1R_ENA
		 | WM8994_DAC1L_ENA | WM8994_DAC1R_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_5, val);

	/*Clocking*/	
	val = wm8994_read(codec, WM8994_CLOCKING_1);
	val |= (WM8994_DSP_FS2CLK_ENA | 0x01); // SYSCLK_SRC=1
	wm8994_write(codec, WM8994_CLOCKING_1, val);

	wm8994_write(codec, WM8994_POWER_MANAGEMENT_6, 0x0);
	
	// AIF1 & AIF2 Output is connected to DAC1	
	val = wm8994_read(codec,WM8994_DAC1_LEFT_MIXER_ROUTING);	
	val &= ~(WM8994_AIF1DAC1L_TO_DAC1L_MASK | WM8994_AIF2DACL_TO_DAC1L_MASK);	
	val |= (WM8994_AIF1DAC1L_TO_DAC1L | WM8994_AIF2DACL_TO_DAC1L);
	wm8994_write(codec,WM8994_DAC1_LEFT_MIXER_ROUTING, val );  

	val = wm8994_read(codec,WM8994_DAC1_RIGHT_MIXER_ROUTING);	
	val &= ~(WM8994_AIF1DAC1R_TO_DAC1R_MASK | WM8994_AIF2DACR_TO_DAC1R_MASK);	
	val |= (WM8994_AIF1DAC1R_TO_DAC1R | WM8994_AIF2DACR_TO_DAC1R);
	wm8994_write(codec,WM8994_DAC1_RIGHT_MIXER_ROUTING, val);
	
	val = wm8994_read(codec, WM8994_AIF1_DAC1_LEFT_VOLUME);
	val &= ~(WM8994_AIF1DAC1L_VOL_MASK);
	val |= (WM8994_AIF1DAC1_VU | TUNING_DAC1L_VOL);
	wm8994_write(codec, WM8994_AIF1_DAC1_LEFT_VOLUME, val);
	
	val = wm8994_read(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME);
	val &= ~(WM8994_AIF1DAC1R_VOL_MASK);
	val |= (WM8994_AIF1DAC1_VU |TUNING_DAC1R_VOL);
	wm8994_write(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME, val);

	wm8994_write(codec, 0x6, 0x0);
}

void wm8994_set_voicecall_receiver(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->private_data;

	int val;
	
	DEBUG_LOG("");
	
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, 
		WM8994_VMID_SEL_NORMAL | WM8994_BIAS_ENA);


	if(a1026_mode_status != NS0)
	{	
		A1026SetFeature(NS0); // set the configure for A1026 to NS0 mode	//ldj_0402	
		a1026_mode_status = NS0;
	}
		
	audio_ctrl_mic_bias_gpio(1);

	wm8994_set_voicecall_common_setting(codec);

	wm8994_write(codec, WM8994_CHARGE_PUMP_1, WM8994_CP_ENA_DEFAULT);	// Turn off charge pump.
	
	// Analogue Input Configuration -Main MIC
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_2, 
		WM8994_TSHUT_ENA | WM8994_TSHUT_OPDIS | WM8994_MIXINL_ENA |WM8994_MIXINR_ENA | WM8994_IN1L_ENA | WM8994_IN2R_ENA);  //hdlnc_ldj_0415_A1026
	
	wm8994_write(codec, WM8994_INPUT_MIXER_2, 
	   WM8994_IN1LP_TO_IN1L | WM8994_IN1LN_TO_IN1L |WM8994_IN2RP_TO_IN2R | WM8994_IN2RN_TO_IN2R); 	// differential(3) or single ended(1) //hdlnc_ldj_0415_A1026
		
	/* Digital Path Enables and Unmutes*/	
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_4, 
	  WM8994_AIF2ADCL_ENA | WM8994_ADCL_ENA | WM8994_AIF2ADCR_ENA | WM8994_ADCR_ENA);	//hdlnc_ldj_0415_A1026
	
	wm8994_write(codec,WM8994_AIF1_DAC1_FILTERS_1, 0x0000 );  	//AIF1DAC1 Unmute, Mono Mix diable, Fast Ramp
	wm8994_write(codec, WM8994_AIF2_DAC_FILTERS_1, 0x0000 );	//AIF2DAC Unmute, Mono Mix diable, Fast Ramp

	wm8994_write(codec, WM8994_DAC2_MIXER_VOLUMES, 0x018C);	  //hdlnc_ldj_0415_A1026
	wm8994_write(codec, WM8994_DAC2_LEFT_VOLUME, 0x01C0 );	
	wm8994_write(codec, WM8994_DAC2_RIGHT_VOLUME, 0x01C0 );	
//[ hdlnc_bp_ytkwon : 20100305
//	wm8994_write(codec,WM8994_SIDETONE,0x0240);
	wm8994_write(codec,WM8994_SIDETONE,0x01C0);
//] hdlnc_bp_ytkwon : 20100305

	// Tx -> AIF2 Path
	wm8994_write(codec, WM8994_DAC2_LEFT_MIXER_ROUTING, WM8994_ADC2_TO_DAC2L);	 //hdlnc_ldj_0415_A1026
	wm8994_write(codec, WM8994_DAC2_RIGHT_MIXER_ROUTING, WM8994_ADC1_TO_DAC2R);

	wm8994_write(codec, WM8994_AIF2_CLOCKING_1, 0x0019);

	if(wm8994->testmode_config_flag == SEC_NORMAL)
	{	
		// Unmute IN1L PGA, update volume
		val = wm8994_read(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME);	
		val &= ~(WM8994_IN1L_MUTE_MASK | WM8994_IN1L_VOL_MASK);
		val |= (WM8994_IN1L_VU |TUNING_CALL_RCV_INPUTMIX_VOL); //volume
		wm8994_write(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME, val);
		val = wm8994_read(codec, WM8994_RIGHT_LINE_INPUT_3_4_VOLUME);	
		val &= ~(WM8994_IN2R_MUTE_MASK | WM8994_IN2R_VOL_MASK);
		val |= (WM8994_IN2R_VU |TUNING_CALL_RCV_INPUTMIX_SUB_VOL); //volume
		wm8994_write(codec, WM8994_RIGHT_LINE_INPUT_3_4_VOLUME, val);  //hdlnc_ldj_0415_A1026
	
		//Unmute the PGA
		val = wm8994_read(codec, WM8994_INPUT_MIXER_3);
		val&= ~(WM8994_IN1L_TO_MIXINL_MASK | WM8994_IN1L_MIXINL_VOL_MASK | WM8994_MIXOUTL_MIXINL_VOL_MASK);
		val |= (WM8994_IN1L_TO_MIXINL |TUNING_CALL_RCV_MIXER_VOL);//0db
		wm8994_write(codec, WM8994_INPUT_MIXER_3, val); 
	
		val = wm8994_read(codec, WM8994_INPUT_MIXER_4);
		val&= ~(WM8994_IN2R_TO_MIXINR_MASK | WM8994_IN2R_MIXINR_VOL_MASK | WM8994_MIXOUTR_MIXINR_VOL_MASK);
		val |= (WM8994_IN2R_TO_MIXINR |TUNING_CALL_RCV_MIXER_SUB_VOL);//0db
		wm8994_write(codec, WM8994_INPUT_MIXER_4, val);   //hdlnc_ldj_0415_A1026
		// Volume Control - Output
		val = wm8994_read(codec, WM8994_OUTPUT_MIXER_5);
		val &= ~(WM8994_DACL_MIXOUTL_VOL_MASK);
		val |= TUNING_RCV_OUTMIX5_VOL << WM8994_DACL_MIXOUTL_VOL_SHIFT;
		wm8994_write(codec,WM8994_OUTPUT_MIXER_5, val );
		
		val = wm8994_read(codec, WM8994_OUTPUT_MIXER_6);
		val &= ~(WM8994_DACR_MIXOUTR_VOL_MASK);
		val |= TUNING_RCV_OUTMIX6_VOL << WM8994_DACR_MIXOUTR_VOL_SHIFT;
		wm8994_write(codec,WM8994_OUTPUT_MIXER_6, val );
	
		val = wm8994_read(codec, WM8994_LEFT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTL_MUTE_N_MASK | WM8994_MIXOUTL_VOL_MASK);
		val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTL_MUTE_N | TUNING_RCV_OPGAL_VOL);
		wm8994_write(codec,WM8994_LEFT_OPGA_VOLUME, val );
	
		val = wm8994_read(codec, WM8994_RIGHT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTR_MUTE_N_MASK | WM8994_MIXOUTR_VOL_MASK);
		val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTR_MUTE_N | TUNING_RCV_OPGAR_VOL);
		wm8994_write(codec,WM8994_RIGHT_OPGA_VOLUME, val );
		
		val = wm8994_read(codec, WM8994_HPOUT2_VOLUME);
		val &= ~(WM8994_HPOUT2_MUTE_MASK | WM8994_HPOUT2_VOL_MASK);
		val |= TUNING_HPOUT2_VOL << WM8994_HPOUT2_VOL_SHIFT;
		wm8994_write(codec,WM8994_HPOUT2_VOLUME, val );
	
		//Unmute DAC1 left
		val = wm8994_read(codec,WM8994_DAC1_LEFT_VOLUME );
		val &= ~(WM8994_DAC1L_MUTE_MASK | WM8994_DAC1L_VOL_MASK);
		val |= TUNING_DAC1L_VOL; 
		wm8994_write(codec,WM8994_DAC1_LEFT_VOLUME ,val);
		
		//Unmute and volume ctrl RightDAC
		val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME ); 
		val &= ~(WM8994_DAC1R_MUTE_MASK | WM8994_DAC1R_VOL_MASK);
		val |= TUNING_DAC1R_VOL; //0 db volume	
		wm8994_write(codec,WM8994_DAC1_RIGHT_VOLUME,val);
	}

	if(wm8994->testmode_config_flag == SEC_TEST_PBA_LOOPBACK)
	{	
		// Unmute IN1L PGA, update volume
		val = wm8994_read(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME);	
		val &= ~(WM8994_IN1L_MUTE_MASK | WM8994_IN1L_VOL_MASK);
		val |= (WM8994_IN1L_VU |TUNING_CALL_RCV_LOOP_INPUTMIX_VOL); //volume
		wm8994_write(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME, val);
		val = wm8994_read(codec, WM8994_RIGHT_LINE_INPUT_3_4_VOLUME);	
		val &= ~(WM8994_IN2R_MUTE_MASK | WM8994_IN2R_VOL_MASK);
		val |= (WM8994_IN2R_VU |TUNING_CALL_RCV_LOOP_INPUTMIX_SUB_VOL); //volume
		wm8994_write(codec, WM8994_RIGHT_LINE_INPUT_3_4_VOLUME, val);  //hdlnc_ldj_0415_A1026
	
		//Unmute the PGA
		val = wm8994_read(codec, WM8994_INPUT_MIXER_3);
		val&= ~(WM8994_IN1L_TO_MIXINL_MASK | WM8994_IN1L_MIXINL_VOL_MASK | WM8994_MIXOUTL_MIXINL_VOL_MASK);
		val |= (WM8994_IN1L_TO_MIXINL |TUNING_CALL_RCV_LOOP_MIXER_VOL);//0db
		wm8994_write(codec, WM8994_INPUT_MIXER_3, val); 
	
		val = wm8994_read(codec, WM8994_INPUT_MIXER_4);
		val&= ~(WM8994_IN2R_TO_MIXINR_MASK | WM8994_IN2R_MIXINR_VOL_MASK | WM8994_MIXOUTR_MIXINR_VOL_MASK);
		val |= (WM8994_IN2R_TO_MIXINR |TUNING_CALL_RCV_LOOP_MIXER_SUB_VOL);//0db
		wm8994_write(codec, WM8994_INPUT_MIXER_4, val);   //hdlnc_ldj_0415_A1026
		// Volume Control - Output
		val = wm8994_read(codec, WM8994_OUTPUT_MIXER_5);
		val &= ~(WM8994_DACL_MIXOUTL_VOL_MASK);
		val |= TUNING_RCV_LOOP_OUTMIX5_VOL << WM8994_DACL_MIXOUTL_VOL_SHIFT;
		wm8994_write(codec,WM8994_OUTPUT_MIXER_5, val );
		
		val = wm8994_read(codec, WM8994_OUTPUT_MIXER_6);
		val &= ~(WM8994_DACR_MIXOUTR_VOL_MASK);
		val |= TUNING_RCV_LOOP_OUTMIX6_VOL << WM8994_DACR_MIXOUTR_VOL_SHIFT;
		wm8994_write(codec,WM8994_OUTPUT_MIXER_6, val );
	
		val = wm8994_read(codec, WM8994_LEFT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTL_MUTE_N_MASK | WM8994_MIXOUTL_VOL_MASK);
		val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTL_MUTE_N | TUNING_RCV_LOOP_OPGAL_VOL);
		wm8994_write(codec,WM8994_LEFT_OPGA_VOLUME, val );
	
		val = wm8994_read(codec, WM8994_RIGHT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTR_MUTE_N_MASK | WM8994_MIXOUTR_VOL_MASK);
		val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTR_MUTE_N | TUNING_RCV_LOOP_OPGAR_VOL);
		wm8994_write(codec,WM8994_RIGHT_OPGA_VOLUME, val );
		
		val = wm8994_read(codec, WM8994_HPOUT2_VOLUME);
		val &= ~(WM8994_HPOUT2_MUTE_MASK | WM8994_HPOUT2_VOL_MASK);
		val |= TUNING_HPOUT2_LOOP_VOL << WM8994_HPOUT2_VOL_SHIFT;
		wm8994_write(codec,WM8994_HPOUT2_VOLUME, val );
	
		//Unmute DAC1 left
		val = wm8994_read(codec,WM8994_DAC1_LEFT_VOLUME );
		val &= ~(WM8994_DAC1L_MUTE_MASK | WM8994_DAC1L_VOL_MASK);
		val |= TUNING_DAC1L_VOL; 
		wm8994_write(codec,WM8994_DAC1_LEFT_VOLUME ,val);
		
		//Unmute and volume ctrl RightDAC
		val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME ); 
		val &= ~(WM8994_DAC1R_MUTE_MASK | WM8994_DAC1R_VOL_MASK);
		val |= TUNING_DAC1R_VOL; //0 db volume	
		wm8994_write(codec,WM8994_DAC1_RIGHT_VOLUME,val);
	}

	// Output Mixing
	wm8994_write(codec, WM8994_OUTPUT_MIXER_1, WM8994_DAC1L_TO_MIXOUTL);
	wm8994_write(codec, WM8994_OUTPUT_MIXER_2, WM8994_DAC1R_TO_MIXOUTR );

	// Analogue Output Configuration
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, 
		WM8994_MIXOUTLVOL_ENA | WM8994_MIXOUTRVOL_ENA |WM8994_MIXOUTL_ENA | WM8994_MIXOUTR_ENA);
	wm8994_write(codec, WM8994_HPOUT2_MIXER, WM8994_MIXOUTLVOL_TO_HPOUT2 |WM8994_MIXOUTRVOL_TO_HPOUT2); 
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, 
		WM8994_HPOUT2_ENA | WM8994_VMID_SEL_NORMAL | WM8994_BIAS_ENA);

	//	wm8994_write(codec, WM8994_INPUT_MIXER_4, 0x0000 ); 
	 wm8994_call_recording_change_path(codec);
}

void wm8994_set_voicecall_receiver_audience(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->private_data;

	int val;
	
	DEBUG_LOG("wm8994_set_voicecall_receiver_audience");

	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, 
		WM8994_VMID_SEL_NORMAL | WM8994_BIAS_ENA);

	if(a1026_mode_status != CLOSETALK) //fix for audio broken issue(close talk parameter is not applied when issue is occured)
	{	
		A1026SetFeature(CLOSETALK); // set the configure for A1026 to NS0 mode	//ldj_0402	
		a1026_mode_status = CLOSETALK;
	}
		
	audio_ctrl_mic_bias_gpio(1);

	wm8994_set_voicecall_common_setting(codec);

	wm8994_write(codec, WM8994_CHARGE_PUMP_1, WM8994_CP_ENA_DEFAULT);	// Turn off charge pump.
	
	// Analogue Input Configuration -Main MIC
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_2, 
		WM8994_TSHUT_ENA | WM8994_TSHUT_OPDIS | WM8994_MIXINL_ENA |WM8994_MIXINR_ENA | WM8994_IN1L_ENA | WM8994_IN2R_ENA);  //hdlnc_ldj_0415_A1026
	
	wm8994_write(codec, WM8994_INPUT_MIXER_2, 
	   WM8994_IN1LP_TO_IN1L | WM8994_IN1LN_TO_IN1L | WM8994_IN2RP_TO_IN2R | WM8994_IN2RN_TO_IN2R); 	// differential(3) or single ended(1) //hdlnc_ldj_0415_A1026
		
	/* Digital Path Enables and Unmutes*/	
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_4, 
	  WM8994_AIF2ADCL_ENA | WM8994_ADCL_ENA | WM8994_AIF2ADCR_ENA | WM8994_ADCR_ENA);	//hdlnc_ldj_0415_A1026
	
	wm8994_write(codec,WM8994_AIF1_DAC1_FILTERS_1, 0x0000 );  	//AIF1DAC1 Unmute, Mono Mix diable, Fast Ramp
	wm8994_write(codec, WM8994_AIF2_DAC_FILTERS_1, 0x0000 );	//AIF2DAC Unmute, Mono Mix diable, Fast Ramp

	wm8994_write(codec, WM8994_DAC2_MIXER_VOLUMES, 0x018C);	  //hdlnc_ldj_0415_A1026
	wm8994_write(codec, WM8994_DAC2_LEFT_VOLUME, 0x01C0 );	
	wm8994_write(codec, WM8994_DAC2_RIGHT_VOLUME, 0x01C0 );	
//[ hdlnc_bp_ytkwon : 20100305
//	wm8994_write(codec,WM8994_SIDETONE,0x0240); //180Hz
	wm8994_write(codec,WM8994_SIDETONE,0x01C0);
//] hdlnc_bp_ytkwon : 20100305


	// Tx -> AIF2 Path
	wm8994_write(codec, WM8994_DAC2_LEFT_MIXER_ROUTING, WM8994_ADC2_TO_DAC2L);	 //hdlnc_ldj_0415_A1026
	wm8994_write(codec, WM8994_DAC2_RIGHT_MIXER_ROUTING, WM8994_ADC1_TO_DAC2R);

	wm8994_write(codec, WM8994_AIF2_CLOCKING_1, 0x0019);
	if(wm8994->testmode_config_flag == SEC_NORMAL)
	{	
		// Unmute IN1L PGA, update volume
		val = wm8994_read(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME);	
		val &= ~(WM8994_IN1L_MUTE_MASK | WM8994_IN1L_VOL_MASK);
		if(HWREV>=0x0B)
			val |= (WM8994_IN1L_VU |TUNING_CALL_AUDIENCE_RCV_INPUTMIX_VOL_REV); //volume
		else
			val |= (WM8994_IN1L_VU |TUNING_CALL_AUDIENCE_RCV_INPUTMIX_VOL); //volume	
		wm8994_write(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME, val);
		
		val = wm8994_read(codec, WM8994_RIGHT_LINE_INPUT_3_4_VOLUME);	
		val &= ~(WM8994_IN2R_MUTE_MASK | WM8994_IN2R_VOL_MASK);
		if(HWREV>=0x0B)
			val |= (WM8994_IN2R_VU |TUNING_CALL_AUDIENCE_RCV_INPUTMIX_SUB_VOL_REV); //volume
		else
			val |= (WM8994_IN2R_VU |TUNING_CALL_AUDIENCE_RCV_INPUTMIX_SUB_VOL); //volume	
		wm8994_write(codec, WM8994_RIGHT_LINE_INPUT_3_4_VOLUME, val);  //hdlnc_ldj_0415_A1026
	
		//Unmute the PGA
		val = wm8994_read(codec, WM8994_INPUT_MIXER_3);
		val&= ~(WM8994_IN1L_TO_MIXINL_MASK | WM8994_IN1L_MIXINL_VOL_MASK | WM8994_MIXOUTL_MIXINL_VOL_MASK);
		val |= (WM8994_IN1L_TO_MIXINL |TUNING_CALL_AUDIENCE_RCV_MIXER_VOL);//0db
		wm8994_write(codec, WM8994_INPUT_MIXER_3, val); 
	
		val = wm8994_read(codec, WM8994_INPUT_MIXER_4);
		val&= ~(WM8994_IN2R_TO_MIXINR_MASK | WM8994_IN2R_MIXINR_VOL_MASK | WM8994_MIXOUTR_MIXINR_VOL_MASK);
		val |= (WM8994_IN2R_TO_MIXINR |TUNING_CALL_AUDIENCE_RCV_MIXER_SUB_VOL);//0db
		wm8994_write(codec, WM8994_INPUT_MIXER_4, val);   //hdlnc_ldj_0415_A1026
		// Volume Control - Output
		val = wm8994_read(codec, WM8994_OUTPUT_MIXER_5);
		val &= ~(WM8994_DACL_MIXOUTL_VOL_MASK);
		val |= TUNING_AUDIENCE_RCV_OUTMIX5_VOL << WM8994_DACL_MIXOUTL_VOL_SHIFT;
		wm8994_write(codec,WM8994_OUTPUT_MIXER_5, val );
		
		val = wm8994_read(codec, WM8994_OUTPUT_MIXER_6);
		val &= ~(WM8994_DACR_MIXOUTR_VOL_MASK);
		val |= TUNING_AUDIENCE_RCV_OUTMIX6_VOL << WM8994_DACR_MIXOUTR_VOL_SHIFT;
		wm8994_write(codec,WM8994_OUTPUT_MIXER_6, val );
	
		val = wm8994_read(codec, WM8994_LEFT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTL_MUTE_N_MASK | WM8994_MIXOUTL_VOL_MASK);
		if(HWREV>=0x0B)
			val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTL_MUTE_N | TUNING_AUDIENCE_RCV_OPGAL_VOL_REV);
		else
			val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTL_MUTE_N | TUNING_AUDIENCE_RCV_OPGAL_VOL);
		wm8994_write(codec,WM8994_LEFT_OPGA_VOLUME, val );
	
		val = wm8994_read(codec, WM8994_RIGHT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTR_MUTE_N_MASK | WM8994_MIXOUTR_VOL_MASK);
		if(HWREV>=0x0B)
			val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTR_MUTE_N | TUNING_AUDIENCE_RCV_OPGAR_VOL_REV);
		else
			val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTR_MUTE_N | TUNING_AUDIENCE_RCV_OPGAR_VOL);	
		wm8994_write(codec,WM8994_RIGHT_OPGA_VOLUME, val );
		
		val = wm8994_read(codec, WM8994_HPOUT2_VOLUME);
		val &= ~(WM8994_HPOUT2_MUTE_MASK | WM8994_HPOUT2_VOL_MASK);
		val |= TUNING_AUDIENCE_HPOUT2_VOL << WM8994_HPOUT2_VOL_SHIFT;
		wm8994_write(codec,WM8994_HPOUT2_VOLUME, val );
	
		//Unmute DAC1 left
		val = wm8994_read(codec,WM8994_DAC1_LEFT_VOLUME );
		val &= ~(WM8994_DAC1L_MUTE_MASK | WM8994_DAC1L_VOL_MASK);
		val |= TUNING_DAC1L_VOL; 
		wm8994_write(codec,WM8994_DAC1_LEFT_VOLUME ,val);
		
		//Unmute and volume ctrl RightDAC
		val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME ); 
		val &= ~(WM8994_DAC1R_MUTE_MASK | WM8994_DAC1R_VOL_MASK);
		val |= TUNING_DAC1R_VOL; //0 db volume	
		wm8994_write(codec,WM8994_DAC1_RIGHT_VOLUME,val);
	}

	if(wm8994->testmode_config_flag == SEC_TEST_PBA_LOOPBACK)
	{	
		// Unmute IN1L PGA, update volume
		val = wm8994_read(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME);	
		val &= ~(WM8994_IN1L_MUTE_MASK | WM8994_IN1L_VOL_MASK);
		if(HWREV>=0x0B)
			val |= (WM8994_IN1L_VU |TUNING_CALL_AUDIENCE_RCV_LOOP_INPUTMIX_VOL_REV); //volume
		else
			val |= (WM8994_IN1L_VU |TUNING_CALL_AUDIENCE_RCV_LOOP_INPUTMIX_VOL); //volume	
		wm8994_write(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME, val);
		
		val = wm8994_read(codec, WM8994_RIGHT_LINE_INPUT_3_4_VOLUME);	
		val &= ~(WM8994_IN2R_MUTE_MASK | WM8994_IN2R_VOL_MASK);
		if(HWREV>=0x0B)
			val |= (WM8994_IN2R_VU |TUNING_CALL_AUDIENCE_RCV_LOOP_INPUTMIX_SUB_VOL_REV); //volume
		else
			val |= (WM8994_IN2R_VU |TUNING_CALL_AUDIENCE_RCV_LOOP_INPUTMIX_SUB_VOL); //volume	
		wm8994_write(codec, WM8994_RIGHT_LINE_INPUT_3_4_VOLUME, val);  //hdlnc_ldj_0415_A1026
	
		//Unmute the PGA
		val = wm8994_read(codec, WM8994_INPUT_MIXER_3);
		val&= ~(WM8994_IN1L_TO_MIXINL_MASK | WM8994_IN1L_MIXINL_VOL_MASK | WM8994_MIXOUTL_MIXINL_VOL_MASK);
		val |= (WM8994_IN1L_TO_MIXINL |TUNING_CALL_AUDIENCE_RCV_LOOP_MIXER_VOL);//0db
		wm8994_write(codec, WM8994_INPUT_MIXER_3, val); 
	
		val = wm8994_read(codec, WM8994_INPUT_MIXER_4);
		val&= ~(WM8994_IN2R_TO_MIXINR_MASK | WM8994_IN2R_MIXINR_VOL_MASK | WM8994_MIXOUTR_MIXINR_VOL_MASK);
		val |= (WM8994_IN2R_TO_MIXINR |TUNING_CALL_AUDIENCE_RCV_LOOP_MIXER_SUB_VOL);//0db
		wm8994_write(codec, WM8994_INPUT_MIXER_4, val);   //hdlnc_ldj_0415_A1026
		// Volume Control - Output
		val = wm8994_read(codec, WM8994_OUTPUT_MIXER_5);
		val &= ~(WM8994_DACL_MIXOUTL_VOL_MASK);
		val |= TUNING_AUDIENCE_RCV_LOOP_OUTMIX5_VOL << WM8994_DACL_MIXOUTL_VOL_SHIFT;
		wm8994_write(codec,WM8994_OUTPUT_MIXER_5, val );
		
		val = wm8994_read(codec, WM8994_OUTPUT_MIXER_6);
		val &= ~(WM8994_DACR_MIXOUTR_VOL_MASK);
		val |= TUNING_AUDIENCE_RCV_LOOP_OUTMIX6_VOL << WM8994_DACR_MIXOUTR_VOL_SHIFT;
		wm8994_write(codec,WM8994_OUTPUT_MIXER_6, val );
	
		val = wm8994_read(codec, WM8994_LEFT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTL_MUTE_N_MASK | WM8994_MIXOUTL_VOL_MASK);
		if(HWREV>=0x0B)
			val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTL_MUTE_N | TUNING_AUDIENCE_RCV_LOOP_OPGAL_VOL_REV);
		else
			val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTL_MUTE_N | TUNING_AUDIENCE_RCV_LOOP_OPGAL_VOL);
		wm8994_write(codec,WM8994_LEFT_OPGA_VOLUME, val );
	
		val = wm8994_read(codec, WM8994_RIGHT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTR_MUTE_N_MASK | WM8994_MIXOUTR_VOL_MASK);
		if(HWREV>=0x0B)
			val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTR_MUTE_N | TUNING_AUDIENCE_RCV_LOOP_OPGAR_VOL_REV);
		else
			val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTR_MUTE_N | TUNING_AUDIENCE_RCV_LOOP_OPGAR_VOL);	
		wm8994_write(codec,WM8994_RIGHT_OPGA_VOLUME, val );
		
		val = wm8994_read(codec, WM8994_HPOUT2_VOLUME);
		val &= ~(WM8994_HPOUT2_MUTE_MASK | WM8994_HPOUT2_VOL_MASK);
		val |= TUNING_AUDIENCE_LOOP_HPOUT2_VOL << WM8994_HPOUT2_VOL_SHIFT;
		wm8994_write(codec,WM8994_HPOUT2_VOLUME, val );
	
		//Unmute DAC1 left
		val = wm8994_read(codec,WM8994_DAC1_LEFT_VOLUME );
		val &= ~(WM8994_DAC1L_MUTE_MASK | WM8994_DAC1L_VOL_MASK);
		val |= TUNING_DAC1L_VOL; 
		wm8994_write(codec,WM8994_DAC1_LEFT_VOLUME ,val);
		
		//Unmute and volume ctrl RightDAC
		val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME ); 
		val &= ~(WM8994_DAC1R_MUTE_MASK | WM8994_DAC1R_VOL_MASK);
		val |= TUNING_DAC1R_VOL; //0 db volume	
		wm8994_write(codec,WM8994_DAC1_RIGHT_VOLUME,val);
	}

	// Output Mixing
	wm8994_write(codec, WM8994_OUTPUT_MIXER_1, WM8994_DAC1L_TO_MIXOUTL);
	wm8994_write(codec, WM8994_OUTPUT_MIXER_2, WM8994_DAC1R_TO_MIXOUTR );

	// Analogue Output Configuration
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, 
		WM8994_MIXOUTLVOL_ENA | WM8994_MIXOUTRVOL_ENA |WM8994_MIXOUTL_ENA | WM8994_MIXOUTR_ENA);
	wm8994_write(codec, WM8994_HPOUT2_MIXER, WM8994_MIXOUTLVOL_TO_HPOUT2 |WM8994_MIXOUTRVOL_TO_HPOUT2); 
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, 
		WM8994_HPOUT2_ENA | WM8994_VMID_SEL_NORMAL | WM8994_BIAS_ENA);

	//	wm8994_write(codec, WM8994_INPUT_MIXER_4, 0x0000 ); 
	wm8994_call_recording_change_path(codec);
}


void wm8994_set_voicecall_headset(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->private_data;

	int val;

	u16 TestReturn1=0;
	u16 TestReturn2=0;
	u16 TestLow1=0;
	u16 TestHigh1=0;
	u8 TestLow=0;
	u8 TestHigh=0;

	DEBUG_LOG("");

	if((HWREV == 0x04)||(HWREV == 0x05) || (HWREV == 0x06))
	{
		if(a1026_mode_status != BYPASSMODE)
		{	
			A1026SetFeature(BYPASSMODE); // set the configure for A1026 to bypass	//hdlnc_ldj_0416_A1026
			a1026_mode_status = BYPASSMODE;
		}
	}
	else
	{
		if(a1026_mode_status != BYPASSMODE)
		{	
			A1026SetFeature(BYPASSMODE); // set the configure for A1026 to bypass	//hdlnc_ldj_0416_A1026
			a1026_mode_status = BYPASSMODE;
		}
	}
	audio_ctrl_mic_bias_gpio(1);
	
	wm8994_set_voicecall_common_setting(codec);
	
	/*Digital Path Enables and Unmutes*/	
	val = wm8994_read(codec,WM8994_DAC1_LEFT_MIXER_ROUTING);	
	val &= ~(WM8994_AIF1DAC1L_TO_DAC1L_MASK | WM8994_AIF2DACL_TO_DAC1L_MASK);	
	val |= (WM8994_AIF1DAC1L_TO_DAC1L | WM8994_AIF2DACL_TO_DAC1L);
	wm8994_write(codec,WM8994_DAC1_LEFT_MIXER_ROUTING, 0x0005 );  //hdlnc_ldj_0416_A1026
	val = wm8994_read(codec,WM8994_DAC1_RIGHT_MIXER_ROUTING);	
	val &= ~(WM8994_AIF1DAC1R_TO_DAC1R_MASK | WM8994_AIF2DACR_TO_DAC1R_MASK);	
	val |= (WM8994_AIF1DAC1R_TO_DAC1R | WM8994_AIF2DACR_TO_DAC1R);
	wm8994_write(codec,WM8994_DAC1_RIGHT_MIXER_ROUTING, 0x0005 );

	if(wm8994->hw_version == 3)	// H/W Rev D
	{

		wm8994_write(codec, WM8994_DAC2_LEFT_MIXER_ROUTING, WM8994_ADC2_TO_DAC2L | WM8994_ADC1_TO_DAC2L);  //hdlnc_ldj_0416_A1026	
		wm8994_write(codec,WM8994_DAC2_MIXER_VOLUMES, 0x018C );    //hdlnc_ldj_0416_A1026
		wm8994_write(codec,WM8994_SIDETONE, 0x01C0);
	}
	else		// H/W Rev B
	{
		wm8994_write(codec,WM8994_DAC2_MIXER_VOLUMES, 0x000C );  
		wm8994_write(codec, WM8994_DAC2_LEFT_MIXER_ROUTING, WM8994_ADC2_TO_DAC2L);  //hdlnc_ldj_0416_A1026
		wm8994_write(codec,WM8994_SIDETONE, 0x01C1 );
	}
	/*Analogue Input Configuration*/
	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_2);	
	val &= ~(WM8994_TSHUT_ENA_MASK|WM8994_TSHUT_OPDIS_MASK|WM8994_MIXINR_ENA_MASK|WM8994_IN1R_ENA_MASK);	
	val |= (WM8994_TSHUT_ENA|WM8994_TSHUT_OPDIS|WM8994_MIXINR_ENA|WM8994_IN1R_ENA);
	wm8994_write(codec,WM8994_POWER_MANAGEMENT_2, 0x6110 );

	if((wm8994->testmode_config_flag == SEC_NORMAL)
		|| (wm8994->testmode_config_flag == SEC_TEST_PBA_LOOPBACK)
	)
	{
		val = wm8994_read(codec,WM8994_RIGHT_LINE_INPUT_1_2_VOLUME);	
		val &= ~(WM8994_IN1R_MUTE_MASK | WM8994_IN1R_VOL_MASK);	// Unmute IN1R
		val |= (WM8994_IN1R_VU | TUNING_CALL_EAR_INPUTMIX_VOL);
		wm8994_write(codec,WM8994_RIGHT_LINE_INPUT_1_2_VOLUME,val);
	
		// unmute right pga, set volume 
		val = wm8994_read(codec,WM8994_INPUT_MIXER_4 );
		val&= ~(WM8994_IN1R_TO_MIXINR_MASK | WM8994_IN1R_MIXINR_VOL_MASK | WM8994_MIXOUTR_MIXINR_VOL_MASK);
		val |= (WM8994_IN1R_TO_MIXINR);//0db
		wm8994_write(codec,WM8994_INPUT_MIXER_4 ,val);
	}	

	val = wm8994_read(codec,WM8994_INPUT_MIXER_2 );
	val&= ~(WM8994_IN1RP_TO_IN1R_MASK |  WM8994_IN1RN_TO_IN1R_MASK);
	val |= (WM8994_IN1RP_TO_IN1R|WM8994_IN1RN_TO_IN1R);//0db
	wm8994_write(codec,WM8994_INPUT_MIXER_2, 0x0003 );	 

	wm8994_write(codec,WM8994_AIF2_CONTROL_1, 	//PCM mode,  BCLK invert, LRCLK Invert  //hdlnc_ldj_0416_A1026
		0x4118);
//	wm8994_write(codec, WM8994_AIF2_DAC_FILTERS_1, 0x0080 );	
	if((wm8994->testmode_config_flag == SEC_NORMAL)
		|| (wm8994->testmode_config_flag == SEC_TEST_PBA_LOOPBACK)
	)
	{
		val = wm8994_read(codec, WM8994_LEFT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTL_MUTE_N_MASK | WM8994_MIXOUTL_VOL_MASK);
		val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTL_MUTE_N | TUNING_CALL_OPGAL_VOL);
		wm8994_write(codec,WM8994_LEFT_OPGA_VOLUME, val );
	
		val = wm8994_read(codec, WM8994_RIGHT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTR_MUTE_N_MASK | WM8994_MIXOUTR_VOL_MASK);
		val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTR_MUTE_N | TUNING_CALL_OPGAR_VOL);
		wm8994_write(codec,WM8994_RIGHT_OPGA_VOLUME, val );
	}
	// Tx -> AIF2 Path
	wm8994_write(codec, WM8994_DAC2_RIGHT_MIXER_ROUTING, WM8994_ADC1_TO_DAC2R);  //hdlnc_ldj_0416_A1026
	wm8994_write(codec,WM8994_POWER_MANAGEMENT_4,   //hdlnc_ldj_0416_A1026
		WM8994_AIF2ADCL_ENA | WM8994_AIF2ADCR_ENA | WM8994_ADCL_ENA | WM8994_ADCR_ENA);
//	wm8994_write(codec,WM8994_POWER_MANAGEMENT_6, 0x0000 );		
	val = wm8994_read(codec, 0x102  ); 	
	val &= ~(0x0003);
	val = 0x0003;
	wm8994_write(codec,0x102,val);
	
	val = wm8994_read(codec, 0x56  ); 	
	val &= ~(0x0003);
	val = 0x0003;
	wm8994_write(codec,0x56,val);
	
	val = wm8994_read(codec, 0x102  ); 	
	val &= ~(0x0000);
	val = 0x0000;
	wm8994_write(codec,0x102,val);

	val = wm8994_read(codec, WM8994_CLASS_W_1  ); 	
	val &= ~(0x0005);
	val |= 0x0005;
	wm8994_write(codec,WM8994_CLASS_W_1,val);

	if(wm8994->testmode_config_flag == SEC_NORMAL)
	{
		val = wm8994_read(codec,WM8994_LEFT_OUTPUT_VOLUME);
		val &= ~(WM8994_HPOUT1L_MUTE_N_MASK | WM8994_HPOUT1L_VOL_MASK);
		val |= (WM8994_HPOUT1_VU | WM8994_HPOUT1L_MUTE_N | TUNING_CALL_OUTPUTL_VOL);
		wm8994_write(codec, WM8994_LEFT_OUTPUT_VOLUME, val);

		val = wm8994_read(codec,WM8994_RIGHT_OUTPUT_VOLUME);
		val &= ~(WM8994_HPOUT1R_MUTE_N_MASK | WM8994_HPOUT1R_VOL_MASK);
		val |= (WM8994_HPOUT1_VU | WM8994_HPOUT1R_MUTE_N | TUNING_CALL_OUTPUTR_VOL);
		wm8994_write(codec, WM8994_RIGHT_OUTPUT_VOLUME, val);
	}

	val = wm8994_read(codec, WM8994_DC_SERVO_2  ); 	
	val &= ~(0x03E0);
	val = 0x03E0;
	wm8994_write(codec,WM8994_DC_SERVO_2,val);
	
	wm8994_write(codec,WM8994_POWER_MANAGEMENT_1, 0x0303 );  
		
	wm8994_write(codec,WM8994_ANALOGUE_HP_1, 0x0022 );  
	wm8994_write(codec,WM8994_CHARGE_PUMP_1, 0x9F25 );  

	msleep(5);

	/*Analogue Output Configuration*/	
	wm8994_write(codec,WM8994_OUTPUT_MIXER_1, 0x0001 );   
	wm8994_write(codec,WM8994_OUTPUT_MIXER_2, 0x0001 );   

	wm8994_write(codec,WM8994_POWER_MANAGEMENT_3, 0x0030 );   
	wm8994_write(codec, WM8994_AIF2_CLOCKING_1, 0x0019);
	wm8994_write(codec,WM8994_DC_SERVO_1, 0x303);  

	msleep(160);	// 160ms delay

	TestReturn1=wm8994_read(codec,WM8994_DC_SERVO_4);
	
	TestLow=(signed char)(TestReturn1 & 0xff);
	TestHigh=(signed char)((TestReturn1>>8) & 0xff);

	TestLow1=((signed short)TestLow-5)&0x00ff;
	TestHigh1=(((signed short)(TestHigh-5)<<8)&0xff00);
	TestReturn2=TestLow1|TestHigh1;
	wm8994_write(codec,WM8994_DC_SERVO_4, TestReturn2);

	val = wm8994_read(codec, WM8994_DC_SERVO_1  ); 	
	val &= ~(0x000F);
	val = 0x000F;
	wm8994_write(codec,WM8994_DC_SERVO_1,val);
	
	msleep(15);
	
	wm8994_write(codec,WM8994_ANALOGUE_HP_1, 0x00EE );  

	//Unmute DAC1 left
	val = wm8994_read(codec,WM8994_DAC1_LEFT_VOLUME );
	val &= ~(WM8994_DAC1L_MUTE_MASK | WM8994_DAC1L_VOL_MASK);
	val |= TUNING_DAC1L_VOL; 
	wm8994_write(codec,WM8994_DAC1_LEFT_VOLUME ,val);

	//Unmute and volume ctrl RightDAC
	val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME ); 
	val &= ~(WM8994_DAC1R_MUTE_MASK | WM8994_DAC1R_VOL_MASK);
	val |= TUNING_DAC1R_VOL; //0 db volume	
	wm8994_write(codec,WM8994_DAC1_RIGHT_VOLUME,val);
	wm8994_write(codec,WM8994_DAC2_RIGHT_VOLUME, 0x01C0 );  //hdlnc_ldj_0416_A1026
	wm8994_write(codec,WM8994_DAC2_LEFT_VOLUME, 0x01C0 );

	wm8994_write(codec,WM8994_AIF1_DAC1_FILTERS_1, 0x0000 );  
	wm8994_write(codec,WM8994_AIF2_DAC_FILTERS_1, 0x0080 );  //hdlnc_ldj_0416_A1026

//	wm8994->codec_HPAmp_state = HPAMP_CALL;

if(wm8994->testmode_config_flag == SEC_TEST_PBA_LOOPBACK)
	{
		DEBUG_LOG("SEC_TEST_PBA_LOOPBACK");
		
		val = wm8994_read(codec,WM8994_RIGHT_LINE_INPUT_1_2_VOLUME);	
		val &= ~(WM8994_IN1R_MUTE_MASK | WM8994_IN1R_VOL_MASK);	// Unmute IN1R
		val |= (WM8994_IN1R_VU | TUNING_LOOPBACK_EAR_INPUTMIX_VOL);
		wm8994_write(codec,WM8994_RIGHT_LINE_INPUT_1_2_VOLUME,val);

		// unmute right pga, set volume 
		val = wm8994_read(codec,WM8994_INPUT_MIXER_4 );
		val&= ~(WM8994_IN1R_TO_MIXINR_MASK | WM8994_IN1R_MIXINR_VOL_MASK | WM8994_MIXOUTR_MIXINR_VOL_MASK);
		val |= (WM8994_IN1R_TO_MIXINR);//0db
		wm8994_write(codec,WM8994_INPUT_MIXER_4 ,val);

		val = wm8994_read(codec, WM8994_LEFT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTL_MUTE_N_MASK | WM8994_MIXOUTL_VOL_MASK);
		val |= (WM8994_MIXOUTL_MUTE_N | TUNING_LOOPBACK_OPGAL_VOL);
		wm8994_write(codec,WM8994_LEFT_OPGA_VOLUME, val );

		val = wm8994_read(codec, WM8994_RIGHT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTR_MUTE_N_MASK | WM8994_MIXOUTR_VOL_MASK);
		val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTR_MUTE_N | TUNING_LOOPBACK_OPGAR_VOL);
		wm8994_write(codec,WM8994_RIGHT_OPGA_VOLUME, val );

		val = wm8994_read(codec,WM8994_LEFT_OUTPUT_VOLUME);
		val &= ~(WM8994_HPOUT1L_MUTE_N_MASK | WM8994_HPOUT1L_VOL_MASK);
		val |= (WM8994_HPOUT1L_MUTE_N | TUNING_LOOPBACK_OUTPUTL_VOL);
		wm8994_write(codec, WM8994_LEFT_OUTPUT_VOLUME, val);

		val = wm8994_read(codec,WM8994_RIGHT_OUTPUT_VOLUME);
		val &= ~(WM8994_HPOUT1R_MUTE_N_MASK | WM8994_HPOUT1R_VOL_MASK);
		val |= (WM8994_HPOUT1_VU | WM8994_HPOUT1R_MUTE_N | TUNING_LOOPBACK_OUTPUTR_VOL);
		wm8994_write(codec, WM8994_RIGHT_OUTPUT_VOLUME, val);
	}
    wm8994_call_recording_change_path(codec);
}

void wm8994_set_voicecall_headphone(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->private_data;

	int val;

	u16 TestReturn1=0;
	u16 TestReturn2=0;
	u16 TestLow1=0;
	u16 TestHigh1=0;
	u8 TestLow=0;
	u8 TestHigh=0;

	DEBUG_LOG("");
	//printk("wm8994_set_voicecall_headphone\n");
	if(a1026_mode_status != BYPASSMODE)
	{	
		A1026SetFeature(BYPASSMODE); // set the configure for A1026 to bypass	//hdlnc_ldj_0416_A1026
		a1026_mode_status = BYPASSMODE;
	}
	audio_ctrl_mic_bias_gpio(1);
	
	wm8994_set_voicecall_common_setting(codec);
	
	wm8994_write(codec,WM8994_SIDETONE,0x01C0);
	// Tx -> AIF2 Path

	wm8994_write(codec,WM8994_DAC2_MIXER_VOLUMES, 0x000C );
	wm8994_write(codec, WM8994_DAC2_LEFT_MIXER_ROUTING, WM8994_ADC1_TO_DAC2L);	
	wm8994_write(codec, WM8994_DAC2_RIGHT_MIXER_ROUTING, WM8994_ADC1_TO_DAC2R);


	/*Analogue Input Configuration*/
	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_2);	
	val &= ~(WM8994_TSHUT_ENA_MASK|WM8994_TSHUT_OPDIS_MASK|WM8994_MIXINR_ENA_MASK|WM8994_IN1R_ENA_MASK);	
	val |= (WM8994_TSHUT_ENA|WM8994_TSHUT_OPDIS|WM8994_MIXINL_ENA|WM8994_IN1L_ENA);
	wm8994_write(codec,WM8994_POWER_MANAGEMENT_2, val );

	if((wm8994->testmode_config_flag == SEC_NORMAL)
		|| (wm8994->testmode_config_flag == SEC_TEST_PBA_LOOPBACK)
	)
	{
		val = wm8994_read(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME);	
		val &= ~(WM8994_IN1L_MUTE_MASK | WM8994_IN1L_VOL_MASK);
		val |= (WM8994_IN1L_VU |TUNING_CALL_SPK_INPUTMIX_VOL);
		wm8994_write(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME, val);

		val = wm8994_read(codec, WM8994_INPUT_MIXER_3);
		val&= ~(WM8994_IN1L_TO_MIXINL_MASK | WM8994_IN1L_MIXINL_VOL_MASK | WM8994_MIXOUTL_MIXINL_VOL_MASK);
		val |= (WM8994_IN1L_TO_MIXINL |WM8994_IN1L_MIXINL_VOL);//30db
		wm8994_write(codec, WM8994_INPUT_MIXER_3, val); 
	}	

	wm8994_write(codec, WM8994_INPUT_MIXER_2, WM8994_IN1LP_TO_IN1L | WM8994_IN1LN_TO_IN1L); 	// differential(3) or single ended(1)

	wm8994_write(codec,WM8994_AIF2_CONTROL_1, 	//PCM mode, BCLK invert, LRCLK Invert  //hdlnc_ldj_0416_A1026
		0x8118);

if((wm8994->testmode_config_flag == SEC_NORMAL)
		|| (wm8994->testmode_config_flag == SEC_TEST_PBA_LOOPBACK)
	)
	{
		val = wm8994_read(codec, WM8994_LEFT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTL_MUTE_N_MASK | WM8994_MIXOUTL_VOL_MASK);
		val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTL_MUTE_N | TUNING_CALL_OPGAL_VOL);
		wm8994_write(codec,WM8994_LEFT_OPGA_VOLUME, val );
	
		val = wm8994_read(codec, WM8994_RIGHT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTR_MUTE_N_MASK | WM8994_MIXOUTR_VOL_MASK);
		val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTR_MUTE_N | TUNING_CALL_OPGAR_VOL);
		wm8994_write(codec,WM8994_RIGHT_OPGA_VOLUME, val );
	}
	// Tx -> AIF2 Path
	wm8994_write(codec, WM8994_DAC2_RIGHT_MIXER_ROUTING, WM8994_ADC1_TO_DAC2R);  //hdlnc_ldj_0416_A1026
	wm8994_write(codec,WM8994_POWER_MANAGEMENT_4,   //hdlnc_ldj_0416_A1026
		WM8994_AIF2ADCL_ENA | WM8994_AIF2ADCR_ENA | WM8994_ADCL_ENA | WM8994_ADCR_ENA);
//	wm8994_write(codec,WM8994_POWER_MANAGEMENT_6, 0x0000 );		
	val = wm8994_read(codec, 0x102  ); 	
	val &= ~(0x0003);
	val = 0x0003;
	wm8994_write(codec,0x102,val);
	
	val = wm8994_read(codec, 0x56  ); 	
	val &= ~(0x0003);
	val = 0x0003;
	wm8994_write(codec,0x56,val);
	
	val = wm8994_read(codec, 0x102  ); 	
	val &= ~(0x0000);
	val = 0x0000;
	wm8994_write(codec,0x102,val);

	val = wm8994_read(codec, WM8994_CLASS_W_1  ); 	
	val &= ~(0x0005);
	val |= 0x0005;
	wm8994_write(codec,WM8994_CLASS_W_1,val);

	if((wm8994->testmode_config_flag == SEC_NORMAL)
		|| (wm8994->testmode_config_flag == SEC_TEST_PBA_LOOPBACK)
	)
	{
		val = wm8994_read(codec,WM8994_LEFT_OUTPUT_VOLUME);
		val &= ~(WM8994_HPOUT1L_MUTE_N_MASK | WM8994_HPOUT1L_VOL_MASK);
		val |= (WM8994_HPOUT1_VU | WM8994_HPOUT1L_MUTE_N | TUNING_CALL_OUTPUTL_VOL);
		wm8994_write(codec, WM8994_LEFT_OUTPUT_VOLUME, val);

		val = wm8994_read(codec,WM8994_RIGHT_OUTPUT_VOLUME);
		val &= ~(WM8994_HPOUT1R_MUTE_N_MASK | WM8994_HPOUT1R_VOL_MASK);
		val |= (WM8994_HPOUT1_VU | WM8994_HPOUT1R_MUTE_N | TUNING_CALL_OUTPUTR_VOL);
		wm8994_write(codec, WM8994_RIGHT_OUTPUT_VOLUME, val);
	}

	val = wm8994_read(codec, WM8994_DC_SERVO_2  ); 	
	val &= ~(0x03E0);
	val = 0x03E0;
	wm8994_write(codec,WM8994_DC_SERVO_2,val);
	
	wm8994_write(codec,WM8994_POWER_MANAGEMENT_1, 0x0303 );  
		
	wm8994_write(codec,WM8994_ANALOGUE_HP_1, 0x0022 );  
	wm8994_write(codec,WM8994_CHARGE_PUMP_1, 0x9F25 );  

	msleep(5);

	/*Analogue Output Configuration*/	
	wm8994_write(codec,WM8994_OUTPUT_MIXER_1, 0x0001 );   
	wm8994_write(codec,WM8994_OUTPUT_MIXER_2, 0x0001 );   

	wm8994_write(codec,WM8994_POWER_MANAGEMENT_3, 0x0030 );   
wm8994_write(codec, WM8994_AIF2_CLOCKING_1, 0x0019);	
	wm8994_write(codec,WM8994_DC_SERVO_1, 0x303);  

	msleep(160);	// 160ms delay

	TestReturn1=wm8994_read(codec,WM8994_DC_SERVO_4);
	
	TestLow=(signed char)(TestReturn1 & 0xff);
	TestHigh=(signed char)((TestReturn1>>8) & 0xff);

	TestLow1=((signed short)TestLow-5)&0x00ff;
	TestHigh1=(((signed short)(TestHigh-5)<<8)&0xff00);
	TestReturn2=TestLow1|TestHigh1;
	wm8994_write(codec,WM8994_DC_SERVO_4, TestReturn2);

	val = wm8994_read(codec, WM8994_DC_SERVO_1  ); 	
	val &= ~(0x000F);
	val = 0x000F;
	wm8994_write(codec,WM8994_DC_SERVO_1,val);
	
	msleep(15);
	
	wm8994_write(codec,WM8994_ANALOGUE_HP_1, 0x00EE );  

	//Unmute DAC1 left
	val = wm8994_read(codec,WM8994_DAC1_LEFT_VOLUME );
	val &= ~(WM8994_DAC1L_MUTE_MASK | WM8994_DAC1L_VOL_MASK);
	val |= TUNING_DAC1L_VOL; 
	wm8994_write(codec,WM8994_DAC1_LEFT_VOLUME ,val);

	//Unmute and volume ctrl RightDAC
	val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME ); 
	val &= ~(WM8994_DAC1R_MUTE_MASK | WM8994_DAC1R_VOL_MASK);
	val |= TUNING_DAC1R_VOL; //0 db volume	
	wm8994_write(codec,WM8994_DAC1_RIGHT_VOLUME,val);
	wm8994_write(codec,WM8994_DAC2_RIGHT_VOLUME, 0x01C0 );  //hdlnc_ldj_0416_A1026
	wm8994_write(codec,WM8994_DAC2_LEFT_VOLUME, 0x01C0 );

	wm8994_write(codec,WM8994_AIF1_DAC1_FILTERS_1, 0x0000 );  
	wm8994_write(codec,WM8994_AIF2_DAC_FILTERS_1, 0x0080 );  //hdlnc_ldj_0416_A1026

//	wm8994->codec_HPAmp_state = HPAMP_CALL;
    wm8994_call_recording_change_path(codec);
}

void wm8994_set_voicecall_tty(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->private_data;

	int val;
		A1026SetFeature(BYPASSMODE); // set the configure for A1026 to bypass	//hdlnc_ldj_0416_A1026
	u16 TestReturn1=0;
	u16 TestReturn2=0;
	u16 TestLow1=0;
	u16 TestHigh1=0;
	u8 TestLow=0;
	u8 TestHigh=0;

	DEBUG_LOG("");
	if((HWREV == 0x04)||(HWREV == 0x05) || (HWREV == 0x06))
	{
		if(a1026_mode_status != BYPASSMODE)
		{	
			A1026SetFeature(BYPASSMODE); // set the configure for A1026 to bypass	//hdlnc_ldj_0416_A1026
			a1026_mode_status = BYPASSMODE;
		}
	}	
	else
	{
		if(a1026_mode_status != BYPASSMODE)
		{	
			A1026SetFeature(BYPASSMODE); // set the configure for A1026 to bypass	//hdlnc_ldj_0416_A1026
			a1026_mode_status = BYPASSMODE;
		}
	}
	audio_ctrl_mic_bias_gpio(1);
	
	wm8994_set_voicecall_common_setting(codec);
	
	/*Digital Path Enables and Unmutes*/	
	val = wm8994_read(codec,WM8994_DAC1_LEFT_MIXER_ROUTING);	
	val &= ~(WM8994_AIF1DAC1L_TO_DAC1L_MASK | WM8994_AIF2DACL_TO_DAC1L_MASK);	
	val |= (WM8994_AIF1DAC1L_TO_DAC1L | WM8994_AIF2DACL_TO_DAC1L);
	wm8994_write(codec,WM8994_DAC1_LEFT_MIXER_ROUTING, 0x0004 );  //hdlnc_ldj_0416_A1026
	val = wm8994_read(codec,WM8994_DAC1_RIGHT_MIXER_ROUTING);	
	val &= ~(WM8994_AIF1DAC1R_TO_DAC1R_MASK | WM8994_AIF2DACR_TO_DAC1R_MASK);	
	val |= (WM8994_AIF1DAC1R_TO_DAC1R | WM8994_AIF2DACR_TO_DAC1R);
	wm8994_write(codec,WM8994_DAC1_RIGHT_MIXER_ROUTING, 0x0004 ); //hdlnc_ldj_0416_A1026

	if(wm8994->hw_version == 3)	// H/W Rev D
	{

		wm8994_write(codec, WM8994_DAC2_LEFT_MIXER_ROUTING, WM8994_ADC2_TO_DAC2L | WM8994_ADC1_TO_DAC2L);	
		wm8994_write(codec,WM8994_DAC2_MIXER_VOLUMES, 0x018C );  
		wm8994_write(codec,WM8994_SIDETONE, 0x01C0);
	}
	else		// H/W Rev B
	{
		wm8994_write(codec,WM8994_DAC2_MIXER_VOLUMES, 0x000C );  
		wm8994_write(codec, WM8994_DAC2_LEFT_MIXER_ROUTING, WM8994_ADC2_TO_DAC2L);
		wm8994_write(codec,WM8994_SIDETONE, 0x01C1 );
	}
	/*Analogue Input Configuration*/
	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_2);	
	val &= ~(WM8994_TSHUT_ENA_MASK|WM8994_TSHUT_OPDIS_MASK|WM8994_MIXINR_ENA_MASK|WM8994_IN1R_ENA_MASK);	
	val |= (WM8994_TSHUT_ENA|WM8994_TSHUT_OPDIS|WM8994_MIXINR_ENA|WM8994_IN1R_ENA);
	wm8994_write(codec,WM8994_POWER_MANAGEMENT_2, 0x6110 );

	if(wm8994->testmode_config_flag == SEC_NORMAL)
	{
		val = wm8994_read(codec,WM8994_RIGHT_LINE_INPUT_1_2_VOLUME);	
		val &= ~(WM8994_IN1R_MUTE_MASK | WM8994_IN1R_VOL_MASK);	// Unmute IN1R
		val |= (WM8994_IN1R_VU | TUNING_TTY_RCV_INPUTMIX_VOL);
		wm8994_write(codec,WM8994_RIGHT_LINE_INPUT_1_2_VOLUME,val);
	
		// unmute right pga, set volume 
		val = wm8994_read(codec,WM8994_INPUT_MIXER_4 );
		val&= ~(WM8994_IN1R_TO_MIXINR_MASK | WM8994_IN1R_MIXINR_VOL_MASK | WM8994_MIXOUTR_MIXINR_VOL_MASK);
		val |= (WM8994_IN1R_TO_MIXINR);//0db
		wm8994_write(codec,WM8994_INPUT_MIXER_4 ,val);
	}	

	val = wm8994_read(codec,WM8994_INPUT_MIXER_2 );
	val&= ~(WM8994_IN1RP_TO_IN1R_MASK |  WM8994_IN1RN_TO_IN1R_MASK);
	val |= (WM8994_IN1RP_TO_IN1R|WM8994_IN1RN_TO_IN1R);//0db
	wm8994_write(codec,WM8994_INPUT_MIXER_2, 0x0003 );	 

	wm8994_write(codec,WM8994_AIF2_CONTROL_1, 	//PCM mode, BCLK invert, LRCLK Invert  //hdlnc_ldj_0416_A1026
		0x4118);
//	wm8994_write(codec, WM8994_AIF2_DAC_FILTERS_1, 0x0080 );	
	if((wm8994->testmode_config_flag == SEC_NORMAL)
		|| (wm8994->testmode_config_flag == SEC_TEST_PBA_LOOPBACK)
	)
	{
		val = wm8994_read(codec, WM8994_LEFT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTL_MUTE_N_MASK | WM8994_MIXOUTL_VOL_MASK);
		val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTL_MUTE_N | TUNING_CALL_OPGAL_VOL);
		wm8994_write(codec,WM8994_LEFT_OPGA_VOLUME, val );
	
		val = wm8994_read(codec, WM8994_RIGHT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTR_MUTE_N_MASK | WM8994_MIXOUTR_VOL_MASK);
		val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTR_MUTE_N | TUNING_CALL_OPGAR_VOL);
		wm8994_write(codec,WM8994_RIGHT_OPGA_VOLUME, val );
	}
	// Tx -> AIF2 Path
	wm8994_write(codec, WM8994_DAC2_RIGHT_MIXER_ROUTING, WM8994_ADC1_TO_DAC2R);
	wm8994_write(codec,WM8994_POWER_MANAGEMENT_4, 
		WM8994_AIF2ADCL_ENA | WM8994_AIF2ADCR_ENA | WM8994_ADCL_ENA | WM8994_ADCR_ENA);
//	wm8994_write(codec,WM8994_POWER_MANAGEMENT_6, 0x0000 );		
	val = wm8994_read(codec, 0x102  ); 	
	val &= ~(0x0003);
	val = 0x0003;
	wm8994_write(codec,0x102,val);
	
	val = wm8994_read(codec, 0x56  ); 	
	val &= ~(0x0003);
	val = 0x0003;
	wm8994_write(codec,0x56,val);
	
	val = wm8994_read(codec, 0x102  ); 	
	val &= ~(0x0000);
	val = 0x0000;
	wm8994_write(codec,0x102,val);

	val = wm8994_read(codec, WM8994_CLASS_W_1  ); 	
	val &= ~(0x0005);
	val |= 0x0005;
	wm8994_write(codec,WM8994_CLASS_W_1,val);

	if((wm8994->testmode_config_flag == SEC_NORMAL)
		|| (wm8994->testmode_config_flag == SEC_TEST_PBA_LOOPBACK)
	)
	{
		val = wm8994_read(codec,WM8994_LEFT_OUTPUT_VOLUME);
		val &= ~(WM8994_HPOUT1L_MUTE_N_MASK | WM8994_HPOUT1L_VOL_MASK);
		val |= (WM8994_HPOUT1_VU | WM8994_HPOUT1L_MUTE_N | TUNING_TTY_OUTPUTL_VOL);
		wm8994_write(codec, WM8994_LEFT_OUTPUT_VOLUME, val);

		val = wm8994_read(codec,WM8994_RIGHT_OUTPUT_VOLUME);
		val &= ~(WM8994_HPOUT1R_MUTE_N_MASK | WM8994_HPOUT1R_VOL_MASK);
		val |= (WM8994_HPOUT1_VU | WM8994_HPOUT1R_MUTE_N | TUNING_TTY_OUTPUTR_VOL);
		wm8994_write(codec, WM8994_RIGHT_OUTPUT_VOLUME, val);
	}

	val = wm8994_read(codec, WM8994_DC_SERVO_2  ); 	
	val &= ~(0x03E0);
	val = 0x03E0;
	wm8994_write(codec,WM8994_DC_SERVO_2,val);
	
	wm8994_write(codec,WM8994_POWER_MANAGEMENT_1, 0x0303 );  
		
	wm8994_write(codec,WM8994_ANALOGUE_HP_1, 0x0022 );  
	wm8994_write(codec,WM8994_CHARGE_PUMP_1, 0x9F25 );  

	msleep(5);

	/*Analogue Output Configuration*/	
	wm8994_write(codec,WM8994_OUTPUT_MIXER_1, 0x0001 );   
	wm8994_write(codec,WM8994_OUTPUT_MIXER_2, 0x0001 );   

	wm8994_write(codec,WM8994_POWER_MANAGEMENT_3, 0x0030 );   
wm8994_write(codec, WM8994_AIF2_CLOCKING_1, 0x0019);
	wm8994_write(codec,WM8994_DC_SERVO_1, 0x303);  

	msleep(160);	// 160ms delay

	TestReturn1=wm8994_read(codec,WM8994_DC_SERVO_4);
	
	TestLow=(signed char)(TestReturn1 & 0xff);
	TestHigh=(signed char)((TestReturn1>>8) & 0xff);

	TestLow1=((signed short)TestLow-5)&0x00ff;
	TestHigh1=(((signed short)(TestHigh-5)<<8)&0xff00);
	TestReturn2=TestLow1|TestHigh1;
	wm8994_write(codec,WM8994_DC_SERVO_4, TestReturn2);

	val = wm8994_read(codec, WM8994_DC_SERVO_1  ); 	
	val &= ~(0x000F);
	val = 0x000F;
	wm8994_write(codec,WM8994_DC_SERVO_1,val);
	
	msleep(15);
	
	wm8994_write(codec,WM8994_ANALOGUE_HP_1, 0x00EE );  

	//Unmute DAC1 left
	val = wm8994_read(codec,WM8994_DAC1_LEFT_VOLUME );
	val &= ~(WM8994_DAC1L_MUTE_MASK | WM8994_DAC1L_VOL_MASK);
	val |= TUNING_DAC1L_VOL; 
	wm8994_write(codec,WM8994_DAC1_LEFT_VOLUME ,val);

	//Unmute and volume ctrl RightDAC
	val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME ); 
	val &= ~(WM8994_DAC1R_MUTE_MASK | WM8994_DAC1R_VOL_MASK);
	val |= TUNING_DAC1R_VOL; //0 db volume	
	wm8994_write(codec,WM8994_DAC1_RIGHT_VOLUME,val);
	wm8994_write(codec,WM8994_DAC2_RIGHT_VOLUME, 0x01C0 );
	wm8994_write(codec,WM8994_DAC2_LEFT_VOLUME, 0x01C0 );

	wm8994_write(codec,WM8994_AIF1_DAC1_FILTERS_1, 0x0000 );  
	wm8994_write(codec,WM8994_AIF2_DAC_FILTERS_1, 0x0080 );  


}

void wm8994_set_voicecall_speaker(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->private_data;

	int val;

	DEBUG_LOG("");

	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, 
		WM8994_VMID_SEL_NORMAL | WM8994_BIAS_ENA);


	if((HWREV == 0x04)||(HWREV == 0x05) || (HWREV == 0x06))
	{
		if(a1026_mode_status != FARTALK)
		{	
			A1026SetFeature(FARTALK); // set the configure for A1026 to bypass	//hdlnc_ldj_0416_A1026
			a1026_mode_status = FARTALK;
		}
	}
	else
	{
		if(a1026_mode_status != FARTALK)
		{	
			A1026SetFeature(FARTALK); // set the configure for A1026 to bypass	//hdlnc_ldj_0416_A1026
			a1026_mode_status = FARTALK;
		}
	}
	
	audio_ctrl_mic_bias_gpio(1);

	wm8994_set_voicecall_common_setting(codec);

	wm8994_write(codec, WM8994_DAC1_LEFT_MIXER_ROUTING, WM8994_AIF2DACL_TO_DAC1L | WM8994_AIF1DAC1L_TO_DAC1L);	
	wm8994_write(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING, WM8994_AIF2DACR_TO_DAC1R | WM8994_AIF1DAC1R_TO_DAC1R);	
	wm8994_write(codec,0x603, 0x018C );   
	// Tx -> AIF2 Path
//	wm8994_write(codec, WM8994_DAC2_LEFT_MIXER_ROUTING, WM8994_ADC2_TO_DAC2L);	
	wm8994_write(codec, WM8994_DAC2_LEFT_MIXER_ROUTING, WM8994_ADC2_TO_DAC2L | WM8994_ADC1_TO_DAC2L);		//hdlnc_ldj_0416_A1026
	wm8994_write(codec, WM8994_DAC2_RIGHT_MIXER_ROUTING, WM8994_ADC2_TO_DAC2R | WM8994_ADC1_TO_DAC2R);  //hdlnc_ldj_0416_A1026

	if(a1026_mode_status == BYPASSMODE)
	{
	/*Analogue Input Configuration*/
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_2,   //hdlnc_ldj_0416_A1026
		WM8994_TSHUT_ENA | WM8994_TSHUT_OPDIS | WM8994_MIXINL_ENA |WM8994_MIXINR_ENA | /*WM8994_IN1L_ENA |*/ WM8994_IN2R_ENA);
	}
	else
	{
	/*Analogue Input Configuration*/
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_2,   //hdlnc_ldj_0416_A1026
		WM8994_TSHUT_ENA | WM8994_TSHUT_OPDIS | WM8994_MIXINL_ENA |WM8994_MIXINR_ENA | /*WM8994_IN1L_ENA |*/ WM8994_IN2R_ENA);
	}

	if(a1026_mode_status == BYPASSMODE)
	{
	wm8994_write(codec, WM8994_INPUT_MIXER_2,   //hdlnc_ldj_0416_A1026
	  /* WM8994_IN1LP_TO_IN1L | WM8994_IN1LN_TO_IN1L|*/WM8994_IN2RN_TO_IN2R|WM8994_IN2RP_TO_IN2R); 	// differential(3) or single ended(1)
	}
	else
	{
	wm8994_write(codec, WM8994_INPUT_MIXER_2,   //hdlnc_ldj_0416_A1026
	  /* WM8994_IN1LP_TO_IN1L | WM8994_IN1LN_TO_IN1L|*/WM8994_IN2RN_TO_IN2R|WM8994_IN2RP_TO_IN2R); 	// differential(3) or single ended(1)
	}
	
	if((wm8994->testmode_config_flag == SEC_NORMAL)
		|| (wm8994->testmode_config_flag == SEC_TEST_PBA_LOOPBACK)
	)
	{
		if(a1026_mode_status == BYPASSMODE)
		{
			// Volume Control - Input
			val = wm8994_read(codec, WM8994_INPUT_MIXER_4);
			val&= ~(WM8994_IN2R_TO_MIXINR_MASK | WM8994_IN2R_MIXINR_VOL_MASK | WM8994_MIXOUTR_MIXINR_VOL_MASK);
			val |= (WM8994_IN2R_TO_MIXINR | WM8994_IN2R_MIXINR_VOL);//0db  //hdlnc_ldj_0416_A1026
			wm8994_write(codec, WM8994_INPUT_MIXER_4, val); 
	
			val = wm8994_read(codec, WM8994_RIGHT_LINE_INPUT_3_4_VOLUME );	
			val &= ~(WM8994_IN2R_MUTE_MASK | WM8994_IN2R_VOL_MASK);	// Unmute IN2R
			val |= (WM8994_IN2R_VU | TUNING_CALL_SPKMIC_INPUTMIX_VOL);
			wm8994_write(codec, WM8994_RIGHT_LINE_INPUT_3_4_VOLUME , val);
		}
		else
		{
			#if 0
			// Unmute IN1L PGA, update volume
			val = wm8994_read(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME);	
			val &= ~(WM8994_IN1L_MUTE_MASK | WM8994_IN1L_VOL_MASK);
			val |= (WM8994_IN1L_VU |TUNING_CALL_AUDIENCE_SPK_INPUTMIX_VOL); //volume
			wm8994_write(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME, val);
			#endif
			
			val = wm8994_read(codec, WM8994_RIGHT_LINE_INPUT_3_4_VOLUME);	
			val &= ~(WM8994_IN2R_MUTE_MASK | WM8994_IN2R_VOL_MASK);
			val |= (WM8994_IN2R_VU |TUNING_CALL_AUDIENCE_SPK_INPUTMIX_SUB_VOL); //volume
			wm8994_write(codec, WM8994_RIGHT_LINE_INPUT_3_4_VOLUME, val);  //hdlnc_ldj_0415_A1026
			#if 0
			//Unmute the PGA
			val = wm8994_read(codec, WM8994_INPUT_MIXER_3);
			val&= ~(WM8994_IN1L_TO_MIXINL_MASK | WM8994_IN1L_MIXINL_VOL_MASK | WM8994_MIXOUTL_MIXINL_VOL_MASK);
			val |= (WM8994_IN1L_TO_MIXINL |TUNING_CALL_AUDIENCE_SPK_MIXER_VOL);//0db
			wm8994_write(codec, WM8994_INPUT_MIXER_3, val); 
			#endif	
			val = wm8994_read(codec, WM8994_INPUT_MIXER_4);
			val&= ~(WM8994_IN2R_TO_MIXINR_MASK | WM8994_IN2R_MIXINR_VOL_MASK | WM8994_MIXOUTR_MIXINR_VOL_MASK);
			val |= (WM8994_IN2R_TO_MIXINR |TUNING_CALL_AUDIENCE_SPK_MIXER_SUB_VOL);//0db
			wm8994_write(codec, WM8994_INPUT_MIXER_4, val);   //hdlnc_ldj_0415_A1026
		}
	}
	wm8994_write(codec,WM8994_AIF2_CONTROL_1, 	//PCM mode, BCLK invert, LRCLK Invert  //hdlnc_ldj_0416_A1026
		0x8118);
	/*Analogue Output Configuration*/	
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_3,   //hdlnc_ldj_0416_A1026
		WM8994_SPKRVOL_ENA|WM8994_SPKLVOL_ENA|WM8994_MIXOUTLVOL_ENA | WM8994_MIXOUTRVOL_ENA |WM8994_MIXOUTL_ENA | WM8994_MIXOUTR_ENA);

	if(a1026_mode_status == BYPASSMODE)
	{
		wm8994_write(codec, WM8994_POWER_MANAGEMENT_4, 
		  /*WM8994_AIF2ADCL_ENA | WM8994_ADCL_ENA |*/ WM8994_AIF2ADCR_ENA | WM8994_ADCR_ENA);	
	}
	else
	{
		wm8994_write(codec, WM8994_POWER_MANAGEMENT_4, 
		  /*WM8994_AIF2ADCL_ENA | WM8994_ADCL_ENA | */WM8994_AIF2ADCR_ENA | WM8994_ADCR_ENA);	
	}
	
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_5 , 
		WM8994_AIF2DACL_ENA | WM8994_AIF2DACR_ENA | WM8994_AIF1DAC1L_ENA | WM8994_AIF1DAC1R_ENA | 
		WM8994_DAC1L_ENA | WM8994_DAC1R_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_6, 0x0000);  //hdlnc_ldj_0416_A1026

	wm8994_write(codec, WM8994_AIF2_CLOCKING_1, 0x0019);

	if((wm8994->testmode_config_flag == SEC_NORMAL)
		|| (wm8994->testmode_config_flag == SEC_TEST_PBA_LOOPBACK)
	)
	{
		// Volume Control - Output
		// Unmute the SPKMIXVOLUME
		val = wm8994_read(codec, WM8994_SPKMIXL_ATTENUATION);
		val &= ~(WM8994_SPKMIXL_VOL_MASK);
		val |= TUNING_SPKMIXL_ATTEN;	
		wm8994_write(codec, WM8994_SPKMIXL_ATTENUATION, val);
		
		val = wm8994_read(codec,WM8994_SPKMIXR_ATTENUATION);
		val &= ~(WM8994_SPKMIXR_VOL_MASK);
		val |= TUNING_SPKMIXR_ATTEN;	
		wm8994_write(codec, WM8994_SPKMIXR_ATTENUATION, val);

		val = wm8994_read(codec,WM8994_SPEAKER_VOLUME_LEFT);
		val &= ~(WM8994_SPKOUTL_MUTE_N_MASK | WM8994_SPKOUTL_VOL_MASK);
		val |= (WM8994_SPKOUT_VU | WM8994_SPKOUTL_MUTE_N | TUNING_CALL_SPKL_VOL);
		wm8994_write(codec, WM8994_SPEAKER_VOLUME_LEFT, val);

		val = wm8994_read(codec,WM8994_SPEAKER_VOLUME_RIGHT);
		val &= ~(WM8994_SPKOUTR_MUTE_N_MASK | WM8994_SPKOUTR_VOL_MASK);
		wm8994_write(codec, WM8994_SPEAKER_VOLUME_RIGHT, 0x7F);  //hdlnc_ldj_0416_A1026

		val = wm8994_read(codec,WM8994_CLASSD);
		val &= ~(WM8994_SPKOUTL_BOOST_MASK);
		val |= TUNING_CALL_CLASSD_VOL << WM8994_SPKOUTL_BOOST_SHIFT;
		wm8994_write(codec, WM8994_CLASSD, 0x17F);
	
	}

	val = wm8994_read(codec,WM8994_SPKOUT_MIXERS );
	val &= ~(WM8994_SPKMIXL_TO_SPKOUTL_MASK | 
		WM8994_SPKMIXR_TO_SPKOUTL_MASK | WM8994_SPKMIXR_TO_SPKOUTR_MASK);
	val |= WM8994_SPKMIXL_TO_SPKOUTL;
	wm8994_write(codec,WM8994_SPKOUT_MIXERS,val );
	wm8994_write(codec,WM8994_SPKOUT_MIXERS,0x11);  //hdlnc_ldj_0416_A1026
	wm8994_write(codec,0x36, 0x0003 );    
	/* Digital Path Enables and Unmutes*/	
	wm8994_write(codec,0x520, 0x0080 );     //hdlnc_ldj_0416_A1026
	wm8994_write(codec,WM8994_SIDETONE, 0x01C0 );   


	wm8994_write(codec, WM8994_ANALOGUE_HP_1,0x0000); 
	wm8994_write(codec, WM8994_DC_SERVO_1,0x0000); 

	if(wm8994->testmode_config_flag == SEC_NORMAL)
	{
		//Unmute DAC1 left
		val = wm8994_read(codec,WM8994_DAC1_LEFT_VOLUME );
		val &= ~(WM8994_DAC1L_MUTE_MASK | WM8994_DAC1L_VOL_MASK);
		val |= TUNING_DAC1L_VOL; 
		wm8994_write(codec,WM8994_DAC1_LEFT_VOLUME ,val);
	
		//Unmute and volume ctrl RightDAC
		val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME ); 
		val &= ~(WM8994_DAC1R_MUTE_MASK | WM8994_DAC1R_VOL_MASK);
		val |= TUNING_DAC1R_VOL; //0 db volume	
		wm8994_write(codec,WM8994_DAC1_RIGHT_VOLUME,val);
	}		
	
	wm8994_write(codec,WM8994_DAC2_LEFT_VOLUME, 0x01C0 );   
	wm8994_write(codec,WM8994_DAC2_RIGHT_VOLUME, 0x01C0 );	  //hdlnc_ldj_0416_A1026
	wm8994_write(codec,WM8994_AIF2_DAC_FILTERS_1, WM8994_AIF1DAC1_UNMUTE | 0x0080);     //hdlnc_ldj_0416_A1026
	wm8994_write(codec,WM8994_AIF1_DAC1_FILTERS_1, WM8994_AIF1DAC2_UNMUTE );   
	
		wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, 
		WM8994_SPKOUTL_ENA | WM8994_VMID_SEL_NORMAL | WM8994_BIAS_ENA);
    wm8994_call_recording_change_path(codec);
}

void wm8994_set_voicecall_bluetooth(struct snd_soc_codec *codec)
{
	int val;
	
	DEBUG_LOG("");

	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, 
		WM8994_VMID_SEL_NORMAL | WM8994_BIAS_ENA);
	
	if(a1026_mode_status != BYPASSMODE)
	{	
		A1026SetFeature(BYPASSMODE); // set the configure for A1026 to bypass	//hdlnc_ldj_0416_A1026
		a1026_mode_status = BYPASSMODE;
	}

	wm8994_set_voicecall_common_setting(codec); 

	/*Digital Path Enables and Unmutes*/		 
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_2, 0x0000);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, 0x0000);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_4, 0x3000);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_5, 
// [ 20100416 ytkwon : bt patch
//		WM8994_AIF2DACL_ENA | WM8994_AIF2DACR_ENA | WM8994_AIF1DAC1L_ENA | WM8994_AIF1DAC1R_ENA);
		WM8994_AIF2DACL_ENA | WM8994_AIF2DACR_ENA | WM8994_AIF1DAC1L_ENA | WM8994_AIF1DAC1R_ENA|WM8994_DAC2L_ENA | WM8994_DAC2R_ENA);
// ] 20100416 ytkwon : bt patch

	// If Input MIC is enabled, bluetooth Rx is muted.
	wm8994_write(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME, WM8994_IN1L_MUTE);
	wm8994_write(codec, WM8994_RIGHT_LINE_INPUT_1_2_VOLUME, WM8994_IN1R_MUTE);
	wm8994_write(codec, WM8994_INPUT_MIXER_2, 0x00);
	wm8994_write(codec, WM8994_INPUT_MIXER_3, 0x00);
	wm8994_write(codec, WM8994_INPUT_MIXER_4, 0x00);
	
	//for BT DTMF Play
	//Rx Path: AIF2ADCDAT2 select
	//CP(CALL) Path:GPIO5/DACDAT2 select
	//AP(DTMF) Path: DACDAT1 select
	//Tx Path: GPIO8/DACDAT3 select

	wm8994_write(codec, WM8994_POWER_MANAGEMENT_6, 0x000C);
	
	// AIF1 & AIF2 Output is connected to DAC1	
	wm8994_write(codec, WM8994_DAC2_LEFT_MIXER_ROUTING, WM8994_AIF2DACL_TO_DAC2L | WM8994_AIF1DAC1L_TO_DAC2L);
	wm8994_write(codec, WM8994_DAC2_RIGHT_MIXER_ROUTING, WM8994_AIF2DACR_TO_DAC2R | WM8994_AIF1DAC1R_TO_DAC2R);


	wm8994_write(codec, WM8994_AIF2_CLOCKING_1, 0x0019);
//	wm8994_write(codec, WM8994_DAC2_MIXER_VOLUMES, 0x000C); 

	wm8994_write(codec, WM8994_DAC2_LEFT_VOLUME, 0x01C0);
	wm8994_write(codec, WM8994_DAC2_RIGHT_VOLUME, 0x01C0); 

	wm8994_write(codec, WM8994_OVERSAMPLING, 0X0000);

	//Unmute DAC1 left
	val = wm8994_read(codec,WM8994_DAC1_LEFT_VOLUME );
	val &= ~(WM8994_DAC1L_MUTE_MASK | WM8994_DAC1L_VOL_MASK);
	val |= TUNING_DAC1L_VOL; 
	wm8994_write(codec,WM8994_DAC1_LEFT_VOLUME ,val);
	
	//Unmute and volume ctrl RightDAC
	val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME ); 
	val &= ~(WM8994_DAC1R_MUTE_MASK | WM8994_DAC1R_VOL_MASK);
	val |= TUNING_DAC1R_VOL; //0 db volume	
	wm8994_write(codec,WM8994_DAC1_RIGHT_VOLUME,val);
	
	wm8994_write(codec, WM8994_AIF1_DAC1_FILTERS_1, 0x0000);  
	wm8994_write(codec, WM8994_AIF2_DAC_FILTERS_1, 0x0080); 

	/*GPIO Configuration*/		
	wm8994_write(codec, WM8994_GPIO_8, WM8994_GP8_DIR | WM8994_GP8_DB);
	wm8994_write(codec, WM8994_GPIO_9, WM8994_GP9_DB);
	wm8994_write(codec, WM8994_GPIO_10, WM8994_GP10_DB);
	wm8994_write(codec, WM8994_GPIO_11, WM8994_GP11_DB);
	
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, 0x0007);		//fast vmid
	msleep(50);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, 0x0003);		//normal vmid

	wm8994_call_recording_change_path(codec);
}

void wm8994_set_voicecall_factory_subMIC(struct snd_soc_codec *codec)
{

	
	struct wm8994_priv *wm8994 = codec->private_data;

	int val;
	
	DEBUG_LOG("");

	audio_ctrl_mic_bias_gpio(1);

	wm8994_set_voicecall_common_setting(codec);

	// Analogue Input Configuration -Main MIC
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_2, 
		WM8994_TSHUT_ENA | WM8994_TSHUT_OPDIS | WM8994_MIXINR_ENA | WM8994_IN2R_ENA);  //hdlnc_ldj_0415_A1026
	
	wm8994_write(codec, WM8994_INPUT_MIXER_2, 
	   WM8994_IN2RN_TO_IN2R); 	// differential(3) or single ended(1) //hdlnc_ldj_0415_A1026
	   
	wm8994_write(codec, WM8994_AIF2_CLOCKING_1, 0x0019);

	if((wm8994->testmode_config_flag == SEC_NORMAL)
		|| (wm8994->testmode_config_flag == SEC_TEST_PBA_LOOPBACK)
	)
	{	
		// Unmute IN1L PGA, update volume

		val = wm8994_read(codec, WM8994_RIGHT_LINE_INPUT_3_4_VOLUME);	
		val &= ~(WM8994_IN2R_MUTE_MASK | WM8994_IN2R_VOL_MASK);
		val |= (WM8994_IN2R_VU |0x15); //volume
		wm8994_write(codec, WM8994_RIGHT_LINE_INPUT_3_4_VOLUME, val);  //hdlnc_ldj_0415_A1026
	
		//Unmute the PGA
		val = wm8994_read(codec, WM8994_INPUT_MIXER_4);
		val&= ~(WM8994_IN2R_TO_MIXINR_MASK | WM8994_IN2R_MIXINR_VOL_MASK | WM8994_MIXOUTR_MIXINR_VOL_MASK);
		val |= (WM8994_IN2R_TO_MIXINR |0x14);//0db
		wm8994_write(codec, WM8994_INPUT_MIXER_4, val);   //hdlnc_ldj_0415_A1026
	
		// Volume Control - Output
		val = wm8994_read(codec, WM8994_OUTPUT_MIXER_5);
		val &= ~(WM8994_DACL_MIXOUTL_VOL_MASK);
		val |= TUNING_RCV_OUTMIX5_VOL << WM8994_DACL_MIXOUTL_VOL_SHIFT;
		wm8994_write(codec,WM8994_OUTPUT_MIXER_5, val );
		
		val = wm8994_read(codec, WM8994_OUTPUT_MIXER_6);
		val &= ~(WM8994_DACR_MIXOUTR_VOL_MASK);
		val |= TUNING_RCV_OUTMIX6_VOL << WM8994_DACR_MIXOUTR_VOL_SHIFT;
		wm8994_write(codec,WM8994_OUTPUT_MIXER_6, val );
	
		val = wm8994_read(codec, WM8994_LEFT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTL_MUTE_N_MASK | WM8994_MIXOUTL_VOL_MASK);
		val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTL_MUTE_N | TUNING_RCV_OPGAL_VOL);
		wm8994_write(codec,WM8994_LEFT_OPGA_VOLUME, val );
	
		val = wm8994_read(codec, WM8994_RIGHT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTR_MUTE_N_MASK | WM8994_MIXOUTR_VOL_MASK);
		val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTR_MUTE_N | TUNING_RCV_OPGAR_VOL);
		wm8994_write(codec,WM8994_RIGHT_OPGA_VOLUME, val );
		
		val = wm8994_read(codec, WM8994_HPOUT2_VOLUME);
		val &= ~(WM8994_HPOUT2_MUTE_MASK | WM8994_HPOUT2_VOL_MASK);
		val |= TUNING_HPOUT2_VOL << WM8994_HPOUT2_VOL_SHIFT;
		wm8994_write(codec,WM8994_HPOUT2_VOLUME, val );

	}
	val = wm8994_read(codec, WM8994_DC_SERVO_2  ); 	
	val &= ~(0x03E0);
	val = 0x03E0;
	wm8994_write(codec,WM8994_DC_SERVO_2,val);
	
	wm8994_write(codec,WM8994_POWER_MANAGEMENT_1, 0x0303 |WM8994_HPOUT2_ENA);  
		
	wm8994_write(codec,WM8994_ANALOGUE_HP_1, 0x0022 );  
	wm8994_write(codec,WM8994_CHARGE_PUMP_1, 0x9F25 );  

	msleep(5);
	
	// Output Mixing
	wm8994_write(codec, WM8994_OUTPUT_MIXER_1,WM8994_MIXINR_TO_MIXOUTL);
	wm8994_write(codec, WM8994_OUTPUT_MIXER_2,WM8994_MIXINR_TO_MIXOUTR);
	
	
	val = wm8994_read(codec, WM8994_DC_SERVO_1  ); 	
	val &= ~(0x000F);
	val = 0x000F;
	wm8994_write(codec,WM8994_DC_SERVO_1,val);
	
	msleep(15);
	wm8994_write(codec,0x36, 0x0040 );
	wm8994_write(codec,WM8994_ANALOGUE_HP_1, 0x00EE );  
	
	wm8994_write(codec,WM8994_POWER_MANAGEMENT_3, 0x0030 | WM8994_MIXOUTLVOL_ENA | WM8994_MIXOUTRVOL_ENA |WM8994_MIXOUTL_ENA | WM8994_MIXOUTR_ENA);   
	wm8994_write(codec, WM8994_HPOUT2_MIXER, WM8994_MIXOUTLVOL_TO_HPOUT2 |WM8994_MIXOUTRVOL_TO_HPOUT2);
	wm8994_write(codec, WM8994_AIF2_CLOCKING_1, 0x0019);
	wm8994_write(codec,WM8994_DC_SERVO_1, 0x303);	
	
	
}
void wm8994_set_voicecall_record(struct snd_soc_codec *codec, int channel)
{
        struct wm8994_priv *wm8994 = codec->private_data;
        u16 val;

        DEBUG_LOG("channel = [%d]", channel);


        if(wm8994 ->call_record_path == CALL_RECORDING_MAIN )
        {
                switch(channel)
                {
                        case CH_UDLINK:  //TxRx
                                val = wm8994_read(codec,WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING );
                                val &= ~( WM8994_ADC1L_TO_AIF1ADC1L_MASK | WM8994_AIF2DACL_TO_AIF1ADC1L_MASK);	
                                val |= (WM8994_ADC1L_TO_AIF1ADC1L | WM8994_AIF2DACL_TO_AIF1ADC1L );  
                                wm8994_write(codec,WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING ,val);

                                val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_4);
                                val &= ~( WM8994_AIF1ADC1L_ENA_MASK | WM8994_AIF1ADC1R_ENA_MASK);
                                val |= ( WM8994_AIF1ADC1L_ENA|WM8994_AIF1ADC1R_ENA);
                                wm8994_write(codec,WM8994_POWER_MANAGEMENT_4,val);	

                                wm8994_write(codec,0x300,0x4010);	

                                wm8994_write(codec,0x400,0x00C0);	// AIF1 INPUT VOLUME : 0x00C0:0db
                        break;

                        case CH_UPLINK:  //Tx only
                                val = wm8994_read(codec,WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING );
                                val &= ~( WM8994_ADC1L_TO_AIF1ADC1L_MASK | WM8994_AIF2DACL_TO_AIF1ADC1L_MASK);	
                                val |= (WM8994_ADC1L_TO_AIF1ADC1L);  
                                wm8994_write(codec,WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING ,val);

                                val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_4);
                                val &= ~( WM8994_AIF1ADC1L_ENA_MASK | WM8994_AIF1ADC1R_ENA_MASK);
                                val |= ( WM8994_AIF1ADC1L_ENA|WM8994_AIF1ADC1R_ENA);
                                wm8994_write(codec,WM8994_POWER_MANAGEMENT_4,val);	

                                wm8994_write(codec,0x300,0x4010);	

                                wm8994_write(codec,0x400,0x00C0);	// AIF1 INPUT VOLUME : 0x00C0:0db	
                        break;

                        case CH_DOWNLINK:  //Rx only
                                val = wm8994_read(codec,WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING );
                                val &= ~( WM8994_ADC1L_TO_AIF1ADC1L_MASK | WM8994_AIF2DACL_TO_AIF1ADC1L_MASK);	
                                val |= (WM8994_AIF2DACL_TO_AIF1ADC1L );  
                                wm8994_write(codec,WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING ,val);

                                val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_4);
                                val &= ~( WM8994_AIF1ADC1L_ENA_MASK | WM8994_AIF1ADC1R_ENA_MASK);
                                val |= ( WM8994_AIF1ADC1L_ENA|WM8994_AIF1ADC1R_ENA);
                                wm8994_write(codec,WM8994_POWER_MANAGEMENT_4,val);	

                                wm8994_write(codec,0x300,0x4010);	

                                wm8994_write(codec,0x400,0x00C0);	// AIF1 INPUT VOLUME : 0x00C0:0db	
                        break;

                        default:
                                DEBUG_LOG("Recording through Main Mic in call");
                                audio_ctrl_mic_bias_gpio(1);
            
                                val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_4);
                                val &= ~(WM8994_ADCL_ENA_MASK |WM8994_AIF1ADC1L_ENA_MASK);
                                val |= (WM8994_AIF1ADC1L_ENA | WM8994_ADCL_ENA);
                                wm8994_write(codec, WM8994_POWER_MANAGEMENT_4, val);

                                //Enable timeslots
                                val = wm8994_read(codec, WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING);
                                val |=WM8994_ADC1L_TO_AIF1ADC1L;  
                                wm8994_write(codec, WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING, val);

                                val = wm8994_read(codec, WM8994_AIF1_MASTER_SLAVE);
                                val |= (WM8994_AIF1_MSTR | WM8994_AIF1_CLK_FRC | WM8994_AIF1_LRCLK_FRC);	// Master mode
                                wm8994_write(codec, WM8994_AIF1_MASTER_SLAVE, val);

                                wm8994_write(codec, WM8994_GPIO_1, 0xA101);   // GPIO1 is Input Enable

                                // for stable pcm input when start google voice recognition
                                if(wm8994->recognition_active == REC_ON
#ifdef FEATURE_VSUITE_RECOGNITION
                                	|| wm8994->vsuite_recognition_active == REC_ON
#endif				
                                ) 
                                {
                                        msleep(300);
                                }
                        break;
                }
        }
        else if(wm8994 ->call_record_path == CALL_RECORDING_SUB)
        {
                switch(channel)
                {
                        case CH_UDLINK:  //TxRx
                                val = wm8994_read(codec,WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING);
                                val &= ~( WM8994_ADC1R_TO_AIF1ADC1R_MASK | WM8994_AIF2DACR_TO_AIF1ADC1R_MASK);
                                val |= (WM8994_ADC1R_TO_AIF1ADC1R |WM8994_AIF2DACR_TO_AIF1ADC1R);
                                wm8994_write(codec,WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING,val);

                                val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_4);
                                val &= ~( WM8994_AIF1ADC1L_ENA_MASK | WM8994_AIF1ADC1R_ENA_MASK);
                                val |= ( WM8994_AIF1ADC1L_ENA|WM8994_AIF1ADC1R_ENA);
                                wm8994_write(codec,WM8994_POWER_MANAGEMENT_4,val);	

                                wm8994_write(codec,0x300,0xC010);	

                                wm8994_write(codec,0x401,0x00C0);	// AIF1 INPUT VOLUME : 0x00C0:0db	
                        break;

                        case CH_UPLINK:  //Tx only
                                val = wm8994_read(codec,WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING);
                                val &= ~( WM8994_ADC1R_TO_AIF1ADC1R_MASK | WM8994_AIF2DACR_TO_AIF1ADC1R_MASK);
                                val |= (WM8994_ADC1R_TO_AIF1ADC1R );
                                wm8994_write(codec,WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING,val);

                                val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_4);
                                val &= ~( WM8994_AIF1ADC1L_ENA_MASK | WM8994_AIF1ADC1R_ENA_MASK);
                                val |= ( WM8994_AIF1ADC1L_ENA|WM8994_AIF1ADC1R_ENA);
                                wm8994_write(codec,WM8994_POWER_MANAGEMENT_4,val);	

                                wm8994_write(codec,0x300,0xC010);	

                                wm8994_write(codec,0x401,0x00C0);	// AIF1 INPUT VOLUME : 0x00C0:0db	
                        break;		

                        case CH_DOWNLINK:  //Rx only
                                val = wm8994_read(codec,WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING);
                                val &= ~( WM8994_ADC1R_TO_AIF1ADC1R_MASK | WM8994_AIF2DACR_TO_AIF1ADC1R_MASK);
                                val |= (WM8994_AIF2DACR_TO_AIF1ADC1R);
                                wm8994_write(codec,WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING,val);

                                val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_4);
                                val &= ~( WM8994_AIF1ADC1L_ENA_MASK | WM8994_AIF1ADC1R_ENA_MASK);
                                val |= ( WM8994_AIF1ADC1L_ENA|WM8994_AIF1ADC1R_ENA);
                                wm8994_write(codec,WM8994_POWER_MANAGEMENT_4,val);	

                                wm8994_write(codec,0x300,0xC010);	

                                wm8994_write(codec,0x401,0x00C0);	// AIF1 INPUT VOLUME : 0x00C0:0db	               
                        break;

                        default:
                                DEBUG_LOG("Recording through Headset Mic in call");



    
                                // Mixing left channel output to right channel.
                                val = wm8994_read(codec, WM8994_AIF1_CONTROL_1);	//605H : 0x0010
                                val &= ~(WM8994_AIF1ADCL_SRC_MASK | WM8994_AIF1ADCR_SRC_MASK);
                                val |= (WM8994_AIF1ADCL_SRC | WM8994_AIF1ADCR_SRC);
                                wm8994_write(codec, WM8994_AIF1_CONTROL_1, val);

                                //Digital Paths	
                                //Enable right ADC and time slot
                                val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_4);
                                val &= ~(WM8994_ADCR_ENA_MASK |WM8994_AIF1ADC1R_ENA_MASK );
                                val |= (WM8994_AIF1ADC1R_ENA | WM8994_ADCR_ENA  );
                                wm8994_write(codec,WM8994_POWER_MANAGEMENT_4 ,val);

                                //ADC Right mixer routing
                                val = wm8994_read(codec,WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING);
                                val &= ~( WM8994_ADC1R_TO_AIF1ADC1R_MASK);
                                val |= WM8994_ADC1R_TO_AIF1ADC1R;
                                wm8994_write(codec,WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING,val);

                                val = wm8994_read(codec, WM8994_AIF1_MASTER_SLAVE);
                                val |= (WM8994_AIF1_MSTR | WM8994_AIF1_CLK_FRC | WM8994_AIF1_LRCLK_FRC);	// Master mode
                                wm8994_write(codec, WM8994_AIF1_MASTER_SLAVE, val);

                                wm8994_write( codec, WM8994_GPIO_1, 0xA101 );   // GPIO1 is Input Enable
                        break;
                }	
        }
}
void wm8994_call_recording_change_path(struct snd_soc_codec *codec)
{

        struct wm8994_priv *wm8994 = codec->private_data;
	DEBUG_LOG("Changing rec_path[%d] -> call_record_path [%d]", wm8994 ->rec_path, wm8994 ->call_record_path);
        if(wm8994->rec_path == MAIN)
        {
                wm8994->call_record_path = CALL_RECORDING_MAIN;
                DEBUG_LOG("Changing rec_path[%d] -> call_record_path [%d]", wm8994 ->rec_path, wm8994 ->call_record_path);           
                
                wm8994->rec_path = MIC_OFF;
        }
        else if(wm8994->rec_path == SUB)
        {
                wm8994->call_record_path = CALL_RECORDING_SUB;
                DEBUG_LOG("Changing rec_path[%d] -> call_record_path [%d]", wm8994 ->rec_path, wm8994 ->call_record_path);
                
                wm8994->rec_path = MIC_OFF;
        }


        if(wm8994 ->call_record_path != CALL_RECORDING_OFF)
        {
                DEBUG_LOG("wm8994 ->call_record_path = [%d]", wm8994 ->call_record_path);    
                wm8994_set_voicecall_record(codec, (int)wm8994 ->call_record_ch);
        }
}

void wm8994_set_voicecall_record_off(struct snd_soc_codec *codec)
{
	DEBUG_LOG("");

	//codec off
	wm8994_write(codec,WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING ,0x0000);
	wm8994_write(codec,WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING,0x0000);
}
//[JKB 2010.12.06 add
#if (defined FEATURE_VOIP)
void wm8994_set_voipcall_receiver(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->private_data;

	u16 val;

	DEBUG_LOG("");

    val = wm8994_read(codec, WM8994_OUTPUT_MIXER_5);
	val &= ~(WM8994_DACL_MIXOUTL_VOL_MASK);
	val |= (TUNING_RCV_OUTMIX5_VOL<<WM8994_DACL_MIXOUTL_VOL_SHIFT);
	wm8994_write(codec, WM8994_OUTPUT_MIXER_5, val);

	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_6);
	val &= ~(WM8994_DACR_MIXOUTR_VOL_MASK);
	val |= (TUNING_RCV_OUTMIX6_VOL<<WM8994_DACR_MIXOUTR_VOL_SHIFT);
	wm8994_write(codec, WM8994_OUTPUT_MIXER_6, val);

	if(wm8994->testmode_config_flag == SEC_NORMAL)
	{
		val = wm8994_read(codec, WM8994_LEFT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTL_MUTE_N_MASK|WM8994_MIXOUTL_VOL_MASK);
		val |= (WM8994_MIXOUT_VU|WM8994_MIXOUTL_MUTE_N|TUNING_VOIP_RCV_OPGAL_VOL);
		wm8994_write(codec, WM8994_LEFT_OPGA_VOLUME, val);

		val = wm8994_read(codec, WM8994_RIGHT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTR_MUTE_N_MASK|WM8994_MIXOUTR_VOL_MASK);
		val |= (WM8994_MIXOUT_VU|WM8994_MIXOUTR_MUTE_N|TUNING_VOIP_RCV_OPGAR_VOL);
		wm8994_write(codec, WM8994_RIGHT_OPGA_VOLUME, val);
	}

    val = wm8994_read(codec, WM8994_HPOUT2_VOLUME);
	val &= ~(WM8994_HPOUT2_MUTE_MASK|WM8994_HPOUT2_VOL_MASK);
    val |= (TUNING_HPOUT2_VOL<<WM8994_HPOUT2_VOL_SHIFT);
	wm8994_write(codec, WM8994_HPOUT2_VOLUME, val);

	val = wm8994_read(codec, WM8994_DAC1_LEFT_VOLUME);
	val &= ~(WM8994_DAC1L_MUTE_MASK|WM8994_DAC1L_VOL_MASK);
	val |= (WM8994_DAC1_VU|TUNING_DAC1L_VOL); // 0xC0 = 0dB (MAX)
	wm8994_write(codec, WM8994_DAC1_LEFT_VOLUME, val);

	val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME); 
	val &= ~(WM8994_DAC1R_MUTE_MASK|WM8994_DAC1R_VOL_MASK);
	val |= (WM8994_DAC1_VU|TUNING_DAC1R_VOL);
	wm8994_write(codec, WM8994_DAC1_RIGHT_VOLUME, val);

	if(wm8994->testmode_config_flag == SEC_NORMAL)
	{
		val = wm8994_read(codec, WM8994_AIF1_CONTROL_2);
		val &= ~(WM8994_AIF1DAC_BOOST_MASK);
		val |= (TUNING_VOIP_RCV_AIF1DAC_BOOST<<WM8994_AIF1DAC_BOOST_SHIFT); // 00 = 0dB, 01 = +6dB, 02 = +12dB, 03 = +18dB
		wm8994_write(codec, WM8994_AIF1_CONTROL_2, val);
	}

	// 2DH // DAC1L_TO_HPOUT1L set
	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);
	val &= ~(WM8994_DAC1L_TO_MIXOUTL_MASK);
	val |= (WM8994_DAC1L_TO_MIXOUTL);
	wm8994_write(codec, WM8994_OUTPUT_MIXER_1, val);

	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);
	val &= ~(WM8994_DAC1R_TO_MIXOUTR_MASK);
	val |= (WM8994_DAC1R_TO_MIXOUTR);
	wm8994_write(codec, WM8994_OUTPUT_MIXER_2, val);

	val = wm8994_read(codec, WM8994_HPOUT2_MIXER);
	val &= ~(WM8994_MIXOUTLVOL_TO_HPOUT2_MASK|WM8994_MIXOUTRVOL_TO_HPOUT2_MASK);
	val |= (WM8994_MIXOUTRVOL_TO_HPOUT2|WM8994_MIXOUTLVOL_TO_HPOUT2);
	wm8994_write(codec, WM8994_HPOUT2_MIXER, val);

	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_5);
	//val &= ~(WM8994_AIF1DAC1L_ENA_MASK|WM8994_AIF1DAC1R_ENA_MASK|WM8994_DAC1L_ENA_MASK|WM8994_DAC1R_ENA_MASK);
	//val |= (WM8994_AIF1DAC1L_ENA|WM8994_AIF1DAC1R_ENA|WM8994_DAC1L_ENA|WM8994_DAC1R_ENA);
	val &= ~(WM8994_AIF1DAC1L_ENA_MASK|WM8994_AIF1DAC1R_ENA_MASK|WM8994_DAC1L_ENA_MASK);
	val |= (WM8994_AIF1DAC1L_ENA|WM8994_AIF1DAC1R_ENA|WM8994_DAC1L_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_5, val);

	val = wm8994_read(codec, WM8994_AIF1_DAC1_FILTERS_1);
	val &= ~(WM8994_AIF1DAC1_MUTE_MASK|WM8994_AIF1DAC1_MONO_MASK);
	val |= (WM8994_AIF1DAC1_UNMUTE|WM8994_AIF1DAC1_MONO);
	wm8994_write(codec, WM8994_AIF1_DAC1_FILTERS_1, val);

	val = wm8994_read(codec, WM8994_DAC1_LEFT_MIXER_ROUTING);
	val &= ~(WM8994_AIF1DAC1L_TO_DAC1L_MASK);
	val |= (WM8994_AIF1DAC1L_TO_DAC1L);
	wm8994_write(codec, WM8994_DAC1_LEFT_MIXER_ROUTING, val);

	//val = wm8994_read(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING);
	//val &= ~(WM8994_AIF1DAC1R_TO_DAC1R_MASK);
	//val |= (WM8994_AIF1DAC1R_TO_DAC1R);
	//wm8994_write(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING, val);

	val = wm8994_read(codec, WM8994_CLOCKING_1);
	val &= ~(WM8994_DSP_FS1CLK_ENA_MASK|WM8994_DSP_FSINTCLK_ENA_MASK);
	val |= (WM8994_DSP_FS1CLK_ENA|WM8994_DSP_FSINTCLK_ENA);
	wm8994_write(codec, WM8994_CLOCKING_1, val);

	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_3);
	val &= ~(WM8994_MIXOUTLVOL_ENA_MASK|WM8994_MIXOUTRVOL_ENA_MASK|WM8994_MIXOUTL_ENA_MASK|WM8994_MIXOUTR_ENA_MASK);
	val |= (WM8994_MIXOUTL_ENA|WM8994_MIXOUTR_ENA|WM8994_MIXOUTRVOL_ENA|WM8994_MIXOUTLVOL_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, val);

	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);
	val &= ~(WM8994_BIAS_ENA_MASK|WM8994_VMID_SEL_MASK|WM8994_HPOUT2_ENA_MASK);
	val |= (WM8994_BIAS_ENA|WM8994_VMID_SEL_NORMAL|WM8994_HPOUT2_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

	// 300H // Mixing left channel output to right channel // val: 0x0010
	val = wm8994_read(codec, WM8994_AIF1_CONTROL_1);
	val &= ~(WM8994_AIF1ADCL_SRC_MASK|WM8994_AIF1ADCR_SRC_MASK);
	wm8994_write(codec, WM8994_AIF1_CONTROL_1, val);

	wm8994_write(codec, WM8994_ANTIPOP_2, 0x68); // Main mic volume issue fix

	// 01H // VMID_SEL_NORMAL, BIAS_ENA, MICB1_ENA
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);
	val &= ~(WM8994_VMID_SEL_MASK|WM8994_BIAS_ENA_MASK);
	val |= (WM8994_VMID_SEL_NORMAL|WM8994_BIAS_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

	// 700H // GPIO1 // GP1_DIR IN, GP1_PD EN, GP1_DB DE-BOUNCE, GP1_FN = LOGIC LVL 0
	wm8994_write(codec, WM8994_GPIO_1, 0xA101);

	// 02H // MIXINL_ENA, IN1L_ENA
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_2);
	val &= ~(WM8994_MIXINL_ENA_MASK|WM8994_IN1L_ENA_MASK);
	val |= (WM8994_MIXINL_ENA|WM8994_IN1L_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_2, val);

	if(wm8994->testmode_config_flag == SEC_NORMAL)
	{	
		// 18H // IN1L PGA // IN1L UNMUTE, SET VOL
		val = wm8994_read(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME);
		val &= ~(WM8994_IN1L_MUTE_MASK|WM8994_IN1L_VOL_MASK);
		val |= (WM8994_IN1L_VU|TUNING_VOIP_RCV_INPUTMIX_VOL);
		wm8994_write(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME, val);

		// 29H // MIXINL PGA // IN2L_TO_MIXINL MUTE, IN1L_TO_MIXINL UNMUTE, 0dB
		val = wm8994_read(codec, WM8994_INPUT_MIXER_3);
		val &= ~(WM8994_IN1L_TO_MIXINL_MASK|WM8994_IN1L_MIXINL_VOL_MASK|WM8994_MIXOUTL_MIXINL_VOL_MASK);
		//val |= (WM8994_IN1L_TO_MIXINL|WM8994_IN1L_MIXINL_VOL); // [DI10-1941] VoIP BoostOn (+30dB)
		val |= (WM8994_IN1L_TO_MIXINL);	// [DI28-2336] back to BoostOff (0dB)
		wm8994_write(codec, WM8994_INPUT_MIXER_3, val);
	}

	// 400H // AIF1 ADC1 Left Volume // Gain Tuning // H/W req.
	val = wm8994_read(codec, WM8994_AIF1_ADC1_LEFT_VOLUME);
	val &= ~(WM8994_AIF1ADC1L_VOL_MASK);
	val |= (WM8994_AIF1ADC1_VU|TUNING_VOIP_MAIN_AIF1ADCL_VOL); // ADC Digital Gain
	wm8994_write(codec, WM8994_AIF1_ADC1_LEFT_VOLUME, val);

	// 28H // INPUT MIXER // IN1LP/N_TO_IN1L PGA
	val = wm8994_read(codec, WM8994_INPUT_MIXER_2);	
	val &= (WM8994_IN1LP_TO_IN1L_MASK|WM8994_IN1LN_TO_IN1L_MASK);
	val |= (WM8994_IN1LP_TO_IN1L|WM8994_IN1LN_TO_IN1L);
	wm8994_write(codec, WM8994_INPUT_MIXER_2, val);

	// 410H // AIF1 ADC1 Filters // AIF1 ADC1 hi-path filter on
	val = wm8994_read(codec, WM8994_AIF1_ADC1_FILTERS);
	val &= ~(WM8994_AIF1ADC1L_HPF|WM8994_AIF1ADC1R_HPF);
	val |= (WM8994_AIF1ADC1L_HPF|WM8994_AIF1ADC1R_HPF); // hi-path filter on (L/R)
	wm8994_write(codec, WM8994_AIF1_ADC1_FILTERS, 0x3800); // [DJ05-2239] // 0x5800 -> 0x3800

	// 04H // AIF1ADC1L_ENA, ADCL_ENA 
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_4);
	val &= ~(WM8994_AIF1ADC1L_ENA_MASK|WM8994_ADCL_ENA_MASK);
	val |= (WM8994_AIF1ADC1L_ENA|WM8994_ADCL_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_4, val);

	// 606H // ADC1L_TO_AIF1ADC1L (TIMESLOT 0) ASSIGN
	val = wm8994_read(codec, WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING);
	val &= ~(WM8994_ADC1L_TO_AIF1ADC1L_MASK);
	val |= (WM8994_ADC1L_TO_AIF1ADC1L);
	wm8994_write(codec, WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING, val);
}

void wm8994_set_voipcall_headset(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->private_data;

	u16 val;

	u16 valBefore = 0;
	u16 valAfter = 0;
	u16 valLow1 = 0;
	u16 valHigh1 = 0;
	u8 valLow = 0;
	u8 valHigh = 0;
	
	DEBUG_LOG("");

	//Configuring the Digital Paths
	// Enable the Timeslot0 to DAC1L
	val = wm8994_read(codec, WM8994_DAC1_LEFT_MIXER_ROUTING);
	val &= ~(WM8994_AIF1DAC1L_TO_DAC1L_MASK);
	val |= WM8994_AIF1DAC1L_TO_DAC1L;
	wm8994_write(codec, WM8994_DAC1_LEFT_MIXER_ROUTING, val);

	// Enable the Timeslot0 to DAC1R
	val = wm8994_read(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING);
	val &= ~(WM8994_AIF1DAC1R_TO_DAC1R_MASK);
	val |= WM8994_AIF1DAC1R_TO_DAC1R;
	wm8994_write(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING, val);

	val = wm8994_read(codec, 0x102);
	val &= ~(0x0003);
	val = (0x0003);
	wm8994_write(codec,0x102, val);

	val = wm8994_read(codec, 0x56);
	val &= ~(0x0003);
	val = (0x0003);
	wm8994_write(codec,0x56, val);

	val = wm8994_read(codec, 0x102);
	val &= ~(0x0000);
	val = (0x0000);
	wm8994_write(codec,0x102, val);

	val = wm8994_read(codec, WM8994_CLASS_W_1);
	val &= ~(0x0005);
	val |= (0x0005);
	wm8994_write(codec, WM8994_CLASS_W_1, val);

	// Headset Control
	if(wm8994->testmode_config_flag == SEC_NORMAL)
	{
		val = wm8994_read(codec, WM8994_LEFT_OUTPUT_VOLUME);
		val &= ~(WM8994_HPOUT1L_MUTE_N_MASK|WM8994_HPOUT1L_VOL_MASK);
		val |= (WM8994_HPOUT1_VU|WM8994_HPOUT1L_MUTE_N|TUNING_VOIP_EAR_OUTPUTL_VOL);
		wm8994_write(codec, WM8994_LEFT_OUTPUT_VOLUME, val);

		val = wm8994_read(codec, WM8994_RIGHT_OUTPUT_VOLUME);
		val &= ~(WM8994_HPOUT1R_MUTE_N_MASK|WM8994_HPOUT1R_VOL_MASK);
		val |= (WM8994_HPOUT1_VU|WM8994_HPOUT1R_MUTE_N|TUNING_VOIP_EAR_OUTPUTR_VOL);
		wm8994_write(codec, WM8994_RIGHT_OUTPUT_VOLUME, val);

		val = wm8994_read(codec, WM8994_LEFT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTL_MUTE_N_MASK|WM8994_MIXOUTL_VOL_MASK);
		val |= (WM8994_MIXOUT_VU|WM8994_MIXOUTL_MUTE_N|TUNING_VOIP_EAR_OPGAL_VOL);
		wm8994_write(codec, WM8994_LEFT_OPGA_VOLUME, val);

		val = wm8994_read(codec, WM8994_RIGHT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTR_MUTE_N_MASK|WM8994_MIXOUTR_VOL_MASK);
		val |= (WM8994_MIXOUT_VU|WM8994_MIXOUTR_MUTE_N|TUNING_VOIP_EAR_OPGAR_VOL);
		wm8994_write(codec, WM8994_RIGHT_OPGA_VOLUME, val);
	}

	val = wm8994_read(codec, WM8994_DC_SERVO_2);
	val &= ~(0x03E0);
	val = (0x03E0);
	wm8994_write(codec, WM8994_DC_SERVO_2, val);

	//Enable vmid,bias, hp left and right
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);
	val &= ~(WM8994_BIAS_ENA_MASK|WM8994_VMID_SEL_MASK|WM8994_HPOUT1L_ENA_MASK|WM8994_HPOUT1R_ENA_MASK);
	val |= (WM8994_BIAS_ENA|WM8994_VMID_SEL_NORMAL|WM8994_HPOUT1R_ENA|WM8994_HPOUT1L_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

	val = wm8994_read(codec, WM8994_ANALOGUE_HP_1);
	val &= ~(0x0022);
	val = 0x0022;
	wm8994_write(codec, WM8994_ANALOGUE_HP_1, val);

	//Enable Charge Pump	
	val = wm8994_read(codec, WM8994_CHARGE_PUMP_1);
	val &= ~(WM8994_CP_ENA_MASK);
	val |= (WM8994_CP_ENA|WM8994_CP_ENA_DEFAULT); // this is from wolfson
	wm8994_write(codec, WM8994_CHARGE_PUMP_1, val);

	msleep(5);// 20ms delay

	if(wm8994->testmode_config_flag == SEC_NORMAL)
	{
		val = wm8994_read(codec, WM8994_AIF1_CONTROL_2);
		val &= ~(WM8994_AIF1DAC_BOOST_MASK);
		val |= (TUNING_VOIP_EAR_AIF1DAC_BOOST<<WM8994_AIF1DAC_BOOST_SHIFT); // 00 = 0dB, 01 = +6dB, 02 = +12dB, 03 = +18dB
		wm8994_write(codec, WM8994_AIF1_CONTROL_2, val);
	}

	//Enable Dac1 and DAC2 and the Timeslot0 for AIF1
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_5);
	val &= ~(WM8994_DAC1R_ENA_MASK|WM8994_DAC1L_ENA_MASK|WM8994_AIF1DAC1R_ENA_MASK|WM8994_AIF1DAC1L_ENA_MASK);
	val |= (WM8994_AIF1DAC1L_ENA|WM8994_AIF1DAC1R_ENA|WM8994_DAC1L_ENA|WM8994_DAC1R_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_5, val);

	// 2DH // DAC1L_TO_HPOUT1L set
	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);
	val &=  ~(WM8994_DAC1L_TO_HPOUT1L_MASK|WM8994_DAC1L_TO_MIXOUTL_MASK);
	val |= (WM8994_DAC1L_TO_MIXOUTL);  	
	wm8994_write(codec, WM8994_OUTPUT_MIXER_1, val);

	// Enable DAC1R to HPOUT1R path
	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);
	val &= ~(WM8994_DAC1R_TO_HPOUT1R_MASK|WM8994_DAC1R_TO_MIXOUTR_MASK);
	val |= (WM8994_DAC1R_TO_MIXOUTR);
	wm8994_write(codec, WM8994_OUTPUT_MIXER_2, val);

	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_3);
	val &= ~(WM8994_MIXOUTLVOL_ENA_MASK|WM8994_MIXOUTRVOL_ENA_MASK|WM8994_MIXOUTL_ENA_MASK|WM8994_MIXOUTR_ENA_MASK);
	val |= (WM8994_MIXOUTL_ENA|WM8994_MIXOUTR_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, 0x0030);

	val = wm8994_read(codec, WM8994_DC_SERVO_1);
	val &= ~(0x0303);
	val = (0x0303);
	wm8994_write(codec, WM8994_DC_SERVO_1, val);

	msleep(160); // 160ms delay

	valBefore = wm8994_read(codec, WM8994_DC_SERVO_4);

	valLow=(signed char)(valBefore & 0xff);
	valHigh=(signed char)((valBefore>>8)&0xff);
	valLow1 = ((signed short)(valLow-5))&0x00ff;
	valHigh1 = (((signed short)(valHigh-5))<<8)&0xff00;
	valAfter = (valLow1|valHigh1);
	wm8994_write(codec, WM8994_DC_SERVO_4, valAfter);

	val = wm8994_read(codec, WM8994_DC_SERVO_1);
	val &= ~(0x000F);
	val = (0x000F);
	wm8994_write(codec, WM8994_DC_SERVO_1, val);

	msleep(20);

	// Intermediate HP settings
	val = wm8994_read(codec, WM8994_ANALOGUE_HP_1);
	val &= ~(WM8994_HPOUT1R_DLY_MASK|WM8994_HPOUT1R_OUTP_MASK|WM8994_HPOUT1R_RMV_SHORT_MASK|
		WM8994_HPOUT1L_DLY_MASK|WM8994_HPOUT1L_OUTP_MASK|WM8994_HPOUT1L_RMV_SHORT_MASK);
	val = (WM8994_HPOUT1L_RMV_SHORT|WM8994_HPOUT1L_OUTP|WM8994_HPOUT1L_DLY|WM8994_HPOUT1R_RMV_SHORT|
		WM8994_HPOUT1R_OUTP|WM8994_HPOUT1R_DLY);
	wm8994_write(codec, WM8994_ANALOGUE_HP_1, val);

	//Unmute DAC1 left
	val = wm8994_read(codec, WM8994_DAC1_LEFT_VOLUME);
	val &= ~(WM8994_DAC1L_MUTE_MASK|WM8994_DAC1L_VOL_MASK);
	val |= (TUNING_DAC1L_VOL);
	wm8994_write(codec, WM8994_DAC1_LEFT_VOLUME, val);

	//Unmute and volume ctrl RightDAC
	val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME);
	val &= ~(WM8994_DAC1R_MUTE_MASK|WM8994_DAC1R_VOL_MASK);
	val |= (TUNING_DAC1R_VOL); //0 db volume
	wm8994_write(codec, WM8994_DAC1_RIGHT_VOLUME, val);

	// Unmute the AF1DAC1	
	val = wm8994_read(codec, WM8994_AIF1_DAC1_FILTERS_1);
	val &= ~(WM8994_AIF1DAC1_MUTE_MASK|WM8994_AIF1DAC1_MONO_MASK);
	val |= (WM8994_AIF1DAC1_UNMUTE);
	wm8994_write(codec, WM8994_AIF1_DAC1_FILTERS_1, val);

	printk("[WM8994] wm8994_set_voipcall_headset - rec ear mic\n");
	// 300H // Mixing left channel output to right channel // val: 0x0010
	val = wm8994_read(codec, WM8994_AIF1_CONTROL_1);
	val &= ~(WM8994_AIF1ADCL_SRC_MASK|WM8994_AIF1ADCR_SRC_MASK);
	val |= (WM8994_AIF1ADCL_SRC|WM8994_AIF1ADCR_SRC);
	wm8994_write(codec, WM8994_AIF1_CONTROL_1, val);

	wm8994_write(codec, WM8994_ANTIPOP_2, 0x68); // Ear mic volume issue fix

	// 01H // VMID_SEL, BIAS_ENA
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);
	val &= ~(WM8994_VMID_SEL_MASK|WM8994_BIAS_ENA_MASK);
	val |= (WM8994_VMID_SEL_NORMAL|WM8994_BIAS_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

	// 700H // GPIO1 // GP1_DIR IN, GP1_PD EN, GP1_DB DE-BOUNCE, GP1_FN = LOGIC LVL 0
	wm8994_write(codec, WM8994_GPIO_1, 0xA101);

	// 02H // MIXINR_ENA, IN1R_ENA
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_2);
	val &= ~(WM8994_MIXINR_ENA_MASK|WM8994_IN1R_ENA_MASK);
	val |= (WM8994_MIXINR_ENA|WM8994_IN1R_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_2, val);

	if(wm8994->testmode_config_flag == SEC_NORMAL)
	{	
		// 1AH // IN1R PGA // IN1R UNMUTE, SET VOL
		val = wm8994_read(codec, WM8994_RIGHT_LINE_INPUT_1_2_VOLUME);
		val &= ~(WM8994_IN1R_MUTE_MASK|WM8994_IN1R_VOL_MASK);
		val |= (WM8994_IN1R_VU|TUNING_VOIP_EAR_INPUTMIX_VOL);
		wm8994_write(codec, WM8994_RIGHT_LINE_INPUT_1_2_VOLUME, val);

		// 2AH // MIXINR PGA // IN2R_TO_MIXINR MUTE, IN1R_TO_MIXINR UNMUTE, 0dB
		val = wm8994_read(codec, WM8994_INPUT_MIXER_4);
		val &= ~(WM8994_IN1R_TO_MIXINR_MASK|WM8994_IN1R_MIXINR_VOL_MASK|WM8994_MIXOUTR_MIXINR_VOL_MASK);
		//val |= (WM8994_IN1R_TO_MIXINR|WM8994_IN1R_MIXINR_VOL); // [DI10-1941] VoIP BoostOn(+30dB)
		val |= (WM8994_IN1R_TO_MIXINR);	// [DI28-2336] back to BoostOff (0dB)
		wm8994_write(codec, WM8994_INPUT_MIXER_4, val);
	}

	// 401H // AIF1 ADC1 Right Volume
	val = wm8994_read(codec, WM8994_AIF1_ADC1_RIGHT_VOLUME);
	val &= ~(WM8994_AIF1ADC1R_VOL_MASK);
	val |= (WM8994_AIF1ADC1_VU|TUNING_VOIP_EAR_AIF1ADCR_VOL); // ADC Digital Gain
	wm8994_write(codec, WM8994_AIF1_ADC1_RIGHT_VOLUME, val);

	// 28H // INPUT MIXER // IN1RP/N_TO_IN1R PGA
	val = wm8994_read(codec, WM8994_INPUT_MIXER_2);
	val &= ~(WM8994_IN1RP_TO_IN1R_MASK|WM8994_IN1RN_TO_IN1R_MASK);
	val |= (WM8994_IN1RP_TO_IN1R|WM8994_IN1RN_TO_IN1R);
	wm8994_write(codec, WM8994_INPUT_MIXER_2, val);

	// 410H // AIF1 ADC1 Filters // AIF1 ADC1 hi-path filter on
	val = wm8994_read(codec, WM8994_AIF1_ADC1_FILTERS);
	val &= ~(WM8994_AIF1ADC1L_HPF|WM8994_AIF1ADC1R_HPF);
	val |= (WM8994_AIF1ADC1L_HPF|WM8994_AIF1ADC1R_HPF); // hi-path filter on (L/R)
	wm8994_write(codec, WM8994_AIF1_ADC1_FILTERS, 0x3800); // [DJ05-2239] // 0x5800 -> 0x3800

	// 04H // AIF1ADC1R_ENA, ADCR_ENA
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_4);
	val &= ~(WM8994_AIF1ADC1R_ENA_MASK|WM8994_ADCR_ENA_MASK);
	val |= (WM8994_AIF1ADC1R_ENA|WM8994_ADCR_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_4, val);

	// 607H // ADC1R_TO_AIF1ADC1R (TIMESLOT 0) ASSIGN
	val = wm8994_read(codec, WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING);
	val &= ~(WM8994_ADC1R_TO_AIF1ADC1R_MASK);
	val |= (WM8994_ADC1R_TO_AIF1ADC1R);
	wm8994_write(codec, WM8994_AIF1_ADC1_RIGHT_MIXER_ROUTING, val);
}

#if (defined FEATURE_3POLE_CALL_SUPPORT)
void wm8994_set_voipcall_headphone(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->private_data;

	u16 val;
	u16 valBefore = 0;
	u16 valAfter = 0;
	u16 valLow1 = 0;
	u16 valHigh1 = 0;
	u8 valLow = 0;
	u8 valHigh = 0;

	DEBUG_LOG("");

	//Configuring the Digital Paths
	// Enable the Timeslot0 to DAC1L
	val = wm8994_read(codec, WM8994_DAC1_LEFT_MIXER_ROUTING);
	val &= ~(WM8994_AIF1DAC1L_TO_DAC1L_MASK);
	val |= WM8994_AIF1DAC1L_TO_DAC1L;
	wm8994_write(codec, WM8994_DAC1_LEFT_MIXER_ROUTING, val);

	//Enable the Timeslot0 to DAC1R
	val = wm8994_read(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING);
	val &= ~(WM8994_AIF1DAC1R_TO_DAC1R_MASK);
	val |= WM8994_AIF1DAC1R_TO_DAC1R;
	wm8994_write(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING, val);

	val = wm8994_read(codec, 0x102);
	val &= ~(0x0003);
	val = (0x0003);
	wm8994_write(codec,0x102, val);

	val = wm8994_read(codec, 0x56);
	val &= ~(0x0003);
	val = (0x0003);
	wm8994_write(codec,0x56, val);

	val = wm8994_read(codec, 0x102);
	val &= ~(0x0000);
	val = (0x0000);
	wm8994_write(codec,0x102, val);

	val = wm8994_read(codec, WM8994_CLASS_W_1);
	val &= ~(0x0005);
	val |= (0x0005);
	wm8994_write(codec, WM8994_CLASS_W_1, val);

	// Headset Control
	if(wm8994->testmode_config_flag == SEC_NORMAL)
	{
		val = wm8994_read(codec, WM8994_LEFT_OUTPUT_VOLUME);
		val &= ~(WM8994_HPOUT1L_MUTE_N_MASK|WM8994_HPOUT1L_VOL_MASK);
		val |= (WM8994_HPOUT1_VU|WM8994_HPOUT1L_MUTE_N|TUNING_VOIP_EAR_OUTPUTL_VOL);
		wm8994_write(codec, WM8994_LEFT_OUTPUT_VOLUME, val);

		val = wm8994_read(codec, WM8994_RIGHT_OUTPUT_VOLUME);
		val &= ~(WM8994_HPOUT1R_MUTE_N_MASK|WM8994_HPOUT1R_VOL_MASK);
		val |= (WM8994_HPOUT1_VU|WM8994_HPOUT1R_MUTE_N|TUNING_VOIP_EAR_OUTPUTR_VOL);
		wm8994_write(codec, WM8994_RIGHT_OUTPUT_VOLUME, val);

		val = wm8994_read(codec, WM8994_LEFT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTL_MUTE_N_MASK|WM8994_MIXOUTL_VOL_MASK);
		val |= (WM8994_MIXOUT_VU|WM8994_MIXOUTL_MUTE_N|TUNING_VOIP_EAR_OPGAL_VOL);
		wm8994_write(codec, WM8994_LEFT_OPGA_VOLUME, val);

		val = wm8994_read(codec, WM8994_RIGHT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTR_MUTE_N_MASK|WM8994_MIXOUTR_VOL_MASK);
		val |= (WM8994_MIXOUT_VU|WM8994_MIXOUTR_MUTE_N|TUNING_VOIP_EAR_OPGAR_VOL);
		wm8994_write(codec, WM8994_RIGHT_OPGA_VOLUME, val);
	}

	val = wm8994_read(codec, WM8994_DC_SERVO_2);
	val &= ~(0x03E0);
	val = (0x03E0);
	wm8994_write(codec, WM8994_DC_SERVO_2, val);

	//Enable vmid,bias, hp left and right
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);
	val &= ~(WM8994_BIAS_ENA_MASK|WM8994_VMID_SEL_MASK|WM8994_HPOUT1L_ENA_MASK|WM8994_HPOUT1R_ENA_MASK);
	val |= (WM8994_BIAS_ENA|WM8994_VMID_SEL_NORMAL|WM8994_HPOUT1R_ENA|WM8994_HPOUT1L_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

	val = wm8994_read(codec, WM8994_ANALOGUE_HP_1);
	val &= ~(0x0022);
	val = 0x0022;
	wm8994_write(codec, WM8994_ANALOGUE_HP_1, val);

	//Enable Charge Pump	
	val = wm8994_read(codec, WM8994_CHARGE_PUMP_1);
	val &= ~(WM8994_CP_ENA_MASK);
	val |= (WM8994_CP_ENA|WM8994_CP_ENA_DEFAULT); // this is from wolfson
	wm8994_write(codec, WM8994_CHARGE_PUMP_1, val);

	msleep(5);// 20ms delay

	if(wm8994->testmode_config_flag == SEC_NORMAL)
	{
		val = wm8994_read(codec, WM8994_AIF1_CONTROL_2);
		val &= ~(WM8994_AIF1DAC_BOOST_MASK);
		val |= (TUNING_VOIP_EAR_AIF1DAC_BOOST<<WM8994_AIF1DAC_BOOST_SHIFT); // 00 = 0dB, 01 = +6dB, 02 = +12dB, 03 = +18dB
		wm8994_write(codec, WM8994_AIF1_CONTROL_2, val);
	}

	//Enable Dac1 and DAC2 and the Timeslot0 for AIF1	
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_5);
	val &= ~(WM8994_DAC1R_ENA_MASK|WM8994_DAC1L_ENA_MASK|WM8994_AIF1DAC1R_ENA_MASK|WM8994_AIF1DAC1L_ENA_MASK);
	val |= (WM8994_AIF1DAC1L_ENA|WM8994_AIF1DAC1R_ENA|WM8994_DAC1L_ENA|WM8994_DAC1R_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_5, val);

	// 2DH // DAC1L_TO_HPOUT1L set
	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);
	val &=  ~(WM8994_DAC1L_TO_HPOUT1L_MASK|WM8994_DAC1L_TO_MIXOUTL_MASK);
	val |= (WM8994_DAC1L_TO_MIXOUTL);  	
	wm8994_write(codec, WM8994_OUTPUT_MIXER_1, val);

	// Enable DAC1R to HPOUT1R path
	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);
	val &= ~(WM8994_DAC1R_TO_HPOUT1R_MASK|WM8994_DAC1R_TO_MIXOUTR_MASK);
	val |= (WM8994_DAC1R_TO_MIXOUTR);
	wm8994_write(codec, WM8994_OUTPUT_MIXER_2, val);

	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_3);
	val &= ~(WM8994_MIXOUTLVOL_ENA_MASK|WM8994_MIXOUTRVOL_ENA_MASK|WM8994_MIXOUTL_ENA_MASK|WM8994_MIXOUTR_ENA_MASK);
	val |= (WM8994_MIXOUTL_ENA|WM8994_MIXOUTR_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, 0x0030);

	val = wm8994_read(codec, WM8994_DC_SERVO_1);
	val &= ~(0x0303);
	val = (0x0303);
	wm8994_write(codec, WM8994_DC_SERVO_1, val);

	msleep(160); // 160ms delay

	valBefore = wm8994_read(codec, WM8994_DC_SERVO_4);

	valLow=(signed char)(valBefore & 0xff);
	valHigh=(signed char)((valBefore>>8)&0xff);
	valLow1 = ((signed short)(valLow-5))&0x00ff;
	valHigh1 = (((signed short)(valHigh-5))<<8)&0xff00;
	valAfter = (valLow1|valHigh1);
	wm8994_write(codec, WM8994_DC_SERVO_4, valAfter);

	val = wm8994_read(codec, WM8994_DC_SERVO_1);
	val &= ~(0x000F);
	val = (0x000F);
	wm8994_write(codec, WM8994_DC_SERVO_1, val);

	msleep(20);

	// Intermediate HP settings
	val = wm8994_read(codec, WM8994_ANALOGUE_HP_1);
	val &= ~(WM8994_HPOUT1R_DLY_MASK|WM8994_HPOUT1R_OUTP_MASK|WM8994_HPOUT1R_RMV_SHORT_MASK|
		WM8994_HPOUT1L_DLY_MASK|WM8994_HPOUT1L_OUTP_MASK|WM8994_HPOUT1L_RMV_SHORT_MASK);
	val = (WM8994_HPOUT1L_RMV_SHORT|WM8994_HPOUT1L_OUTP|WM8994_HPOUT1L_DLY|WM8994_HPOUT1R_RMV_SHORT|
		WM8994_HPOUT1R_OUTP|WM8994_HPOUT1R_DLY);
	wm8994_write(codec, WM8994_ANALOGUE_HP_1, val);

	//Unmute DAC1 left
	val = wm8994_read(codec, WM8994_DAC1_LEFT_VOLUME);
	val &= ~(WM8994_DAC1L_MUTE_MASK|WM8994_DAC1L_VOL_MASK);
	val |= (TUNING_DAC1L_VOL);
	wm8994_write(codec, WM8994_DAC1_LEFT_VOLUME, val);

	//Unmute and volume ctrl RightDAC
	val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME);
	val &= ~(WM8994_DAC1R_MUTE_MASK|WM8994_DAC1R_VOL_MASK);
	val |= (TUNING_DAC1R_VOL); //0 db volume
	wm8994_write(codec, WM8994_DAC1_RIGHT_VOLUME, val);

	// Unmute the AF1DAC1	
	val = wm8994_read(codec, WM8994_AIF1_DAC1_FILTERS_1);
	val &= ~(WM8994_AIF1DAC1_MUTE_MASK|WM8994_AIF1DAC1_MONO_MASK);
	val |= (WM8994_AIF1DAC1_UNMUTE);
	wm8994_write(codec, WM8994_AIF1_DAC1_FILTERS_1, val);

	printk("[WM8994] wm8994_set_voipcall_headphone - rec main mic\n");

	// 300H // Mixing left channel output to right channel // val: 0x0010
	val = wm8994_read(codec, WM8994_AIF1_CONTROL_1);
	val &= ~(WM8994_AIF1ADCL_SRC_MASK|WM8994_AIF1ADCR_SRC_MASK);
	wm8994_write(codec, WM8994_AIF1_CONTROL_1, val);

	wm8994_write(codec, WM8994_ANTIPOP_2, 0x68); // Main mic volume issue fix

	// 01H // VMID_SEL_NORMAL, BIAS_ENA, MICB1_ENA
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);
	val &= ~(WM8994_VMID_SEL_MASK|WM8994_BIAS_ENA_MASK);
	val |= (WM8994_VMID_SEL_NORMAL|WM8994_BIAS_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

	// 700H // GPIO1 // GP1_DIR IN, GP1_PD EN, GP1_DB DE-BOUNCE, GP1_FN = LOGIC LVL 0
	wm8994_write(codec, WM8994_GPIO_1, 0xA101);

	// 02H // MIXINL_ENA, IN1L_ENA
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_2);
	val &= ~(WM8994_MIXINL_ENA_MASK|WM8994_IN1L_ENA_MASK);
	val |= (WM8994_MIXINL_ENA|WM8994_IN1L_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_2, val);

	if(wm8994->testmode_config_flag == SEC_NORMAL)
	{	
		// 18H // IN1L PGA // IN1L UNMUTE, SET VOL
		val = wm8994_read(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME);
		val &= ~(WM8994_IN1L_MUTE_MASK|WM8994_IN1L_VOL_MASK);
		val |= (WM8994_IN1L_VU|TUNING_VOIP_3P_INPUTMIX_VOL);
		wm8994_write(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME, val);

		// 29H // MIXINL PGA // IN2L_TO_MIXINL MUTE, IN1L_TO_MIXINL UNMUTE, 0dB
		val = wm8994_read(codec, WM8994_INPUT_MIXER_3);
		val &= ~(WM8994_IN1L_TO_MIXINL_MASK|WM8994_IN1L_MIXINL_VOL_MASK|WM8994_MIXOUTL_MIXINL_VOL_MASK);
		//val |= (WM8994_IN1L_TO_MIXINL|WM8994_IN1L_MIXINL_VOL); // [DI10-1941] VoIP BoostOn (+30dB)
		val |= (WM8994_IN1L_TO_MIXINL);	// [DI28-2336] back to BoostOff (0dB)
		wm8994_write(codec, WM8994_INPUT_MIXER_3, val);
	}

	// 400H // AIF1 ADC1 Left Volume // Gain Tuning // H/W req.
	val = wm8994_read(codec, WM8994_AIF1_ADC1_LEFT_VOLUME);
	val &= ~(WM8994_AIF1ADC1L_VOL_MASK);
	val |= (WM8994_AIF1ADC1_VU|TUNING_VOIP_MAIN_AIF1ADCL_VOL); // ADC Digital Gain
	wm8994_write(codec, WM8994_AIF1_ADC1_LEFT_VOLUME, val);

	// 28H // INPUT MIXER // IN1LP/N_TO_IN1L PGA
	val = wm8994_read(codec, WM8994_INPUT_MIXER_2);	
	val &= (WM8994_IN1LP_TO_IN1L_MASK|WM8994_IN1LN_TO_IN1L_MASK);
	val |= (WM8994_IN1LP_TO_IN1L|WM8994_IN1LN_TO_IN1L);
	wm8994_write(codec, WM8994_INPUT_MIXER_2, val);

	// 410H // AIF1 ADC1 Filters // AIF1 ADC1 hi-path filter on
	val = wm8994_read(codec, WM8994_AIF1_ADC1_FILTERS);
	val &= ~(WM8994_AIF1ADC1L_HPF|WM8994_AIF1ADC1R_HPF);
	val |= (WM8994_AIF1ADC1L_HPF|WM8994_AIF1ADC1R_HPF); // hi-path filter on (L/R)
	wm8994_write(codec, WM8994_AIF1_ADC1_FILTERS, 0x3800); // [DJ05-2239] // 0x5800 -> 0x3800

	// 04H // AIF1ADC1L_ENA, ADCL_ENA 
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_4);
	val &= ~(WM8994_AIF1ADC1L_ENA_MASK|WM8994_ADCL_ENA_MASK);
	val |= (WM8994_AIF1ADC1L_ENA|WM8994_ADCL_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_4, val);

	// 606H // ADC1L_TO_AIF1ADC1L (TIMESLOT 0) ASSIGN
	val = wm8994_read(codec, WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING);
	val &= ~(WM8994_ADC1L_TO_AIF1ADC1L_MASK);
	val |= (WM8994_ADC1L_TO_AIF1ADC1L);
	wm8994_write(codec, WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING, val);
}
#endif

void wm8994_set_voipcall_speaker(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->private_data;

	u16 val;

	DEBUG_LOG("");
    
	// 01H // Disable end point for preventing pop up noise.
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);
	val &= ~(WM8994_SPKOUTL_ENA_MASK);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, 0x0003);

	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_3);
	val &= ~(WM8994_SPKLVOL_ENA_MASK);
	val |= (WM8994_SPKLVOL_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, val);

	val = wm8994_read(codec, WM8994_SPKMIXL_ATTENUATION);
	val &= ~(WM8994_SPKMIXL_VOL_MASK);
	val |= (TUNING_SPKMIXL_ATTEN);
	wm8994_write(codec, WM8994_SPKMIXL_ATTENUATION, val);

	val = wm8994_read(codec, WM8994_SPKMIXR_ATTENUATION);
	val &= ~(WM8994_SPKMIXR_VOL_MASK);
	//val |= (TUNING_SPKMIXR_ATTEN);
	wm8994_write(codec, WM8994_SPKMIXR_ATTENUATION, val);

	if(wm8994->testmode_config_flag == SEC_NORMAL)
	{
		val = wm8994_read(codec, WM8994_SPEAKER_VOLUME_LEFT);
		val &= ~(WM8994_SPKOUTL_MUTE_N_MASK|WM8994_SPKOUTL_VOL_MASK);
		val |= (WM8994_SPKOUT_VU|WM8994_SPKOUTL_MUTE_N|TUNING_VOIP_SPKL_VOL);
		wm8994_write(codec, WM8994_SPEAKER_VOLUME_LEFT, val);

		val = wm8994_read(codec, WM8994_SPEAKER_VOLUME_RIGHT);
		val &= ~(WM8994_SPKOUTR_MUTE_N_MASK|WM8994_SPKOUTR_VOL_MASK);
		//val |= (WM8994_SPKOUT_VU|WM8994_SPKOUTR_MUTE_N|TUNING_MP3_SPKL_VOL);
		wm8994_write(codec, WM8994_SPEAKER_VOLUME_RIGHT, val);

		val = wm8994_read(codec, WM8994_CLASSD);
		val &= ~(WM8994_SPKOUTL_BOOST_MASK);
		val |= (TUNING_VOIP_CLASSD_VOL<<WM8994_SPKOUTL_BOOST_SHIFT);
		wm8994_write(codec, WM8994_CLASSD, val);
	}

	// 610H // DAC1 Left VOL, Unmute
	val = wm8994_read(codec, WM8994_DAC1_LEFT_VOLUME);
	val &= ~(WM8994_DAC1L_MUTE_MASK|WM8994_DAC1L_VOL_MASK);
	val |= (WM8994_DAC1_VU|TUNING_DAC1L_VOL); 
	wm8994_write(codec, WM8994_DAC1_LEFT_VOLUME, val);

	// 611H // DAC1 Right VOL, Unmute
	val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME); 
	val &= ~(WM8994_DAC1R_MUTE_MASK|WM8994_DAC1R_VOL_MASK);
	val |= (WM8994_DAC1_VU|TUNING_DAC1R_VOL);
	wm8994_write(codec, WM8994_DAC1_RIGHT_VOLUME, val);

	if(wm8994->testmode_config_flag == SEC_NORMAL)
	{
		val = wm8994_read(codec, WM8994_AIF1_CONTROL_2); 
		val &= ~(WM8994_AIF1DAC_BOOST_MASK);
		val |= (TUNING_VOIP_SPK_AIF1DAC_BOOST<<WM8994_AIF1DAC_BOOST_SHIFT); // 00 0dB 01 +6dB 02 +12dB 03 +18dB
		wm8994_write(codec, WM8994_AIF1_CONTROL_2, val);
	}

	val = wm8994_read(codec, WM8994_SPKOUT_MIXERS);
	val &= ~(WM8994_SPKMIXL_TO_SPKOUTL_MASK|WM8994_SPKMIXL_TO_SPKOUTR_MASK|WM8994_SPKMIXR_TO_SPKOUTR_MASK);
	val |= (WM8994_SPKMIXL_TO_SPKOUTL);
	wm8994_write(codec, WM8994_SPKOUT_MIXERS, val);

	// 36H // Unmute the DAC path
	val = wm8994_read(codec, WM8994_SPEAKER_MIXER);
	val &= ~(WM8994_DAC1L_TO_SPKMIXL_MASK);
	val |= (WM8994_DAC1L_TO_SPKMIXL);
	wm8994_write(codec, WM8994_SPEAKER_MIXER, val);

	// 05H // AIF1DAC1L/R_ENA, DAC1L_ENA
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_5);
	val &= ~(WM8994_AIF1DAC1L_ENA_MASK|WM8994_AIF1DAC1R_ENA_MASK|WM8994_DAC1L_ENA_MASK);
	val |= (WM8994_AIF1DAC1L_ENA|WM8994_AIF1DAC1R_ENA|WM8994_DAC1L_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_5, val);

	// 420H // AIF1DAC1 Unmute, Mono
	val = wm8994_read(codec, WM8994_AIF1_DAC1_FILTERS_1);
	val &= ~(WM8994_AIF1DAC1_MUTE_MASK|WM8994_AIF1DAC1_MONO_MASK);
	val |= (WM8994_AIF1DAC1_UNMUTE|WM8994_AIF1DAC1_MONO);
	wm8994_write(codec, WM8994_AIF1_DAC1_FILTERS_1, val);

	// 601H // AIF1DAC1L_TO_DAC1L // Enable timeslot0 to left dac
	val = wm8994_read(codec, WM8994_DAC1_LEFT_MIXER_ROUTING);
	val &= ~(WM8994_AIF1DAC1L_TO_DAC1L_MASK);
	val |= (WM8994_AIF1DAC1L_TO_DAC1L);
	wm8994_write(codec, WM8994_DAC1_LEFT_MIXER_ROUTING, val);
	
	// 01H // SPKOUTL_ENA
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);
	val &= ~(WM8994_SPKOUTL_ENA_MASK|WM8994_VMID_SEL_MASK|WM8994_BIAS_ENA_MASK);
	val |= (WM8994_SPKOUTL_ENA|WM8994_VMID_SEL_NORMAL|WM8994_BIAS_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

	printk("[WM8994] wm8994_set_voipcall_speaker - rec main mic\n");

	// 300H // Mixing left channel output to right channel // val: 0x0010
	val = wm8994_read(codec, WM8994_AIF1_CONTROL_1);
	val &= ~(WM8994_AIF1ADCL_SRC_MASK|WM8994_AIF1ADCR_SRC_MASK);
	wm8994_write(codec, WM8994_AIF1_CONTROL_1, val);

	wm8994_write(codec, WM8994_ANTIPOP_2, 0x68); // Main mic volume issue fix

	// 01H // VMID_SEL_NORMAL, BIAS_ENA, MICB1_ENA
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1);
	val &= ~(WM8994_VMID_SEL_MASK|WM8994_BIAS_ENA_MASK);
	val |= (WM8994_VMID_SEL_NORMAL|WM8994_BIAS_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

	// 700H // GPIO1 // GP1_DIR IN, GP1_PD EN, GP1_DB DE-BOUNCE, GP1_FN = LOGIC LVL 0
	wm8994_write(codec, WM8994_GPIO_1, 0xA101);

	// 02H // MIXINL_ENA, IN1L_ENA
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_2);
	val &= ~(WM8994_MIXINL_ENA_MASK|WM8994_IN1L_ENA_MASK);
	val |= (WM8994_MIXINL_ENA|WM8994_IN1L_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_2, val);

	if(wm8994->testmode_config_flag == SEC_NORMAL)
	{	
		// 18H // IN1L PGA // IN1L UNMUTE, SET VOL
		val = wm8994_read(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME);
		val &= ~(WM8994_IN1L_MUTE_MASK|WM8994_IN1L_VOL_MASK);
		val |= (WM8994_IN1L_VU|TUNING_VOIP_SPK_INPUTMIX_VOL);
		wm8994_write(codec, WM8994_LEFT_LINE_INPUT_1_2_VOLUME, val);

		// 29H // MIXINL PGA // IN2L_TO_MIXINL MUTE, IN1L_TO_MIXINL UNMUTE, 0dB
		val = wm8994_read(codec, WM8994_INPUT_MIXER_3);
		val &= ~(WM8994_IN1L_TO_MIXINL_MASK|WM8994_IN1L_MIXINL_VOL_MASK|WM8994_MIXOUTL_MIXINL_VOL_MASK);
		//val |= (WM8994_IN1L_TO_MIXINL|WM8994_IN1L_MIXINL_VOL); // [DI10-1941] VoIP BoostOn (+30dB)
		val |= (WM8994_IN1L_TO_MIXINL);	// [DI28-2336] back to BoostOff (0dB)
		wm8994_write(codec, WM8994_INPUT_MIXER_3, val);
	}

	// 400H // AIF1 ADC1 Left Volume // Gain Tuning // H/W req.
	val = wm8994_read(codec, WM8994_AIF1_ADC1_LEFT_VOLUME);
	val &= ~(WM8994_AIF1ADC1L_VOL_MASK);
	val |= (WM8994_AIF1ADC1_VU|TUNING_VOIP_MAIN_AIF1ADCL_VOL); // ADC Digital Gain
	wm8994_write(codec, WM8994_AIF1_ADC1_LEFT_VOLUME, val);

	// 28H // INPUT MIXER // IN1LP/N_TO_IN1L PGA
	val = wm8994_read(codec, WM8994_INPUT_MIXER_2);	
	val &= (WM8994_IN1LP_TO_IN1L_MASK|WM8994_IN1LN_TO_IN1L_MASK);
	val |= (WM8994_IN1LP_TO_IN1L|WM8994_IN1LN_TO_IN1L);
	wm8994_write(codec, WM8994_INPUT_MIXER_2, val);

	// 410H // AIF1 ADC1 Filters // AIF1 ADC1 hi-path filter on
	val = wm8994_read(codec, WM8994_AIF1_ADC1_FILTERS);
	val &= ~(WM8994_AIF1ADC1L_HPF|WM8994_AIF1ADC1R_HPF);
	val |= (WM8994_AIF1ADC1L_HPF|WM8994_AIF1ADC1R_HPF); // hi-path filter on (L/R)
	wm8994_write(codec, WM8994_AIF1_ADC1_FILTERS, 0x3800); // [DJ05-2239] // 0x5800 -> 0x3800

	// 04H // AIF1ADC1L_ENA, ADCL_ENA 
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_4);
	val &= ~(WM8994_AIF1ADC1L_ENA_MASK|WM8994_ADCL_ENA_MASK);
	val |= (WM8994_AIF1ADC1L_ENA|WM8994_ADCL_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_4, val);

	// 606H // ADC1L_TO_AIF1ADC1L (TIMESLOT 0) ASSIGN
	val = wm8994_read(codec, WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING);
	val &= ~(WM8994_ADC1L_TO_AIF1ADC1L_MASK);
	val |= (WM8994_ADC1L_TO_AIF1ADC1L);
	wm8994_write(codec, WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING, val);
}

void wm8994_set_voipcall_bluetooth(struct snd_soc_codec *codec)
{
//	struct wm8994_priv *wm8994 = codec->private_data;

	u16 val;

	DEBUG_LOG("");

    /* GPIO Configuration */
	wm8994_write(codec, WM8994_GPIO_1, 0xA101);
    wm8994_write(codec, WM8994_GPIO_2, 0xA101);
    wm8994_write(codec, WM8994_GPIO_3, 0xA101);
    wm8994_write(codec, WM8994_GPIO_4, 0xA101);
    wm8994_write(codec, WM8994_GPIO_5, 0xA101);
	wm8994_write(codec, WM8994_GPIO_6, 0xA101);
    wm8994_write(codec, WM8994_GPIO_7, 0xA101);
	wm8994_write(codec, WM8994_GPIO_8, 0xA101);
	wm8994_write(codec, WM8994_GPIO_9, 0xA101);
	wm8994_write(codec, WM8994_GPIO_10, 0xA101);
	wm8994_write(codec, WM8994_GPIO_11, 0xA101);

	/* FLL2	Setting */
	wm8994_write(codec, WM8994_FLL2_CONTROL_2, 0x2F00); // FLL1 Ctrl2, FLL1 Setting
	wm8994_write(codec, WM8994_FLL2_CONTROL_3, 0x3126); // FLL1 Ctrl3, K Value
	wm8994_write(codec, WM8994_FLL2_CONTROL_4, 0x0100); // FLL1 Ctrl4, N Value
	wm8994_write(codec, WM8994_FLL2_CONTROL_5, 0x0C88); // FLL1 Ctrl5
	wm8994_write(codec, WM8994_FLL2_CONTROL_1, (WM8994_FLL2_FRACN_ENA|WM8994_FLL2_ENA));

	wm8994_write(codec, WM8994_AIF2_CLOCKING_1, 0x0018); // AIF2 Clock Source = FLL2

	/* Clocking - 8KHz */
    wm8994_write(codec, WM8994_AIF2_RATE, 0x0003);

	/* AIF2 Interface - PCM Stereo mode */
	wm8994_write(codec, WM8994_AIF2_CONTROL_1, 0x4118); // DSP Mode, BCLK invert, LRCLK normal
    wm8994_write(codec, WM8994_AIF2_BCLK, 0x40);
    wm8994_write(codec, WM8994_AIF2_CONTROL_2, 0x4000); // Left & Right DAC receives left interface data
    wm8994_write(codec, WM8994_AIF2_MASTER_SLAVE, 0x7000);

    /* Analog Output Power Management */
    wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, 0x0003);
    wm8994_write(codec, WM8994_POWER_MANAGEMENT_2, 0x6000);

	/* Digital Path Enables and Unmutes */
    wm8994_write(codec, WM8994_POWER_MANAGEMENT_4, 0x2202); // AIF2ADCL_ENA, AIF1ADC1L_ENA, ADCL_ENA
    wm8994_write(codec, WM8994_POWER_MANAGEMENT_5, 0x220A); // AIF2DACL_ENA, AIF1DAC1L_ENA, DAC2L_ENA, DAC1L_ENA

	/* Clocking */
	val = wm8994_read(codec, WM8994_CLOCKING_1);
	val |= (WM8994_DSP_FS2CLK_ENA);
	wm8994_write(codec, WM8994_CLOCKING_1, val);

    wm8994_write(codec, WM8994_GPIO_3, 0x0100); // 702h // GPIO3|BCLK2
    wm8994_write(codec, WM8994_GPIO_5, 0x0100); // 704h // GPIO5|DACDAT2
    wm8994_write(codec, WM8994_GPIO_8, 0x8100); // GPIO8|DACDAT3
    wm8994_write(codec, WM8994_GPIO_9, 0x0100); // GPIO9|ADCDAT3
    wm8994_write(codec, WM8994_GPIO_10, 0x0100); // GPIO10|LRCLK3
    wm8994_write(codec, WM8994_GPIO_11, 0x0100); // GPIO11|BCLK3

	//for BT DTMF Play
	//Rx Path: AIF2ADCDAT2 select
    //CP(CALL) Path: Analog
	//AP(DTMF) Path: DACDAT1 select
	//Tx Path: GPIO8/DACDAT3 select
    wm8994_write(codec, WM8994_POWER_MANAGEMENT_6, 0x000A);

    wm8994_write(codec, 0x604, WM8994_AIF1DAC1L_TO_DAC2L); // WM8994_DAC2_LEFT_MIXER_ROUTING
    wm8994_write(codec, 0x612, 0x01C0); // DAC2L/R_VOL, 0dB, update

	wm8994_write(codec, WM8994_OVERSAMPLING, 0x0000);
    wm8994_write(codec, WM8994_AIF2_CLOCKING_1, 0x0019); // AIF2 Clock Source = FLL2

	wm8994_write(codec, WM8994_AIF1_DAC1_FILTERS_1, 0x0000);  
	wm8994_write(codec, WM8994_AIF2_DAC_FILTERS_1, 0x0080);

    // 300H // Mixing left channel output to right channel // val: 0x0010
	val = wm8994_read(codec, WM8994_AIF1_CONTROL_1);
	val &= ~(WM8994_AIF1ADCL_SRC_MASK|WM8994_AIF1ADCR_SRC_MASK);
	wm8994_write(codec, WM8994_AIF1_CONTROL_1, val);

	// 606H // ADC1L_TO_AIF1ADC1L (TIMESLOT 0) ASSIGN
	val = wm8994_read(codec, WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING);
	val &= ~(WM8994_ADC1L_TO_AIF1ADC1L_MASK);
	val |= (WM8994_AIF2DACL_TO_AIF1ADC1L);
	wm8994_write(codec, WM8994_AIF1_ADC1_LEFT_MIXER_ROUTING, val);
}
#endif
//JKB]
void wm8994_set_fmradio_common(struct snd_soc_codec *codec, int onoff)
{	
	struct wm8994_priv *wm8994 = codec->private_data;
	
	u16 val;

	DEBUG_LOG("onoff = [%d]", onoff);

	wm8994_write(codec, 0x39, 0x8);	//Cross Talk (H/W requested)

	if(onoff)
	{
		val = wm8994_read(codec, WM8994_LEFT_LINE_INPUT_3_4_VOLUME); 	
		val &= ~(WM8994_IN2L_VU_MASK | WM8994_IN2L_MUTE_MASK | WM8994_IN2L_VOL_MASK);

		if(wm8994->fmradio_path == FMR_HP)
			val |= (WM8994_IN2L_VU | TUNING_FMRADIO_EAR_INPUTMIXL_VOL);
		else
			val |= (WM8994_IN2L_VU | TUNING_FMRADIO_SPK_INPUTMIXL_VOL);
		
		wm8994_write(codec, WM8994_LEFT_LINE_INPUT_3_4_VOLUME, val);

		val = wm8994_read(codec, WM8994_RIGHT_LINE_INPUT_3_4_VOLUME); 	
		val &= ~(WM8994_IN2R_VU_MASK | WM8994_IN2R_MUTE_MASK | WM8994_IN2R_VOL_MASK);

		if(wm8994->fmradio_path == FMR_HP)
			val |= (WM8994_IN2R_VU_MASK | TUNING_FMRADIO_EAR_INPUTMIXL_VOL);
		else
			val |= (WM8994_IN2R_VU_MASK | TUNING_FMRADIO_SPK_INPUTMIXL_VOL);
			
		wm8994_write(codec, WM8994_RIGHT_LINE_INPUT_3_4_VOLUME, val);

		// Input mixer setting - Temporary inserted for blocking MIC and FM radio mixing - DW Shim 2010.02.25
	//	val = wm8994_read(codec, WM8994_INPUT_MIXER_2);
	//	val &= ~(WM8994_IN2LN_TO_IN2L_MASK | WM8994_IN2RN_TO_IN2R_MASK);
	//	val |= (WM8994_IN2LN_TO_IN2L | WM8994_IN2RN_TO_IN2R);
		val = (WM8994_IN2LN_TO_IN2L | WM8994_IN2RN_TO_IN2R);
		wm8994_write(codec, WM8994_INPUT_MIXER_2, val); 	

		if(wm8994->testmode_config_flag == SEC_NORMAL)
		{
			// IN2L to MIXINL
			val = wm8994_read(codec, WM8994_INPUT_MIXER_3);
			val &= ~(WM8994_IN2L_TO_MIXINL_MASK);
			val |= WM8994_IN2L_TO_MIXINL;
			wm8994_write(codec, WM8994_INPUT_MIXER_3, val);

			//IN2R to MIXINR
			val = wm8994_read(codec, WM8994_INPUT_MIXER_4);
			val &= ~(WM8994_IN2R_TO_MIXINR_MASK);
			val |= WM8994_IN2R_TO_MIXINR;
			wm8994_write(codec, WM8994_INPUT_MIXER_4, val);	
		}
		
		//DRC for Noise-gate (AIF2)
		wm8994_write(codec, WM8994_AIF2_ADC_FILTERS, 0xF800);
		wm8994_write(codec, WM8994_AIF2_DAC_FILTERS_1, 0x0036);
		wm8994_write(codec, WM8994_AIF2_DAC_FILTERS_2, 0x0010);
		wm8994_write(codec, WM8994_AIF2_DRC_2, 0x0840);
		wm8994_write(codec, WM8994_AIF2_DRC_3, 0x2400);
		wm8994_write(codec, WM8994_AIF2_DRC_4, 0x0000);
		wm8994_write(codec, WM8994_AIF2_DRC_5, 0x0000);
		wm8994_write(codec, WM8994_AIF2_DRC_1, 0x019C);
	}
	else
	{		
		val = wm8994_read(codec, WM8994_LEFT_LINE_INPUT_3_4_VOLUME);	
		val &= ~(WM8994_IN2L_VU_MASK | WM8994_IN2L_MUTE_MASK | WM8994_IN2L_VOL_MASK);
		val |= (WM8994_IN2L_VU | WM8994_IN2L_MUTE);
		wm8994_write(codec, WM8994_LEFT_LINE_INPUT_3_4_VOLUME, val);

		val = wm8994_read(codec, WM8994_RIGHT_LINE_INPUT_3_4_VOLUME);
		val &= ~(WM8994_IN2R_VU_MASK | WM8994_IN2R_MUTE_MASK | WM8994_IN2R_VOL_MASK);
		val |= (WM8994_IN2R_VU_MASK | WM8994_IN2R_MUTE);
		wm8994_write(codec, WM8994_RIGHT_LINE_INPUT_3_4_VOLUME, val);

		val = wm8994_read(codec, WM8994_INPUT_MIXER_2);
		val &= ~(WM8994_IN2LN_TO_IN2L | WM8994_IN2RN_TO_IN2R);
		wm8994_write(codec, WM8994_INPUT_MIXER_2, val);
		
		if(wm8994->testmode_config_flag == SEC_NORMAL)
		{
			// IN2L to MIXINL
			val = wm8994_read(codec, WM8994_INPUT_MIXER_3);
			val &= ~(WM8994_IN2L_TO_MIXINL_MASK);
			wm8994_write(codec, WM8994_INPUT_MIXER_3, val);

			//IN2R to MIXINR
			val = wm8994_read(codec, WM8994_INPUT_MIXER_4);
			val &= ~(WM8994_IN2R_TO_MIXINR_MASK);
			wm8994_write(codec, WM8994_INPUT_MIXER_4, val);	
		}
	}		
}

void wm8994_set_fmradio_headset(struct snd_soc_codec *codec)
{	
	struct wm8994_priv *wm8994 = codec->private_data;

	u16 val;
	
	u16 nReadServo4Val = 0;
	u16 ncompensationResult = 0;
	u16 nCompensationResultLow=0;
	u16 nCompensationResultHigh=0;
	u8  nServo4Low = 0;
	u8  nServo4High = 0;
	
	DEBUG_LOG("Routing ear path : FM Radio -> EAR Out");

	wm8994->fmradio_path = FMR_HP;

	wm8994_disable_fmradio_path(codec, FMR_SPK);

	//DAC1 Setting
	val = wm8994_read(codec, WM8994_DAC1_LEFT_MIXER_ROUTING);	//601H : 0x05
	val &= ~(WM8994_AIF1DAC1L_TO_DAC1L_MASK | WM8994_AIF1DAC2L_TO_DAC1L_MASK | WM8994_AIF2DACL_TO_DAC1L_MASK);
	val |= (WM8994_AIF2DACL_TO_DAC1L | WM8994_AIF1DAC1L_TO_DAC1L);
	wm8994_write(codec, WM8994_DAC1_LEFT_MIXER_ROUTING, val);

	val = wm8994_read(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING);	//602H : 0x05
	val &= ~(WM8994_AIF1DAC1R_TO_DAC1R_MASK | WM8994_AIF1DAC2R_TO_DAC1R_MASK | WM8994_AIF2DACR_TO_DAC1R_MASK);
	val |= (WM8994_AIF1DAC1R_TO_DAC1R | WM8994_AIF2DACR_TO_DAC1R);
	wm8994_write(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING, val);

	//* Headset
	wm8994_write(codec, 0x102, 0x0003);
	wm8994_write(codec, 0x56, 0x0003);
	wm8994_write(codec, 0x102, 0x0000);

	wm8994_write(codec, WM8994_GPIO_3, 0x0100);
	wm8994_write(codec, WM8994_GPIO_4, 0x0100);
	wm8994_write(codec, WM8994_GPIO_5, 0x8100);
	wm8994_write(codec, WM8994_GPIO_6, 0xA101);
	wm8994_write(codec, WM8994_GPIO_7, 0x0100);

	val = wm8994_read(codec, WM8994_LEFT_OUTPUT_VOLUME); 	
	val = 0x0000;
	wm8994_write(codec, WM8994_LEFT_OUTPUT_VOLUME, val);

	val = wm8994_read(codec, WM8994_RIGHT_OUTPUT_VOLUME); 	
	val = 0x0000;
	wm8994_write(codec, WM8994_RIGHT_OUTPUT_VOLUME, val);
	
	// Disable reg sync to MCLK
	val = wm8994_read(codec, WM8994_AIF1_CLOCKING_1); 	
	val &= ~(WM8994_AIF1CLK_ENA_MASK);
	val |= WM8994_AIF1CLK_ENA;
	wm8994_write(codec, WM8994_AIF1_CLOCKING_1, val);


	// Analogue Path Config
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_2  ); 	
	val &= ~(WM8994_MIXINL_ENA_MASK | WM8994_MIXINR_ENA_MASK | WM8994_IN2L_ENA_MASK | WM8994_IN2R_ENA_MASK);
	val |= (WM8994_MIXINL_ENA | WM8994_MIXINR_ENA| WM8994_IN2L_ENA| WM8994_IN2R_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_2 , val );

	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1); 	
	val &= ~(WM8994_BIAS_ENA_MASK | WM8994_VMID_SEL_NORMAL);
	val |= (WM8994_BIAS_ENA | WM8994_VMID_SEL_NORMAL);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1 , 0x0003);
		
	//* Unmutes
	// Output setting
	if(wm8994->testmode_config_flag == SEC_NORMAL)
	{
	val = wm8994_read(codec, WM8994_LEFT_OUTPUT_VOLUME); 	
		val &= ~(WM8994_HPOUT1L_MUTE_N_MASK | WM8994_HPOUT1L_VOL_MASK);
		val |= (WM8994_HPOUT1_VU | WM8994_HPOUT1L_MUTE_N | TUNING_FMRADIO_OUTPUTL_VOL);
	wm8994_write(codec, WM8994_LEFT_OUTPUT_VOLUME, val);

	val = wm8994_read(codec, WM8994_RIGHT_OUTPUT_VOLUME); 	
		val &= ~(WM8994_HPOUT1R_MUTE_N_MASK | WM8994_HPOUT1R_VOL_MASK);
		val |= (WM8994_HPOUT1_VU | WM8994_HPOUT1R_MUTE_N | TUNING_FMRADIO_OUTPUTR_VOL);
	wm8994_write(codec, WM8994_RIGHT_OUTPUT_VOLUME, val);

		val = wm8994_read(codec, WM8994_LEFT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTL_MUTE_N_MASK | WM8994_MIXOUTL_VOL_MASK);
		val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTL_MUTE_N | TUNING_FMRADIO_OPGAL_VOL);
		wm8994_write(codec,WM8994_LEFT_OPGA_VOLUME, val );
	
		val = wm8994_read(codec, WM8994_RIGHT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTR_MUTE_N_MASK | WM8994_MIXOUTR_VOL_MASK);
		val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTR_MUTE_N | TUNING_FMRADIO_OPGAR_VOL);
		wm8994_write(codec,WM8994_RIGHT_OPGA_VOLUME, val );
		
		val = wm8994_read(codec, WM8994_AIF1_DAC1_LEFT_VOLUME);
		val &= ~(WM8994_AIF1DAC1L_VOL_MASK);
		val |= (WM8994_AIF1DAC1_VU | TUNING_DAC1L_RADIO_VOL);
		wm8994_write(codec, WM8994_AIF1_DAC1_LEFT_VOLUME, val);

		val = wm8994_read(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME);
		val &= ~(WM8994_AIF1DAC1R_VOL_MASK);
		val |= (WM8994_AIF1DAC1_VU |TUNING_DAC1R_RADIO_VOL);
		wm8994_write(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME, val);
	}	

	wm8994_set_fmradio_common(codec, 1);
	
	//FLL2 Setting
	val = wm8994_read(codec, WM8994_AIF2_CLOCKING_1);	//204H : 0x0011
	val &= ~(WM8994_AIF2CLK_ENA_MASK | WM8994_AIF2CLK_SRC_MASK);
	val |= (WM8994_AIF2CLK_ENA | 0x2 << WM8994_AIF2CLK_SRC_SHIFT);
	wm8994_write(codec, WM8994_AIF2_CLOCKING_1, val);

	val = wm8994_read(codec, WM8994_CLOCKING_1);	//208H : 0xF -> 0xE
	val &= ~(WM8994_SYSCLK_SRC_MASK | WM8994_DSP_FSINTCLK_ENA_MASK | WM8994_DSP_FS2CLK_ENA_MASK | WM8994_DSP_FS1CLK_ENA_MASK);
	val |= (WM8994_DSP_FS1CLK_ENA | WM8994_DSP_FS2CLK_ENA | WM8994_DSP_FSINTCLK_ENA);
	wm8994_write(codec, WM8994_CLOCKING_1, val);

	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_4);	//04H
	val &= ~(WM8994_AIF2ADCL_ENA_MASK | WM8994_AIF2ADCR_ENA_MASK | WM8994_ADCL_ENA_MASK | WM8994_ADCR_ENA_MASK);
	val |= (WM8994_AIF2ADCL_ENA | WM8994_AIF2ADCR_ENA | WM8994_ADCL_ENA | WM8994_ADCR_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_4, val);

	//DAC2 Setting
	wm8994_write(codec, WM8994_DAC2_MIXER_VOLUMES, 0x018C);	//603H : 0x018C

	val = wm8994_read(codec, WM8994_DAC2_LEFT_MIXER_ROUTING);	//604H : 0x0010
	val &= ~(WM8994_ADC1_TO_DAC2L_MASK);
	val |= (WM8994_ADC1_TO_DAC2L);
	wm8994_write(codec, WM8994_DAC2_LEFT_MIXER_ROUTING, val);

	val = wm8994_read(codec, WM8994_DAC2_RIGHT_MIXER_ROUTING);	//605H : 0x0010
	val &= ~(WM8994_ADC1_TO_DAC2R_MASK);
	val |= (WM8994_ADC1_TO_DAC2R);
	wm8994_write(codec, WM8994_DAC2_RIGHT_MIXER_ROUTING, val);

	val = wm8994_read(codec, WM8994_DAC2_LEFT_VOLUME);	//612 : 1C0
	val &= ~(WM8994_DAC2L_MUTE_MASK | WM8994_DAC2L_VOL_MASK);
	val |= (WM8994_DAC2_VU | TUNING_DAC2L_VOL); //0 db volume	
	wm8994_write(codec,WM8994_DAC2_LEFT_VOLUME,val);
	
	val = wm8994_read(codec, WM8994_DAC2_RIGHT_VOLUME);	//613 : 1C0
	val &= ~(WM8994_DAC2R_MUTE_MASK | WM8994_DAC2R_VOL_MASK);
	val |= (WM8994_DAC2_VU | TUNING_DAC2R_VOL); //0 db volume	
	wm8994_write(codec, WM8994_DAC2_RIGHT_VOLUME, val);

	wm8994_write(codec, WM8994_AIF1_DAC1_EQ_GAINS_1, 0x0000);	//480 : 0
	 
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_3);	//03 : F
	val &= ~(WM8994_MIXOUTLVOL_ENA_MASK | WM8994_MIXOUTRVOL_ENA_MASK | WM8994_MIXOUTL_ENA_MASK | WM8994_MIXOUTR_ENA_MASK);
	val |= (WM8994_MIXOUTLVOL_ENA | WM8994_MIXOUTRVOL_ENA | WM8994_MIXOUTL_ENA | WM8994_MIXOUTR_ENA);	
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, val);

	wm8994_write(codec, WM8994_OVERSAMPLING, 0x0000);
	
//AIF2 Master/Slave , LOOPBACK, AIF2DAC Unmute
	val = wm8994_read(codec, WM8994_AIF2_MASTER_SLAVE);	//312 : 7000
	val &= ~(WM8994_AIF2_LRCLK_FRC_MASK | WM8994_AIF2_CLK_FRC_MASK | WM8994_AIF2_MSTR_MASK);
	val |= (WM8994_AIF2_LRCLK_FRC | WM8994_AIF2_CLK_FRC | WM8994_AIF2_MSTR);
	wm8994_write(codec, WM8994_AIF2_MASTER_SLAVE, val);

	val = wm8994_read(codec, WM8994_AIF2_CONTROL_2);	//311 : 4001
	val &= ~(WM8994_AIF2_LOOPBACK_MASK);
	val |= (WM8994_AIF2_LOOPBACK);
	wm8994_write(codec, WM8994_AIF2_CONTROL_2, val);

	wm8994_write(codec, WM8994_SIDETONE, 0x01c0);

	//* DC Servo Series Count
	val = 0x03E0;
	wm8994_write(codec, WM8994_DC_SERVO_2, val);

	//* HP first and second stage
	//Enable vmid,bias, hp left and right
	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_1 );
	val &= ~(WM8994_BIAS_ENA_MASK | WM8994_VMID_SEL_MASK |WM8994_HPOUT1L_ENA_MASK |WM8994_HPOUT1R_ENA_MASK);
	val |= (WM8994_BIAS_ENA | WM8994_VMID_SEL_NORMAL |WM8994_HPOUT1R_ENA	 |WM8994_HPOUT1L_ENA);  
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

	val = (WM8994_HPOUT1L_DLY | WM8994_HPOUT1R_DLY);
	wm8994_write(codec, WM8994_ANALOGUE_HP_1, val);

	//Enable Charge Pump	
	val = wm8994_read(codec, WM8994_CHARGE_PUMP_1);
	val &= ~WM8994_CP_ENA_MASK ;
	val |= WM8994_CP_ENA | WM8994_CP_ENA_DEFAULT ; // this is from wolfson  	
	wm8994_write(codec, WM8994_CHARGE_PUMP_1, 0x9F25);

	msleep(5);

	//Digital  Mixer setting
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_5);	//05 : 3303
	val &= ~(WM8994_AIF2DACL_ENA_MASK | WM8994_AIF2DACR_ENA_MASK | WM8994_AIF1DAC1L_ENA_MASK | WM8994_AIF1DAC1R_ENA_MASK
		 | WM8994_DAC1L_ENA_MASK | WM8994_DAC1R_ENA_MASK);
	val |= (WM8994_AIF2DACL_ENA | WM8994_AIF2DACR_ENA | WM8994_AIF1DAC1L_ENA | WM8994_AIF1DAC1R_ENA
		 | WM8994_DAC1L_ENA | WM8994_DAC1R_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_5, val);

	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);	//2D : 1
	val &= ~(WM8994_DAC1L_TO_MIXOUTL_MASK);
	val |= (WM8994_DAC1L_TO_MIXOUTL);
	wm8994_write(codec, WM8994_OUTPUT_MIXER_1, val);

	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);	//2E : 1
	val &= ~(WM8994_DAC1R_TO_MIXOUTR_MASK);
	val |= (WM8994_DAC1R_TO_MIXOUTR);	
	wm8994_write(codec, WM8994_OUTPUT_MIXER_2, val);

	//* DC Servo 
	val = (WM8994_DCS_TRIG_SERIES_1 | WM8994_DCS_TRIG_SERIES_0 | WM8994_DCS_ENA_CHAN_1 | WM8994_DCS_ENA_CHAN_0);
	wm8994_write(codec,WM8994_DC_SERVO_1, 0x0303 );

	msleep(160);

	nReadServo4Val=wm8994_read(codec,WM8994_DC_SERVO_4);
	nServo4Low=(signed char)(nReadServo4Val & 0xff);
	nServo4High=(signed char)((nReadServo4Val>>8) & 0xff);

	nCompensationResultLow=((signed short)nServo4Low -5)&0x00ff;
	nCompensationResultHigh=((signed short)(nServo4High -5)<<8)&0xff00;
	ncompensationResult=nCompensationResultLow|nCompensationResultHigh;
	wm8994_write(codec,WM8994_DC_SERVO_4, ncompensationResult);

	val = (WM8994_DCS_TRIG_DAC_WR_1 | WM8994_DCS_TRIG_DAC_WR_0 | WM8994_DCS_ENA_CHAN_1 | WM8994_DCS_ENA_CHAN_0);
	wm8994_write(codec,WM8994_DC_SERVO_1, val );

	msleep(20);

	//* Headphone Output
		// Intermediate HP settings
	val = wm8994_read(codec, WM8994_ANALOGUE_HP_1); 	
	val &= ~(WM8994_HPOUT1R_DLY_MASK |WM8994_HPOUT1R_OUTP_MASK |WM8994_HPOUT1R_RMV_SHORT_MASK |
		WM8994_HPOUT1L_DLY_MASK |WM8994_HPOUT1L_OUTP_MASK | WM8994_HPOUT1L_RMV_SHORT_MASK);
	val |= (WM8994_HPOUT1L_RMV_SHORT | WM8994_HPOUT1L_OUTP|WM8994_HPOUT1L_DLY |WM8994_HPOUT1R_RMV_SHORT | 
		WM8994_HPOUT1R_OUTP | WM8994_HPOUT1R_DLY);
	wm8994_write(codec, WM8994_ANALOGUE_HP_1, val);

	//DAC1, DAC2 Volume Setting
	//Unmute DAC1 left
	val = wm8994_read(codec,WM8994_DAC1_LEFT_VOLUME );	//610H : 1C0
	val &= ~(WM8994_DAC1L_MUTE_MASK | WM8994_DAC1L_VOL_MASK);
	val |= (WM8994_DAC1_VU | TUNING_DAC1L_VOL); 
	wm8994_write(codec,WM8994_DAC1_LEFT_VOLUME ,val);

	//Unmute and volume ctrl RightDAC
	val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME);	//611 : 1C0
	val &= ~(WM8994_DAC1R_MUTE_MASK | WM8994_DAC1R_VOL_MASK);
	val |= (WM8994_DAC1_VU | TUNING_DAC1R_VOL); //0 db volume	
	wm8994_write(codec,WM8994_DAC1_RIGHT_VOLUME,val);

	//DAC1 Unmute
	wm8994_write(codec, WM8994_AIF1_DAC1_FILTERS_1, 0x0000);

	val = wm8994_read(codec, WM8994_AIF2_DAC_FILTERS_1);	//520 : 0
	val &= ~(WM8994_AIF2DAC_MUTE_MASK);
	wm8994_write(codec, WM8994_AIF2_DAC_FILTERS_1, val);

}
	
void wm8994_set_fmradio_speaker(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->private_data;

	u16 val;

	DEBUG_LOG("Routing spk path : FM Radio -> SPK Out");

	wm8994_disable_fmradio_path(codec, FMR_HP);

	wm8994->fmradio_path = FMR_SPK;

	//Disable end point for preventing pop up noise.
	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_1);
	val &= ~(WM8994_SPKOUTL_ENA_MASK);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_3);	//03 : 0100
	val &= ~(WM8994_SPKRVOL_ENA_MASK | WM8994_SPKLVOL_ENA_MASK);
	val |= (WM8994_SPKLVOL_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, val);

	if(wm8994->testmode_config_flag == SEC_NORMAL)
	{	
		// Unmute the SPKMIXVOLUME
		val = wm8994_read(codec, WM8994_SPKMIXL_ATTENUATION);
		val &= ~(WM8994_SPKMIXL_VOL_MASK);
		val |= TUNING_SPKMIXL_ATTEN;	
		wm8994_write(codec, WM8994_SPKMIXL_ATTENUATION, val);
			
		val = wm8994_read(codec, WM8994_SPKMIXR_ATTENUATION);
		val &= ~(WM8994_SPKMIXR_VOL_MASK);
		wm8994_write(codec, WM8994_SPKMIXR_ATTENUATION, val);
	
		val = wm8994_read(codec, WM8994_SPEAKER_VOLUME_LEFT);
		val &= ~(WM8994_SPKOUTL_MUTE_N_MASK | WM8994_SPKOUTL_VOL_MASK);
		val |= (WM8994_SPKOUT_VU | WM8994_SPKOUTL_MUTE_N | TUNING_FMRADIO_SPKL_VOL);
		wm8994_write(codec, WM8994_SPEAKER_VOLUME_LEFT, val);
	
		val = wm8994_read(codec, WM8994_SPEAKER_VOLUME_RIGHT);
		val &= ~(WM8994_SPKOUTR_MUTE_N_MASK | WM8994_SPKOUTR_VOL_MASK);
		wm8994_write(codec, WM8994_SPEAKER_VOLUME_RIGHT, val);
	
		val = wm8994_read(codec, WM8994_CLASSD);
		val &= ~(WM8994_SPKOUTL_BOOST_MASK);
		val |= TUNING_FMRADIO_CLASSD_VOL << WM8994_SPKOUTL_BOOST_SHIFT;
		wm8994_write(codec, WM8994_CLASSD, val);
		
		val = wm8994_read(codec, WM8994_AIF1_DAC1_LEFT_VOLUME);
		val &= ~(WM8994_AIF1DAC1L_VOL_MASK);
		val |= (WM8994_AIF1DAC1_VU | TUNING_DAC1L_VOL);
		wm8994_write(codec, WM8994_AIF1_DAC1_LEFT_VOLUME, val);

		val = wm8994_read(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME);
		val &= ~(WM8994_AIF1DAC1R_VOL_MASK);
		val |= (WM8994_AIF1DAC1_VU |TUNING_DAC1R_VOL);
		wm8994_write(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME, val);		
	}

	wm8994_set_fmradio_common(codec, 1);

	wm8994_write(codec, WM8994_GPIO_3, 0x0100);	//702
	wm8994_write(codec, WM8994_GPIO_4, 0x0100);	//703
	wm8994_write(codec, WM8994_GPIO_5, 0x8100);	//704
	wm8994_write(codec, WM8994_GPIO_6, 0xA101);	//705
	wm8994_write(codec, WM8994_GPIO_7, 0x0100);	//706

	/*Output MIxer-Output PGA*/
	val = wm8994_read(codec,WM8994_SPKOUT_MIXERS );
	val &= ~(WM8994_SPKMIXL_TO_SPKOUTL_MASK | 
		WM8994_SPKMIXR_TO_SPKOUTL_MASK | WM8994_SPKMIXR_TO_SPKOUTR_MASK);
	val |= (WM8994_SPKMIXL_TO_SPKOUTL);
	wm8994_write(codec, WM8994_SPKOUT_MIXERS, val);

	// Output mixer setting
	val = wm8994_read(codec, WM8994_SPEAKER_MIXER);
	val &= ~(WM8994_MIXINL_TO_SPKMIXL_MASK | WM8994_MIXINR_TO_SPKMIXR_MASK | WM8994_DAC1L_TO_SPKMIXL_MASK | WM8994_DAC1R_TO_SPKMIXR_MASK);
	val |= (WM8994_DAC1L_TO_SPKMIXL);
	wm8994_write(codec, WM8994_SPEAKER_MIXER, val);

	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_5);	//05 : 3303
	val &= ~(WM8994_AIF2DACL_ENA_MASK | WM8994_AIF2DACR_ENA_MASK | WM8994_AIF1DAC1L_ENA_MASK | WM8994_AIF1DAC1R_ENA_MASK
		 | WM8994_DAC1L_ENA_MASK | WM8994_DAC1R_ENA_MASK);
	val |= (WM8994_AIF2DACL_ENA | WM8994_AIF2DACR_ENA | WM8994_AIF1DAC1L_ENA | WM8994_AIF1DAC1R_ENA
		 | WM8994_DAC1L_ENA | WM8994_DAC1R_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_5, val);

	// Enable IN2 and MIXIN - Temporary inserted for blocking MIC and FM radio mixing - DW Shim 2010.03.04
//	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_2);
//	val &= ~(WM8994_TSHUT_ENA_MASK | WM8994_TSHUT_OPDIS_MASK | WM8994_OPCLK_ENA_MASK | 
//			WM8994_MIXINL_ENA_MASK | WM8994_MIXINR_ENA_MASK | WM8994_IN2L_ENA_MASK | WM8994_IN2R_ENA_MASK);
//	val |= (WM8994_TSHUT_ENA | WM8994_TSHUT_OPDIS | WM8994_OPCLK_ENA | WM8994_MIXINL_ENA | 
//			WM8994_MIXINR_ENA | WM8994_IN2L_ENA | WM8994_IN2R_ENA);
	val = (WM8994_TSHUT_ENA | WM8994_TSHUT_OPDIS | WM8994_OPCLK_ENA | WM8994_MIXINL_ENA | 
			WM8994_MIXINR_ENA | WM8994_IN2L_ENA | WM8994_IN2R_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_2, val);

	//AIF2 clock source <- FLL1
	val = wm8994_read(codec, WM8994_AIF2_CLOCKING_1);	//204H : 0x0011
	val &= ~(WM8994_AIF2CLK_ENA_MASK | WM8994_AIF2CLK_SRC_MASK);
	val |= (WM8994_AIF2CLK_ENA | 0x2 << WM8994_AIF2CLK_SRC_SHIFT);
	wm8994_write(codec, WM8994_AIF2_CLOCKING_1, val);

	val = wm8994_read(codec, WM8994_CLOCKING_1);	//208H : 0xF -> 0xE
	val &= ~(WM8994_SYSCLK_SRC_MASK | WM8994_DSP_FSINTCLK_ENA_MASK | WM8994_DSP_FS2CLK_ENA_MASK | WM8994_DSP_FS1CLK_ENA_MASK);
	val |= (WM8994_DSP_FS1CLK_ENA | WM8994_DSP_FS2CLK_ENA | WM8994_DSP_FSINTCLK_ENA);
	wm8994_write(codec, WM8994_CLOCKING_1, val);

	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_4);	//04H
	val &= ~(WM8994_AIF2ADCL_ENA_MASK | WM8994_AIF2ADCR_ENA_MASK | WM8994_ADCL_ENA_MASK | WM8994_ADCR_ENA_MASK);
	val |= (WM8994_AIF2ADCL_ENA | WM8994_AIF2ADCR_ENA | WM8994_ADCL_ENA | WM8994_ADCR_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_4, val);

	//DAC2 Setting
	//ADCR_TO_DAC2 vol, ADCL_TO_DAC2 vol
	wm8994_write(codec, WM8994_DAC2_MIXER_VOLUMES, 0x018C); //603H : 0x018C
	
	//ADCL_TO_DAC2L
	val = wm8994_read(codec, WM8994_DAC2_LEFT_MIXER_ROUTING);	//604H : 0x0010
	val &= ~(WM8994_ADC1_TO_DAC2L_MASK);
	val |= (WM8994_ADC1_TO_DAC2L);
	wm8994_write(codec, WM8994_DAC2_LEFT_MIXER_ROUTING, val);

	//ADCR_TO_DAC2R
	val = wm8994_read(codec, WM8994_DAC2_RIGHT_MIXER_ROUTING);	//605H : 0x0010
	val &= ~(WM8994_ADC1_TO_DAC2R_MASK);
	val |= (WM8994_ADC1_TO_DAC2R);
	wm8994_write(codec, WM8994_DAC2_RIGHT_MIXER_ROUTING, val);

	//DAC block volume
	//DAC1, DAC2 Volume Setting
	//Unmute DAC1 left
	val = wm8994_read(codec,WM8994_DAC1_LEFT_VOLUME );	//610H : 1C0
	val &= ~(WM8994_DAC1L_MUTE_MASK | WM8994_DAC1L_VOL_MASK);
	val |= (WM8994_DAC1_VU | TUNING_DAC1L_VOL); 
	wm8994_write(codec,WM8994_DAC1_LEFT_VOLUME ,val);

	//Unmute and volume ctrl RightDAC
	val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME);	//611 : 1C0
	val &= ~(WM8994_DAC1R_MUTE_MASK | WM8994_DAC1R_VOL_MASK);
	val |= (WM8994_DAC1_VU | TUNING_DAC1R_VOL); //0 db volume	
	wm8994_write(codec,WM8994_DAC1_RIGHT_VOLUME,val);

	val = wm8994_read(codec, WM8994_DAC2_LEFT_VOLUME);	//612 : 1C0
	val &= ~(WM8994_DAC2L_MUTE_MASK | WM8994_DAC2L_VOL_MASK);
	val |= (WM8994_DAC2_VU | TUNING_DAC2L_VOL); //0 db volume	
	wm8994_write(codec,WM8994_DAC2_LEFT_VOLUME,val);
	
	val = wm8994_read(codec, WM8994_DAC2_RIGHT_VOLUME);	//613 : 1C0
	val &= ~(WM8994_DAC2R_MUTE_MASK | WM8994_DAC2R_VOL_MASK);
	val |= (WM8994_DAC2_VU | TUNING_DAC2R_VOL); //0 db volume	
	wm8994_write(codec, WM8994_DAC2_RIGHT_VOLUME, val);

	wm8994_write(codec, WM8994_AIF1_DAC1_EQ_GAINS_1, 0x0000);	//480 : 0

//	wm8994_write(codec, 0x01, 0x1003);
	
	wm8994_write(codec, WM8994_OVERSAMPLING, 0x0000);

	//AIF2 Master/Slave , LOOPBACK, AIF2DAC Unmute
	val = wm8994_read(codec, WM8994_AIF2_MASTER_SLAVE); //312 : 7000
	val &= ~(WM8994_AIF2_LRCLK_FRC_MASK | WM8994_AIF2_CLK_FRC_MASK | WM8994_AIF2_MSTR_MASK);
	val |= (WM8994_AIF2_LRCLK_FRC | WM8994_AIF2_CLK_FRC | WM8994_AIF2_MSTR);
	wm8994_write(codec, WM8994_AIF2_MASTER_SLAVE, val);

	val = wm8994_read(codec, WM8994_AIF2_CONTROL_2);	//311 : 4001
	val &= ~(WM8994_AIF2_LOOPBACK_MASK);
	val |= (WM8994_AIF2_LOOPBACK);
	wm8994_write(codec, WM8994_AIF2_CONTROL_2, val);

	val = wm8994_read(codec, WM8994_AIF2_DAC_FILTERS_1);	//520 : 0
	val &= ~(WM8994_AIF2DAC_MUTE_MASK);
	wm8994_write(codec, WM8994_AIF2_DAC_FILTERS_1, val);
	
	//Enbale bias,vmid and Left speaker
	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_1);
	val &= ~(WM8994_BIAS_ENA_MASK | WM8994_VMID_SEL_MASK |WM8994_SPKOUTL_ENA_MASK);
	val |= (WM8994_BIAS_ENA | WM8994_VMID_SEL_NORMAL | WM8994_SPKOUTL_ENA);  
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

	//DAC Routing
	val = wm8994_read(codec, WM8994_DAC1_LEFT_MIXER_ROUTING);	//601H : 0x05
	val &= ~(WM8994_AIF1DAC1L_TO_DAC1L_MASK | WM8994_AIF1DAC2L_TO_DAC1L_MASK | WM8994_AIF2DACL_TO_DAC1L_MASK);
	val |= (WM8994_AIF2DACL_TO_DAC1L | WM8994_AIF1DAC1L_TO_DAC1L);
	wm8994_write(codec, WM8994_DAC1_LEFT_MIXER_ROUTING, val);

	val = wm8994_read(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING);	//602H : 0x05
	val &= ~(WM8994_AIF1DAC1R_TO_DAC1R_MASK | WM8994_AIF1DAC2R_TO_DAC1R_MASK | WM8994_AIF2DACR_TO_DAC1R_MASK);
	val |= (WM8994_AIF1DAC1R_TO_DAC1R | WM8994_AIF2DACR_TO_DAC1R);
	wm8994_write(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING, val);	

	wm8994_write(codec, WM8994_AIF1_DAC1_FILTERS_1, 0x0000);

}

void wm8994_set_fmradio_headset_mix(struct snd_soc_codec *codec)
{	
	struct wm8994_priv *wm8994 = codec->private_data;

	int val;

	DEBUG_LOG("");
	
	if(wm8994->fmradio_path == FMR_SPK)
		wm8994_set_playback_headset(codec);
	else
	{
		// Unmute the AF1DAC1	
		val = wm8994_read(codec, WM8994_AIF1_DAC1_FILTERS_1 );	
		val &= ~(WM8994_AIF1DAC1_MUTE_MASK | WM8994_AIF1DAC1_MONO_MASK);
		val |= WM8994_AIF1DAC1_UNMUTE;
		wm8994_write(codec,WM8994_AIF1_DAC1_FILTERS_1 ,val);
		
		// Enable the Timeslot0 to DAC1L
		val = wm8994_read(codec, WM8994_DAC1_LEFT_MIXER_ROUTING  ); 	
		val &= ~(WM8994_AIF1DAC1L_TO_DAC1L_MASK);
		val |= WM8994_AIF1DAC1L_TO_DAC1L;
		wm8994_write(codec,WM8994_DAC1_LEFT_MIXER_ROUTING ,val);
				
		//Enable the Timeslot0 to DAC1R
		val = wm8994_read(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING  );	
		val &= ~(WM8994_AIF1DAC1R_TO_DAC1R_MASK);
		val |= WM8994_AIF1DAC1R_TO_DAC1R;
		wm8994_write(codec,WM8994_DAC1_RIGHT_MIXER_ROUTING ,val);
		
		// Enable DAC1L to HPOUT1L path
		val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);
		val &=	~(WM8994_DAC1L_TO_HPOUT1L_MASK | WM8994_DAC1L_TO_MIXOUTL_MASK);
		val |= WM8994_DAC1L_TO_MIXOUTL ;	
		wm8994_write(codec,WM8994_OUTPUT_MIXER_1, val);
		
		// Enable DAC1R to HPOUT1R path
		val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);
		val &= ~(WM8994_DAC1R_TO_HPOUT1R_MASK | WM8994_DAC1R_TO_MIXOUTR_MASK);
		val |= WM8994_DAC1R_TO_MIXOUTR;
		wm8994_write(codec, WM8994_OUTPUT_MIXER_2, val);
				
		//Enable Dac1 and DAC2 and the Timeslot0 for AIF1	
		val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_5 );	
		val &= ~(WM8994_DAC1R_ENA_MASK | WM8994_DAC1L_ENA_MASK|WM8994_AIF1DAC1R_ENA_MASK |	WM8994_AIF1DAC1L_ENA_MASK );
		val |= (WM8994_AIF1DAC1L_ENA | WM8994_AIF1DAC1R_ENA  | WM8994_DAC1L_ENA |WM8994_DAC1R_ENA );
		wm8994_write(codec, WM8994_POWER_MANAGEMENT_5 ,val);
				
		//Unmute DAC1 left
		val = wm8994_read(codec,WM8994_DAC1_LEFT_VOLUME );
		val &= ~(WM8994_DAC1L_MUTE_MASK | WM8994_DAC1L_VOL_MASK);
		val |= (WM8994_DAC1_VU | TUNING_DAC1L_VOL); 
		wm8994_write(codec,WM8994_DAC1_LEFT_VOLUME ,val);
		
		//Unmute and volume ctrl RightDAC
		val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME ); 
		val &= ~(WM8994_DAC1R_MUTE_MASK | WM8994_DAC1R_VOL_MASK);
		val |= (WM8994_DAC1_VU | TUNING_DAC1R_VOL); //0 db volume	
		wm8994_write(codec,WM8994_DAC1_RIGHT_VOLUME,val);
		
		val = wm8994_read(codec, WM8994_AIF1_DAC1_LEFT_VOLUME);
		val &= ~(WM8994_AIF1DAC1L_VOL_MASK);
		val |= (WM8994_AIF1DAC1_VU | TUNING_DAC1L_RADIO_VOL);
		wm8994_write(codec, WM8994_AIF1_DAC1_LEFT_VOLUME, val);

		val = wm8994_read(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME);
		val &= ~(WM8994_AIF1DAC1R_VOL_MASK);
		val |= (WM8994_AIF1DAC1_VU |TUNING_DAC1R_RADIO_VOL);
		wm8994_write(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME, val);
	}
}

void wm8994_set_fmradio_speaker_mix(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->private_data;

	int val;
	
	DEBUG_LOG("");

	if(wm8994->fmradio_path == FMR_HP)
		wm8994_set_playback_speaker(codec);
	else
	{
		//Unmute DAC1 left
		val = wm8994_read(codec,WM8994_DAC1_LEFT_VOLUME );
		val &= ~(WM8994_DAC1L_MUTE_MASK | WM8994_DAC1L_VOL_MASK);
		val |= TUNING_DAC1L_VOL; 
		wm8994_write(codec,WM8994_DAC1_LEFT_VOLUME ,val);
	
		//Unmute and volume ctrl RightDAC
		val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME ); 
		val &= ~(WM8994_DAC1R_MUTE_MASK | WM8994_DAC1R_VOL_MASK);
		val |= TUNING_DAC1R_VOL; //0 db volume	
		wm8994_write(codec,WM8994_DAC1_RIGHT_VOLUME,val);

		//Unmute the DAC path
		val = wm8994_read(codec,WM8994_SPEAKER_MIXER);
		val &= ~(WM8994_DAC1L_TO_SPKMIXL_MASK);
		val |= WM8994_DAC1L_TO_SPKMIXL;
		wm8994_write(codec, WM8994_SPEAKER_MIXER, val);

		// Eable DAC1 Left and timeslot left
		val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_5);	
		val &= ~( WM8994_DAC1L_ENA_MASK | WM8994_AIF1DAC1R_ENA_MASK | WM8994_AIF1DAC1L_ENA_MASK);
		val |= (WM8994_AIF1DAC1L_ENA | WM8994_AIF1DAC1R_ENA | WM8994_DAC1L_ENA);
		wm8994_write(codec, WM8994_POWER_MANAGEMENT_5, val);   

		//Unmute
		val = wm8994_read(codec, WM8994_AIF1_DAC1_FILTERS_1);
		val &= ~(WM8994_AIF1DAC1_MUTE_MASK | WM8994_AIF1DAC1_MONO_MASK);
		val |= (WM8994_AIF1DAC1_UNMUTE | WM8994_AIF1DAC1_MONO);
		wm8994_write(codec, WM8994_AIF1_DAC1_FILTERS_1, val);

		//enable timeslot0 to left dac
		val = wm8994_read(codec, WM8994_DAC1_LEFT_MIXER_ROUTING);
		val &= ~(WM8994_AIF1DAC1L_TO_DAC1L_MASK);
		val |= WM8994_AIF1DAC1L_TO_DAC1L;
		wm8994_write(codec, WM8994_DAC1_LEFT_MIXER_ROUTING, val);
	}
	wm8994->fmradio_path = FMR_SPK_MIX;
}

void wm8994_set_fmradio_speaker_headset_mix(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->private_data;

	u16 val;
	
	u16 nReadServo4Val = 0;
	u16 ncompensationResult = 0;
	u16 nServo4Low = 0;
	u16 nServo4High = 0;
	u8  nCompensationResultLow=0;
	u8 nCompensationResultHigh=0;

	DEBUG_LOG("");

	if(wm8994->fmradio_path == FMR_HP)
		wm8994_disable_fmradio_path(codec, FMR_HP);
	else
		wm8994_disable_fmradio_path(codec, FMR_SPK);
	
	wm8994->fmradio_path = FMR_DUAL_MIX;

	wm8994_write(codec, WM8994_GPIO_3, 0x0100);
	wm8994_write(codec, WM8994_GPIO_4, 0x0100);
	wm8994_write(codec, WM8994_GPIO_5, 0x8100);
	wm8994_write(codec, WM8994_GPIO_6, 0xA101);
	wm8994_write(codec, WM8994_GPIO_7, 0x0100);

	// Disable reg sync to MCLK
	val = wm8994_read(codec, WM8994_AIF1_CLOCKING_1); 	
	val &= ~(WM8994_AIF1CLK_ENA_MASK);
	val |= WM8994_AIF1CLK_ENA;
	wm8994_write(codec, WM8994_AIF1_CLOCKING_1, val);


	// Analogue Path Config
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_2  ); 	
	val &= ~(WM8994_MIXINL_ENA_MASK | WM8994_MIXINR_ENA_MASK | WM8994_IN2L_ENA_MASK | WM8994_IN2R_ENA_MASK);
	val |= (WM8994_MIXINL_ENA | WM8994_MIXINR_ENA| WM8994_IN2L_ENA| WM8994_IN2R_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_2 , val );

	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_1); 	
	val &= ~(WM8994_BIAS_ENA_MASK | WM8994_VMID_SEL_NORMAL);
	val |= (WM8994_BIAS_ENA | WM8994_VMID_SEL_NORMAL);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1 , 0x0003);
		
	val = wm8994_read(codec, WM8994_LEFT_OUTPUT_VOLUME); 	
	val = 0x0000;
	wm8994_write(codec, WM8994_LEFT_OUTPUT_VOLUME, val);

	val = wm8994_read(codec, WM8994_RIGHT_OUTPUT_VOLUME); 	
	val = 0x0000;
	wm8994_write(codec, WM8994_RIGHT_OUTPUT_VOLUME, val);

//	wm8994_set_fmradio_common(codec, 1);

	//FLL2 Setting
	val = wm8994_read(codec, WM8994_AIF2_CLOCKING_1);	//204H : 0x0011
	val &= ~(WM8994_AIF2CLK_ENA_MASK | WM8994_AIF2CLK_SRC_MASK);
	val |= (WM8994_AIF2CLK_ENA | 0x2 << WM8994_AIF2CLK_SRC_SHIFT);
	wm8994_write(codec, WM8994_AIF2_CLOCKING_1, val);

	val = wm8994_read(codec, WM8994_CLOCKING_1);	//208H : 0xF -> 0xE
	val &= ~(WM8994_SYSCLK_SRC_MASK | WM8994_DSP_FSINTCLK_ENA_MASK | WM8994_DSP_FS2CLK_ENA_MASK | WM8994_DSP_FS1CLK_ENA_MASK);
	val |= (WM8994_DSP_FS1CLK_ENA | WM8994_DSP_FS2CLK_ENA | WM8994_DSP_FSINTCLK_ENA);
	wm8994_write(codec, WM8994_CLOCKING_1, val);

	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_4);	//04H
	val &= ~(WM8994_AIF2ADCL_ENA_MASK | WM8994_AIF2ADCR_ENA_MASK | WM8994_ADCL_ENA_MASK | WM8994_ADCR_ENA_MASK);
	val |= (WM8994_AIF2ADCL_ENA | WM8994_AIF2ADCR_ENA | WM8994_ADCL_ENA | WM8994_ADCR_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_4, val);

	//DAC2 Setting
	wm8994_write(codec, WM8994_DAC2_MIXER_VOLUMES, 0x018C);	//603H : 0x018C

	val = wm8994_read(codec, WM8994_DAC2_LEFT_MIXER_ROUTING);	//604H : 0x0010
	val &= ~(WM8994_ADC1_TO_DAC2L_MASK);
	val |= (WM8994_ADC1_TO_DAC2L);
	wm8994_write(codec, WM8994_DAC2_LEFT_MIXER_ROUTING, val);

	val = wm8994_read(codec, WM8994_DAC2_RIGHT_MIXER_ROUTING);	//605H : 0x0010
	val &= ~(WM8994_ADC1_TO_DAC2R_MASK);
	val |= (WM8994_ADC1_TO_DAC2R);
	wm8994_write(codec, WM8994_DAC2_RIGHT_MIXER_ROUTING, val);
	
	//DRC for Noise-gate (AIF2)
	wm8994_write(codec, 0x541, 0x0850);
	wm8994_write(codec, 0x542, 0x0800);
	wm8994_write(codec, 0x543, 0x0001);
	wm8994_write(codec, 0x544, 0x0008);
	wm8994_write(codec, 0x540, 0x01BC);

	//DAC1 Setting
	val = wm8994_read(codec, WM8994_DAC1_LEFT_MIXER_ROUTING);	//601H : 0x05
	val &= ~(WM8994_AIF1DAC1L_TO_DAC1L_MASK | WM8994_AIF1DAC2L_TO_DAC1L_MASK | WM8994_AIF2DACL_TO_DAC1L_MASK);
	val |= (WM8994_AIF2DACL_TO_DAC1L | WM8994_AIF1DAC1L_TO_DAC1L);
	wm8994_write(codec, WM8994_DAC1_LEFT_MIXER_ROUTING, val);


	val = wm8994_read(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING);	//602H : 0x05
	val &= ~(WM8994_AIF1DAC1R_TO_DAC1R_MASK | WM8994_AIF1DAC2R_TO_DAC1R_MASK | WM8994_AIF2DACR_TO_DAC1R_MASK);
	val |= (WM8994_AIF1DAC1R_TO_DAC1R | WM8994_AIF2DACR_TO_DAC1R);
	wm8994_write(codec, WM8994_DAC1_RIGHT_MIXER_ROUTING, val);

	//DAC1, DAC2 Volume Setting
	//Unmute DAC1 left
	val = wm8994_read(codec,WM8994_DAC1_LEFT_VOLUME );	//610H : 1C0
	val &= ~(WM8994_DAC1L_MUTE_MASK | WM8994_DAC1L_VOL_MASK);
	val |= (WM8994_DAC1_VU | TUNING_DAC1L_VOL); 
	wm8994_write(codec,WM8994_DAC1_LEFT_VOLUME ,val);

	//Unmute and volume ctrl RightDAC
	val = wm8994_read(codec, WM8994_DAC1_RIGHT_VOLUME);	//611 : 1C0
	val &= ~(WM8994_DAC1R_MUTE_MASK | WM8994_DAC1R_VOL_MASK);
	val |= (WM8994_DAC1_VU | TUNING_DAC1R_VOL); //0 db volume	
	wm8994_write(codec,WM8994_DAC1_RIGHT_VOLUME,val);


	val = wm8994_read(codec, WM8994_DAC2_LEFT_VOLUME);	//612 : 1C0
	val &= ~(WM8994_DAC2L_MUTE_MASK | WM8994_DAC2L_VOL_MASK);
	val |= (WM8994_DAC2_VU | TUNING_DAC2L_VOL); //0 db volume	
	wm8994_write(codec,WM8994_DAC2_LEFT_VOLUME,val);
	
	val = wm8994_read(codec, WM8994_DAC2_RIGHT_VOLUME);	//613 : 1C0
	val &= ~(WM8994_DAC2R_MUTE_MASK | WM8994_DAC2R_VOL_MASK);
	val |= (WM8994_DAC2_VU | TUNING_DAC2R_VOL); //0 db volume	
	wm8994_write(codec, WM8994_DAC2_RIGHT_VOLUME, val);

	wm8994_write(codec, WM8994_AIF1_DAC1_EQ_GAINS_1, 0x0000);	//480 : 0
	 
	//Digital  Mixer setting
	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_5);	//05 : 3303
	val &= ~(WM8994_AIF2DACL_ENA_MASK | WM8994_AIF2DACR_ENA_MASK | WM8994_AIF1DAC1L_ENA_MASK | WM8994_AIF1DAC1R_ENA_MASK
		 | WM8994_DAC1L_ENA_MASK | WM8994_DAC1R_ENA_MASK);
	val |= (WM8994_AIF2DACL_ENA | WM8994_AIF2DACR_ENA | WM8994_AIF1DAC1L_ENA | WM8994_AIF1DAC1R_ENA
		 | WM8994_DAC1L_ENA | WM8994_DAC1R_ENA);
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_5, val);

	val = wm8994_read(codec, WM8994_POWER_MANAGEMENT_3);	//03 : F
	val &= ~(WM8994_MIXOUTLVOL_ENA_MASK | WM8994_MIXOUTRVOL_ENA_MASK | WM8994_MIXOUTL_ENA_MASK | WM8994_MIXOUTR_ENA_MASK
		|WM8994_SPKLVOL_ENA_MASK | WM8994_SPKRVOL_ENA_MASK);
	val |= (WM8994_MIXOUTLVOL_ENA | WM8994_MIXOUTRVOL_ENA | WM8994_MIXOUTL_ENA | WM8994_MIXOUTR_ENA | WM8994_SPKLVOL_ENA);	
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_3, val);

	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_1);	//2D : 1
	val &= ~(WM8994_DAC1L_TO_MIXOUTL_MASK);
	val |= (WM8994_DAC1L_TO_MIXOUTL);
	wm8994_write(codec, WM8994_OUTPUT_MIXER_1, val);

	val = wm8994_read(codec, WM8994_OUTPUT_MIXER_2);	//2E : 1
	val &= ~(WM8994_DAC1R_TO_MIXOUTR_MASK);
	val |= (WM8994_DAC1R_TO_MIXOUTR);	
	wm8994_write(codec, WM8994_OUTPUT_MIXER_2, val);

	val = wm8994_read(codec, WM8994_SPKOUT_MIXERS );
	val &= ~(WM8994_SPKMIXL_TO_SPKOUTL_MASK | 
		WM8994_SPKMIXR_TO_SPKOUTL_MASK | WM8994_SPKMIXR_TO_SPKOUTR_MASK);
	val |= (WM8994_SPKMIXL_TO_SPKOUTL);
	wm8994_write(codec, WM8994_SPKOUT_MIXERS, val);

	//Unmute the DAC path
	val = wm8994_read(codec,WM8994_SPEAKER_MIXER);
	val &= ~(WM8994_DAC1L_TO_SPKMIXL_MASK);
	val |= WM8994_DAC1L_TO_SPKMIXL;
	wm8994_write(codec, WM8994_SPEAKER_MIXER, val);	

	//AIF1 FLL Setting	
	wm8994_write(codec, WM8994_OVERSAMPLING, 0x0000);
	
	//AIF2 Master/Slave , LOOPBACK, AIF2DAC Unmute
	val = wm8994_read(codec, WM8994_AIF2_MASTER_SLAVE);	//312 : 7000
	val &= ~(WM8994_AIF2_LRCLK_FRC_MASK | WM8994_AIF2_CLK_FRC_MASK | WM8994_AIF2_MSTR_MASK);
	val |= (WM8994_AIF2_LRCLK_FRC | WM8994_AIF2_CLK_FRC | WM8994_AIF2_MSTR);
	wm8994_write(codec, WM8994_AIF2_MASTER_SLAVE, val);

	val = wm8994_read(codec, WM8994_AIF2_CONTROL_2);	//311 : 4001
	val &= ~(WM8994_AIF2_LOOPBACK_MASK);
	val |= (WM8994_AIF2_LOOPBACK);
	wm8994_write(codec, WM8994_AIF2_CONTROL_2, val);

	//* Unmutes
	// Output setting
	if(wm8994->testmode_config_flag == SEC_NORMAL)
	{
		val = wm8994_read(codec, WM8994_LEFT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTL_MUTE_N_MASK | WM8994_MIXOUTL_VOL_MASK);
		val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTL_MUTE_N | TUNING_FMRADIO_OPGAL_VOL);
		wm8994_write(codec,WM8994_LEFT_OPGA_VOLUME, val );
	
		val = wm8994_read(codec, WM8994_RIGHT_OPGA_VOLUME);
		val &= ~(WM8994_MIXOUTR_MUTE_N_MASK | WM8994_MIXOUTR_VOL_MASK);
		val |= (WM8994_MIXOUT_VU | WM8994_MIXOUTR_MUTE_N | TUNING_FMRADIO_OPGAR_VOL);
		wm8994_write(codec,WM8994_RIGHT_OPGA_VOLUME, val );
		
		val = wm8994_read(codec, WM8994_LEFT_OUTPUT_VOLUME);
		val &= ~(WM8994_HPOUT1L_MUTE_N_MASK | WM8994_HPOUT1L_VOL_MASK);
		val |= (WM8994_HPOUT1_VU | WM8994_HPOUT1L_MUTE_N | TUNING_FMRADIO_OUTPUTL_VOL);
		wm8994_write(codec, WM8994_LEFT_OUTPUT_VOLUME, val);

		val = wm8994_read(codec, WM8994_RIGHT_OUTPUT_VOLUME);
		val &= ~(WM8994_HPOUT1R_MUTE_N_MASK | WM8994_HPOUT1R_VOL_MASK);
		val |= (WM8994_HPOUT1_VU | WM8994_HPOUT1R_MUTE_N | TUNING_FMRADIO_OUTPUTR_VOL);
		wm8994_write(codec, WM8994_RIGHT_OUTPUT_VOLUME, val);

		val = wm8994_read(codec, WM8994_AIF1_DAC1_LEFT_VOLUME);
		val &= ~(WM8994_AIF1DAC1L_VOL_MASK);
		val |= (WM8994_AIF1DAC1_VU | TUNING_DAC1L_VOL);
		wm8994_write(codec, WM8994_AIF1_DAC1_LEFT_VOLUME, val);

		val = wm8994_read(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME);
		val &= ~(WM8994_AIF1DAC1R_VOL_MASK);
		val |= (WM8994_AIF1DAC1_VU |TUNING_DAC1R_VOL);
		wm8994_write(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME, val);

		val = wm8994_read(codec, WM8994_CLASSD);
		val &= ~(WM8994_SPKOUTL_BOOST_MASK);
		val |= TUNING_FMRADIO_CLASSD_VOL << WM8994_SPKOUTL_BOOST_SHIFT;
		wm8994_write(codec, WM8994_CLASSD, val);		

		val = wm8994_read(codec, WM8994_SPEAKER_VOLUME_LEFT);
		val &= ~(WM8994_SPKOUTL_MUTE_N_MASK | WM8994_SPKOUTL_VOL_MASK);
		val |= (WM8994_SPKOUT_VU | WM8994_SPKOUTL_MUTE_N | TUNING_FMRADIO_SPKL_VOL);
		wm8994_write(codec, WM8994_SPEAKER_VOLUME_LEFT, val);		
	}	
	//* Headset
	wm8994_write(codec, 0x102, 0x0003);
	wm8994_write(codec, 0x56, 0x0003);
	wm8994_write(codec, 0x102, 0x0000);
	wm8994_write(codec, 0x5D, 0x0002);


	//* DC Servo Series Count
	val = 0x03E0;
	wm8994_write(codec, WM8994_DC_SERVO_2, val);

	//* HP first and second stage
	//Enable vmid,bias, hp left and right
	val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_1 );
	val &= ~(WM8994_BIAS_ENA_MASK | WM8994_VMID_SEL_MASK |WM8994_HPOUT1L_ENA_MASK | WM8994_HPOUT1R_ENA_MASK | WM8994_SPKOUTR_ENA_MASK | WM8994_SPKOUTL_ENA_MASK);
	val |= (WM8994_BIAS_ENA | WM8994_VMID_SEL_NORMAL | WM8994_HPOUT1R_ENA | WM8994_HPOUT1L_ENA | WM8994_SPKOUTL_ENA);  
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, val);

	val = (WM8994_HPOUT1L_DLY | WM8994_HPOUT1R_DLY);
	wm8994_write(codec, WM8994_ANALOGUE_HP_1, val);


	//Enable Charge Pump	
	val = wm8994_read(codec, WM8994_CHARGE_PUMP_1);
	val &= ~WM8994_CP_ENA_MASK ;
	val |= WM8994_CP_ENA | WM8994_CP_ENA_DEFAULT ; // this is from wolfson  	
	wm8994_write(codec, WM8994_CHARGE_PUMP_1, 0x9F25);

	msleep(5);

	//* DC Servo 
	val = (WM8994_DCS_TRIG_SERIES_1 | WM8994_DCS_TRIG_SERIES_0 | WM8994_DCS_ENA_CHAN_1 | WM8994_DCS_ENA_CHAN_0);
	wm8994_write(codec,WM8994_DC_SERVO_1, 0x0303 );

	msleep(160);

	nReadServo4Val=wm8994_read(codec,WM8994_DC_SERVO_4);
	nServo4Low=(u8)(nReadServo4Val & 0xff);
	nServo4High=(u8)((nReadServo4Val>>8) & 0xff);

	nCompensationResultLow=((u16)nServo4Low-5)&0x00ff;
	nCompensationResultHigh=(((u16)nServo4High-5)<<8)&0xff00;
	ncompensationResult=nCompensationResultLow|nCompensationResultHigh;
	wm8994_write(codec,WM8994_DC_SERVO_4, ncompensationResult);

	val = (WM8994_DCS_TRIG_DAC_WR_1 | WM8994_DCS_TRIG_DAC_WR_0 | WM8994_DCS_ENA_CHAN_1 | WM8994_DCS_ENA_CHAN_0);
	wm8994_write(codec,WM8994_DC_SERVO_1, val );

	msleep(20);

	//* Headphone Output
		// Intermediate HP settings
	val = wm8994_read(codec, WM8994_ANALOGUE_HP_1); 	
	val &= ~(WM8994_HPOUT1R_DLY_MASK |WM8994_HPOUT1R_OUTP_MASK |WM8994_HPOUT1R_RMV_SHORT_MASK |
		WM8994_HPOUT1L_DLY_MASK |WM8994_HPOUT1L_OUTP_MASK | WM8994_HPOUT1L_RMV_SHORT_MASK);
	val |= (WM8994_HPOUT1L_RMV_SHORT | WM8994_HPOUT1L_OUTP|WM8994_HPOUT1L_DLY |WM8994_HPOUT1R_RMV_SHORT | 
		WM8994_HPOUT1R_OUTP | WM8994_HPOUT1R_DLY);
	wm8994_write(codec, WM8994_ANALOGUE_HP_1, val);

	//DAC1 Unmute
	val = wm8994_read(codec, WM8994_AIF1_DAC1_FILTERS_1);
	val &= ~(WM8994_AIF1DAC1_MUTE_MASK | WM8994_AIF1DAC1_MONO_MASK);
	val |= (WM8994_AIF1DAC1_UNMUTE | WM8994_AIF1DAC1_MONO);
	wm8994_write(codec, WM8994_AIF1_DAC1_FILTERS_1, val);	
	
	val = wm8994_read(codec, WM8994_AIF2_DAC_FILTERS_1);	//520 : 0
	val &= ~(WM8994_AIF2DAC_MUTE_MASK);
	wm8994_write(codec, WM8994_AIF2_DAC_FILTERS_1, val);

}

#if defined WM8994_REGISTER_DUMP
void wm8994_register_dump(struct snd_soc_codec *codec)
{
	int wm8994_register;

	for(wm8994_register = 0; wm8994_register <= 0x6; wm8994_register++)
		DEBUG_LOG_ERR("Register= [%X], Value = [%X]", wm8994_register, wm8994_read(codec, wm8994_register));

	DEBUG_LOG_ERR("Register= [%X], Value = [%X]", 0x15, wm8994_read(codec, 0x15));

	for(wm8994_register = 0x18; wm8994_register <= 0x3C; wm8994_register++)
		DEBUG_LOG_ERR("Register= [%X], Value = [%X]", wm8994_register, wm8994_read(codec, wm8994_register));

	DEBUG_LOG_ERR("Register= [%X], Value = [%X]", 0x4C, wm8994_read(codec, 0x4C));

	for(wm8994_register = 0x51; wm8994_register <= 0x5C; wm8994_register++)
		DEBUG_LOG_ERR("Register= [%X], Value = [%X]", wm8994_register, wm8994_read(codec, wm8994_register));

	DEBUG_LOG_ERR("Register= [%X], Value = [%X]", 0x60, wm8994_read(codec, 0x60));
	DEBUG_LOG_ERR("Register= [%X], Value = [%X]", 0x101, wm8994_read(codec, 0x101));
	DEBUG_LOG_ERR("Register= [%X], Value = [%X]", 0x110, wm8994_read(codec, 0x110));
	DEBUG_LOG_ERR("Register= [%X], Value = [%X]", 0x111, wm8994_read(codec, 0x111));

	for(wm8994_register = 0x200; wm8994_register <= 0x212; wm8994_register++)
		DEBUG_LOG_ERR("Register= [%X], Value = [%X]", wm8994_register, wm8994_read(codec, wm8994_register));
	
	for(wm8994_register = 0x220; wm8994_register <= 0x224; wm8994_register++)
		DEBUG_LOG_ERR("Register= [%X], Value = [%X]", wm8994_register, wm8994_read(codec, wm8994_register));
	
	for(wm8994_register = 0x240; wm8994_register <= 0x244; wm8994_register++)
		DEBUG_LOG_ERR("Register= [%X], Value = [%X]", wm8994_register, wm8994_read(codec, wm8994_register));

	for(wm8994_register = 0x300; wm8994_register <= 0x317; wm8994_register++)
		DEBUG_LOG_ERR("Register= [%X], Value = [%X]", wm8994_register, wm8994_read(codec, wm8994_register));

	for(wm8994_register = 0x400; wm8994_register <= 0x411; wm8994_register++)
		DEBUG_LOG_ERR("Register= [%X], Value = [%X]", wm8994_register, wm8994_read(codec, wm8994_register));

	for(wm8994_register = 0x420; wm8994_register <= 0x423; wm8994_register++)
		DEBUG_LOG_ERR("Register= [%X], Value = [%X]", wm8994_register, wm8994_read(codec, wm8994_register));

	for(wm8994_register = 0x440; wm8994_register <= 0x444; wm8994_register++)
		DEBUG_LOG_ERR("Register= [%X], Value = [%X]", wm8994_register, wm8994_read(codec, wm8994_register));

	for(wm8994_register = 0x450; wm8994_register <= 0x454; wm8994_register++)
		DEBUG_LOG_ERR("Register= [%X], Value = [%X]", wm8994_register, wm8994_read(codec, wm8994_register));

	for(wm8994_register = 0x480; wm8994_register <= 0x493; wm8994_register++)
		DEBUG_LOG_ERR("Register= [%X], Value = [%X]", wm8994_register, wm8994_read(codec, wm8994_register));

	for(wm8994_register = 0x4A0; wm8994_register <= 0x4B3; wm8994_register++)
		DEBUG_LOG_ERR("Register= [%X], Value = [%X]", wm8994_register, wm8994_read(codec, wm8994_register));
	
	for(wm8994_register = 0x500; wm8994_register <= 0x503; wm8994_register++)
		DEBUG_LOG_ERR("Register= [%X], Value = [%X]", wm8994_register, wm8994_read(codec, wm8994_register));
	
	DEBUG_LOG_ERR("Register= [%X], Value = [%X]", 0x510, wm8994_read(codec, 0x510));
	DEBUG_LOG_ERR("Register= [%X], Value = [%X]", 0x520, wm8994_read(codec, 0x520));
	DEBUG_LOG_ERR("Register= [%X], Value = [%X]", 0x521, wm8994_read(codec, 0x521));

	for(wm8994_register = 0x540; wm8994_register <= 0x544; wm8994_register++)
		DEBUG_LOG_ERR("Register= [%X], Value = [%X]", wm8994_register, wm8994_read(codec, wm8994_register));
	
	for(wm8994_register = 0x580; wm8994_register <= 0x593; wm8994_register++)
		DEBUG_LOG_ERR("Register= [%X], Value = [%X]", wm8994_register, wm8994_read(codec, wm8994_register));

	for(wm8994_register = 0x600; wm8994_register <= 0x614; wm8994_register++)
		DEBUG_LOG_ERR("Register= [%X], Value = [%X]", wm8994_register, wm8994_read(codec, wm8994_register));
	
	DEBUG_LOG_ERR("Register= [%X], Value = [%X]", 0x620, wm8994_read(codec, 0x620));
	DEBUG_LOG_ERR("Register= [%X], Value = [%X]", 0x621, wm8994_read(codec, 0x621));

	for(wm8994_register = 0x700; wm8994_register <= 0x70A; wm8994_register++)
		DEBUG_LOG_ERR("Register= [%X], Value = [%X]", wm8994_register, wm8994_read(codec, wm8994_register));
		
}
#endif
