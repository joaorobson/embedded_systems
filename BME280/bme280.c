/**
* Copyright (c) 2020 Bosch Sensortec GmbH. All rights reserved.
*
* BSD-3-Clause
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
* 3. Neither the name of the copyright holder nor the names of its
*    contributors may be used to endorse or promote products derived from
*    this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
* @file       bme280.c
* @date       2020-03-28
* @version    v3.5.0
*
*/

/*! @file bme280.c
 * @brief Sensor driver for BME280 sensor
 */
#include "bme280.h"

/**\name Internal macros */
/* To identify osr settings selected by user */
#define OVERSAMPLING_SETTINGS    UINT8_C(0x07)

/* To identify filter and standby settings selected by user */
#define FILTER_STANDBY_SETTINGS  UINT8_C(0x18)

/*!
 * @brief This internal API puts the device to sleep mode.
 *
 * @param[in] dev : Structure instance of bme280_dev.
 *
 * @return Result of API execution status.
 *
 * @retval   0 -> Success.
 * @retval > 0 -> Warning.
 * @retval < 0 -> Fail.
 *
 */
static int8_t put_device_to_sleep(struct bme280_dev *dev);

/*!
 * @brief This internal API writes the power mode in the sensor.
 *
 * @param[in] dev         : Structure instance of bme280_dev.
 * @param[in] sensor_mode : Variable which contains the power mode to be set.
 *
 * @return Result of API execution status.
 *
 * @retval   0 -> Success.
 * @retval > 0 -> Warning.
 * @retval < 0 -> Fail.
 *
 */
static int8_t write_power_mode(uint8_t sensor_mode, struct bme280_dev *dev);

/*!
 * @brief This internal API is used to validate the device pointer for
 * null conditions.
 *
 * @param[in] dev : Structure instance of bme280_dev.
 *
 * @return Result of API execution status
 *
 * @retval   0 -> Success.
 * @retval > 0 -> Warning.
 * @retval < 0 -> Fail.
 *
 */
static int8_t null_ptr_check(const struct bme280_dev *dev);

/*!
 * @brief This internal API interleaves the register address between the
 * register data buffer for burst write operation.
 *
 * @param[in] reg_addr   : Contains the register address array.
 * @param[out] temp_buff : Contains the temporary buffer to store the
 * register data and register address.
 * @param[in] reg_data   : Contains the register data to be written in the
 * temporary buffer.
 * @param[in] len        : No of bytes of data to be written for burst write.
 *
 */
static void interleave_reg_addr(const uint8_t *reg_addr, uint8_t *temp_buff, const uint8_t *reg_data, uint8_t len);

/*!
 * @brief This internal API reads the calibration data from the sensor, parse
 * it and store in the device structure.
 *
 * @param[in] dev : Structure instance of bme280_dev.
 *
 * @return Result of API execution status
 *
 * @retval   0 -> Success.
 * @retval > 0 -> Warning.
 * @retval < 0 -> Fail.
 *
 */
static int8_t get_calib_data(struct bme280_dev *dev);

/*!
 *  @brief This internal API is used to parse the temperature and
 *  pressure calibration data and store it in the device structure.
 *
 *  @param[out] dev     : Structure instance of bme280_dev to store the calib data.
 *  @param[in] reg_data : Contains the calibration data to be parsed.
 *
 */
static void parse_temp_press_calib_data(const uint8_t *reg_data, struct bme280_dev *dev);

/*!
 *  @brief This internal API is used to parse the humidity calibration data
 *  and store it in device structure.
 *
 *  @param[out] dev     : Structure instance of bme280_dev to store the calib data.
 *  @param[in] reg_data : Contains calibration data to be parsed.
 *
 */

#ifdef BME280_FLOAT_ENABLE

/*!
 * @brief This internal API is used to compensate the raw pressure data and
 * return the compensated pressure data in double data type.
 *
 * @param[in] uncomp_data : Contains the uncompensated pressure data.
 * @param[in] calib_data  : Pointer to the calibration data structure.
 *
 * @return Compensated pressure data in double.
 *
 */
static double compensate_pressure(const struct bme280_uncomp_data *uncomp_data,
                                  const struct bme280_calib_data *calib_data);

/*!
 * @brief This internal API is used to compensate the raw humidity data and
 * return the compensated humidity data in double data type.
 *
 * @param[in] uncomp_data : Contains the uncompensated humidity data.
 * @param[in] calib_data  : Pointer to the calibration data structure.
 *
 * @return Compensated humidity data in double.
 *
 */
