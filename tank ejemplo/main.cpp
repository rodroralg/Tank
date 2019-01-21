#include <iostream>
#include <string>
#include <fstream>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "Mapa.h"
#include "Tank.h"
#include "Tela.h"
#include "SDL/SDL_rotozoom.h"

using namespace std;

int main(int argc, char* args[])
{
    //inicia o SDL
    SDL_Init(SDL_INIT_EVERYTHING);

    //se nao inicializar, fecha o programa
    if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        return 1;
    }

    //nome do programa
    SDL_WM_SetCaption("Tank", NULL);

    //variaveis
    bool running = true;
    SDL_Event evento;

    //dimensoes
    struct dimensoes
    {
        float altura, largura;
    }janelaDim;

    //janela
    SDL_Surface *janela = NULL;
    janelaDim.largura = 1300;
    janelaDim.altura = 660;
    janela = SDL_SetVideoMode(janelaDim.largura, janelaDim.altura, 32, SDL_HWSURFACE);

    //Objetos
    Mapa fundo;
    Tank player1(1,janela, janelaDim.altura, janelaDim.largura);
    Tank player2(2,janela, janelaDim.altura, janelaDim.largura);

    //inicializa o mapa de colisoes
    fundo.Colisoes(janela,janelaDim.altura,janelaDim.largura, "tank");
    float altura = fundo.getAltura();
    float largura = fundo.getLargura();

    //Frames
    const int FRAMES_PER_SECOND = 150;
    int frame = 0;
    float tempoInicial = 0;
    float tempoFinal = 0;
    float DeltaT = 0;

    //TESTE
    SDL_Rect teste;
    teste.x = 0;
    teste.y = 0;
    teste.h = 660;
    teste.w = 1330;

    while (running)
    {
        tempoInicial = SDL_GetTicks();

        //sai do jogo se ele for fechado
        SDL_PollEvent(&evento);
        if(evento.type == SDL_QUIT)
        {
            running = false;
            break;
        }

        Uint8 *tecla = SDL_GetKeyState(NULL);

        //sai do jogo se a tecla ESC for apertada
        if(tecla[SDLK_ESCAPE])
        {
            running = false;
            break;
        }



        //move o jogador 1
        player1.move(janelaDim.altura, janelaDim.largura,
                     tecla,altura, largura,
                     fundo.verificaColisao(player1.getX(), player1.getY(),
                                           player1.getAltura(),player1.getLargura(),""),
                     player2.getX(), player2.getY(),player2.getAltura(),player2.getLargura());

        //verifica se o jogador 1 ainda esta colidindo com o cenario e atualiza a sua posicao
        // eh necessario corrigir a posicao do jogador 1 antes o jogador 2 se mover, pois
        //se o jogador 2 se move-se com o jogador 1 ainda colidindo, o jogador 2 de deslocaria
        //de tal forma que haveria colisao entre os jogadores, ou seja, ao invez de sua posicao
        //ser a posicao do jogador 1 + a altura/largura, seria somada a distancia em que o jogador
        // 1 esta colidindo, implicando em colisao no jogador 2

        if(fundo.verificaColisao(player1.getX(), player1.getY(),player1.getAltura(),
                                           player1.getLargura(),"") == 1)
        {
            player1.move(janelaDim.altura, janelaDim.largura,
                          tecla,altura, largura,
                          fundo.verificaColisao(player1.getX(), player1.getY(),
                                                player1.getAltura(),player1.getLargura(),""),
                         player2.getX(), player2.getY(),player2.getAltura(),player2.getLargura());
        }

        //move o jogador 2
        player2.move(janelaDim.altura, janelaDim.largura,
                     tecla,altura, largura,
                     fundo.verificaColisao(player2.getX(), player2.getY(),
                                           player2.getAltura(),player2.getLargura(),""),
                     player1.getX(), player1.getY(), player1.getAltura(), player1.getLargura());

        //verifica se o jogador 2 ainda colide com o cenario
        if(fundo.verificaColisao(player2.getX(), player2.getY(),player2.getAltura(),
                                           player2.getLargura(),"") == 1)
        {
            player2.move(janelaDim.altura, janelaDim.largura,
                          tecla,altura, largura,
                          fundo.verificaColisao(player2.getX(), player2.getY(),
                                                player2.getAltura(),player2.getLargura(),""),
                         player1.getX(), player1.getY(), player1.getAltura(), player1.getLargura());
        }

        //move a bala
        player1.moveBala();
        player2.moveBala();
        player1.colisaoBala(player2.getX(), player2.getY(), player2.getAltura(), player2.getLargura());
        player2.colisaoBala(player1.getX(), player1.getY(), player1.getAltura(), player1.getLargura());

        /*
        PS: nunca mais criar um objeto dentro de outro
        PS2: criar uma classe para atirar ao invez de colocar tudo na classe do jogador
        PS3: comentar mais
        PS4: criar uma classe so para o tijolo
        */

        //causa dano
        player1.leva_dano(player2.getCausa_dano());
        player2.leva_dano(player1.getCausa_dano());

        //posiciona as imagens

        fundo.printFundo(janela, janelaDim.altura, janelaDim.largura);
        player1.printTank(janela, player2.getX(),player2.getY(), player2.getAltura(), player2.getLargura());
        player2.printTank(janela, player1.getX(),player1.getY(), player1.getAltura(), player1.getLargura());
        player1.printbala();
        player2.printbala();
        player1.printLife(janelaDim.altura, janelaDim.largura);
        player2.printLife(janelaDim.altura, janelaDim.largura);

        //atualiza a janela
        SDL_Flip(janela);

        //frames
        frame++;
        tempoFinal = SDL_GetTicks();
        DeltaT = tempoFinal - tempoInicial;
        if(DeltaT < 1000/ FRAMES_PER_SECOND)
        {
            SDL_Delay((1000 / FRAMES_PER_SECOND) - DeltaT);
        }


    }

    //fecha o programa
    SDL_Quit();

    return 0;

}
