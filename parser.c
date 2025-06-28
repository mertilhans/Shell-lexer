#ifndef PARSER_H
# define PARSER_H

# include "tokenizer.h" // Önceki tokenizer kodun

// AST Node türleri
typedef enum e_node_type
{
    NODE_COMMAND,        // Basit komut (ls -la)
    NODE_PIPE,           // Pipe (|)
    NODE_AND,            // And (&&)
    NODE_OR,             // Or (||)
    NODE_SEMICOLON,      // Semicolon (;)
    NODE_PARENTHESES,    // Parantez ()
    NODE_REDIRECT_IN,    // < input
    NODE_REDIRECT_OUT,   // > output
    NODE_REDIRECT_APPEND,// >> append
    NODE_HEREDOC         // << heredoc
} t_node_type;

// Redirect yapısı
typedef struct s_redirect
{
    t_node_type         type;        // Redirect türü
    char                *filename;   // Dosya adı
    struct s_redirect   *next;       // Sonraki redirect
} t_redirect;

// AST Node yapısı
typedef struct s_ast_node
{
    t_node_type         type;        // Node türü
    
    // Command için
    char                **args;      // Komut argümanları ["ls", "-la", NULL]
    t_redirect          *redirects;  // Redirect listesi
    
    // Binary operations için (PIPE, AND, OR)
    struct s_ast_node   *left;       // Sol çocuk
    struct s_ast_node   *right;      // Sağ çocuk
    
    // Parentheses için
    struct s_ast_node   *child;      // İç içerik
    
} t_ast_node;

// Parser yapısı
typedef struct s_parser
{
    t_token     *tokens;      // Token listesi
    t_token     *current;     // Mevcut token
    int         has_error;    // Hata var mı?
    char        *error_msg;   // Hata mesajı
} t_parser;

// Parser fonksiyonları
t_parser    *parser_init(t_token *tokens);
void        parser_free(t_parser *parser);
t_ast_node  *parse(t_token *tokens);
t_ast_node  *parse_expression(t_parser *parser);
t_ast_node  *parse_or(t_parser *parser);
t_ast_node  *parse_and(t_parser *parser);
t_ast_node  *parse_pipe(t_parser *parser);
t_ast_node  *parse_command(t_parser *parser);
t_ast_node  *parse_primary(t_parser *parser);

// AST fonksiyonları
t_ast_node  *ast_node_create(t_node_type type);
void        ast_node_free(t_ast_node *node);
void        ast_print(t_ast_node *root, int depth);

// Redirect fonksiyonları
t_redirect  *redirect_create(t_node_type type, char *filename);
void        redirect_free(t_redirect *redirect);
void        redirect_list_free(t_redirect *redirects);

// Yardımcı fonksiyonlar
int         match_token(t_parser *parser, t_token_type type);
void        advance_parser(t_parser *parser);
int         is_redirect_token(t_token_type type);
char        **create_args_array(t_token *start, t_token *end);

#endif

/* ========================= IMPLEMENTATION ========================= */

// Parser başlatma
t_parser *parser_init(t_token *tokens)
{
    t_parser *parser;

    parser = malloc(sizeof(t_parser));
    if (!parser)
        return (NULL);
    
    parser->tokens = tokens;
    parser->current = tokens;
    parser->has_error = 0;
    parser->error_msg = NULL;
    
    return (parser);
}

// Parser temizleme
void parser_free(t_parser *parser)
{
    if (!parser)
        return;
    
    if (parser->error_msg)
        free(parser->error_msg);
    free(parser);
}

// AST Node oluşturma
t_ast_node *ast_node_create(t_node_type type)
{
    t_ast_node *node;

    node = malloc(sizeof(t_ast_node));
    if (!node)
        return (NULL);
    
    node->type = type;
    node->args = NULL;
    node->redirects = NULL;
    node->left = NULL;
    node->right = NULL;
    node->child = NULL;
    
    return (node);
}

// AST Node temizleme
void ast_node_free(t_ast_node *node)
{
    int i;

    if (!node)
        return;
    
    // Args array temizle
    if (node->args)
    {
        i = 0;
        while (node->args[i])
        {
            free(node->args[i]);
            i++;
        }
        free(node->args);
    }
    
    // Redirects temizle
    redirect_list_free(node->redirects);
    
    // Çocukları temizle
    ast_node_free(node->left);
    ast_node_free(node->right);
    ast_node_free(node->child);
    
    free(node);
}

