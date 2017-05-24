#include "SDL.h"
#include "SDL_image.h"

//void clean_up_sdl();

int sdl_test()
{
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) < 0)
    {
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return 1;
    }
    SDL_Window *main_window=NULL;
    main_window=SDL_CreateWindow("Labyrinth", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
    //main_window=SDL_CreateWindow("Labyrinth", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_FULLSCREEN_DESKTOP);
    //main_window=SDL_CreateWindow("Labyrinth", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_RESIZABLE);
    if (main_window == NULL)
    {
		SDL_Log("Could not create window: %s\n", SDL_GetError());
		return 1;
	}
	/*SDL_Renderer *renderer = NULL;
	SDL_Texture *bitmapTex = NULL;
	renderer = SDL_CreateRenderer(main_window, -1, SDL_RENDERER_ACCELERATED);*/

	SDL_Surface *screen=NULL;
	screen=SDL_GetWindowSurface(main_window);
	if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
	{
		printf("IMG_Init: Failed to init required png support!\n");
		printf("IMG_Init: %s\n", IMG_GetError());
		return 1;
	}
	SDL_Surface *wall=NULL;
	wall=IMG_Load("gfx/wall.png");
	if(!wall)
	{
		printf("IMG_Load: %s\n", IMG_GetError());
		return 1;
	}
	/*bitmapTex = SDL_CreateTextureFromSurface(renderer, wall);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, bitmapTex, NULL, NULL);
	SDL_RenderPresent(renderer);*/
	SDL_BlitSurface(wall, NULL, screen, NULL);
	SDL_UpdateWindowSurface(main_window);
	//SDL_Delay(3000);
	SDL_Event event;
	int request_for_quit=0;
	while (!request_for_quit)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				request_for_quit=1;
			if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.scancode)
				{
					case SDL_SCANCODE_ESCAPE:
						request_for_quit=1;
						break;
				}
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						request_for_quit=1;
						break;
				}
			}
		}
	}
	SDL_FreeSurface(wall);
	IMG_Quit();
	//SDL_DestroyTexture(bitmapTex);
    //SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(main_window);
	SDL_Quit();
	return 0;
}

/*void clean_up_sdl()
{
	if (main_window != NULL)
		SDL_DestroyWindow(main_window);
	SDL_Quit();
	return;
}*/
