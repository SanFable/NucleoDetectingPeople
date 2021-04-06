/**
 ******************************************************************************
 * @file    Vl53l1x_Class.cpp
 * @author  JS
 * @version V0.0.1
 * @date    15-January-2019
 * @brief   Implementation file for the VL53L1 sensor component driver class
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2019 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
*/

/* Includes */
#include <stdlib.h>
#include "VL53L1X_Class.h"


#define ALGO__PART_TO_PART_RANGE_OFFSET_MM  0x001E
#define MM_CONFIG__INNER_OFFSET_MM          0x0020
#define MM_CONFIG__OUTER_OFFSET_MM          0x0022

#include "vl53l1x_configuration.h"


VL53L1X_ERROR VL53L1X::vl53l1x_get_sw_version(VL53L1X_Version_t *pVersion)
{
    VL53L1X_ERROR Status = 0;

    pVersion->major = VL53L1X_IMPLEMENTATION_VER_MAJOR;
    pVersion->minor = VL53L1X_IMPLEMENTATION_VER_MINOR;
    pVersion->build = VL53L1X_IMPLEMENTATION_VER_SUB;
    pVersion->revision = VL53L1X_IMPLEMENTATION_VER_REVISION;
    return Status;
}

VL53L1X_ERROR VL53L1X::vl53l1x_set_i2c_address(uint8_t new_address)
{
    VL53L1X_ERROR status = 0;

    status = vl53l1_wr_byte(Device, VL53L1_I2C_SLAVE__DEVICE_ADDRESS, new_address >> 1);
    Device->I2cDevAddr = new_address;
    return status;
}


VL53L1X_ERROR VL53L1X::vl53l1x_sensor_init()
{
    VL53L1X_ERROR status = 0;
    uint8_t Addr = 0x00;

    for (Addr = 0x2D; Addr <= 0x87; Addr++){
        status = vl53l1_wr_byte(Device, Addr, VL51L1X_DEFAULT_CONFIGURATION[Addr - 0x2D]);
        if (status != 0)
        {
            printf("Writing config failed - %d\r\n", status);
        }
    }
    
    uint16_t sensorID= 0;
    status = vl53l1x_get_sensor_id(&sensorID);
    printf("Sensor id is - %d (%X)\r\n", sensorID, sensorID);
    
    status = vl53l1x_start_ranging();
    if (status != 0)
    {
        printf("start ranging failed - %d\r\n", status);
    }
 
    status = vl53l1x_clear_interrupt();
    status = vl53l1x_stop_ranging();
    status = vl53l1_wr_byte(Device, VL53L1_VHV_CONFIG__TIMEOUT_MACROP_LOOP_BOUND, 0x09); /* two bounds VHV */
    status = vl53l1_wr_byte(Device, 0x0B, 0); /* start VHV from the previous temperature */
    return status;
}


VL53L1X_ERROR VL53L1X::vl53l1x_clear_interrupt()
{
    VL53L1X_ERROR status = 0;

    status = vl53l1_wr_byte(Device, SYSTEM__INTERRUPT_CLEAR, 0x01);
    return status;
}


VL53L1X_ERROR VL53L1X::vl53l1x_set_interrupt_polarity(uint8_t NewPolarity)
{
    uint8_t Temp;
    VL53L1X_ERROR status = 0;

    status = vl53l1_rd_byte(Device, GPIO_HV_MUX__CTRL, &Temp);
    Temp = Temp & 0xEF;
    status = vl53l1_wr_byte(Device, GPIO_HV_MUX__CTRL, Temp | (!(NewPolarity & 1)) << 4);
    return status;
}



VL53L1X_ERROR VL53L1X::vl53l1x_get_interrupt_polarity(uint8_t *pInterruptPolarity)
{
    uint8_t Temp;
    VL53L1X_ERROR status = 0;

    status = vl53l1_rd_byte(Device, GPIO_HV_MUX__CTRL, &Temp);
    Temp = Temp & 0x10;
    *pInterruptPolarity = !(Temp>>4);
    return status;
}



VL53L1X_ERROR VL53L1X::vl53l1x_start_ranging()
{
    VL53L1X_ERROR status = 0;

    status = vl53l1_wr_byte(Device, SYSTEM__MODE_START, 0x40);   /* Enable VL53L1X */
    return status;
}

VL53L1X_ERROR VL53L1X::vl53l1x_stop_ranging()
{
    VL53L1X_ERROR status = 0;

    status = vl53l1_wr_byte(Device, SYSTEM__MODE_START, 0x00);   /* Disable VL53L1X */
    return status;
}



VL53L1X_ERROR VL53L1X::vl53l1x_check_for_data_ready(uint8_t *isDataReady)
{
    uint8_t Temp;
    uint8_t IntPol;
    VL53L1X_ERROR status = 0;

    status = vl53l1x_get_interrupt_polarity(&IntPol);
    status = vl53l1_rd_byte(Device, GPIO__TIO_HV_STATUS, &Temp);
    /* Read in the register to check if a new value is available */
    if (status == 0){
        if ((Temp & 1) == IntPol)
            *isDataReady = 1;
        else
            *isDataReady = 0;
    }
    return status;
}


