#include <SDL.h>

SDL_Event evento;
SDL_Surface *tela = NULL;
bool executando = true;

const int screen_width = 300;
const int screen_height = 400;
const int screen_bpp = 32;

int main(int argc, char*args[])
{
SDL_Init(SDL_INIT_EVERYTHING);
tela = SDL_SetVideoMode(screen_width,screen_height,screen_bpp,SDL_SWSURFACE);


// game loop
while(executando)
{
    while(SDL_PollEvent(&evento))
    {
        // clicou no x
        if(evento.type == SDL_QUIT)
        {
            executando = false;
        }
    }

    SDL_FillRect(tela, 0, 0xffffff);
    SDL_Flip(tela);
}

SDL_Quit();
return 0;
}
