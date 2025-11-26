#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ---------------------------------------------------------------------------
// Estrutura principal: Modelo de Fórmula 1
// ---------------------------------------------------------------------------
typedef struct {
    char nome[50];       // Nome do modelo (ex: RB20, SF-24, W15)
    int ano;             // Ano de fabricação (ex: 2024)
    float potencia;      // Potência em cavalos (ex: 1050.5)
} Formula1;

// ---------------------------------------------------------------------------
// Protótipos das funções
// ---------------------------------------------------------------------------
void limpaBuffer();
int tamanho(FILE *arq);
void cadastrar(FILE *arq);
void consultar(FILE *arq);
void excluir(FILE *arq);
void gerar_relatorio(FILE *arq);

// ---------------------------------------------------------------------------
// Função auxiliar: limpaBuffer()
// Remove caracteres residuais do buffer após scanf()
// ---------------------------------------------------------------------------
void limpaBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

// ---------------------------------------------------------------------------
// Função: tamanho()
// Retorna o número total de registros armazenados no arquivo binário
// ---------------------------------------------------------------------------
int tamanho(FILE *arq) {
    long bytes;
    fseek(arq, 0, SEEK_END);
    bytes = ftell(arq);
    rewind(arq);
    return (int)(bytes / sizeof(Formula1));
}

// ---------------------------------------------------------------------------
// Função: cadastrar()
// Lê os dados de um novo modelo de Fórmula 1 e grava no arquivo
// ---------------------------------------------------------------------------
void cadastrar(FILE *arq) {
    Formula1 f;

    printf("\n=== Cadastro de Modelo de Fórmula 1 ===\n");

    printf("Nome do modelo (ex: RB20): ");
    fgets(f.nome, sizeof(f.nome), stdin);
    f.nome[strcspn(f.nome, "\n")] = '\0'; // remove o \n do final da string

    printf("Ano do modelo: ");
    scanf("%d", &f.ano);
    limpaBuffer();

    printf("Potência (em cavalos): ");
    scanf("%f", &f.potencia);
    limpaBuffer();

    // Posiciona no final do arquivo e grava o novo registro
    fseek(arq, 0, SEEK_END);
    fwrite(&f, sizeof(Formula1), 1, arq);
    fflush(arq);

    printf("\n✅ Modelo cadastrado com sucesso!\n");
}

// ---------------------------------------------------------------------------
// Função: consultar()
// Solicita o índice e exibe as informações do modelo correspondente
// ---------------------------------------------------------------------------
void consultar(FILE *arq) {
    int pos;
    Formula1 f;
    int total = tamanho(arq);

    if (total == 0) {
        printf("\n⚠ Nenhum modelo cadastrado ainda!\n");
        return;
    }

    printf("\nDigite o índice do modelo (0 até %d): ", total - 1);
    scanf("%d", &pos);
    limpaBuffer();

    if (pos < 0 || pos >= total) {
        printf("\n⚠ Índice inválido! Total de registros: %d\n", total);
        return;
    }

    fseek(arq, pos * sizeof(Formula1), SEEK_SET);
    fread(&f, sizeof(Formula1), 1, arq);

    printf("\n=== Modelo %d ===\n", pos);
    printf("Nome: %s\n", f.nome);
    printf("Ano: %d\n", f.ano);
    printf("Potência: %.2f cavalos\n", f.potencia);
}

