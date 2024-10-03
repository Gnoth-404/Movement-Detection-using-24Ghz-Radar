/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"
#include "global.h"
#include "led.h"
#include "logging.h"
#include <stdio.h>
#include <stdlib.h>
#include "fft_application.h"

#include "dsp.h"
#include "cfar.h"

typedef enum{
    IDLE,
    SAMPLING,
    DETECTING
}state_t;

typedef enum{
    NONE,
    RX_SAMPLING,
    RX_DONE,
}uartStatus_t;

typedef struct
{
    sint32_t fft_sensor_data[2*NUMBER_OF_SAMPLE];
    sint32_t mag[NUMBER_OF_SAMPLE];
    sint32_t power[NUMBER_OF_SAMPLE];
} fft_handler_t;


typedef struct
{
    sint32_t threshhold[NUMBER_OF_SAMPLE];
    uint16_t startCUTIdx;
    uint16_t stopCUTIdx;
} cfar_handler_t;


typedef struct
{
    uint8_t count;
    uint16_t sensor_data[NUMBER_OF_SAMPLE];
    fft_handler_t sensor_fft;
    cfar_handler_t cfar;   
    state_t currentState;
    uartStatus_t uartStatus;
}shield_data_t;


static shield_data_t shieldData;



CY_ISR_PROTO(isr_button_handler);

CY_ISR_PROTO(isr_rx_handler);


/* Defines for DMA_ADC */
#define DMA_ADC_BYTES_PER_BURST 2
#define DMA_ADC_REQUEST_PER_BURST 1
#define DMA_ADC_SRC_BASE (CYDEV_PERIPH_BASE)
#define DMA_ADC_DST_BASE (CYDEV_SRAM_BASE)

/* Variable declarations for DMA_ADC */
/* Move these variable declarations to the top of the function */

uint8 DMA_ADC_Chan;
uint8 DMA_ADC_TD[1];

void DMA_Config(){


    /* DMA Configuration for DMA_ADC */

    DMA_ADC_Chan = DMA_ADC_DmaInitialize(DMA_ADC_BYTES_PER_BURST, DMA_ADC_REQUEST_PER_BURST, 
        HI16(DMA_ADC_SRC_BASE), HI16(DMA_ADC_DST_BASE));

    DMA_ADC_TD[0] = CyDmaTdAllocate();

    CyDmaTdSetConfiguration(DMA_ADC_TD[0], 2048, CY_DMA_DISABLE_TD, DMA_ADC__TD_TERMOUT_EN | CY_DMA_TD_INC_DST_ADR);

    CyDmaTdSetAddress(DMA_ADC_TD[0], LO16((uint32)ADC_DEC_SAMP_PTR), LO16((uint32)shieldData.sensor_data));
    CyDmaChSetInitialTd(DMA_ADC_Chan, DMA_ADC_TD[0]);
    
}



void shieldInit(){

   shieldData.currentState = IDLE;
   shieldData.count = 1;
   shieldData.uartStatus= NONE;
   for (int16_t i=0; i< NUMBER_OF_SAMPLE; i++){
    shieldData.sensor_data[i] =0;    
    }

}


// Interrupt handler
CY_ISR(isr_button_handler)
{
    Button_ClearInterrupt();
    
    if(shieldData.currentState == IDLE)
        shieldData.currentState = SAMPLING;
    isr_button_ClearPending();
}


// UART RX Interrupt
CY_ISR(isr_rx_handler)
{
    uint8 rxRead = UART_LOG_ReadRxData();
    if (rxRead == 's')
    {
        shieldData.uartStatus= RX_SAMPLING;
    }
    if (rxRead =='o')
    {
       shieldData.uartStatus = RX_DONE;
    }
}
int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    WaveDAC8_Start();
    WaveDAC8_Enable(); 
    
    // Assign Interrupt
    isr_button_StartEx(isr_button_handler);
    isr_rx_StartEx(isr_rx_handler);
    
    ADC_Start();
    
    LED_Init();

    LOG_Init();
    shieldInit();

    DMA_Config();
    
    
    for(;;)
    {
        /* Place your application code here. */

        switch(shieldData.currentState){
            
        case IDLE:

            
            LED_Set(LED_GREEN, 1);
            
            break;
        case SAMPLING:
            LED_Set(LED_YELLOW, 1);
            ADC_StartConvert();
            CyDmaChEnable(DMA_ADC_Chan, 1);

            if (Status_Reg_1_Read()){
                    
                    // When DMA transfer finished , turn on DMA Channel 
                    // Switch to next stage       
                    ADC_StopConvert();
                    CyDmaChDisable(DMA_ADC_Chan);
                    shieldData.currentState = DETECTING;
                    
                    // Apply Hanning window for ADC data sampling
                    applyHanningWindow(shieldData.sensor_data, NUMBER_OF_SAMPLE);
                    
                    // Fast fourier transform our windowned data
                    fft_app(shieldData.sensor_data, shieldData.sensor_fft.fft_sensor_data, NUMBER_OF_SAMPLE);
                    
                    // Calculate power spectrum , return in Db 
                    calcPowerSpectrum(shieldData.sensor_fft.fft_sensor_data, NUMBER_OF_SAMPLE*2,shieldData.sensor_fft.mag, shieldData.sensor_fft.power);

                    // Calculate threshold using CA_CFAR algorithm
                    CA_CFAR(shieldData.sensor_fft.mag, NUMBER_OF_SAMPLE, NUMBER_OF_GUARD, NUMBER_OF_REF, PROB_FALSE_ALARM, shieldData.cfar.threshhold,&shieldData.cfar.startCUTIdx,&shieldData.cfar.stopCUTIdx);
                    LED_Set(LED_YELLOW, 0);
  

            }

            break;
        case DETECTING:

            shieldData.currentState = SAMPLING;
            LED_Set(LED_GREEN, 1);
            shieldData.uartStatus = NONE;

            uint32_t count_detection = 0;
            for (int i=shieldData.cfar.startCUTIdx; i <= shieldData.cfar.stopCUTIdx; i++){
                if ( shieldData.cfar.threshhold[i] < shieldData.sensor_fft.power[i]){
                    count_detection++;
                }                        
            }
            LOG_I("Count detection","%i",count_detection);
            if (count_detection > COUNT_DETECTION_THRESHOLD){
                LED_Set(LED_GREEN,0);
                LED_Set(LED_RED, 1);
                CyDelay(500);
                LED_Set(LED_RED, 0);
            }
            
            break;
        default:
            break;
            
        }
    }
}

/* [] END OF FILE */




