/*******************************************************************************
 * @file    VL53L1X_Class.h
 * @author  JS
 * @version V0.0.1
 * @date    15-January-2019
 * @brief   Header file for VL53L1 sensor component
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
 *****************************************************************************/

#ifndef __VL53L1X_CLASS_H
#define __VL53L1X_CLASS_H


#ifdef _MSC_VER
#   ifdef VL53L1X_API_EXPORTS
#       define VL53L1X_API  __declspec(dllexport)
#   else
#       define VL53L1X_API
#   endif
#else
#   define VL53L1X_API
#endif


/* Includes ------------------------------------------------------------------*/
#include "mbed.h"
#include "PinNames.h"
#include "RangeSensor.h"
#include "vl53l1x_error_codes.h"
#include "VL53L1X_I2C.h"
#include "Stmpe1600.h"


#define VL53L1X_IMPLEMENTATION_VER_MAJOR       1
#define VL53L1X_IMPLEMENTATION_VER_MINOR       0
#define VL53L1X_IMPLEMENTATION_VER_SUB         1
#define VL53L1X_IMPLEMENTATION_VER_REVISION  0000

typedef int8_t VL53L1X_ERROR;

#define VL53L1_I2C_SLAVE__DEVICE_ADDRESS                    0x0001
#define VL53L1_VHV_CONFIG__TIMEOUT_MACROP_LOOP_BOUND        0x0008
#define ALGO__CROSSTALK_COMPENSATION_PLANE_OFFSET_KCPS      0x0016
#define ALGO__CROSSTALK_COMPENSATION_X_PLANE_GRADIENT_KCPS  0x0018
#define ALGO__CROSSTALK_COMPENSATION_Y_PLANE_GRADIENT_KCPS  0x001A
#define ALGO__PART_TO_PART_RANGE_OFFSET_MM                  0x001E
#define MM_CONFIG__INNER_OFFSET_MM                          0x0020
#define MM_CONFIG__OUTER_OFFSET_MM                          0x0022
#define GPIO_HV_MUX__CTRL                                   0x0030
#define GPIO__TIO_HV_STATUS                                 0x0031
#define SYSTEM__INTERRUPT_CONFIG_GPIO                       0x0046
#define PHASECAL_CONFIG__TIMEOUT_MACROP                     0x004B
#define RANGE_CONFIG__TIMEOUT_MACROP_A_HI                   0x005E
#define RANGE_CONFIG__VCSEL_PERIOD_A                        0x0060
#define RANGE_CONFIG__VCSEL_PERIOD_B                        0x0063
#define RANGE_CONFIG__TIMEOUT_MACROP_B_HI                   0x0061
#define RANGE_CONFIG__TIMEOUT_MACROP_B_LO                   0x0062
#define RANGE_CONFIG__SIGMA_THRESH                          0x0064
#define RANGE_CONFIG__MIN_COUNT_RATE_RTN_LIMIT_MCPS         0x0066
#define RANGE_CONFIG__VALID_PHASE_HIGH                      0x0069
#define VL53L1_SYSTEM__INTERMEASUREMENT_PERIOD              0x006C
#define SYSTEM__THRESH_HIGH                                 0x0072
#define SYSTEM__THRESH_LOW                                  0x0074
#define SD_CONFIG__WOI_SD0                                  0x0078
#define SD_CONFIG__INITIAL_PHASE_SD0                        0x007A
#define ROI_CONFIG__USER_ROI_CENTRE_SPAD                    0x007F
#define ROI_CONFIG__USER_ROI_REQUESTED_GLOBAL_XY_SIZE       0x0080
#define SYSTEM__SEQUENCE_CONFIG                             0x0081
#define VL53L1_SYSTEM__GROUPED_PARAMETER_HOLD               0x0082
#define SYSTEM__INTERRUPT_CLEAR                             0x0086
#define SYSTEM__MODE_START                                  0x0087
#define VL53L1_RESULT__RANGE_STATUS                         0x0089
#define VL53L1_RESULT__DSS_ACTUAL_EFFECTIVE_SPADS_SD0       0x008C
#define RESULT__AMBIENT_COUNT_RATE_MCPS_SD                  0x0090
#define VL53L1_RESULT__FINAL_CROSSTALK_CORRECTED_RANGE_MM_SD0               0x0096
#define VL53L1_RESULT__PEAK_SIGNAL_COUNT_RATE_CROSSTALK_CORRECTED_MCPS_SD0  0x0098
#define VL53L1_RESULT__OSC_CALIBRATE_VAL                    0x00DE
#define VL53L1_FIRMWARE__SYSTEM_STATUS                      0x00E5
#define VL53L1_IDENTIFICATION__MODEL_ID                     0x010F
#define VL53L1_ROI_CONFIG__MODE_ROI_CENTRE_SPAD             0x013E