static double compensate_humidity(const struct bme280_uncomp_data *uncomp_data,
                                  const struct bme280_calib_data *calib_data);

/*!
 * @brief This internal API is used to compensate the raw temperature data and
 * return the compensated temperature data in double data type.
 *
 * @param[in] uncomp_data : Contains the uncompensated temperature data.
 * @param[in] calib_data  : Pointer to calibration data structure.
 *
 * @return Compensated temperature data in double.
 *
 */
static double compensate_temperature(const struct bme280_uncomp_data *uncomp_data,
                                     struct bme280_calib_data *calib_data);

#else

/*!
 * @brief This internal API is used to compensate the raw temperature data and
 * return the compensated temperature data in integer data type.
 *
 * @param[in] uncomp_data : Contains the uncompensated temperature data.
 * @param[in] calib_data  : Pointer to calibration data structure.
 *
 * @return Compensated temperature data in integer.
 *
 */
static int32_t compensate_temperature(const struct bme280_uncomp_data *uncomp_data,
                                      struct bme280_calib_data *calib_data);

/*!
 * @brief This internal API is used to compensate the raw pressure data and
 * return the compensated pressure data in integer data type.
 *
 * @param[in] uncomp_data : Contains the uncompensated pressure data.
 * @param[in] calib_data  : Pointer to the calibration data structure.
 *
 * @return Compensated pressure data in integer.
 *
 */
static uint32_t compensate_pressure(const struct bme280_uncomp_data *uncomp_data,
                                    const struct bme280_calib_data *calib_data);

/*!
 * @brief This internal API is used to compensate the raw humidity data and
 * return the compensated humidity data in integer data type.
 *
 * @param[in] uncomp_data : Contains the uncompensated humidity data.
 * @param[in] calib_data  : Pointer to the calibration data structure.
 *
 * @return Compensated humidity data in integer.
 *
 */
static uint32_t compensate_humidity(const struct bme280_uncomp_data *uncomp_data,
                                    const struct bme280_calib_data *calib_data);

#endif

/*!
 * @brief This internal API is used to identify the settings which the user
 * wants to modify in the sensor.
 *
 * @param[in] sub_settings     : Contains the settings subset to identify particular
 * group of settings which the user is interested to change.
 * @param[in] desired_settings : Contains the user specified settings.
 *
 * @return Indicates whether user is interested to modify the settings which
 * are related to sub_settings.
 * @return True -> User wants to modify this group of settings
 * @return False -> User does not want to modify this group of settings
 *
 */
static uint8_t are_settings_changed(uint8_t sub_settings, uint8_t desired_settings);

/*!
 * @brief This internal API sets the oversampling settings for pressure,
 * temperature and humidity in the sensor.
 *
 * @param[in] desired_settings : Variable used to select the settings which
 * are to be set.
 * @param[in] settings         : Pointer variable which contains the settings to
 * be set in the sensor.
 * @param[in] dev              : Structure instance of bme280_dev.
 *
 * @return Result of API execution status
 *
 * @retval   0 -> Success.
 * @retval > 0 -> Warning.
 * @retval < 0 -> Fail.
 *
 */
static int8_t set_osr_settings(uint8_t desired_settings, const struct bme280_settings *settings,
                               struct bme280_dev *dev);

/*!
 * @brief This API sets the pressure and/or temperature oversampling settings
 * in the sensor according to the settings selected by the user.
 *
 * @param[in] dev : Structure instance of bme280_dev.
 * @param[in] desired_settings: variable to select the pressure and/or
 * temperature oversampling settings.
 * @param[in] settings : Pointer variable which contains the settings to
 * be set in the sensor.
 *
 * @return Result of API execution status
 *
 * @retval   0 -> Success.
 * @retval > 0 -> Warning.
 * @retval < 0 -> Fail.
 *
 */
static int8_t set_osr_press_temp_settings(uint8_t desired_settings,
                                          const struct bme280_settings *settings,
                                          struct bme280_dev *dev);

/*!
 * @brief This internal API fills the temperature oversampling settings provided
 * by the user in the data buffer so as to write in the sensor.
 *
 * @param[in] settings : Pointer variable which contains the settings to
 * be set in the sensor.
 * @param[out] reg_data : Variable which is filled according to the temperature
 * oversampling data provided by the user.
 *
 */
static void fill_osr_temp_settings(uint8_t *reg_data, const struct bme280_settings *settings);