VL53L1X_ERROR VL53L1X::vl53l1x_set_timing_budget_in_ms(uint16_t TimingBudgetInMs)
{
    uint16_t DM;
    VL53L1X_ERROR  status=0;

    status = vl53l1x_get_distance_mode(&DM);
    if (DM == 0)
        return 1;
    else if (DM == 1) { /* Short DistanceMode */
        switch (TimingBudgetInMs) {
        case 15: /* only available in short distance mode */
            vl53l1_wr_word(Device, RANGE_CONFIG__TIMEOUT_MACROP_A_HI,
                    0x01D);
            vl53l1_wr_word(Device, RANGE_CONFIG__TIMEOUT_MACROP_B_HI,
                    0x0027);
            break;
        case 20:
            vl53l1_wr_word(Device, RANGE_CONFIG__TIMEOUT_MACROP_A_HI,
                    0x0051);
            vl53l1_wr_word(Device, RANGE_CONFIG__TIMEOUT_MACROP_B_HI,
                    0x006E);
            break;
        case 33:
            vl53l1_wr_word(Device, RANGE_CONFIG__TIMEOUT_MACROP_A_HI,
                    0x00D6);
            vl53l1_wr_word(Device, RANGE_CONFIG__TIMEOUT_MACROP_B_HI,
                    0x006E);
            break;
        case 50:
            vl53l1_wr_word(Device, RANGE_CONFIG__TIMEOUT_MACROP_A_HI,
                    0x1AE);
            vl53l1_wr_word(Device, RANGE_CONFIG__TIMEOUT_MACROP_B_HI,
                    0x01E8);
            break;
        case 100:
            vl53l1_wr_word(Device, RANGE_CONFIG__TIMEOUT_MACROP_A_HI,
                    0x02E1);
            vl53l1_wr_word(Device, RANGE_CONFIG__TIMEOUT_MACROP_B_HI,
                    0x0388);
            break;
        case 200:
            vl53l1_wr_word(Device, RANGE_CONFIG__TIMEOUT_MACROP_A_HI,
                    0x03E1);
            vl53l1_wr_word(Device, RANGE_CONFIG__TIMEOUT_MACROP_B_HI,
                    0x0496);
            break;
        case 500:
            vl53l1_wr_word(Device, RANGE_CONFIG__TIMEOUT_MACROP_A_HI,
                    0x0591);
            vl53l1_wr_word(Device, RANGE_CONFIG__TIMEOUT_MACROP_B_HI,
                    0x05C1);
            break;
        default:
            status = 1;
            break;
        }
    } else {
        switch (TimingBudgetInMs) {
        case 20:
            vl53l1_wr_word(Device, RANGE_CONFIG__TIMEOUT_MACROP_A_HI,
                    0x001E);
            vl53l1_wr_word(Device, RANGE_CONFIG__TIMEOUT_MACROP_B_HI,
                    0x0022);
            break;
        case 33:
            vl53l1_wr_word(Device, RANGE_CONFIG__TIMEOUT_MACROP_A_HI,
                    0x0060);
            vl53l1_wr_word(Device, RANGE_CONFIG__TIMEOUT_MACROP_B_HI,
                    0x006E);
            break;
        case 50:
            vl53l1_wr_word(Device, RANGE_CONFIG__TIMEOUT_MACROP_A_HI,
                    0x00AD);
            vl53l1_wr_word(Device, RANGE_CONFIG__TIMEOUT_MACROP_B_HI,
                    0x00C6);
            break;
        case 100:
            vl53l1_wr_word(Device, RANGE_CONFIG__TIMEOUT_MACROP_A_HI,
                    0x01CC);
            vl53l1_wr_word(Device, RANGE_CONFIG__TIMEOUT_MACROP_B_HI,
                    0x01EA);
            break;
        case 200:
            vl53l1_wr_word(Device, RANGE_CONFIG__TIMEOUT_MACROP_A_HI,
                    0x02D9);
            vl53l1_wr_word(Device, RANGE_CONFIG__TIMEOUT_MACROP_B_HI,
                    0x02F8);
            break;
        case 500:
            vl53l1_wr_word(Device, RANGE_CONFIG__TIMEOUT_MACROP_A_HI,
                    0x048F);
            vl53l1_wr_word(Device, RANGE_CONFIG__TIMEOUT_MACROP_B_HI,
                    0x04A4);
            break;
        default:
            status = 1;
            break;
        }
    }
    return status;
}