// Redirect oluşturma
t_redirect *redirect_create(t_node_type type, char *filename)
{
    t_redirect *redirect;

    redirect = malloc(sizeof(t_redirect));
    if (!redirect)
        return (NULL);
    
    redirect->type = type;
    redirect->filename = filename ? strdup(filename) : NULL;
    redirect->next = NULL;
    
    return (redirect);
}

// Redirect temizleme
void redirect_free(t_redirect *redirect)
{
    if (!redirect)
        return;
    
    if (redirect->filename)
        free(redirect->filename);
    free(redirect);
}

// Redirect listesi temizleme
void redirect_list_free(t_redirect *redirects)
{
    t_redirect *current = redirects;
    t_redirect *next;

    while (current)
    {
        next = current->next;
        redirect_free(current);
        current = next;
    }
}

// Token eşleşme kontrolü
int match_token(t_parser *parser, t_token_type type)
{
    return (parser->current && parser->current->type == type);
}

// Sonraki token'a geç
void advance_parser(t_parser *parser)
{
    if (parser->current)
        parser->current = parser->current->next;
}

// Redirect token kontrolü
int is_redirect_token(t_token_type type)
{
    return (type == TOKEN_REDIR_IN || type == TOKEN_REDIR_OUT || 
            type == TOKEN_REDIR_APPEND || type == TOKEN_HEREDOC);
}

/* ====================== RECURSIVE DESCENT PARSER ====================== */

// Ana parse fonksiyonu
t_ast_node *parse(t_token *tokens)
{
    t_parser *parser;
    t_ast_node *root;

    parser = parser_init(tokens);
    if (!parser)
        return (NULL);
    
    root = parse_expression(parser);
    
    parser_free(parser);
    return (root);
}

// En üst seviye: semicolon ayrılmış ifadeler
t_ast_node *parse_expression(t_parser *parser)
{
    t_ast_node *left;
    t_ast_node *node;

    left = parse_or(parser);
    
    while (match_token(parser, TOKEN_SEMICOLON))
    {
        advance_parser(parser); // ; tokenini geç
        
        node = ast_node_create(NODE_SEMICOLON);
        if (!node)
        {
            ast_node_free(left);
            return (NULL);
        }
        
        node->left = left;
        node->right = parse_or(parser);
        left = node;
    }
    
    return (left);
}

// OR operatörü (||) - en düşük öncelik
t_ast_node *parse_or(t_parser *parser)
{
    t_ast_node *left;
    t_ast_node *node;

    left = parse_and(parser);
    
    while (match_token(parser, TOKEN_OR))
    {
        advance_parser(parser); // || tokenini geç
        
        node = ast_node_create(NODE_OR);
        if (!node)
        {
            ast_node_free(left);
            return (NULL);
        }
        
        node->left = left;
        node->right = parse_and(parser);
        left = node;
    }
    
    return (left);
}

// AND operatörü (&&)
t_ast_node *parse_and(t_parser *parser)
{
    t_ast_node *left;
    t_ast_node *node;

    left = parse_pipe(parser);
    
    while (match_token(parser, TOKEN_AND))
    {
        advance_parser(parser); // && tokenini geç
        
        node = ast_node_create(NODE_AND);
        if (!node)
        {
            ast_node_free(left);
            return (NULL);
        }
        
        node->left = left;
        node->right = parse_pipe(parser);
        left = node;
    }
    
    return (left);
}

// PIPE operatörü (|) - en yüksek öncelik
t_ast_node *parse_pipe(t_parser *parser)
{
    t_ast_node *left;
    t_ast_node *node;

    left = parse_command(parser);
    
    while (match_token(parser, TOKEN_PIPE))
    {
        advance_parser(parser); // | tokenini geç
        
        node = ast_node_create(NODE_PIPE);
        if (!node)
        {
            ast_node_free(left);
            return (NULL);
        }
        
        node->left = left;
        node->right = parse_command(parser);
        left = node;
    }
    
    return (left);
}

