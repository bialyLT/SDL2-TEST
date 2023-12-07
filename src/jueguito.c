#include <stdio.h>
#include <include/SDL.h>
#include <include/SDL_image.h>
#include <include/SDL_audio.h>


// Definimos los valores para el audio
SDL_AudioSpec wavSpec;
Uint32 wavLength;
Uint8 *wavBuffer;
SDL_AudioDeviceID deviceId;


// Definir la estructura para el jugador
typedef struct 
{
    SDL_Texture *texture; //Textura para la imagen del jugador
    int x, y; // Posición del jugador
    int width, height; // Tamaño del jugador
} Player;

void loadWAV(const char *file) {
    if (SDL_LoadWAV(file, &wavSpec, &wavBuffer, &wavLength) == NULL) {
        perror("Error al cargar el WAV");
    }
    wavSpec.callback = NULL;
    deviceId = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);
}

void playSound() {
    int success = SDL_QueueAudio(deviceId, wavBuffer, wavLength);
    SDL_PauseAudioDevice(deviceId, 0);
}

void moverPersonaje(Player *player, int speed) 
{
    const Uint8  *state = SDL_GetKeyboardState(NULL);

    if (state[SDL_SCANCODE_UP]) 
    {
        player->y -= speed;
    }
    if (state[SDL_SCANCODE_DOWN]) 
    {
        player->y += speed;
    }
    if (state[SDL_SCANCODE_LEFT]) 
    {
        player->x -= speed;
    }
    if (state[SDL_SCANCODE_RIGHT]) 
    {
        player->x += speed;
    }
    
}

void moverDiagonal(Player *player, int speed)
{
    const Uint8  *state = SDL_GetKeyboardState(NULL);

    //Mover hacia:

    // Arriba Izquierda
    if (state[SDL_SCANCODE_UP] && state[SDL_SCANCODE_LEFT])
    {
        player->x -= speed/2;
        player->y -= speed/2;

    // Arriba Derecha
    }else if (state[SDL_SCANCODE_UP] && state[SDL_SCANCODE_RIGHT])
    {
        player->x += speed/2;
        player->y -= speed/2;
    // Abajo Izquierda
    }else if (state[SDL_SCANCODE_DOWN] && state[SDL_SCANCODE_LEFT])
    {
        player->x -= speed/2;
        player->y += speed/2;
        
    // Abajo Derecha
    }else if (state[SDL_SCANCODE_DOWN] && state[SDL_SCANCODE_RIGHT])
    {
        player->x += speed/2;
        player->y += speed/2;
    }
}

int main(int argc, char *argv[])
{

    int speed = 10;
    // Inicializar SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) 
    {
        printf("SDL no pudo inicializarse. Error: %s\n", SDL_GetError());
        return -1;
    }

    // Inicializar Audio
    if (SDL_Init(SDL_INIT_AUDIO) < 0) 
    {
        printf("SDL no pudo inicializarse. Error: %s\n", SDL_GetError());
        return -1;
    }
    loadWAV("./assets/keyico-audio.wav");

    // Inicializar SDL_image
    if (IMG_Init(IMG_INIT_JPG) < 0) 
    {
        printf("SDL_image no pudo inicializarse. Error: %s\n", IMG_GetError());
        return -1;
    }
    // Crear una ventana
    SDL_Window* window = SDL_CreateWindow("Mi Juego SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1080, 720, SDL_WINDOW_SHOWN);
    if (window == NULL) 
    {
        printf("La ventana no pudo crearse. Error: %s\n", SDL_GetError());
        return -1;
    }
    // Crear un renderizador
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);


    // Cargar imagen del jugador
    SDL_Surface* playerSurface = IMG_Load("./assets/keyico.jpg");
    if (playerSurface == NULL) 
    {
        printf("No se pudo cargar la imagen. Error: %s\n", IMG_GetError());
        return -1;
    }

    // Inicializar el jugador
    Player player;
    player.texture = SDL_CreateTextureFromSurface(renderer, playerSurface);
    SDL_FreeSurface(playerSurface);

    player.x = 320;
    player.y = 240;
    player.width = 200;
    player.height = 200;

    // Bucle principal
    int quit = 0;
    while (!quit) 
    {
        SDL_Event event;
        while (SDL_PollEvent(&event) != 0) 
        {
            if (event.type == SDL_QUIT) 
            {
                quit = 1;
            }
            moverPersonaje(&player, speed);
            if (event.key.keysym.sym == SDLK_SPACE) {
                    playSound();
                }
        }
        moverDiagonal(&player, speed);


        // Limpiar la pantalla
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Dibujar al jugador
        SDL_Rect playerRect = {player.x, player.y, player.width, player.height};
        SDL_RenderCopy(renderer, player.texture, NULL, &playerRect);

        // Actualizar la pantalla
        SDL_RenderPresent(renderer);

        // Pequeña pausa para evitar consumo excesivo de CPU
        SDL_Delay(10);
    }

    // Limpiar y cerrar SDL_image
    SDL_DestroyTexture(player.texture);
    IMG_Quit();

    // Limpiar y cerrar SDL y la superficie del jugador
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    // Limpiar el audio despues de utilizar
    SDL_CloseAudioDevice(deviceId);
    SDL_FreeWAV(wavBuffer);

    return 0;
}