// ---------------------------------------------------------------------------
// Função: excluir()
// Remove o registro de índice informado recriando o arquivo sem esse registro
// Mantém o FILE *arq válido em main (usa freopen para truncar e reescrever)
// ---------------------------------------------------------------------------
void excluir(FILE *arq) {
    int pos;
    int total = tamanho(arq);
    Formula1 f;

    if (total == 0) {
        printf("\n⚠ Nenhum modelo cadastrado para excluir!\n");
        return;
    }

    printf("\nDigite o índice do modelo a excluir (0 até %d): ", total - 1);
    scanf("%d", &pos);
    limpaBuffer();

    if (pos < 0 || pos >= total) {
        printf("\n⚠ Índice inválido! Total de registros: %d\n", total);
        return;
    }

    // Cria arquivo temporário e copia todos os registros exceto o escolhido
    FILE *temp = fopen("temp.dat", "w+b");
    if (temp == NULL) {
        printf("\nErro ao criar arquivo temporário!\n");
        return;
    }

    rewind(arq);
    for (int i = 0; i < total; i++) {
        fread(&f, sizeof(Formula1), 1, arq);
        if (i == pos) continue; // pula o registro a ser excluído
        fwrite(&f, sizeof(Formula1), 1, temp);
    }
    fflush(temp);
    rewind(temp);

    // Reabre (trunca) o arquivo original mantendo o mesmo FILE* em main:
    if (freopen("formula1.dat", "w+b", arq) == NULL) {
        printf("\nErro ao reabrir e truncar o arquivo original!\n");
        fclose(temp);
        return;
    }

    // Copia de temp de volta para o arquivo original
    while (fread(&f, sizeof(Formula1), 1, temp) == 1) {
        fwrite(&f, sizeof(Formula1), 1, arq);
    }
    fflush(arq);
    fclose(temp);

    // Remove o arquivo temporário
    remove("temp.dat");

    printf("\n✅ Registro de índice %d excluído com sucesso!\n", pos);
}

// ---------------------------------------------------------------------------
// Função: gerar_relatorio()
// Gera um arquivo texto listando todos os modelos com seus índices
// ---------------------------------------------------------------------------
void gerar_relatorio(FILE *arq) {
    int total = tamanho(arq);
    Formula1 f;

    if (total == 0) {
        printf("\n⚠ Nenhum modelo cadastrado — relatório não gerado.\n");
        return;
    }

    FILE *txt = fopen("relatorio_formula1.txt", "w");
    if (txt == NULL) {
        printf("\nErro ao criar relatorio_formula1.txt\n");
        return;
    }

    rewind(arq);
    fprintf(txt, "Relatório de Modelos de Fórmula 1\n");
    fprintf(txt, "=================================\n");
    fprintf(txt, "Total de registros: %d\n\n", total);

    for (int i = 0; i < total; i++) {
        fread(&f, sizeof(Formula1), 1, arq);
        fprintf(txt, "Índice: %d\n", i);
        fprintf(txt, "Nome: %s\n", f.nome);
        fprintf(txt, "Ano: %d\n", f.ano);
        fprintf(txt, "Potência: %.2f cavalos\n", f.potencia);
        fprintf(txt, "---------------------------------\n");
    }

    fclose(txt);
    printf("\n✅ Relatório gerado: relatorio_formula1.txt\n");
}

// ---------------------------------------------------------------------------
// Função principal (main)
// Controla o menu e abre/cria o arquivo binário
// ---------------------------------------------------------------------------
int main() {
    FILE *arq;
    int opcao;

    // Tenta abrir o arquivo binário, ou cria um novo se não existir
    arq = fopen("formula1.dat", "r+b");
    if (arq == NULL) {
        arq = fopen("formula1.dat", "w+b");
        if (arq == NULL) {
            printf("Erro ao abrir ou criar o arquivo!\n");
            return 1;
        }
    }

    do {
        printf("\n===== MENU PRINCIPAL =====\n");
        printf("1 - Cadastrar modelo\n");
        printf("2 - Consultar modelo por índice\n");
        printf("3 - Mostrar quantidade de registros\n");
        printf("4 - Excluir modelo por índice\n");
        printf("5 - Gerar relatório em .txt\n");
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
            case 4:
                excluir(arq);
                break;
            case 5:
                gerar_relatorio(arq);
                break;
            case 0:
                printf("\nEncerrando o programa...\n");
                break;
            default:
                printf("\n⚠ Opção inválida! Tente novamente.\n");
                break;
        }
    } while (opcao != 0);

    fclose(arq);
    return 0;
}
