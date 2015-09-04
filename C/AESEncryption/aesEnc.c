#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/aes.h>

#include <openssl/rand.h>






char * decrypt(char * buffer, char* iv_encPass, char * aes_keyPass)
{
    char * iv_dec = iv_encPass;
    
    char *aes_key = aes_keyPass;
    int keylength = 128;
    
    
    size_t inputslength = 0;
    
  const size_t encslength = 1000;
    
    inputslength = strlen(buffer);

//	const size_t encslength = inputslength;
    
    char * enc_out = buffer;
    
    unsigned char *dec_out = (unsigned char*)malloc(sizeof(unsigned char) *inputslength);
     memset(dec_out, 0, sizeof(dec_out));
    AES_KEY dec_key;
    AES_set_decrypt_key(aes_key, keylength, &dec_key);
    AES_cbc_encrypt(enc_out, dec_out, encslength, &dec_key, iv_dec, AES_DECRYPT);
    
    return dec_out;
    
}


char * encrypt(char * buffer, char* iv_encPass, char * aes_keyPass)
{
    
    char * iv_enc = iv_encPass;
    
    char *aes_key = aes_keyPass;
	int keylength = 128;
    
    
    size_t inputslength = 0;
    
  
    inputslength = strlen(buffer);
    //printf("length of buffer is %d\n", inputslength);
    
    unsigned char *aes_input = (unsigned char*)malloc(sizeof(unsigned char) *2*inputslength);
    
    aes_input = buffer;
    //printf("%s is the buffer\n" , buffer);
   // printf("%s is the aes_input\n" , aes_input);
  
    
    const size_t encslength = ((inputslength + AES_BLOCK_SIZE) / AES_BLOCK_SIZE) * AES_BLOCK_SIZE;
    

    
    unsigned char *enc_out = (unsigned char*)malloc(sizeof(unsigned char) *encslength);
    memset(enc_out, 0, sizeof(enc_out));
   
    
    AES_KEY enc_key;
    
    AES_set_encrypt_key(aes_key, keylength, &enc_key);
    AES_cbc_encrypt(aes_input, enc_out, inputslength, &enc_key, iv_enc, AES_ENCRYPT);
    
    
    
    
  /*  printf("original:\t");
    printf("%s\n",aes_input);
    printf("encrypt:\t");
    printf("%s\n",enc_out);
    
    */
    
   // printf("decrypt:\t");
   // printf("%s\n",dec_out);
    
    return enc_out;
    
}

// main entrypoint
int main(int argc, char **argv)
{
    
    
    
    unsigned char iv_enc[16] = "intxcharystringz";
    unsigned char iv_dec[16] = "intxcharystringz";
    unsigned char aes_key[16] = "1234567890123456";
    
  
    char * buffer = "this is a sample buffer to be encrypted";

    printf("buffer is:\n %s\n", buffer);
    printf("\n\n\n");
    printf("running AES encryption ..... ");
    printf("\n\n\n");
    char*  encrypted=encrypt(buffer, iv_enc,aes_key);
    
    printf("encrypted buffer is now: \n %s\n",encrypted);
    printf("\n\n\n");
    printf("running AES decryption ..... ");
    printf("\n\n\n");
    
    char * decrypted = decrypt(encrypted, iv_dec,aes_key);
    
    printf("decrypted is:\n %s\n",decrypted);

    


    
    // free memory here
    
    return 0;
}
