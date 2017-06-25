/*  sdl_main.c

  Лабиринт
  Version 0.2

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

void show_labyrinth_in_cmd(int const player_coordinate, const struct players player[], int const length, int const width, int const *labyrinth_temp); //для отладки и контроля через консоль

void clean_up_sdl(SDL_Window *main_window);

int sdl_main(int *labyrinth, struct players player[], int const size_labyrinth_length, int const size_labyrinth_width)
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
		clean_up_sdl(NULL);
		return 1;
	}
	/*SDL_Renderer *renderer = NULL;
	SDL_Texture *bitmapTex = NULL;
	renderer = SDL_CreateRenderer(main_window, -1, SDL_RENDERER_ACCELERATED);*/
	//Инициализация поверхности окна
	SDL_Surface *screen=NULL;
	screen=SDL_GetWindowSurface(main_window);
	//Загрузка графических ресурсов
	if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
	{
		printf("IMG_Init: Failed to init required png support!\n");
		printf("IMG_Init: %s\n", IMG_GetError());
		clean_up_sdl(main_window);
		return 1;
	}
	SDL_Surface *background=NULL;
	SDL_Surface *wall=NULL;
	SDL_Surface *wall_side_left=NULL;
	SDL_Surface *wall_side_right=NULL;
	SDL_Surface *wall_far=NULL;
	SDL_Surface *wall_side_far_left=NULL;
	SDL_Surface *wall_side_far_right=NULL;
	SDL_Surface *line=NULL;
	SDL_Surface *empty_background=NULL;
	SDL_Surface *empty_far_background=NULL;
	
	background=IMG_Load("gfx/background.png");
	wall=IMG_Load("gfx/wall.png");
	wall_side_left=IMG_Load("gfx/wall_side_left.png");
	wall_side_right=IMG_Load("gfx/wall_side_right.png");
	wall_far=IMG_Load("gfx/wall_far.png");
	wall_side_far_left=IMG_Load("gfx/wall_side_far_left.png");
	wall_side_far_right=IMG_Load("gfx/wall_side_far_right.png");
	line=IMG_Load("gfx/line.png");
	empty_background=IMG_Load("gfx/empty_background.png");
	empty_far_background=IMG_Load("gfx/empty_far_background.png");
	//Если не удалось загрузить какие-то файлы - вывести сообщение об ошибке с указанием последнего не загруженного файла
	if (!background || !wall || !wall_side_left || !wall_side_right || !wall_far || !wall_side_far_left || !wall_side_far_right || !line || !empty_background || !empty_far_background)
	{
		printf("IMG_Load: %s\n", IMG_GetError());
		clean_up_sdl(main_window);
		return 1;
	}
	/*if(!wall)
	{
		printf("IMG_Load: %s\n", IMG_GetError());
		clean_up_sdl(main_window);
		return 1;
	}*/
	
	SDL_Event event;
	int request_for_quit=0;
	int count=0;
	//Начало основного цикла - пока не произойдёт вызов закрытия программы
	while (!request_for_quit)
	{
		Uint32 ticks_begin=SDL_GetTicks(); //время начала очередного кадра
		//Вычисление координаты игрока
		int player_coordinate=player[0].x*size_labyrinth_length+player[0].y;
		//Подготовка кадра (расположение изображений в нужном порядке по соответствующим координатам)
		/*bitmapTex = SDL_CreateTextureFromSurface(renderer, wall);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, bitmapTex, NULL, NULL);
		SDL_RenderPresent(renderer);*/
		SDL_Rect srcrect_wall, dstrect_wall;
		//Отрисовка фона
		SDL_BlitSurface(background, NULL, screen, NULL);
		//FIXME: Проверка границ лабиринта в условиях! + добавить крайние стены
		//Если игрок направлен вверх или вниз по структуре лабиринта
		if (player[0].direction == 0 || player[0].direction == 2)
		{
			int sign; //левая и правая стороны ниже - относительно игрока
			//Если игрок направлен вверх по структуре лабиринта
			if (player[0].direction == 0)
				sign=1;
			//Если игрок направлен вниз по структуре лабиринта
			else
				sign=-1;
			//Если впереди стена (в том числе крайняя) - отрисовка пустого фона и стены, иначе - отрисовка дальнего плана
			if ((player[0].x-sign*1) < 0 || (player_coordinate-sign*size_labyrinth_length) >= size_labyrinth_width*size_labyrinth_length || labyrinth[player_coordinate-sign*size_labyrinth_length] == 1)
			{
				//SDL_BlitSurface(empty_far_background, NULL, screen, NULL);
				dstrect_wall.x=0;
				dstrect_wall.y=(window_height-391)/2; //wall.png height=391 empty_background height=391
				SDL_BlitSurface(empty_background, NULL, screen, &dstrect_wall);
				dstrect_wall.x=(window_width-538)/2; //wall.png widht=538
				SDL_BlitSurface(wall, NULL, screen, &dstrect_wall);
			}
			else
			{
				//Отрисовка пустоты на заднем фоне
				dstrect_wall.x=0;
				dstrect_wall.y=(window_height-209)/2; //empty_far_background.png height=209
				SDL_BlitSurface(empty_far_background, NULL, screen, &dstrect_wall);
				//Отрисовка дальней стены
				if (labyrinth[player_coordinate-sign*size_labyrinth_length*2] == 1)
				{
					dstrect_wall.x=(window_width-324)/2; //wall_far width=324
					dstrect_wall.y=(window_height-211)/2; //wall_far height=211
					SDL_BlitSurface(wall_far, NULL, screen, &dstrect_wall);
				}
				//Если присутствует дальняя боковая левая стена - отрисовка её, иначе - отрисовка дальней левой стены (не боковой)
				if (labyrinth[player_coordinate-sign*size_labyrinth_length-1*sign] == 1)
				{
					dstrect_wall.x=(window_width-324)/2-109; //wall_far width=324 wall_side_far_left width=109
					dstrect_wall.y=(window_height-391)/2; //wall_side_far_left height=391
					SDL_BlitSurface(wall_side_far_left, NULL, screen, &dstrect_wall);
				}
				else if (labyrinth[player_coordinate-sign*size_labyrinth_length*2-1*sign] == 1)
				{
					srcrect_wall.x=324-(window_width-324)/2;
					srcrect_wall.y=0;
					srcrect_wall.h=211;
					srcrect_wall.w=(window_width-324)/2;
					dstrect_wall.x=0;
					dstrect_wall.y=(window_height-211)/2;
					SDL_BlitSurface(wall_far, &srcrect_wall, screen, &dstrect_wall);
				}
				//Если присутствует дальняя боковая правая стена - отрисовка её, иначе - отрисовка дальней правой стены (не боковой)
				if (labyrinth[player_coordinate-sign*size_labyrinth_length+1*sign] == 1)
				{
					//(window_width-324)/2+324=(window_width-324)/2+648/2=(window_width-324+648)/2=(window_width+324)/2
					dstrect_wall.x=(window_width+324)/2; //wall_far width=324
					dstrect_wall.y=(window_height-393)/2; //wall_side_far_left height=393
					SDL_BlitSurface(wall_side_far_right, NULL, screen, &dstrect_wall);
				}
				else if (labyrinth[player_coordinate-sign*size_labyrinth_length*2+1*sign] == 1)
				{
					srcrect_wall.x=0;
					srcrect_wall.y=0;
					srcrect_wall.h=211;
					srcrect_wall.w=(window_width-324)/2;
					dstrect_wall.x=window_width-(window_width-324)/2;
					dstrect_wall.y=(window_height-211)/2;
					SDL_BlitSurface(wall_far, &srcrect_wall, screen, &dstrect_wall);
				}
				//Отрисовка дальних линий-разделителей левой и правой
				srcrect_wall.x=0;
				srcrect_wall.y=0;
				srcrect_wall.h=211; //line.png height=391 wall_far.png height=211
				srcrect_wall.w=1; //line.png width-2
				dstrect_wall.x=(window_width-324)/2; //wall_far.png width=324
				dstrect_wall.y=(window_height-211)/2;
				SDL_BlitSurface(line, &srcrect_wall, screen, &dstrect_wall);
				dstrect_wall.x=window_width-(window_width-324)/2; //wall_far.png width=324
				SDL_BlitSurface(line, &srcrect_wall, screen, &dstrect_wall);
			}
			//Если присутствует боковая левая стена (ближняя) - отрисовка её, иначе производится проверка
			//на присутствие дальней боковой левой стены //FIXME: повторно!
			//и отрисовка её передней части
			//В любом случае затем отрисовывается разделительная линия
			if (labyrinth[player_coordinate-1*sign] == 1)
			{
				SDL_BlitSurface(wall_side_left, NULL, screen, NULL);
				srcrect_wall.x=0;
				srcrect_wall.y=0;
				srcrect_wall.h=391; //line.png height=391 wall.png height=391
				srcrect_wall.w=3; //line.png width=3
				dstrect_wall.x=(window_width-538)/2; //wall.png width=538
				dstrect_wall.y=(window_height-391)/2;
				SDL_BlitSurface(line, &srcrect_wall, screen, &dstrect_wall);
			}
			else if (labyrinth[player_coordinate-sign*size_labyrinth_length-1*sign] == 1)
			{
				srcrect_wall.x=538-(window_width-538)/2;
				srcrect_wall.y=0;
				srcrect_wall.h=391;
				srcrect_wall.w=(window_width-538)/2;
				dstrect_wall.x=0; //wall.png widht=538
				dstrect_wall.y=(window_height-391)/2; //wall.png height=391
				SDL_BlitSurface(wall, &srcrect_wall, screen, &dstrect_wall);
				srcrect_wall.x=0;
				srcrect_wall.y=0;
				srcrect_wall.h=391; //line.png height=391 wall.png height=391
				srcrect_wall.w=3; //line.png width=3
				dstrect_wall.x=(window_width-538)/2; //wall.png width=538
				dstrect_wall.y=(window_height-391)/2;
				SDL_BlitSurface(line, &srcrect_wall, screen, &dstrect_wall);
			}
			//Если присутствует боковая правая стена (ближняя) - отрисовка её, иначе производится проверка
			//на присутствие дальней боковой правой стены //FIXME: повторно!
			//и отрисовка её передней части
			//В любом случае затем отрисовывается разделительная линия
			if (labyrinth[player_coordinate+1*sign] == 1)
			{
				dstrect_wall.x=window_width-(window_width-538)/2; //wall.png widht=538
				dstrect_wall.y=0;
				SDL_BlitSurface(wall_side_right, NULL, screen, &dstrect_wall);
				srcrect_wall.x=0;
				srcrect_wall.y=0;
				srcrect_wall.h=391; //line.png height=391 wall.png height=391
				srcrect_wall.w=3; //line.png width=3
				dstrect_wall.x=window_width-(window_width-538)/2; //wall.png width=538
				dstrect_wall.y=(window_height-391)/2;
				SDL_BlitSurface(line, &srcrect_wall, screen, &dstrect_wall);
			}
			else if (labyrinth[player_coordinate-sign*size_labyrinth_length+1*sign] == 1)
			{
				srcrect_wall.x=0;
				srcrect_wall.y=0;
				srcrect_wall.h=391;
				srcrect_wall.w=(window_width-538)/2;
				dstrect_wall.x=window_width-(window_width-538)/2; //wall.png widht=538
				dstrect_wall.y=(window_height-391)/2; //wall.png height=391
				SDL_BlitSurface(wall, &srcrect_wall, screen, &dstrect_wall);
				srcrect_wall.x=0;
				srcrect_wall.y=0;
				srcrect_wall.h=391; //line.png height=391 wall.png height=391
				srcrect_wall.w=3; //line.png width=3
				dstrect_wall.x=window_width-(window_width-538)/2; //wall.png width=538
				dstrect_wall.y=(window_height-391)/2;
				SDL_BlitSurface(line, &srcrect_wall, screen, &dstrect_wall);
			}
		}
		//FIXME: Проверка границ лабиринта в условиях! + добавить крайние стены
		//Если игрок направлен вправо или влево по структуре лабиринта
		if (player[0].direction == 1 || player[0].direction == 3)
		{
			int sign; //левая и правая стороны ниже - относительно игрока
			//Если игрок направлен вправо по структуре лабиринта
			if (player[0].direction == 1)
				sign=1;
			//Если игрок направлен влево по структуре лабиринта
			else
				sign=-1;
			//Если впереди стена  (в том числе крайняя) - отрисовка пустого фона и стены, иначе - отрисовка дальнего плана
			if ((player[0].y+1*sign) < 0 || (player[0].y+1*sign) >= size_labyrinth_length || labyrinth[player_coordinate+1*sign] == 1)
			{
				//SDL_BlitSurface(empty_far_background, NULL, screen, NULL);
				dstrect_wall.x=0;
				dstrect_wall.y=(window_height-391)/2; //wall.png height=391 empty_background height=391
				SDL_BlitSurface(empty_background, NULL, screen, &dstrect_wall);
				dstrect_wall.x=(window_width-538)/2; //wall.png widht=538
				SDL_BlitSurface(wall, NULL, screen, &dstrect_wall);
			}
			else
			{
				//Отрисовка пустоты на заднем фоне
				dstrect_wall.x=0;
				dstrect_wall.y=(window_height-209)/2; //empty_far_background.png height=209
				SDL_BlitSurface(empty_far_background, NULL, screen, &dstrect_wall);
				//Отрисовка дальней стены
				if (labyrinth[player_coordinate+2*sign] == 1)
				{
					dstrect_wall.x=(window_width-324)/2; //wall_far width=324
					dstrect_wall.y=(window_height-211)/2; //wall_far height=211
					SDL_BlitSurface(wall_far, NULL, screen, &dstrect_wall);
				}
				//Если присутствует дальняя боковая левая стена - отрисовка её, иначе - отрисовка дальней левой стены (не боковой)
				if (labyrinth[player_coordinate-sign*size_labyrinth_length+1*sign] == 1)
				{
					dstrect_wall.x=(window_width-324)/2-109; //wall_far width=324 wall_side_far_left width=109
					dstrect_wall.y=(window_height-391)/2; //wall_side_far_left height=391
					SDL_BlitSurface(wall_side_far_left, NULL, screen, &dstrect_wall);
				}
				else if (labyrinth[player_coordinate-sign*size_labyrinth_length+2*sign] == 1)
				{
					srcrect_wall.x=324-(window_width-324)/2;
					srcrect_wall.y=0;
					srcrect_wall.h=211;
					srcrect_wall.w=(window_width-324)/2;
					dstrect_wall.x=0;
					dstrect_wall.y=(window_height-211)/2;
					SDL_BlitSurface(wall_far, &srcrect_wall, screen, &dstrect_wall);
				}
				//Если присутствует дальняя боковая правая стена - отрисовка её, иначе - отрисовка дальней правой стены (не боковой)
				if (labyrinth[player_coordinate+sign*size_labyrinth_length+1*sign] == 1)
				{
					//(window_width-324)/2+324=(window_width-324)/2+648/2=(window_width-324+648)/2=(window_width+324)/2
					dstrect_wall.x=(window_width+324)/2; //wall_far width=324
					dstrect_wall.y=(window_height-393)/2; //wall_side_far_left height=393
					SDL_BlitSurface(wall_side_far_right, NULL, screen, &dstrect_wall);
				}
				else if (labyrinth[player_coordinate+sign*size_labyrinth_length+2*sign] == 1)
				{
					srcrect_wall.x=0;
					srcrect_wall.y=0;
					srcrect_wall.h=211;
					srcrect_wall.w=(window_width-324)/2;
					dstrect_wall.x=window_width-(window_width-324)/2;
					dstrect_wall.y=(window_height-211)/2;
					SDL_BlitSurface(wall_far, &srcrect_wall, screen, &dstrect_wall);
				}
				//Отрисовка дальних линий-разделителей левой и правой
				srcrect_wall.x=0;
				srcrect_wall.y=0;
				srcrect_wall.h=211; //line.png height=391 wall_far.png height=211
				srcrect_wall.w=1; //line.png width-2
				dstrect_wall.x=(window_width-324)/2; //wall_far.png width=324
				dstrect_wall.y=(window_height-211)/2;
				SDL_BlitSurface(line, &srcrect_wall, screen, &dstrect_wall);
				dstrect_wall.x=window_width-(window_width-324)/2; //wall_far.png width=324
				SDL_BlitSurface(line, &srcrect_wall, screen, &dstrect_wall);
			}
			//Если присутствует боковая левая стена (ближняя) - отрисовка её, иначе производится проверка
			//на присутствие дальней боковой левой стены //FIXME: повторно!
			//и отрисовка её передней части
			//В любом случае затем отрисовывается разделительная линия
			if (labyrinth[player_coordinate-size_labyrinth_length*sign] == 1)
			{
				SDL_BlitSurface(wall_side_left, NULL, screen, NULL);
				srcrect_wall.x=0;
				srcrect_wall.y=0;
				srcrect_wall.h=391; //line.png height=391 wall.png height=391
				srcrect_wall.w=3; //line.png width=3
				dstrect_wall.x=(window_width-538)/2; //wall.png width=538
				dstrect_wall.y=(window_height-391)/2;
				SDL_BlitSurface(line, &srcrect_wall, screen, &dstrect_wall);
			}
			else if (labyrinth[player_coordinate-sign*size_labyrinth_length+1*sign] == 1)
			{
				srcrect_wall.x=538-(window_width-538)/2;
				srcrect_wall.y=0;
				srcrect_wall.h=391;
				srcrect_wall.w=(window_width-538)/2;
				dstrect_wall.x=0; //wall.png widht=538
				dstrect_wall.y=(window_height-391)/2; //wall.png height=391
				SDL_BlitSurface(wall, &srcrect_wall, screen, &dstrect_wall);
				srcrect_wall.x=0;
				srcrect_wall.y=0;
				srcrect_wall.h=391; //line.png height=391 wall.png height=391
				srcrect_wall.w=3; //line.png width=3
				dstrect_wall.x=(window_width-538)/2; //wall.png width=538
				dstrect_wall.y=(window_height-391)/2;
				SDL_BlitSurface(line, &srcrect_wall, screen, &dstrect_wall);
			}
			//Если присутствует боковая правая стена (ближняя) - отрисовка её, иначе производится проверка
			//на присутствие дальней боковой правой стены //FIXME: повторно!
			//и отрисовка её передней части
			//В любом случае затем отрисовывается разделительная линия
			if (labyrinth[player_coordinate+size_labyrinth_length*sign] == 1)
			{
				dstrect_wall.x=window_width-(window_width-538)/2; //wall.png widht=538
				dstrect_wall.y=0;
				SDL_BlitSurface(wall_side_right, NULL, screen, &dstrect_wall);
				srcrect_wall.x=0;
				srcrect_wall.y=0;
				srcrect_wall.h=391; //line.png height=391 wall.png height=391
				srcrect_wall.w=3; //line.png width=3
				dstrect_wall.x=window_width-(window_width-538)/2; //wall.png width=538
				dstrect_wall.y=(window_height-391)/2;
				SDL_BlitSurface(line, &srcrect_wall, screen, &dstrect_wall);
			}
			else if (labyrinth[player_coordinate+sign*size_labyrinth_length+1*sign] == 1)
			{
				srcrect_wall.x=0;
				srcrect_wall.y=0;
				srcrect_wall.h=391;
				srcrect_wall.w=(window_width-538)/2;
				dstrect_wall.x=window_width-(window_width-538)/2; //wall.png widht=538
				dstrect_wall.y=(window_height-391)/2; //wall.png height=391
				SDL_BlitSurface(wall, &srcrect_wall, screen, &dstrect_wall);
				srcrect_wall.x=0;
				srcrect_wall.y=0;
				srcrect_wall.h=391; //line.png height=391 wall.png height=391
				srcrect_wall.w=3; //line.png width=3
				dstrect_wall.x=window_width-(window_width-538)/2; //wall.png width=538
				dstrect_wall.y=(window_height-391)/2;
				SDL_BlitSurface(line, &srcrect_wall, screen, &dstrect_wall);
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
		SDL_UpdateWindowSurface(main_window);
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
				labyrinth[player_coordinate]=0; //для отладки и контроля через консоль
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
						if ((player[0].direction ==  0) && (player[0].x > 1) && (labyrinth[player_coordinate-size_labyrinth_length] != 1))
							player[0].x=player[0].x-1;
						//break после каждого if ?
						if ((player[0].direction ==  1) && (player[0].y < size_labyrinth_length-2) && (labyrinth[player_coordinate+1] != 1))
							player[0].y=player[0].y+1;
						if ((player[0].direction ==  2) && (player[0].x < size_labyrinth_width-2) && (labyrinth[player_coordinate+size_labyrinth_length] != 1))
							player[0].x=player[0].x+1;
						if ((player[0].direction ==  3) && (player[0].y > 1) && (labyrinth[player_coordinate-1] != 1))
							player[0].y=player[0].y-1;
						break;
					case SDLK_DOWN:
					case SDLK_s:
						if ((player[0].direction ==  0) && (player[0].x < size_labyrinth_width-1) && (labyrinth[player_coordinate+size_labyrinth_length] != 1))
							player[0].x=player[0].x+1;
						if ((player[0].direction ==  1) && (player[0].y > 0) && (labyrinth[player_coordinate-1] != 1)) 
							player[0].y=player[0].y-1;
						if ((player[0].direction ==  2) && (player[0].x > 0) && (labyrinth[player_coordinate-size_labyrinth_length] != 1))
							player[0].x=player[0].x-1;
						if ((player[0].direction ==  3) && (player[0].y < size_labyrinth_length-1) && (labyrinth[player_coordinate+1] != 1))
							player[0].y=player[0].y+1;
						break;
					case SDLK_LEFT:
					case SDLK_a: //если подключить работу мыши - задать на AD шаг влево/вправо вместо поворота
						//~ if (player[0].direction == 0)
							//~ player[0].direction=3;
						//~ break;
						//~ player[0].direction=player[0].direction-1;
						//~ break;						
						if (player[0].direction == 0)
							player[0].direction=3;
						else
							player[0].direction=player[0].direction-1;
						break;
					case SDLK_RIGHT:
					case SDLK_d: //если подключить работу мыши - задать на AD шаг влево/вправо вместо поворота
						if (player[0].direction == 3)
							player[0].direction=0;
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
		//Для отладки и контроля через консоль
		labyrinth[player[0].x*size_labyrinth_length+player[0].y]=4;
		show_labyrinth_in_cmd(player_coordinate, player, size_labyrinth_length, size_labyrinth_width, labyrinth);
		//Если с момента начала обработки кадра прошло меньше времени,
		//чем требуется для заданного значения FPS - обработка приостанавливается на остаток требуемого времени
		if ((SDL_GetTicks()-ticks_begin) < (1000/FPS))
			SDL_Delay((1000/FPS)-(SDL_GetTicks()-ticks_begin));
		count++;
		//char *dir[]={"вверх", "вправо", "вниз", "влево"};
		//printf("\rcell: %i  x: %i  y: %i  dir: %s  ", player_coordinate, player[0].x, player[0].y, dir[player[0].direction]);
	}
	printf("\nКадров: %i Миллисекунд: %i FPS: %.2f\n", count, SDL_GetTicks(), (float)count/SDL_GetTicks()*1000);

	//TODO: перенести в clean_up_sdl()
	SDL_FreeSurface(background);
	SDL_FreeSurface(wall);
	SDL_FreeSurface(wall_side_left);
	SDL_FreeSurface(wall_side_right);
	SDL_FreeSurface(wall_far);
	SDL_FreeSurface(wall_side_far_left);
	SDL_FreeSurface(wall_side_far_right);
	SDL_FreeSurface(line);
	SDL_FreeSurface(empty_background);
	SDL_FreeSurface(empty_far_background);	
/*	IMG_Quit();
	//SDL_DestroyTexture(bitmapTex);
    //SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(main_window);
	SDL_Quit();*/
	clean_up_sdl(main_window);
	return 0;
}

//Корректное завершение работы SDL
void clean_up_sdl(SDL_Window *main_window)
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
	if (main_window != NULL)
		SDL_DestroyWindow(main_window);
	//Uint32 SDL_WasInit(Uint32 flags)
	SDL_Quit();
	return;
}

void show_labyrinth_in_cmd(int const player_coordinate, const struct players player[], int const length, int const width, int const *labyrinth_temp)
{
	for (int j = 0; j < width; j++)
	{
		for (int k = 0; k < length; k++)
		{
			//0 - пустая клетка, 1 - стена, 2 - будущая пустая клетка, 3 - один из соперников, 4 - игрок
			if (!labyrinth_temp[(j*length)+k])
				printf("%s", "  ");
			if (labyrinth_temp[(j*length)+k] == 1)
				printf("%s", " X");
			if (labyrinth_temp[(j*length)+k] == 2)
				printf("%s", " .");
			if (labyrinth_temp[(j*length)+k] == 3)
				printf("%s", " p");
			if (labyrinth_temp[(j*length)+k] == 4)
			{
				if (!player[0].direction)
					printf("%s", " ^");
				if (player[0].direction == 1)
					printf("%s", " >");
				if (player[0].direction == 2)
					printf("%s", " v");
				if (player[0].direction == 3)
					printf("%s", " <");
			}
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
