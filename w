#include <jet_ast.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

static bool jet_ast_generate_tree(jet_ast* ast); 
static bool jet_ast_node_dispose_block     ;   
static bool jet_ast_node_dispose_dot_access;    
static bool jet_ast_node_dispose_binary_op ;    
static bool jet_ast_node_dispose_return    ;    
static bool jet_ast_node_dispose_while     ;    
static bool jet_ast_node_dispose_var_ref   ;    
static bool jet_ast_node_dispose_func_def  ;    
static bool jet_ast_node_dispose_func_call ;    
static bool jet_ast_node_dispose_list      ; 
static bool jet_ast_node_dispose_ident     ;  
static bool jet_ast_node_dispose_param     ;  
static bool jet_ast_node_dispose_type_decl ;  
static bool jet_ast_node_dispose_unary_op  ;
static bool jet_ast_node_dispose_if        ;
static bool jet_ast_node_dispose_for       ;
static bool jet_ast_node_dispose_var_decl  ;
static bool jet_ast_node_dispose_func_decl ;   
static bool jet_ast_node_dispose_expr_stmt ;  

jet_ast* jet_ast_create(jet_list* token_list)
{
    if(!token_list)
    {
        fprintf(stderr, "error: cannot create jet_ast, invalid arg token_list.\n");
        return NULL;
    }

    if(jet_list_count(token_list) == 0)
    {
        fprintf(stderr, "error: cannot create AST, token list empty.\n");
        return NULL;
    }

    jet_ast* ast = (jet_ast*)malloc(sizeof(jet_ast));
        
    if(!ast)
    {
        fprintf(stderr, "error: cannot create jet_ast, failed to alloc mmemory.\n");
        ast = NULL;
        return NULL;
    }
    
    memset(ast, 0, sizeof(jet_ast));
   
    ast->token_list = token_list;

    if(!jet_ast_generate_tree(ast))
    {
        free(ast);
        fprintf(stderr, "error: cannot create jet_ast, failed to generate AST.\n");
        return NULL;
    }

    return ast;
}

bool jet_ast_dispose(jet_ast* ast)
{ 
    if(!ast)
    {
        fprintf(stderr, "error: cannot dispose jet_ast, invalid arg ast.\n");
        return false;
    }
    if(ast->root)
        jet_ast_node_deep_dispose(root);
    free(ast);
    return true;
}

jet_ast_node* jet_ast_node_create(jet_ast_node_type node_type)
{
    jet_ast_node* node = (jet_ast_node*)malloc(sizeof(jet_ast_node));
    node->node_type = node_type;
    return node;
}

static bool jet_ast_generate_tree(jet_ast* ast)
{
    puts("generating ast...");
    assert(ast != NULL);
    jet_ast_node* root = jet_ast_node_create(AST_ROOT);
    root->value.root = (jet_ast_node_root*)malloc(sizeof(jet_ast_node_root));
    assert(root != NULL);
    ast->root = root;
    puts("ast generated successfully!");
    return true;
}

bool jet_ast_node_deep_dispose(jet_ast_node* node)
{
    if(!node) return false;

    switch(node->node_type)
    {
        case AST_ROOT:
            return jet_ast_node_root_dispose(node->value.root);
        case AST_BLOCK:
            return jet_ast_node_
        case AST_BLOCK     
        case AST_DOT_ACCESS
        case AST_BINARY_OP 
        case AST_RETURN    
        case AST_WHILE     
        case AST_VAR_REF   
        case AST_FUNC_DEF  
        case AST_FUNC_CALL 
        case AST_LIST      
        case AST_IDENT     
        case AST_PARAM     
        case AST_TYPE_DECL 
        case AST_UNARY_OP  
        case AST_IF        
        case AST_FOR       
        case AST_VAR_DECL  
        case AST_FUNC_DECL 
        case AST_EXPR_STMT 


        default:
            fprintf(stderr, "wrn: cannot dispose node, unable to find type-corresponding dispose function. (ast_node_type: %d)", (int)node->type):
            return false;
    }
}

static bool jet_ast_node_root_dispose(jet_ast_node_root* root)
{

}

static bool jet_ast_node_block_dispose(jet_ast_node_block* block){}        
static bool jet_ast_node_dot_access(jet_ast_node_dot_access* dot_access){}    
static bool jet_ast_node_binop(jet_ast_node_binary_op* binop){}     
static bool jet_ast_node_return        
static bool jet_ast_node_while         
static bool jet_ast_node_var_ref       
static bool jet_ast_node_func_def      
static bool jet_ast_node_func_call     
static bool jet_ast_node_list       
static bool jet_ast_node_ident       
static bool jet_ast_node_param       
static bool jet_ast_node_type_decl   
static bool jet_ast_node_unary_op  
static bool jet_ast_node_if        
static bool jet_ast_node_for       
static bool jet_ast_node_var_decl  
static bool jet_ast_node_func_decl    
static bool jet_ast_node_expr_stmt   

