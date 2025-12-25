#include <stdlib.h>
#include <stdio.h>

#include <elf_lexer.h>
#include <toolbox/tbdefines.h>
    
int main(void)
{
    const char* source = "test: lexer creation--->";

    t_elf_lexer* lexer = elf_create_lexer(source);
    
    if(!lexer)
        return _false;

    if(!lexer->tokens)
        return _false;

    free(lexer->tokens);
    free(lexer);     
    printf("lexer disposed...\n");


	printf("* all tests ran successfully\n");
    return _true;
}
