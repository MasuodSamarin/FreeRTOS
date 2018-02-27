/*********************************************************************************************
    *   Filename        : ad_parser.h

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2017-01-17 15:15

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/
// *****************************************************************************
//
// Advertising Data Parser
//
// *****************************************************************************

#ifndef __AD_PARSER_H
#define __AD_PARSER_H

#include "btstack-config.h"

#if defined __cplusplus
extern "C" {
#endif

    /* API_START */

    typedef struct ad_context {
        const uint8_t *data;
        uint8_t   offset;
        uint8_t   length;
    } ad_context_t;

// Advertising or Scan Response data iterator
    void ad_iterator_init(ad_context_t *context, uint8_t ad_len, const uint8_t *ad_data);
    int  ad_iterator_has_more(const ad_context_t *context);
    void ad_iterator_next(ad_context_t *context);

// Access functions
    uint8_t         ad_iterator_get_data_type(const ad_context_t *context);
    uint8_t         ad_iterator_get_data_len(const ad_context_t *context);
    const uint8_t *ad_iterator_get_data(const ad_context_t *context);

// convenience function on complete advertisements
    int ad_data_contains_uuid16(uint8_t ad_len, const uint8_t *ad_data, uint16_t uuid);
    int ad_data_contains_uuid128(uint8_t ad_len, const uint8_t *ad_data, const uint8_t *uuid128);

    /* API_END */

#if defined __cplusplus
}
#endif
#endif // __AD_PARSER_H
