#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

// Variaveis Globais
#define LARGURA_TELA 800
#define ALTURA_TELA 600
#define COR_FUNDO (Color){59, 34, 23, 255}

#define ESCURO (Color){23, 20, 19, 255}
#define CLARO (Color){224, 223, 222, 255}

#define TAMANHO 70

#define COLUNA 8
#define LINHA 8
#define TABULEIRO_X 120
#define TABULEIRO_Y 40

#define PECAS 12

int tabuleiro[8][8];
int movimentos[8][8];

int turno = 1;

Vector2 *historico = NULL;

int quantidadeMovimentos = 0;
int capacidadeHistorico = 0;

Vector2 ultimaPosicao;

//Adicionar no historico 
void AdicionarHistorico(Vector2 posicao){
    if(quantidadeMovimentos >= capacidadeHistorico){
        int novaCapacidade = capacidadeHistorico == 0 ? 8 : capacidadeHistorico * 2;
        Vector2 *temp = realloc( historico, novaCapacidade * sizeof(Vector2));
        if (temp == NULL)
        {
            printf("Erro ao alocar memoria!\n");
            return;
        }
        historico = temp;
        capacidadeHistorico = novaCapacidade;
    }
    historico[quantidadeMovimentos] = posicao;
    quantidadeMovimentos++;
    ultimaPosicao = posicao;
}

// GUIA TABULEIRO 0=vazio 1=branca 2=pretas

void RedefinirTabuleiro(){
    int peca_preta = 0;
    int peca_branca = 0;
    for (int linha = 0; linha < COLUNA; linha++)
    {
        for (int coluna = 0; coluna < LINHA; coluna++)
        {
            if ((linha+coluna) % 2 == 1)
            {
                if(peca_preta<PECAS && linha<=2){
                    tabuleiro[linha][coluna] = 2;
                    peca_preta++;
                    continue;
                }
                if(peca_branca<PECAS && linha>=5){
                    tabuleiro[linha][coluna] = 1;
                    peca_branca++;
                    continue;
                }
            }
            tabuleiro[linha][coluna] = 0;
        }

    }
    printf("Tabuleiro Redefinido");

}

int VerificarVencedor()
{
    int pecasBrancas = 0;
    int pecasPretas = 0;

    for (int linha = 0; linha < LINHA; linha++)
    {
        for (int coluna = 0; coluna < COLUNA; coluna++)
        {
            if (tabuleiro[linha][coluna] == 1) pecasBrancas++;
            if (tabuleiro[linha][coluna] == 2) pecasPretas++;
        }
    }

    if (pecasBrancas == 0) return 2;
    if (pecasPretas == 0) return 1;
    return 0;
}

void DrawTable()
{
    const char *textoTurno = turno == 1 ? "Turno das brancas" : "Turno das pretas";
    int tamanhoTexto = 20;
    int larguraFundo = MeasureText("Turno das brancas", tamanhoTexto) + 12;
    int larguraTexto = MeasureText(textoTurno, tamanhoTexto);
    DrawRectangle((LARGURA_TELA - larguraFundo) / 2, 6, larguraFundo, 28, BLACK);
    DrawText(textoTurno, (LARGURA_TELA - larguraTexto) / 2, 10, tamanhoTexto, WHITE);

    for (int linha = 0; linha < COLUNA; linha++)
    {
        for (int coluna = 0; coluna < LINHA; coluna++)
        {
            int x = TABULEIRO_X + coluna * TAMANHO;
            int y = TABULEIRO_Y + linha * TAMANHO;

            Color cor;

            if ((linha + coluna) % 2 == 0)
            {
                cor = CLARO;
            }
            else
            {
                cor = ESCURO;
            }
            DrawRectangle(x, y, TAMANHO, TAMANHO, cor);

            //Reproduzir a matriz no tabuleiro;

            if ((linha + coluna) % 2 == 1){
                if(ultimaPosicao.x == coluna && ultimaPosicao.y == linha ){
                    DrawText("X", x + TAMANHO/2, y + TAMANHO/2, 12, BLUE);
                }
                if(tabuleiro[linha][coluna] !=0){
                    Color corPeca = tabuleiro[linha][coluna] == 1 ? WHITE : BLACK;
                    Color corContorno = tabuleiro[linha][coluna] == 1 ? DARKGRAY : WHITE;
                    int centroX = x + TAMANHO / 2;
                    int centroY = y + TAMANHO / 2;
                    DrawCircle(centroX, centroY, 24, corPeca);
                    DrawCircleLines(centroX, centroY, 24, corContorno);
                }
                if(movimentos[linha][coluna] !=0){
                    DrawText("X", x + TAMANHO/2, y + TAMANHO/2, 12, YELLOW);
                }
            }
        }

        int vencedor = VerificarVencedor();
        if (vencedor != 0)
        {
            const char *textoVitoria = vencedor == 1 ? "Vitoria Brancas" : "Vitoria Pretas";
            int tamanhoTexto = 32;
            int larguraTexto = MeasureText(textoVitoria, tamanhoTexto);
            int alturaFundo = 48;
            DrawRectangle((LARGURA_TELA - larguraTexto - 24) / 2,
                          (ALTURA_TELA - alturaFundo) / 2,
                          larguraTexto + 24, alturaFundo, BLACK);
            DrawText(textoVitoria, (LARGURA_TELA - larguraTexto) / 2,
                     (ALTURA_TELA - tamanhoTexto) / 2, tamanhoTexto, WHITE);
        }
    }
}

