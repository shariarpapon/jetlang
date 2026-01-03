#include <stdio.h>
#include <elf_lexer.h>

int main(void)
{
    const char* source = "{ num = 123; }";

    elf_Lexer* lexer = elf_Lexer_create(source);
    
    if(!lexer)
        return FALSE;

    if(!lexer->tokens)
        return FALSE; 

    elf_Lexer_tokenize(lexer);
    printf("tokenization complete...\n");
    
    printf("\n******* printing-token-list *******\n\n");
    elf_Token_printList((elf_Token**)lexer->tokens->elements, lexer->tokens->count); 
    printf("\n******* list-end *******\n\n");
    
    elf_Lexer_fullDispose(lexer);
    printf("lexer disposed...\n");

	printf("* all tests ran successfully\n");
    return TRUE;
}
