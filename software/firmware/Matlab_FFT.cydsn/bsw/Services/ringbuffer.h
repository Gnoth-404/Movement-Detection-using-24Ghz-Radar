/**
 * \file 	ringbuffer.h
 * \author	Thomas Barth 				Hochschule Darmstadt - thomas.barth@h-da.de
 * \date 	03.05.2019
 *
 * \brief 	General purpose ringbuffer mechanism for the use with ERIKA os 
 *
 * \todo This helper is WIP, use with caution 
 *
 * ----- Changelog -----
 *
 *
 * \copyright Copyright ?DateTime inactive during development
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
#ifndef RINGBUFFER_H
#define RINGBUFFER_H
    
/*=======================[ Includes ]==============================================================*/
    
#include <project.h>
#include "global.h"
    
/*=======================[ Symbols ]===============================================================*/

/**\brief Indicator if a ringbuffer shall not use ERIKA ressources */
#define RINGBUFFER_NO_ERIKA_RES     uint32_t_MAX
    
/*=======================[ Types ]=================================================================*/

/**
 * \brief Rungbuffer handler structure
 */
typedef struct{
    uint32_t    erika_ressource;	/**< Erika Ressource which will be "Get" and "Release" in read/write operations */
    uint32_t    idx_read;           /**< Read index, points to the next byte to be read */
    uint32_t    idx_write;          /**< Write index, points to the next free byte in the buffer */
    uint32_t    filled;             /**< Number of bytes stored in the buffer*/
    uint32_t    size;               /**< Size of the buffer */
    void*       p_payload;          /**< Pointer to the buffer itself */
}ringbuffer_hdl_t;
    
/**
 * \brief initializes a ringbuffer handle
 * @param rb            pointer to the ringbuffer handle
 * @param erika_res     ERIKA OS resource to protect against concurrent access.
 *                      If no ERIKA ressource shall be used, set to #RINGBUFFER_NO_ERIKA_RES
 * @param p_payload     pointer to memory in which the payload will be saved
 * @param size          Size in bytes for p_payload
 * @return always #RC_SUCCESS
 */
inline RC_t Ringbuffer_Init(ringbuffer_hdl_t* rb, uint32_t erika_res, void* p_payload, uint32_t size){
    rb->erika_ressource=erika_res;
    rb->idx_read=0;
    rb->idx_write=0;
    rb->filled=0;
    rb->p_payload=p_payload;
    rb->size=size;
    
    return RC_SUCCESS;
}

inline RC_t Ringbuffer_Write(ringbuffer_hdl_t* rb, void* p_payload, uint32_t size){
    
    //byte iterator
    uint32_t i;
    
    //before we can do anything, we need to get the resssource
    if(RINGBUFFER_NO_ERIKA_RES!=rb->erika_ressource)
        GetResource(rb->erika_ressource);
    
    //we check if there is enough memory left to store the new data
    if((rb->filled+size)>rb->size)
        return RC_ERROR_BUFFER_FULL;
    
    //increment the fill level
    rb->filled+=size;
    
    //deep copy payload
    for(i=0;i<size;i++){
        //copy byte
        ((uint8_t*)rb->p_payload)[rb->idx_write]=((uint8_t*)p_payload)[i];
        
        //update write index
        rb->idx_write++;
        rb->idx_write%=rb->size;
    }
    
    //ok, we are done, release ressource
    if(RINGBUFFER_NO_ERIKA_RES!=rb->erika_ressource)
        ReleaseResource(rb->erika_ressource);
    
    return RC_SUCCESS;
}

inline RC_t Ringbuffer_Read(ringbuffer_hdl_t* rb, void* p_payload, uint32_t size){
    
    //byte iterator
    uint32_t i;
    
    //before we can do anything, we need to get the resssource
    if(RINGBUFFER_NO_ERIKA_RES!=rb->erika_ressource)
        GetResource(rb->erika_ressource);
    
    //we check if the requested size is actually present in the buffer
    if(size>rb->filled)
        return RC_ERROR_BUFFER_EMTPY;
    
    //decrement the fill level
    rb->filled-=size;
    
    //deep copy payload
    for(i=0;i<size;i++){
        //copy byte
        ((uint8_t*)p_payload)[i]=((uint8_t*)rb->p_payload)[rb->idx_read];
        
        //update read index
        rb->idx_read++;
        rb->idx_read%=rb->size;
    }
    
    //ok, we are done, release ressource
    if(RINGBUFFER_NO_ERIKA_RES!=rb->erika_ressource)
        ReleaseResource(rb->erika_ressource);
    
    return RC_SUCCESS;
}

#endif /*RINGBUFFER_H*/