VL53L1X_ERROR VL53L1X::vl53l1x_get_timing_budget_in_ms(uint16_t *pTimingBudget)
{
    uint16_t Temp;
    VL53L1X_ERROR status = 0;

    status = vl53l1_rd_word(Device, RANGE_CONFIG__TIMEOUT_MACROP_A_HI, &Temp);
    switch (Temp) {
        case 0x001D :
            *pTimingBudget = 15;
            break;
        case 0x0051 :
        case 0x001E :
            *pTimingBudget = 20;
            break;
        case 0x00D6 :
        case 0x0060 :
            *pTimingBudget = 33;
            break;
        case 0x1AE :
        case 0x00AD :
            *pTimingBudget = 50;
            break;
        case 0x02E1 :
        case 0x01CC :
            *pTimingBudget = 100;
            break;
        case 0x03E1 :
        case 0x02D9 :
            *pTimingBudget = 200;
            break;
        case 0x0591 :
        case 0x048F :
            *pTimingBudget = 500;
            break;
        default:
            *pTimingBudget = 0;
            break;
    }
    return status;
}


VL53L1X_ERROR VL53L1X::vl53l1x_set_distance_mode(uint16_t DM)
{
    uint16_t TB;
    VL53L1X_ERROR status = 0;

    status = vl53l1x_get_timing_budget_in_ms(&TB);
    
    
    switch (DM) {
    case 1:
        status = vl53l1_wr_byte(Device, PHASECAL_CONFIG__TIMEOUT_MACROP, 0x14);
        status = vl53l1_wr_byte(Device, RANGE_CONFIG__VCSEL_PERIOD_A, 0x07);
        status = vl53l1_wr_byte(Device, RANGE_CONFIG__VCSEL_PERIOD_B, 0x05);
        status = vl53l1_wr_byte(Device, RANGE_CONFIG__VALID_PHASE_HIGH, 0x38);
        status = vl53l1_wr_word(Device, SD_CONFIG__WOI_SD0, 0x0705);
        status = vl53l1_wr_word(Device, SD_CONFIG__INITIAL_PHASE_SD0, 0x0606);
        break;
    case 2:
        status = vl53l1_wr_byte(Device, PHASECAL_CONFIG__TIMEOUT_MACROP, 0x0A);
        status = vl53l1_wr_byte(Device, RANGE_CONFIG__VCSEL_PERIOD_A, 0x0F);
        status = vl53l1_wr_byte(Device, RANGE_CONFIG__VCSEL_PERIOD_B, 0x0D);
        status = vl53l1_wr_byte(Device, RANGE_CONFIG__VALID_PHASE_HIGH, 0xB8);
        status = vl53l1_wr_word(Device, SD_CONFIG__WOI_SD0, 0x0F0D);
        status = vl53l1_wr_word(Device, SD_CONFIG__INITIAL_PHASE_SD0, 0x0E0E);
        break;
    default:
        break;
    }
    status = vl53l1x_set_timing_budget_in_ms(TB);
    return status;
}




VL53L1X_ERROR VL53L1X::vl53l1x_get_distance_mode(uint16_t *DM)
{
    uint8_t TempDM, status=0;

    status = vl53l1_rd_byte(Device,PHASECAL_CONFIG__TIMEOUT_MACROP, &TempDM);
    if (TempDM == 0x14)
        *DM=1;
    if(TempDM == 0x0A)
        *DM=2;
    return status;
}



VL53L1X_ERROR VL53L1X::vl53l1x_set_inter_measurement_in_ms(uint16_t InterMeasMs)
{
    uint16_t ClockPLL;
    VL53L1X_ERROR status = 0;

    status = vl53l1_rd_word(Device, VL53L1_RESULT__OSC_CALIBRATE_VAL, &ClockPLL);
    ClockPLL = ClockPLL&0x3FF;
    vl53l1_wr_double_word(Device, VL53L1_SYSTEM__INTERMEASUREMENT_PERIOD,
            (uint32_t)(ClockPLL * InterMeasMs * 1.075));
    return status;

}


VL53L1X_ERROR VL53L1X::vl53l1x_get_inter_measurement_in_ms(uint16_t *pIM)
{
    uint16_t ClockPLL;
    VL53L1X_ERROR status = 0;
    uint32_t tmp;

    status = vl53l1_rd_double_word(Device,VL53L1_SYSTEM__INTERMEASUREMENT_PERIOD, &tmp);
    *pIM = (uint16_t)tmp;
    status = vl53l1_rd_word(Device, VL53L1_RESULT__OSC_CALIBRATE_VAL, &ClockPLL);
    ClockPLL = ClockPLL&0x3FF;
    *pIM= (uint16_t)(*pIM/(ClockPLL*1.065));
    return status;
}


VL53L1X_ERROR VL53L1X::vl53l1x_boot_state(uint8_t *state)
{
    VL53L1X_ERROR status = 0;
    uint8_t tmp = 0;

    status = vl53l1_rd_byte(Device,VL53L1_FIRMWARE__SYSTEM_STATUS, &tmp);
    *state = tmp;
    return status;
}


