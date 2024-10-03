/* =================================================================
 * Copyright Hochschule Darmstadt, 2020
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION WHICH IS THE PROPERTY 
 * OF H_DA.
 *
 * Author: Guillermo Urizar
 * Tutor: Prof. Dr. Stephen Bannwarth
 * Spring Project
 *
 * FFT Module for 24GHz-Radar for Movement Detection
 * v1.0
 *
 * Content of file: Header file for the FFT application.
 * =================================================================
*/

#ifndef _FFT_APPLICATION_H
#define _FFT_APPLICATION_H
    
    #include "project.h"
    #include <stdlib.h>
    #include "arm_math.h"

    /* fft_app =====================================================
     * INOUT adc_samples   : array from main.c which contains the
     *                       ADC samples.
     * INOUT fft_output    : array from main.c which will contain
     *                       the result of the FFT calculation. It
     *                       should be double the size of
     *                       adc_samples. The result is downscaled
     *                       by no_of_samples to prevent overflow.
     * IN    no_of_samples : number of samples. Either 512 or 1024.
     * =============================================================
    */
    extern void fft_app(uint16 *adc_samples, int32 *fft_output, uint16 no_of_samples);
    
#endif
    
/* [] END OF FILE */