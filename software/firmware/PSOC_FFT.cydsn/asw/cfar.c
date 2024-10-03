/**
* \file <filename>
* \author <author-name>
* \date <date>
*
* \brief <Symbolic File name>
*
* \copyright Copyright ©2016
* Department of electrical engineering and information technology, Hochschule Darmstadt - University of applied sciences (h_da). All Rights Reserved.
* Permission to use, copy, modify, and distribute this software and its documentation for educational, and research purposes in the context of non-commercial
* (unless permitted by h_da) and official h_da projects, is hereby granted for enrolled students of h_da, provided that the above copyright notice,
* this paragraph and the following paragraph appear in all copies, modifications, and distributions.
* Contact Prof.Dr.-Ing. Peter Fromm, peter.fromm@h-da.de, Birkenweg 8 64295 Darmstadt - GERMANY for commercial requests.
*
* \warning This software is a PROTOTYPE version and is not designed or intended for use in production, especially not for safety-critical applications!
* The user represents and warrants that it will NOT use or redistribute the Software for such purposes.
* This prototype is for research purposes only. This software is provided "AS IS," without a warranty of any kind.
*/

/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/
#include <math.h>
#include <stdint.h>
#include <string.h>
#include "cfar.h"
#include "logging.h"
#include "arm_math.h"
/*****************************************************************************/
/* Local pre-processor symbols/macros ('#define')                            */
/*****************************************************************************/

/*****************************************************************************/
/* Global variable definitions (declared in header file with 'extern')       */
/*****************************************************************************/

/*****************************************************************************/
/* Local type definitions ('typedef')                                        */
/*****************************************************************************/

/*****************************************************************************/
/* Local variable definitions ('static')                                     */
/*****************************************************************************/



/*****************************************************************************/
/* Local function prototypes ('static')                                      */
/*****************************************************************************/


/**
 * <Description>
 * \param <first para>
 * \return <return value>
 */
RC_t calcPowerSpectrum(sint32_t *fft_buffer, uint16_t fft_length, sint32_t *mag_square, sint32_t *power) {
    if (fft_length % 2 != 0) {
        return RC_ERROR_BAD_PARAM;
    }
    
    uint16_t half_length = fft_length / 2;

    for (uint16_t idx = 0; idx < half_length; idx++) {
        float32_t real = (float32_t)fft_buffer[2 * idx];
        float32_t imag = (float32_t)fft_buffer[2 * idx + 1];
        float64_t magnitude = (real * real + imag * imag);
        
        mag_square[idx] = (sint32_t)(sqrt(magnitude));

        if (magnitude == 0) {
            
            power[idx] = 0;  // or any minimum value you consider appropriate
        } else {
            float power_dB = 10*log10(magnitude);
            
            power[idx] = (sint32_t)power_dB;
        }
    }

    return RC_SUCCESS;
}
/**
 * Cell Averaging CFAR algorithm
 * \param NR : Number of reference cell
 * \param NG : Number of guard cell
 * \param PFA : Probability of False Alarm
 * \return <return value>
 */

RC_t CA_CFAR(sint32_t *fftInput, uint16_t N, uint8_t NG, uint8_t NR, float64_t PFA, sint32_t* CAFRThreshold, uint16_t* first_CUT_Index, uint16_t* last_CUT_index ){
    
    
    if ( N <= (2* NG+ NR)){
        return RC_ERROR_BAD_PARAM;
    }
    
    float64_t alpha = NR*2*(pow(PFA, -1.0/(NR*2)) -1);
    *first_CUT_Index = NG + NR;
    *last_CUT_index = N - NG - NR -1;

    for(uint16_t CUT = 0; CUT < N; ++CUT){
        
        if(CUT >= *first_CUT_Index && CUT <= *last_CUT_index){
        
            float64_t sumLeading = 0.0;
            float64_t sumTrailing = 0.0;
            // Calculate sum of leading and trailing window
            for (uint8_t i =0; i < NR; ++i){
                sumLeading += fftInput[CUT - NG - NR + i];
                sumTrailing += fftInput[CUT + NG + i +1];  
            }
    
            float64_t noiseLevel = (sumLeading + sumTrailing) / (2* NR);
            
            
            float64_t threshold = alpha * noiseLevel;
            
            CAFRThreshold[CUT] = (sint32_t)(10*log10(threshold));
            

        }
        else {
            CAFRThreshold[CUT] = 0;
        }
 
    }
    

    
    return RC_SUCCESS;
}