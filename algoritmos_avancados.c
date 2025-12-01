#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Desafio Detective Quest
// Tema 4 - Árvores e Tabela Hash
 
typedef struct Sala {
    char nome[64];
    struct Sala *esquerda;
    struct Sala *direita;
    int visitada; /* 0 = não visitada, 1 = visitada */
} Sala;

/* Funções para Salas (árvore binária simples) */
void criarSala(Sala **sala, const char *nome) {
    if (sala == NULL) return;
    *sala = (Sala *)malloc(sizeof(Sala));
    if (*sala == NULL) {
        fprintf(stderr, "malloc failed\n");
        exit(EXIT_FAILURE);
    }
    strncpy((*sala)->nome, nome, sizeof((*sala)->nome) - 1);
    (*sala)->nome[sizeof((*sala)->nome) - 1] = '\0';
    (*sala)->esquerda = NULL;
    (*sala)->direita = NULL;
    (*sala)->visitada = 0;
}

void conectarSalas(Sala *pai, Sala *esquerda, Sala *direita) {
    if (pai == NULL) return;
    pai->esquerda = esquerda;
    pai->direita = direita;
}

/* Pista: BST */
typedef struct Pista {
    char texto[256];
    struct Pista *esquerda;
    struct Pista *direita;
} Pista;

Pista* criarPista(const char *texto) {
    Pista* nova = (Pista*)malloc(sizeof(Pista));
    if (!nova) {
        fprintf(stderr, "malloc failed\n");
        exit(EXIT_FAILURE);
    }
    strncpy(nova->texto, texto, sizeof(nova->texto) - 1);
    nova->texto[sizeof(nova->texto) - 1] = '\0';
    nova->esquerda = nova->direita = NULL;
    return nova;
}

 
void inserirBST(Pista** raiz, const char* texto) {
    if (raiz == NULL) return;
    if (*raiz == NULL) {
        *raiz = criarPista(texto);
        return;
    }
    int cmp = strcmp(texto, (*raiz)->texto);
    if (cmp < 0) {
        inserirBST(&((*raiz)->esquerda), texto);
    } else if (cmp > 0) {
        inserirBST(&((*raiz)->direita), texto);
    } else {
        /* texto igual: não inserir duplicata */
        return;
    }
}

void emOrdem(Pista* raiz) {
    if (raiz == NULL) return;
    emOrdem(raiz->esquerda);
    printf("%s\n", raiz->texto);
    emOrdem(raiz->direita);
}

void inserirPista(Pista** raiz, const char* texto) {
    inserirBST(raiz, texto);
}

void listarPistas(Pista* raiz) {
    printf("Pistas coletadas:\n");
    if (raiz == NULL) {
        printf("  (nenhuma)\n");
    } else {
        emOrdem(raiz);
    }
}

/* Funções para liberar memória da árvore de pistas */
void liberarPistas(Pista* raiz) {
    if (!raiz) return;
    liberarPistas(raiz->esquerda);
    liberarPistas(raiz->direita);
    free(raiz);
}

/* Hash de Suspeitos */
typedef struct PistaNode {
    char texto[256];
    struct PistaNode* proximo;
} PistaNode;

typedef struct Suspeito {
    char nome[64];
    PistaNode* pistas;
    struct Suspeito* proximo;
} Suspeito;

unsigned int funcaoHash(const char* chave, int tamanho) {
    unsigned int soma = 0;
    for (int i = 0; chave && chave[i] != '\0'; i++) soma += (unsigned char)chave[i];
    return (tamanho > 0) ? (soma % (unsigned int)tamanho) : 0;
}

void inicializarHash(Suspeito** tabela, int tamanho) {
    if (!tabela) return;
    for (int i = 0; i < tamanho; i++) tabela[i] = NULL;
}

Suspeito* buscarSuspeito(Suspeito** tabela, int tamanho, const char* suspeitoNome) {
    if (!tabela || !suspeitoNome) return NULL;
    unsigned int indice = funcaoHash(suspeitoNome, tamanho);
    Suspeito* atual = tabela[indice];
    while (atual != NULL) {
        if (strcmp(atual->nome, suspeitoNome) == 0) return atual;
        atual = atual->proximo;
    }
    return NULL;
}

