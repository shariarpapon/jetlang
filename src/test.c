#include <stdio.h>
#include <string.h>
#include <elf_lexer.h>

int main(void)
{
    const char* source = read_text_file("elfcode.ef");
    if(source == NULL)
    {
        return FALSE;
    }

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
    printf("lexer disposed...\n");

	printf("* all tests ran successfully\n");
    return TRUE;
 }