/*!
 * @brief This internal API sets the filter and/or standby duration settings
 * in the sensor according to the settings selected by the user.
 *
 * @param[in] dev : Structure instance of bme280_dev.
 * @param[in] settings : Pointer variable which contains the settings to
 * be set in the sensor.
 * @param[in] settings : Structure instance of bme280_settings.
 *
 * @return Result of API execution status
 *
 * @retval   0 -> Success.
 * @retval > 0 -> Warning.
 * @retval < 0 -> Fail.
 *
 */
static int8_t set_filter_standby_settings(uint8_t desired_settings,
                                          const struct bme280_settings *settings,
                                          struct bme280_dev *dev);

/*!
 * @brief This internal API fills the filter settings provided by the user
 * in the data buffer so as to write in the sensor.
 *
 * @param[in] settings : Pointer variable which contains the settings to
 * be set in the sensor.
 * @param[out] reg_data : Variable which is filled according to the filter
 * settings data provided by the user.
 *
 */
static void fill_filter_settings(uint8_t *reg_data, const struct bme280_settings *settings);

/*!
 * @brief This internal API fills the standby duration settings provided by the
 * user in the data buffer so as to write in the sensor.
 *
 * @param[in] settings : Pointer variable which contains the settings to
 * be set in the sensor.
 * @param[out] reg_data : Variable which is filled according to the standby
 * settings data provided by the user.
 *
 */
static void fill_standby_settings(uint8_t *reg_data, const struct bme280_settings *settings);

/*!
 * @brief This internal API parse the oversampling(pressure, temperature
 * and humidity), filter and standby duration settings and store in the
 * device structure.
 *
 * @param[in] settings : Pointer variable which contains the settings to
 * be get in the sensor.
 * @param[in] reg_data : Register data to be parsed.
 *
 */
static void parse_device_settings(const uint8_t *reg_data, struct bme280_settings *settings);

/*!
 * @brief This internal API reloads the already existing device settings in the
 * sensor after soft reset.
 *
 * @param[in] dev : Structure instance of bme280_dev.
 * @param[in] settings : Pointer variable which contains the settings to
 * be set in the sensor.
 *
 * @return Result of API execution status
 *
 * @retval   0 -> Success.
 * @retval > 0 -> Warning.
 * @retval < 0 -> Fail.
 *
 */
static int8_t reload_device_settings(const struct bme280_settings *settings, struct bme280_dev *dev);

/****************** Global Function Definitions *******************************/

/*!
 *  @brief This API is the entry point.
 *  It reads the chip-id and calibration data from the sensor.
 */
int8_t bme280_init(struct bme280_dev *dev)
{
    int8_t rslt;

    /* chip id read try count */
    uint8_t try_count = 5;
    uint8_t chip_id = 0;

    /* Check for null pointer in the device structure*/
    rslt = null_ptr_check(dev);

    /* Proceed if null check is fine */
    if (rslt == BME280_OK)
    {
        while (try_count)
        {
            /* Read the chip-id of bme280 sensor */
            rslt = bme280_get_regs(BME280_CHIP_ID_ADDR, &chip_id, 1, dev);

            /* Check for chip id validity */
            if ((rslt == BME280_OK) && (chip_id == BME280_CHIP_ID))
            {
                dev->chip_id = chip_id;

                /* Reset the sensor */
                rslt = bme280_soft_reset(dev);

                if (rslt == BME280_OK)
                {
                    /* Read the calibration data */
                    rslt = get_calib_data(dev);
                }

                break;
            }

            /* Wait for 1 ms */
            dev->delay_us(1000, dev->intf_ptr);
            --try_count;
        }

        /* Chip id check failed */
        if (!try_count)
        {
            rslt = BME280_E_DEV_NOT_FOUND;
        }
    }

    return rslt;
}

/*!
 * @brief This API reads the data from the given register address of the sensor.
 */
int8_t bme280_get_regs(uint8_t reg_addr, uint8_t *reg_data, uint16_t len, struct bme280_dev *dev)
{
    int8_t rslt;

    /* Check for null pointer in the device structure*/
    rslt = null_ptr_check(dev);

    /* Proceed if null check is fine */
    if ((rslt == BME280_OK) && (reg_data != NULL))
    {
        /* If interface selected is SPI */
        if (dev->intf != BME280_I2C_INTF)
        {
            reg_addr = reg_addr | 0x80;
        }

        /* Read the data  */
        dev->intf_rslt = dev->read(reg_addr, reg_data, len, dev->intf_ptr);

        /* Check for communication error */
        if (dev->intf_rslt != BME280_INTF_RET_SUCCESS)
        {
            rslt = BME280_E_COMM_FAIL;
        }
    }
    else
    {
        rslt = BME280_E_NULL_PTR;
    }

    return rslt;
}