#define VL53L1X_DEFAULT_DEVICE_ADDRESS                      0x52

#define VL53L1X_REG_IDENTIFICATION_MODEL_ID                 0x010F
/****************************************
 * PRIVATE define do not edit
 ****************************************/

/**
 *  @brief defines SW Version
 */
typedef struct {
    uint8_t      major;    /*!< major number */
    uint8_t      minor;    /*!< minor number */
    uint8_t      build;    /*!< build number */
    uint32_t     revision; /*!< revision number */
} VL53L1X_Version_t;


typedef struct {

    uint8_t   I2cDevAddr;

} VL53L1_Dev_t;

typedef VL53L1_Dev_t *VL53L1_DEV;


/* Classes -------------------------------------------------------------------*/
/** Class representing a VL53L1 sensor component
 */
class VL53L1X : public RangeSensor
{
 public:
    /** Constructor
     * @param[in] &i2c device I2C to be used for communication
     * @param[in] &pin_gpio1 pin Mbed InterruptIn PinName to be used as component GPIO_1 INT
     * @param[in] DevAddr device address, 0x52 by default
     */
    VL53L1X(VL53L1X_DevI2C *i2c, DigitalOut *pin, PinName pin_gpio1 = PA_3, uint8_t dev_addr = VL53L1X_DEFAULT_DEVICE_ADDRESS)
    : RangeSensor(), dev_i2c(i2c), _gpio0(pin)
    {
        MyDevice.I2cDevAddr=dev_addr;
        Device = &MyDevice;
               
        _expgpio0 = NULL;
        if (pin_gpio1 != NC) {
            _gpio1Int = new InterruptIn(pin_gpio1);
        } else {
            _gpio1Int = NULL;
        }
    }
    
    
    /** Constructor 2 (STMPE1600DigiOut)
     * @param[in] i2c device I2C to be used for communication
     * @param[in] &pin Gpio Expander STMPE1600DigiOut pin to be used as component GPIO_0 CE
     * @param[in] pin_gpio1 pin Mbed InterruptIn PinName to be used as component GPIO_1 INT
     * @param[in] device address, 0x29 by default
     */
    VL53L1X(VL53L1X_DevI2C *i2c, Stmpe1600DigiOut *pin, PinName pin_gpio1,
            uint8_t dev_addr = VL53L1X_DEFAULT_DEVICE_ADDRESS) 
            : dev_i2c(i2c), _expgpio0(pin)
    {
        MyDevice.I2cDevAddr=dev_addr;
        Device = &MyDevice;
               
        _gpio0 = NULL;
        if (pin_gpio1 != NC) {
            _gpio1Int = new InterruptIn(pin_gpio1);
        } else {
            _gpio1Int = NULL;
        }
    }    
    
   /** Destructor
    */
    virtual ~VL53L1X()
    {        
        if (_gpio1Int != NULL) {
            delete _gpio1Int;
        }
    }
    
        
    
    VL53L1_DEV getDevicePtr() { return Device; }

    
    /* warning: VL53L1 class inherits from GenericSensor, RangeSensor and LightSensor, that haven`t a destructor.
       The warning should request to introduce a virtual destructor to make sure to delete the object */

    /*** Interface Methods ***/
    /*** High level API ***/
    /**
     * @brief       PowerOn the sensor
     * @return      void
     */
    /* turns on the sensor */
    virtual void vl53l1_on(void)
    {
        printf("VL53L1_On\r\n");
        if (_gpio0) {
            *_gpio0 = 1;
        } else {
            if (_expgpio0) {
                *_expgpio0 = 1;
            }
        }
        wait_us(10000);
    }

    /**
     * @brief       PowerOff the sensor
     * @return      void
     */
    /* turns off the sensor */
    virtual void vl53l1_off(void)
    {
        printf("VL53L1_Off\r\n");
        if (_gpio0) {
            *_gpio0 = 0;
        } else {
            if (_expgpio0) {
                *_expgpio0 = 0;
            }
        }
        wait_us(10000);
    }
    
