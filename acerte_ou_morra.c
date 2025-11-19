#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


typedef struct {
    char* enunciado;
    char** alternativas;  
    int num_alternativas;
    int resposta_correta;  
    char* dica;
    int nivel;
} Pergunta;


typedef struct {
    Pergunta** perguntas;  
    int num_perguntas;
} BancoNivel;

//ser usado no futuro
void liberarPergunta(Pergunta* p) {
    free(p->enunciado);
    for (int i = 0; i < p->num_alternativas; i++) {
        free(p->alternativas[i]);
    }
    free(p->alternativas);
    free(p->dica);
}


void liberarBanco(BancoNivel** bancos, int num_niveis) {
    for (int i = 0; i < num_niveis; i++) {
        for (int j = 0; j < bancos[i]->num_perguntas; j++) {
            liberarPergunta(bancos[i]->perguntas[j]);
            free(bancos[i]->perguntas[j]);
        }
        free(bancos[i]->perguntas);
        free(bancos[i]);
    }
    free(bancos);
}



char* skip_whitespace(char* str) {
    while (*str && (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r')) str++;
    return str;
}

char* parse_string(char* str, char** out) {
    str = skip_whitespace(str);
    if (*str != '"') return NULL;
    str++;
    char* start = str;
    while (*str && *str != '"') {
        if (*str == '\\') str++; 
        str++;
    }
    if (*str != '"') return NULL;
    *out = (char*)malloc(str - start + 1);
    strncpy(*out, start, str - start);
    (*out)[str - start] = '\0';
    return str + 1;
}

char* parse_int(char* str, int* out) {
    str = skip_whitespace(str);
    *out = atoi(str);
    while (*str && *str != ',' && *str != '}' && *str != ']') str++;
    return str;
}

char* parse_array_strings(char* str, char*** out, int* num) {
    str = skip_whitespace(str);
    if (*str != '[') return NULL;
    str++;
    *num = 0;
    *out = NULL;
    while (*str && *str != ']') {
        str = skip_whitespace(str);
        if (*str == '"') {
            char* s;
            str = parse_string(str, &s);
            (*num)++;
            *out = (char**)realloc(*out, *num * sizeof(char*));
            (*out)[*num - 1] = s;
        }
        str = skip_whitespace(str);
        if (*str == ',') str++;
    }
    if (*str != ']') return NULL;
    return str + 1;
}

char* parse_object(char* str, Pergunta* p) {
    str = skip_whitespace(str);
    if (*str != '{') return NULL;
    str++;
    while (*str && *str != '}') {
        str = skip_whitespace(str);
        char* key;
        str = parse_string(str, &key);
        str = skip_whitespace(str);
        if (*str != ':') { free(key); return NULL; }
        str++;
        if (strcmp(key, "enunciado") == 0) {
            str = parse_string(str, &p->enunciado);
        } else if (strcmp(key, "alternativas") == 0) {
            str = parse_array_strings(str, &p->alternativas, &p->num_alternativas);
        } else if (strcmp(key, "resposta_correta") == 0) {
            str = parse_int(str, &p->resposta_correta);
        } else if (strcmp(key, "dica") == 0) {
            str = parse_string(str, &p->dica);
        } else if (strcmp(key, "nivel") == 0) {
            str = parse_int(str, &p->nivel);
        }
        free(key);
        str = skip_whitespace(str);
        if (*str == ',') str++;
    }
    if (*str != '}') return NULL;
    return str + 1;
}

BancoNivel** carregarPerguntas(const char* arquivo, int* num_niveis) {
    FILE* f = fopen(arquivo, "r");
    if (!f) {
        printf("Erro ao abrir arquivo %s\n", arquivo);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* data = (char*)malloc(len + 1);
    fread(data, 1, len, f);
    data[len] = '\0';
    fclose(f);

    char* str = data;
    str = skip_whitespace(str);
    if (*str != '[') {
        free(data);
        printf("Formato JSON inválido\n");
        return NULL;
    }
    str++;

    int max_nivel = 0;
    // Primeiro, contar os níveis
    char* temp = str;
    while (*temp && *temp != ']') {
        Pergunta dummy;
        temp = parse_object(temp, &dummy);
        if (dummy.nivel > max_nivel) max_nivel = dummy.nivel;
        liberarPergunta(&dummy);
        temp = skip_whitespace(temp);
        if (*temp == ',') temp++;
    }

    *num_niveis = max_nivel;
    BancoNivel** bancos = (BancoNivel**)malloc(max_nivel * sizeof(BancoNivel*));
    for (int i = 0; i < max_nivel; i++) {
        bancos[i] = (BancoNivel*)malloc(sizeof(BancoNivel));
        bancos[i]->perguntas = NULL;
        bancos[i]->num_perguntas = 0;
    }

    //  parsear novamente e armazenar
    while (*str && *str != ']') {
        Pergunta* p = (Pergunta*)malloc(sizeof(Pergunta));
        str = parse_object(str, p);
        int nivel = p->nivel - 1;  // Níveis 1-based para 0-based
        bancos[nivel]->num_perguntas++;
        bancos[nivel]->perguntas = (Pergunta**)realloc(bancos[nivel]->perguntas, bancos[nivel]->num_perguntas * sizeof(Pergunta*));
        bancos[nivel]->perguntas[bancos[nivel]->num_perguntas - 1] = p;
        str = skip_whitespace(str);
        if (*str == ',') str++;
    }

    free(data);
    return bancos;
}

// sortear uma pergunta de um nível
Pergunta* sortearPergunta(BancoNivel* banco) {
    if (banco->num_perguntas == 0) return NULL;
    int idx = rand() % banco->num_perguntas;
    return banco->perguntas[idx];
}

// Função para exibir a pergunta
void exibirPergunta(Pergunta* p) {
    printf("Pergunta (Nível %d): %s\n", p->nivel, p->enunciado);
    for (int i = 0; i < p->num_alternativas; i++) {
        printf("%d. %s\n", i + 1, p->alternativas[i]);
    }
}

// Função para mostrar estado do jogo
void mostrarEstado(int nivel_atual, int num_niveis, int vidas, int pular_disp, int trocar_disp, int dica_disp) {
    printf("Estado do Jogo:\n");
    printf("Nível atual: %d\n", nivel_atual);
    printf("Total de níveis: %d\n", num_niveis);
    printf("Vidas: %d\n", vidas);
    printf("Ações especiais disponíveis:\n");
    printf("  Pular: %s\n", pular_disp ? "Sim" : "Não");
    printf("  Trocar: %s\n", trocar_disp ? "Sim" : "Não");
    printf("  Dica: %s\n", dica_disp ? "Sim" : "Não");
}


int main() {
    srand(time(NULL));

    int num_niveis;
    BancoNivel** bancos = carregarPerguntas("perguntas.json", &num_niveis);
    if (!bancos) {
        return 1;
    }

    int nivel_atual = 1;
    int vidas = 1;
    int pular_disp = 1;
    int trocar_disp = 1;
    int dica_disp = 1;
    Pergunta* pergunta_atual = sortearPergunta(bancos[nivel_atual - 1]);

    while (1) {
        printf("\nMenu:\n");
        printf("1. Responder pergunta\n");
        printf("2. Sair do jogo\n");
        printf("3. Utilizar ação especial (pular, trocar ou dica)\n");
        printf("4. Mostrar estado do jogo\n");
        printf("5. Exibir pergunta atual novamente\n");
        printf("Escolha uma opção: ");

        int opcao;
        scanf("%d", &opcao);
        getchar();  // Consumir newline

        if (opcao == 1) {
            // Responder pergunta
            if (!pergunta_atual) {
                printf("Nenhuma pergunta disponível para este nível.\n");
                continue;
            }
            exibirPergunta(pergunta_atual);
            printf("Digite o número da alternativa (1-%d): ", pergunta_atual->num_alternativas);
            int resposta;
            scanf("%d", &resposta);
            getchar();
            if (resposta - 1 == pergunta_atual->resposta_correta) {
                printf("Correto! Avançando para o próximo nível.\n");
                nivel_atual++;
                if (nivel_atual > num_niveis) {
                    printf("Parabéns! Você venceu o jogo!\n");
                    break;
                }
                pergunta_atual = sortearPergunta(bancos[nivel_atual - 1]);
            } else {
                printf("Errado! Você perdeu uma vida.\n");
                vidas--;
                if (vidas <= 0) {
                    printf("Game Over! Você perdeu todas as vidas.\n");
                    break;
                }
            }
        } else if (opcao == 2) {
            // Sair do jogo
            printf("Saindo do jogo...\n");
            break;
        } else if (opcao == 3) {
            // Utilizar ação especial
            printf("Escolha a ação especial:\n");
            printf("1. Pular\n");
            printf("2. Trocar\n");
            printf("3. Dica\n");
            printf("Escolha: ");
            int acao;
            scanf("%d", &acao);
            getchar();
            if (acao == 1 && pular_disp) {
                printf("Pulando pergunta. Avançando para o próximo nível.\n");
                pular_disp = 0;
                nivel_atual++;
                if (nivel_atual > num_niveis) {
                    printf("Parabéns! Você venceu o jogo!\n");
                    break;
                }
                pergunta_atual = sortearPergunta(bancos[nivel_atual - 1]);
            } else if (acao == 2 && trocar_disp) {
                printf("Trocando pergunta.\n");
                trocar_disp = 0;
                pergunta_atual = sortearPergunta(bancos[nivel_atual - 1]);
            } else if (acao == 3 && dica_disp) {
                printf("Dica: %s\n", pergunta_atual->dica);
                dica_disp = 0;
            } else {
                printf("Ação não disponível ou inválida.\n");
            }
        } else if (opcao == 4) {
            // Mostrar estado
            mostrarEstado(nivel_atual, num_niveis, vidas, pular_disp, trocar_disp, dica_disp);
        } else if (opcao == 5) {
            // Exibir pergunta novamente
            if (pergunta_atual) {
                exibirPergunta(pergunta_atual);
            } else {
                printf("Nenhuma pergunta atual.\n");
            }
        } else {
            printf("Opção inválida.\n");
        }
    }

    liberarBanco(bancos, num_niveis);
    return 0;
}