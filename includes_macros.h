/*  includes_macros.h

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

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
//#include 
//#include 

#define DEFAULT_LENGTH 11 //FIXME: алгоритм генерации корректно работает только с нечётными числами
#define DEFAULT_WIDTH 11 //FIXME: алгоритм генерации корректно работает только с нечётными числами
#define DEFAULT_MAX_LENGTH 1000
#define DEFAULT_MAX_WIDTH 1000
#define DEFAULT_MIN_LENGTH 5
#define DEFAULT_MIN_WIDTH 5
#define RIVALS 4 //включая игрока!

struct players{
	int type; //0 - CPU, 1 - игрок
	int x; //координата по вертикали
	int y; //координата по горизонтали
	int direction; //направление, в котором смотрит участник (0 - вверх, 1 - вправо, 2 - вниз, 3 - влево)
	//
};

//Типы клеток и событий в них:
//0 - пустая клетка, 1 - стена, 2 - будущая пустая клетка, 3 - один из соперников, 4 - игрок, 5 - выход
enum CellType { CELL, WALL, DUMMY, RIVAL, PLAYER, EXIT, TREASURE, FAKE_TREASURE, HOLE, TRAP, HOSPITAL, ARSENAL };
//Перечисление возможных направлений (необходимость под вопросом, только для удобства чтения кода)
//0 - вверх, 1 - вправо, 2 - вниз, 3 - влево
enum Direction { UP, RIGHT, DOWN, LEFT };