    int is_present()
    {
        int status;
        uint8_t id = 0;

        status = read_id(&id);
        if (status) {
            printf("Failed to read ID device. Device not present!\n\r");
        }
        return status;
    }

    /**
     * @brief       Initialize the sensor with default values
     * @return      0 on Success
     */
     
     VL53L1X_ERROR init_sensor(uint8_t address){
        VL53L1X_ERROR status = 0;
        uint8_t sensorState = 0;
        vl53l1_off();
        vl53l1_on();
        status = vl53l1x_set_i2c_address(address);
        
        if(!status){
            status = vl53l1x_sensor_init();
        }      
        
        while(!status && !sensorState) {
            status = vl53l1x_boot_state(&sensorState);
            wait_us(2000);
        }
        
        return status;
     }



    /**
     *
     * @brief One time device initialization
     * @param void
     * @return     0 on success,  @a #CALIBRATION_WARNING if failed
     */
    virtual int init(void *init)
    {
       return vl53l1x_sensor_init();

    }

    /* Read function of the ID device */
    virtual int read_id(uint8_t *id){
        int status = 0;
        uint16_t rl_id = 0;
    
        uint8_t ExpanderData[2];
    
        ExpanderData[0] = 0;
        ExpanderData[1] = 0;
        rl_id = 0;
        dev_i2c->v53l1x_i2c_read(&ExpanderData[0], Device->I2cDevAddr, VL53L1X_REG_IDENTIFICATION_MODEL_ID, 2);
    
        rl_id = (ExpanderData[0] << 8) + ExpanderData[1];
        printf("Model ID is: %d (%X)  \r\n",rl_id, rl_id);
    
        uint8_t tmp = 0;
        ExpanderData[0] = VL53L1_FIRMWARE__SYSTEM_STATUS >> 8;
        ExpanderData[1] = VL53L1_FIRMWARE__SYSTEM_STATUS & 0x0FF;
        dev_i2c->v53l1x_i2c_read(&tmp, Device->I2cDevAddr, VL53L1_FIRMWARE__SYSTEM_STATUS, 1);

        printf("Firmware system is: %d\r\n",tmp);
    
        if (rl_id == 0xEACC) {
            printf("Device is present %d:\r\n", rl_id);
            return status;
        }
        return -1;
    }

    /**
     * @brief       Interrupt handling func to be called by user after an INT is occurred
     * @param[out]  Data pointer to the distance to read data in to
     * @return      0 on Success
     */
    int handle_irq(uint16_t *distance);

    /**
     * @brief       Start the measure indicated by operating mode
     * @param[in]   fptr specifies call back function must be !NULL in case of interrupt measure
     * @return      0 on Success
     */
    int start_measurement(void (*fptr)(void));
    /**
     * @brief       Stop the currently running measure indicate by operating_mode
     * @return      0 on Success
     */
    int stop_measurement();
    /**
     * @brief       Get results for the measure
     * @param[out]  Data pointer to the distance_data to read data in to
     * @return      0 on Success
     */
    int get_measurement(uint16_t *distance);
    /**
     * @brief       Enable interrupt measure IRQ
     * @return      0 on Success
     */
    void enable_interrupt_measure_detection_irq(void)
    {
        if (_gpio1Int != NULL)
            _gpio1Int->enable_irq();
    }

    /**
     * @brief       Disable interrupt measure IRQ
     * @return      0 on Success
     */
    void disable_interrupt_measure_detection_irq(void)
    {
        if (_gpio1Int != NULL)
            _gpio1Int->disable_irq();
    }
    /**
     * @brief       Attach a function to call when an interrupt is detected, i.e. measurement is ready
     * @param[in]   fptr pointer to call back function to be called whenever an interrupt occours
     * @return      0 on Success
     */
    void attach_interrupt_measure_detection_irq(void (*fptr)(void))
    {
        if (_gpio1Int != NULL)
            _gpio1Int->rise(fptr);
    }
    /**
     * @brief Get ranging result and only that
     * @param pRange_mm  Pointer to range distance
     * @return           0 on success
     */
    virtual int get_distance(uint32_t *piData)
    {
    int status;
    uint16_t distance;
    status = vl53l1x_get_distance(&distance);
    *piData = (uint32_t) distance;
    return status;
    }


    /* VL53L1X_api.h functions */