/*!
 * @brief This API writes the given data to the register address
 * of the sensor.
 */
int8_t bme280_set_regs(uint8_t *reg_addr, const uint8_t *reg_data, uint8_t len, struct bme280_dev *dev)
{
    int8_t rslt;
    uint8_t temp_buff[20]; /* Typically not to write more than 10 registers */

    if (len > 10)
    {
        len = 10;
    }

    uint16_t temp_len;
    uint8_t reg_addr_cnt;

    /* Check for null pointer in the device structure*/
    rslt = null_ptr_check(dev);

    /* Check for arguments validity */
    if ((rslt == BME280_OK) && (reg_addr != NULL) && (reg_data != NULL))
    {
        if (len != 0)
        {
            temp_buff[0] = reg_data[0];

            /* If interface selected is SPI */
            if (dev->intf != BME280_I2C_INTF)
            {
                for (reg_addr_cnt = 0; reg_addr_cnt < len; reg_addr_cnt++)
                {
                    reg_addr[reg_addr_cnt] = reg_addr[reg_addr_cnt] & 0x7F;
                }
            }

            /* Burst write mode */
            if (len > 1)
            {
                /* Interleave register address w.r.t data for
                 * burst write
                 */
                interleave_reg_addr(reg_addr, temp_buff, reg_data, len);
                temp_len = ((len * 2) - 1);
            }
            else
            {
                temp_len = len;
            }

            dev->intf_rslt = dev->write(reg_addr[0], temp_buff, temp_len, dev->intf_ptr);

            /* Check for communication error */
            if (dev->intf_rslt != BME280_INTF_RET_SUCCESS)
            {
                rslt = BME280_E_COMM_FAIL;
            }
        }
        else
        {
            rslt = BME280_E_INVALID_LEN;
        }
    }
    else
    {
        rslt = BME280_E_NULL_PTR;
    }

    return rslt;
}

/*!
 * @brief This API sets the oversampling, filter and standby duration
 * (normal mode) settings in the sensor.
 */
int8_t bme280_set_sensor_settings(uint8_t desired_settings, struct bme280_dev *dev)
{
    int8_t rslt;
    uint8_t sensor_mode;

    /* Check for null pointer in the device structure*/
    rslt = null_ptr_check(dev);

    /* Proceed if null check is fine */
    if (rslt == BME280_OK)
    {
        rslt = bme280_get_sensor_mode(&sensor_mode, dev);

        if ((rslt == BME280_OK) && (sensor_mode != BME280_SLEEP_MODE))
        {
            rslt = put_device_to_sleep(dev);
        }

        if (rslt == BME280_OK)
        {
            /* Check if user wants to change oversampling
             * settings
             */
            if (are_settings_changed(OVERSAMPLING_SETTINGS, desired_settings))
            {
                rslt = set_osr_settings(desired_settings, &dev->settings, dev);
            }

            /* Check if user wants to change filter and/or
             * standby settings
             */
            if ((rslt == BME280_OK) && are_settings_changed(FILTER_STANDBY_SETTINGS, desired_settings))
            {
                rslt = set_filter_standby_settings(desired_settings, &dev->settings, dev);
            }
        }
    }

    return rslt;
}

/*!
 * @brief This API gets the oversampling, filter and standby duration
 * (normal mode) settings from the sensor.
 */
int8_t bme280_get_sensor_settings(struct bme280_dev *dev)
{
    int8_t rslt;
    uint8_t reg_data[4];

    /* Check for null pointer in the device structure*/
    rslt = null_ptr_check(dev);

    /* Proceed if null check is fine */
    if (rslt == BME280_OK)
    {
        rslt = bme280_get_regs(BME280_CTRL_HUM_ADDR, reg_data, 4, dev);

        if (rslt == BME280_OK)
        {
            parse_device_settings(reg_data, &dev->settings);
        }
    }

    return rslt;
}

/*!
 * @brief This API sets the power mode of the sensor.
 */
int8_t bme280_set_sensor_mode(uint8_t sensor_mode, struct bme280_dev *dev)
{
    int8_t rslt;
    uint8_t last_set_mode;

    /* Check for null pointer in the device structure*/
    rslt = null_ptr_check(dev);

    if (rslt == BME280_OK)
    {
        rslt = bme280_get_sensor_mode(&last_set_mode, dev);

        /* If the sensor is not in sleep mode put the device to sleep
         * mode
         */
        if ((rslt == BME280_OK) && (last_set_mode != BME280_SLEEP_MODE))
        {
            rslt = put_device_to_sleep(dev);
        }

        /* Set the power mode */
        if (rslt == BME280_OK)
        {
            rslt = write_power_mode(sensor_mode, dev);
        }
    }

    return rslt;
}

