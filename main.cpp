#include <SDL.h>
#include <SDL_mixer.h>

// os estados de tela do jogo
enum
{
    MENU,
    TUTORIAL,
    ABOUT,
    PAUSE,
    GAME,
};
int game_estado = MENU;

bool playsound = true;

SDL_Event evento;
SDL_Surface *tela = NULL;
bool executando = true;


int pontos = 0;
int record = 0;

const int screen_width = 300;
const int screen_height = 400;
const int screen_bpp = 32;

// para o framerate
Uint32 start = 0;
const int fps = 16;
const int framerate =  1000/fps;

// use essa função pra carregar uma imagem.bmp e deixar o fundo transparente
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


// use essa função pra desenhar uma imagem na tela
void DrawImage(int x, int y, SDL_Surface *image)
{
    SDL_Rect mover;
    mover.x = x;
    mover.y = y;

    SDL_BlitSurface(image, NULL, tela, &mover);
}

// use essa função pra desenhar uma imagem cortada na tela
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

// use essa função pra desenhar texto na tela
void DrawText(int x, int y, SDL_Surface *source, SDL_Surface *destination, char texto[], int charSize, int start)
{
	for(unsigned int i = 0; i < strlen(texto); i++)
    {
        DrawImageFrame(x+i*charSize, y, source, destination, charSize, charSize, texto[i]-start);
    }
}


const int ALL_DOTS = 900;

// o tamanho da cobrinha
// rect = rectangle = retângulo
const int rectW = 10; // width = largura
const int rectH = 10; // height = altura

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

// inicia a posição, velocidade, tamanho da cobrinha
void ResetGame()
{
    cobrinha.tamanho = 3;
    pontos = 0;
    // inicia as posições da cobrinha
    for(int i = 0; i < cobrinha.tamanho; i++)
    {
        cobrinha.x[i] = 50 - i*rectW;
        cobrinha.y[i] = 50;

        cobrinha.vx = rectW;
        cobrinha.vy = 0;
    }
}


// use essa função pra física e controle do jogo
void UpdateGame()
{
    // faz o corpo ir atrás da cabeça da cobrinha
    for(int z = cobrinha.tamanho; z > 0; z--)
    {
        cobrinha.x[z] = cobrinha.x[(z-1)];
        cobrinha.y[z] = cobrinha.y[(z-1)];

        // se a cabeça da cobrinha se tocar
        if((z > 4) && (cobrinha.x[0] == cobrinha.x[z]) && (cobrinha.y[0] == cobrinha.y[z]))
        {
            ResetGame();
        }
    }

    // move a cabeça da cobrinha
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

    else if(tecla[SDLK_DOWN] && cobrinha.vy >= 0)
    {
        cobrinha.vx = 0;
        cobrinha.vy = rectH;
    }

    // ativa/desativa o som
    else if(tecla[SDLK_s])
    {
        playsound = !playsound;
    }

    // colisões nas bordas da tela
    // lado esquerdo, lado direto, em cima, em baixo da tela
    if(cobrinha.x[0] < 0 ||
       cobrinha.x[0] > screen_width - rectW ||
       cobrinha.y[0] < 0 ||
       cobrinha.y[0] > screen_height - rectH - 100)
    {
        ResetGame();
    }
}


// essa classe representa a comida ou maçã
class _Apple
{
    public:
    int x;
    int y;
};

_Apple apple;

// inicia a posição da comida em lugar aleatório
void PlaceApple()
{
    static int r = 0;

    r = rand() % 30;
    apple.x = r*rectW;

    r = rand() % 30;
    apple.y = r*rectH;

    for(int z = cobrinha.tamanho; z > 0; z--)
    {
        // Verifique se a comida se sobrepõe à nossa cobrinha
        if(apple.x == cobrinha.x[z] && apple.y == cobrinha.y[z])
        {
            PlaceApple();
            break;
        }
    }
}


SDL_Surface *blackfontImage = NULL;
SDL_Surface *bluefontImage = NULL;
SDL_Surface *cyanfontImage = NULL;
SDL_Surface *greenfontImage = NULL;
SDL_Surface *greyfontImage = NULL;
SDL_Surface *purplefontImage = NULL;
SDL_Surface *redfontImage = NULL;
SDL_Surface *whitefontImage = NULL;
SDL_Surface *yellowfontImage = NULL;



SDL_Surface *fundoImage = NULL;
SDL_Surface *tutorialImage = NULL;
SDL_Surface *cobrinhaImage = NULL;
SDL_Surface *appleImage = NULL;

SDL_Surface *cursorImage = NULL;

SDL_Surface *iconImage = NULL;

Mix_Chunk * AppleSound = NULL;

