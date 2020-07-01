/*
 * regulator_types.h
 *
 *  Created on: 2. 5. 2019
 *      Author: Petr Dvorak
 */

#ifndef REGULATOR_TYPES_H_
#define REGULATOR_TYPES_H_

#include "stm32f0xx.h"
#include "defs.h"

typedef enum {
	OUTPUT0 = 0,
	OUTPUT1,
	OUTPUT2,
	OUTPUT3,
	OUTPUT4,
	OUTPUT5,
	OUTPUT6,
	OUTPUT7,
	OUTPUT8,
	OUTPUT9,
	OUTPUT10,
	OUTPUT11,
	OUTPUT12,
	OUTPUT13,
	OUTPUT14,
	OUTPUT15
} OUTPUT_INDEX;

typedef enum {
	HEATBEDLET0 = 0,
	HEATBEDLET1,
	HEATBEDLET2,
	HEATBEDLET3,
	HEATBEDLET4,
	HEATBEDLET5,
	HEATBEDLET6,
	HEATBEDLET7,
	HEATBEDLET8,
	HEATBEDLET9,
	HEATBEDLET10,
	HEATBEDLET11,
	HEATBEDLET12,
	HEATBEDLET13,
	HEATBEDLET14,
	HEATBEDLET15
} HEATBEDLETID;

typedef enum {
	HBLOK = 0,
	HBLDISCONNECTED,
	HBLHEATINGERROR,
	HBLSHORT,
	HBLLOST,
	HBLFOUND,
	HBLNOTFOUND,
	HBLREBORN,
	HBLTHRMLRAWAYCOOL,
	HBLTHRMLRAWAYHEAT,
	HBLTHRMLRAWAYSTAB,
	HBLGENERALERROR = 127
} HBL_ERRORCODES;

// BED STATUS
typedef enum {
	HBLDISABLED = 0,
	HBLENABLED,
	HBLUNINITIALIZED,
	HBLINITIALIZED,
	HBLANIMATION,
	HBLHEATING,
	HBLCOOLING,
	HBLREGULATING,
	HBLIDLE,
	HBLTOBECHECKED,
	HBLSTATUSUNKNOWN = 127
} HBL_STATUS;

typedef enum {
	HBLPHASE0 = 0,
	HBLPHASE1,
	HBLPHASE2,
	HBLPHASE3,
	HBLPHASE4,
	HBLPHASE5,
	HBLPHASE6,
	HBLPHASE7,
	HBLPHASE8,
	HBLPHASE9,
	HBLPHASE10,
	HBLPHASE11,
	HBLPHASE12,
	HBLPHASE13,
	HBLPHASE14,
	HBLPHASE15
} HBL_PHASE;

typedef enum {
	HBLLOAD0 = 0,
	HBLLOAD1,
	HBLLOAD2,
	HBLLOAD3,
	HBLLOAD4,
	HBLLOAD5,
	HBLLOAD6,
	HBLLOAD7,
	HBLLOAD8,
	HBLLOAD9,
	HBLLOAD10,
	HBLLOAD11,
	HBLLOAD12,
	HBLLOAD13,
	HBLLOAD14,
	HBLLOAD15,
	HBLLOAD16
} HBL_LOAD;

typedef enum {
	HBLINIT = 0,
	HBLDACRUNNING,
	HBLDACDUTYUPDATED,
	HBLDACON,
	HBLDACOFF
} HBL_DAC_STATUS;

typedef enum {
	DATA0 = 0,
	DATA1,
	DATA2,
	DATA3,
	DATA4,
	DATA5,
	DATA6,
	DATA7,
	DATAMUX0,
	DATAMUX1
} DMADATAINDEX;

typedef enum {
	ADCMUX89 = 8,
	ADCMUX1011 = 10,
	ADCMUX1213 = 12,
	ADCMUX1415 = 14,
} MUXSETTINGS;

#define DUTY0		0

typedef struct {
	const OUTPUT_INDEX index;
	const GPIO_TypeDef * port;
	const uint16_t pin;
	const HBL_PHASE phase;
	uint8_t duty;
	HBL_DAC_STATUS dacstatus;
} hbl_output_t;

typedef struct {
	uint8_t	req_temperature;
	uint8_t curr_temperature;
	uint8_t counter;
	int16_t psd_kp;
	int16_t psd_ki;
	int16_t psd_kd;
	int16_t psd_intreg;
	int16_t psd_error;
	int16_t psd_value;
	uint16_t heatingcounter;
	uint16_t coolingcounter;
	uint8_t heating_inittemp;
	uint8_t cooling_inittemp;
} hbl_regulator_t;

typedef struct {
	uint16_t temp_mult;
	uint16_t temp_dev;
	uint16_t temp_offset;
	DMADATAINDEX dmadataindex;
} hbl_ntc_t;

typedef struct {
	HEATBEDLETID heatbedletid;
	hbl_regulator_t *regulator;
	hbl_output_t *output;
	hbl_ntc_t *ntc;
	uint8_t enabled;
	HBL_STATUS errorcode;
} hbl_heatbedlet_t;

#define HBL_MODULAR_BED_COUNT	16

typedef struct {
	uint8_t heatbedid;
	hbl_heatbedlet_t *heatbedlet[HBL_MODULAR_BED_COUNT];
	HBL_STATUS status;
	HBL_LOAD load;
	HBL_LOAD maxload;
} hbl_heatbed_t;

typedef struct {
	uint8_t heatbedid;
	int16_t diff;
} hbl_diff_t;

#define REPORTLEN				15
typedef enum {
	SENT = 0,
	TOBESEND
} MSGFLAG;

typedef struct {
	uint8_t msg[REPORTLEN];
	MSGFLAG flag;
} report_msg_t;


#endif /* REGULATOR_TYPES_H_ */