void LimparMovimentos(){
    for (int i = 0; i < LINHA; i++)
    {
        for (int j = 0; j < COLUNA; j++)
        {
            movimentos[i][j] = 0;
        }
    }
}

void ValidarPosicoesValidas(Vector2 posicao, int peca)
{
    int linha = (int)posicao.y;
    int coluna = (int)posicao.x;

    int direcoes[4][2] = {
        {-1, -1},
        {-1,  1},
        { 1, -1},
        { 1,  1}
    };

    if (peca == 0) return;

    for (int i = 0; i < 4; i++)
    {
        int deltaLinha = direcoes[i][0];
        int deltaColuna = direcoes[i][1];

        int proximaLinha = linha + deltaLinha;
        int proximaColuna = coluna + deltaColuna;

        int destinoLinha = linha + deltaLinha * 2;
        int destinoColuna = coluna + deltaColuna * 2;

        // Verifica limites da próxima posição
        if (proximaLinha < 0 || proximaLinha >= LINHA ||
            proximaColuna < 0 || proximaColuna >= COLUNA)
            continue;

        // Movimento simples
        if (tabuleiro[proximaLinha][proximaColuna] == 0)
        {
            if ((peca == 1 && deltaLinha == -1) ||
                (peca == 2 && deltaLinha == 1))
            {
                movimentos[proximaLinha][proximaColuna] = 1;
            }
        }

        // Verifica limites do destino
        if (destinoLinha < 0 || destinoLinha >= LINHA ||
            destinoColuna < 0 || destinoColuna >= COLUNA)
            continue;

        // Captura
        if (tabuleiro[proximaLinha][proximaColuna] != 0 &&
            tabuleiro[proximaLinha][proximaColuna] != peca &&
            tabuleiro[destinoLinha][destinoColuna] == 0)
        {
            if(tabuleiro[destinoLinha+deltaLinha][destinoColuna+deltaColuna] != peca && tabuleiro[destinoLinha+deltaLinha][destinoColuna+deltaColuna] != 0 && tabuleiro[destinoLinha+deltaLinha*2][destinoColuna+deltaColuna*2] == 0){
                Vector2 posicaoNova;
                posicaoNova.x = destinoColuna;
                posicaoNova.y = destinoLinha;
                ValidarPosicoesValidas(posicaoNova,peca);
            }else{
                movimentos[destinoLinha][destinoColuna] = 1;
            }
        }
    }
}


void CapturarMouse(){
    Vector2 mouse = GetMousePosition();
    if (mouse.x >= TABULEIRO_X && mouse.x < TABULEIRO_X + LINHA * TAMANHO &&
        mouse.y >= TABULEIRO_Y && mouse.y < TABULEIRO_Y + COLUNA * TAMANHO)
    {
        int coluna = (mouse.x - TABULEIRO_X) / TAMANHO;
        int linha = (mouse.y - TABULEIRO_Y) / TAMANHO;
        printf("Linha: %d | Coluna: %d\n", linha, coluna);
        Vector2 posicao;
        posicao.x = coluna;
        posicao.y = linha;
        if (movimentos[linha][coluna] == 1 && quantidadeMovimentos > 0)
        {
            Vector2 origem = historico[quantidadeMovimentos - 1];
            int linhaOrigem = (int)origem.y;
            int colunaOrigem = (int)origem.x;
            int peca = tabuleiro[linhaOrigem][colunaOrigem];

            if (peca != turno) return;

            tabuleiro[linha][coluna] = peca;
            int somalinha,somacoluna;
            if (linha > linhaOrigem)
            {
                somalinha = 1;
            }
            else{
                somalinha = -1;
            }
            if(coluna > colunaOrigem){
                somacoluna = 1;
            }else{
                somacoluna = -1;
            }
            for (int i = 0; i < COLUNA; i++)
            {
                if (colunaOrigem+somacoluna*i == coluna && linhaOrigem+somalinha* i == linha)
                {
                    break;
                }
                tabuleiro[linhaOrigem+somalinha* i][colunaOrigem+somacoluna*i] = 0;
            }
            
            
            tabuleiro[linhaOrigem][colunaOrigem] = 0;
            AdicionarHistorico(posicao);
            LimparMovimentos();
            turno = turno == 1 ? 2 : 1;
        }else{
            int pecaAtual = tabuleiro[linha][coluna];
            if (pecaAtual == turno)
            {
                AdicionarHistorico(posicao);
                LimparMovimentos();
                ValidarPosicoesValidas(posicao, pecaAtual);
            }
        }                                                                          
    }
} 

int main()
{
    InitWindow(LARGURA_TELA, ALTURA_TELA, "Damas");
    SetTargetFPS(60);
    RedefinirTabuleiro();
    while (!WindowShouldClose())
    {
        BeginDrawing();
        DrawTable();
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && VerificarVencedor() == 0){
            CapturarMouse();
        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}