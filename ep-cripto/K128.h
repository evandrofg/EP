#include <stdint.h>
#include <string.h>
#define R 12
#define NUMKEYS 50

/* basic operation ⊙ */
void init_f_y();
uint64_t op_circled_dot (uint64_t a, uint64_t b);

/* circular shift β << α */
uint64_t op_rot (uint64_t a, uint64_t b);

uint64_t op_circled_dot (uint64_t a, uint64_t b);
uint64_t op_circled_dot_inv (uint64_t a, uint64_t b);

void generate_subkeys (uint64_t l, uint64_t r, uint64_t Keys[]);

/* steps of the algorithm */
void iteration_part1 (uint64_t Xa, uint64_t Xb,
                        uint64_t *Xal, uint64_t *Xbl,
                        uint64_t ka, uint64_t kb);
void iteration_part2 (uint64_t Xe, uint64_t Xf,
                        uint64_t *Xel, uint64_t *Xfl,
                        uint64_t ke, uint64_t kf);
void iteration (uint64_t in_l, uint64_t in_r,
                uint64_t *out_l, uint64_t *out_r,
                uint64_t keys[]);

/* inverses (for decrypting)*/
void iteration_inv (uint64_t in_l, uint64_t in_r,
                uint64_t *out_l, uint64_t *out_r,
                uint64_t keys[]);


void encrypt_K128 (uint64_t in_l, uint64_t in_r, 
                    uint64_t *out_l, uint64_t *out_r, 
                    uint64_t key_l, uint64_t key_r);
void decrypt_K128 (uint64_t in_l, uint64_t in_r, 
                    uint64_t *out_l, uint64_t *out_r, 
                    uint64_t key_l, uint64_t key_r);

void password_to_key(char* senha, uint64_t *k_l, uint64_t *k_r);