VL53L1X_ERROR VL53L1X::vl53l1x_get_sensor_id(uint16_t *sensorId)
{
    VL53L1X_ERROR status = 0;
    uint16_t tmp = 0;

    status = vl53l1_rd_word(Device, VL53L1_IDENTIFICATION__MODEL_ID, &tmp);
    *sensorId = tmp;
    return status;
}


VL53L1X_ERROR VL53L1X::vl53l1x_get_distance(uint16_t *distance)
{
    VL53L1X_ERROR status = 0;
    uint16_t tmp;

    status = (vl53l1_rd_word(Device,
            VL53L1_RESULT__FINAL_CROSSTALK_CORRECTED_RANGE_MM_SD0, &tmp));
    *distance = tmp;
    return status;
}

VL53L1X_ERROR VL53L1X::vl53l1x_get_signal_per_spad(uint16_t *signalRate)
{
    VL53L1X_ERROR status = 0;
    uint16_t SpNb=1, signal;

    status = vl53l1_rd_word(Device,
        VL53L1_RESULT__PEAK_SIGNAL_COUNT_RATE_CROSSTALK_CORRECTED_MCPS_SD0, &signal);
    status = vl53l1_rd_word(Device,
        VL53L1_RESULT__DSS_ACTUAL_EFFECTIVE_SPADS_SD0, &SpNb);
    *signalRate = (uint16_t) (2000.0*signal/SpNb);
    return status;
}


VL53L1X_ERROR VL53L1X::vl53l1x_get_ambient_per_spad(uint16_t *ambPerSp)
{
    VL53L1X_ERROR status=0;
    uint16_t AmbientRate, SpNb=1;

    status = vl53l1_rd_word(Device, RESULT__AMBIENT_COUNT_RATE_MCPS_SD, &AmbientRate);
    status = vl53l1_rd_word(Device, VL53L1_RESULT__DSS_ACTUAL_EFFECTIVE_SPADS_SD0, &SpNb);
    *ambPerSp=(uint16_t) (2000.0 * AmbientRate / SpNb);
    return status;
}


VL53L1X_ERROR VL53L1X::vl53l1x_get_signal_rate(uint16_t *signal)
{
    VL53L1X_ERROR status = 0;
    uint16_t tmp;

    status = vl53l1_rd_word(Device,
        VL53L1_RESULT__PEAK_SIGNAL_COUNT_RATE_CROSSTALK_CORRECTED_MCPS_SD0, &tmp);
    *signal = tmp*8;
    return status;
}


VL53L1X_ERROR VL53L1X::vl53l1x_get_spad_nb(uint16_t *spNb)
{
    VL53L1X_ERROR status = 0;
    uint16_t tmp;

    status = vl53l1_rd_word(Device,
                  VL53L1_RESULT__DSS_ACTUAL_EFFECTIVE_SPADS_SD0, &tmp);
    *spNb = tmp >> 8;
    return status;
}


VL53L1X_ERROR VL53L1X::vl53l1x_get_ambient_rate(uint16_t *ambRate)
{
    VL53L1X_ERROR status = 0;
    uint16_t tmp;

    status = vl53l1_rd_word(Device, RESULT__AMBIENT_COUNT_RATE_MCPS_SD, &tmp);
    *ambRate = tmp*8;
    return status;
}


VL53L1X_ERROR VL53L1X::vl53l1x_get_range_status(uint8_t *rangeStatus)
{
    VL53L1X_ERROR status = 0;
    uint8_t RgSt;

    status = vl53l1_rd_byte(Device, VL53L1_RESULT__RANGE_STATUS, &RgSt);
    RgSt = RgSt&0x1F;
    switch (RgSt) {
    case 9:
        RgSt = 0;
        break;
    case 6:
        RgSt = 1;
        break;
    case 4:
        RgSt = 2;
        break;
    case 8:
        RgSt = 3;
        break;
    case 5:
        RgSt = 4;
        break;
    case 3:
        RgSt = 5;
        break;
    case 19:
        RgSt = 6;
        break;
    case 7:
        RgSt = 7;
        break;
    case 12:
        RgSt = 9;
        break;
    case 18:
        RgSt = 10;
        break;
    case 22:
        RgSt = 11;
        break;
    case 23:
        RgSt = 12;
        break;
    case 13:
        RgSt = 13;
        break;
    default:
        RgSt = 255;
        break;
    }
    *rangeStatus = RgSt;
    return status;
}


VL53L1X_ERROR VL53L1X::vl53l1x_set_offset(int16_t OffsetValue)
{
    VL53L1X_ERROR status = 0;
    int16_t Temp;

    Temp = (OffsetValue*4);
    vl53l1_wr_word(Device, ALGO__PART_TO_PART_RANGE_OFFSET_MM,
            (uint16_t)Temp);
    vl53l1_wr_word(Device, MM_CONFIG__INNER_OFFSET_MM, 0x0);
    vl53l1_wr_word(Device, MM_CONFIG__OUTER_OFFSET_MM, 0x0);
    return status;
}


