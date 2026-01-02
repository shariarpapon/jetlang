
#include <stdio.h>

#include <elf_lexer.h>
#include <elf_token.h>
#include <toolbox/tbdefines.h>

int main(void)
{
    const char* source = "{ num = 123; }";

    elf_Lexer* lexer = elf_Lexer_create(source);
    
    if(!lexer)
        return _false;

    if(!lexer->tokens)
        return _false; 

    elf_Lexer_fullDispose(lexer);
    printf("lexer disposed...\n");

	printf("* all tests ran successfully\n");
    return _true;
}
