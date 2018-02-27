#ifndef __AES_CCM_H__
#define __AES_CCM_H__

void aes_ccm_encrypt_with_key(
    uint8_t *enc_data,
    uint8_t *mic,
    uint32_t mic_len,
    const uint8_t *key,
    const uint8_t *nonce,
    const uint8_t *plain_text,
    uint32_t plain_text_len,
    const uint8_t *addition_data,
    uint32_t addition_len);

void aes_ccm_decrypt_with_key(
    uint8_t *plain_text,
    uint8_t *mic,
    uint32_t mic_len,
    const uint8_t *key,
    const uint8_t *nonce,
    const uint8_t *enc_data,
    uint32_t enc_data_len,
    const uint8_t *addition_data,
    uint32_t addition_len);


#endif /* __AES_CCM_H__ */