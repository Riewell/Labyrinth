/*  sdl_main_hwsw.c

  Лабиринт
  Version 0.2.2

  Copyright 2017 Konstantin Zyryanov <post.herzog@gmail.com>
  
  This file is part of Labyrinth.
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation; either version 2.1 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301, USA.
*/

#include "SDL.h"
#include "SDL_image.h"
#include "includes_macros.h"

//TODO: перенести в Настройки!
#define FPS 60

void show_labyrinth_in_cmd_hw(int const player_coordinate, const struct players player[], int const length, int const width, int const *labyrinth_temp); //для отладки и контроля через консоль
int previous_state_hw=0; //для отладки и контроля через консоль

void clean_up_sdl_hw(SDL_Window *main_window, SDL_Renderer *renderer);

int sdl_main_hwsw(int *labyrinth, struct players player[], int const size_labyrinth_length, int const size_labyrinth_width, int requested_mode)
{
	//Запрос на инициализацию поддержки видео и аудио
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) < 0)
    {
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return 1;
    }
    //Создание основного окна
    SDL_Window *main_window=NULL;
    //Перенести настройки разрешения в Настройки
    //TODO: изменить первоначальное разрешение на текущее экранное (fullscreen?)
    int window_width=640;
    int window_height=480;
    main_window=SDL_CreateWindow("Labyrinth", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width, window_height, 0);
    //main_window=SDL_CreateWindow("Labyrinth", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_FULLSCREEN_DESKTOP);
    //main_window=SDL_CreateWindow("Labyrinth", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_RESIZABLE);
    if (main_window == NULL)
    {
		SDL_Log("Could not create window: %s\n", SDL_GetError());
		clean_up_sdl_hw(NULL, NULL);
		return 1;
	}
	SDL_Renderer *renderer = NULL;
	if (requested_mode == 1)
		renderer = SDL_CreateRenderer(main_window, -1, SDL_RENDERER_SOFTWARE);
	else if (requested_mode == 2)
		renderer = SDL_CreateRenderer(main_window, -1, SDL_RENDERER_SOFTWARE || SDL_RENDERER_PRESENTVSYNC);
	else if (requested_mode == 3)
		renderer = SDL_CreateRenderer(main_window, -1, SDL_RENDERER_ACCELERATED);
	else if (requested_mode == 4)
		renderer = SDL_CreateRenderer(main_window, -1, SDL_RENDERER_ACCELERATED || SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL)
	{
		SDL_Log("Could not create renderer: %s\n", SDL_GetError());
		clean_up_sdl_hw(main_window, NULL);
	}
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	//Инициализация поверхности окна
	//SDL_Surface *screen=NULL;
	//screen=SDL_GetWindowSurface(main_window);
	//Загрузка графических ресурсов
	if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
	{
		printf("IMG_Init: Failed to init required png support!\n");
		printf("IMG_Init: %s\n", IMG_GetError());
		clean_up_sdl_hw(main_window, renderer);
		return 1;
	}
	SDL_Surface *background_sf=NULL;
	SDL_Surface *wall_sf=NULL;
	SDL_Surface *wall_side_left_sf=NULL;
	SDL_Surface *wall_side_right_sf=NULL;
	SDL_Surface *wall_far_sf=NULL;
	SDL_Surface *wall_side_far_left_sf=NULL;
	SDL_Surface *wall_side_far_right_sf=NULL;
	SDL_Surface *line_sf=NULL;
	SDL_Surface *empty_background_sf=NULL;
	SDL_Surface *empty_far_background_sf=NULL;
	
	background_sf=IMG_Load("gfx/background.png");
	wall_sf=IMG_Load("gfx/wall.png");
	wall_side_left_sf=IMG_Load("gfx/wall_side_left.png");
	wall_side_right_sf=IMG_Load("gfx/wall_side_right.png");
	wall_far_sf=IMG_Load("gfx/wall_far.png");
	wall_side_far_left_sf=IMG_Load("gfx/wall_side_far_left.png");
	wall_side_far_right_sf=IMG_Load("gfx/wall_side_far_right.png");
	line_sf=IMG_Load("gfx/line.png");
	empty_background_sf=IMG_Load("gfx/empty_background.png");
	empty_far_background_sf=IMG_Load("gfx/empty_far_background.png");
	//Если не удалось загрузить какие-то файлы - вывести сообщение об ошибке с указанием последнего не загруженного файла
	if (!background_sf || !wall_sf || !wall_side_left_sf || !wall_side_right_sf || !wall_far_sf || !wall_side_far_left_sf || !wall_side_far_right_sf || !line_sf || !empty_background_sf || !empty_far_background_sf)
	{
		printf("IMG_Load: %s\n", IMG_GetError());
		clean_up_sdl_hw(main_window, renderer);
		return 1;
	}
	SDL_Texture *background=NULL;
	SDL_Texture *wall=NULL;
	SDL_Texture *wall_side_left=NULL;
	SDL_Texture *wall_side_right=NULL;
	SDL_Texture *wall_far=NULL;
	SDL_Texture *wall_side_far_left=NULL;
	SDL_Texture *wall_side_far_right=NULL;
	SDL_Texture *line=NULL;
	SDL_Texture *empty_background=NULL;
	SDL_Texture *empty_far_background=NULL;
	
	background=SDL_CreateTextureFromSurface(renderer, background_sf);
	wall=SDL_CreateTextureFromSurface(renderer, wall_sf);
	wall_side_left=SDL_CreateTextureFromSurface(renderer, wall_side_left_sf);
	wall_side_right=SDL_CreateTextureFromSurface(renderer, wall_side_right_sf);
	wall_far=SDL_CreateTextureFromSurface(renderer, wall_far_sf);
	wall_side_far_left=SDL_CreateTextureFromSurface(renderer, wall_side_far_left_sf);
	wall_side_far_right=SDL_CreateTextureFromSurface(renderer, wall_side_far_right_sf);
	line=SDL_CreateTextureFromSurface(renderer, line_sf);
	empty_background=SDL_CreateTextureFromSurface(renderer, empty_background_sf);
	empty_far_background=SDL_CreateTextureFromSurface(renderer, empty_far_background_sf);
	//Если не удалось загрузить какие-то текстуры - вывести сообщение об ошибке
	if (!background || !wall || !wall_side_left || !wall_side_right || !wall_far || !wall_side_far_left || !wall_side_far_right || !line || !empty_background || !empty_far_background)
	{
		printf("IMG_Load: %s\n", IMG_GetError());
		SDL_FreeSurface(background_sf);
		SDL_FreeSurface(wall_sf);
		SDL_FreeSurface(wall_side_left_sf);
		SDL_FreeSurface(wall_side_right_sf);
		SDL_FreeSurface(wall_far_sf);
		SDL_FreeSurface(wall_side_far_left_sf);
		SDL_FreeSurface(wall_side_far_right_sf);
		SDL_FreeSurface(line_sf);
		SDL_FreeSurface(empty_background_sf);
		SDL_FreeSurface(empty_far_background_sf);
		SDL_DestroyTexture(background);
		SDL_DestroyTexture(wall);
		SDL_DestroyTexture(wall_side_left);
		SDL_DestroyTexture(wall_side_right);
		SDL_DestroyTexture(wall_far);
		SDL_DestroyTexture(wall_side_far_left);
		SDL_DestroyTexture(wall_side_far_right);
		SDL_DestroyTexture(line);
		SDL_DestroyTexture(empty_background);
		SDL_DestroyTexture(empty_far_background);
		clean_up_sdl_hw(main_window, renderer);
		return 1;
	}
	SDL_FreeSurface(background_sf);
	SDL_FreeSurface(wall_sf);
	SDL_FreeSurface(wall_side_left_sf);
	SDL_FreeSurface(wall_side_right_sf);
	SDL_FreeSurface(wall_far_sf);
	SDL_FreeSurface(wall_side_far_left_sf);
	SDL_FreeSurface(wall_side_far_right_sf);
	SDL_FreeSurface(line_sf);
	SDL_FreeSurface(empty_background_sf);
	SDL_FreeSurface(empty_far_background_sf);
	
	SDL_Event event;
	int request_for_quit=0;
	int count=0;
	//Начало основного цикла - пока не произойдёт вызов закрытия программы
	while (!request_for_quit)
	{
		Uint32 ticks_begin=SDL_GetTicks(); //время начала очередного кадра
		//Вычисление координаты игрока
		int player_coordinate=player[0].x*size_labyrinth_length+player[0].y;
		//Очистка экрана перед отрисовкой следующего кадра
		SDL_RenderClear(renderer);
		//Подготовка кадра (расположение изображений в нужном порядке по соответствующим координатам)
		SDL_Rect srcrect_wall, dstrect_wall;
		//Отрисовка фона
		SDL_RenderCopy(renderer, background, NULL, NULL);
		//SDL_BlitSurface(background, NULL, screen, NULL);
		//FIXME: Проверка границ лабиринта в условиях! + добавить крайние стены
		//Если игрок направлен вверх или вниз по структуре лабиринта
		if (player[0].direction == UP || player[0].direction == DOWN)
		{
			int sign; //левая и правая стороны ниже - относительно игрока
			//Если игрок направлен вверх по структуре лабиринта
			if (player[0].direction == UP)
				sign=1;
			//Если игрок направлен вниз по структуре лабиринта
			else
				sign=-1;
			//Если впереди стена (в том числе крайняя) - отрисовка пустого фона и стены, иначе - отрисовка дальнего плана
			if ((player[0].x-sign*1) < 0 || (player_coordinate-sign*size_labyrinth_length) >= size_labyrinth_width*size_labyrinth_length || labyrinth[player_coordinate-sign*size_labyrinth_length] == WALL)
			{
				//SDL_BlitSurface(empty_far_background, NULL, screen, NULL);
				dstrect_wall.x=0;
				dstrect_wall.y=(window_height-391)/2; //wall.png height=391 empty_background height=391
				dstrect_wall.h=391;
				dstrect_wall.w=640;
				SDL_RenderCopy(renderer, empty_background, NULL, &dstrect_wall);
				//SDL_BlitSurface(empty_background, NULL, screen, &dstrect_wall);
				dstrect_wall.x=(window_width-538)/2; //wall.png widht=538
				dstrect_wall.h=391;
				dstrect_wall.w=538;
				SDL_RenderCopy(renderer, wall, NULL, &dstrect_wall);
				//SDL_BlitSurface(wall, NULL, screen, &dstrect_wall);
			}
			else
			{
				//Отрисовка пустоты на заднем фоне
				dstrect_wall.x=0;
				dstrect_wall.y=(window_height-209)/2; //empty_far_background.png height=209
				dstrect_wall.h=209;
				dstrect_wall.w=640;
				SDL_RenderCopy(renderer, empty_far_background, NULL, &dstrect_wall);
				//SDL_BlitSurface(empty_far_background, NULL, screen, &dstrect_wall);
				//Отрисовка дальней стены
				if (labyrinth[player_coordinate-sign*size_labyrinth_length*2] == WALL)
				{
					dstrect_wall.x=(window_width-324)/2; //wall_far width=324
					dstrect_wall.y=(window_height-211)/2; //wall_far height=211
					dstrect_wall.h=211;
					dstrect_wall.w=324;
					SDL_RenderCopy(renderer, wall_far, NULL, &dstrect_wall);
					//SDL_BlitSurface(wall_far, NULL, screen, &dstrect_wall);
				}
				//Если присутствует дальняя боковая левая стена - отрисовка её, иначе - отрисовка дальней левой стены (не боковой)
				if (labyrinth[player_coordinate-sign*size_labyrinth_length-1*sign] == WALL)
				{
					dstrect_wall.x=(window_width-324)/2-109; //wall_far width=324 wall_side_far_left width=109
					dstrect_wall.y=(window_height-391)/2; //wall_side_far_left height=391
					dstrect_wall.h=391;
					dstrect_wall.w=109;
					SDL_RenderCopy(renderer, wall_side_far_left, NULL, &dstrect_wall);
					//SDL_BlitSurface(wall_side_far_left, NULL, screen, &dstrect_wall);
				}
				else if (labyrinth[player_coordinate-sign*size_labyrinth_length*2-1*sign] == WALL)
				{
					srcrect_wall.x=324-(window_width-324)/2;
					srcrect_wall.y=0;
					srcrect_wall.h=211;
					srcrect_wall.w=(window_width-324)/2;
					dstrect_wall.x=0;
					dstrect_wall.y=(window_height-211)/2;
					dstrect_wall.h=211;
					dstrect_wall.w=(window_width-324)/2;
					SDL_RenderCopy(renderer, wall_far, &srcrect_wall, &dstrect_wall);
					//SDL_BlitSurface(wall_far, &srcrect_wall, screen, &dstrect_wall);
				}
				//Если присутствует дальняя боковая правая стена - отрисовка её, иначе - отрисовка дальней правой стены (не боковой)
				if (labyrinth[player_coordinate-sign*size_labyrinth_length+1*sign] == WALL)
				{
					//(window_width-324)/2+324=(window_width-324)/2+648/2=(window_width-324+648)/2=(window_width+324)/2
					dstrect_wall.x=(window_width+324)/2; //wall_far width=324
					dstrect_wall.y=(window_height-393)/2; //wall_side_far_left height=393
					dstrect_wall.h=393;
					dstrect_wall.w=109;
					SDL_RenderCopy(renderer, wall_side_far_right, NULL, &dstrect_wall);
					//SDL_BlitSurface(wall_side_far_right, NULL, screen, &dstrect_wall);
				}
				else if (labyrinth[player_coordinate-sign*size_labyrinth_length*2+1*sign] == WALL)
				{
					srcrect_wall.x=0;
					srcrect_wall.y=0;
					srcrect_wall.h=211;
					srcrect_wall.w=(window_width-324)/2;
					dstrect_wall.x=window_width-(window_width-324)/2;
					dstrect_wall.y=(window_height-211)/2;
					dstrect_wall.h=211;
					dstrect_wall.w=(window_width-324)/2;
					SDL_RenderCopy(renderer, wall_far, &srcrect_wall, &dstrect_wall);
					//SDL_BlitSurface(wall_far, &srcrect_wall, screen, &dstrect_wall);
				}
				//Отрисовка дальних линий-разделителей левой и правой
				srcrect_wall.x=0;
				srcrect_wall.y=0;
				srcrect_wall.h=211; //line.png height=391 wall_far.png height=211
				srcrect_wall.w=1; //line.png width-2
				dstrect_wall.x=(window_width-324)/2; //wall_far.png width=324
				dstrect_wall.y=(window_height-211)/2;
				dstrect_wall.h=211;
				dstrect_wall.w=1;
				SDL_RenderCopy(renderer, line, &srcrect_wall, &dstrect_wall);
				//SDL_BlitSurface(line, &srcrect_wall, screen, &dstrect_wall);
				dstrect_wall.x=window_width-(window_width-324)/2; //wall_far.png width=324
				SDL_RenderCopy(renderer, line, &srcrect_wall, &dstrect_wall);
				//SDL_BlitSurface(line, &srcrect_wall, screen, &dstrect_wall);
			}
			//Если присутствует боковая левая стена (ближняя) - отрисовка её, иначе производится проверка
			//на присутствие дальней боковой левой стены //FIXME: повторно!
			//и отрисовка её передней части
			//В любом случае затем отрисовывается разделительная линия
			if (labyrinth[player_coordinate-1*sign] == WALL)
			{
				dstrect_wall.x=0;
				dstrect_wall.y=0;
				dstrect_wall.w=53; //wall_side_left width=53
				dstrect_wall.h=480; //wall_side_left height=480
				SDL_RenderCopy(renderer, wall_side_left, NULL, &dstrect_wall);
				//SDL_BlitSurface(wall_side_left, NULL, screen, NULL);
				srcrect_wall.x=0;
				srcrect_wall.y=0;
				srcrect_wall.h=391; //line.png height=391 wall.png height=391
				srcrect_wall.w=3; //line.png width=3
				dstrect_wall.x=(window_width-538)/2; //wall.png width=538
				dstrect_wall.y=(window_height-391)/2;
				dstrect_wall.h=391;
				dstrect_wall.w=3;
				SDL_RenderCopy(renderer, line, &srcrect_wall, &dstrect_wall);
				//SDL_BlitSurface(line, &srcrect_wall, screen, &dstrect_wall);
			}
			else if (labyrinth[player_coordinate-sign*size_labyrinth_length-1*sign] == WALL)
			{
				srcrect_wall.x=538-(window_width-538)/2;
				srcrect_wall.y=0;
				srcrect_wall.h=391;
				srcrect_wall.w=(window_width-538)/2;
				dstrect_wall.x=0; //wall.png widht=538
				dstrect_wall.y=(window_height-391)/2; //wall.png height=391
				dstrect_wall.h=391;
				dstrect_wall.w=(window_width-538)/2;
				SDL_RenderCopy(renderer, wall, &srcrect_wall, &dstrect_wall);
				//SDL_BlitSurface(wall, &srcrect_wall, screen, &dstrect_wall);
				srcrect_wall.x=0;
				srcrect_wall.y=0;
				srcrect_wall.h=391; //line.png height=391 wall.png height=391
				srcrect_wall.w=3; //line.png width=3
				dstrect_wall.x=(window_width-538)/2; //wall.png width=538
				dstrect_wall.y=(window_height-391)/2;
				dstrect_wall.h=391;
				dstrect_wall.w=3;
				SDL_RenderCopy(renderer, line, &srcrect_wall, &dstrect_wall);
				//SDL_BlitSurface(line, &srcrect_wall, screen, &dstrect_wall);
			}
			//Если присутствует боковая правая стена (ближняя) - отрисовка её, иначе производится проверка
			//на присутствие дальней боковой правой стены //FIXME: повторно!
			//и отрисовка её передней части
			//В любом случае затем отрисовывается разделительная линия
			if (labyrinth[player_coordinate+1*sign] == WALL)
			{
				dstrect_wall.x=window_width-(window_width-538)/2; //wall.png widht=538
				dstrect_wall.y=0;
				dstrect_wall.w=53; //wall_side_right width=53
				dstrect_wall.h=481; //wall_side_right height=481
				SDL_RenderCopy(renderer, wall_side_right, NULL, &dstrect_wall);
				//SDL_BlitSurface(wall_side_right, NULL, screen, &dstrect_wall);
				srcrect_wall.x=0;
				srcrect_wall.y=0;
				srcrect_wall.h=391; //line.png height=391 wall.png height=391
				srcrect_wall.w=3; //line.png width=3
				dstrect_wall.x=window_width-(window_width-538)/2; //wall.png width=538
				dstrect_wall.y=(window_height-391)/2;
				dstrect_wall.h=391;
				dstrect_wall.w=3;
				SDL_RenderCopy(renderer, line, &srcrect_wall, &dstrect_wall);
				//SDL_BlitSurface(line, &srcrect_wall, screen, &dstrect_wall);
			}
			else if (labyrinth[player_coordinate-sign*size_labyrinth_length+1*sign] == WALL)
			{
				srcrect_wall.x=0;
				srcrect_wall.y=0;
				srcrect_wall.h=391;
				srcrect_wall.w=(window_width-538)/2;
				dstrect_wall.x=window_width-(window_width-538)/2; //wall.png widht=538
				dstrect_wall.y=(window_height-391)/2; //wall.png height=391
				dstrect_wall.h=391;
				dstrect_wall.w=(window_width-538)/2;
				SDL_RenderCopy(renderer, wall, &srcrect_wall, &dstrect_wall);
				//SDL_BlitSurface(wall, &srcrect_wall, screen, &dstrect_wall);
				srcrect_wall.x=0;
				srcrect_wall.y=0;
				srcrect_wall.h=391; //line.png height=391 wall.png height=391
				srcrect_wall.w=3; //line.png width=3
				dstrect_wall.x=window_width-(window_width-538)/2; //wall.png width=538
				dstrect_wall.y=(window_height-391)/2;
				dstrect_wall.h=391;
				dstrect_wall.w=3;
				SDL_RenderCopy(renderer, line, &srcrect_wall, &dstrect_wall);
				//SDL_BlitSurface(line, &srcrect_wall, screen, &dstrect_wall);
			}
		}
		//FIXME: Проверка границ лабиринта в условиях! + добавить крайние стены
		//Если игрок направлен вправо или влево по структуре лабиринта
		if (player[0].direction == RIGHT || player[0].direction == LEFT)
		{
			int sign; //левая и правая стороны ниже - относительно игрока
			//Если игрок направлен вправо по структуре лабиринта
			if (player[0].direction == RIGHT)
				sign=1;
			//Если игрок направлен влево по структуре лабиринта
			else
				sign=-1;
			//Если впереди стена  (в том числе крайняя) - отрисовка пустого фона и стены, иначе - отрисовка дальнего плана
			if ((player[0].y+1*sign) < 0 || (player[0].y+1*sign) >= size_labyrinth_length || labyrinth[player_coordinate+1*sign] == WALL)
			{
				//SDL_BlitSurface(empty_far_background, NULL, screen, NULL);
				dstrect_wall.x=0;
				dstrect_wall.y=(window_height-391)/2; //wall.png height=391 empty_background height=391
				dstrect_wall.h=391;
				dstrect_wall.w=640;
				SDL_RenderCopy(renderer, empty_background, NULL, &dstrect_wall);
				//SDL_BlitSurface(empty_background, NULL, screen, &dstrect_wall);
				dstrect_wall.x=(window_width-538)/2; //wall.png widht=538
				dstrect_wall.w=538;
				SDL_RenderCopy(renderer, wall, NULL, &dstrect_wall);
				//SDL_BlitSurface(wall, NULL, screen, &dstrect_wall);
			}
			else
			{
				//Отрисовка пустоты на заднем фоне
				dstrect_wall.x=0;
				dstrect_wall.y=(window_height-209)/2; //empty_far_background.png height=209
				dstrect_wall.h=209;
				dstrect_wall.w=640;
				SDL_RenderCopy(renderer, empty_far_background, NULL, &dstrect_wall);
				//SDL_BlitSurface(empty_far_background, NULL, screen, &dstrect_wall);
				//Отрисовка дальней стены
				if (labyrinth[player_coordinate+2*sign] == WALL)
				{
					dstrect_wall.x=(window_width-324)/2; //wall_far width=324
					dstrect_wall.y=(window_height-211)/2; //wall_far height=211
					dstrect_wall.h=211;
					dstrect_wall.w=324;
					SDL_RenderCopy(renderer, wall_far, NULL, &dstrect_wall);
					//SDL_BlitSurface(wall_far, NULL, screen, &dstrect_wall);
				}
				//Если присутствует дальняя боковая левая стена - отрисовка её, иначе - отрисовка дальней левой стены (не боковой)
				if (labyrinth[player_coordinate-sign*size_labyrinth_length+1*sign] == WALL)
				{
					dstrect_wall.x=(window_width-324)/2-109; //wall_far width=324 wall_side_far_left width=109
					dstrect_wall.y=(window_height-391)/2; //wall_side_far_left height=391
					dstrect_wall.h=391;
					dstrect_wall.w=109;
					SDL_RenderCopy(renderer, wall_side_far_left, NULL, &dstrect_wall);
					//SDL_BlitSurface(wall_side_far_left, NULL, screen, &dstrect_wall);
				}
				else if (labyrinth[player_coordinate-sign*size_labyrinth_length+2*sign] == WALL)
				{
					srcrect_wall.x=324-(window_width-324)/2;
					srcrect_wall.y=0;
					srcrect_wall.h=211;
					srcrect_wall.w=(window_width-324)/2;
					dstrect_wall.x=0;
					dstrect_wall.y=(window_height-211)/2;
					dstrect_wall.h=211;
					dstrect_wall.w=(window_width-324)/2;
					SDL_RenderCopy(renderer, wall_far, &srcrect_wall, &dstrect_wall);
					//SDL_BlitSurface(wall_far, &srcrect_wall, screen, &dstrect_wall);
				}
				//Если присутствует дальняя боковая правая стена - отрисовка её, иначе - отрисовка дальней правой стены (не боковой)
				if (labyrinth[player_coordinate+sign*size_labyrinth_length+1*sign] == WALL)
				{
					//(window_width-324)/2+324=(window_width-324)/2+648/2=(window_width-324+648)/2=(window_width+324)/2
					dstrect_wall.x=(window_width+324)/2; //wall_far width=324
					dstrect_wall.y=(window_height-393)/2; //wall_side_far_left height=393
					dstrect_wall.h=393;
					dstrect_wall.w=109;
					SDL_RenderCopy(renderer, wall_side_far_right, NULL, &dstrect_wall);
					//SDL_BlitSurface(wall_side_far_right, NULL, screen, &dstrect_wall);
				}
				else if (labyrinth[player_coordinate+sign*size_labyrinth_length+2*sign] == WALL)
				{
					srcrect_wall.x=0;
					srcrect_wall.y=0;
					srcrect_wall.h=211;
					srcrect_wall.w=(window_width-324)/2;
					dstrect_wall.x=window_width-(window_width-324)/2;
					dstrect_wall.y=(window_height-211)/2;
					dstrect_wall.h=211;
					dstrect_wall.w=(window_width-324)/2;
					SDL_RenderCopy(renderer, wall_far, &srcrect_wall, &dstrect_wall);
					//SDL_BlitSurface(wall_far, &srcrect_wall, screen, &dstrect_wall);
				}
				//Отрисовка дальних линий-разделителей левой и правой
				srcrect_wall.x=0;
				srcrect_wall.y=0;
				srcrect_wall.h=211; //line.png height=391 wall_far.png height=211
				srcrect_wall.w=1; //line.png width-2
				dstrect_wall.x=(window_width-324)/2; //wall_far.png width=324
				dstrect_wall.y=(window_height-211)/2;
				dstrect_wall.h=211;
				dstrect_wall.w=1;
				SDL_RenderCopy(renderer, line, &srcrect_wall, &dstrect_wall);
				//SDL_BlitSurface(line, &srcrect_wall, screen, &dstrect_wall);
				dstrect_wall.x=window_width-(window_width-324)/2; //wall_far.png width=324
				SDL_RenderCopy(renderer, line, &srcrect_wall, &dstrect_wall);
				//SDL_BlitSurface(line, &srcrect_wall, screen, &dstrect_wall);
			}
			//Если присутствует боковая левая стена (ближняя) - отрисовка её, иначе производится проверка
			//на присутствие дальней боковой левой стены //FIXME: повторно!
			//и отрисовка её передней части
			//В любом случае затем отрисовывается разделительная линия
			if (labyrinth[player_coordinate-size_labyrinth_length*sign] == WALL)
			{
				dstrect_wall.x=0;
				dstrect_wall.y=0;
				dstrect_wall.w=53; //wall_side_left.png width=53
				dstrect_wall.h=480; //wall_side_left.png height=480
				SDL_RenderCopy(renderer, wall_side_left, NULL, &dstrect_wall);
				//SDL_BlitSurface(wall_side_left, NULL, screen, NULL);
				srcrect_wall.x=0;
				srcrect_wall.y=0;
				srcrect_wall.h=391; //line.png height=391 wall.png height=391
				srcrect_wall.w=3; //line.png width=3
				dstrect_wall.x=(window_width-538)/2; //wall.png width=538
				dstrect_wall.y=(window_height-391)/2;
				dstrect_wall.h=391;
				dstrect_wall.w=3;
				SDL_RenderCopy(renderer, line, &srcrect_wall, &dstrect_wall);
				//SDL_BlitSurface(line, &srcrect_wall, screen, &dstrect_wall);
			}
			else if (labyrinth[player_coordinate-sign*size_labyrinth_length+1*sign] == WALL)
			{
				srcrect_wall.x=538-(window_width-538)/2;
				srcrect_wall.y=0;
				srcrect_wall.h=391;
				srcrect_wall.w=(window_width-538)/2;
				dstrect_wall.x=0; //wall.png widht=538
				dstrect_wall.y=(window_height-391)/2; //wall.png height=391
				dstrect_wall.h=391;
				dstrect_wall.w=(window_width-538)/2;
				SDL_RenderCopy(renderer, wall, &srcrect_wall, &dstrect_wall);
				//SDL_BlitSurface(wall, &srcrect_wall, screen, &dstrect_wall);
				srcrect_wall.x=0;
				srcrect_wall.y=0;
				srcrect_wall.h=391; //line.png height=391 wall.png height=391
				srcrect_wall.w=3; //line.png width=3
				dstrect_wall.x=(window_width-538)/2; //wall.png width=538
				dstrect_wall.y=(window_height-391)/2;
				dstrect_wall.h=391;
				dstrect_wall.w=3;
				SDL_RenderCopy(renderer, line, &srcrect_wall, &dstrect_wall);
				//SDL_BlitSurface(line, &srcrect_wall, screen, &dstrect_wall);
			}
			//Если присутствует боковая правая стена (ближняя) - отрисовка её, иначе производится проверка
			//на присутствие дальней боковой правой стены //FIXME: повторно!
			//и отрисовка её передней части
			//В любом случае затем отрисовывается разделительная линия
			if (labyrinth[player_coordinate+size_labyrinth_length*sign] == WALL)
			{
				dstrect_wall.x=window_width-(window_width-538)/2; //wall.png widht=538
				dstrect_wall.y=0;
				dstrect_wall.w=53; //wall_side_right.png width=53
				dstrect_wall.h=481; //wall_side_right.png height=481
				SDL_RenderCopy(renderer, wall_side_right, NULL, &dstrect_wall);
				//SDL_BlitSurface(wall_side_right, NULL, screen, &dstrect_wall);
				srcrect_wall.x=0;
				srcrect_wall.y=0;
				srcrect_wall.h=391; //line.png height=391 wall.png height=391
				srcrect_wall.w=3; //line.png width=3
				dstrect_wall.x=window_width-(window_width-538)/2; //wall.png width=538
				dstrect_wall.y=(window_height-391)/2;
				dstrect_wall.h=391;
				dstrect_wall.w=3;
				SDL_RenderCopy(renderer, line, &srcrect_wall, &dstrect_wall);
				//SDL_BlitSurface(line, &srcrect_wall, screen, &dstrect_wall);
			}
			else if (labyrinth[player_coordinate+sign*size_labyrinth_length+1*sign] == WALL)
			{
				srcrect_wall.x=0;
				srcrect_wall.y=0;
				srcrect_wall.h=391;
				srcrect_wall.w=(window_width-538)/2;
				dstrect_wall.x=window_width-(window_width-538)/2; //wall.png widht=538
				dstrect_wall.y=(window_height-391)/2; //wall.png height=391
				dstrect_wall.h=391;
				dstrect_wall.w=(window_width-538)/2;
				SDL_RenderCopy(renderer, wall, &srcrect_wall, &dstrect_wall);
				//SDL_BlitSurface(wall, &srcrect_wall, screen, &dstrect_wall);
				srcrect_wall.x=0;
				srcrect_wall.y=0;
				srcrect_wall.h=391; //line.png height=391 wall.png height=391
				srcrect_wall.w=3; //line.png width=3
				dstrect_wall.x=window_width-(window_width-538)/2; //wall.png width=538
				dstrect_wall.y=(window_height-391)/2;
				dstrect_wall.h=391;
				dstrect_wall.w=3;
				SDL_RenderCopy(renderer, line, &srcrect_wall, &dstrect_wall);
				//SDL_BlitSurface(line, &srcrect_wall, screen, &dstrect_wall);
			}
		}
		//~ SDL_BlitSurface(wall_side_left, NULL, screen, NULL);
		//~ dstrect_wall.x=window_width-(window_width-538)/2; //wall.png widht=538
		//~ dstrect_wall.y=0;
		//~ SDL_BlitSurface(wall_side_right, NULL, screen, &dstrect_wall);
		//~ dstrect_wall.x=(window_width-538)/2; //wall.png widht=538
		//~ dstrect_wall.y=(window_height-391)/2; //wall.png height=391
		//SDL_BlitSurface(wall, NULL, screen, &dstrect_wall);
		//Вывод кадра на экран
		SDL_RenderPresent(renderer);
		//SDL_UpdateWindowSurface(main_window);
		//SDL_Delay(3000);
		
		//Ожидание действий игрока
		while (SDL_PollEvent(&event))
		{
			//Если произошёл вызов закрытия программы по причине закрытия окна программы
			//или игрок нажал ESC - завершение работы программы
			if (event.type == SDL_QUIT)
				request_for_quit=1;
			if (event.type == SDL_KEYDOWN)
			{
				labyrinth[player_coordinate]=previous_state_hw; //для отладки и контроля через консоль
				/*switch (event.key.keysym.scancode)
				{
					case SDL_SCANCODE_ESCAPE:
						request_for_quit=1;
						break;
				}*/
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						request_for_quit=1;
						break;
					//Если игрок нажал клавиши стрелок "вверх" или "вниз", или WS - 
					//производится изменение координат игрока в заданную сторону
					//(если игрок не пытается пройти сквозь стены, в том числе крайние)
					//Если игрок нажал клавиши "влево" или "вправо", или AD - 
					//производится изменение направления, в котором смотрит игрок
					//(0 - вверх, 1 - вправо, 2 - вниз, 3 - влево)
					case SDLK_UP:
					case SDLK_w:
						if ((player[0].direction ==  UP) && (player[0].x > 1) && (labyrinth[player_coordinate-size_labyrinth_length] != WALL))
							player[0].x=player[0].x-1;
						//break после каждого if ?
						if ((player[0].direction ==  RIGHT) && (player[0].y < size_labyrinth_length-2) && (labyrinth[player_coordinate+1] != WALL))
							player[0].y=player[0].y+1;
						if ((player[0].direction ==  DOWN) && (player[0].x < size_labyrinth_width-2) && (labyrinth[player_coordinate+size_labyrinth_length] != WALL))
							player[0].x=player[0].x+1;
						if ((player[0].direction ==  LEFT) && (player[0].y > 1) && (labyrinth[player_coordinate-1] != WALL))
							player[0].y=player[0].y-1;
						break;
					case SDLK_DOWN:
					case SDLK_s:
						if ((player[0].direction ==  UP) && (player[0].x < size_labyrinth_width-1) && (labyrinth[player_coordinate+size_labyrinth_length] != WALL))
							player[0].x=player[0].x+1;
						if ((player[0].direction ==  RIGHT) && (player[0].y > 0) && (labyrinth[player_coordinate-1] != WALL))
							player[0].y=player[0].y-1;
						if ((player[0].direction ==  DOWN) && (player[0].x > 0) && (labyrinth[player_coordinate-size_labyrinth_length] != WALL))
							player[0].x=player[0].x-1;
						if ((player[0].direction ==  LEFT) && (player[0].y < size_labyrinth_length-1) && (labyrinth[player_coordinate+1] != WALL))
							player[0].y=player[0].y+1;
						break;
					case SDLK_LEFT:
					case SDLK_a: //если подключить работу мыши/модификаторов (Shift) - задать на AD шаг влево/вправо вместо поворота
						//~ if (player[0].direction == 0)
							//~ player[0].direction=3;
						//~ break;
						//~ player[0].direction=player[0].direction-1;
						//~ break;						
						if (player[0].direction == UP)
							player[0].direction=LEFT;
						else
							player[0].direction=player[0].direction-1;
						break;
					case SDLK_RIGHT:
					case SDLK_d: //если подключить работу мыши/модификаторов (Shift) - задать на AD шаг влево/вправо вместо поворота
						if (player[0].direction == LEFT)
							player[0].direction=UP;
						else
							player[0].direction=player[0].direction+1;
						break;
				}
			}
			if (event.type == SDL_KEYUP)
			{
				switch (event.key.keysym.sym)
				{
					//Простая пауза
					case SDLK_p:
						SDL_WaitEvent(&event);
						break;
				}
			}
		}
		//Предыдущее состояние клетки сохраняется для последующего восстановления
		if (labyrinth[player[0].x*size_labyrinth_length+player[0].y] != PLAYER)
			previous_state_hw=labyrinth[player[0].x*size_labyrinth_length+player[0].y];
		labyrinth[player[0].x*size_labyrinth_length+player[0].y]=PLAYER;
		//Для отладки и контроля через консоль
		show_labyrinth_in_cmd_hw(player_coordinate, player, size_labyrinth_length, size_labyrinth_width, labyrinth);
		//Если с момента начала обработки кадра прошло меньше времени,
		//чем требуется для заданного значения FPS - обработка приостанавливается на остаток требуемого времени
		if ((SDL_GetTicks()-ticks_begin) < (1000/FPS))
			SDL_Delay((1000/FPS)-(SDL_GetTicks()-ticks_begin));
		count++;
		//char *dir[]={"вверх", "вправо", "вниз", "влево"};
		//printf("\rcell: %i  x: %i  y: %i  dir: %s  ", player_coordinate, player[0].x, player[0].y, dir[player[0].direction]);
	}
	char *mode[]={"software", "software vsync", "hardware", "hardware vsync"};
	printf("\nКадров: %i Миллисекунд: %i FPS: %.2f mode: %s\n", count, SDL_GetTicks(), (float)count/SDL_GetTicks()*1000, mode[requested_mode-1]);

	//TODO: перенести в clean_up_sdl()
	/*SDL_FreeSurface(background);
	SDL_FreeSurface(wall);
	SDL_FreeSurface(wall_side_left);
	SDL_FreeSurface(wall_side_right);
	SDL_FreeSurface(wall_far);
	SDL_FreeSurface(wall_side_far_left);
	SDL_FreeSurface(wall_side_far_right);
	SDL_FreeSurface(line);
	SDL_FreeSurface(empty_background);
	SDL_FreeSurface(empty_far_background);*/
	SDL_DestroyTexture(background);
	SDL_DestroyTexture(wall);
	SDL_DestroyTexture(wall_side_left);
	SDL_DestroyTexture(wall_side_right);
	SDL_DestroyTexture(wall_far);
	SDL_DestroyTexture(wall_side_far_left);
	SDL_DestroyTexture(wall_side_far_right);
	SDL_DestroyTexture(line);
	SDL_DestroyTexture(empty_background);
	SDL_DestroyTexture(empty_far_background);