// Basit komut + redirect parsing
t_ast_node *parse_command(t_parser *parser)
{
    t_ast_node *node;
    t_redirect *redirect;
    t_redirect *last_redirect = NULL;
    
    // Parantez kontrolü
    if (match_token(parser, TOKEN_LPAREN))
        return (parse_primary(parser));
    
    node = ast_node_create(NODE_COMMAND);
    if (!node)
        return (NULL);
    
    // Args toplama
    int arg_count = 0;
    t_token *arg_start = parser->current;
    
    // İlk kelimeyi say (komut adı)
    while (parser->current && match_token(parser, TOKEN_WORD))
    {
        arg_count++;
        advance_parser(parser);
        
        // Redirect varsa dur
        if (parser->current && is_redirect_token(parser->current->type))
            break;
    }
    
    // Args array oluştur
    if (arg_count > 0)
    {
        node->args = malloc((arg_count + 1) * sizeof(char *));
        if (!node->args)
        {
            ast_node_free(node);
            return (NULL);
        }
        
        // Args'ları kopyala
        for (int i = 0; i < arg_count; i++)
        {
            node->args[i] = strdup(arg_start->value);
            arg_start = arg_start->next;
        }
        node->args[arg_count] = NULL;
    }
    
    // Redirections işle
    while (parser->current && is_redirect_token(parser->current->type))
    {
        t_node_type redirect_type;
        
        if (match_token(parser, TOKEN_REDIR_IN))
            redirect_type = NODE_REDIRECT_IN;
        else if (match_token(parser, TOKEN_REDIR_OUT))
            redirect_type = NODE_REDIRECT_OUT;
        else if (match_token(parser, TOKEN_REDIR_APPEND))
            redirect_type = NODE_REDIRECT_APPEND;
        else if (match_token(parser, TOKEN_HEREDOC))
            redirect_type = NODE_HEREDOC;
        else
            break;
        
        advance_parser(parser); // Redirect tokenini geç
        
        // Dosya adı olmalı
        if (!parser->current || !match_token(parser, TOKEN_WORD))
        {
            ast_node_free(node);
            return (NULL);
        }
        
        redirect = redirect_create(redirect_type, parser->current->value);
        if (!redirect)
        {
            ast_node_free(node);
            return (NULL);
        }
        
        // Redirect listesine ekle
        if (!node->redirects)
            node->redirects = redirect;
        else
            last_redirect->next = redirect;
        last_redirect = redirect;
        
        advance_parser(parser); // Dosya adını geç
    }
    
    return (node);
}

// Parantez işleme
t_ast_node *parse_primary(t_parser *parser)
{
    t_ast_node *node;

    if (match_token(parser, TOKEN_LPAREN))
    {
        advance_parser(parser); // ( geç
        
        node = ast_node_create(NODE_PARENTHESES);
        if (!node)
            return (NULL);
        
        node->child = parse_expression(parser);
        
        if (!match_token(parser, TOKEN_RPAREN))
        {
            ast_node_free(node);
            return (NULL);
        }
        
        advance_parser(parser); // ) geç
        return (node);
    }
    
    return (parse_command(parser));
}

// AST yazdırma (debug için)
void ast_print(t_ast_node *root, int depth)
{
    int i;

    if (!root)
        return;
    
    // Indent
    for (i = 0; i < depth; i++)
        printf("  ");
    
    switch (root->type)
    {
        case NODE_COMMAND:
            printf("COMMAND: ");
            if (root->args)
            {
                i = 0;
                while (root->args[i])
                {
                    printf("%s ", root->args[i]);
                    i++;
                }
            }
            printf("\n");
            
            // Redirects yazdır
            t_redirect *r = root->redirects;
            while (r)
            {
                for (int j = 0; j < depth + 1; j++)
                    printf("  ");
                printf("REDIRECT: %s\n", r->filename);
                r = r->next;
            }
            break;
            
        case NODE_PIPE:
            printf("PIPE\n");
            ast_print(root->left, depth + 1);
            ast_print(root->right, depth + 1);
            break;
            
        case NODE_AND:
            printf("AND\n");
            ast_print(root->left, depth + 1);
            ast_print(root->right, depth + 1);
            break;
            
        case NODE_OR:
            printf("OR\n");
            ast_print(root->left, depth + 1);
            ast_print(root->right, depth + 1);
            break;
            
        case NODE_SEMICOLON:
            printf("SEMICOLON\n");
            ast_print(root->left, depth + 1);
            ast_print(root->right, depth + 1);
            break;
            
        case NODE_PARENTHESES:
            printf("PARENTHESES\n");
            ast_print(root->child, depth + 1);
            break;
            
        default:
            printf("UNKNOWN\n");
    }
}

// Test fonksiyonu
int main(void)
{
    char input[] = "ls -la | grep test > output.txt && echo done";
    t_token *tokens;
    t_ast_node *ast;

    printf("Input: %s\n\n", input);
    
    // Tokenize
    tokens = tokenize_input(input);
    if (!tokens)
    {
        printf("Tokenization failed!\n");
        return (1);
    }
    
    // Parse
    ast = parse(tokens);
    if (!ast)
    {
        printf("Parsing failed!\n");
        token_list_free(tokens);
        return (1);
    }
    
    // Print AST
    printf("=== AST ===\n");
    ast_print(ast, 0);
    
    // Cleanup
    token_list_free(tokens);
    ast_node_free(ast);
    
    return (0);
}