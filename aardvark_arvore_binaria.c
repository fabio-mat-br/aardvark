
/* Regras de nomenclatura adotadas:
    + nome_s : define uma estrutura
    + nome_t : define um tipo
    + nome_p : define um ponteiro
    + nomes maiusculos representam constantes (macros, enum ou const) */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#define MAXIMO_STRING 256

/* ----------------- Estruturas basicas --------------------- */

typedef enum {
    QUESTAO,
    NOME_ANIMAL
} tipo_t;

typedef enum { /* poderiam ser declarados como macros, mas preferi usar enum */
    NAO = 0,
    SIM = 1
} resposta_t;

typedef struct no_s {
    char info[MAXIMO_STRING]; /* questao ou nome do animal */
    tipo_t tipo;              /* se eh uma questao ou nome de animal */
    struct no_s *ramo[2];     /* ramos do noh, indexados por NAO == 0 ou SIM == 1 */
} no_t;

typedef no_t *no_p;


/* ----------------- Funcoes de Arvore Binaria -------------- */

/* Cria um novo noh com a informacao dada. O novo noh nao tem filhos (eh uma folha). */
no_p cria_novo_noh(char *i) {
    no_p novo = (no_p) malloc(sizeof (no_t));
    strcpy(novo->info, i);    /* copia a informacao do novo noh */
    novo->tipo = NOME_ANIMAL; /* por padrao, todo novo noh sera um nome de animal */
    novo->ramo[NAO] = NULL;   /* filhos do noh sao NULL -- esse noh eh uma folha */
    novo->ramo[SIM] = NULL;
    return novo;
}

/* Adiciona um noh como filho do noh atual, usando ou o ramo NAO ou o ramo SIM.
   ATENCAO: a funcao sobre-escreve o ramo, perdendo o ponteiro do filho anterior que estava ali. */
void adiciona_noh(no_p atual, no_p filho, resposta_t lado) {
    if (atual == NULL) {
        fputs("ERRO: Insercao em um noh nulo\n", stderr);
        abort();
    }
    else {
        atual->ramo[lado] = filho;
    }    
}

/* Desaloca a memoria alocada para a arvore, percorrendo os ramos recursivamente */
no_p libera_arvore(no_p atual) {
    if (atual != NULL) {
        libera_arvore(atual->ramo[NAO]);
        libera_arvore(atual->ramo[SIM]);
        free(atual);
    }
    return NULL;
}


/* ------------------ Funcoes de uso geral ------------------ */

/* Remove os ultimos caraceters de uma string a partir de um dos caracteres fornecidos */
void limpa_string(char *string, char *caracteres) {
    char *posicao = strpbrk(string, caracteres);
    if (posicao != NULL) {
        *posicao = '\0';
    }
}


/* -------------- Funcoes do Programa Aardvark -------------- */

/* Pede uma nova pergunta para desambiguar o animal desconhecido */
void insere_nova_questao(no_p atual) {
    if (atual == NULL || atual->tipo != NOME_ANIMAL) {
        fputs("ERRO: Noh com nome de animal incorreto ou nulo\n", stderr);
        abort();
    }
    else {
        char animal_correto[MAXIMO_STRING];
        char pergunta[MAXIMO_STRING];
        char linha[MAXIMO_STRING];
        resposta_t resp; 
        no_p novo_animal;

        printf("\nEU DESISTO! EM QUE ANIMAL ESTAVA PENSANDO? ");
        fgets(animal_correto, MAXIMO_STRING, stdin);  /* Verifica qual o animal correto */
        limpa_string(animal_correto, "\n");

        printf("QUE PERGUNTA EU POSSO FAZER PARA DISTINGUIR UM(A) %s DE UM(A) %s ?\n> ",
            atual->info, animal_correto);
        fgets(pergunta, MAXIMO_STRING, stdin); /* Obtem uma nova pergunta */
        limpa_string(pergunta, "?\n");
        
        do {
            printf("PARA UM(A) %s, QUAL E A RESPOSTA PARA \"%s\" [S/N]? ", 
                animal_correto, pergunta);       /* Apresenta a nova questao para o novo animal */
            fgets(linha, MAXIMO_STRING, stdin);  /* Le uma linha do teclado */
        } 
        while (toupper(linha[0]) != 'S' && toupper(linha[0]) != 'N');
        resp = toupper(linha[0]) == 'S' ? SIM : NAO;

        /* Aqui eh a parte complicada -- o no atual vai virar uma pergunta, e dois nohs
           vao ser inseridos nele, um para cada animal envolvido na desambiguacao */

        novo_animal = cria_novo_noh(animal_correto); /* Adiciona animal correto no ramo correto */
        adiciona_noh(atual, novo_animal, resp);

        novo_animal = cria_novo_noh(atual->info);    /* Adiciona o animal errado no outro ramo */
        adiciona_noh(atual, novo_animal, (resp == SIM ? NAO : SIM));

        strcpy(atual->info, pergunta); /* Transforma noh atual de nome em pergunta */
        atual->tipo = QUESTAO;                
    }
}