VL53L1X_ERROR  VL53L1X::vl53l1x_get_offset(int16_t *offset)
{
    VL53L1X_ERROR status = 0;
    uint16_t Temp;

    status = vl53l1_rd_word(Device,ALGO__PART_TO_PART_RANGE_OFFSET_MM, &Temp);
    Temp = Temp<<3;
    Temp = Temp >>5;
    *offset = (int16_t)(Temp);
    return status;
}

VL53L1X_ERROR VL53L1X::vl53l1x_set_xtalk(uint16_t XtalkValue)
{
    /* XTalkValue in count per second to avoid float type */
    VL53L1X_ERROR status = 0;

    status = vl53l1_wr_word(Device,
            ALGO__CROSSTALK_COMPENSATION_X_PLANE_GRADIENT_KCPS,
            0x0000);
    status = vl53l1_wr_word(Device, ALGO__CROSSTALK_COMPENSATION_Y_PLANE_GRADIENT_KCPS,
            0x0000);
    status = vl53l1_wr_word(Device, ALGO__CROSSTALK_COMPENSATION_PLANE_OFFSET_KCPS,
            (XtalkValue<<9)/1000); /* * << 9 (7.9 format) and /1000 to convert cps to kpcs */
    return status;
}


VL53L1X_ERROR VL53L1X::vl53l1x_get_xtalk(uint16_t *xtalk )
{
    VL53L1X_ERROR status = 0;
    uint16_t tmp;

    status = vl53l1_rd_word(Device,ALGO__CROSSTALK_COMPENSATION_PLANE_OFFSET_KCPS, &tmp);
    *xtalk = (tmp*1000)>>9; /* * 1000 to convert kcps to cps and >> 9 (7.9 format) */
    return status;
}


VL53L1X_ERROR VL53L1X::vl53l1x_set_distance_threshold(uint16_t ThreshLow,
                  uint16_t ThreshHigh, uint8_t Window,
                  uint8_t IntOnNoTarget)
{
    VL53L1X_ERROR status = 0;
    uint8_t Temp = 0;

    status = vl53l1_rd_byte(Device, SYSTEM__INTERRUPT_CONFIG_GPIO, &Temp);
    Temp = Temp & 0x47;
    if (IntOnNoTarget == 0) {
        status = vl53l1_wr_byte(Device, SYSTEM__INTERRUPT_CONFIG_GPIO,
                   (Temp | (Window & 0x07)));
    } else {
        status = vl53l1_wr_byte(Device, SYSTEM__INTERRUPT_CONFIG_GPIO,
                   ((Temp | (Window & 0x07)) | 0x40));
    }
    status = vl53l1_wr_word(Device, SYSTEM__THRESH_HIGH, ThreshHigh);
    status = vl53l1_wr_word(Device, SYSTEM__THRESH_LOW, ThreshLow);
    return status;
}


VL53L1X_ERROR VL53L1X::vl53l1x_get_distance_threshold_window(uint16_t *window)
{
    VL53L1X_ERROR status = 0;
    uint8_t tmp;
    status = vl53l1_rd_byte(Device,SYSTEM__INTERRUPT_CONFIG_GPIO, &tmp);
    *window = (uint16_t)(tmp & 0x7);
    return status;
}


VL53L1X_ERROR VL53L1X::vl53l1x_get_distance_threshold_low(uint16_t *low)
{
    VL53L1X_ERROR status = 0;
    uint16_t tmp;

    status = vl53l1_rd_word(Device,SYSTEM__THRESH_LOW, &tmp);
    *low = tmp;
    return status;
}

VL53L1X_ERROR VL53L1X::vl53l1x_get_distance_threshold_high(uint16_t *high)
{
    VL53L1X_ERROR status = 0;
    uint16_t tmp;

    status = vl53l1_rd_word(Device,SYSTEM__THRESH_HIGH, &tmp);
    *high = tmp;
    return status;
}

VL53L1X_ERROR VL53L1X::VL53L1X_SetROICenter(uint8_t ROICenter)
{
 VL53L1X_ERROR status = 0;
 status = vl53l1_wr_byte(Device, ROI_CONFIG__USER_ROI_CENTRE_SPAD, ROICenter);
 return status;
}

VL53L1X_ERROR VL53L1X::vl53l1x_set_roi(uint16_t X, uint16_t Y)
{
    uint8_t OpticalCenter;
    VL53L1X_ERROR status = 0;
 
    status =vl53l1_rd_byte(Device, VL53L1_ROI_CONFIG__MODE_ROI_CENTRE_SPAD, &OpticalCenter);
    if (X > 16)
        X = 16;
    if (Y > 16)
        Y = 16;
    if (X > 10 || Y > 10){
        OpticalCenter = 199;
    }
    status = vl53l1_wr_byte(Device, ROI_CONFIG__USER_ROI_CENTRE_SPAD, OpticalCenter);
    status = vl53l1_wr_byte(Device, ROI_CONFIG__USER_ROI_REQUESTED_GLOBAL_XY_SIZE,
               (Y - 1) << 4 | (X - 1));
    return status;
}