/*!
 * @brief This API gets the power mode of the sensor.
 */
int8_t bme280_get_sensor_mode(uint8_t *sensor_mode, struct bme280_dev *dev)
{
    int8_t rslt;

    /* Check for null pointer in the device structure*/
    rslt = null_ptr_check(dev);

    if ((rslt == BME280_OK) && (sensor_mode != NULL))
    {
        /* Read the power mode register */
        rslt = bme280_get_regs(BME280_PWR_CTRL_ADDR, sensor_mode, 1, dev);

        /* Assign the power mode in the device structure */
        *sensor_mode = BME280_GET_BITS_POS_0(*sensor_mode, BME280_SENSOR_MODE);
    }
    else
    {
        rslt = BME280_E_NULL_PTR;
    }

    return rslt;
}

/*!
 * @brief This API performs the soft reset of the sensor.
 */
int8_t bme280_soft_reset(struct bme280_dev *dev)
{
    int8_t rslt;
    uint8_t reg_addr = BME280_RESET_ADDR;
    uint8_t status_reg = 0;
    uint8_t try_run = 5;

    /* 0xB6 is the soft reset command */
    uint8_t soft_rst_cmd = BME280_SOFT_RESET_COMMAND;

    /* Check for null pointer in the device structure*/
    rslt = null_ptr_check(dev);

    /* Proceed if null check is fine */
    if (rslt == BME280_OK)
    {
        /* Write the soft reset command in the sensor */
        rslt = bme280_set_regs(&reg_addr, &soft_rst_cmd, 1, dev);

        if (rslt == BME280_OK)
        {
            /* If NVM not copied yet, Wait for NVM to copy */
            do
            {
                /* As per data sheet - Table 1, startup time is 2 ms. */
                dev->delay_us(2000, dev->intf_ptr);
                rslt = bme280_get_regs(BME280_STATUS_REG_ADDR, &status_reg, 1, dev);

            } while ((rslt == BME280_OK) && (try_run--) && (status_reg & BME280_STATUS_IM_UPDATE));

            if (status_reg & BME280_STATUS_IM_UPDATE)
            {
                rslt = BME280_E_NVM_COPY_FAILED;
            }
        }
    }

    return rslt;
}

/*!
 * @brief This API reads the pressure, temperature and humidity data from the
 * sensor, compensates the data and store it in the bme280_data structure
 * instance passed by the user.
 */
int8_t bme280_get_sensor_data(uint8_t sensor_comp, struct bme280_data *comp_data, struct bme280_dev *dev)
{
    int8_t rslt;

    /* Array to store the pressure, temperature and humidity data read from
     * the sensor
     */
    uint8_t reg_data[BME280_P_T_H_DATA_LEN] = { 0 };
    struct bme280_uncomp_data uncomp_data = { 0 };

    /* Check for null pointer in the device structure*/
    rslt = null_ptr_check(dev);

    if ((rslt == BME280_OK) && (comp_data != NULL))
    {
        /* Read the pressure and temperature data from the sensor */
        rslt = bme280_get_regs(BME280_DATA_ADDR, reg_data, BME280_P_T_H_DATA_LEN, dev);

        if (rslt == BME280_OK)
        {
            /* Parse the read data from the sensor */
            bme280_parse_sensor_data(reg_data, &uncomp_data);

            /* Compensate the pressure and/or temperature and/or
             * humidity data from the sensor
             */
            rslt = bme280_compensate_data(sensor_comp, &uncomp_data, comp_data, &dev->calib_data);
        }
    }
    else
    {
        rslt = BME280_E_NULL_PTR;
    }

    return rslt;
}

/*!
 *  @brief This API is used to parse the pressure, temperature and
 *  humidity data and store it in the bme280_uncomp_data structure instance.
 */
void bme280_parse_sensor_data(const uint8_t *reg_data, struct bme280_uncomp_data *uncomp_data)
{
    /* Variables to store the sensor data */
    uint32_t data_xlsb;
    uint32_t data_lsb;
    uint32_t data_msb;

    /* Store the parsed register values for temperature data */
    data_msb = (uint32_t)reg_data[3] << 12;
    data_lsb = (uint32_t)reg_data[4] << 4;
    data_xlsb = (uint32_t)reg_data[5] >> 4;
    uncomp_data->temperature = data_msb | data_lsb | data_xlsb;

}

