#include <stdlib.h>
#include <stdio.h>

#include <elf_lexer.h>
#include <elf_token.h>
#include <toolbox/tbdefines.h>

int main(void)
{
    const char* source = "{ code = 123 }";

    elf_Lexer* lexer = elf_Lexer_create(source);
    
    if(!lexer)
        return 0;

    if(!lexer->tokens)
        return 0;

    free(lexer->tokens);
    free(lexer);     
    printf("lexer disposed...\n");

	printf("* all tests ran successfully\n");
    return 1;
}
