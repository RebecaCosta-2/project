#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARQ_BIN "modelos_f1.bin"
#define MAX 50

typedef struct {
    int id;
    char equipe[MAX];
    char modelo[MAX];
    int ano;
    float potencia;
} ModeloF1;

void cadastrarModelo();
void consultarModelo();
void excluirModelo();
void gerarRelatorio();
void menu();

int main() {
    menu();
    return 0;
}

void menu() {
    int op;
    do {
        printf("\n===== SISTEMA DE MODELOS DE FORMULA 1 =====\n");
        printf("1 - Cadastrar modelo\n");
        printf("2 - Consultar modelo por ID\n");
        printf("3 - Excluir modelo por ID\n");
        printf("4 - Gerar relatório (TXT)\n");
        printf("5 - Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &op);

        switch(op) {
            case 1: cadastrarModelo(); break;
            case 2: consultarModelo(); break;
            case 3: excluirModelo(); break;
            case 4: gerarRelatorio(); break;
            case 5: printf("Encerrando...\n"); break;
            default: printf("Opção inválida!\n");
        }
    } while(op != 5);
}

void cadastrarModelo() {
    FILE *fp = fopen(ARQ_BIN, "ab");
    if(!fp) {
        printf("Erro ao abrir arquivo.\n");
        return;
    }

    ModeloF1 m;

    printf("ID do modelo: ");
    scanf("%d", &m.id);

    printf("Equipe: ");
    getchar();
    fgets(m.equipe, MAX, stdin);
    m.equipe[strcspn(m.equipe, "\n")] = 0;

    printf("Modelo: ");
    fgets(m.modelo, MAX, stdin);
    m.modelo[strcspn(m.modelo, "\n")] = 0;

    printf("Ano: ");
    scanf("%d", &m.ano);

    printf("Potência (HP): ");
    scanf("%f", &m.potencia);

    fwrite(&m, sizeof(ModeloF1), 1, fp);
    fclose(fp);

    printf("Modelo cadastrado com sucesso!\n");
}

void consultarModelo() {
    FILE *fp = fopen(ARQ_BIN, "rb");
    if(!fp) {
        printf("Arquivo inexistente.\n");
        return;
    }

    int id;
    printf("Informe o ID para consulta: ");
    scanf("%d", &id);

    ModeloF1 m;
    int encontrado = 0;

    while(fread(&m, sizeof(ModeloF1), 1, fp)) {
        if(m.id == id) {
            printf("\n--- Modelo Encontrado ---\n");
            printf("ID: %d\n", m.id);
            printf("Equipe: %s\n", m.equipe);
            printf("Modelo: %s\n", m.modelo);
            printf("Ano: %d\n", m.ano);
            printf("Potência: %.1f HP\n", m.potencia);
            encontrado = 1;
            break;
        }
    }

    if(!encontrado) printf("Modelo não encontrado!\n");

    fclose(fp);
}

void excluirModelo() {
    FILE *fp = fopen(ARQ_BIN, "rb");
    if(!fp) {
        printf("Arquivo inexistente.\n");
        return;
    }

    FILE *temp = fopen("temp.bin", "wb");

    int id;
    printf("Informe o ID para exclusão: ");
    scanf("%d", &id);

    ModeloF1 m;
    int removido = 0;

    while(fread(&m, sizeof(ModeloF1), 1, fp)) {
        if(m.id != id) {
            fwrite(&m, sizeof(ModeloF1), 1, temp);
        } else {
            removido = 1;
        }
    }

    fclose(fp);
    fclose(temp);

    remove(ARQ_BIN);
    rename("temp.bin", ARQ_BIN);

    if(removido)
        printf("Modelo removido com sucesso!\n");
    else
        printf("ID não encontrado!\n");
}

void gerarRelatorio() {
    FILE *fp = fopen(ARQ_BIN, "rb");
    if(!fp) {
        printf("Nenhum dado para gerar relatório.\n");
        return;
    }

    FILE *txt = fopen("relatorio_modelos_f1.txt", "w");

    ModeloF1 m;

    fprintf(txt, "======= RELATÓRIO DE MODELOS DE F1 =======\n\n");

    while(fread(&m, sizeof(ModeloF1), 1, fp)) {
        fprintf(txt, "ID: %d\n", m.id);
        fprintf(txt, "Equipe: %s\n", m.equipe);
        fprintf(txt, "Modelo: %s\n", m.modelo);
        fprintf(txt, "Ano: %d\n", m.ano);
        fprintf(txt, "Potência: %.1f HP\n", m.potencia);
        fprintf(txt, "---------------------------------------\n");
    }

    fclose(fp);
    fclose(txt);

    printf("Relatório gerado: relatorio_modelos_f1.txt\n");
}