/*	IMG_Quit();
	//SDL_DestroyTexture(bitmapTex);
    //SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(main_window);
	SDL_Quit();*/
	clean_up_sdl_hw(main_window, renderer);
	return 0;
}

//Корректное завершение работы SDL
void clean_up_sdl_hw(SDL_Window *main_window, SDL_Renderer *renderer)
{
	//~ SDL_FreeSurface(background);
	//~ SDL_FreeSurface(wall);
	//~ SDL_FreeSurface(wall_side_left);
	//~ SDL_FreeSurface(wall_side_right);
	//~ SDL_FreeSurface(wall_far);
	//~ SDL_FreeSurface(wall_side_far_left);
	//~ SDL_FreeSurface(wall_side_far_right);
	//~ SDL_FreeSurface(line);
	//~ SDL_FreeSurface(empty_background);
	//~ SDL_FreeSurface(empty_far_background);	
	IMG_Quit();
	if (renderer != NULL)
		SDL_DestroyRenderer(renderer);
	if (main_window != NULL)
		SDL_DestroyWindow(main_window);
	//Uint32 SDL_WasInit(Uint32 flags)
	SDL_Quit();
	return;
}

void show_labyrinth_in_cmd_hw(int const player_coordinate, const struct players player[], int const length, int const width, int const *labyrinth_temp)
{
	for (int j = 0; j < width; j++)
	{
		for (int k = 0; k < length; k++)
		{
			//0 - пустая клетка, 1 - стена, 2 - будущая пустая клетка, 3 - один из соперников, 4 - игрок, 5 - выход
			if (!labyrinth_temp[(j*length)+k])
				printf("%s", "  ");
			if (labyrinth_temp[(j*length)+k] == WALL)
				printf("%s", " X");
			if (labyrinth_temp[(j*length)+k] == DUMMY)
				printf("%s", " .");
			if (labyrinth_temp[(j*length)+k] == RIVAL)
				printf("%s", " p");
			if (labyrinth_temp[(j*length)+k] == PLAYER)
			{
				//0 - вверх, 1 - вправо, 2 - вниз, 3 - влево
				if (!player[0].direction)
					printf("%s", " ^");
				if (player[0].direction == RIGHT)
					printf("%s", " >");
				if (player[0].direction == DOWN)
					printf("%s", " v");
				if (player[0].direction == LEFT)
					printf("%s", " <");
			}
			if (labyrinth_temp[(j*length)+k] == EXIT)
				printf("%s", " e");
		}
		char *dir[]={"вверх", "вправо", "вниз", "влево"};
		if (j == 0)
			printf("  %s%i", "cell:", player_coordinate);
		if (j == 1)
			printf("  %s%i %s%i", "x=", player[0].x, "y=", player[0].y);
		if (j == 2)
			printf("  %s%s", "dir:", dir[player[0].direction]);
		printf("%s", "\n");
	}
	printf("%s", "\n");
	return;
}
