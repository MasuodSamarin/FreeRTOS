#ifndef __AES_CMAC_H__
#define __AES_CMAC_H__


void mesh_s1(uint8_t *output, const uint8_t *m, int len);

void mesh_k1(uint8_t *output,
             const uint8_t *N, int len_N,
             const uint8_t *SALT,
             const uint8_t *P, int len_P);

void mesh_k2(uint8_t *output,  // 33-byte long (only 263 bits valid)
             const uint8_t *N, // 128-bit
             const uint8_t *P, int len_P);

void mesh_k3(uint8_t *output,   //  64-bit long
             const uint8_t *N); // 128-bit long

void mesh_k4(uint8_t *output,   // 6-bit valid
             const uint8_t *N); // 128-bit long


void AES_CMAC(unsigned char *key, unsigned char *input, int length,
              unsigned char *mac);

#endif /* __AES_CMAC_H__ */