void inserirHash(Suspeito** tabela, int tamanho, const char* pista, const char* suspeitoNome) {
    if (!tabela || !suspeitoNome) return;
    unsigned int indice = funcaoHash(suspeitoNome, tamanho);
    Suspeito* atual = tabela[indice];
    while (atual != NULL) {
        if (strcmp(atual->nome, suspeitoNome) == 0) {
            /* não inserir pista vazia/nula */
            if (!pista || pista[0] == '\0') return;
            /* verificar se a pista já existe para este suspeito */
            PistaNode* p = atual->pistas;
            while (p != NULL) {
                if (strcmp(p->texto, pista) == 0) return; /* já existe: não duplicar */
                p = p->proximo;
            }
            /* inserir nova pista no início da lista */
            PistaNode* novaP = (PistaNode*)malloc(sizeof(PistaNode));
            if (!novaP) { fprintf(stderr, "malloc failed\n"); exit(EXIT_FAILURE); }
            strncpy(novaP->texto, pista, sizeof(novaP->texto) - 1);
            novaP->texto[sizeof(novaP->texto) - 1] = '\0';
            novaP->proximo = atual->pistas;
            atual->pistas = novaP;
            return;
        }
        atual = atual->proximo;
    }
    /* criar novo suspeito */
    Suspeito* novo = (Suspeito*)malloc(sizeof(Suspeito));
    if (!novo) { fprintf(stderr, "malloc failed\n"); exit(EXIT_FAILURE); }
    strncpy(novo->nome, suspeitoNome, sizeof(novo->nome) - 1);
    novo->nome[sizeof(novo->nome) - 1] = '\0';
    novo->pistas = NULL;
    novo->proximo = tabela[indice];
    tabela[indice] = novo;
    if (pista && pista[0] != '\0') {
        PistaNode* novaP = (PistaNode*)malloc(sizeof(PistaNode));
        if (!novaP) { fprintf(stderr, "malloc failed\n"); exit(EXIT_FAILURE); }
        strncpy(novaP->texto, pista, sizeof(novaP->texto) - 1);
        novaP->texto[sizeof(novaP->texto) - 1] = '\0';
        novaP->proximo = novo->pistas;
        novo->pistas = novaP;
    }
}

void listarAssociacoes(Suspeito** tabela, int tamanho) {
    if (!tabela) return;
    for (int i = 0; i < tamanho; i++) {
        Suspeito* atual = tabela[i];
        while (atual != NULL) {
            printf("Suspeito: %s\n", atual->nome);
            PistaNode* p = atual->pistas;
            while (p != NULL) {
                printf("  Pista: %s\n", p->texto);
                p = p->proximo;
            }
            atual = atual->proximo;
        }
    }
}

void listarSuspeitos(Suspeito** tabela, int tamanho) {
    listarAssociacoes(tabela, tamanho);
}

void encontrarSuspeitoMaisProvavel(Suspeito** tabela, int tamanho) {
    if (!tabela) { printf("Nenhum suspeito encontrado.\n"); return; }
    Suspeito* mais = NULL;
    int max = 0;
    for (int i = 0; i < tamanho; i++) {
        Suspeito* atual = tabela[i];
        while (atual != NULL) {
            int cnt = 0;
            PistaNode* p = atual->pistas;
            while (p) { cnt++; p = p->proximo; }
            if (cnt > max) { max = cnt; mais = atual; }
            atual = atual->proximo;
        }
    }
    if (mais) printf("Suspeito mais provável: %s com %d pistas.\n", mais->nome, max);
    else printf("Nenhum suspeito encontrado.\n");
}

/* Função para liberar toda a tabela hash (suspeitos e suas listas de pistas) */
void liberarHash(Suspeito** tabela, int tamanho) {
    if (!tabela) return;
    for (int i = 0; i < tamanho; i++) {
        Suspeito* atual = tabela[i];
        while (atual != NULL) {
            PistaNode* p = atual->pistas;
            while (p != NULL) {
                PistaNode* tmpP = p->proximo;
                free(p);
                p = tmpP;
            }
            Suspeito* tmpS = atual->proximo;
            free(atual);
            atual = tmpS;
        }
        tabela[i] = NULL;
    }
}

