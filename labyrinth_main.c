/*  labyrinth_main.c

  Лабиринт
  Version 0.3.1

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

#include "includes_macros.h"

void help(char *program_title, int help_size);
int change_settings_file(struct options_array *options_arrayPtr, FILE *settings, char *settings_filename);

int set_options(FILE *settings, char *settings_filename, short int *window_height, short int *window_width, short int *fullscreen, short int *refresh_rate, short int *fps, short int *hw_accel, short int *vsync, short int *sdl_12, short int *size_labyrinth_length, short int *size_labyrinth_width, short int *no_walls_removing, short int *rivals, short int *holes, short int *trap_time, short int *hole_time, short int *speed, short int *turn_speed, short int *debug, short int *visual, short int *result);

int parameters(char *args[], int count, short int *window_height, short int *window_width, short int *fullscreen, short int *refresh_rate, short int *fps, short int *hw_accel, short int *vsync, short int *sdl_12, short int *size_labyrinth_length, short int *size_labyrinth_width, short int *no_walls_removing, short int *rivals, short int *holes, short int *trap_time, short int *hole_time, short int *speed, short int *turn_speed, short int *debug, short int *visual, short int *result);

int labyrinth_generation(int *labyrinth, short int const size_labyrinth_length, short int const size_labyrinth_width, short int const visual, short int const no_walls_removing, short int const result, short int const rivals, short int const holes, short int *holes_array, struct players player[]);

void clean_up(int *labyrinth, short int const size_labyrinth_width, short int const size_labyrinth_length, short int *holes_array, short int const holes, char *settings_filename);

int sdl_main(int *labyrinth, struct players player[], short int const rivals, short int const size_labyrinth_length, short int const size_labyrinth_width, short int const holes, short int const *holes_array, short int window_height, short int window_width, short int fullscreen, short int refresh_rate, short int fps, short int trap_time, short int hole_time, short int speed, short int turn_speed, short int const debug, FILE *settings, char const *settings_filename);
//~ int sdl_main(int *labyrinth, struct players player[], short int const size_labyrinth_length, short int const size_labyrinth_width, short int const holes, short int const *holes_array);

int sdl_main_hwsw(int *labyrinth, struct players player[], short int const rivals, short int const size_labyrinth_length, short int const size_labyrinth_width, short int const holes, short int const *holes_array, short int window_height, short int window_width, short int fullscreen, short int refresh_rate, short int fps, short int hw_accel, short int vsync, short int trap_time, short int hole_time, short int speed, short int turn_speed, short int const debug, FILE *settings, char const *settings_filename);
//~ int sdl_main_hwsw(int *labyrinth, struct players player[], short int const size_labyrinth_length, short int const size_labyrinth_width, short int const holes, short int const *holes_array, int requested_mode);

int main(int argc, char *argv[])
{
	//Проверка на запрос вызова справки, если запрошена - вызов справочной информации и выход из программы
	if (argc > 1)
	{
		for (int i = 1; i < argc; i++)
		{
			if (!strcmp("-h", argv[i]) || !strcmp("--help", argv[i]))
			{
				char *program_title=argv[0];
				help(program_title, strlen(argv[i]));
				return 0;
			}
		}
	}
	
	//Инициализация основных переменных
	//Значения по умолчанию - в файле includes_macros.h
	//Значения можно менять через параметры запуска и файл настроек (по умолчанию "settings.ini" в каталоге с программой)
	//VIDEO
	//Данные параметры (особенно refresh_rate) скорее всего, будут перезаписаны
	//после загрузки файла настроек и/или инициализации SDL
	short int window_height=DEFAULT_RES_Y; //возможно, стоит вернуть на int для покрытия больших разрешений
	short int window_width=DEFAULT_RES_X; //возможно, стоит вернуть на int для покрытия больших разрешений
	short int fullscreen=0;
	short int refresh_rate=0;
	short int fps=FPS; //органичение на количество обрабатываемых SDL кадров в секунду
	short int hw_accel=1; //только для SDL2
	short int vsync=1; //только для SDL2
	short int sdl_12=0; //только для SDL1.2
	//GAME
	short int size_labyrinth_length=DEFAULT_LENGTH+2; //длина - задаётся в настройках+добавляются ячейки для крайних стенок
	//~ if (size_labyrinth_length%2 == 0)
	//~ {
		//~ size_labyrinth_length=size_labyrinth_length-1; //если задано чётное число - убирается дублирующая сторона (стена)
	//~ }
	short int size_labyrinth_width=DEFAULT_WIDTH+2; //ширина - задаётся в настройках+добавляются ячейки для крайних стенок
	//~ if (size_labyrinth_width%2 == 0)
	//~ {
		//~ size_labyrinth_width=size_labyrinth_width-1; //если задано чётное число - убирается дублирующая сторона (стена)
	//~ }
	short int no_walls_removing=0; //не убирать случайные стены в лабиринте (по умолчанию - нет (убирать))
	short int rivals=RIVALS; //количество соперников в лабиринте (включая игрока!)
	short int holes=HOLES; //количество дыр в лабиринте
	short int trap_time=TIME_IN_TRAP; //время, проведённое в ловушке (в миллисекундах)
	short int hole_time=TIME_BETWEEN_HOLES; //время, требуемое для перемещения между дырами (в миллисекундах)
	short int speed=SPEED_MOVE; //время в миллисекундах, требуемое для совершения одного шага (продвижения на 1 ячейку)
	short int turn_speed=SPEED_TURN; //время в миллисекундах, требуемое для поворота
	//DEBUG
	short int debug=0; //отображение лабиринта в консоли/терминале во время игры
	short int visual=0; //для отладки - выводить скелет лабиринта (по умолчанию - нет) или да
	short int result=0; //отображать окончательный результат или нет (по умолчанию - нет)
	
	//Считывание параметров из файла настроек
	//Сначала производится проверка на присутствие среди параметров запуска
	//запроса на создание/использование другого файла с настройками
	//или перезаписи существующего значениями по умолчанию
	FILE *settings=NULL;
	char *settings_filename=NULL;
	if (argc > 1)
	{
		for (int i = 1; i < argc; i++)
		{
			if (!strcmp("--conf", argv[i]) || !strcmp("-c", argv[i]))
			{
				if ((i+1) < argc)
				{
					if (!(settings = fopen(argv[i+1], "r")))
					{
						perror("Не могу открыть файл настроек, заданный пользователем");
						printf("Будет использоваться файл по умолчанию (\"%s\")\n", DEFAULT_SETTINGS_FILE);
					}
					else
					{
						int filename_length=strlen(argv[i+1]);
						settings_filename=calloc(filename_length+1, 1);
						snprintf(settings_filename, filename_length+1, "%s", argv[i+1]);
					}
				}
				else
				{
					puts("Не указан файл настроек!");
					printf("Будет использоваться файл по умолчанию (\"%s\")\n", DEFAULT_SETTINGS_FILE);
				}
			}
			//Если требуется создать новый файл настроек - будут использованы имя и путь, указанные пользователем
			//Если первым символом в пути/имени будет "-" - оно будет считаться одним из параметров запуска и проигнорировано
			//(будет использоваться файл по умолчанию)
			if (!strcmp("--new-conf", argv[i]) || !strcmp("-N", argv[i]))
			{
				if (((i+1) < argc) && strncmp("-", argv[i+1], 1))
				{
					int filename_length=strlen(argv[i+1]);
					settings_filename=calloc(filename_length+1, 1);
					snprintf(settings_filename, filename_length+1, "%s", argv[i+1]);
					printf("Создаю новый файл настроек \"%s\"...\n", settings_filename);
					if (change_settings_file(NULL, settings, settings_filename))
					{
						printf("Будет использоваться файл по умолчанию (\"%s\")\n", DEFAULT_SETTINGS_FILE);
						free(settings_filename);
						settings_filename=NULL;
					}
				}
				//Если не указан новый файл для создания настроек - будет перезаписан существующий файл
				//TODO: обеспечить резервное копирование для таких случаев
				else
				{
					settings_filename=calloc(strlen(DEFAULT_SETTINGS_FILE)+1, 1);
					snprintf(settings_filename, strlen(DEFAULT_SETTINGS_FILE)+1, "%s", DEFAULT_SETTINGS_FILE);
					puts("Не указан файл настроек!");
					printf("Будет перезаписан файл по умолчанию (\"%s\")\n", settings_filename);
					if (change_settings_file(NULL, settings, settings_filename))
						return 1;
				}
			}
		}
	}
	if (!settings_filename)
	{
		settings_filename=calloc(strlen(DEFAULT_SETTINGS_FILE)+1, 1);
		snprintf(settings_filename, strlen(DEFAULT_SETTINGS_FILE)+1, "%s", DEFAULT_SETTINGS_FILE);
	}
	//После производится считывание файла настроек ("settings.ini" или заданного через параметры запуска)
	short int success;
	success=set_options(settings, settings_filename, &window_height, &window_width, &fullscreen, &refresh_rate, &fps, &hw_accel, &vsync, &sdl_12, &size_labyrinth_length, &size_labyrinth_width, &no_walls_removing, &rivals, &holes, &trap_time, &hole_time, &speed, &turn_speed, &debug, &visual, &result);
	//Обработка ошибок чтения фала настроек ошибок памяти
	if (success == 1 || success == 2)
		return success;
	//Просмотр параметров запуска, если они есть
	//В случае задания взаимоисключающих параметров или их дублирования
	//будет учтён последний указанный параметр/последнее значение
	if (argc > 1)
	{
		if (parameters(argv, argc, &window_height, &window_width, &fullscreen, &refresh_rate, &fps, &hw_accel, &vsync, &sdl_12, &size_labyrinth_length, &size_labyrinth_width, &no_walls_removing, &rivals, &holes, &trap_time, &hole_time, &speed, &turn_speed, &debug, &visual, &result))
			return 0;
	}
	//Инициализация массива лабиринта
	//FIXME: может, перенести в labyrinth_generation?
	int *labyrinth=(int*)calloc((size_labyrinth_width*size_labyrinth_length), sizeof( int ));
	if (labyrinth == NULL)
	{
		puts("Недостаточно свободной памяти для инициализации лабиринта");
		return 1;
	}
	struct players player[rivals]; //массив структур с типом, координатами и т. д. соперников (включая игрока)
	//Инициализация массива дыр (если требуется)
	//if (holes)
	//{
		short int *holes_array=(short int*)calloc(holes, sizeof( short int ));
		if (holes_array == NULL)
		{
			holes=0;
			puts("Недостаточно свободной памяти для инициализации массива дыр");
		}
	//}
	if (labyrinth_generation(labyrinth, size_labyrinth_length, size_labyrinth_width, visual, no_walls_removing, result, rivals, holes, holes_array, player))
	{
		puts("Недостаточно свободной памяти для генерации лабиринта");
		return 1;
	}
	
	//Для отладки аппаратного ускорения
	//TODO: добавить в параметры запуска
	//Теперь можно менять через файл настроек
	//~ int mode=0;
	//~ if (argc > 1)
	//~ {
		//~ if ((argv[1]) && (!strcmp("-sw", argv[1])))
		//~ {
			//~ mode=1;
		//~ }
		//~ if ((argv[1]) && (!strcmp("-swvs", argv[1])))
		//~ {
			//~ mode=2;
		//~ }
		//~ if ((argv[1]) && (!strcmp("-hw", argv[1])))
		//~ {
			//~ mode=3;	
		//~ }
		//~ if ((argv[1]) && (!strcmp("-hwvs", argv[1])))
		//~ {
			//~ mode=4;
		//~ }
	//~ }
	//~ if (mode)
	if (!sdl_12)
	{
		if (sdl_main_hwsw(labyrinth, player, rivals, size_labyrinth_length, size_labyrinth_width, holes, holes_array, window_height, window_width, fullscreen, refresh_rate, fps, hw_accel, vsync, trap_time, hole_time, speed, turn_speed, debug, settings, settings_filename))
		//~ if (sdl_main_hwsw(labyrinth, player, size_labyrinth_length, size_labyrinth_width, holes, holes_array, mode))
		{
			clean_up(labyrinth, size_labyrinth_width, size_labyrinth_length, holes_array, holes, settings_filename);
			return 1;
		}
		else
		{
			clean_up(labyrinth, size_labyrinth_width, size_labyrinth_length, holes_array, holes, settings_filename);
			return 0;
		}
	}
	//FIXME: изменить имя функции!
	if (sdl_main(labyrinth, player, rivals, size_labyrinth_length, size_labyrinth_width, holes, holes_array, window_height, window_width, fullscreen, refresh_rate, fps, trap_time, hole_time, speed, turn_speed, debug, settings, settings_filename))
	//~ if (sdl_main(labyrinth, player, size_labyrinth_length, size_labyrinth_width, holes, holes_array))
	{
		clean_up(labyrinth, size_labyrinth_width, size_labyrinth_length, holes_array, holes, settings_filename);
		return 1;
	}
	//labyrinth={0};
	/*memset(labyrinth, 0, sizeof(*labyrinth)*size_labyrinth_width*size_labyrinth_length);
	free(labyrinth);
	labyrinth=NULL;*/
	clean_up(labyrinth, size_labyrinth_width, size_labyrinth_length, holes_array, holes, settings_filename);
	return 0;
}

//Попытка корректного освобождения ресурсов
void clean_up(int *labyrinth, short int const size_labyrinth_width, short int const size_labyrinth_length, short int *holes_array, short int const holes, char *settings_filename)
{
	memset(labyrinth, 0, sizeof(*labyrinth)*size_labyrinth_width*size_labyrinth_length);
	memset(holes_array, 0, sizeof(*holes_array)*holes);
	//FIXME: перенести в atexit()
	free(labyrinth);
	free(holes_array);
	free(settings_filename);
	labyrinth=NULL;
	holes_array=NULL;
	settings_filename=NULL;
	return;
}
