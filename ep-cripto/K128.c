#include <stdio.h>
#include "K128.h"

/* K128 implementation as defined in the exercise by professor Routo Terada

   Convention: '_' prefix is used for internal functions and variables.
*/

uint8_t _exp[256];
uint8_t _log[256];

/* initialize _exp and _log, arrays used in circled_dot */
void init_f_y(){
    unsigned int x, result;
    
    result = 1;
    _exp[0] = 1;
    _log[1] = 0;
    
    for (x = 1; x < 256; x++){
        result = (result * 45) % 257;
        if (x == 128){
            _exp[(uint8_t) x] = 0;
            _log[0] = (uint8_t) x;
        }
        else{
            _exp[(uint8_t) x] = (uint8_t) result;
            _log[(uint8_t) result] = (uint8_t) x;
        }
    }
}


/* converts a 64 bit sequence to a byte-array of length 8 */
void _64bit_to_8_8bit_bytes (uint64_t in, uint8_t out[8]) {
    out[0] = (uint8_t)((in & 0xFF00000000000000) >> 56);
    out[1] = (uint8_t)((in & 0x00FF000000000000) >> 48);
    out[2] = (uint8_t)((in & 0x0000FF0000000000) >> 40);
    out[3] = (uint8_t)((in & 0x000000FF00000000) >> 32);
    out[4] = (uint8_t)((in & 0x00000000FF000000) >> 24);
    out[5] = (uint8_t)((in & 0x0000000000FF0000) >> 16);
    out[6] = (uint8_t)((in & 0x000000000000FF00) >> 8);
    out[7] = (uint8_t)((in & 0x00000000000000FF));
}


/* converts a byte-array of length 8 back to a 64 bit sequence */
uint64_t _8_8_bit_bytes_to_64bit (uint8_t in[8]) {
    uint64_t out = 0;
    out |= ((uint64_t)(in[0]) << 56);
    out |= ((uint64_t)(in[1]) << 48);
    out |= ((uint64_t)(in[2]) << 40);
    out |= ((uint64_t)(in[3]) << 32);
    out |= ((uint64_t)(in[4]) << 24);
    out |= ((uint64_t)(in[5]) << 16);
    out |= ((uint64_t)(in[6]) <<  8);
    out |= ((uint64_t)(in[7])      );
    return out;
}


/* circled dot operation ⊙ */
uint64_t op_circled_dot (uint64_t a, uint64_t b){
    uint8_t a8[8];
    uint8_t b8[8];
    
    // divir os 64 bits em 8 bytes de 8 bits
    _64bit_to_8_8bit_bytes (a, a8);
    _64bit_to_8_8bit_bytes (b, b8);
    
    // para cada byte fazer a soma
    for (int i = 0; i < 8; i++){
        a8[i] = _exp[a8[i]];
        b8[i] = _exp[b8[i]];
    }

    return _8_8_bit_bytes_to_64bit(a8) ^ _8_8_bit_bytes_to_64bit(b8);
}


/* inverse of ⊙ */
uint64_t op_circled_dot_inv (uint64_t a, uint64_t b){
    uint8_t a8[8];
    uint8_t b8[8];
    
    _64bit_to_8_8bit_bytes (a, a8);
    _64bit_to_8_8bit_bytes (b, b8);
    
    for (int i = 0; i < 8; i++){
        a8[i] = _log[ a8[i] ^ _exp[b8[i]] ];
    }
    return _8_8_bit_bytes_to_64bit(a8);
}


/* circular shift β << α
 * see: http://en.wikipedia.org/wiki/Circular_shift
 */
uint64_t op_rot (uint64_t a, uint64_t b){
    return (a << b) | (a >> (64 - b));
}


/* generate subkeys as defined in section 3 of the specs */
void generate_subkeys(uint64_t l, uint64_t r, uint64_t Keys[]){
    uint64_t L[NUMKEYS + 1];
    uint64_t k[NUMKEYS + 1];
    int i, j, s;
    
    // passo 1
    L[0] = l;
    L[1] = r;
    
    // passo 2
    for (j = 2; j < NUMKEYS; j++)
        L[j] = L[j - 1] + 0x9E3779B97F4A7C15;
    
    // passo 3
    k[0] = 0x8AED2A6BB7E15162;
    
    // passo 4
    for (j = 1; j < NUMKEYS; j++)
        k[j] = k[j-1] + 0x7C159E3779B97F4A;

    // passo 5
    i = 0;
    j = 0;
    uint64_t A = 0;
    uint64_t B = 0;
    
    // passo 6
    for (s = 1; s < NUMKEYS; s++){
        uint64_t tmp_a, tmp_b;
        
        // 6a
        tmp_a = k[i] + A + B;
        k[i] = op_rot (tmp_a, 3);
        A = k[i];
        i++;
        
        // 6b
        tmp_b = L[j] + (A + B);
        L[j] = op_rot (tmp_b, (A + B) & 0x3F);
        B = L[j];
        j++;
    }
    
    // a saida do algoritmo e k[1....4R+2]. Ajustar
    for (i = 0; i < NUMKEYS; i++){
        Keys[i] = k[i + 1];
    }

}


