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

#include "dsp.h"
#include <math.h>
#include <stdint.h>
#include <string.h>
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




/*****************************************************************************/
/* Function implementation - global ('extern') and local ('static')          */
/*****************************************************************************/

/**
 * <Apply Hanninng  window coefficient>
 * \param uint16_t *samples: ADC Samples input array
 * \param uint16_t length: ADC Sample length
 * \return <return value>
 */


RC_t applyHanningWindow(uint16_t *samples, uint16_t length) {
    if (samples == NULL || length == 0) {
        return RC_ERROR_BAD_PARAM;
    }
    
    for (uint16_t idx = 0; idx < length; ++idx) {
        // Multiply by the Hanning window coefficient (which is between 0 and 1)
        float32_t scaled_sample = (float32_t)samples[idx] * hanning_table[idx];
        
        // Clamping the result to the range of uint16_t
        if (scaled_sample > UINT16_MAX) {
            samples[idx] = UINT16_MAX;
        } else if (scaled_sample < 0) {
            samples[idx] = 0;
        } else {
            samples[idx] = (uint16_t)scaled_sample;
        }
    }

    return RC_SUCCESS;
}