/*!
 * @brief This API is used to compensate the pressure and/or
 * temperature and/or humidity data according to the component selected
 * by the user.
 */
int8_t bme280_compensate_data(uint8_t sensor_comp,
                              const struct bme280_uncomp_data *uncomp_data,
                              struct bme280_data *comp_data,
                              struct bme280_calib_data *calib_data)
{
    int8_t rslt = BME280_OK;

    if ((uncomp_data != NULL) && (comp_data != NULL) && (calib_data != NULL))
    {
        /* Initialize to zero */
        comp_data->temperature = 0;

        /* If pressure or temperature component is selected */
        if (sensor_comp & BME280_TEMP)
        {
            /* Compensate the temperature data */
            comp_data->temperature = compensate_temperature(uncomp_data, calib_data);
        }
    }
    else
    {
        rslt = BME280_E_NULL_PTR;
    }

    return rslt;
}

/*!
 * @brief This API is used to calculate the maximum delay in milliseconds required for the
 * temperature/pressure/humidity(which ever at enabled) measurement to complete.
 */
uint32_t bme280_cal_meas_delay(const struct bme280_settings *settings)
{
    uint32_t max_delay;
    uint8_t temp_osr;

    /*Array to map OSR config register value to actual OSR */
    uint8_t osr_sett_to_act_osr[] = { 0, 1, 2, 4, 8, 16 };

    /* Mapping osr settings to the actual osr values e.g. 0b101 -> osr X16  */
    if (settings->osr_t <= 5)
    {
        temp_osr = osr_sett_to_act_osr[settings->osr_t];
    }
    else
    {
        temp_osr = 16;
    }

    max_delay =
        (uint32_t)((BME280_MEAS_OFFSET + (BME280_MEAS_DUR * temp_osr)) / BME280_MEAS_SCALING_FACTOR);

    return max_delay;
}

/*!
 * @brief This internal API sets the oversampling settings for pressure,
 * temperature and humidity in the sensor.
 */
static int8_t set_osr_settings(uint8_t desired_settings, const struct bme280_settings *settings, struct bme280_dev *dev)
{
    int8_t rslt = BME280_W_INVALID_OSR_MACRO;

    if (desired_settings & BME280_OSR_TEMP_SEL)
    {
        rslt = set_osr_press_temp_settings(desired_settings, settings, dev);
    }

    return rslt;
}

/*!
 * @brief This API sets the pressure and/or temperature oversampling settings
 * in the sensor according to the settings selected by the user.
 */
static int8_t set_osr_press_temp_settings(uint8_t desired_settings,
                                          const struct bme280_settings *settings,
                                          struct bme280_dev *dev)
{
    int8_t rslt;
    uint8_t reg_addr = BME280_CTRL_MEAS_ADDR;
    uint8_t reg_data;

    rslt = bme280_get_regs(reg_addr, &reg_data, 1, dev);

    if (rslt == BME280_OK)
    {
        if (desired_settings & BME280_OSR_TEMP_SEL)
        {
            fill_osr_temp_settings(&reg_data, settings);
        }

        /* Write the oversampling settings in the register */
        rslt = bme280_set_regs(&reg_addr, &reg_data, 1, dev);
    }

    return rslt;
}

/*!
 * @brief This internal API sets the filter and/or standby duration settings
 * in the sensor according to the settings selected by the user.
 */
static int8_t set_filter_standby_settings(uint8_t desired_settings,
                                          const struct bme280_settings *settings,
                                          struct bme280_dev *dev)
{
    int8_t rslt;
    uint8_t reg_addr = BME280_CONFIG_ADDR;
    uint8_t reg_data;

    rslt = bme280_get_regs(reg_addr, &reg_data, 1, dev);

    if (rslt == BME280_OK)
    {
        if (desired_settings & BME280_FILTER_SEL)
        {
            fill_filter_settings(&reg_data, settings);
        }

        if (desired_settings & BME280_STANDBY_SEL)
        {
            fill_standby_settings(&reg_data, settings);
        }

        /* Write the oversampling settings in the register */
        rslt = bme280_set_regs(&reg_addr, &reg_data, 1, dev);
    }

    return rslt;
}

/*!
 * @brief This internal API fills the filter settings provided by the user
 * in the data buffer so as to write in the sensor.
 */
