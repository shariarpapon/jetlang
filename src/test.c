#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <elf_lexer.h>
#include <elf_utils.h>

int main(void)
{
    size_t src_len = 0;
    const char* source = (const char*)load_text_file("elfcode.ef", &src_len);
    if(!source)
    {
        fprintf(stderr, "error: could not load source text\n");
    }

    printf("source text loaded (len: %zu):\n\n%s\n", src_len, source);

    elf_lexer* lexer = elf_lexer_create(source);
    
    if(!lexer)
        return FALSE;

    if(!lexer->tokens)
        return FALSE; 

    elf_lexer_tokenize(lexer);
    printf("tokenization complete...\n");
    
    printf("\n******* printing-token-list *******\n\n");
    elf_token_print_list((elf_token**)lexer->tokens->elements, lexer->tokens->count); 
    printf("\n******* list-end *******\n\n");
    
    elf_lexer_full_dispose(lexer);
    free((void*)source);
    printf("lexer disposed...\n");
    

	printf("* all tests ran successfully\n");
    return TRUE;
 }
