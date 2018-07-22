/*  includes_macros.h

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

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
//#include "SDL_image.h"
//#include 
//#include 

#define DEFAULT_LENGTH 11 //FIXME: алгоритм генерации корректно работает только с нечётными числами
#define DEFAULT_WIDTH 11 //FIXME: алгоритм генерации корректно работает только с нечётными числами
#define DEFAULT_MAX_LENGTH 1000
#define DEFAULT_MAX_WIDTH 1000
#define DEFAULT_MIN_LENGTH 5
#define DEFAULT_MIN_WIDTH 5
#define RIVALS 4 //включая игрока!
#define HOLES 2 //количество дыр в лабиринте по умолчанию
//TODO: перенести в настройки
#define TIME_IN_TRAP 3000 //время, проведённое в ловушке (в миллисекундах)
#define TIME_BETWEEN_HOLES 3000 //время, требуемое для перемещения между дырами (в миллисекундах)
#define SPEED_MOVE 250 //время в миллисекундах, требуемое для совершения одного шага (продвижения на 1 ячейку)
#define SPEED_TURN 200 //время в миллисекундах, требуемое для поворота

struct players{
	int type; //0 - CPU, 1 - игрок
	int x; //координата по вертикали
	int y; //координата по горизонтали
	int direction; //направление, в котором смотрит участник (0 - вверх, 1 - вправо, 2 - вниз, 3 - влево)
	int previous_cell_state; //значение ячейки до попадания туда участником
	//Флаги или переменные?
	short int has_treasure; //взял ли участник клад
	short int has_fake_treasure; //взял ли участник ложный клад
	short int has_weapon; //взял ли участник оружие из арсенала
	short int is_wounded; //не ранен ли участник
	short int in_hole; //не попал ли участник в дыру
	short int in_trap; //не попал ли участник в ловушку
	time_t trap_start; //когда участник попал в ловушку
	//Uint32 step_start; //время начала хода участника
	//unsigned int32_t step_start; //время начала хода участника
	unsigned int step_start;
	//int state; //has treasure | has Ftreasure | in hole (?) | in trap | is wounded
	//
};

//Типы клеток и событий в них:
//0 - пустая клетка, 1 - стена, 2 - будущая пустая клетка, 3 - один из соперников, 4 - игрок, 5 - выход
//6 - клад, 7 - ложный клад, 8 - дыра, 9 - ловушка, 10 - госпиталь, 11 - арсенал,
//12 - конец данного перечисления (необходимость под вопросом, пока используется в циклах расстановки событий)
enum CellType { CELL, WALL, DUMMY, RIVAL, PLAYER, EXIT, TREASURE, FAKE_TREASURE, HOLE, TRAP, HOSPITAL, ARSENAL, END_OF_CT_ENUM };
//Перечисление возможных направлений (необходимость под вопросом, только для удобства чтения кода)
//0 - вверх, 1 - вправо, 2 - вниз, 3 - влево
enum Direction { UP, RIGHT, DOWN, LEFT };
//Время суток, задающее начало периода, с которого оно начинается; используется для определения версии изображения выхода из лабиринта
enum TimeOfDay { MORNING=6, DAY=10, EVENING=18, NIGHT=21 };