static void fill_filter_settings(uint8_t *reg_data, const struct bme280_settings *settings)
{
    *reg_data = BME280_SET_BITS(*reg_data, BME280_FILTER, settings->filter);
}

/*!
 * @brief This internal API fills the standby duration settings provided by
 * the user in the data buffer so as to write in the sensor.
 */
static void fill_standby_settings(uint8_t *reg_data, const struct bme280_settings *settings)
{
    *reg_data = BME280_SET_BITS(*reg_data, BME280_STANDBY, settings->standby_time);
}

/*!
 * @brief This internal API fills the temperature oversampling settings
 * provided by the user in the data buffer so as to write in the sensor.
 */
static void fill_osr_temp_settings(uint8_t *reg_data, const struct bme280_settings *settings)
{
    *reg_data = BME280_SET_BITS(*reg_data, BME280_CTRL_TEMP, settings->osr_t);
}

/*!
 * @brief This internal API parse the oversampling(pressure, temperature
 * and humidity), filter and standby duration settings and store in the
 * device structure.
 */
static void parse_device_settings(const uint8_t *reg_data, struct bme280_settings *settings)
{
    settings->osr_t = BME280_GET_BITS(reg_data[2], BME280_CTRL_TEMP);
    settings->filter = BME280_GET_BITS(reg_data[3], BME280_FILTER);
    settings->standby_time = BME280_GET_BITS(reg_data[3], BME280_STANDBY);
}

/*!
 * @brief This internal API writes the power mode in the sensor.
 */
static int8_t write_power_mode(uint8_t sensor_mode, struct bme280_dev *dev)
{
    int8_t rslt;
    uint8_t reg_addr = BME280_PWR_CTRL_ADDR;

    /* Variable to store the value read from power mode register */
    uint8_t sensor_mode_reg_val;

    /* Read the power mode register */
    rslt = bme280_get_regs(reg_addr, &sensor_mode_reg_val, 1, dev);

    /* Set the power mode */
    if (rslt == BME280_OK)
    {
        sensor_mode_reg_val = BME280_SET_BITS_POS_0(sensor_mode_reg_val, BME280_SENSOR_MODE, sensor_mode);

        /* Write the power mode in the register */
        rslt = bme280_set_regs(&reg_addr, &sensor_mode_reg_val, 1, dev);
    }

    return rslt;
}

/*!
 * @brief This internal API puts the device to sleep mode.
 */
static int8_t put_device_to_sleep(struct bme280_dev *dev)
{
    int8_t rslt;
    uint8_t reg_data[4];
    struct bme280_settings settings;

    rslt = bme280_get_regs(BME280_CTRL_HUM_ADDR, reg_data, 4, dev);

    if (rslt == BME280_OK)
    {
        parse_device_settings(reg_data, &settings);
        rslt = bme280_soft_reset(dev);

        if (rslt == BME280_OK)
        {
            rslt = reload_device_settings(&settings, dev);
        }
    }

    return rslt;
}

/*!
 * @brief This internal API reloads the already existing device settings in
 * the sensor after soft reset.
 */
static int8_t reload_device_settings(const struct bme280_settings *settings, struct bme280_dev *dev)
{
    int8_t rslt;

    rslt = set_osr_settings(BME280_ALL_SETTINGS_SEL, settings, dev);

    if (rslt == BME280_OK)
    {
        rslt = set_filter_standby_settings(BME280_ALL_SETTINGS_SEL, settings, dev);
    }

    return rslt;
}

#ifdef BME280_FLOAT_ENABLE

/*!
 * @brief This internal API is used to compensate the raw temperature data and
 * return the compensated temperature data in double data type.
 */
static double compensate_temperature(const struct bme280_uncomp_data *uncomp_data, struct bme280_calib_data *calib_data)
{
    double var1;
    double var2;
    double temperature;
    double temperature_min = -40;
    double temperature_max = 85;

    var1 = ((double)uncomp_data->temperature) / 16384.0 - ((double)calib_data->dig_t1) / 1024.0;
    var1 = var1 * ((double)calib_data->dig_t2);
    var2 = (((double)uncomp_data->temperature) / 131072.0 - ((double)calib_data->dig_t1) / 8192.0);
    var2 = (var2 * var2) * ((double)calib_data->dig_t3);
    calib_data->t_fine = (int32_t)(var1 + var2);
    temperature = (var1 + var2) / 5120.0;

    if (temperature < temperature_min)
    {
        temperature = temperature_min;
    }
    else if (temperature > temperature_max)
    {
        temperature = temperature_max;
    }

    return temperature;
}

