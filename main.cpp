#include <SDL.h>

SDL_Event evento;
SDL_Surface *tela = NULL;
bool executando = true;

const int screen_width = 300;
const int screen_height = 300;
const int screen_bpp = 32;

// para o framerate
Uint32 start = 0;
const int fps = 16;
const int framerate =  1000/fps;

// use essa fun��o pra carregar uma imagem.bmp e deixar o fundo transparente
SDL_Surface *fundo_transparente(const char *filename, Uint8 red, Uint8 green, Uint8 blue)
{
    SDL_Surface *load = NULL;
    SDL_Surface *otimizado = NULL;

    load = SDL_LoadBMP(filename);

    if(load != NULL)
    {
        otimizado = SDL_DisplayFormat(load);
        SDL_FreeSurface(load);

        if(otimizado != NULL)
        {
            SDL_SetColorKey(otimizado, SDL_SRCCOLORKEY, SDL_MapRGB(otimizado->format, red, green, blue));
        }
    }

    return otimizado;
}




// use essa fun��o pra desenhar uma imagem na tela
void DrawImage(int x, int y, SDL_Surface *image)
{
    SDL_Rect mover;
    mover.x = x;
    mover.y = y;

    SDL_BlitSurface(image, NULL, tela, &mover);
}

// use essa fun��o pra desenhar uma imagem cortada na tela
void DrawImageFrame(int x, int y, SDL_Surface *source, SDL_Surface *destination, int width, int height, int frame)
{
    SDL_Rect mover;
    mover.x = x;
    mover.y = y;

    int coluna = source->w / width;

    SDL_Rect corte;
    corte.x = (frame % coluna)*width;
    corte.y = (frame / coluna)*height;
    corte.w = width;
    corte.h = height;

    SDL_BlitSurface(source, &corte, destination, &mover);
}

// use essa fun��o pra desenhar texto na tela
void DrawText(int x, int y, SDL_Surface *source, SDL_Surface *destination, char texto[], int charSize, int start)
{
	for(unsigned int i = 0; i < strlen(texto); i++)
    {
        DrawImageFrame(x+i*charSize, y, source, destination, charSize, charSize, texto[i]-start);
    }
}


const int ALL_DOTS = 900;

// o tamanho da cobrinha
const int rectW = 10;
const int rectH = 10;

// essa classe representa a cobrinha
class _Snake
{
public:
    int x[ALL_DOTS];
    int y[ALL_DOTS];
    int vx;
    int vy;

    int tamanho;

};

_Snake cobrinha;

// inicia a posi��o, velocidade, tamanho da cobrinha
void ResetGame()
{
    cobrinha.tamanho = 3;
    // inicia as posi��es da cobrinha
    for(int i = 0; i < cobrinha.tamanho; i++)
    {
        cobrinha.x[i] = 50 - i*rectW;
        cobrinha.y[i] = 50;

        cobrinha.vx = rectW;
        cobrinha.vy = 0;
    }
}


// use essa fun��o pra f�sica e controle do jogo
void UpdateGame()
{

    // faz o corpo ir atr�s da cabe�a da cobrinha
    for(int z = cobrinha.tamanho; z > 0; z--)
    {
        cobrinha.x[z] = cobrinha.x[(z-1)];
        cobrinha.y[z] = cobrinha.y[(z-1)];

        // se a cabe�a da cobrinha se tocar
        if((z > 4) && (cobrinha.x[0] == cobrinha.x[z]) && (cobrinha.y[0] == cobrinha.y[z]))
        {
            ResetGame();
        }
    }

    // move a cabe�a da cobrinha
    cobrinha.x[0] = cobrinha.x[0] + cobrinha.vx;
    cobrinha.y[0] = cobrinha.y[0] + cobrinha.vy;

    Uint8 *tecla = SDL_GetKeyState(NULL);
    if(tecla[SDLK_LEFT] && cobrinha.vx <= 0)
    {
        cobrinha.vx = -rectW;
        cobrinha.vy = 0;
    }

    else if(tecla[SDLK_RIGHT] && cobrinha.vx >= 0)
    {
        cobrinha.vx = rectW;
        cobrinha.vy = 0;
    }

    else if(tecla[SDLK_UP] && cobrinha.vy <= 0)
    {
        cobrinha.vx = 0;
        cobrinha.vy = -rectH;
    }

    if(tecla[SDLK_DOWN] && cobrinha.vy >= 0)
    {
        cobrinha.vx = 0;
        cobrinha.vy = rectH;
    }

    // coli��o do snake com a comida?

    // coli��es nas bordas da tela
    // lado esquerdo, direto, em cima, em baixo da tela
    if(cobrinha.x[0] < 0 ||
       cobrinha.x[0] > screen_width - rectW ||
       cobrinha.y[0] < 0 ||
       cobrinha.y[0] > screen_height - rectH)
    {
        ResetGame();
    }
}


SDL_Surface *tutorialImage = NULL;
SDL_Surface *pauseImage = NULL;
SDL_Surface *whitefontImage = NULL;

SDL_Surface *cobrinhaImage = NULL;

// use essa fun��o pra carregar arquivos
// nota: essa fun��o s� deve ser chamada no come�o do programa
void LoadFiles()
{
    tutorialImage = SDL_LoadBMP("tutorial.bmp");
    pauseImage = fundo_transparente("pause.bmp", 0,255,255);
    whitefontImage = fundo_transparente("fontes/whitefont.bmp", 255,0,255);

    cobrinhaImage = SDL_LoadBMP("cobrinha.bmp");
}


// use essa fun��o pra fechar arquivos
// nota: essa fun��o s� deve ser chamada no final do programa
void CloseFiles()
{
    SDL_FreeSurface(tutorialImage);
    SDL_FreeSurface(pauseImage);
    SDL_FreeSurface(whitefontImage);
    SDL_FreeSurface(cobrinhaImage);
}

// desenha a cobrinha na tela
void DrawSnake()
{
    for(int i = 0; i < cobrinha.tamanho; i++)
    {
        DrawImage(cobrinha.x[i], cobrinha.y[i], cobrinhaImage);
    }
}

int main(int argc, char*args[])
{
SDL_Init(SDL_INIT_EVERYTHING);
tela = SDL_SetVideoMode(screen_width,screen_height,screen_bpp,SDL_SWSURFACE);

LoadFiles();

ResetGame();

// game loop
while(executando)
{
    start = SDL_GetTicks();
    while(SDL_PollEvent(&evento))
    {
        // clicou no x da janela
        if(evento.type == SDL_QUIT)
        {
            executando = false; // fecha o programa
        }
    }


    SDL_FillRect(tela, 0,0);

    UpdateGame();
    DrawSnake();


    SDL_Flip(tela);
    if(framerate > (SDL_GetTicks()-start))
    {
        SDL_Delay(framerate - (SDL_GetTicks()-start));
    }
}


CloseFiles();
SDL_Quit();
return 0;
}