/* Exploração: percorre a árvore de salas; ao entrar em salas específicas, coleta pistas */
 
void explorarSalas(Sala *sala, Pista** raiz) {
    if (!sala) return;
    char buf[32];
    while (1) {
        printf("\nVocê está na sala: %s\n", sala->nome);
        if (!sala->visitada) {
            char pistaBuf[256];
            snprintf(pistaBuf, sizeof(pistaBuf), "Pista encontrada em %s", sala->nome);
            inserirPista(raiz, pistaBuf);
            sala->visitada = 1;
        } else {
            printf("Você já coletou a pista desta sala.\n");
        }

        printf("Escolha uma direção (e: esquerda, d: direita, s: sair): ");
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            /* EOF ou erro de leitura: sair */
            printf("\nEntrada não disponível. Saindo da exploração.\n");
            break;
        }
        /* pegar primeiro caractere não-espaco */
        char escolha = '\0';
        for (size_t i = 0; i < strlen(buf); i++) {
            if (buf[i] != ' ' && buf[i] != '\t' && buf[i] != '\n' && buf[i] != '\r') { escolha = buf[i]; break; }
        }
        if (escolha == 'e') {
            if (sala->esquerda != NULL) sala = sala->esquerda;
            else printf("Não há sala à esquerda.\n");
        } else if (escolha == 'd') {
            if (sala->direita != NULL) sala = sala->direita;
            else printf("Não há sala à direita.\n");
        } else if (escolha == 's') {
            break;
        } else {
            printf("Direção inválida.\n");
        }
    }
}

/* Função para liberar a árvore de salas   */
void liberarSalas(Sala* raiz) {
    if (!raiz) return;
    liberarSalas(raiz->esquerda);
    liberarSalas(raiz->direita);
    free(raiz);
}

/* Função principal: monta a mansão, coleta pistas e relaciona suspeitos */
int main(void) {
    // montar salas
    Sala *hall = NULL, *biblioteca = NULL, *cozinha = NULL, *sotao = NULL;
    criarSala(&hall, "Hall de Entrada");
    criarSala(&biblioteca, "Biblioteca");
    criarSala(&cozinha, "Cozinha");
    criarSala(&sotao, "Sótão");
    conectarSalas(hall, biblioteca, cozinha);
    conectarSalas(biblioteca, sotao, NULL);

    // árvore de pistas
    Pista* raiz = NULL;

    // explorar salas e coletar pistas automaticamente
    printf("Iniciando exploração (pistas serão coletadas automaticamente ao entrar nas salas).\n");
    explorarSalas(hall, &raiz);

    // listar pistas coletadas
    printf("\nRevisando pistas coletadas:\n");
    listarPistas(raiz);

    // tabela hash de suspeitos
    const int TAM = 101;
    Suspeito** tabela = (Suspeito**)calloc(TAM, sizeof(Suspeito*));
    if (!tabela) { fprintf(stderr, "calloc failed\n"); /* liberar recursos antes de sair */ liberarPistas(raiz); liberarSalas(hall); return EXIT_FAILURE; }
    inicializarHash(tabela, TAM);

    // Exemplo: associar algumas pistas a suspeitos  
    inserirHash(tabela, TAM, "Pista encontrada em Biblioteca", "Sr. Verde");
    inserirHash(tabela, TAM, "Pista encontrada em Cozinha", "Sra. Rosa");
    inserirHash(tabela, TAM, "Pista encontrada em Biblioteca", "Sr. Verde");
    inserirHash(tabela, TAM, "Pista encontrada em Sótão", "Prof. Azul");

    // listar associações e encontrar suspeito mais provável
    printf("\nListando suspeitos e pistas:\n");
    listarSuspeitos(tabela, TAM);

    printf("\nDeterminando o suspeito mais provável:\n");
    encontrarSuspeitoMaisProvavel(tabela, TAM);

    // liberar memória alocada corretamente
    liberarHash(tabela, TAM);
    free(tabela);
    liberarPistas(raiz);
    liberarSalas(hall);

    return 0;
}