    /**
     * @brief This function returns the SW driver version
     */
    VL53L1X_ERROR vl53l1x_get_sw_version(VL53L1X_Version_t *pVersion);

    /**
     * @brief This function sets the sensor I2C address used in case multiple devices application, default address 0x52
     */
    VL53L1X_ERROR vl53l1x_set_i2c_address(uint8_t new_address);

    /**
     * @brief This function loads the 135 bytes default values to initialize the sensor.
     * @param dev Device address
     * @return 0:success, != 0:failed
     */
    VL53L1X_ERROR vl53l1x_sensor_init();

    /**
     * @brief This function clears the interrupt, to be called after a ranging data reading
     * to arm the interrupt for the next data ready event.
     */
    VL53L1X_ERROR vl53l1x_clear_interrupt();

    /**
     * @brief This function programs the interrupt polarity\n
     * 1=active high (default), 0=active low
     */
    VL53L1X_ERROR vl53l1x_set_interrupt_polarity(uint8_t IntPol);

    /**
     * @brief This function returns the current interrupt polarity\n
     * 1=active high (default), 0=active low
     */
    VL53L1X_ERROR vl53l1x_get_interrupt_polarity(uint8_t *pIntPol);

    /**
     * @brief This function starts the ranging distance operation\n
     * The ranging operation is continuous. The clear interrupt has to be done after each get data to allow the interrupt to raise when the next data is ready\n
     * 1=active high (default), 0=active low, use SetInterruptPolarity() to change the interrupt polarity if required.
     */
    VL53L1X_ERROR vl53l1x_start_ranging();

    /**
     * @brief This function stops the ranging.
     */
    VL53L1X_ERROR vl53l1x_stop_ranging();

    /**
     * @brief This function checks if the new ranging data is available by polling the dedicated register.
     * @param : isDataReady==0 -> not ready; isDataReady==1 -> ready
     */
    VL53L1X_ERROR vl53l1x_check_for_data_ready(uint8_t *isDataReady);
    
    /**
     * @brief This function programs the timing budget in ms.
     * Predefined values = 15, 20, 33, 50, 100(default), 200, 500.
     */
    VL53L1X_ERROR vl53l1x_set_timing_budget_in_ms(uint16_t TimingBudgetInMs);

    /**
     * @brief This function returns the current timing budget in ms.
     */
    VL53L1X_ERROR vl53l1x_get_timing_budget_in_ms(uint16_t *pTimingBudgetInMs);

    /**
     * @brief This function programs the distance mode (1=short, 2=long(default)).
     * Short mode max distance is limited to 1.3 m but better ambient immunity.\n
     * Long mode can range up to 4 m in the dark with 200 ms timing budget.
     */
    VL53L1X_ERROR vl53l1x_set_distance_mode(uint16_t DistanceMode);

    /**
     * @brief This function returns the current distance mode (1=short, 2=long).
     */
    VL53L1X_ERROR vl53l1x_get_distance_mode(uint16_t *pDistanceMode);

    /**
     * @brief This function programs the Intermeasurement period in ms\n
     * Intermeasurement period must be >/= timing budget. This condition is not checked by the API,
     * the customer has the duty to check the condition. Default = 100 ms
     */
    VL53L1X_ERROR vl53l1x_set_inter_measurement_in_ms(uint16_t InterMeasurementInMs);

    /**
     * @brief This function returns the Intermeasurement period in ms.
     */
    VL53L1X_ERROR vl53l1x_get_inter_measurement_in_ms(uint16_t * pIM);

    /**
     * @brief This function returns the boot state of the device (1:booted, 0:not booted)
     */
    VL53L1X_ERROR vl53l1x_boot_state(uint8_t *state);

    /**
     * @brief This function returns the sensor id, sensor Id must be 0xEEAC
     */
    VL53L1X_ERROR vl53l1x_get_sensor_id(uint16_t *id);

    /**
     * @brief This function returns the distance measured by the sensor in mm
     */
    VL53L1X_ERROR vl53l1x_get_distance(uint16_t *distance);

    /**
     * @brief This function returns the returned signal per SPAD in kcps/SPAD.
     * With kcps stands for Kilo Count Per Second
     */
    VL53L1X_ERROR vl53l1x_get_signal_per_spad(uint16_t *signalPerSp);

    /**
     * @brief This function returns the ambient per SPAD in kcps/SPAD
     */
    VL53L1X_ERROR vl53l1x_get_ambient_per_spad(uint16_t *amb);