// use essa função pra carregar arquivos
// nota: essa função só deve ser chamada no começo do programa
void LoadFiles()
{
    blackfontImage = fundo_transparente("fontes/blackfont.bmp", 255,0,255);
    bluefontImage = fundo_transparente("fontes/bluefont.bmp", 255,0,255);
    cyanfontImage = fundo_transparente("fontes/cyanfont.bmp", 255,0,255);
    greenfontImage = fundo_transparente("fontes/greenfont.bmp", 255,0,255);
    greyfontImage = fundo_transparente("fontes/greyfont.bmp", 255,0,255);
    purplefontImage = fundo_transparente("fontes/purplefont.bmp", 255,0,0);
    redfontImage = fundo_transparente("fontes/redfont.bmp", 255,0,255);
    whitefontImage = fundo_transparente("fontes/whitefont.bmp", 255,0,255);
    yellowfontImage = fundo_transparente("fontes/yellowfont.bmp", 255,0,255);

    fundoImage = SDL_LoadBMP("gfx/fundo.bmp");
    tutorialImage = SDL_LoadBMP("gfx/tutorial.bmp");
    cobrinhaImage = SDL_LoadBMP("gfx/cobrinha.bmp");
    appleImage = SDL_LoadBMP("gfx/apple.bmp");
    cursorImage = fundo_transparente("gfx/cursor.bmp", 0,255,255);

    AppleSound = Mix_LoadWAV("sounds/Apple_Crunch_16.wav");

}


// use essa função pra fechar arquivos
// nota: essa função só deve ser chamada no final do programa
void CloseFiles()
{
    SDL_FreeSurface(blackfontImage);
    SDL_FreeSurface(bluefontImage);
    SDL_FreeSurface(cyanfontImage);
    SDL_FreeSurface(greenfontImage);
    SDL_FreeSurface(greyfontImage);
    SDL_FreeSurface(purplefontImage );
    SDL_FreeSurface(redfontImage);
    SDL_FreeSurface(whitefontImage);
    SDL_FreeSurface(yellowfontImage);



    SDL_FreeSurface(fundoImage);
    SDL_FreeSurface(tutorialImage);
    SDL_FreeSurface(cobrinhaImage);
    SDL_FreeSurface(appleImage);
    SDL_FreeSurface(cursorImage);

    SDL_FreeSurface(iconImage);
    Mix_FreeChunk(AppleSound);
}

// desenha o background na tela
void DrawBackground()
{
    DrawImage(0,0,fundoImage);
}

// monstra os pontos, tamanho, record
void DrawHUD()
{
    char str1[10];
    sprintf(str1, "%d", pontos);
    DrawText(0,301,blackfontImage,tela, "Seus Pontos: ",16,32);
    DrawText(200,301,greenfontImage,tela, str1,16,32);

    char str2[10];
    sprintf(str2, "%d", record);
    DrawText(0,301+22,blackfontImage,tela, "Seu Record: ",16,32);
    DrawText(200,301+22,greenfontImage,tela, str2,16,32);


    char str3[10];
    sprintf(str3, "%d", cobrinha.tamanho);
    DrawText(0,301+2*22,blackfontImage,tela, "Seu Tamanho: ",16,32);
    DrawText(200,301+2*22,greenfontImage,tela, str3,16,32);

    if(playsound == true)
    {
        DrawText(0,301+3*22,blackfontImage,tela, "Som ativado",16,32);
    }

    else if(playsound == false)
    {
        DrawText(0,301+3*22,blackfontImage,tela, "Som desativado",16,32);
    }

    if(pontos > record)
    {
        record = pontos;
    }
}

// desenha a cobrinha na tela
void DrawSnake()
{
    for(int i = 0; i < cobrinha.tamanho; i++)
    {
        DrawImage(cobrinha.x[i], cobrinha.y[i], cobrinhaImage);
    }
}

// desenha a comida ou maça na tela
void DrawApple()
{
    DrawImage(apple.x,apple.y,appleImage);
}


// use essa função pra ver se a cobrinha comeu a comida
void CollideWithSnake()
{
    // se cabeça da cobrinha colidir com a comida
    if((cobrinha.x[0] == apple.x) && (cobrinha.y[0] == apple.y))
    {
        cobrinha.tamanho = cobrinha.tamanho+1;
        pontos = pontos+1;
        PlaceApple();
        if(playsound == true)
        {
            Mix_PlayChannel(-1, AppleSound, 0);
        }
    }
}

// essa classe representa o cursor
class _Cursor
{
public:
    int x;
    int y;
};
_Cursor cursor;

