
#include "crypto.h"
#include "K128.h"

// usamos um struct para ler 16 bytes (128 bits) por vez
typedef struct Block {
    uint64_t l;
    uint64_t r;
} block;


// faz a criptografia de um arquivo usando CBC
unsigned long encrypt_cbc (FILE* file_in, FILE* file_out, uint64_t k_l, uint64_t k_r) {
    unsigned long total_size;
    size_t n_read;
    block in, out;
    uint64_t in_l, in_r, out_l, out_r;

    total_size = 0;
    n_read = 16;
    block prev_block = { 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF };
    
    while (n_read == 16) {
        // read 16 bytes (128 bits)
        n_read = fread (&in, 1, 16, file_in);
        total_size += n_read;

        in.l ^= prev_block.l;
        in.r  ^= prev_block.r;

        in_l = in.l; in_r = in.r;

        encrypt_K128 (in_l, in_r, 
                    &out_l, &out_r, 
                    k_l, k_r);

        out.l = out_l; out.r = out_r;
        fwrite (&out, 1, 16, file_out);

        prev_block.l = out_l;
        prev_block.r = out_r;

    }
    return total_size;
}


// faz a descriptografia usando CBC
void decrypt_cbc (FILE* file_in, FILE* file_out, uint64_t k_l, uint64_t k_r) {
    unsigned long total_size;
    block in, out;
    uint64_t in_l, in_r, out_l, out_r;
    
    total_size = 0;
    block prev_block = { 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF };

    fread(&total_size, 4, 1, file_in);
    
    while (total_size > 0) {
        fread (&in, 1, 16, file_in);

        in_l = in.l;
        in_r = in.r;
        decrypt_K128 (in_l, in_r, 
                    &out_l, &out_r, 
                    k_l, k_r);
        out.l = out_l;
        out.r = out_r;

        out.l ^= prev_block.l;
        out.r  ^= prev_block.r;

        if (total_size >= 16) {
            fwrite (&out, 1, 16, file_out);
            prev_block.l = in.l;
            prev_block.r = in.r;
            total_size -= 16;
        }
        else {
            fwrite (&out, 1, total_size, file_out);
            break;
        }
    }
}


// dado o nome do arquivo de entrada e saída, e a senha, chama a 
// função para encriptografar
void encrypt (char* input, char* output, char* pvalue, int a){
    FILE* file_in = fopen (input, "r");
    FILE* file_out = fopen (output, "w");
    
    unsigned long size;
    uint64_t k_l;
    uint64_t k_r;
    
    size = 0;
    password_to_key (pvalue, &k_l, &k_r);
    
    // escreve o tamanho no comeco do arquivo, por enquanto errado
    fwrite (&size, 4, 1, file_out);
    printf ("Encrypting...\n");
    size = encrypt_cbc (file_in, file_out, k_l, k_r);
    
    // agora volta e escreve o tamanho certo
    fseek (file_out, 0, SEEK_SET);
    fwrite (&size, 4, 1, file_out);
    
    // apagar o arquivo se pedido
    if (a == 1){
        FILE *f = fopen (input, "w");
        fclose(f);
    }
    
    fclose(file_in);
    fclose(file_out);
}


// dado o nome do arquivo de entrada e saída, e a senha, chama a 
// função para descriptografar
void decrypt (char* input, char* output, char* pvalue){
    FILE* file_in = fopen (input, "r");
    FILE* file_out = fopen (output, "w");

    uint64_t k_l;
    uint64_t k_r;

    password_to_key (pvalue, &k_l, &k_r);
    
    printf ("Decrypting...\n");
    decrypt_cbc (file_in, file_out, k_l, k_r);
    
    fclose(file_in);
    fclose(file_out);
}