#else

/*!
 * @brief This internal API is used to compensate the raw temperature data and
 * return the compensated temperature data in integer data type.
 */
static int32_t compensate_temperature(const struct bme280_uncomp_data *uncomp_data,
                                      struct bme280_calib_data *calib_data)
{
    int32_t var1;
    int32_t var2;
    int32_t temperature;
    int32_t temperature_min = -4000;
    int32_t temperature_max = 8500;

    var1 = (int32_t)((uncomp_data->temperature / 8) - ((int32_t)calib_data->dig_t1 * 2));
    var1 = (var1 * ((int32_t)calib_data->dig_t2)) / 2048;
    var2 = (int32_t)((uncomp_data->temperature / 16) - ((int32_t)calib_data->dig_t1));
    var2 = (((var2 * var2) / 4096) * ((int32_t)calib_data->dig_t3)) / 16384;
    calib_data->t_fine = var1 + var2;
    temperature = (calib_data->t_fine * 5 + 128) / 256;

    if (temperature < temperature_min)
    {
        temperature = temperature_min;
    }
    else if (temperature > temperature_max)
    {
        temperature = temperature_max;
    }

    return temperature;
}

#endif

/*!
 * @brief This internal API reads the calibration data from the sensor, parse
 * it and store in the device structure.
 */
static int8_t get_calib_data(struct bme280_dev *dev)
{
    int8_t rslt;
    uint8_t reg_addr = BME280_TEMP_PRESS_CALIB_DATA_ADDR;

    /* Array to store calibration data */
    uint8_t calib_data[BME280_TEMP_PRESS_CALIB_DATA_LEN] = { 0 };

    /* Read the calibration data from the sensor */
    rslt = bme280_get_regs(reg_addr, calib_data, BME280_TEMP_PRESS_CALIB_DATA_LEN, dev);

    if (rslt == BME280_OK)
    {
        /* Parse temperature and pressure calibration data and store
         * it in device structure
         */
        parse_temp_press_calib_data(calib_data, dev);
        reg_addr = BME280_HUMIDITY_CALIB_DATA_ADDR;

        /* Read the humidity calibration data from the sensor */
        rslt = bme280_get_regs(reg_addr, calib_data, BME280_HUMIDITY_CALIB_DATA_LEN, dev);
    }

    return rslt;
}

/*!
 * @brief This internal API interleaves the register address between the
 * register data buffer for burst write operation.
 */
static void interleave_reg_addr(const uint8_t *reg_addr, uint8_t *temp_buff, const uint8_t *reg_data, uint8_t len)
{
    uint8_t index;

    for (index = 1; index < len; index++)
    {
        temp_buff[(index * 2) - 1] = reg_addr[index];
        temp_buff[index * 2] = reg_data[index];
    }
}

/*!
 *  @brief This internal API is used to parse the temperature and
 *  pressure calibration data and store it in device structure.
 */
static void parse_temp_press_calib_data(const uint8_t *reg_data, struct bme280_dev *dev)
{
    struct bme280_calib_data *calib_data = &dev->calib_data;

    calib_data->dig_t1 = BME280_CONCAT_BYTES(reg_data[1], reg_data[0]);
    calib_data->dig_t2 = (int16_t)BME280_CONCAT_BYTES(reg_data[3], reg_data[2]);
    calib_data->dig_t3 = (int16_t)BME280_CONCAT_BYTES(reg_data[5], reg_data[4]);
}

/*!
 * @brief This internal API is used to identify the settings which the user
 * wants to modify in the sensor.
 */
static uint8_t are_settings_changed(uint8_t sub_settings, uint8_t desired_settings)
{
    uint8_t settings_changed = FALSE;

    if (sub_settings & desired_settings)
    {
        /* User wants to modify this particular settings */
        settings_changed = TRUE;
    }
    else
    {
        /* User don't want to modify this particular settings */
        settings_changed = FALSE;
    }

    return settings_changed;
}

/*!
 * @brief This internal API is used to validate the device structure pointer for
 * null conditions.
 */
static int8_t null_ptr_check(const struct bme280_dev *dev)
{
    int8_t rslt;

    if ((dev == NULL) || (dev->read == NULL) || (dev->write == NULL) || (dev->delay_us == NULL))
    {
        /* Device structure pointer is not valid */
        rslt = BME280_E_NULL_PTR;
    }
    else
    {
        /* Device structure is fine */
        rslt = BME280_OK;
    }

    return rslt;
}