VL53L1X_ERROR VL53L1X::vl53l1x_get_roi_xy(uint16_t *ROI_X, uint16_t *ROI_Y)
{
    VL53L1X_ERROR status = 0;
    uint8_t tmp;

    status = vl53l1_rd_byte(Device,ROI_CONFIG__USER_ROI_REQUESTED_GLOBAL_XY_SIZE, &tmp);
    *ROI_X = ((uint16_t)tmp & 0x0F) + 1;
    *ROI_Y = (((uint16_t)tmp & 0xF0) >> 4) + 1;
    return status;
}

VL53L1X_ERROR VL53L1X::vl53l1x_set_signal_threshold(uint16_t Signal)
{
    VL53L1X_ERROR status = 0;

    vl53l1_wr_word(Device,RANGE_CONFIG__MIN_COUNT_RATE_RTN_LIMIT_MCPS,Signal>>3);
    return status;
}

VL53L1X_ERROR VL53L1X::vl53l1x_get_signal_threshold(uint16_t *signal)
{
    VL53L1X_ERROR status = 0;
    uint16_t tmp;

    status = vl53l1_rd_word(Device,
                RANGE_CONFIG__MIN_COUNT_RATE_RTN_LIMIT_MCPS, &tmp);
    *signal = tmp <<3;
    return status;
}


VL53L1X_ERROR VL53L1X::vl53l1x_set_sigma_threshold(uint16_t Sigma)
{
    VL53L1X_ERROR status = 0;

    if(Sigma>(0xFFFF>>2)){
        return 1;
    }
    /* 16 bits register 14.2 format */
    status = vl53l1_wr_word(Device,RANGE_CONFIG__SIGMA_THRESH,Sigma<<2);
    return status;
}

VL53L1X_ERROR VL53L1X::vl53l1x_get_sigma_threshold(uint16_t *sigma)
{
    VL53L1X_ERROR status = 0;
    uint16_t tmp;

    status = vl53l1_rd_word(Device,RANGE_CONFIG__SIGMA_THRESH, &tmp);
    *sigma = tmp >> 2;
    return status;

}

VL53L1X_ERROR VL53L1X::vl53l1x_start_temperature_update()
{
    VL53L1X_ERROR status = 0;
    uint8_t tmp=0;

    status = vl53l1_wr_byte(Device,VL53L1_VHV_CONFIG__TIMEOUT_MACROP_LOOP_BOUND,0x81); /* full VHV */
    status = vl53l1_wr_byte(Device,0x0B,0x92);
    status = vl53l1x_start_ranging();
    while(tmp==0){
        status = vl53l1x_check_for_data_ready(&tmp);
    }
    tmp  = 0;
    status = vl53l1x_clear_interrupt();
    status = vl53l1x_stop_ranging();
    status = vl53l1_wr_byte(Device, VL53L1_VHV_CONFIG__TIMEOUT_MACROP_LOOP_BOUND, 0x09); /* two bounds VHV */
    status = vl53l1_wr_byte(Device, 0x0B, 0); /* start VHV from the previous temperature */
    return status;
}

    /* VL53L1X_calibration.h functions */
    
int8_t VL53L1X::vl53l1x_calibrate_offset(uint16_t TargetDistInMm, int16_t *offset)
{
    uint8_t i = 0, tmp;
    int16_t AverageDistance = 0;
    uint16_t distance;
    VL53L1X_ERROR status = 0;

    status = vl53l1_wr_word(Device, ALGO__PART_TO_PART_RANGE_OFFSET_MM, 0x0);
    status = vl53l1_wr_word(Device, MM_CONFIG__INNER_OFFSET_MM, 0x0);
    status = vl53l1_wr_word(Device, MM_CONFIG__OUTER_OFFSET_MM, 0x0);
    status = vl53l1x_start_ranging();    /* Enable VL53L1X sensor */
    for (i = 0; i < 50; i++) {
        while (tmp == 0){
            status = vl53l1x_check_for_data_ready(&tmp);
        }
        tmp = 0;
        status = vl53l1x_get_distance(&distance);
        status = vl53l1x_clear_interrupt();
        AverageDistance = AverageDistance + distance;
    }
    status = vl53l1x_stop_ranging();
    AverageDistance = AverageDistance / 50;
    *offset = TargetDistInMm - AverageDistance;
    status = vl53l1_wr_word(Device, ALGO__PART_TO_PART_RANGE_OFFSET_MM, *offset*4);
    return status;
}


