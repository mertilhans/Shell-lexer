#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strchr, strncmp

// Basit bir token yapısı
typedef enum {
    TOKEN_WORD,
    TOKEN_PIPE,
    TOKEN_REDIRECT_IN,
    TOKEN_REDIRECT_OUT,
    TOKEN_END
} t_token_type;

typedef struct s_token {
    char        *value;
    t_token_type type;
} t_token;

// Çok basit bir tokenization fonksiyonu // daha fazla geliştirmemiz gerekli yeterli değil aq
// Tırnakları ve diğer karmaşık durumları ele almıyoruz şuanda basic bir
t_token *tokenize_command(const char *input) {
    // Gerçek bir parser için linked list veya dinamik dizi daha uygun o yüzden garbage collector 
    // Bundan devam edebiliriz 
    // Örneğin 10 token ile sınırlı
    t_token *tokens = (t_token *)malloc(sizeof(t_token) * 10);
    if (!tokens) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    int token_idx = 0;
    while (input[i] != '\0' && token_idx < 9) 
    {
        // Boşlukları atla
        while (input[i] == ' ' || input[i] == '\t') 
            i++;
        if (input[i] == '\0') break;

        // Operatörleri kontrol et
        if (input[i] == '|') 
        {
            tokens[token_idx].value = strdup("|");
            tokens[token_idx].type = TOKEN_PIPE;
            i++;
        } 
        else if (input[i] == '<') 
        {
            tokens[token_idx].value = strdup("<");
            tokens[token_idx].type = TOKEN_REDIRECT_IN;
            i++;
        }
        else if (input[i] == '>') 
        {
            tokens[token_idx].value = strdup(">");
            tokens[token_idx].type = TOKEN_REDIRECT_OUT;
            i++;
        }
        else 
        {
            // Kelime token'ı (boşluğa veya operatöre kadar)
            int start = i;
            while (input[i] != '\0' && input[i] != ' ' && input[i] != '\t' &&
                   input[i] != '|' && input[i] != '<' && input[i] != '>') 
                i++;
            int len = i - start;
            tokens[token_idx].value = (char *)malloc(sizeof(char) * (len + 1));
            if (!tokens[token_idx].value) 
            {
                perror("malloc");
                exit(EXIT_FAILURE);
            }
            strncpy(tokens[token_idx].value, input + start, len);
            tokens[token_idx].value[len] = '\0';
            tokens[token_idx].type = TOKEN_WORD;
        }
        token_idx++;
    }
    tokens[token_idx].value = NULL;
    tokens[token_idx].type = TOKEN_END;
    return tokens;
}

// Cleanup
void free_tokens(t_token *tokens) 
{
    if (!tokens) return;
    for (int i = 0; tokens[i].type != TOKEN_END; i++) 
        free(tokens[i].value);
    free(tokens);
}

int main() 
{
    char *command_line = "ls -l | grep .c > output.txt";
    printf("Komut Satırı: \"%s\"\n", command_line);

    t_token *tokens = tokenize_command(command_line);

    if (tokens)
    {
        printf("Ayrıştırılan Tokenlar:\n");
        for (int i = 0; tokens[i].type != TOKEN_END; i++)
            printf("  Token %d: \"%s\" (Tip: %d)\n", i, tokens[i].value, tokens[i].type);
        free_tokens(tokens);
    }

    // optimize etçez anasını sikiyim zormuş bu amk
    // HEREDOC İŞLEMİYORUZ ONU ONUTMAYALIM 
    // strtokr yeterli değil daha fazla genişletme 

    return 0;
}