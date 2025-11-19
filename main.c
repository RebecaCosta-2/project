#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// =======================================================
// STRUCT PRINCIPAL
// =======================================================
typedef struct {
    char nome[50];
    int codigo;
    float preco;
} Produto;

// =======================================================
// PROTÓTIPOS
// =======================================================
void limpaBuffer();
int tamanho(FILE *arq);
void cadastrar(FILE *arq);
void consultar(FILE *arq);

// =======================================================
// FUNÇÃO limpaBuffer()
// =======================================================
void limpaBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

// =======================================================
// FUNÇÃO tamanho()
// =======================================================
int tamanho(FILE *arq) {
    long bytes;
    fseek(arq, 0, SEEK_END);
    bytes = ftell(arq);
    rewind(arq); // volta para o início após medir
    return (int)(bytes / sizeof(Produto));
}

// =======================================================
// FUNÇÃO cadastrar()
// =======================================================
void cadastrar(FILE *arq) {
    Produto p;

    printf("\n=== Cadastro de Produto ===\n");

    printf("Nome do produto: ");
    fgets(p.nome, sizeof(p.nome), stdin);
    p.nome[strcspn(p.nome, "\n")] = '\0'; // remove o \n

    printf("Código do produto: ");
    scanf("%d", &p.codigo);
    limpaBuffer();

    printf("Preço do produto: ");
    scanf("%f", &p.preco);
    limpaBuffer();

    fseek(arq, 0, SEEK_END);
    fwrite(&p, sizeof(Produto), 1, arq);
    fflush(arq);

    printf("\n✅ Produto cadastrado com sucesso!\n");
}

// =======================================================
// FUNÇÃO consultar()
// =======================================================
void consultar(FILE *arq) {
    int pos;
    Produto p;
    int total = tamanho(arq);

    if (total == 0) {
        printf("\nNenhum produto cadastrado ainda!\n");
        return;
    }

    printf("\nDigite o índice do produto (0 até %d): ", total - 1);
    scanf("%d", &pos);
    limpaBuffer();

    if (pos < 0 || pos >= total) {
        printf("\n⚠ Índice inválido! Total de registros: %d\n", total);
        return;
    }

    fseek(arq, pos * sizeof(Produto), SEEK_SET);
    fread(&p, sizeof(Produto), 1, arq);

    printf("\n=== Produto %d ===\n", pos);
    printf("Nome: %s\n", p.nome);
    printf("Código: %d\n", p.codigo);
    printf("Preço: R$ %.2f\n", p.preco);
}

// =======================================================
// FUNÇÃO main()
// =======================================================
int main() {
    FILE *arq;
    int opcao;

    // Em ambientes online, o arquivo é salvo no diretório temporário
    arq = fopen("produtos.dat", "r+b");
    if (arq == NULL) {
        arq = fopen("produtos.dat", "w+b");
        if (arq == NULL) {
            printf("Erro ao abrir ou criar o arquivo!\n");
            return 1;
        }
    }

    do {
        printf("\n===== MENU =====\n");
        printf("1 - Cadastrar produto\n");
        printf("2 - Consultar produto por índice\n");
        printf("3 - Mostrar quantidade de registros\n");
        printf("0 - Sair\n");
        printf("Escolha: ");
        scanf("%d", &opcao);
        limpaBuffer();

        switch (opcao) {
            case 1:
                cadastrar(arq);
                break;
            case 2:
                consultar(arq);
                break;
            case 3:
                printf("\n📦 Total de registros: %d\n", tamanho(arq));
                break;
            case 0:
                printf("\nEncerrando o programa...\n");
                break;
            default:
                printf("\n⚠ Opção inválida!\n");
                break;
        }

    } while (opcao != 0);

    fclose(arq);
    return 0;
}