int8_t VL53L1X::vl53l1x_calibrate_xtalk(uint16_t TargetDistInMm, uint16_t *xtalk)
{
    uint8_t i, tmp= 0;
    float AverageSignalRate = 0;
    float AverageDistance = 0;
    float AverageSpadNb = 0;
    uint16_t distance = 0, spadNum;
    uint16_t sr;
    VL53L1X_ERROR status = 0;

    status = vl53l1_wr_word(Device, 0x0016,0);
    status = vl53l1x_start_ranging();
    for (i = 0; i < 50; i++) {
        while (tmp == 0){
            status = vl53l1x_check_for_data_ready(&tmp);
        }
        tmp=0;
        status= vl53l1x_get_signal_rate(&sr);
        status= vl53l1x_get_distance(&distance);
        status = vl53l1x_clear_interrupt();
        AverageDistance = AverageDistance + distance;
        status = vl53l1x_get_spad_nb(&spadNum);
        AverageSpadNb = AverageSpadNb + spadNum;
        AverageSignalRate =
            AverageSignalRate + sr;
    }
    status = vl53l1x_stop_ranging();
    AverageDistance = AverageDistance / 50;
    AverageSpadNb = AverageSpadNb / 50;
    AverageSignalRate = AverageSignalRate / 50;
    /* Calculate Xtalk value */
    *xtalk = (uint16_t)(512*(AverageSignalRate*(1-(AverageDistance/TargetDistInMm)))/AverageSpadNb);
    status = vl53l1_wr_word(Device, 0x0016, *xtalk);
    return status;
}




    /* Write and read functions from I2C */


VL53L1X_ERROR VL53L1X::vl53l1_write_multi(VL53L1_DEV Dev, uint16_t index, uint8_t *pdata, uint32_t count)
{
   int  status;

   status = vl53l1_i2c_write(Dev->I2cDevAddr, index, pdata, (uint16_t)count);
   return status;
}

VL53L1X_ERROR VL53L1X::vl53l1_read_multi(VL53L1_DEV Dev, uint16_t index, uint8_t *pdata, uint32_t count)
{
    int status;

    status = vl53l1_i2c_read(Dev->I2cDevAddr, index, pdata, (uint16_t)count);

    return status;
}


VL53L1X_ERROR VL53L1X::vl53l1_wr_byte(VL53L1_DEV Dev, uint16_t index, uint8_t data)
{
   int  status;

   status=vl53l1_i2c_write(Dev->I2cDevAddr, index, &data, 1);
   return status;
}

VL53L1X_ERROR VL53L1X::vl53l1_wr_word(VL53L1_DEV Dev, uint16_t index, uint16_t data)
{
   int  status;
   uint8_t buffer[2];

     buffer[0] = data >> 8;
     buffer[1] = data & 0x00FF;
   status=vl53l1_i2c_write(Dev->I2cDevAddr, index, (uint8_t *)buffer, 2);
   return status;
}

VL53L1X_ERROR VL53L1X::vl53l1_wr_double_word(VL53L1_DEV Dev, uint16_t index, uint32_t data)
{
   int  status;
   uint8_t buffer[4];

     buffer[0] = (data >> 24) & 0xFF;
     buffer[1] = (data >> 16) & 0xFF;
     buffer[2] = (data >>  8) & 0xFF;
     buffer[3] = (data >>  0) & 0xFF;
   status=vl53l1_i2c_write(Dev->I2cDevAddr, index, (uint8_t *)buffer, 4);
   return status;
}


VL53L1X_ERROR VL53L1X::vl53l1_rd_byte(VL53L1_DEV Dev, uint16_t index, uint8_t *data)
{
   int  status;

   status = vl53l1_i2c_read(Dev->I2cDevAddr, index, data, 1);

   if(status)
     return -1;

   return 0;
}

VL53L1X_ERROR VL53L1X::vl53l1_rd_word(VL53L1_DEV Dev, uint16_t index, uint16_t *data)
{
   int  status;
   uint8_t buffer[2] = {0,0};

   status = vl53l1_i2c_read(Dev->I2cDevAddr, index, buffer, 2);
   if (!status)
   {
       *data = (buffer[0] << 8) + buffer[1];
   }
   return status;

}

VL53L1X_ERROR VL53L1X::vl53l1_rd_double_word(VL53L1_DEV Dev, uint16_t index, uint32_t *data)
{
   int status;
   uint8_t buffer[4] = {0,0,0,0};

   status = vl53l1_i2c_read(Dev->I2cDevAddr, index, buffer, 4);
   if(!status)
   {
       *data = (buffer[0] << 24) + (buffer[1] << 16) + (buffer[2] << 8) + buffer[3];
   }
   return status;

}

VL53L1X_ERROR VL53L1X::vl53l1_update_byte(VL53L1_DEV Dev, uint16_t index, uint8_t AndData, uint8_t OrData)
{
   int  status;
   uint8_t buffer = 0;

   /* read data direct onto buffer */
   status = vl53l1_i2c_read(Dev->I2cDevAddr, index, &buffer,1);
   if (!status)
   {
      buffer = (buffer & AndData) | OrData;
      status = vl53l1_i2c_write(Dev->I2cDevAddr, index, &buffer, (uint16_t)1);
   }
   return status;
}

