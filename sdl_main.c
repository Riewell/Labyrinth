/*  sdl_main.c

  Лабиринт
  Version 0.2.4

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

void checking_for_events(SDL_Window *main_window, SDL_Surface *screen, int const coordinate, struct players player[], int *labyrinth, int const size_labyrinth_length, int const holes, int const *holes_array);
int request_for_exit(SDL_Window *main_window, const struct players player[]);
void clean_up_sdl(SDL_Window *main_window);

int sdl_main(int *labyrinth, struct players player[], int const size_labyrinth_length, int const size_labyrinth_width, int const holes, int const *holes_array)
{
	//Запрос на инициализацию поддержки видео и аудио
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) < 0)
    {
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return 1;
    }
    //Создание основного окна
    SDL_Window *main_window=NULL;
    //Получение значений текущего режима экрана (дисплея) №0
    SDL_DisplayMode desktop_display0_mode;
    SDL_GetDesktopDisplayMode(0, &desktop_display0_mode);
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
	SDL_Surface *background_=NULL;
	SDL_Surface *wall_=NULL;
	SDL_Surface *wall_side_left_=NULL;
	SDL_Surface *wall_side_right_=NULL;
	SDL_Surface *wall_far_=NULL;
	SDL_Surface *wall_side_far_left_=NULL;
	SDL_Surface *wall_side_far_right_=NULL;
	SDL_Surface *line_=NULL;
	SDL_Surface *empty_background_=NULL;
	SDL_Surface *empty_far_background_=NULL;
	SDL_Surface *arsenal_=NULL;
	SDL_Surface *hole_=NULL;
	SDL_Surface *hole_ceiling_=NULL;
	SDL_Surface *chest_=NULL;
	SDL_Surface *grate_=NULL;
	SDL_Surface *hospital_=NULL;
	SDL_Surface *hospital_near_=NULL;
	SDL_Surface *exit_morning_=NULL;
	SDL_Surface *exit_day_=NULL;
	SDL_Surface *exit_evening_=NULL;
	SDL_Surface *exit_night_=NULL;
	
	SDL_Surface *black_rect=NULL; //для эффекта плавного затухания экрана
	
	background_=IMG_Load("gfx/background.png");
	wall_=IMG_Load("gfx/wall.png");
	wall_side_left_=IMG_Load("gfx/wall_side_left.png");
	wall_side_right_=IMG_Load("gfx/wall_side_right.png");
	wall_far_=IMG_Load("gfx/wall_far.png");
	wall_side_far_left_=IMG_Load("gfx/wall_side_far_left.png");
	wall_side_far_right_=IMG_Load("gfx/wall_side_far_right.png");
	line_=IMG_Load("gfx/line.png");
	empty_background_=IMG_Load("gfx/empty_background.png");
	empty_far_background_=IMG_Load("gfx/empty_far_background.png");
	arsenal_=IMG_Load("gfx/arsenal_boxes.png");
	hole_=IMG_Load("gfx/big_hole_in_floor.png");
	hole_ceiling_=IMG_Load("gfx/big_holes_in_ceiling.png");
	chest_=IMG_Load("gfx/chest.png");
	grate_=IMG_Load("gfx/grate.png");
	hospital_=IMG_Load("gfx/hospital_far.png");
	hospital_near_=IMG_Load("gfx/hospital_near.png");
	exit_morning_=IMG_Load("gfx/exit_morning.png");
	exit_day_=IMG_Load("gfx/exit_day.png");
	exit_evening_=IMG_Load("gfx/exit_evening.png");
	exit_night_=IMG_Load("gfx/exit_night.png");
	//Если не удалось загрузить какие-то файлы - вывести сообщение об ошибке с указанием последнего не загруженного файла
	if (!background_ || !wall_ || !wall_side_left_ || !wall_side_right_ || !wall_far_ || !wall_side_far_left_ || !wall_side_far_right_ || !line_ || !empty_background_ || !empty_far_background_ || !arsenal_ || !hole_ || !hole_ceiling_ || !chest_ || !grate_ || !hospital_ || !hospital_near_ || !exit_morning_ || !exit_day_ || !exit_evening_ || !exit_night_)
	{
		printf("IMG_Load: %s\n", IMG_GetError());
		clean_up_sdl(main_window);
		return 1;
	}
	//Создание и настройка режима смешивания поверхности чёрного квадрата (для эффекта плавного затухания экрана)
	if ( (black_rect=SDL_CreateRGBSurface(0, window_width, window_height, 32, 0, 0, 0, 0)) == NULL)
	{
		SDL_Log("SDL_CreateRGBSurface(black_rect) failed: %s", SDL_GetError());
	}
	else
		SDL_SetSurfaceBlendMode(black_rect, SDL_BLENDMODE_BLEND);
	
	//Оптимизация загруженных изображений
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
	SDL_Surface *arsenal=NULL;
	SDL_Surface *hole=NULL;
	SDL_Surface *hole_ceiling=NULL;
	SDL_Surface *chest=NULL;
	SDL_Surface *grate=NULL;
	SDL_Surface *hospital=NULL;
	SDL_Surface *hospital_near=NULL;
	SDL_Surface *exit_morning=NULL;
	SDL_Surface *exit_day=NULL;
	SDL_Surface *exit_evening=NULL;
	SDL_Surface *exit_night=NULL;

	//background=SDL_ConvertSurface(background_, screen->format, 0);
	background=SDL_ConvertSurface(background_, (*screen).format, 0);
	wall=SDL_ConvertSurface(wall_, (*screen).format, 0);
	wall_side_left=SDL_ConvertSurface(wall_side_left_, (*screen).format, 0);
	wall_side_right=SDL_ConvertSurface(wall_side_right_, (*screen).format, 0);
	wall_far=SDL_ConvertSurface(wall_far_, (*screen).format, 0);
	wall_side_far_left=SDL_ConvertSurface(wall_side_far_left_, (*screen).format, 0);
	wall_side_far_right=SDL_ConvertSurface(wall_side_far_right_, (*screen).format, 0);
	line=SDL_ConvertSurface(line_, (*screen).format, 0);
	empty_background=SDL_ConvertSurface(empty_background_, (*screen).format, 0);
	empty_far_background=SDL_ConvertSurface(empty_far_background_, (*screen).format, 0);
	arsenal=SDL_ConvertSurface(arsenal_, (*screen).format, 0);
	hole=SDL_ConvertSurface(hole_, (*screen).format, 0);
	hole_ceiling=SDL_ConvertSurface(hole_ceiling_, (*screen).format, 0);
	chest=SDL_ConvertSurface(chest_, (*screen).format, 0);
	grate=SDL_ConvertSurface(grate_, (*screen).format, 0);
	hospital=SDL_ConvertSurface(hospital_, (*screen).format, 0);
	hospital_near=SDL_ConvertSurface(hospital_near_, (*screen).format, 0);
	exit_morning=SDL_ConvertSurface(exit_morning_, (*screen).format, 0);
	exit_day=SDL_ConvertSurface(exit_day_, (*screen).format, 0);
	exit_evening=SDL_ConvertSurface(exit_evening_, (*screen).format, 0);
	exit_night=SDL_ConvertSurface(exit_night_, (*screen).format, 0);
	//Если не удалось оптимизировать какие-то изображения - вывести сообщение об ошибке с указанием последнего неудавшегося преобразования
	if (!background || !wall || !wall_side_left || !wall_side_right || !wall_far || !wall_side_far_left || !wall_side_far_right || !line || !empty_background || !empty_far_background || !arsenal || !hole || !hole_ceiling || !chest || !grate || !hospital || !hospital_near || !exit_morning || !exit_day || !exit_evening || !exit_night)
	{
		printf("IMG_Load: %s\n", IMG_GetError());
		clean_up_sdl(main_window);
		return 1;
	}
	//Выгрузка неоптимизированных изображений
	SDL_FreeSurface(background_);
	SDL_FreeSurface(wall_);
	SDL_FreeSurface(wall_side_left_);
	SDL_FreeSurface(wall_side_right_);
	SDL_FreeSurface(wall_far_);
	SDL_FreeSurface(wall_side_far_left_);
	SDL_FreeSurface(wall_side_far_right_);
	SDL_FreeSurface(line_);
	SDL_FreeSurface(empty_background_);
	SDL_FreeSurface(empty_far_background_);
	SDL_FreeSurface(arsenal_);
	SDL_FreeSurface(hole_);
	SDL_FreeSurface(hole_ceiling_);
	SDL_FreeSurface(chest_);
	SDL_FreeSurface(grate_);
	SDL_FreeSurface(hospital_);
	SDL_FreeSurface(hospital_near_);
	SDL_FreeSurface(exit_morning_);
	SDL_FreeSurface(exit_day_);
	SDL_FreeSurface(exit_evening_);
	SDL_FreeSurface(exit_night_);
	
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
		//Если игрок попал в дыру (и поверхность чёрного квадрата корректно инициализировалась) -
		//происходит затухание экрана на время, равное половине времени,
		//требуемого для перемещения между соседними (связанными) дырами (TIME_BETWEEN_HOLES)
		//Новое положение игрока в это время не отрисовывается
		if (player[0].in_hole && (ticks_begin-player[0].trap_start < TIME_BETWEEN_HOLES/2) && black_rect)
		{
			SDL_SetSurfaceAlphaMod(black_rect, (SDL_GetTicks()-player[0].trap_start)*255/(TIME_BETWEEN_HOLES/2));
			printf("%i\n", (int)(SDL_GetTicks()-player[0].trap_start)*255/(TIME_BETWEEN_HOLES/2));
			SDL_BlitSurface(black_rect, NULL, screen, NULL);
			SDL_UpdateWindowSurface(main_window);
		}
		//Если игрок не в дыре, или прошло больше половины времени,
		//требуемого для перемещения между соседними (связанными) дырами (TIME_BETWEEN_HOLES) -
		//отрисовка и обработка событий от игрока продолжаются в обычном режиме
		else
		{
			//Подготовка кадра (расположение изображений в нужном порядке по соответствующим координатам)
			/*bitmapTex = SDL_CreateTextureFromSurface(renderer, wall);
			SDL_RenderClear(renderer);
			SDL_RenderCopy(renderer, bitmapTex, NULL, NULL);
			SDL_RenderPresent(renderer);*/
			SDL_Rect srcrect_wall, dstrect_wall;
			//Отрисовка фона
			SDL_BlitSurface(background, NULL, screen, NULL);
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
					if (labyrinth[player_coordinate-sign*size_labyrinth_length*2] == WALL)
					{
						dstrect_wall.x=(window_width-324)/2; //wall_far width=324
						dstrect_wall.y=(window_height-211)/2; //wall_far height=211
						SDL_BlitSurface(wall_far, NULL, screen, &dstrect_wall);
					}
					//Если присутствует дальняя боковая левая стена - отрисовка её, иначе - отрисовка дальней левой стены (не боковой)
					//и событий, если такие есть (кроме событий Клад и Ложный клад) 
					if (labyrinth[player_coordinate-sign*size_labyrinth_length-1*sign] == WALL)
					{
						dstrect_wall.x=(window_width-324)/2-109; //wall_far width=324 wall_side_far_left width=109
						dstrect_wall.y=(window_height-391)/2; //wall_side_far_left height=391
						SDL_BlitSurface(wall_side_far_left, NULL, screen, &dstrect_wall);
					}
					else if (labyrinth[player_coordinate-sign*size_labyrinth_length*2-1*sign] == WALL)
					{
						srcrect_wall.x=324-(window_width-324)/2;
						srcrect_wall.y=0;
						srcrect_wall.h=211;
						srcrect_wall.w=(window_width-324)/2;
						dstrect_wall.x=0;
						dstrect_wall.y=(window_height-211)/2;
						SDL_BlitSurface(wall_far, &srcrect_wall, screen, &dstrect_wall);
						if (labyrinth[player_coordinate-sign*size_labyrinth_length*2-1*sign] != CELL)
						{
							switch (labyrinth[player_coordinate-sign*size_labyrinth_length*2-1*sign])
							{
								case ARSENAL:
									SDL_BlitSurface(arsenal, &srcrect_wall, screen, &dstrect_wall);
									break;
								case HOLE:
									SDL_BlitSurface(hole, &srcrect_wall, screen, &dstrect_wall);
									break;
								case HOSPITAL:
									SDL_BlitSurface(hospital, &srcrect_wall, screen, &dstrect_wall);
									break;
								default:
									break;
							}
						}
					}
					//Если присутствует дальняя боковая правая стена - отрисовка её, иначе - отрисовка дальней правой стены (не боковой)
					if (labyrinth[player_coordinate-sign*size_labyrinth_length+1*sign] == WALL)
					{
						//(window_width-324)/2+324=(window_width-324)/2+648/2=(window_width-324+648)/2=(window_width+324)/2
						dstrect_wall.x=(window_width+324)/2; //wall_far width=324
						dstrect_wall.y=(window_height-393)/2; //wall_side_far_left height=393
						SDL_BlitSurface(wall_side_far_right, NULL, screen, &dstrect_wall);
					}
					else if (labyrinth[player_coordinate-sign*size_labyrinth_length*2+1*sign] == WALL)
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
				if (labyrinth[player_coordinate-1*sign] == WALL)
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
				else if (labyrinth[player_coordinate-sign*size_labyrinth_length-1*sign] == WALL)
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
				if (labyrinth[player_coordinate+1*sign] == WALL)
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
				else if (labyrinth[player_coordinate-sign*size_labyrinth_length+1*sign] == WALL)
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
				//Если впереди выход - отрисовка его, в соответствии с текущим локальным временем
				//(точное определение времени суток находится в includes_headers; можно будет перенести в файл конфигурации/настройки)
				//FIXME: определиться с порядком отрисовки!
				if (labyrinth[player_coordinate-sign*size_labyrinth_length] == EXIT)
				{
					if (time_of_day->tm_hour >= EVENING && time_of_day->tm_hour < NIGHT)
					{
						dstrect_wall.x=(window_width-538)/2; //exit_*.png widht=538
						dstrect_wall.y=(window_height-394)/2; //exit_*.png height=394
						SDL_BlitSurface(exit_evening, NULL, screen, &dstrect_wall);
					}
					if (time_of_day->tm_hour >= NIGHT || time_of_day->tm_hour < MORNING)
					{
						dstrect_wall.x=(window_width-538)/2; //exit_*.png widht=538
						dstrect_wall.y=(window_height-394)/2; //exit_*.png height=394
						SDL_BlitSurface(exit_night, NULL, screen, &dstrect_wall);
					}
					if (time_of_day->tm_hour >= MORNING && time_of_day->tm_hour < DAY)
					{
						dstrect_wall.x=(window_width-538)/2; //exit_*.png widht=538
						dstrect_wall.y=(window_height-394)/2; //exit_*.png height=394
						SDL_BlitSurface(exit_morning, NULL, screen, &dstrect_wall);
					}
					if (time_of_day->tm_hour >= DAY && time_of_day->tm_hour < EVENING)
					{
						dstrect_wall.x=(window_width-538)/2; //exit_*.png widht=538
						dstrect_wall.y=(window_height-394)/2; //exit_*.png height=394
						SDL_BlitSurface(exit_day, NULL, screen, &dstrect_wall);
					}
				}
				if ((labyrinth[player_coordinate-sign*size_labyrinth_length] == TREASURE) || (labyrinth[player_coordinate-sign*size_labyrinth_length] == FAKE_TREASURE))
				{
					dstrect_wall.x=(window_width-302)/2; //big_hole_in_floor.png width=302
					dstrect_wall.y=(window_height+211)/2; //wall.png height=391 wall_far.png height=211
					SDL_BlitSurface(chest, NULL, screen, &dstrect_wall);
				}
				if (labyrinth[player_coordinate-sign*size_labyrinth_length] == HOLE)
				{
					dstrect_wall.x=(window_width-302)/2; //big_hole_in_floor.png width=302
					dstrect_wall.y=(window_height+211)/2; //wall.png height=391 wall_far.png height=211
					//dstrect_wall.h=391;
					//dstrect_wall.w=640;
					SDL_BlitSurface(hole, NULL, screen, &dstrect_wall);
				}
				if (labyrinth[player_coordinate-sign*size_labyrinth_length] == HOSPITAL)
				{
					dstrect_wall.x=(window_width-489)/2; //hospital_far.png width=302
					dstrect_wall.y=window_height/2; //временное решение
					SDL_BlitSurface(hospital, NULL, screen, &dstrect_wall);
				}
				if (labyrinth[player_coordinate-sign*size_labyrinth_length] == ARSENAL)
				{
					dstrect_wall.x=(window_width-478)/2; //arsenal_boxes.png width=302
					dstrect_wall.y=(window_height-(window_height-391)); //временное решение; wall.png height=391
					SDL_BlitSurface(arsenal, NULL, screen, &dstrect_wall);
				}
			}
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
				if ((player[0].y+1*sign) <= 0 || (player[0].y+1*sign) >= size_labyrinth_length || labyrinth[player_coordinate+1*sign] == WALL)
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
					if (labyrinth[player_coordinate+2*sign] == WALL)
					{
						dstrect_wall.x=(window_width-324)/2; //wall_far width=324
						dstrect_wall.y=(window_height-211)/2; //wall_far height=211
						SDL_BlitSurface(wall_far, NULL, screen, &dstrect_wall);
					}
					//Если присутствует дальняя боковая левая стена - отрисовка её, иначе - отрисовка дальней левой стены (не боковой)
					if (labyrinth[player_coordinate-sign*size_labyrinth_length+1*sign] == WALL)
					{
						dstrect_wall.x=(window_width-324)/2-109; //wall_far width=324 wall_side_far_left width=109
						dstrect_wall.y=(window_height-391)/2; //wall_side_far_left height=391
						SDL_BlitSurface(wall_side_far_left, NULL, screen, &dstrect_wall);
					}
					else if (labyrinth[player_coordinate-sign*size_labyrinth_length+2*sign] == WALL)
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
					if (labyrinth[player_coordinate+sign*size_labyrinth_length+1*sign] == WALL)
					{
						//(window_width-324)/2+324=(window_width-324)/2+648/2=(window_width-324+648)/2=(window_width+324)/2
						dstrect_wall.x=(window_width+324)/2; //wall_far width=324
						dstrect_wall.y=(window_height-393)/2; //wall_side_far_left height=393
						SDL_BlitSurface(wall_side_far_right, NULL, screen, &dstrect_wall);
					}
					else if (labyrinth[player_coordinate+sign*size_labyrinth_length+2*sign] == WALL)
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
				if (labyrinth[player_coordinate-size_labyrinth_length*sign] == WALL)
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
				else if (labyrinth[player_coordinate-sign*size_labyrinth_length+1*sign] == WALL)
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
				if (labyrinth[player_coordinate+size_labyrinth_length*sign] == WALL)
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
				else if (labyrinth[player_coordinate+sign*size_labyrinth_length+1*sign] == WALL)
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
				//Если впереди выход - отрисовка его, в соответствии с текущим локальным временем
				//(точное определение времени суток находится в includes_headers; можно будет перенести в файл конфигурации/настройки)
				//FIXME: определиться с порядком отрисовки!
				if (labyrinth[player_coordinate+1*sign] == EXIT)
				{
					if (time_of_day->tm_hour >= EVENING && time_of_day->tm_hour < NIGHT)
					{
						dstrect_wall.x=(window_width-538)/2; //exit_*.png widht=538
						dstrect_wall.y=(window_height-394)/2; //exit_*.png height=394
						SDL_BlitSurface(exit_evening, NULL, screen, &dstrect_wall);
					}
					if (time_of_day->tm_hour >= NIGHT || time_of_day->tm_hour < MORNING) //использовано ИЛИ, поскольку NIGHT > MORNING
					{
						dstrect_wall.x=(window_width-538)/2; //exit_*.png widht=538
						dstrect_wall.y=(window_height-394)/2; //exit_*.png height=394
						SDL_BlitSurface(exit_night, NULL, screen, &dstrect_wall);
					}
					if (time_of_day->tm_hour >= MORNING && time_of_day->tm_hour < DAY)
					{
						dstrect_wall.x=(window_width-538)/2; //exit_*.png widht=538
						dstrect_wall.y=(window_height-394)/2; //exit_*.png height=394
						SDL_BlitSurface(exit_morning, NULL, screen, &dstrect_wall);
					}
					if (time_of_day->tm_hour >= DAY && time_of_day->tm_hour < EVENING)
					{
						dstrect_wall.x=(window_width-538)/2; //exit_*.png widht=538
						dstrect_wall.y=(window_height-394)/2; //exit_*.png height=394
						SDL_BlitSurface(exit_day, NULL, screen, &dstrect_wall);
					}
				}
			}
			//Если игрок попал в дыру (и поверхность чёрного квадрата корректно инициализировалась) -
			//происходит постепенное проявление экрана на время, равное половине времени,
			//требуемого для перемещения между соседними (связанными) дырами (TIME_BETWEEN_HOLES)
			//в новом местоположении игрока
			if (player[0].in_hole && (ticks_begin-player[0].trap_start > TIME_BETWEEN_HOLES/2) && (ticks_begin-player[0].trap_start < TIME_BETWEEN_HOLES) && black_rect)
			{
				SDL_SetSurfaceAlphaMod(black_rect, ((SDL_GetTicks()-player[0].trap_start)*255/(TIME_BETWEEN_HOLES/2)*(-1)+255));
				printf("%i\n", (int)((SDL_GetTicks()-player[0].trap_start)*255/(TIME_BETWEEN_HOLES/2)*(-1)+255));
				SDL_BlitSurface(black_rect, NULL, screen, NULL);
			}
			if (player[0].in_trap && (ticks_begin-player[0].trap_start < TIME_IN_TRAP))
			{
				dstrect_wall.x=0; //big_holes_in_ceiling.png width=640
				dstrect_wall.y=(window_height-391)/2-12; //big_holes_in_ceiling.png height=12 wall.png height=391
				SDL_BlitSurface(hole_ceiling, NULL, screen, &dstrect_wall);
				srcrect_wall.x=0;
				srcrect_wall.h=400;
				srcrect_wall.w=640;
				if (ticks_begin-player[0].trap_start < 1000) //1 секунда на опускание/поднимание решётки
				{
					srcrect_wall.y=400-((SDL_GetTicks()-player[0].trap_start)*400/1000); //grate.png height=400
				}
				else if (ticks_begin-player[0].trap_start > TIME_IN_TRAP-1000) //1 секунда на опускание/поднимание решётки
				{
					srcrect_wall.y=((SDL_GetTicks()-player[0].trap_start)*400/1000)-400*2;
				}
				else
					srcrect_wall.y=0;
				SDL_BlitSurface(grate, &srcrect_wall, screen, &dstrect_wall);
				//SDL_UpdateWindowSurface(main_window);
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
				//Если произошёл вызов закрытия программы по причине закрытия окна программы -
				//производится завершение работы программы
				if (event.type == SDL_QUIT)
					request_for_quit=1;
				//Если игрок нажал ESC - производится завершение работы программы
				if (event.type == SDL_KEYDOWN)
				{
					switch (event.key.keysym.sym)
					{
						case SDLK_ESCAPE:
							request_for_quit=1;
							break;
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
				//TODO: на будущее - подчистить код
				/*if (player[0].in_trap)
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
					break;*/
				if (event.type == SDL_KEYDOWN && !request_for_quit)
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
						//Выход по нажатию ESC перенесён выше
						/*case SDLK_ESCAPE:
							request_for_quit=1;
							break;*/
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
										checking_for_events(main_window, screen, player_coordinate-size_labyrinth_length, player, labyrinth, size_labyrinth_length, holes, holes_array);
										/*switch (labyrinth[player_coordinate-size_labyrinth_length])
										{
											case TREASURE:
												player[0].has_treasure=1;
												labyrinth[player_coordinate-size_labyrinth_length]=CELL;
												break;
											case FAKE_TREASURE:
												player[0].has_fake_treasure=1;
												labyrinth[player_coordinate-size_labyrinth_length]=CELL;
												break;
											case HOLE:
												player[0].in_hole=1;
												break;
											case TRAP:
												player[0].in_trap=1;
												break;
											case HOSPITAL:
												if (player[0].is_wounded)
													player[0].is_wounded=0;
												break;
											case ARSENAL:
												player[0].has_weapon=1;
												break;
											default:
												break;
										}*/
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
										checking_for_events(main_window, screen, player_coordinate+1, player, labyrinth, size_labyrinth_length, holes, holes_array);
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
										checking_for_events(main_window, screen, player_coordinate+size_labyrinth_length, player, labyrinth, size_labyrinth_length, holes, holes_array);
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
										checking_for_events(main_window, screen, player_coordinate-1, player, labyrinth, size_labyrinth_length, holes, holes_array);
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
										checking_for_events(main_window, screen, player_coordinate+size_labyrinth_length, player, labyrinth, size_labyrinth_length, holes, holes_array);
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
										checking_for_events(main_window, screen, player_coordinate-1, player, labyrinth, size_labyrinth_length, holes, holes_array);
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
										checking_for_events(main_window, screen, player_coordinate-size_labyrinth_length, player, labyrinth, size_labyrinth_length, holes, holes_array);
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
										checking_for_events(main_window, screen, player_coordinate+1, player, labyrinth, size_labyrinth_length, holes, holes_array);
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
											checking_for_events(main_window, screen, player_coordinate-1, player, labyrinth, size_labyrinth_length, holes, holes_array);
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
											checking_for_events(main_window, screen, player_coordinate-size_labyrinth_length, player, labyrinth, size_labyrinth_length, holes, holes_array);
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
											checking_for_events(main_window, screen, player_coordinate+1, player, labyrinth, size_labyrinth_length, holes, holes_array);
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
											checking_for_events(main_window, screen, player_coordinate+size_labyrinth_length, player, labyrinth, size_labyrinth_length, holes, holes_array);
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
											checking_for_events(main_window, screen, player_coordinate+1, player, labyrinth, size_labyrinth_length, holes, holes_array);
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
											checking_for_events(main_window, screen, player_coordinate+size_labyrinth_length, player, labyrinth, size_labyrinth_length, holes, holes_array);
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
											checking_for_events(main_window, screen, player_coordinate-1, player, labyrinth, size_labyrinth_length, holes, holes_array);
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
											checking_for_events(main_window, screen, player_coordinate-size_labyrinth_length, player, labyrinth, size_labyrinth_length, holes, holes_array);
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
			}
			//Предыдущее состояние клетки сохраняется для последующего восстановления
			if (labyrinth[player[0].x*size_labyrinth_length+player[0].y] != PLAYER)
			{
				player[0].previous_cell_state=labyrinth[player[0].x*size_labyrinth_length+player[0].y];
				labyrinth[player[0].x*size_labyrinth_length+player[0].y]=PLAYER;
			}
		}
		//Для отладки и контроля через консоль
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
	SDL_FreeSurface(exit_morning);
	SDL_FreeSurface(exit_day);
	SDL_FreeSurface(exit_evening);
	SDL_FreeSurface(exit_night);
	SDL_FreeSurface(black_rect);
/*	IMG_Quit();
	//SDL_DestroyTexture(bitmapTex);
    //SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(main_window);
	SDL_Quit();*/
	clean_up_sdl(main_window);
	return 0;
}

