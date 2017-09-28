/*  labyrinth_generation.c

  Лабиринт
  Version 0.2.3

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

//int check_in_neighbor_cells (int x, int y, int const max_distance, int const size_labyrinth_length, int const size_labyrinth_width, int const *labyrinth);
int check_in_neighbor_cells_border (int x, int y, int const max_distance, int const size_labyrinth_length, int const size_labyrinth_width, int const *labyrinth, int i); //for debug - вывод переменной "i"
int check_in_neighbor_cells_around (int x, int y, unsigned short int const direction_mask, int depth, int const size_labyrinth_length, int const size_labyrinth_width, int const *labyrinth);
void show_labyrinth_in_progress(int x, int y, char *info_string, int info, int count, int progress, int length, int width, const int *labyrinth_temp);

int labyrinth_generation(int *labyrinth, int const size_labyrinth_length, int const size_labyrinth_width, int const visual, int const no_walls_removing, int const result, int const rivals, struct players player[])
{
	time_t start_global_generation=time(NULL); //Время начала общей генерации лабиринта
	//Заполнение массива значениями по умолчанию
	//Крайние верняя и нижняя стенки
	for (int i = 0; i < size_labyrinth_length; i++)
	{
		labyrinth[i]=WALL;
		labyrinth[(size_labyrinth_length*size_labyrinth_width-size_labyrinth_length+i)]=WALL;
	}
	//Крайние левая и правая стенки
	for (int i = 1; i < size_labyrinth_width-1; i++)
	{
		labyrinth[(i*size_labyrinth_length)]=WALL;
		labyrinth[(i*size_labyrinth_length+size_labyrinth_length-1)]=WALL;
	}
	//Внутренняя структура
	for (int i = 1; i < size_labyrinth_width-1; i++)
	{
		for (int j = 1; j < size_labyrinth_length-1; j++)
		{
			//if ((i%2 == 0)&&(j%2 == 0))
			if ((i%2 != 0)&&(j%2 != 0))
			{
				labyrinth[(i*size_labyrinth_length)+j]=DUMMY; //DUMMY=2 - будущая пустая клетка
			}
			else
			{
				labyrinth[(i*size_labyrinth_length)+j]=WALL; //WALL=1 - будущая стена
			}	
		}
	}
	//Генерация лабиринта
	srand(time(NULL)); //инициализация генератора случайных чисел
	//Установка начальных координат для генерации					x y-->
	int x=0; //координата ячейки по вертикали (номер строки)		|
	int y=0; //координата ячейки по горизонтали (номер столбца)		V
	//Координаты должны быть целыми нечётными числами, чтобы не оказаться в ячейке, в которой расположена стена,
	//и находится в диапазоне от 1 до size_labyrinth_length-2/size_labyrinth_width-2 (внутри лабиринта, не выпадая на крайние стены)
	do
	{
		x=rand()%(size_labyrinth_width-2)+1;
	} while (x%2 == 0);

	do
	{
		y=rand()%(size_labyrinth_length-2)+1;
	} while (y%2 == 0);
	
	labyrinth[(x*size_labyrinth_length)+y]=CELL; //признак просмотренной ячейки (CELL=0 - пустая клетка)
	int cells_for_review[(size_labyrinth_width*size_labyrinth_length)]; //отложенные ячейки, имеющие непросмотренные соседние ячейки
	int count=0; //индекс массива cells_for_review[]
	int progress=0; //для шкалы прогресса (а надо ?)
	int all_viewed=0; //признак наличия непросмотренных ячеек массива
	while (!all_viewed)
	{
		//Определение количества непросмотренных соседних ячеек
		int neighbor_for_review=0;
		int vector=0;
		if (x-2 >= 0 && labyrinth[(x-2)*size_labyrinth_length+y] == DUMMY)
		{
			neighbor_for_review++;
		}
		if (y-2 >= 0 && labyrinth[(x*size_labyrinth_length)+(y-2)] == DUMMY)
		{
			neighbor_for_review++;
		}
		//if (x+2 < size_labyrinth_width && x+1 < size_labyrinth_width && labyrinth[(x+2)*size_labyrinth_length+y] == 2)
		if (x+2 < size_labyrinth_width && labyrinth[(x+2)*size_labyrinth_length+y] == DUMMY)
		{
			neighbor_for_review++;
		}
		//if (y+2 < size_labyrinth_length && y+1 < size_labyrinth_length && labyrinth[(x*size_labyrinth_length)+(y+2)] == 2)
		if (y+2 < size_labyrinth_length && labyrinth[(x*size_labyrinth_length)+(y+2)] == DUMMY)
		{
			neighbor_for_review++;
		}
		//Если есть непросмотренные соседние ячейки - случайным образом определяем, к какой направиться, если возможно,
		//(0 - вверх, 1 - вправо, 2 - вниз, 3 - влево)
		//меняем состояние найденной ячейки и ячейки между ней и текущей на состояние "пустая клетка"
		//и устанавливаем новые координаты в качестве текущих
		if (neighbor_for_review > 0)
		{
			//Если количество непросмотренных соседних ячеек больше одной - запоминаем адрес текущей ячейки, если она ещё не записана
			if (neighbor_for_review > 1)
			{
				int repeat=0;
				for (int i = 0; i < count; i=i+2)
				{
					if (cells_for_review[i] == x && cells_for_review[i+1] == y)
					{
						repeat++;
					}
				}
				if (!repeat)
				{
					cells_for_review[count]=x;
					cells_for_review[count+1]=y;
					count=count+2;
				}
			}
			while (vector != 4 )
			{
				vector=rand()%4;
				if (vector == UP && (x-2) >= 0 && labyrinth[(x-2)*size_labyrinth_length+y] == DUMMY)
				{
					x=x-2;
					labyrinth[(x+1)*size_labyrinth_length+y]=CELL;
					vector=4;
				}
				if (vector == RIGHT && (y-2) >= 0 && labyrinth[(x*size_labyrinth_length)+(y-2)] == DUMMY)
				{
					y=y-2;
					labyrinth[(x*size_labyrinth_length)+(y+1)]=CELL;
					vector=4;
				}
				if (vector == DOWN && (x+2) < size_labyrinth_width && labyrinth[(x+2)*size_labyrinth_length+y] == DUMMY)
				{
					x=x+2;
					labyrinth[(x-1)*size_labyrinth_length+y]=CELL;
					vector=4;
				}
				if (vector == LEFT && (y+2) < size_labyrinth_length && labyrinth[(x*size_labyrinth_length)+(y+2)] == DUMMY)
				{
					y=y+2;
					labyrinth[(x*size_labyrinth_length)+(y-1)]=CELL;
					vector=4;
				}
			}
			labyrinth[(x*size_labyrinth_length)+y]=CELL;
		}
		//Если все соседние ячейки просмотрены - меняем состояние текущей ячейки на "пустая клетка"
		else
		{
			labyrinth[(x*size_labyrinth_length)+y]=CELL;
			//Если есть непросмотренные ячейки (массив cells_for_review не пуст) - просматриваем их на следующем проходе цикла
			if (count)
			{
				x=cells_for_review[count-2];
				y=cells_for_review[count-1];
				count=count-2;
			}
			//Иначе - считаем, что все ячейки просмотрены (лабиринт сгенерирован), устанавливаем соответствующий признак 
			else
			{
				all_viewed=1;
			}
		}
		progress++;
	//}
	
	
		//Вывод отладки и конечного результата - скелет лабиринта (если не отключено через параметры)
		//double progress_print=progress/(size_labyrinth_width*size_labyrinth_length/2)*100;
		//Для шкалы прогресса (а надо оно?)
		if (visual || result)
		{
			if (!visual && !all_viewed)
				continue;
			show_labyrinth_in_progress(x, y, "neighbor_for_review=", neighbor_for_review, count, progress, size_labyrinth_length, size_labyrinth_width, labyrinth);
		}
	}
	
	//Убираем случайные стенки (если не отключено через параметры)
	if (!no_walls_removing)
	{
		int walls=size_labyrinth_width*size_labyrinth_length/10; //количество стенок, подлежащих уничтожению - 1/10 от размера лабиринта
		for (int i = 0; i < walls; i++)
		{
			int x_wall, y_wall;
			int is_wall=0;
			while (!is_wall)
			{
				x_wall=rand()%size_labyrinth_width;
				y_wall=rand()%size_labyrinth_length;
				//Если координата указывает на крайнюю стену - переход к следующей итерации цикла
				if (!x_wall || !y_wall || x_wall == size_labyrinth_width-1 || y_wall == size_labyrinth_length-1)
					continue;
				if (labyrinth[(x_wall*size_labyrinth_length)+y_wall])
				{
					//Если рядом есть хотя бы одна пустая клетка (проход), значит, стену можно убрать
					//(в противном случае есть риск получить недоступные зоны, в которые, при дальнейшей генерации,
					//могут попасть участники/события)
					if (x_wall)
					{
						if (!labyrinth[(x_wall-1)*size_labyrinth_length+y_wall])
						{
							labyrinth[(x_wall*size_labyrinth_length)+y_wall]=CELL;
							is_wall=1;
							break;
						}
					}
					if (y_wall)
					{
						if (!labyrinth[x_wall*size_labyrinth_length+(y_wall-1)])
						{
							labyrinth[(x_wall*size_labyrinth_length)+y_wall]=CELL;
							is_wall=1;
							break;
						}
					}
					if (x_wall < size_labyrinth_width-1)
					{
						if (!labyrinth[(x_wall+1)*size_labyrinth_length+y_wall])
						{
							labyrinth[(x_wall*size_labyrinth_length)+y_wall]=CELL;
							is_wall=1;
							break;
						}
					}
					if (y_wall < size_labyrinth_length-1)
					{
						if (!labyrinth[x_wall*size_labyrinth_length+(y_wall+1)])
						{
							labyrinth[(x_wall*size_labyrinth_length)+y_wall]=CELL;
							is_wall=1;
							break;
						}
					}
					//labyrinth[(x_wall*size_labyrinth_length)+y_wall]=0;
					//is_wall=1;
				}
			}
			
			//Вывод отладки и конечного результата - скелет лабиринта (если не отключено через параметры)
			//double progress_print=progress/(size_labyrinth_width*size_labyrinth_length/2)*100;
			//Для шкалы прогресса (а надо оно?)
			if (visual)
			{
				show_labyrinth_in_progress(x_wall, y_wall, "walls=", walls, i+1, progress, size_labyrinth_length, size_labyrinth_width, labyrinth);
			}
		}
	}
	time_t end_generation=time(NULL); //Время окончания генерации непосредственно лабиринта
	//printf("Время на генерацию скелета: %.2f сек.\n", difftime(end_generation, start_generation));
	//Для MinGW
	double time_diff=end_generation-start_global_generation;
	printf("Время на генерацию скелета: %.2f сек.\n", time_diff);
	//Генерация координат соперников (включая игрока)
	//Клетки с полученными координатами должны быть пусты и находиться у одной из крайних стен лабиринта
	//(внутри лабиринта, не выпадая на крайние стены)
	time_t start_generation=time(NULL); //Время начала генерации участников
	int *rivals_coordinates=(int*)calloc(rivals*2+2, sizeof( int )); //временный массив координат участников и выхода из лабиринта
	if (rivals_coordinates == NULL)
		return 1;
	for (int i=0; i < rivals+1; i++) //Генерация координат участников и выхода
	{
		time_t start_rival_generation=time(NULL); //Время начала генерации координат очередного участника
		int wall, near_edge, distance;
		do
		{
			wall=0, near_edge=0, distance=1;
			x=rand()%(size_labyrinth_width-2)+1;
			y=rand()%(size_labyrinth_length-2)+1;
			if (labyrinth[x*size_labyrinth_length+y] != CELL)
				wall=1;
			if (x == 1 || x == size_labyrinth_width-2 || y == 1 || y == size_labyrinth_length-2)
				near_edge=1;
			//FIXME: в идеале, клетки с полученными координатами должны быть
			//ещё и на некотором расстоянии от других соперников и иных событий
			//(например, на, как минимум, половину значения по наибольшей стороне лабиринта,
			//но менее, чем большая сторона лабиринта, во избежание проблем с расчётами в формулах)
			if (!wall && near_edge)
			{
				//int max_distance, part_1, part_2, part_3;
				int max_distance;
				//~ max_distance=(size_labyrinth_length/2)+(size_labyrinth_width/2);
				if (size_labyrinth_length > size_labyrinth_width)
					//max_distance=size_labyrinth_length/2-2;
					max_distance=(size_labyrinth_length-2)/2;
				else
					//max_distance=size_labyrinth_width/2-2;
					max_distance=(size_labyrinth_width-2)/2;
				distance=check_in_neighbor_cells_border (x, y, max_distance, size_labyrinth_length, size_labyrinth_width, labyrinth, i);
			}
			//Если прошло более 5 сек. (?) - что-то идёт не так, счётчик for сбрасывается к началу цикла,
			//временный массив обнуляется, основной массив восстанавливается к начальному состоянию
			//if (difftime(time(NULL), start_rival_generation) > 5)
			//Для MinGW
			if ((time(NULL)-start_rival_generation) > 5)
			{
				printf("%i: Resetted\n", i+1);
				for (int reset_count = 0; reset_count < rivals*2+2; reset_count=reset_count+2)
				{
					if (labyrinth[rivals_coordinates[reset_count]*size_labyrinth_length+rivals_coordinates[reset_count+1]] == RIVAL)
						labyrinth[rivals_coordinates[reset_count]*size_labyrinth_length+rivals_coordinates[reset_count+1]]=CELL;
				}
				memset(rivals_coordinates, 0, sizeof(*rivals_coordinates)*(rivals*2+2));
				i=-1;
				break;
			}
		} while (wall || !near_edge || !distance);
		//Сохранение координаты в основном и во временном массивах (если не было сброса)
		if (i >= 0)
		{
			rivals_coordinates[i+i]=x; //0->0 1->2 2->4 3->6
			rivals_coordinates[i+i+1]=y; //0->1 1->3 2->5 3->7
			labyrinth[(x*size_labyrinth_length)+y]=RIVAL;
		}
		printf("%iF: %i %i\n", i+1, x, y);
		//Если прошло более 20 сек. (?) с момента начала генерации участников (или лучше общей?) -
		//что-то идёт не так, нормальная генерация невозможна,
		//устанавливаются значения по умолчанию (по углам лабиринта)
		//Выход из лабиринта уставливается посередине одной из стен в случайном порядке;
		//если ширина и длина лабиринта различаются - то по одной из наибольших стен, также в случайном порядке
		//FIXME: текущая реализация не сработает, если участников больше 4-х
		//if (difftime(time(NULL), start_generation) > 20)
		//Для MinGW
		if ((time(NULL)-start_generation) > 20)
		{
			printf("%i: Breaked\n", i+1);
			//Если не было сброса - временный массив обнуляется, основной массив восстанавливается к начальному состоянию
			if (i >= 0)
			{
				for (int reset_count = 0; reset_count < rivals*2+2; reset_count=reset_count+2)
				{
					//if (labyrinth[rivals_coordinates[reset_count]*size_labyrinth_length+rivals_coordinates[reset_count+1]] != CELL || labyrinth[rivals_coordinates[reset_count]*size_labyrinth_length+rivals_coordinates[reset_count+1]] != WALL)
					if (labyrinth[rivals_coordinates[reset_count]*size_labyrinth_length+rivals_coordinates[reset_count+1]] == RIVAL)
					{
						labyrinth[rivals_coordinates[reset_count]*size_labyrinth_length+rivals_coordinates[reset_count+1]]=CELL;
					}
				}
				memset(rivals_coordinates, 0, sizeof(*rivals_coordinates)*(rivals*2+2));
			}
			//~ if (labyrinth[0])
			//~ {
				//~ if (!labyrinth[1])
					//~ labyrinth[1]=3;
				//~ else if (!labyrinth[size_labyrinth_length])
					//~ labyrinth[size_labyrinth_length]=3;
				//~ else if (!labyrinth[size_labyrinth_length+1])
					//~ labyrinth[size_labyrinth_length+1]=3;
				//~ else
					//~ printf("Участник %i не расположен :-(\n", i/2+1);
			rivals_coordinates[0]=1;
			rivals_coordinates[1]=1;
			rivals_coordinates[2]=1;
			rivals_coordinates[3]=size_labyrinth_length-2;
			rivals_coordinates[4]=size_labyrinth_width-2;
			rivals_coordinates[5]=1;
			rivals_coordinates[6]=size_labyrinth_width-2;
			rivals_coordinates[7]=size_labyrinth_length-2;
			int wall_with_exit=0;
			if (size_labyrinth_length-size_labyrinth_width > 0)
			{
				do
				{
					wall_with_exit=rand()%3; //Ожидается 0 или 2 - UP/DOWN
				} while (wall_with_exit%2);
			}
			else if (size_labyrinth_length-size_labyrinth_width < 0)
			{
				do
				{
					wall_with_exit=rand()%3+1; //Ожидается 1 или 3 - RIGHT/LEFT
				} while (!(wall_with_exit%2));
			}
			else
				wall_with_exit=rand()%4;
			printf("wall_with_exit:%i\n", wall_with_exit);
			switch (wall_with_exit)
			{
				case UP:
					rivals_coordinates[8]=1;
					if (size_labyrinth_length%2)
					{
						rivals_coordinates[9]=(size_labyrinth_length-2)/2+1; //Для компенсации возможного остатка от деления
					}
					else
						rivals_coordinates[9]=(size_labyrinth_length-2)/2;
					break;
				case RIGHT:
					if (size_labyrinth_width%2)
					{
						rivals_coordinates[8]=(size_labyrinth_width-2)/2+1; //Для компенсации возможного остатка от деления
					}
					else
						rivals_coordinates[8]=(size_labyrinth_width-2)/2;
					rivals_coordinates[9]=size_labyrinth_length-2;
					break;
				case DOWN:
					rivals_coordinates[8]=size_labyrinth_width-2;
					if (size_labyrinth_length%2)
					{
						rivals_coordinates[9]=(size_labyrinth_length-2)/2+1; //Для компенсации возможного остатка от деления
					}
					else
						rivals_coordinates[9]=(size_labyrinth_length-2)/2;
					break;
				case LEFT:
					if (size_labyrinth_width%2)
					{
						rivals_coordinates[8]=(size_labyrinth_width-2)/2+1; //Для компенсации возможного остатка от деления
					}
					else
						rivals_coordinates[8]=(size_labyrinth_width-2)/2;
					rivals_coordinates[9]=1;
					break;
			}
			//FIXME: не сработает, если участников больше 4-х
			//~ Если участников более 4-х - заполянем координты автоматически (примерно посередине между предыдущими участниками)
			//~ for (int rival_count=4; rival_count < rivals; rival_count++)
			//~ {
				//~ rivals_coordinates[rival_count*2]=rivals_coordinates[rival_count*2+1]
			for (int break_count=0; break_count < rivals*2+2; break_count=break_count+2)
			{
				//Если возможно установить координату участника непосредственно в углу
				//(клетка пуста) - так и происходит
				//Иначе, проверяется возможность установки в соседние клетки (+/-x, +/-y, +/-x+/-y)
				if (!labyrinth[rivals_coordinates[break_count]*size_labyrinth_length+rivals_coordinates[break_count+1]])
				{
					labyrinth[rivals_coordinates[break_count]*size_labyrinth_length+rivals_coordinates[break_count+1]]=RIVAL;
					continue;
				}
				int sign_x, sign_y;
				if (rivals_coordinates[break_count] == 1)
					sign_x=1;
				else
					sign_x=-1;
				if (rivals_coordinates[break_count+1] == 1)
					sign_y=1;
				else
					sign_y=-1;
				if (!labyrinth[(rivals_coordinates[break_count]+sign_x*1)*size_labyrinth_length+rivals_coordinates[break_count+1]])
				{
					rivals_coordinates[break_count]=rivals_coordinates[break_count]+sign_x*1;
					labyrinth[rivals_coordinates[break_count]*size_labyrinth_length+rivals_coordinates[break_count+1]]=RIVAL;
				}
				else if (!labyrinth[rivals_coordinates[break_count]*size_labyrinth_length+(rivals_coordinates[break_count+1]+sign_y*1)])
				{
					rivals_coordinates[break_count+1]=rivals_coordinates[break_count+1]+sign_y*1;
					labyrinth[rivals_coordinates[break_count]*size_labyrinth_length+rivals_coordinates[break_count+1]]=RIVAL;
				}
				else if (!labyrinth[(rivals_coordinates[break_count]+sign_x*1)*size_labyrinth_length+(rivals_coordinates[break_count+1]+sign_y*1)])
				{
					rivals_coordinates[break_count]=rivals_coordinates[break_count]+sign_x*1;
					rivals_coordinates[break_count+1]=rivals_coordinates[break_count+1]+sign_y*1;
					labyrinth[rivals_coordinates[break_count]*size_labyrinth_length+rivals_coordinates[break_count+1]]=RIVAL;
				}
				else
					printf("Участник %i не расположен :-(\n", break_count/2+1);
			}
			break;
		}
	}
	//Передача одного из соперников под управление игрока (в случайном порядке)
	//и запись типа и координат соперников в массив структур
	//int player_number=rand()%rivals;
	int player_number=0; //для простоты и ясности - всё равно соперники располагаются по лабиринту в случайном порядке
	for (int i = 0; i < rivals; i++)
	{
		count=0;
		player[i].x=rivals_coordinates[i+i];
		player[i].y=rivals_coordinates[i+i+1];
		if (player_number == i)
		{
			player[i].type=1;
			labyrinth[(rivals_coordinates[i+i]*size_labyrinth_length+rivals_coordinates[i+i+1])]=PLAYER;
		}
		else
			player[i].type=0;
		//Определение, в какую сторону смотрит участник (0 - вверх, 1 - вправо, 2 - вниз, 3 - влево)
		//Если участник находится в углу - из двух возможных направлений выбирается случайное
		//Учитываются также возможные сдвиги при сбросе координат в значения по умолчанию при превышении лимита времени на генерацию
		if ((((rivals_coordinates[i+i] == 1) || (rivals_coordinates[i+i] == size_labyrinth_width-2)) && ((rivals_coordinates[i+i+1] == 1) || (rivals_coordinates[i+i+1] == size_labyrinth_length-2))) \
		|| (((rivals_coordinates[i+i] == 2) || (rivals_coordinates[i+i] == size_labyrinth_width-3)) && ((rivals_coordinates[i+i+1] == 2) || (rivals_coordinates[i+i+1] == size_labyrinth_length-3))))
		{
			count=rand()%2;
		}
		if (rivals_coordinates[i+i] == 1 || rivals_coordinates[i+i] == 2)
		{
			if (!count)
			{
				player[i].direction=DOWN;
				count=count+2;
			}
			count--;
		}
		if (rivals_coordinates[i+i] == size_labyrinth_width-2 || rivals_coordinates[i+i] == size_labyrinth_width-3)
		{
			if (!count)
			{
				player[i].direction=UP;
				count=count+2;
			}
			count--;
		}
		if (rivals_coordinates[i+i+1] == 1 || rivals_coordinates[i+i+1] == 2)
		{
			if (!count)
			{
				player[i].direction=RIGHT;
				count=count+2;
			}
			count--;
		}
		if (rivals_coordinates[i+i+1] == size_labyrinth_length-2 || rivals_coordinates[i+i+1] == size_labyrinth_length-3)
		{
			if (!count)
			{
				player[i].direction=LEFT;
				count=count+2;
			}
			count--;
		}
	}
	//Установка значения последней генерации в событие "Выход"
	//(с учётом возможных сдвигов в случае сброса координат в значения по умолчанию при превышении лимита времени на генерацию)
	//Если точка выхода находится в углу - из двух направлений выбирается случайное
	count=0;
	if ((((rivals_coordinates[rivals*2] == 1) || (rivals_coordinates[rivals*2] == size_labyrinth_width-2)) \
	&& ((rivals_coordinates[rivals*2+1] == 1) || (rivals_coordinates[rivals*2+1] == size_labyrinth_length-2))) \
	\
	|| (((rivals_coordinates[rivals*2] == 2) || (rivals_coordinates[rivals*2] == size_labyrinth_width-3)) \
	&& ((rivals_coordinates[rivals*2+1] == 2) || (rivals_coordinates[rivals*2+1] == size_labyrinth_length-3))))
	{
		count=rand()%2;
	}
	if (rivals_coordinates[rivals*2] == 1 || rivals_coordinates[rivals*2] == 2)
	{
		//Защита от ложного обнаружения сдвига
		if (labyrinth[(rivals_coordinates[rivals*2]-1)*size_labyrinth_length+rivals_coordinates[rivals*2+1]] == CELL)
			count++;
		if (!count)
		{
			for (int i = 0; i < rivals_coordinates[rivals*2]; )
			{
				labyrinth[rivals_coordinates[rivals*2]*size_labyrinth_length+rivals_coordinates[rivals*2+1]]=CELL;
				rivals_coordinates[rivals*2]--;
			}
			labyrinth[rivals_coordinates[rivals*2]*size_labyrinth_length+rivals_coordinates[rivals*2+1]]=EXIT;
			count=count+2;
		}
		count--;
	}
	if (rivals_coordinates[rivals*2] == size_labyrinth_width-2 || rivals_coordinates[rivals*2] == size_labyrinth_width-3)
	{
		//Защита от ложного обнаружения сдвига
		if (labyrinth[(rivals_coordinates[rivals*2]+1)*size_labyrinth_length+rivals_coordinates[rivals*2+1]] == CELL)
			count++;
		if (!count)
		{
			for (int i = 0; i < (size_labyrinth_width-1)-rivals_coordinates[rivals*2]; )
			{
				labyrinth[rivals_coordinates[rivals*2]*size_labyrinth_length+rivals_coordinates[rivals*2+1]]=CELL;
				rivals_coordinates[rivals*2]++;
			}
			labyrinth[rivals_coordinates[rivals*2]*size_labyrinth_length+rivals_coordinates[rivals*2+1]]=EXIT;
			count=count+2;
		}
		count--;
	}
	if (rivals_coordinates[rivals*2+1] == 1 || rivals_coordinates[rivals*2+1] == 3)
	{
		//Защита от ложного обнаружения сдвига
		if (labyrinth[(rivals_coordinates[rivals*2])*size_labyrinth_length+(rivals_coordinates[rivals*2+1]-1)] == CELL)
			count++;
		if (!count)
		{
			for (int i = 0; i < rivals_coordinates[rivals*2+1]; )
			{
				labyrinth[rivals_coordinates[rivals*2]*size_labyrinth_length+rivals_coordinates[rivals*2+1]]=CELL;
				rivals_coordinates[rivals*2+1]--;
			}
			labyrinth[rivals_coordinates[rivals*2]*size_labyrinth_length+rivals_coordinates[rivals*2+1]]=EXIT;
			count=count+2;
		}
		count--;
	}
	if (rivals_coordinates[rivals*2+1] == size_labyrinth_length-2 || rivals_coordinates[rivals*2+1] == size_labyrinth_length-3)
	{
		//Защита от ложного обнаружения сдвига
		if (labyrinth[(rivals_coordinates[rivals*2])*size_labyrinth_length+(rivals_coordinates[rivals*2+1]+1)] == CELL)
			count++;
		if (!count)
		{
			for (int i = 0; i < (size_labyrinth_length-1)-rivals_coordinates[rivals*2+1]; )
			{
				labyrinth[rivals_coordinates[rivals*2]*size_labyrinth_length+rivals_coordinates[rivals*2+1]]=CELL;
				rivals_coordinates[rivals*2+1]++;
			}
			labyrinth[rivals_coordinates[rivals*2]*size_labyrinth_length+rivals_coordinates[rivals*2+1]]=EXIT;
			count=count+2;
		}
		count--;
	}
	//~ int player=rand()%rivals;
	//~ count=0;
	//~ for (int i=0; i < size_labyrinth_length*size_labyrinth_width; i++)
	//~ {
		//~ if (labyrinth[i] == 3)
		//~ {
			//~ if (count == player)
			//~ {
				//~ labyrinth[i]=4;
				//~ break;
			//~ }
			//~ count++;
		//~ }
	//~ }
	free(rivals_coordinates);
	rivals_coordinates=NULL;
	end_generation=time(NULL); //Время окончания генерации
	//printf("Время на генерацию: %.2f сек.\n", difftime(end_generation, start_generation));
	//Для MinGW
	time_diff=end_generation-start_generation;
	printf("Время на генерацию участников: %.2f сек.\n", time_diff);
	char *dir[]={"вверх", "вправо", "вниз", "влево"};
	for (int i=0; i < rivals; i++)
	{
		printf("Player %i: type=%i, x=%i, y=%i, direction=%s\n", i, player[i].type, player[i].x, player[i].y, dir[player[i].direction]);
	}
	puts("");
	//printf("Время на генерацию: %.2f сек.\n", end_generation-start_generation);
	//Вывод отладки и конечного результата - скелет лабиринта (если не отключено через параметры)
	if (result)
		show_labyrinth_in_progress(0, 0, "Finish!", 0, 0, 100, size_labyrinth_length, size_labyrinth_width, labyrinth);
	time_diff=time(NULL)-start_global_generation;
	printf("Общее время генерации: %.2f сек.\n", time_diff);
	return 0;
}

void show_labyrinth_in_progress(int x, int y, char *info_string, int info, int count, int progress, int length, int width, const int *labyrinth_temp)
{
	double progress_print=width*length/2;
	progress_print=progress/progress_print;
	progress_print=progress_print*100;
	for (int j = 0; j < width; j++)
	//for (int j = -1; j <= width; j++)
	{
		//~ if (j == -1 || j == (width))
		//~ {
			//~ for (int j_walls = 0; j_walls < (length+2); j_walls++)
			//~ {
				//~ printf ("%s", " X");
			//~ }
			//~ puts("");
			//~ continue;
		//~ }
		for (int k = 0; k < length; k++)
		{
			//~ if (k == 0)
				//~ printf ("%s", " X");
			//0 - пустая клетка, 1 - стена, 2 - будущая пустая клетка, 3 - один из соперников, 4 - игрок
			if (!labyrinth_temp[(j*length)+k])
				printf("%s", "  ");
			if (labyrinth_temp[(j*length)+k] == WALL)
				printf("%s", " X");
			if (labyrinth_temp[(j*length)+k] == DUMMY)
				printf("%s", " .");
			if (labyrinth_temp[(j*length)+k] == RIVAL)
				printf("%s", " p");
			if (labyrinth_temp[(j*length)+k] == PLAYER)
				printf("%s", " g");
			if (labyrinth_temp[(j*length)+k] == EXIT)
				printf("%s", " e");
			//~ if (k == (length-1))
				//~ printf ("%s", " X");
		}
		if (j == 0)
			printf("  %s%i %s%i", "x=", x, "y=", y);
		if (j == 1)
			printf("  %s%i", info_string, info);
		if (j == 2)
			printf("  %s%i %s%.2f%%", "count=", count, "progress=", progress_print);
		printf("%s", "\n");
	}
	printf("%s", "\n");
	system("sleep 1");
	return;
}
