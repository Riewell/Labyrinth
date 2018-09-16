/*  sdl_main_hwsw.c

  Лабиринт
  Version 0.3

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

void show_labyrinth_in_cmd_hw(int const player_coordinate, const struct players player[], short int const length, short int const width, int const *labyrinth_temp); //для отладки и контроля через консоль

void checking_for_events(SDL_Window *main_window, int const coordinate, struct players player[], int *labyrinth, short int const size_labyrinth_length, short int const holes, short int const *holes_array);

int request_for_exit(SDL_Window *main_window, const struct players player[]);

void clean_up_sdl_hw(SDL_Window *main_window, SDL_Renderer *renderer);

int sdl_main_hwsw(int *labyrinth, struct players player[], short int const rivals, short int const size_labyrinth_length, short int const size_labyrinth_width, short int const holes, short int const *holes_array, short int window_height, short int window_width, short int fullscreen, short int refresh_rate, short int fps, short int hw_accel, short int vsync, short int trap_time, short int hole_time, short int speed, short int turn_speed, short int const debug, FILE *settings, char const *settings_filename)
//~ int sdl_main_hwsw(int *labyrinth, struct players player[], short int const size_labyrinth_length, short int const size_labyrinth_width, short int const holes, short int const *holes_array, int requested_mode)
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
    //~ int window_width=640;
    //~ int window_height=480;
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
	//~ if (requested_mode == 1)
	if (!hw_accel && !vsync)
		renderer = SDL_CreateRenderer(main_window, -1, SDL_RENDERER_SOFTWARE);
	//~ else if (requested_mode == 2)
	else if (!hw_accel && vsync)
		renderer = SDL_CreateRenderer(main_window, -1, SDL_RENDERER_SOFTWARE || SDL_RENDERER_PRESENTVSYNC);
	//~ else if (requested_mode == 3)
	else if (hw_accel && !vsync)
		renderer = SDL_CreateRenderer(main_window, -1, SDL_RENDERER_ACCELERATED);
	//~ else if (requested_mode == 4)
	else if (hw_accel && vsync)
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
	SDL_Surface *exit_morning_sf=NULL;
	SDL_Surface *exit_day_sf=NULL;
	SDL_Surface *exit_evening_sf=NULL;
	SDL_Surface *exit_night_sf=NULL;
	
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
	exit_morning_sf=IMG_Load("gfx/exit_morning.png");
	exit_day_sf=IMG_Load("gfx/exit_day.png");
	exit_evening_sf=IMG_Load("gfx/exit_evening.png");
	exit_night_sf=IMG_Load("gfx/exit_night.png");
	//Если не удалось загрузить какие-то файлы - вывести сообщение об ошибке с указанием последнего не загруженного файла
	if (!background_sf || !wall_sf || !wall_side_left_sf || !wall_side_right_sf || !wall_far_sf || !wall_side_far_left_sf || !wall_side_far_right_sf || !line_sf || !empty_background_sf || !empty_far_background_sf || !exit_morning_sf || !exit_day_sf || !exit_evening_sf || !exit_night_sf)
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
	SDL_Texture *exit_morning=NULL;
	SDL_Texture *exit_day=NULL;
	SDL_Texture *exit_evening=NULL;
	SDL_Texture *exit_night=NULL;
	
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
	exit_morning=SDL_CreateTextureFromSurface(renderer, exit_morning_sf);
	exit_day=SDL_CreateTextureFromSurface(renderer, exit_day_sf);
	exit_evening=SDL_CreateTextureFromSurface(renderer, exit_evening_sf);
	exit_night=SDL_CreateTextureFromSurface(renderer, exit_night_sf);
	
	//Если не удалось загрузить какие-то текстуры - вывести сообщение об ошибке
	if (!background || !wall || !wall_side_left || !wall_side_right || !wall_far || !wall_side_far_left || !wall_side_far_right || !line || !empty_background || !empty_far_background || !exit_morning || !exit_day || !exit_evening || !exit_night)
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
		SDL_FreeSurface(exit_morning_sf);
		SDL_FreeSurface(exit_day_sf);
		SDL_FreeSurface(exit_evening_sf);
		SDL_FreeSurface(exit_night_sf);
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
		SDL_DestroyTexture(exit_morning);
		SDL_DestroyTexture(exit_day);
		SDL_DestroyTexture(exit_evening);
		SDL_DestroyTexture(exit_night);
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
	SDL_FreeSurface(exit_morning_sf);
	SDL_FreeSurface(exit_day_sf);
	SDL_FreeSurface(exit_evening_sf);
	SDL_FreeSurface(exit_night_sf);
	
	//Получение значения текущего локального времени для отображения соответствующего варианта изображения выхода из лабиринта
	//(возможно, стоит перенести в основной цикл для поддержания актуальных данных,
	//но есть сомнения относительно производительности и необходимости в подобной возможности)
	time_t local_rawtime=time(NULL);
	struct tm *time_of_day;
	time_of_day=localtime(&local_rawtime);
	//Если что-то с получением времени пойдёт не так - устанавливается значение по умолчанию (день)
	if (time_of_day->tm_hour < 0 || time_of_day->tm_hour > 23)
		time_of_day->tm_hour=12;
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
			if ((player[0].x-sign*1) <= 0 || (player_coordinate-sign*size_labyrinth_length) >= size_labyrinth_width*size_labyrinth_length || labyrinth[player_coordinate-sign*size_labyrinth_length] == WALL)
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
			//Если впереди выход - отрисовка его, в соответствии с текущим локальным временем
			//(точное определение времени суток находится в includes_headers; можно будет перенести в файл конфигурации/настройки)
			//FIXME: определиться с порядком отрисовки!
			if (labyrinth[player_coordinate-sign*size_labyrinth_length] == EXIT)
			{
				srcrect_wall.x=0;
				srcrect_wall.y=0;
				srcrect_wall.h=394; //exit_*.png height=394
				srcrect_wall.w=538; //exit_*.png widht=538
				dstrect_wall.x=(window_width-538)/2;
				dstrect_wall.y=(window_height-394)/2;
				dstrect_wall.h=394;
				dstrect_wall.w=538;
				if (time_of_day->tm_hour >= EVENING && time_of_day->tm_hour < NIGHT)
				{
					SDL_RenderCopy(renderer, exit_evening, &srcrect_wall, &dstrect_wall);
				}
				if (time_of_day->tm_hour >= NIGHT || time_of_day->tm_hour < MORNING) //использовано ИЛИ, поскольку NIGHT > MORNING
				{
					SDL_RenderCopy(renderer, exit_night, &srcrect_wall, &dstrect_wall);
				}
				if (time_of_day->tm_hour >= MORNING && time_of_day->tm_hour < DAY)
				{
					SDL_RenderCopy(renderer, exit_morning, &srcrect_wall, &dstrect_wall);
				}
				if (time_of_day->tm_hour >= DAY && time_of_day->tm_hour < EVENING)
				{
					SDL_RenderCopy(renderer, exit_day, &srcrect_wall, &dstrect_wall);
				}
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
			//Если впереди выход - отрисовка его, в соответствии с текущим локальным временем
			//(точное определение времени суток находится в includes_headers; можно будет перенести в файл конфигурации/настройки)
			//FIXME: определиться с порядком отрисовки!
			if (labyrinth[player_coordinate+1*sign] == EXIT)
			{
				srcrect_wall.x=0;
				srcrect_wall.y=0;
				srcrect_wall.h=394; //exit_*.png height=394
				srcrect_wall.w=538; //exit_*.png widht=538
				dstrect_wall.x=(window_width-538)/2;
				dstrect_wall.y=(window_height-394)/2;
				dstrect_wall.h=394;
				dstrect_wall.w=538;
				if (time_of_day->tm_hour >= EVENING && time_of_day->tm_hour < NIGHT)
				{
					SDL_RenderCopy(renderer, exit_evening, &srcrect_wall, &dstrect_wall);
				}
				if (time_of_day->tm_hour >= NIGHT || time_of_day->tm_hour < MORNING) //использовано ИЛИ, поскольку NIGHT > MORNING
				{
					SDL_RenderCopy(renderer, exit_night, &srcrect_wall, &dstrect_wall);
				}
				if (time_of_day->tm_hour >= MORNING && time_of_day->tm_hour < DAY)
				{
					SDL_RenderCopy(renderer, exit_morning, &srcrect_wall, &dstrect_wall);
				}
				if (time_of_day->tm_hour >= DAY && time_of_day->tm_hour < EVENING)
				{
					SDL_RenderCopy(renderer, exit_day, &srcrect_wall, &dstrect_wall);
				}
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
				//Восстановление значения ячейки в состояние до попадания туда участником
				labyrinth[player_coordinate]=player[0].previous_cell_state;
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
					//производится проверка, не пытается ли игрок пройти в выход из лабиринта - 
					//в этом случае у него будет запрошено подтверждение на выход
					//В противном случае, производится изменение координат игрока в заданную сторону
					//(если игрок не пытается пройти сквозь стены, в том числе крайние)
					//После этого производится проверка на присутствие события в ячейке, в которую попал игрок
					//(с соответствующими действиями - описаны в функции checking_for_events)
					//Если игрок нажал клавиши "влево" или "вправо", или AD - 
					//производится изменение направления, в котором смотрит игрок
					//(0 - вверх, 1 - вправо, 2 - вниз, 3 - влево)
					//При зажимании клавиши Shift клавиши "влево", "вправо", A и D позволяют смещаться в соответствующую сторону
					//(без поворота)
					//Скорость передвижения и поворота игрока контролируется таймером
					//через SDL_GetTicks() и настройки SPEED_MOVE и SPEED_TURN
					case SDLK_UP:
					case SDLK_w:
						if (player[0].in_trap)
						{
							if ( (SDL_GetTicks()-player[0].trap_start) < TIME_IN_TRAP)
								break;
							else
								player[0].in_trap=0;
						}
						if (player[0].in_hole)
						{
							if ( (SDL_GetTicks()-player[0].trap_start) < TIME_BETWEEN_HOLES)
								break;
							else
								player[0].in_hole=0;
						}
						if ( (SDL_GetTicks()-player[0].step_start) < SPEED_MOVE)
							break;
						switch (player[0].direction)
						{
							case UP:
								if (labyrinth[player_coordinate-size_labyrinth_length] == EXIT && request_for_exit(main_window, player) )
								{
									request_for_quit=1;
									break;
								}
								if ((player[0].x > 1) && (labyrinth[player_coordinate-size_labyrinth_length] != WALL))
								{
									player[0].x=player[0].x-1;
									player[0].step_start=SDL_GetTicks();
									//Проверка на события в ячейке, в которую попал участник
									checking_for_events(main_window, player_coordinate-size_labyrinth_length, player, labyrinth, size_labyrinth_length, holes, holes_array);
									break;
								}
								break;
							case RIGHT:
								if (labyrinth[player_coordinate+1] == EXIT && request_for_exit(main_window, player) )
								{
									request_for_quit=1;
									break;
								}
								if ((player[0].y < size_labyrinth_length-2) && (labyrinth[player_coordinate+1] != WALL))
								{
									player[0].y=player[0].y+1;
									player[0].step_start=SDL_GetTicks();
									checking_for_events(main_window, player_coordinate+1, player, labyrinth, size_labyrinth_length, holes, holes_array);
									break;
								}
								break;
							case DOWN:
								if (labyrinth[player_coordinate+size_labyrinth_length] == EXIT && request_for_exit(main_window, player) )
								{
									request_for_quit=1;
									break;
								}
								if ((player[0].x < size_labyrinth_width-2) && (labyrinth[player_coordinate+size_labyrinth_length] != WALL))
								{
									player[0].x=player[0].x+1;
									player[0].step_start=SDL_GetTicks();
									checking_for_events(main_window, player_coordinate+size_labyrinth_length, player, labyrinth, size_labyrinth_length, holes, holes_array);
									break;
								}
								break;
							case LEFT:
								if (labyrinth[player_coordinate-1] == EXIT && request_for_exit(main_window, player) )
								{
									request_for_quit=1;
									break;
								}
								if ((player[0].y > 1) && (labyrinth[player_coordinate-1] != WALL))
								{
									player[0].y=player[0].y-1;
									player[0].step_start=SDL_GetTicks();
									checking_for_events(main_window, player_coordinate-1, player, labyrinth, size_labyrinth_length, holes, holes_array);
									break;
								}
								break;
						}
						break;
					case SDLK_DOWN:
					case SDLK_s:
						if (player[0].in_trap)
						{
							if ( (SDL_GetTicks()-player[0].trap_start) < TIME_IN_TRAP)
								break;
							else
								player[0].in_trap=0;
						}
						if (player[0].in_hole)
						{
							if ( (SDL_GetTicks()-player[0].trap_start) < TIME_BETWEEN_HOLES)
								break;
							else
								player[0].in_hole=0;
						}
						if ( (SDL_GetTicks()-player[0].step_start) < SPEED_MOVE)
							break;
						switch (player[0].direction)
						{
							case UP:
								if (labyrinth[player_coordinate+size_labyrinth_length] == EXIT && request_for_exit(main_window, player) )
								{
									request_for_quit=1;
									break;
								}
								if ((player[0].x < size_labyrinth_width-2) && (labyrinth[player_coordinate+size_labyrinth_length] != WALL))
								{
									player[0].x=player[0].x+1;
									player[0].step_start=SDL_GetTicks();
									checking_for_events(main_window, player_coordinate+size_labyrinth_length, player, labyrinth, size_labyrinth_length, holes, holes_array);
									break;
								}
								break;
							case RIGHT:
								if (labyrinth[player_coordinate-1] == EXIT && request_for_exit(main_window, player) )
								{
									request_for_quit=1;
									break;
								}
								if ((player[0].y > 1) && (labyrinth[player_coordinate-1] != WALL))
								{
									player[0].y=player[0].y-1;
									player[0].step_start=SDL_GetTicks();
									checking_for_events(main_window, player_coordinate-1, player, labyrinth, size_labyrinth_length, holes, holes_array);
									break;
								}
								break;
							case DOWN:
								if (labyrinth[player_coordinate-size_labyrinth_length] == EXIT && request_for_exit(main_window, player) )
								{
									request_for_quit=1;
									break;
								}
								if ((player[0].x > 1) && (labyrinth[player_coordinate-size_labyrinth_length] != WALL))
								{
									player[0].x=player[0].x-1;
									player[0].step_start=SDL_GetTicks();
									checking_for_events(main_window, player_coordinate-size_labyrinth_length, player, labyrinth, size_labyrinth_length, holes, holes_array);
									break;
								}
								break;
							case LEFT:
								if (labyrinth[player_coordinate+1] == EXIT && request_for_exit(main_window, player) )
								{
									request_for_quit=1;
									break;
								}
								if ((player[0].y < size_labyrinth_length-2) && (labyrinth[player_coordinate+1] != WALL))
								{
									player[0].y=player[0].y+1;
									player[0].step_start=SDL_GetTicks();
									checking_for_events(main_window, player_coordinate+1, player, labyrinth, size_labyrinth_length, holes, holes_array);
									break;
								}
								break;
						}
						break;
					case SDLK_LEFT:
					case SDLK_a:
						//~ if (player[0].direction == 0)
							//~ player[0].direction=3;
						//~ break;
						//~ player[0].direction=player[0].direction-1;
						//~ break;
						//При зажимании Shift производится сдвиг влево (без поворота)
						if (SDL_GetModState() && KMOD_SHIFT)
						{
							if (player[0].in_trap)
							{
								if ( (SDL_GetTicks()-player[0].trap_start) < TIME_IN_TRAP)
									break;
								else
									player[0].in_trap=0;
							}
							if (player[0].in_hole)
							{
								if ( (SDL_GetTicks()-player[0].trap_start) < TIME_BETWEEN_HOLES)
									break;
								else
									player[0].in_hole=0;
							}
							if ( (SDL_GetTicks()-player[0].step_start) < SPEED_MOVE)
								break;
							switch (player[0].direction)
							{
								case UP:
									//if (labyrinth[player_coordinate-1] == EXIT && request_for_exit(main_window, player) )
									if (labyrinth[player_coordinate-1] == EXIT)
									{
										//Вынесено в отдельное условие, после которого добавлено обнуление поля mod стуктуры SDL_Keysym
										//В противном случае, при заходе в данное условие, но невыполнении его
										//(при отказе игрока выйти из игры), состояние клавиши Shift сохранялось как "нажато",
										//вне зависимости от её действительного состояния
										if (request_for_exit(main_window, player) )
										{
											request_for_quit=1;
											break;
										}
										SDL_SetModState(KMOD_NONE);
									}
									if ((player[0].y > 1) && (labyrinth[player_coordinate-1] != WALL))
									{
										player[0].y=player[0].y-1;
										player[0].step_start=SDL_GetTicks();
										checking_for_events(main_window, player_coordinate-1, player, labyrinth, size_labyrinth_length, holes, holes_array);
										break;
									}
									break;
								case RIGHT:
									if (labyrinth[player_coordinate-size_labyrinth_length] == EXIT)
									{
										if (request_for_exit(main_window, player) )
										{
											request_for_quit=1;
											break;
										}
										SDL_SetModState(KMOD_NONE);
									}
									if ((player[0].x > 1) && (labyrinth[player_coordinate-size_labyrinth_length] != WALL))
									{
										player[0].x=player[0].x-1;
										player[0].step_start=SDL_GetTicks();
										checking_for_events(main_window, player_coordinate-size_labyrinth_length, player, labyrinth, size_labyrinth_length, holes, holes_array);
										break;
									}
									break;
								case DOWN:
									if (labyrinth[player_coordinate+1] == EXIT)
									{
										if (request_for_exit(main_window, player) )
										{
											request_for_quit=1;
											break;
										}
										SDL_SetModState(KMOD_NONE);
									}
									if ((player[0].y < size_labyrinth_length-2) && (labyrinth[player_coordinate+1] != WALL))
									{
										player[0].y=player[0].y+1;
										player[0].step_start=SDL_GetTicks();
										checking_for_events(main_window, player_coordinate+1, player, labyrinth, size_labyrinth_length, holes, holes_array);
										break;
									}
									break;
								case LEFT:
									if (labyrinth[player_coordinate+size_labyrinth_length] == EXIT)
									{
										if (request_for_exit(main_window, player) )
										{
											request_for_quit=1;
											break;
										}
										SDL_SetModState(KMOD_NONE);
									}
									if ((player[0].x < size_labyrinth_width-2) && (labyrinth[player_coordinate+size_labyrinth_length] != WALL))
									{
										player[0].x=player[0].x+1;
										player[0].step_start=SDL_GetTicks();
										checking_for_events(main_window, player_coordinate+size_labyrinth_length, player, labyrinth, size_labyrinth_length, holes, holes_array);
										break;
									}
									break;
							}
							break;
						}
						if ( (SDL_GetTicks()-player[0].step_start) < SPEED_TURN)
							break;
						if (player[0].direction == UP)
							player[0].direction=LEFT;
						else
							player[0].direction=player[0].direction-1;
						player[0].step_start=SDL_GetTicks();
						break;
					case SDLK_RIGHT:
					case SDLK_d:
						//При зажимании Shift производится сдвиг вправо (без поворота)
						if (SDL_GetModState() && KMOD_SHIFT)
						{
							if (player[0].in_trap)
							{
								if ( (SDL_GetTicks()-player[0].trap_start) < TIME_IN_TRAP)
									break;
								else
									player[0].in_trap=0;
							}
							if (player[0].in_hole)
							{
								if ( (SDL_GetTicks()-player[0].trap_start) < TIME_BETWEEN_HOLES)
									break;
								else
									player[0].in_hole=0;
							}
							if ( (SDL_GetTicks()-player[0].step_start) < SPEED_MOVE)
								break;
							switch (player[0].direction)
							{
								case UP:
									if (labyrinth[player_coordinate+1] == EXIT)
									{
										if (request_for_exit(main_window, player) )
										{
											request_for_quit=1;
											break;
										}
										SDL_SetModState(KMOD_NONE);
									}
									if ((player[0].y < size_labyrinth_length-2) && (labyrinth[player_coordinate+1] != WALL))
									{
										player[0].y=player[0].y+1;
										player[0].step_start=SDL_GetTicks();
										checking_for_events(main_window, player_coordinate+1, player, labyrinth, size_labyrinth_length, holes, holes_array);
										break;
									}
									break;
								case RIGHT:
									if (labyrinth[player_coordinate+size_labyrinth_length] == EXIT)
									{
										if (request_for_exit(main_window, player) )
										{
											request_for_quit=1;
											break;
										}
										SDL_SetModState(KMOD_NONE);
									}
									if ((player[0].x < size_labyrinth_width-2) && (labyrinth[player_coordinate+size_labyrinth_length] != WALL))
									{
										player[0].x=player[0].x+1;
										player[0].step_start=SDL_GetTicks();
										checking_for_events(main_window, player_coordinate+size_labyrinth_length, player, labyrinth, size_labyrinth_length, holes, holes_array);
										break;
									}
									break;
								case DOWN:
									if (labyrinth[player_coordinate-1] == EXIT)
									{
										if (request_for_exit(main_window, player) )
										{
											request_for_quit=1;
											break;
										}
										SDL_SetModState(KMOD_NONE);
									}
									if ((player[0].y > 1) && (labyrinth[player_coordinate-1] != WALL))
									{
										player[0].y=player[0].y-1;
										player[0].step_start=SDL_GetTicks();
										checking_for_events(main_window, player_coordinate-1, player, labyrinth, size_labyrinth_length, holes, holes_array);
										break;
									}
									break;
								case LEFT:
									if (labyrinth[player_coordinate-size_labyrinth_length] == EXIT)
									{
										if (request_for_exit(main_window, player) )
										{
											request_for_quit=1;
											break;
										}
										SDL_SetModState(KMOD_NONE);
									}
									if ((player[0].x > 1) && (labyrinth[player_coordinate-size_labyrinth_length] != WALL))
									{
										player[0].x=player[0].x-1;
										player[0].step_start=SDL_GetTicks();
										checking_for_events(main_window, player_coordinate-size_labyrinth_length, player, labyrinth, size_labyrinth_length, holes, holes_array);
										break;
									}
									break;
							}
							break;
						}
						if ( (SDL_GetTicks()-player[0].step_start) < SPEED_TURN)
							break;
						if (player[0].direction == LEFT)
							player[0].direction=UP;
						else
							player[0].direction=player[0].direction+1;
						player[0].step_start=SDL_GetTicks();
						break;
				}
			}
			if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
					//Простая пауза по нажатию клавиши "p" (лат.)
					case SDLK_p:
						do
						{
							SDL_WaitEvent(&event);
							//Пауза не блокирует возможность выхода из приложения
							//(как по нажатию клавиши "ESC", так и путём закрытия окна приложения
							if (event.type == SDL_QUIT || event.key.keysym.sym == SDLK_ESCAPE)
							{
								request_for_quit=1;
								break;
							}
						}
						while (event.type != SDL_KEYDOWN || event.key.keysym.sym != SDLK_p);
				}
			}
		}
		//Предыдущее состояние клетки сохраняется для последующего восстановления
		if (labyrinth[player[0].x*size_labyrinth_length+player[0].y] != PLAYER)
		{
			player[0].previous_cell_state=labyrinth[player[0].x*size_labyrinth_length+player[0].y];
			labyrinth[player[0].x*size_labyrinth_length+player[0].y]=PLAYER;
		}
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
	//~ char *mode[]={"software", "software vsync", "hardware", "hardware vsync"};
	//~ printf("\nКадров: %i Миллисекунд: %i FPS: %.2f mode: %s\n", count, SDL_GetTicks(), (float)count/SDL_GetTicks()*1000, mode[requested_mode-1]);
	printf("\nКадров: %i Миллисекунд: %i FPS: %.2f hardware: %hi vsync: %hi\n", count, SDL_GetTicks(), (float)count/SDL_GetTicks()*1000, hw_accel, vsync);

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
	SDL_DestroyTexture(exit_morning);
	SDL_DestroyTexture(exit_day);
	SDL_DestroyTexture(exit_evening);
	SDL_DestroyTexture(exit_night);
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

void show_labyrinth_in_cmd_hw(int const player_coordinate, const struct players player[], short int const length, short int const width, int const *labyrinth_temp)
{
	for (int j = 0; j < width; j++)
	{
		for (int k = 0; k < length; k++)
		{
			//0 - пустая клетка, 1 - стена, 2 - будущая пустая клетка, 3 - один из соперников, 4 - игрок, 5 - выход
			//6 - клад, 7 - ложный клад, 8 - дыра, 9 - ловушка, 10 - госпиталь, 11 - арсенал
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
			if (labyrinth_temp[(j*length)+k] == TREASURE)
				printf("%s", " T");
			if (labyrinth_temp[(j*length)+k] == FAKE_TREASURE)
				printf("%s", " F");
			if (labyrinth_temp[(j*length)+k] == HOLE)
				printf("%s", " O");
			if (labyrinth_temp[(j*length)+k] == TRAP)
				printf("%s", " #");
			if (labyrinth_temp[(j*length)+k] == HOSPITAL)
				printf("%s", " H");
			if (labyrinth_temp[(j*length)+k] == ARSENAL)
				printf("%s", " A");
		}
		if (j == 0)
			printf("  %s%i", "cell:", player_coordinate);
		if (j == 1)
			printf("  %s%i %s%i", "x=", player[0].x, "y=", player[0].y);
		if (j == 2)
		{
			char *dir[]={"вверх", "вправо", "вниз", "влево"};
			printf("  %s%s", "dir:", dir[player[0].direction]);
		}
		if (j == 3)
			printf("  %s", "T F W @ H # (h)");
		if (j == 4)
			printf("  %i %i %i %i %i %i %li", player[0].has_treasure, player[0].has_fake_treasure, player[0].has_weapon, player[0].is_wounded, player[0].in_hole, player[0].in_trap, TIME_IN_TRAP-(SDL_GetTicks()-player[0].trap_start));
		printf("%s", "\n");
	}
	printf("%s", "\n");
	return;
}
