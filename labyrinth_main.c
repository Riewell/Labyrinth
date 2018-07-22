/*  labyrinth_main.c

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

#include "includes_macros.h"

int parameters(char *args[], int count, int *length, int *width, int *visual, int *no_walls_removing, int *show_result, int *num_holes);
int labyrinth_generation(int *labyrinth, int const size_labyrinth_length, int const size_labyrinth_width, int const visual, int const no_walls_removing, int const result, int const rivals, int const holes, int *holes_array, struct players player[]);
void clean_up(int *labyrinth, int const size_labyrinth_width, int const size_labyrinth_length, int *holes_array, int const holes);
int sdl_main(int *labyrinth, struct players player[], int const size_labyrinth_length, int const size_labyrinth_width, int const holes, int const *holes_array);
int sdl_main_hwsw(int *labyrinth, struct players player[], int const size_labyrinth_length, int const size_labyrinth_width, int const holes, int const *holes_array, int requested_mode);

int main(int argc, char *argv[])
{
	//~ for (i = 0; i < dimensions; i++)
	//~ {
		//~ floor_width[i]=rand()%DEFAULT_MAX_WIDTH+DEFAULT_MIN_WIDTH;
		//~ floor_length[i]=rand()%DEFAULT_MAX_LENGTH+DEFAULT_MIN_LENGTH;
		//~ floors[i]=rand()%DEFAULT_MAX_FLOORS+DEFAULT_MIN_FLOORS;
	//~ }
	
	//Инициализация основных переменных
	//Значения по умолчанию - в файле includes_macros.h
	//Значения можно менять через параметры запуска (добавить в Настройки и файл опций!)
	int size_labyrinth_length=DEFAULT_LENGTH+2; //длина - задаётся в настройках+добавляются ячейки для крайних стенок
	//~ if (size_labyrinth_length%2 == 0)
	//~ {
		//~ size_labyrinth_length=size_labyrinth_length-1; //если задано чётное число - убирается дублирующая сторона (стена)
	//~ }
	int size_labyrinth_width=DEFAULT_WIDTH+2; //ширина - задаётся в настройках+добавляются ячейки для крайних стенок
	//~ if (size_labyrinth_width%2 == 0)
	//~ {
		//~ size_labyrinth_width=size_labyrinth_width-1; //если задано чётное число - убирается дублирующая сторона (стена)
	//~ }
	int visual=0; //для отладки - выводить скелет лабиринта (по умолчанию - нет) или да
	int no_walls_removing=0; //убирать случайные стены в лабиринте (по умолчанию - да) или нет
	int result=0; //отображать окончательный результат или нет (по умолчанию - нет)
	int rivals=RIVALS; //количество соперников в лабиринте (включая игрока!)
	int holes=HOLES; //количество дыр в лабиринте
	struct players player[rivals]; //массив структур с типом, координатами и т. д. соперников (включая игрока)
	//Просмотр параметров запуска, если они есть
	//Если среди параметров есть требующие завершения работы после их обработки
	//(например, вызов справки) - производится проверка значения возврата из функции
	//и, если требуется, завершение работы программы
	if (argc > 1)
	{
		if (parameters(argv, argc, &size_labyrinth_length, &size_labyrinth_width, &visual, &no_walls_removing, &result, &holes))
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
	//Инициализация массива дыр (если требуется)
	//if (holes)
	//{
		int *holes_array=(int*)calloc(holes, sizeof( int ));
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
	int mode=0;
	if (argc > 1)
	{
		if ((argv[1]) && (!strcmp("-sw", argv[1])))
		{
			mode=1;
		}
		if ((argv[1]) && (!strcmp("-swvs", argv[1])))
		{
			mode=2;
		}
		if ((argv[1]) && (!strcmp("-hw", argv[1])))
		{
			mode=3;	
		}
		if ((argv[1]) && (!strcmp("-hwvs", argv[1])))
		{
			mode=4;
		}
	}
	if (mode)
	{
		if (sdl_main_hwsw(labyrinth, player, size_labyrinth_length, size_labyrinth_width, holes, holes_array, mode))
		{
			clean_up(labyrinth, size_labyrinth_width, size_labyrinth_length, holes_array, holes);
			return 1;
		}
		else
		{
			clean_up(labyrinth, size_labyrinth_width, size_labyrinth_length, holes_array, holes);
			return 0;
		}
	}
	//FIXME: изменить имя функции!
	if (sdl_main(labyrinth, player, size_labyrinth_length, size_labyrinth_width, holes, holes_array))
	{
		clean_up(labyrinth, size_labyrinth_width, size_labyrinth_length, holes_array, holes);
		return 1;
	}
	//labyrinth={0};
	/*memset(labyrinth, 0, sizeof(*labyrinth)*size_labyrinth_width*size_labyrinth_length);
	free(labyrinth);
	labyrinth=NULL;*/
	clean_up(labyrinth, size_labyrinth_width, size_labyrinth_length, holes_array, holes);
	return 0;
}

//Попытка корректного освобождения ресурсов
void clean_up(int *labyrinth, int const size_labyrinth_width, int const size_labyrinth_length, int *holes_array, int const holes)
{
	memset(labyrinth, 0, sizeof(*labyrinth)*size_labyrinth_width*size_labyrinth_length);
	memset(holes_array, 0, sizeof(*holes_array)*holes);
	//FIXME: перенести в atexit()
	free(labyrinth);
	free(holes_array);
	labyrinth=NULL;
	holes_array=NULL;
	return;
}