    /**
     * @brief This function returns the returned signal in kcps.
     */
    VL53L1X_ERROR vl53l1x_get_signal_rate(uint16_t *signalRate);

    /**
     * @brief This function returns the current number of enabled SPADs
     */
    VL53L1X_ERROR vl53l1x_get_spad_nb(uint16_t *spNb);

    /**
     * @brief This function returns the ambient rate in kcps
     */
    VL53L1X_ERROR vl53l1x_get_ambient_rate(uint16_t *ambRate);

    /**
     * @brief This function returns the ranging status error \n
     * (0:no error, 1:sigma failed, 2:signal failed, ..., 7:wrap-around)
     */
    VL53L1X_ERROR vl53l1x_get_range_status(uint8_t *rangeStatus);

    /**
     * @brief This function programs the offset correction in mm
     * @param OffsetValue:the offset correction value to program in mm
     */
    VL53L1X_ERROR vl53l1x_set_offset(int16_t OffsetValue);

    /**
     * @brief This function returns the programmed offset correction value in mm
     */
    VL53L1X_ERROR vl53l1x_get_offset(int16_t *Offset);

    /**
     * @brief This function programs the xtalk correction value in cps (Count Per Second).\n
     * This is the number of photons reflected back from the cover glass in cps.
     */
    VL53L1X_ERROR vl53l1x_set_xtalk(uint16_t XtalkValue);

    /**
     * @brief This function returns the current programmed xtalk correction value in cps
     */
    VL53L1X_ERROR vl53l1x_get_xtalk(uint16_t *Xtalk);

    /**
     * @brief This function programs the threshold detection mode\n
     * Example:\n
     * vl53l1x_set_distance_threshold(dev,100,300,0,1): Below 100 \n
     * vl53l1x_set_distance_threshold(dev,100,300,1,1): Above 300 \n
     * vl53l1x_set_distance_threshold(dev,100,300,2,1): Out of window \n
     * vl53l1x_set_distance_threshold(dev,100,300,3,1): In window \n
     * @param   dev : device address
     * @param   ThreshLow(in mm) : the threshold under which one the device raises an interrupt if Window = 0
     * @param   ThreshHigh(in mm) :  the threshold above which one the device raises an interrupt if Window = 1
     * @param   Window detection mode : 0=below, 1=above, 2=out, 3=in
     * @param   IntOnNoTarget = 1 (No longer used - just use 1)
     */
    VL53L1X_ERROR vl53l1x_set_distance_threshold(uint16_t ThreshLow,
                      uint16_t ThreshHigh, uint8_t Window,
                      uint8_t IntOnNoTarget);

    /**
     * @brief This function returns the window detection mode (0=below; 1=above; 2=out; 3=in)
     */
    VL53L1X_ERROR vl53l1x_get_distance_threshold_window(uint16_t *window);

    /**
     * @brief This function returns the low threshold in mm
     */
    VL53L1X_ERROR vl53l1x_get_distance_threshold_low(uint16_t *low);

    /**
     * @brief This function returns the high threshold in mm
     */
    VL53L1X_ERROR vl53l1x_get_distance_threshold_high(uint16_t *high);

    /**
     * @brief This function programs the ROI (Region of Interest)\n
     * The ROI position is centered, only the ROI size can be reprogrammed.\n
     * The smallest acceptable ROI size = 4\n
     * @param X:ROI Width; Y=ROI Height
     */
    VL53L1X_ERROR vl53l1x_set_roi(uint16_t X, uint16_t Y);


    /**
 * @brief This function programs the center of the ROI (Region of Interest)
 * It MUST be called after having called VL53L1X_SetROI
 * It allows to change the ROI position
* @param ROICenter: Region of Interest center
 */
VL53L1X_ERROR VL53L1X_SetROICenter(uint8_t ROICenter);


    /**
     *@brief This function returns width X and height Y
     */
    VL53L1X_ERROR vl53l1x_get_roi_xy(uint16_t *ROI_X, uint16_t *ROI_Y);

    /**
     * @brief This function programs a new signal threshold in kcps (default=1024 kcps\n
     */
    VL53L1X_ERROR vl53l1x_set_signal_threshold(uint16_t signal);

    /**
     * @brief This function returns the current signal threshold in kcps
     */
    VL53L1X_ERROR vl53l1x_get_signal_threshold(uint16_t *signal);