int request_for_exit(SDL_Window *main_window, const struct players player[])
{
	//TODO: Создать собственную реализацию для вывода кириллицы
    SDL_MessageBoxButtonData buttons_exit[] = { { 0, 0, "Yes" }, { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 1, "No" }, };
	SDL_MessageBoxData message_box_exit={SDL_MESSAGEBOX_WARNING , main_window, "", "Quit labyrinth? It will end game.", 2, buttons_exit, NULL};
	SDL_MessageBoxData message_box_treasure={SDL_MESSAGEBOX_WARNING , main_window, "", "You have not found treasure. Quit anyway?", 2, buttons_exit, NULL};
	SDL_MessageBoxButtonData buttons_fake_treasure[] = { { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 0, "Yes" }, { 0, 1, "No" }, };
	SDL_MessageBoxData message_box_fake_treasure={SDL_MESSAGEBOX_WARNING , main_window, "", "You have found fake treasure. Want to return?", 2, buttons_fake_treasure, NULL};
	int button_id;
	if (SDL_ShowMessageBox(&message_box_exit, &button_id) < 0)
	{
        SDL_Log("Error displaying message box");
        return 1;
    }
	if (button_id == -1)
	{
        SDL_Log("no selection");
    }
    else
    {
        if (!button_id)
		{
			if (!player[0].has_treasure && !player[0].has_fake_treasure)
			{
				SDL_ShowMessageBox(&message_box_treasure, &button_id);
				if (!button_id)
					return 1;
			}
			if (player[0].has_fake_treasure)
			{
				SDL_ShowMessageBox(&message_box_fake_treasure, &button_id);
				if (button_id)
					return 1;
			}
			if (player[0].has_treasure)
			{
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Victory", "Congratulations! You have found treasure!", main_window);
				return 1;
			}
		}
    }
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
			printf("  %s", "T F W @ H # (h) (t)");
		if (j == 4)
			printf("  %i %i %i %i %i %i %li %i", player[0].has_treasure, player[0].has_fake_treasure, player[0].has_weapon, player[0].is_wounded, player[0].in_hole, player[0].in_trap, TIME_IN_TRAP-(SDL_GetTicks()-player[0].trap_start), (int)player[0].trap_start);
		printf("%s", "\n");
	}
	printf("%s", "\n");
	return;
}