/* Tenta adivinhar o nome do animal. Se acertar, encerra; senao, deve insere nova questao */
void adivinha_animal(no_p atual) {
    if (atual == NULL || atual->tipo != NOME_ANIMAL) {
        fputs("ERRO: Noh com nome de animal incorreto ou nulo\n", stderr);
        abort();
    }
    else {
        char linha[MAXIMO_STRING];
        do {
            printf("VOCE ESTA PENSANDO EM UM(A) %s [S/N]? ", atual->info); /* Apresenta o animal */
            fgets(linha, MAXIMO_STRING, stdin);                            /* Le uma linha do teclado */
        } 
        while (toupper(linha[0]) != 'S' && toupper(linha[0]) != 'N');

        if (toupper(linha[0]) == 'N') {
            insere_nova_questao(atual); /* Nao acertou, entao vai inserir uma pergunta para 
                                           desambiguar esse animal daquele que estava sendo pensado */
        }
        else {
            printf("EU JA IMAGINAVA!\n"); /* Ele acertou! Fim de jogo! */
        }
    }        
}

/* Visita recursivamente cada noh de questao, ate chegar em um nome de animal */
void pergunta_iterativa(no_p atual) {
    if (atual == NULL) {
        fputs("ERRO: Noh nulo encontrado na arvore de questoes\n", stderr);
        abort();
    }
    if (atual->tipo == NOME_ANIMAL) {
        adivinha_animal(atual); /* Se chegou a um noh contendo nome de animal, eh hora de advinhar! */
    }
    else /* atual->tipo == QUESTAO */ {
        char linha[MAXIMO_STRING];
        resposta_t resp; 
        do {
            printf("RESPONDA: %s [S/N]? ", atual->info); /* Apresenta a pergunta */
            fgets(linha, MAXIMO_STRING, stdin);          /* Le uma linha do teclado */
        } 
        while (toupper(linha[0]) != 'S' && toupper(linha[0]) != 'N');

        resp = toupper(linha[0]) == 'S' ? SIM : NAO;
        pergunta_iterativa(atual->ramo[resp]); /* Procede para o ramo correspondente a resposta */
    }
}

/* Inicializa arvore de questoes adicionando uma questao e duas respostas */
no_p inicializa_arvore_questoes(char * questao, char *resp_correta, char *resp_errada) {
    no_p pergunta, resposta;
    pergunta = cria_novo_noh(questao);      /* Cria noh raiz com a questao */
    pergunta->tipo = QUESTAO;               /* Marca o noh raiz como sendo uma questao */
    resposta = cria_novo_noh(resp_correta); /* Cria um noh filho com animal correto */
    adiciona_noh(pergunta, resposta, SIM);  /* Adiciona noh com animal correto no ramo SIM */
    resposta = cria_novo_noh(resp_errada);  /* Cria um noh filho com animal errado */
    adiciona_noh(pergunta, resposta, NAO);  /* Adiciona noh com animal errado no ramo NAO */
    return pergunta;
}


/* --------------------- Funcao Principal ------------------- */

int main() {
    no_p arvore; /* Nossa arvore de questoes */
    arvore = inicializa_arvore_questoes("tem penas", "pato", "cachorro"); /* Inicializa arvore */

    printf("AADVARK: VOCE RESPONDE E EU ADVINHO\n\nPENSE EM UM ANIMAL\n\n");
    do {
        pergunta_iterativa(arvore);
        printf("\n\nPENSE EM OUTRO ANIMAL\n\n");
    } 
    while (1);

    /* O codigo aqui nao eh alcancavel por causa do loop infinito acima. 
       Se fosse deveria ter o seguinte comando:
            arvore = libera_arvore(arvore);
    */
    return 0;
}