    /**
     * @brief This function programs a new sigma threshold in mm (default=15 mm)
     */
    VL53L1X_ERROR vl53l1x_set_sigma_threshold(uint16_t sigma);

    /**
     * @brief This function returns the current sigma threshold in mm
     */
    VL53L1X_ERROR vl53l1x_get_sigma_threshold(uint16_t *signal);

    /**
     * @brief This function performs the temperature calibration.
     * It is recommended to call this function any time the temperature might have changed by more than 8 deg C
     * without sensor ranging activity for an extended period.
     */
    VL53L1X_ERROR vl53l1x_start_temperature_update();


    /* VL53L1X_calibration.h functions */
    
        /**
     * @brief This function performs the offset calibration.\n
     * The function returns the offset value found and programs the offset compensation into the device.
     * @param TargetDistInMm target distance in mm, ST recommended 100 mm
     * Target reflectance = grey17%
     * @return 0:success, !=0: failed
     * @return offset pointer contains the offset found in mm
     */
    int8_t vl53l1x_calibrate_offset(uint16_t TargetDistInMm, int16_t *offset);

    /**
     * @brief This function performs the xtalk calibration.\n
     * The function returns the xtalk value found and programs the xtalk compensation to the device
     * @param TargetDistInMm target distance in mm\n
     * The target distance : the distance where the sensor start to "under range"\n
     * due to the influence of the photons reflected back from the cover glass becoming strong\n
     * It's also called inflection point\n
     * Target reflectance = grey 17%
     * @return 0: success, !=0: failed
     * @return xtalk pointer contains the xtalk value found in cps (number of photons in count per second)
     */
    int8_t vl53l1x_calibrate_xtalk(uint16_t TargetDistInMm, uint16_t *xtalk);
    

    /* Write and read functions from I2C */

    VL53L1X_ERROR vl53l1_wr_byte(VL53L1_DEV dev, uint16_t index, uint8_t data);
    VL53L1X_ERROR vl53l1_wr_word(VL53L1_DEV dev, uint16_t index, uint16_t data);
    VL53L1X_ERROR vl53l1_wr_double_word(VL53L1_DEV dev, uint16_t index, uint32_t data);
    VL53L1X_ERROR vl53l1_rd_byte(VL53L1_DEV dev, uint16_t index, uint8_t *data);
    VL53L1X_ERROR vl53l1_rd_word(VL53L1_DEV dev, uint16_t index, uint16_t *data);
    VL53L1X_ERROR vl53l1_rd_double_word(VL53L1_DEV dev, uint16_t index, uint32_t *data);
    VL53L1X_ERROR vl53l1_update_byte(VL53L1_DEV dev, uint16_t index, uint8_t AndData, uint8_t OrData);

    VL53L1X_ERROR vl53l1_write_multi(VL53L1_DEV Dev, uint16_t index, uint8_t *pdata, uint32_t count);
    VL53L1X_ERROR vl53l1_read_multi(VL53L1_DEV Dev, uint16_t index, uint8_t *pdata, uint32_t count);

    VL53L1X_ERROR vl53l1_i2c_write(uint8_t dev, uint16_t index, uint8_t *data, uint16_t number_of_bytes);
    VL53L1X_ERROR vl53l1_i2c_read(uint8_t dev, uint16_t index, uint8_t *data, uint16_t number_of_bytes);
    VL53L1X_ERROR vl53l1_get_tick_count(uint32_t *ptick_count_ms);
    VL53L1X_ERROR vl53l1_wait_us(VL53L1_Dev_t *pdev, int32_t wait_us);
    VL53L1X_ERROR vl53l1_wait_ms(VL53L1_Dev_t *pdev, int32_t wait_ms);
    
    VL53L1X_ERROR vl53l1_wait_value_mask_ex(VL53L1_Dev_t *pdev, uint32_t timeout_ms, uint16_t index, uint8_t value, uint8_t mask, uint32_t poll_delay_ms);

 protected:

    /* IO Device */
    VL53L1X_DevI2C *dev_i2c;
    
    /* Digital out pin */
    DigitalOut *_gpio0;
    /* GPIO expander */
    Stmpe1600DigiOut *_expgpio0;
    /* Measure detection IRQ */
    InterruptIn *_gpio1Int;
 
    /* Device data */
    VL53L1_Dev_t MyDevice;
    VL53L1_DEV Device;
};


#endif /* _VL53L1X_CLASS_H_ */
