/*  checking_in_neighbor_cells.c

  Лабиринт
  Version 0.2.5

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

//Проверка соседних ячеек по  краям лабиринта (заданная ячейка должна находиться около края лабиринта)

//int check_in_neighbor_cells(int x, int y, int const max_distance, int const size_labyrinth_length, int const size_labyrinth_width, int const *labyrinth)
int check_in_neighbor_cells_border(int x, int y, int const max_distance, short int const size_labyrinth_length, short int const size_labyrinth_width, int const *labyrinth, int i) //for debug - вывод переменной "i"
{
	int part_1, part_2, part_3;
	int distance=1;
	//Дистанция делится на три части - по текущей стене, и, в случае, если достигнут край лабиринта,
	//по перпендикулярной стене и (в случае достижения другого края) по параллельной стене
	//Изначально считается, что проверка проводится по одной стене, за один проход (одну часть) сразу,
	//сначала в одну, затем в другую сторону
	//FIXME: сократить простыню; пересмотреть проверки distance и x_count/y_count
	if ((x == 1 || x == size_labyrinth_width-2) && (distance))
	{
		//x==1 || x===size_labyrinth_width-2; y++
		part_1=max_distance, part_2=0, part_3=0;
		if (y+max_distance >= size_labyrinth_length-2)
		{
			part_1=size_labyrinth_length-y-2;
			part_2=max_distance-part_1;
		}
		if (part_2 >= size_labyrinth_width-2)
		{
			part_2=size_labyrinth_width-3;
			part_3=max_distance-part_1-part_2;
		}
		for (int y_count = 1; y_count <= part_1; y_count++)
		{
			printf("%i: %i %i\n", i+1, x, y+y_count);
			if (labyrinth[((x)*size_labyrinth_length)+(y+y_count)] != RIVAL)
				distance=1;
			else
			{
				return 0; //distance == 0
			}
		}
		//if (!distance) //distance уже равно "0", выход из функции строкой выше
			//continue;
		int x_count=0;
		//~ Если x==size_labyrinth_width-1, то его значение будет уменьшаться при каждом проходе, иначе - увеличиваться
		//Если x==1, то его значение будет увеличиваться при каждом проходе, иначе - уменьшаться
		for (int y_count = 1; y_count <= part_2; y_count++)
		{
		
			//~ if (x)
				//~ x_count=-y_count;
			//~ else
				//~ x_count=y_count;
			if (x == 1)
				x_count=y_count;
			else
				x_count=-y_count;
			printf("%i: %i %i\n", i+1, x+x_count, size_labyrinth_length-2);
			if (labyrinth[((x+x_count)*size_labyrinth_length)+(size_labyrinth_length-2)] != RIVAL)
				distance=1;
			else
			{
				return 0; //distance == 0
			}
		}
		//if (!distance) //distance уже равно "0", выход из функции строкой выше
			//continue;
		//Если при проходе part_2 x увеличивался, то x=size_labyrinth_width-2,
		//если уменьшался - то x=1
		if (x_count > 0)
			x_count=size_labyrinth_width-2;
		else
			x_count=1;
		for (int y_count = 1; y_count <= part_3; y_count++)
		{
			printf("%i: %i %i\n", i+1, x_count, size_labyrinth_length-2-y_count);
			if (labyrinth[(x_count*size_labyrinth_length)+(size_labyrinth_length-2-y_count)] != RIVAL)
				distance=1;
			else
			{
				return 0; //distance == 0
			}
		}
		//if (!distance) //distance уже равно "0", выход из функции строкой выше
			//continue;
		//x==1 || x===size_labyrinth_width-2; y--
		part_1=max_distance, part_2=0, part_3=0;
		if (y-max_distance < 1)
		{
			part_1=y-1;
			part_2=max_distance-part_1;
		}
		if (part_2 >= size_labyrinth_width-2)
		{
			part_2=size_labyrinth_width-3;
			part_3=max_distance-part_1-part_2;
		}
		for (int y_count = 1; y_count <= part_1; y_count++)
		{
			printf("%i: %i %i\n", i+1, x, y-y_count);
			if (labyrinth[((x)*size_labyrinth_length)+(y-y_count)] != RIVAL)
				distance=1;
			else
			{
				return 0; //distance == 0
			}
		}
		//if (!distance) //distance уже равно "0", выход из функции строкой выше
			//continue;
		//~ Если x==size_labyrinth_width-1, то его значение будет уменьшаться при каждом проходе, иначе - увеличиваться
		//Если x==1, то его значение будет увеличиваться при каждом проходе, иначе - уменьшаться
		for (int y_count = 1; y_count <= part_2; y_count++)
		{
			if (x == 1)
				x_count=y_count;
			else
				x_count=-y_count;
			printf("%i: %i %i\n", i+1, x+x_count, 1);
			if (labyrinth[((x+x_count)*size_labyrinth_length)+1] != RIVAL)
				distance=1;
			else
			{
				return 0; //distance == 0
			}
		}
		//if (!distance) //distance уже равно "0", выход из функции строкой выше
			//continue;
		//Если при проходе part_2 x увеличивался, то x_count=size_labyrinth_width-2,
		//если уменьшался - то x_count=1
		if (x_count > 0)
			x_count=size_labyrinth_width-2;
		else
			x_count=1;
		for (int y_count = 1; y_count <= part_3; y_count++)
		{
			printf("%i: %i %i\n", i+1, x_count, y_count+1);
			if (labyrinth[(x_count*size_labyrinth_length)+(y_count+1)] != RIVAL)
				distance=1;
			else
			{
				return 0; //distance == 0
			}
		}
		//if (!distance) //distance уже равно "0", выход из функции строкой выше
			//continue;
	}
	if ((y == 1 || y == size_labyrinth_length-2) && (distance))
	{
		//y==1 || y===size_labyrinth_length-2; x++
		part_1=max_distance, part_2=0, part_3=0;
		if (x+max_distance >= size_labyrinth_width-2)
		{
			part_1=size_labyrinth_width-x-2;
			part_2=max_distance-part_1;
		}
		if (part_2 >= size_labyrinth_length-2)
		{
			part_2=size_labyrinth_length-3;
			part_3=max_distance-part_1-part_2;
		}
		for (int x_count = 1; x_count <= part_1; x_count++)
		{
			printf("%i: %i %i\n", i+1, x+x_count, y);
			if (labyrinth[((x+x_count)*size_labyrinth_length)+y] != RIVAL)
				distance=1;
			else
			{
				return 0; //distance == 0
			}
		}
		//if (!distance) //distance уже равно "0", выход из функции строкой выше
			//continue;
		int y_count=0;
		//~ Если y==size_labyrinth_length-1, то его значение будет уменьшаться при каждом проходе, иначе - увеличиваться
		//Если y==1, то его значение будет увеличиваться при каждом проходе, иначе - уменьшаться
		for (int x_count = 1; x_count <= part_2; x_count++)
		{
			if (y == 1)
				y_count=x_count+1;
			else
				y_count=size_labyrinth_length-2-x_count;
			printf("%i: %i %i\n", i+1, size_labyrinth_width-2, y_count);
			if (labyrinth[((size_labyrinth_width-2)*size_labyrinth_length)+(y_count)] != RIVAL)
				distance=1;
			else
			{
				return 0; //distance == 0
			}
		}
		//if (!distance) //distance уже равно "0", выход из функции строкой выше
			//continue;
		for (int x_count = 1; x_count <= part_3; x_count++)
		{
			//Если при проходе part_2 y увеличивался, то y_count должен быть равен size_labyrinth_length-2,
			//если уменьшался - то y_count=1
			//~ if (y_count > 0)
				//~ y_count=size_labyrinth_length-1;
			//~ else
				//~ y_count=0;
			printf("%i: %i %i\n", i+1, size_labyrinth_width-2-x_count, y_count);
			if (labyrinth[((size_labyrinth_width-2-x_count)*size_labyrinth_length)+(y_count)] != RIVAL)
				distance=1;
			else
			{
				return 0; //distance == 0
			}
		}
		//if (!distance) //distance уже равно "0", выход из функции строкой выше
			//continue;
		//y==1 || y===size_labyrinth_length-2; x--
		part_1=max_distance, part_2=0, part_3=0;
		if (x-max_distance < 1)
		{
			part_1=x-1;
			part_2=max_distance-part_1;
		}
		if (part_2 >= size_labyrinth_length-2)
		{
			part_2=size_labyrinth_length-3;
			part_3=max_distance-part_1-part_2;
		}
		for (int x_count = 1; x_count <= part_1; x_count++)
		{
			printf("%i: %i %i\n", i+1, x-x_count, y);
			if (labyrinth[((x-x_count)*size_labyrinth_length)+(y)] != RIVAL)
				distance=1;
			else
			{
				return 0; //distance == 0
			}
		}
		//if (!distance) //distance уже равно "0", выход из функции строкой выше
			//continue;
		//~ Если y==size_labyrinth_length-1, то его значение будет уменьшаться при каждом проходе, иначе - увеличиваться
		//Если y==1, то его значение будет увеличиваться при каждом проходе, иначе - уменьшаться
		for (int x_count = 1; x_count <= part_2; x_count++)
		{
			if (y == 1)
				y_count=x_count+1;
			else
				y_count=size_labyrinth_length-2-x_count;
			printf("%i: %i %i\n", i+1, 1, y_count);
			if (labyrinth[(size_labyrinth_length)+(y_count)] != RIVAL)
				distance=1;
			else
			{
				return 0; //distance == 0
			}
		}
		//if (!distance) //distance уже равно "0", выход из функции строкой выше
			//continue;
		for (int x_count = 1; x_count <= part_3; x_count++)
		{
			//Если при проходе part_2 y увеличивался, то y_count должен быть равен size_labyrinth_length-2,
			//если уменьшался - то y_count=1
			printf("%i: %i %i\n", i+1, x_count+1, y_count);
			if (labyrinth[((x_count+1)*size_labyrinth_length)+(y_count)] != RIVAL)
				distance=1;
			else
			{
				return 0; //distance == 0
			}
		}
	}
	return distance;
}

//Првоверка клеток, окружающих заданную ячейку
//Проверка производится, начиная сверху, по часовой стрелке; возможно задать только некоторые ячейки для проверки,
//для этого необходимо передать в параметрах требуемую маску direction_mask,
//где "1" (2^0) будет означать ячейку выше заданной, а "128" (2^7) - ячейку выше и левее заданной:
// 8 1 2 (порядок проходов)  7 0 1 (маски направлений по степеням двойки)   128  1  2 (значения масок направлений в десятичной системе)
// 7 x 3                     6 x 2                                           64  x  4
// 6 5 4                     5 4 3                                           32 16  8
//Проверка может проводиться на произвольную дистанцию от заданной клетки, от ближних ячеек к более дальним
//Если в проверяемых ячейках будет обнаружено что-то, кроме заданного в параметрах
//(через массив cell_types) - производится выход из функции
//FIXME: сейчас производится проверка на всё, кроме искомого, что может сказываться на производительности
int check_in_neighbor_cells_around(int x, int y, unsigned short int const direction_mask, int depth, short int *cell_types, short int const size_labyrinth_length, short int const size_labyrinth_width, int const *labyrinth) //Возможно, будет лучше заменить short int на char?
{
	int distance=1;
	for (int depth_count = 1; depth_count <= depth; depth_count++)
	{
		for (int direction_count = 1; direction_count <= 128; direction_count=direction_count*2)
		{
			for (int cell_type=0; cell_type < END_OF_CT_ENUM; cell_type++)
			{
				if (!cell_types[cell_type])
				{
					switch (direction_mask&direction_count) //говорят, операции с битами медленные, возможно, стоит поискать другой способ
					{
						case 1:
							if (x-depth_count > 0)
							{
								//if (labyrinth[(x-depth_count)*size_labyrinth_length+y] != CELL && labyrinth[(x-depth_count)*size_labyrinth_length+y] != WALL)
								if (labyrinth[(x-depth_count)*size_labyrinth_length+y] == cell_type)
								{
									distance=0;
									return distance;
								}
							}
							break;
						case 2:
							if (x-depth_count > 0 && y+depth_count < size_labyrinth_length-1)
							{
								//if (labyrinth[(x-depth_count)*size_labyrinth_length+(y+depth_count)] != CELL && labyrinth[(x-depth_count)*size_labyrinth_length+(y+depth_count)] != WALL)
								if (labyrinth[(x-depth_count)*size_labyrinth_length+(y+depth_count)] == cell_type)
								{
									distance=0;
									return distance;
								}
							}
							if (depth_count > 1)
							{
								for (int i = 1; i < depth_count; i++)
								{
									if (x-depth_count > 0 && y+i < size_labyrinth_length-1)
									{
										//if (labyrinth[(x-depth_count)*size_labyrinth_length+(y+i)] != CELL && labyrinth[(x-depth_count)*size_labyrinth_length+(y+i)] != WALL)
										if (labyrinth[(x-depth_count)*size_labyrinth_length+(y+i)] == cell_type)
										{
											distance=0;
											return distance;
										}
									}
									if (x-i > 0 && y+depth_count < size_labyrinth_length-1)
									{
										//if (labyrinth[(x-i)*size_labyrinth_length+(y+depth_count)] != CELL && labyrinth[(x-i)*size_labyrinth_length+(y+depth_count)] != WALL)
										if (labyrinth[(x-i)*size_labyrinth_length+(y+depth_count)] == cell_type)
										{
											distance=0;
											return distance;
										}
									}
								}
							}
							break;
						case 4:
							if (y+depth_count < size_labyrinth_length-1)
							{
								//if (labyrinth[x*size_labyrinth_length+(y+depth_count)] != CELL && labyrinth[x*size_labyrinth_length+(y+depth_count)] != WALL)
								if (labyrinth[x*size_labyrinth_length+(y+depth_count)] == cell_type)
								{
									distance=0;
									return distance;
								}
							}
							break;
						case 8:
							if (x+depth_count < size_labyrinth_width-1  && y+depth_count < size_labyrinth_length-1)
							{
								//if (labyrinth[(x+depth_count)*size_labyrinth_length+(y+depth_count)] != CELL && labyrinth[(x+depth_count)*size_labyrinth_length+(y+depth_count)] != WALL)
								if (labyrinth[(x+depth_count)*size_labyrinth_length+(y+depth_count)] == cell_type)
								{
									distance=0;
									return distance;
								}
							}
							if (depth_count > 1)
							{
								for (int i = 1; i < depth_count; i++)
								{
									if (x+depth_count < size_labyrinth_width-1  && y+i < size_labyrinth_length-1)
									{
										//if (labyrinth[(x+depth_count)*size_labyrinth_length+(y+i)] != CELL && labyrinth[(x+depth_count)*size_labyrinth_length+(y+i)] != WALL)
										if (labyrinth[(x+depth_count)*size_labyrinth_length+(y+i)] == cell_type)
										{
											distance=0;
											return distance;
										}
									}
									if (x+i < size_labyrinth_width-1  && y+depth_count < size_labyrinth_length-1)
									{
										//if (labyrinth[(x+i)*size_labyrinth_length+(y+depth_count)] != CELL && labyrinth[(x+i)*size_labyrinth_length+(y+depth_count)] != WALL)
										if (labyrinth[(x+i)*size_labyrinth_length+(y+depth_count)] == cell_type)
										{
											distance=0;
											return distance;
										}
									}
								}
							}
							break;
						case 16:
							if (x+depth_count < size_labyrinth_width-1)
							{
								//if (labyrinth[(x+depth_count)*size_labyrinth_length+y] != CELL && labyrinth[(x+depth_count)*size_labyrinth_length+y] != WALL)
								if (labyrinth[(x+depth_count)*size_labyrinth_length+y] == cell_type)
								{
									distance=0;
									return distance;
								}
							}
							break;
						case 32:
							if (x+depth_count < size_labyrinth_width-1 && y-depth_count > 0)
							{
								//if (labyrinth[(x+depth_count)*size_labyrinth_length+(y-depth_count)] != CELL && labyrinth[(x+depth_count)*size_labyrinth_length+(y-depth_count)] != WALL)
								if (labyrinth[(x+depth_count)*size_labyrinth_length+(y-depth_count)] == cell_type)
								{
									distance=0;
									return distance;
								}
							}
							if (depth_count > 1)
							{
								for (int i = 1; i < depth_count; i++)
								{
									if (x+depth_count < size_labyrinth_width-1 && y-i > 0)
									{
										//if (labyrinth[(x+depth_count)*size_labyrinth_length+(y-i)] != CELL && labyrinth[(x+depth_count)*size_labyrinth_length+(y-i)] != WALL)
										if (labyrinth[(x+depth_count)*size_labyrinth_length+(y-i)] == cell_type)
										{
											distance=0;
											return distance;
										}
									}
									if (x+i < size_labyrinth_width-1 && y-depth_count > 0)
									{
										//if (labyrinth[(x+i)*size_labyrinth_length+(y-depth_count)] != CELL && labyrinth[(x+i)*size_labyrinth_length+(y-depth_count)] != WALL)
										if (labyrinth[(x+i)*size_labyrinth_length+(y-depth_count)] == cell_type)
										{
											distance=0;
											return distance;
										}
									}
								}
							}
							break;
						case 64:
							if (y-depth_count > 0)
							{
								//if (labyrinth[x*size_labyrinth_length+(y-depth_count)] != CELL && labyrinth[x*size_labyrinth_length+(y-depth_count)] != WALL)
								if (labyrinth[x*size_labyrinth_length+(y-depth_count)] == cell_type)
								{
									distance=0;
									return distance;
								}
							}
							break;
						case 128:
							if (x-depth_count > 0 && y-depth_count > 0)
							{
								//if (labyrinth[(x-depth_count)*size_labyrinth_length+(y-depth_count)] != CELL && labyrinth[(x-depth_count)*size_labyrinth_length+(y-depth_count)] != WALL)
								if (labyrinth[(x-depth_count)*size_labyrinth_length+(y-depth_count)] == cell_type)
								{
									distance=0;
									return distance;
								}
							}
							if (depth_count > 1)
							{
								for (int i = 1; i < depth_count; i++)
								{
									if (x-depth_count > 0 && y-i > 0)
									{
										//if (labyrinth[(x-depth_count)*size_labyrinth_length+(y-i)] != CELL && labyrinth[(x-depth_count)*size_labyrinth_length+(y-i)] != WALL)
										if (labyrinth[(x-depth_count)*size_labyrinth_length+(y-i)] == cell_type)
										{
											distance=0;
											return distance;
										}
									}
									if (x-i > 0 && y-depth_count > 0)
									{
										//if (labyrinth[(x-i)*size_labyrinth_length+(y-depth_count)] != CELL && labyrinth[(x-i)*size_labyrinth_length+(y-depth_count)] != WALL)
										if (labyrinth[(x-i)*size_labyrinth_length+(y-depth_count)] == cell_type)
										{
											distance=0;
											return distance;
										}
									}
								}
							}
							break;
					}
				}
			}
		}
	}
	return distance;
}

//Проверка на события в ячейке, в которую попал участник
//с соответствующим изменением состояния участника и ячейки (где требуется)
//В случае, если игрок найдёт клад, обманный клад или арсенал, имея при этом что-то из перечисленного -
//ему будет выдан запрос на обмен (так как одновременно можно нести только один из вышеперечисленных предметов)
//В случае согласия - игрок "поднимает" содержимое ячейки в руки, оставляя в ячейке то, что у него было
//(кроме оружия оно может быть получено только из Арсенала, соответственно, при выбрасывании в данной ячейке не появляется (изчезает))

//FIXME: проверить необходимсть наличия обращениея к screen (возможно, понадобится для вывода сообщений в дальнейшем)

void checking_for_events(SDL_Window *main_window, SDL_Surface *screen, int const coordinate, struct players player[], int *labyrinth, short int const size_labyrinth_length, short int const holes, short int const *holes_array)
{
	char drop_take_string[80];
	SDL_MessageBoxButtonData drop_take[] = { { 0, 0, "Yes" }, { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 1, "No" }, };
	SDL_MessageBoxData message_drop_take={SDL_MESSAGEBOX_WARNING , main_window, "", drop_take_string, 2, drop_take, NULL};
	int button_id;
	switch (labyrinth[coordinate])
	{
		case TREASURE:
			if (player[0].has_fake_treasure)
			{
				strcpy(drop_take_string, "Throw out treasure and pick up another?");
				if (SDL_ShowMessageBox(&message_drop_take, &button_id) < 0)
				{
			        SDL_Log("Error displaying message box");
			        return;
			    }
			    SDL_SetModState(KMOD_NONE);
				if (button_id == -1)
				{
			        SDL_Log("no selection");
			    }
			    else
			    {
			        if (!button_id)
			        {
						player[0].has_fake_treasure=0;
						player[0].has_treasure=1;
						labyrinth[coordinate]=FAKE_TREASURE;
						break;
					}
					else
						break;
				}
			}
			if (player[0].has_weapon)
			{
				strcpy(drop_take_string, "Throw out weapon and pick up treasure?");
				if (SDL_ShowMessageBox(&message_drop_take, &button_id) < 0)
				{
			        SDL_Log("Error displaying message box");
			        return;
			    }
			    SDL_SetModState(KMOD_NONE);
				if (button_id == -1)
				{
			        SDL_Log("no selection");
			    }
			    else
			    {
			        if (!button_id)
			        {
						player[0].has_weapon=0;
						player[0].has_treasure=1;
						labyrinth[coordinate]=CELL;
						break;
					}
					else
						break;
				}
			}
			player[0].has_treasure=1;
			labyrinth[coordinate]=CELL;
			break;
		case FAKE_TREASURE:
			if (player[0].has_treasure)
			{
				strcpy(drop_take_string, "Throw out treasure and pick up another?");
				if (SDL_ShowMessageBox(&message_drop_take, &button_id) < 0)
				{
			        SDL_Log("Error displaying message box");
			        return;
			    }
			    SDL_SetModState(KMOD_NONE);
				if (button_id == -1)
				{
			        SDL_Log("no selection");
			    }
			    else
			    {
			        if (!button_id)
			        {
						player[0].has_treasure=0;
						player[0].has_fake_treasure=1;
						labyrinth[coordinate]=TREASURE;
						break;
					}
					else
						break;
				}
			}
			if (player[0].has_weapon)
			{
				strcpy(drop_take_string, "Throw out weapon and pick up treasure?");
				if (SDL_ShowMessageBox(&message_drop_take, &button_id) < 0)
				{
			        SDL_Log("Error displaying message box");
			        return;
			    }
			    SDL_SetModState(KMOD_NONE);
				if (button_id == -1)
				{
			        SDL_Log("no selection");
			    }
			    else
			    {
			        if (!button_id)
			        {
						player[0].has_weapon=0;
						player[0].has_fake_treasure=1;
						labyrinth[coordinate]=CELL;
						break;
					}
					else
						break;
				}
			}
			player[0].has_fake_treasure=1;
			labyrinth[coordinate]=CELL;
			break;
		case HOLE:
			player[0].in_hole=1;
			//player[0].trap_start=time(NULL);
			player[0].trap_start=SDL_GetTicks();
			
			/*//Попытка плавного затухания экрана ("спецэффекты") - SDL 1.2
			int win_width;
			int win_height;
			SDL_GetWindowSize(main_window, &win_width, &win_height);
			SDL_Surface *black_rect;
			if ( (black_rect=SDL_CreateRGBSurface(0, win_width, win_height, 32, 0, 0, 0, 0)) == NULL)
				SDL_Log("SDL_CreateRGBSurface(black_rect) failed: %s", SDL_GetError());
			SDL_SetSurfaceBlendMode(black_rect, SDL_BLENDMODE_BLEND);
			Uint32 ticks=SDL_GetTicks();
			//while (SDL_GetTicks()-player[0].trap_start < TIME_BETWEEN_HOLES/2*1000)
			while (ticks-player[0].trap_start < TIME_BETWEEN_HOLES/2)
			{
				SDL_SetSurfaceAlphaMod(black_rect, (SDL_GetTicks()-player[0].trap_start)*255/(TIME_BETWEEN_HOLES/2));
				printf("%i\n", (SDL_GetTicks()-player[0].trap_start)*255/(TIME_BETWEEN_HOLES/2));
				SDL_BlitSurface(black_rect, NULL, screen, NULL);
				SDL_UpdateWindowSurface(main_window);
				ticks=SDL_GetTicks();
			}
			free(black_rect);
			black_rect=NULL;*/
			
			//Если дыр больше одной - при попадании в одну из них, он перемещается в следующую
			//(по массиву дыр) или в первую, если текущая была последней в массиве
			//TODO: реализовать выбор между связанными (связанные пары) и последовательными (все подряд, друг за другом) дырами
			if (holes > 1)
			{
				for (int i = 0; i < holes; i++)
				{
					if (holes_array[i] == coordinate)
					{
						if (i == holes-1)
							i=-1;
						player[0].x=holes_array[i+1]/(size_labyrinth_length+1);
						player[0].y=(player[0].x*size_labyrinth_length-holes_array[i+1])*(-1);
						//Коррекция подсчёта из-за проблем с округлением
						if (player[0].y > size_labyrinth_length)
						{
							player[0].x++;
							player[0].y=player[0].y-size_labyrinth_length;
						}
						break;
						//x=c/(l+1)
						//y=(x*l-c)*(-1)
					}
				}
				
			}
			//Если присутствует только одна дыра - игрок будет перемещён в случайное место лабиринта
			//Если удастся это реализовать без особых накладных расходов
			//else
				
			break;
		case TRAP:
			player[0].in_trap=1;
			//player[0].trap_start=time(NULL);
			player[0].trap_start=SDL_GetTicks();
			break;
		case HOSPITAL:
			if (player[0].is_wounded)
				player[0].is_wounded=0;
			break;
		case ARSENAL:
			if (player[0].has_treasure || player[0].has_fake_treasure)
			{
				strcpy(drop_take_string, "Throw out treasure and pick up weapon?");
				if (SDL_ShowMessageBox(&message_drop_take, &button_id) < 0)
				{
			        SDL_Log("Error displaying message box");
			        return;
			    }
			    SDL_SetModState(KMOD_NONE);
				if (button_id == -1)
				{
			        SDL_Log("no selection");
			    }
			    else
			    {
			        if (!button_id)
			        {
						//FIXME: при смене сокровища на оружие Арсенал пропадает из лабиринта
						if (player[0].has_treasure)
						{
							labyrinth[coordinate]=TREASURE;
							player[0].has_treasure=0;
							player[0].has_weapon=1;
						}
						if (player[0].has_fake_treasure)
						{
							labyrinth[coordinate]=FAKE_TREASURE;
							player[0].has_fake_treasure=0;
							player[0].has_weapon=1;
						}
						break;
					}
					else
						break;
				}
			}
			player[0].has_weapon=1;
			break;
		default:
			break;
	}
	return;
}
