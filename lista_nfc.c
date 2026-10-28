#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

void clear_terminal(void) {
    printf("\033[2J\033[H");
    fflush(stdout);
}

/* Toda entrada consome uma linha, incluindo o Enter. */
int LerTexto(const char *mensagem, char *texto, size_t tamanho) {
    for (;;) {
        printf("%s", mensagem);
        fflush(stdout);

        if (fgets(texto, (int)tamanho, stdin) == NULL) {
            return 0;
        }

        char *quebra = strchr(texto, '\n');
        if (quebra != NULL) {
            *quebra = '\0';
        } else {
            int caractere = getchar();
            if (caractere != '\n' && caractere != EOF) {
                while ((caractere = getchar()) != '\n' && caractere != EOF) {
                }
                printf("Texto muito longo. Use no máximo %zu caracteres.\n", tamanho - 1);
                continue;
            }
        }

        size_t inicio = 0;
        while (isspace((unsigned char)texto[inicio])) {
            inicio++;
        }
        memmove(texto, texto + inicio, strlen(texto + inicio) + 1);
        size_t fim = strlen(texto);
        while (fim > 0 && isspace((unsigned char)texto[fim - 1])) {
            texto[--fim] = '\0';
        }
        return 1;
    }
}

int LerObrigatorio(const char *mensagem, char *texto, size_t tamanho) {
    while (LerTexto(mensagem, texto, tamanho)) {
        if (texto[0] != '\0') {
            return 1;
        }
        printf("Este campo não pode ficar vazio.\n");
    }
    return 0;
}

int LerId(int *id) {
    char texto[32];
    while (LerTexto("ID do aluno (0 para cancelar): ", texto, sizeof(texto))) {
        char *fim;
        errno = 0;
        long numero = strtol(texto, &fim, 10);
        if (fim != texto && *fim == '\0' && errno == 0 && numero >= 0 && numero <= INT_MAX) {
            *id = (int)numero;
            return 1;
        }
        printf("Digite um ID válido, usando apenas números.\n");
    }
    return 0;
}

void Pausar(void) {
    char linha[2];
    LerTexto("\nPressione Enter para voltar ao menu...", linha, sizeof(linha));
}

void Tela(const char *titulo) {
    clear_terminal();
    printf("========================================\n");
    printf("  %s\n", titulo);
    printf("========================================\n\n");
}

struct Aluno {
    int id;
    char nome[50];
    char nfc[20];
    struct Aluno *next;
};

struct Aula {
    int id;
    char nome[50];
    int professor_id;
    int alunos[10];
    struct Aula *next;
};

struct Professor {
    int id;
    char nome[50];
    int aulas[10];
    struct Professor *next;
};

struct Aluno *CadAluno(struct Aluno *lista) {
    struct Aluno *novo = malloc(sizeof(struct Aluno));
    if (novo == NULL) {
        printf("Erro ao alocar memória para o aluno.\n");
        return lista;
    }

    if (!LerObrigatorio("Nome completo: ", novo->nome, sizeof(novo->nome)) ||
        !LerObrigatorio("NFC: ", novo->nfc, sizeof(novo->nfc))) {
        free(novo);
        return lista;
    }

    if (lista != NULL && lista->id == INT_MAX) {
        printf("Não foi possível gerar outro ID.\n");
        free(novo);
        return lista;
    }
    novo->id = (lista == NULL) ? 1 : lista->id + 1;
    novo->next = lista;
    printf("\nAluno cadastrado! ID: %d | Nome: %s\n", novo->id, novo->nome);
    return novo;
}

void ShowAlunos(struct Aluno *alunos) {
    if (alunos == NULL) {
        printf("Nenhum aluno cadastrado. Escolha Cadastrar no menu.\n");
        return;
    }

    int total = 0;
    while (alunos != NULL) {
        printf("ID: %d | Nome: %s | NFC: %s\n",
               alunos->id, alunos->nome, alunos->nfc);
        alunos = alunos->next;
        total++;
    }
    printf("\nTotal de alunos: %d\n", total);
}

void EditAlunos(struct Aluno *lista) {
    if (lista == NULL) {
        ShowAlunos(lista);
        return;
    }

    ShowAlunos(lista);
    printf("\n");
    int id;
    if (!LerId(&id) || id == 0) {
        printf("\nEdição cancelada.\n");
        return;
    }

    struct Aluno *atual = lista;
    while (atual != NULL && atual->id != id) {
        atual = atual->next;
    }
    if (atual == NULL) {
        printf("\nAluno com ID %d não encontrado.\n", id);
        return;
    }

    char nome[50];
    char nfc[20];
    printf("\nEditando: %s | NFC: %s\n", atual->nome, atual->nfc);
    printf("Deixe um campo vazio e pressione Enter para manter o valor atual.\n\n");
    if (!LerTexto("Novo nome: ", nome, sizeof(nome)) ||
        !LerTexto("Novo NFC: ", nfc, sizeof(nfc))) {
        printf("\nEdição cancelada.\n");
        return;
    }
    if (nome[0] == '\0' && nfc[0] == '\0') {
        printf("\nNenhum dado alterado.\n");
        return;
    }
    if (nome[0] != '\0') {
        strcpy(atual->nome, nome);
    }
    if (nfc[0] != '\0') {
        strcpy(atual->nfc, nfc);
    }
    printf("\nAluno atualizado!\nID: %d | Nome: %s | NFC: %s\n",
           atual->id, atual->nome, atual->nfc);
}

void CadProfessor(struct Professor *professor) {

    professor->id = 0;

    printf("Digite o nome do professor: ");

    scanf("%s", professor->nome);

    printf(" \nProfessor cadastrado com sucesso!\n");

}

void CadAula(struct Aula *aula) {

    aula->id = 0;

    printf("Digite o nome da aula: ");

    scanf("%s", aula->nome);

    printf(" \nDigite o ID do professor: ");

    

    scanf("%d", &aula->professor_id);

    printf(" \nAula cadastrada com sucesso!\n");

}

int main(void) {
    char opc[16];
    struct Aluno *alunos = NULL;

    for (;;) {
        Tela("ALUNOS");
        printf("1 - Cadastrar aluno\n");
        printf("2 - Editar aluno\n");
        printf("3 - Listar alunos\n");
        printf("0 - Sair\n\n");

        if (!LerTexto("Escolha uma opção: ", opc, sizeof(opc)) ||
            strcmp(opc, "0") == 0 || strcmp(opc, "x") == 0 || strcmp(opc, "X") == 0) {
            break;
        }

        if (strcmp(opc, "1") == 0) {
            Tela("CADASTRAR ALUNO");
            alunos = CadAluno(alunos);
        } else if (strcmp(opc, "2") == 0) {
            Tela("EDITAR ALUNO");
            EditAlunos(alunos);
        } else if (strcmp(opc, "3") == 0) {
            Tela("LISTA DE ALUNOS");
            ShowAlunos(alunos);
        } else {
            printf("\nOpção inválida. Escolha 1, 2, 3 ou 0.\n");
        }

        if (feof(stdin)) {
            break;
        }
        Pausar();
    }

    while (alunos != NULL) {
        struct Aluno *proximo = alunos->next;
        free(alunos);
        alunos = proximo;
    }
    printf("\nPrograma encerrado.\n");
    return 0;
}
