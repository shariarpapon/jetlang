#include <stdlib.h>
#include <stdio.h>

#include <elf_lexer.h>

//TEST
#include <toolbox/tbdefines.h>

int main(void)
{
    const char* source = "test: lexer creation--->";

    t_elf_lexer* lexer = elf_create_lexer(source);
    
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