// use essa função pra desenhar e controlar o menu na tela
void DrawMenuAndUpdateMenu()
{
    char *options[] = {"Jogar", "Tutorial", "Sobre", "Sair"};
    static int NumberOfNames = sizeof options / sizeof *options;

    static int index = 0;
    static int dist = 30;
    static int x = 0;
    static int y = 0;
    static int NumberOfLetters = 0;
    cursor.x = 0;
    cursor.y = 0;

    // programação do teclado
    Uint8 *tecla = SDL_GetKeyState(NULL);

    if(tecla[SDLK_DOWN])
    {
        index = index+1;
    }

    if(tecla[SDLK_UP])
    {
        index = index-1;
    }

    if(index > NumberOfNames - 1)
    {
        index = 0;
    }

    if(index < 0)
    {
        index = NumberOfNames - 1;
    }

    if(index == 0 && tecla[SDLK_RETURN])
    {
        game_estado = GAME;
    }

    if(index == 1 && tecla[SDLK_RETURN])
    {
        game_estado = TUTORIAL;
    }

    if(index == 2 && tecla[SDLK_RETURN])
    {
        game_estado = ABOUT;
    }

    if(index == 3 && tecla[SDLK_RETURN])
    {
        // fecha o programa
        executando = false;
    }

    for(int i = 0; i < NumberOfNames; i++)
    {
        NumberOfLetters = strlen(options[i]);

        x = (screen_width - 16*NumberOfLetters)/2;
        y = 120;

        // valor selecionado
		if(index == i)
        {
            cursor.x = 45;
            cursor.y = y + (dist*i)-2;
            DrawImage(cursor.x,cursor.y,cursorImage);
            DrawText(x, y+(dist*i), bluefontImage, tela, options[i], 16, 32);
        }

        // valor não selecionado
        else
        {
            DrawText(x, y+(dist*i), greyfontImage,tela, options[i], 16, 32);
        }
    }
}

// desenha o tutorial na tela
void DrawTutorial()
{
    DrawImage(0,0,tutorialImage);
}

// os comandos do tutorial
void updateTutorial()
{
    Uint8 *tecla = SDL_GetKeyState(NULL);

    if(tecla[SDLK_ESCAPE])
    {
        game_estado = MENU;
    }
}

// desenha o about na tela
void DrawAbout()
{
    DrawText(0,0,whitefontImage,tela, "Jogo da cobrinha", 16, 32);
    DrawText(0,32,whitefontImage,tela, "Feito por", 16, 32);
    DrawText(0,64,whitefontImage,tela, "Jonatas Ricosti", 16, 32);
    DrawText(0,96,whitefontImage,tela, "Audio: Koops", 16, 32);
}

// os comandos de about
void updateAbout()
{
    Uint8 *tecla = SDL_GetKeyState(NULL);

    if(tecla[SDLK_ESCAPE])
    {
        game_estado = MENU;
    }
}

// desenha o jogo na tela
void DrawGame()
{
    DrawBackground();
    DrawHUD();
    DrawSnake();
    DrawApple();
}


// desenha o pause na tela
void DrawPause()
{
    DrawGame();
    DrawText(110,110,whitefontImage,tela, "Pause", 16,32);
}

// use essa função pra trocar as telas do jogo
// nota: essa função deve ser respaldada
// pois ela é responsável por dar vida ao jogo
void RunGame()
{
    switch(game_estado)
    {
        case MENU: DrawMenuAndUpdateMenu(); break;
        case TUTORIAL: DrawTutorial(); updateTutorial(); break;
        case ABOUT: DrawAbout(); updateAbout(); break;
        case PAUSE: DrawPause(); break;
        case GAME: DrawGame(); UpdateGame(); CollideWithSnake(); break;
    }
}

int main(int argc, char*args[])
{
SDL_Init(SDL_INIT_EVERYTHING);

iconImage = SDL_LoadBMP("gfx/icon.bmp");
SDL_WM_SetIcon(iconImage, NULL);
SDL_putenv("SDL_VIDEO_WINDOW_POS=center"); // centraliza a janela
tela = SDL_SetVideoMode(screen_width,screen_height,screen_bpp,SDL_SWSURFACE);

SDL_WM_SetCaption("Jogo da cobrinha", NULL);

Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);

LoadFiles();
ResetGame();
PlaceApple();


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

        if(evento.type == SDL_KEYDOWN)
        {
            if(evento.key.keysym.sym == SDLK_ESCAPE)
            {
                if(game_estado == GAME)
                {
                    game_estado = PAUSE;
                }

                else if(game_estado == PAUSE)
                {
                    game_estado = GAME;
                }
            }
        }
    }


    SDL_FillRect(tela, 0,0);

    RunGame();

    SDL_Flip(tela);
    if(framerate > (SDL_GetTicks()-start))
    {
        SDL_Delay(framerate - (SDL_GetTicks()-start));
    }
}


CloseFiles();
Mix_CloseAudio();
SDL_Quit();
return 0;
}