VL53L1X_ERROR VL53L1X::vl53l1_i2c_write(uint8_t DeviceAddr, uint16_t RegisterAddr, uint8_t* pBuffer, uint16_t NumByteToWrite)
{
    int ret;
    ret = dev_i2c->v53l1x_i2c_write(pBuffer, DeviceAddr, RegisterAddr, NumByteToWrite);
    if (ret) {
        return -1;
    }
    return 0;

}

VL53L1X_ERROR VL53L1X::vl53l1_i2c_read(uint8_t DeviceAddr, uint16_t RegisterAddr, uint8_t* pBuffer, uint16_t NumByteToRead)
{
    int ret;

    ret = dev_i2c->v53l1x_i2c_read(pBuffer, DeviceAddr, RegisterAddr, NumByteToRead);

    if (ret) {
        return -1;
    }
    return 0;
}


VL53L1X_ERROR VL53L1X::vl53l1_get_tick_count(
    uint32_t *ptick_count_ms)
{

    /* Returns current tick count in [ms] */

    VL53L1X_ERROR status  = VL53L1_ERROR_NONE;

    *ptick_count_ms = 0;

    return status;
}



VL53L1X_ERROR VL53L1X::vl53l1_wait_us(VL53L1_Dev_t *pdev, int32_t wait_us)
{
    return VL53L1_ERROR_NONE;
}


VL53L1X_ERROR VL53L1X::vl53l1_wait_ms(VL53L1_Dev_t *pdev, int32_t wait_ms)
{
    return VL53L1_ERROR_NONE;
}


VL53L1X_ERROR VL53L1X::vl53l1_wait_value_mask_ex(
    VL53L1_Dev_t *pdev,
    uint32_t      timeout_ms,
    uint16_t      index,
    uint8_t       value,
    uint8_t       mask,
    uint32_t      poll_delay_ms)
{

    /*
     * Platform implementation of WaitValueMaskEx V2WReg script command
     *
     * WaitValueMaskEx(
     *          duration_ms,
     *          index,
     *          value,
     *          mask,
     *          poll_delay_ms);
     */

    VL53L1_Error status         = VL53L1_ERROR_NONE;
    uint32_t     start_time_ms = 0;
    uint32_t     current_time_ms = 0;
    uint32_t     polling_time_ms = 0;
    uint8_t      byte_value      = 0;
    uint8_t      found           = 0;



    /* calculate time limit in absolute time */

     vl53l1_get_tick_count(&start_time_ms);

    /* remember current trace functions and temporarily disable
     * function logging
     */


    /* wait until value is found, timeout reached on error occurred */

    while ((status == VL53L1_ERROR_NONE) &&
           (polling_time_ms < timeout_ms) &&
           (found == 0)) {

        if (status == VL53L1_ERROR_NONE)
            status = vl53l1_rd_byte(
                            pdev,
                            index,
                            &byte_value);

        if ((byte_value & mask) == value)
            found = 1;

        if (status == VL53L1_ERROR_NONE  &&
            found == 0 &&
            poll_delay_ms > 0)
            status = vl53l1_wait_ms(
                    pdev,
                    poll_delay_ms);

        /* Update polling time (Compare difference rather than absolute to
        negate 32bit wrap around issue) */
        vl53l1_get_tick_count(&current_time_ms);
        polling_time_ms = current_time_ms - start_time_ms;

    }
    

    if (found == 0 && status == VL53L1_ERROR_NONE)
        status = VL53L1_ERROR_TIME_OUT;

    return status;
}

int VL53L1X::handle_irq(uint16_t *distance)
{
    int status;
    status = get_measurement(distance);
    enable_interrupt_measure_detection_irq();
    return status;
}

int VL53L1X::get_measurement(uint16_t *distance)
{
    int status = 0;

    status = vl53l1x_get_distance(distance);
    status = vl53l1x_clear_interrupt();

    return status;
}

int VL53L1X::start_measurement(void (*fptr)(void))
{
    int status = 0;

    if (_gpio1Int == NULL) {
        printf("GPIO1 Error\r\n");
        return 1;
    }

    status = vl53l1x_stop_ranging(); // it is safer to do this while sensor is stopped

    if (status == 0) {
        attach_interrupt_measure_detection_irq(fptr);
        enable_interrupt_measure_detection_irq();
    }

    if (status == 0) {
        status = vl53l1x_start_ranging();
    }

    return status;
}

int VL53L1X::stop_measurement()
{
    int status = 0;

    if (status == 0) {
        printf("Call of VL53L0X_StopMeasurement\n");
        status = vl53l1x_stop_ranging();
    }

    if (status == 0)
        status = vl53l1x_clear_interrupt();

    return status;
}
