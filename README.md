# faculdade
# Acerte ou Morra --- Jogo de Perguntas e Respostas em C

##  Sobre o Jogo

Este é um jogo de perguntas e respostas no console, desenvolvido em
linguagem C.\
O jogador começa com **1 vida** e precisa responder perguntas de
dificuldade crescente. As perguntas são carregadas de um arquivo
**JSON**, que contém bancos de perguntas separados por nível.

O jogo oferece três ações especiais, cada uma utilizável **uma única
vez** por partida:

-   **Pular pergunta** → Avança para o próximo nível sem responder\
-   **Trocar pergunta** → Sorteia outra pergunta do mesmo nível\
-   **Dica** → Mostra a dica da pergunta atual

O jogador vence o jogo ao acertar a pergunta do **último nível**.

------------------------------------------------------------------------

#  Como Compilar o Código

É necessário ter um compilador C instalado (GCC, Clang, MinGW etc.).

### Linux / macOS / Windows (MinGW):

``` bash
gcc -o acerte_ou_morra acerte_ou_morra.c
```

Isso gera o executável:

-   `acerte_ou_morra` (Linux/macOS)\
-   `acerte_ou_morra.exe` (Windows)

------------------------------------------------------------------------

#  Como Executar o Jogo

Coloque o arquivo **`perguntas.json`** na mesma pasta do executável e
execute:

### Linux/macOS

``` bash
./acerte_ou_morra
```

### Windows

``` bash
acerte_ou_morra.exe
```

------------------------------------------------------------------------

#  Estrutura dos Arquivos

    /pasta_do_jogo/
       acerte_ou_morra.c
       acerte_ou_morra.exe     (opcional – gerado após compilação)
       perguntas.json
       README.md

------------------------------------------------------------------------

#  Como Criar o Arquivo JSON com os Bancos de Perguntas

O arquivo `perguntas.json` contém **todas as perguntas do jogo**,
independentemente do nível de dificuldade.\
Cada pergunta deve estar em um **objeto JSON**, e o arquivo inteiro deve
ser um **array** contendo essas perguntas.

###  Campos obrigatórios de cada pergunta:

  ------------------------------------------------------------------------
  Campo                            Tipo                Descrição
  -------------------------------- ------------------- -------------------
  **enunciado**                    string              Texto da pergunta

  **alternativas**                 array               Lista de
                                                       alternativas
                                                       (quantas quiser)

  **resposta_correta**             número              Índice da
                                                       alternativa correta
                                                       (começa em 0)

  **dica**                         string              Texto da dica

  **nivel**                        número              Nível de
                                                       dificuldade (1, 2,
                                                       3...)
  ------------------------------------------------------------------------

###  Quantidades totalmente flexíveis:

-   Você pode ter **quantos níveis quiser**\
-   Quantas perguntas quiser por nível\
-   Quantas alternativas quiser em cada pergunta\
-   O programa se adapta automaticamente

------------------------------------------------------------------------

#  Exemplo de `perguntas.json`

    [
      {
        "enunciado": "Qual a capital do Brasil?",
        "alternativas": ["São Paulo", "Brasília", "Rio de Janeiro"],
        "resposta_correta": 1,
        "dica": "Foi planejada e inaugurada em 1960.",
        "nivel": 1
      },
      {
        "enunciado": "Quanto é 5 * 3?",
        "alternativas": ["15", "10", "20"],
        "resposta_correta": 0,
        "dica": "Multiplicação básica",
        "nivel": 1
      }
    ]

------------------------------------------------------------------------

# 📌 Regras do Jogo

-   O jogador começa com **1 vida**\
-   Resposta errada → perde 1 vida\
-   Vida chega a 0 → game over\
-   Resposta correta → avança para o próximo nível\
-   Vitória → responder corretamente a pergunta do último nível

### Ações especiais (uma vez por partida):

-   **Pular** → ignora a pergunta e avança nível\
-   **Trocar** → nova pergunta do mesmo nível\
-   **Dica** → mostra a dica da pergunta

------------------------------------------------------------------------

# 🏁 Pronto!

Este README contém: - Instruções de compilação\
- Instruções de uso\
- Explicação de criação do JSON\
- Estrutura completa do jogo