// primeira parte da iteração
// secao 2.2.1
void iteration_part1 (uint64_t Xa, uint64_t Xb,
                        uint64_t *Xal, uint64_t *Xbl,
                        uint64_t ka, uint64_t kb){
    *Xal = op_circled_dot (Xa, ka);
    *Xbl = Xb + kb;
}


void iteration_part1_inv (uint64_t Xa, uint64_t Xb,
                        uint64_t *Xal, uint64_t *Xbl,
                        uint64_t ka, uint64_t kb){
    *Xal = op_circled_dot_inv (Xa, ka);
    *Xbl = Xb - kb;
}


// segunda parte da iteracao
// secao 2.2.2
void iteration_part2 (uint64_t Xe, uint64_t Xf,
                        uint64_t *Xel, uint64_t *Xfl,
                        uint64_t ke, uint64_t kf){
    uint64_t Y1, Y2, Z;
    
    // step 1
    Y1 = Xe ^ Xf;
    
    // step 2
    Y2 = op_circled_dot ( op_circled_dot (ke, Y1) + Y1, kf);
    Z = op_circled_dot ( ke, Y1) + Y2;
    
    // step 3
    *Xel = Xe ^ Z;
    *Xfl = Xf ^ Z;

}


// chama os dois estagios do algoritmo
void iteration (uint64_t in_l, uint64_t in_r,
                uint64_t *out_l, uint64_t *out_r,
                uint64_t keys[]){
    uint64_t tmp_l;
    uint64_t tmp_r;
    
    iteration_part1 (in_l, in_r, &tmp_l, &tmp_r, keys[0], keys[1]);
    iteration_part2 (tmp_l, tmp_r, out_l, out_r, keys[2], keys[3]);
}


// inverso de uma iteracao
void iteration_inv (uint64_t in_l, uint64_t in_r,
                uint64_t *out_l, uint64_t *out_r,
                uint64_t keys[]){
    uint64_t tmp_l;
    uint64_t tmp_r;

    iteration_part2 (in_l, in_r, &tmp_l, &tmp_r, keys[2], keys[3]);
    iteration_part1_inv (tmp_l, tmp_r, out_l, out_r, keys[0], keys[1]);
}


/* algoritmo K128 para criptografia 
 * gera as chaves, faz as iteracoes do K128 e depois a T final
 */
void encrypt_K128 (uint64_t in_l, uint64_t in_r, 
                    uint64_t *out_l, uint64_t *out_r, 
                    uint64_t key_l, uint64_t key_r){
                    
    uint64_t Keys[NUMKEYS];
    int i;
    
    generate_subkeys (key_l, key_r, Keys);

    for (i = 0; i < R; i++){
        iteration (in_l, in_r, out_l, out_r, Keys + 4 * i);
        in_l = *out_l;
        in_r = *out_r;
    }
    // T final
    iteration_part1 (in_l, in_r, out_l, out_r, Keys[4*R], Keys[4*R + 1]);
}


/* algoritmo K128 para descriptografia
 * gera as chaves, desfaz a T final e depois as iteracoes normais
 */
void decrypt_K128(uint64_t in_l, uint64_t in_r, 
                    uint64_t *out_l, uint64_t *out_r, 
                    uint64_t key_l, uint64_t key_r){

    uint64_t Keys[NUMKEYS];
    int i;
    uint64_t tmp_l, tmp_r;
    
    generate_subkeys (key_l, key_r, Keys);

    iteration_part1_inv (in_l, in_r, &tmp_l, &tmp_r, Keys[4*R], Keys[4*R + 1]);

    for (i = R - 1; i >= 0; i--){
        iteration_inv (tmp_l, tmp_r, out_l, out_r, Keys + 4 * i);
        tmp_l = *out_l;
        tmp_r = *out_r;
    }
}


/* converts a password in char to a 128bit (2*64bit) key */
void password_to_key (char *password, uint64_t *k_l, uint64_t *k_r){

    uint8_t p[16];
    size_t size;
    
    size = strlen(password);
    if (size < 16){
        memcpy (p, password, size);
        memcpy (p + size, password, 16 - size);
    }
    else{
        memcpy (p, password, 16);
    }
    
    *k_l = _8_8_bit_bytes_to_64bit (p);
    *k_r = _8_8_bit_bytes_to_64bit (p+8);
}


