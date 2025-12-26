#include <stdlib.h>
#include <stdio.h>

#include <elf_lexer.h>
#include <toolbox/tbdefines.h>
#include <toolbox/tblist.h>

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

    printf("tblist created...\n");
    int cpc = 16;
    tblist* list = tblist_create(cpc);
    for(int i = 0; i < cpc; i++)
    {
        int* apd = malloc(sizeof(int));
        *apd = i;
        tblist_append(list, (void*)apd);
        printf("\tadded: %d\n", *((int*)tblist_get(list, (size_t)i)));
    }
    printf("integers [0, 15] added to list...\n");
    printf("printing list...\n");
    for(int i =0; i < cpc; i++)
    {
        void* data = tblist_get(list, (size_t)i);
        int* intPtr = (int*)data;
        int num = *intPtr;
        printf("\t%d\n", num);
    }

	printf("* all tests ran successfully\n");
    return 1;
}
