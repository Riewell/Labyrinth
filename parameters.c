/*  parameters.c

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

short int parameters_check(const char *parameter_name, const char *parameter_value, const short int default_value, const short int *parameter);

//~ int parameters(char *args[], int count, short int *length, short int *width, short int *visual, short int *no_walls_removing, short int *show_result, short int *num_holes)
int parameters(char *args[], int count, short int *window_height, short int *window_width, short int *fullscreen, short int *refresh_rate, short int *fps, short int *hw_accel, short int *vsync, short int *sdl_12, short int *length, short int *width, short int *no_walls_removing, short int *rivals, short int *num_holes, short int *trap_time, short int *hole_time, short int *speed, short int *turn_speed, short int *debug, short int *visual, short int *show_result)
{
	//В случае задания взаимоисключающих параметров или их дублирования
	//будет учтён последний указанный параметр/последнее значение
	int parameter_count=1;
	while (parameter_count < count)
	{
		int result=0;
		//VIDEO
		//Размеры окна будут проверены и при необходимости изменены при инициализации SDL
		if (!strcmp("-H", args[parameter_count]) || !strcmp("--screen-height", args[parameter_count]))
		{
			//Проверка наличия непосредственно значения высоты окна, если оно отсутствует - переход к следующему параметру
			if ((parameter_count+1) >= count)
			{
				printf("Параметр '%s' пуст. ", args[parameter_count]);
				if (*window_height == DEFAULT_RES_Y)
					printf("Установлено значение по умолчанию: %i.\n", DEFAULT_RES_Y);
				else
					printf("Значение не изменено (%hi).\n", *window_height);
				parameter_count++;
				continue;
			}
			result=parameters_check(args[parameter_count], args[parameter_count+1], DEFAULT_RES_Y, window_height);
			if (result == -2)
			{
				puts("Допустимы только положительные целочисленные значения больше нуля.");
				parameter_count++;
				continue;
			}
			//Любая сторона окна не может быть отрицательной или нулевой величиной
			if (result < 1)
			{
				printf("Значение '%s' параметра '%s' должно быть больше нуля. ", args[parameter_count+1], args[parameter_count]);
				if (*window_height == DEFAULT_RES_Y)
					printf("Установлено значение по умолчанию: %i.\n", DEFAULT_RES_Y);
				else
					printf("Значение не изменено (%hi).\n", *window_height);
				parameter_count++;
				continue;
			}
			*window_height=result;
			parameter_count++;
		}
		//Размеры окна будут проверены и при необходимости изменены при инициализации SDL
		else if (!strcmp("-W", args[parameter_count]) || !strcmp("--screen-width", args[parameter_count]))
		{
			//Проверка наличия непосредственно значения высоты окна, если оно отсутствует - переход к следующему параметру
			if ((parameter_count+1) >= count)
			{
				printf("Параметр '%s' пуст. ", args[parameter_count]);
				if (*window_width == DEFAULT_RES_X)
					printf("Установлено значение по умолчанию: %i.\n", DEFAULT_RES_X);
				else
					printf("Значение не изменено (%hi).\n", *window_width);
				parameter_count++;
				continue;
			}
			result=parameters_check(args[parameter_count], args[parameter_count+1], DEFAULT_RES_X, window_width);
			if (result == -2)
			{
				puts("Допустимы только положительные целочисленные значения больше нуля.");
				parameter_count++;
				continue;
			}
			//Любая сторона окна не может быть отрицательной или нулевой величиной
			if (result < 1)
			{
				printf("Значение '%s' параметра '%s' должно быть больше нуля. ", args[parameter_count+1], args[parameter_count]);
				if (*window_width == DEFAULT_RES_X)
					printf("Установлено значение по умолчанию: %i.\n", DEFAULT_RES_X);
				else
					printf("Значение не изменено (%hi).\n", *window_width);
				parameter_count++;
				continue;
			}
			*window_width=result;
			parameter_count++;
		}
		//Если указан параметр полноэкранного режима - будет произведена попытка создания окна
		//в полноэкранном режиме при заданном разрешении, при необходимости параметры будут изменены после инициализации SDL
		else if (!strcmp("-F", args[parameter_count]) || !strcmp("--fullscreen", args[parameter_count]))
		{
			*fullscreen=1;
			parameter_count++;
		}
		//На данный момент значение FPS влияет только на возможное ускорение/замедление игры (внутренних расчётов)
		//Значение "-1" означает подстройку данного параметра под частоту обновления экрана (refresh rate)
		else if (!strcmp("-f", args[parameter_count]) || !strcmp("--fps", args[parameter_count]))
		{
			//Проверка наличия непосредственно значения высоты окна, если оно отсутствует - переход к следующему параметру
			if ((parameter_count+1) >= count)
			{
				printf("Параметр '%s' пуст. ", args[parameter_count]);
				if (*fps == FPS)
					printf("Установлено значение по умолчанию: %i.\n", FPS);
				else
					printf("Значение не изменено (%hi).\n", *fps);
				parameter_count++;
				continue;
			}
			result=parameters_check(args[parameter_count], args[parameter_count+1], FPS, fps);
			if (result == -2)
			{
				puts("Допустимы только положительные целочисленные значения больше нуля (или -1 для подстройки значения к частоте обновления экрана).");
				parameter_count++;
				continue;
			}
			//Значение FPS не может быть меньше "1"
			//(кроме значения "-1", которое показывает необходимость в подстройке к частоте обновления экрана (refresh rate)
			if (!result || result < -1)
			{
				printf("Значение '%s' параметра '%s' должно быть больше нуля или равно \'-1\'. ", args[parameter_count+1], args[parameter_count]);
				if (*fps == FPS)
					printf("Установлено значение по умолчанию: %i.\n", FPS);
				else
					printf("Значение не изменено (%hi).\n", *fps);
				parameter_count++;
				continue;
			}
			*fps=result;
			parameter_count++;
		}
		//Значение "Hardware_Acceleration" указывает на необходимость
		//инициализации SDL с задействованием аппаратного ускорения
		else if (!strcmp("-a", args[parameter_count]) || !strcmp("--hw-accel", args[parameter_count]))
		{
			//Проверка наличия непосредственно значения высоты окна, если оно отсутствует - переход к следующему параметру
			if ((parameter_count+1) >= count)
			{
				printf("Параметр '%s' пуст. Значение не изменено (%hi).\n", args[parameter_count], *hw_accel);
				parameter_count++;
				continue;
			}
			//Числовые значения не всегда удобны - даём пользователю возможность
			//использовать человекочитаемые значения параметра
			if (!strncmp("y", args[parameter_count+1], 1) || !strncmp("Y", args[parameter_count+1], 1) || !strncmp("on", args[parameter_count+1], 2) || !strncmp("ON", args[parameter_count+1], 2) ||!strncmp("On", args[parameter_count+1], 2))
			{
				//Значение "-2" означает, что у параметра нет значения по умолчанию
				result=parameters_check(args[parameter_count], "1", -2, hw_accel);
			}
			else if (!strncmp("n", args[parameter_count+1], 1) || !strncmp("N", args[parameter_count+1], 1) || !strncmp("off", args[parameter_count+1], 3) || !strncmp("OFF", args[parameter_count+1], 3) ||!strncmp("Off", args[parameter_count+1], 3))
			{
				//Значение "-2" означает, что у параметра нет значения по умолчанию
				result=parameters_check(args[parameter_count], "0", -2, hw_accel);
			}
			else
			{
				//Значение "-2" означает, что у параметра нет значения по умолчанию
				result=parameters_check(args[parameter_count], args[parameter_count+1], -2, hw_accel);
			}
			if (result == -2)
			{
				puts("Допустимы только значения '1' и '0' (без кавычек).");
				parameter_count++;
				continue;
			}
			if (result != 0 && result != 1)
			{
				printf("Значение параметра '%s' должно быть равно \"0\" или \"1\". Значение не изменено (%hi).\n", args[parameter_count], *hw_accel);
				parameter_count++;
				continue;
			}
			//Этот параметр несовместим с параметром "sdl12", который используются в API SDL 1.2
			//Если значение последнего будет равно "1",
			//и значение параметра "hw_accel" также будет установлено в "1" -
			//последний имеет приоритет, значения несовместимого параметра будет сброшено в состояние "0"
			if (*sdl_12)
			{
				printf("Параметр '%s' несовместим с параметром 'sdl12' и настройкой 'SDL_12_style'. Значение параметра 'sdl12' изменено (0)\n", args[parameter_count]);
				*sdl_12=0;
			}
			*hw_accel=result;
			parameter_count++;
		}
		//Значение "VSync" указывает на необходимость инициализации SDL
		//с задействованием режима синхронизации вертикального обновления экрана
		else if (!strcmp("-v", args[parameter_count]) || !strcmp("--vsync", args[parameter_count]))
		{
			//Проверка наличия непосредственно значения высоты окна, если оно отсутствует - переход к следующему параметру
			if ((parameter_count+1) >= count)
			{
				printf("Параметр '%s' пуст. Значение не изменено (%hi).\n", args[parameter_count], *vsync);
				parameter_count++;
				continue;
			}
			//Числовые значения не всегда удобны - даём пользователю возможность
			//использовать человекочитаемые значения параметра
			if (!strncmp("y", args[parameter_count+1], 1) || !strncmp("Y", args[parameter_count+1], 1) || !strncmp("on", args[parameter_count+1], 2) || !strncmp("ON", args[parameter_count+1], 2) ||!strncmp("On", args[parameter_count+1], 2))
			{
				//Значение "-2" означает, что у параметра нет значения по умолчанию
				result=parameters_check(args[parameter_count], "1", -2, vsync);
			}
			else if (!strncmp("n", args[parameter_count+1], 1) || !strncmp("N", args[parameter_count+1], 1) || !strncmp("off", args[parameter_count+1], 3) || !strncmp("OFF", args[parameter_count+1], 3) ||!strncmp("Off", args[parameter_count+1], 3))
			{
				//Значение "-2" означает, что у параметра нет значения по умолчанию
				result=parameters_check(args[parameter_count], "0", -2, vsync);
			}
			else
			{
				//Значение "-2" означает, что у параметра нет значения по умолчанию
				result=parameters_check(args[parameter_count], args[parameter_count+1], -2, vsync);
			}
			if (result == -2)
			{
				puts("Допустимы только значения '1' и '0' (без кавычек).");
				parameter_count++;
				continue;
			}
			if (result != 0 && result != 1)
			{
				printf("Значение параметра '%s' должно быть равно \"0\" или \"1\". Значение не изменено (%hi).\n", args[parameter_count], *vsync);
				parameter_count++;
				continue;
			}
			//Этот параметр несовместим с параметром "sdl12", который используются в API SDL 1.2
			//Если значение последнего будет равно "1",
			//и значение параметра "vsync" также будет установлено в "1" -
			//последний имеет приоритет, значения несовместимого параметра будет сброшено в состояние "0"
			if (*sdl_12)
			{
				printf("Параметр '%s' несовместим с параметром 'sdl12' и настройкой 'SDL_12_style'. Значение параметра 'sdl12' изменено (0)\n", args[parameter_count]);
				*sdl_12=0;
			}
			*vsync=result;
			parameter_count++;
		}
		//Параметр "sdl12" указывает на необходимость использования программного ускорения с помощью API SDL 1.2
		//Этот параметр несовместим с параметрами "hw_accel" и "vsync",
		//которые используются в API SDL 2.x
		//Если значение хотя бы одного из них будет равно "1",
		//и значение параметра "sdl12" также будет установлено в "1" -
		//последний имеет приоритет, значения несовместимых параметров будут сброшены в состояние "0"
		else if (!strcmp("--sdl12", args[parameter_count]))
		{
			if (*hw_accel)
			{
				printf("Параметр '%s' несовместим с параметром 'hw_accel' и настройкой 'Hardware_Acceleration'. Значение параметра 'hw_accel' изменено (0)\n", args[parameter_count]);
				*hw_accel=0;
			}
			if (*vsync)
			{
				printf("Параметр '%s' несовместим с параметром 'vsync' и настройкой 'VSync'. Значение параметра 'vsync' изменено (0)\n", args[parameter_count]);
				*vsync=0;
			}
			*sdl_12=1;
			parameter_count++;
		}
		//GAME
		//Проверка, желает ли пользователь установить свою длину лабиринта
		else if (!strcmp("-l", args[parameter_count]) || !strcmp("--length", args[parameter_count]))
		{
			//Проверка наличия непосредственно значения длины, если оно отсутствует - переход к следующему параметру
			if ((parameter_count+1) >= count)
			{
				printf("Параметр '%s' пуст. ", args[parameter_count]);
				if (*length == DEFAULT_LENGTH)
					printf("Установлено значение по умолчанию: %i.\n", DEFAULT_LENGTH);
				else
					printf("Значение не изменено (%hi).\n", *length);
				parameter_count++;
				continue;
			}
			result=parameters_check(args[parameter_count], args[parameter_count+1], DEFAULT_LENGTH, length);
			if (result == -2)
			{
				printf("Допустимы положительные целочисленные значения в диапазоне от %i до %i включительно.\n", DEFAULT_MIN_LENGTH, DEFAULT_MAX_LENGTH);
				parameter_count++;
				continue;
			}
			//Если значение больше определённого максимально возможного - присваивание максимально возможного значения
			//Если значение меньше определённого минимально возможного - присваивание минимально возможного значения
			if (result < DEFAULT_MIN_LENGTH)
			{
				printf("Значение '%s' параметра '%s' не должно быть меньше минимального значения (%i). ", args[parameter_count+1], args[parameter_count], DEFAULT_MIN_LENGTH);
				if (*length == DEFAULT_MIN_LENGTH+2)
					printf("Значение не изменено (%hi).\n", *length-2);
				else
					printf("Установлено минимальное значение по умолчанию: %i.\n", DEFAULT_MIN_LENGTH);
				result=DEFAULT_MIN_LENGTH;
			}
			if (result > DEFAULT_MAX_LENGTH)
			{
				printf("Значение '%s' параметра '%s' не должно быть больше максимального значения (%i). ", args[parameter_count+1], args[parameter_count], DEFAULT_MAX_LENGTH);
				if (*length == DEFAULT_MAX_LENGTH+2)
					printf("Значение не изменено (%hi).\n", *length-2);
				else
					printf("Установлено максимальное значение по умолчанию: %i.\n", DEFAULT_MAX_LENGTH);
				result=DEFAULT_MAX_LENGTH;
			}
			*length=result+2;
			parameter_count++;
		}
		//Проверка, желает ли пользователь установить свою ширину лабиринта
		else if (!strcmp("-w", args[parameter_count]) || !strcmp("--width", args[parameter_count]))
		{
			//Проверка наличия непосредственно значения ширины, если оно отсутствует - переход к следующему параметру
			if ((parameter_count+1) >= count)
			{
				printf("Параметр '%s' пуст. ", args[parameter_count]);
				if (*width == DEFAULT_WIDTH)
					printf("Установлено значение по умолчанию: %i.\n", DEFAULT_WIDTH);
				else
					printf("Значение не изменено (%hi).\n", *width);
				parameter_count++;
				continue;
			}
			result=parameters_check(args[parameter_count], args[parameter_count+1], DEFAULT_WIDTH, width);
			if (result == -2)
			{
				printf("Допустимы положительные целочисленные значения в диапазоне от %i до %i включительно.\n", DEFAULT_MIN_WIDTH, DEFAULT_MAX_WIDTH);
				parameter_count++;
				continue;
			}
			//Если значение больше определённого максимально возможного - присваивание максимально возможного значения
			//Если значение меньше определённого минимально возможного - присваивание минимально возможного значения
			if (result < DEFAULT_MIN_WIDTH)
			{
				printf("Значение '%s' параметра '%s' не должно быть меньше минимального значения (%i). ", args[parameter_count+1], args[parameter_count], DEFAULT_MIN_WIDTH);
				if (*width == DEFAULT_MIN_WIDTH+2)
					printf("Значение не изменено (%hi).\n", *width-2);
				else
					printf("Установлено минимальное значение по умолчанию: %i.\n", DEFAULT_MIN_WIDTH);
				result=DEFAULT_MIN_WIDTH;
			}
			if (result > DEFAULT_MAX_WIDTH)
			{
				printf("Значение '%s' параметра '%s' не должно быть больше максимального значения (%i). ", args[parameter_count+1], args[parameter_count], DEFAULT_MAX_WIDTH);
				if (*width == DEFAULT_MAX_WIDTH+2)
					printf("Значение не изменено (%hi).\n", *width-2);
				else
					printf("Установлено максимальное значение по умолчанию: %i.\n", DEFAULT_MAX_WIDTH);
				result=DEFAULT_MAX_WIDTH;
			}
			*width=result+2;
			parameter_count++;
		}
		//Проверка, желает ли пользователь оставить удаление стен в лабиринте для создания цикличного лабиринта
		//(по умолчанию - да), или нет (путём передачи данного параметра)
		//и переход к следующему параметру (если есть)
		else if (!strcmp("-n", args[parameter_count]) || !strcmp("--no-wall-removing", args[parameter_count]))
		{
			*no_walls_removing=1;
			parameter_count++;
		}
		//Параметр "Rivals" указывает общее на количество соперников, участвующих в игре (включая игрока)
		//FIXME: Максимальное количество участников пока не ограничено
		else if (!strcmp("-r", args[parameter_count]) || !strcmp("--rivals", args[parameter_count]))
		{
			//Проверка наличия непосредственно значения количества участников, если оно отсутствует  - переход к следующему параметру
			if ((parameter_count+1) >= count)
			{
				printf("Параметр '%s' пуст. ", args[parameter_count]);
				if (*rivals == RIVALS)
					printf("Установлено значение по умолчанию: %i.\n", RIVALS);
				else
					printf("Значение не изменено (%hi).\n", *rivals);
				parameter_count++;
				continue;
			}
			result=parameters_check(args[parameter_count], args[parameter_count+1], RIVALS, rivals);
			if (result == -2)
			{
				puts("Допустимы только положительные целочисленные значения больше нуля.");
				parameter_count++;
				continue;
			}
			//Количество соперников не может быть отрицательной или нулевой величиной
			//(хотя бы один участник (игрок) будет присутствовать)
			if (result < 1)
			{
				printf("Значение '%s' параметра '%s' должно быть больше нуля. ", args[parameter_count+1], args[parameter_count]);
				if (*rivals == RIVALS)
					printf("Установлено значение по умолчанию: %i.\n", RIVALS);
				else
					printf("Значение не изменено (%hi).\n", *rivals);
				parameter_count++;
				continue;
			}
			*rivals=result;
			parameter_count++;
		}
		//Параметр "Holes" ("pits") указывает на количество дыр в полу
		//TODO: пока все дыры циклически связаны между собой
		//(проверка на чётное количество пока не требуется, однако их должно быть или 0, или минимум 2)
		//FIXME: максимальное количество пока не ограничено!
		//TODO: Пока не найден способ без особых накладных расходов перемещать участника
		//в случайное место лабиринта - количество дыр должно быть или больше 1, или равно 0
		else if (!strcmp("-p", args[parameter_count]) || !strcmp("--pits", args[parameter_count]))
		{
			//Проверка наличия непосредственно значения количества дыр, если оно отсутствует - переход к следующему параметру
			if ((parameter_count+1) >= count)
			{
				printf("Параметр '%s' пуст. ", args[parameter_count]);
				if (*num_holes == HOLES)
					printf("Установлено значение по умолчанию: %i.\n", HOLES);
				else
					printf("Значение не изменено (%hi).\n", *num_holes);
				parameter_count++;
				continue;
			}
			result=parameters_check(args[parameter_count], args[parameter_count+1], HOLES, num_holes);
			if (result == -2)
			{
				puts("Допустимы только положительные целочисленные значения кроме '1'.");
				parameter_count++;
				continue;
			}
			//Количество дыр не может быть отрицательной величиной
			if (result < 0)
			{
				printf("Значение '%s' параметра '%s' должно быть больше '1' или равно нулю. ", args[parameter_count+1], args[parameter_count]);
				if (*num_holes == HOLES)
					printf("Установлено значение по умолчанию: %i.\n", HOLES);
				else
					printf("Значение не изменено (%hi).\n", *num_holes);
				parameter_count++;
				continue;
			}
			*num_holes=result;
			parameter_count++;
		}
		//Время, проведённое в ловушке, указывается в секундах
		else if (!strcmp("-t", args[parameter_count]) || !strcmp("--trap-time", args[parameter_count]))
		{
			//Проверка наличия непосредственно значения времени, если оно отсутствует - переход к следующему параметру
			if ((parameter_count+1) >= count)
			{
				printf("Параметр '%s' пуст. ", args[parameter_count]);
				if (*trap_time == TIME_IN_TRAP)
					printf("Установлено значение по умолчанию: %i.\n", TIME_IN_TRAP);
				else
					printf("Значение не изменено (%hi).\n", *trap_time);
				parameter_count++;
				continue;
			}
			result=parameters_check(args[parameter_count], args[parameter_count+1], TIME_IN_TRAP, trap_time);
			if (result == -2)
			{
				//Ограничение в 30 секунд связано с максимально возможным значением переменной типа short int - 32767
				puts("Допустимы только положительные целочисленные значения в диапазоне между 0 и 30 включительно.");
				parameter_count++;
				continue;
			}
			//Время не может быть отрицательной величиной
			if (result < 0 || result > 30)
			{
				printf("Значение '%s' параметра '%s' должно быть в диапазоне между 0 и 30 включительно. ", args[parameter_count+1], args[parameter_count]);
				if (*trap_time == TIME_IN_TRAP)
					printf("Установлено значение по умолчанию: %i.\n", TIME_IN_TRAP);
				else
					printf("Значение не изменено (%hi).\n", *trap_time);
				parameter_count++;
				continue;
			}
			//Поскольку SDL API воспринимает миллисекунды - производится умножение на 1000
			*trap_time=result*1000;
			parameter_count++;
		}
		//Время, потраченное при перемещении между дырами, указывается в секундах
		else if (!strcmp("-T", args[parameter_count]) || !strcmp("--pit-time", args[parameter_count]))
		{
			//Проверка наличия непосредственно значения времени, если оно отсутствует - переход к следующему параметру
			if ((parameter_count+1) >= count)
			{
				printf("Параметр '%s' пуст. ", args[parameter_count]);
				if (*hole_time == TIME_BETWEEN_HOLES)
					printf("Установлено значение по умолчанию: %i.\n", TIME_BETWEEN_HOLES);
				else
					printf("Значение не изменено (%hi).\n", *hole_time);
				parameter_count++;
				continue;
			}
			result=parameters_check(args[parameter_count], args[parameter_count+1], TIME_BETWEEN_HOLES, hole_time);
			if (result == -2)
			{
				//Ограничение в 30 секунд связано с максимально возможным значением переменной типа short int - 32767
				puts("Допустимы только положительные целочисленные значения в диапазоне между 0 и 30 включительно.");
				parameter_count++;
				continue;
			}
			//Время не может быть отрицательной величиной
			if (result < 0 || result > 30)
			{
				printf("Значение '%s' параметра '%s' должно быть в диапазоне между 0 и 30 включительно. ", args[parameter_count+1], args[parameter_count]);
				if (*hole_time == TIME_BETWEEN_HOLES)
					printf("Установлено значение по умолчанию: %i.\n", TIME_BETWEEN_HOLES);
				else
					printf("Значение не изменено (%hi).\n", *hole_time);
				parameter_count++;
				continue;
			}
			//Поскольку SDL API воспринимает миллисекунды - производится умножение на 1000
			*hole_time=result*1000;
			parameter_count++;
		}
		//Параметры скорости влияют на задержку ввода (отзывчивость)
		//от игрока команд движения (перемещения между клетками и поворота)
		//TODO: ограничений на максимальное значение пока нет
		else if (!strcmp("-s", args[parameter_count]) || !strcmp("--speed", args[parameter_count]))
		{
			//Проверка наличия непосредственно значения скорости движения, если оно отсутствует - переход к следующему параметру
			if ((parameter_count+1) >= count)
			{
				printf("Параметр '%s' пуст. ", args[parameter_count]);
				if (*speed == SPEED_MOVE)
					printf("Установлено значение по умолчанию: %i.\n", SPEED_MOVE);
				else
					printf("Значение не изменено (%hi).\n", *speed);
				parameter_count++;
				continue;
			}
			result=parameters_check(args[parameter_count], args[parameter_count+1], SPEED_MOVE, speed);
			if (result == -2)
			{
				puts("Допустимы только положительные целочисленные значения.");
				parameter_count++;
				continue;
			}
			//Начальная скорость не может быть отрицательной величиной
			if (result < 0)
			{
				printf("Значение '%s' параметра '%s' должно быть больше нуля или равно нулю. ", args[parameter_count+1], args[parameter_count]);
				if (*speed == SPEED_MOVE)
					printf("Установлено значение по умолчанию: %i.\n", SPEED_MOVE);
				else
					printf("Значение не изменено (%hi).\n", *speed);
				parameter_count++;
				continue;
			}
			*speed=result;
			parameter_count++;
		}
		//Параметры скорости влияют на задержку ввода (отзывчивость)
		//от игрока команд движения (перемещения между клетками и поворота)
		//TODO: ограничений на максимальное значение пока нет
		else if (!strcmp("-S", args[parameter_count]) || !strcmp("--speed-turn", args[parameter_count]))
		{
			//Проверка наличия непосредственно значения скорости поворота, если оно отсутствует - переход к следующему параметру
			if ((parameter_count+1) >= count)
			{
				printf("Параметр '%s' пуст. ", args[parameter_count]);
				if (*turn_speed == SPEED_TURN)
					printf("Установлено значение по умолчанию: %i.\n", SPEED_TURN);
				else
					printf("Значение не изменено (%hi).\n", *turn_speed);
				parameter_count++;
				continue;
			}
			result=parameters_check(args[parameter_count], args[parameter_count+1], SPEED_TURN, turn_speed);
			if (result == -2)
			{
				puts("Допустимы только положительные целочисленные значения.");
				parameter_count++;
				continue;
			}
			//Начальная скорость не может быть отрицательной величиной
			if (result < 0)
			{
				printf("Значение '%s' параметра '%s' должно быть больше нуля или равно нулю. ", args[parameter_count+1], args[parameter_count]);
				if (*turn_speed == SPEED_TURN)
					printf("Установлено значение по умолчанию: %i.\n", SPEED_TURN);
				else
					printf("Значение не изменено (%hi).\n", *turn_speed);
				parameter_count++;
				continue;
			}
			*turn_speed=result;
			parameter_count++;
		}
		//DEBUG
		//Проверка, желает ли пользователь для целей отладки
		//видеть схематичное изображение лабиринта (отрисовывается каждый цикл - не рекомендуется для размеров больше 20*20)
		//и состояние некоторых переменных
		else if (!strcmp("-d", args[parameter_count]) || !strcmp("--debug", args[parameter_count]))
		{
			*debug=1;
			parameter_count++;
		}
		//Проверка, желает ли пользователь для целей отладки
		//видеть процесс создания скелета лабиринта (по умолчанию -нет), или да (путём передачи данного параметра)
		//и переход к следующему параметру (если есть)
		else if (!strcmp("-b", args[parameter_count]) || !strcmp("--show-building", args[parameter_count]))
		{
			*visual=1;
			parameter_count++;
		}
		//Проверка, желает ли пользователь видеть окончательный результат работы программы
		//(на данный момент - вывод скелета лабиринта), путём передачи данного параметра, или нет (по умолчанию)
		//и переход к следующему параметру (если есть)
		else if (!strcmp("-R", args[parameter_count]) || !strcmp("--result", args[parameter_count]))
		{
			*show_result=1;
			parameter_count++;
		}
		
		//OTHER
		
		//Если в качестве параметра передано что-то иное (например, значение для предыдущего параметра) - переход к следующему параметру (если он есть)
		else
		{
			parameter_count++;
		}
	}
	return 0;
}

//Проверка параметров, заданных пользователем
//Функция возвращает или введённое пользователм значение, либо "-2" в качестве ошибки
//Обработка корректности непосредственно самих значений производится на уровне обработки параметров
short int parameters_check(const char *parameter_name, const char *parameter_value, const short int default_value, const short int *parameter)
{
	//Защита от значений, превышающих размер short int, а также от нечисловых символов
	long int temp_int;
	char *tile_string[100]={""};
	temp_int=strtol(parameter_value, tile_string, 10);
	if ((!temp_int) && (parameter_value == *tile_string))
	{
		printf("Значение '%s' параметра '%s' непонятно. ", parameter_value, parameter_name);
		if (*parameter == default_value)
			printf("Установлено значение по умолчанию: %i.\n", default_value);
		else
			printf("Значение не изменено (%hi).\n", *parameter);
		return -2;
	}
	short int value=temp_int;
	//Если в параметрах передано дробное число/мусор -
	//такие значения будут усечены до целых чисел/исправлены на нули или значения по умолчанию
	char value_string[10]; //в 10 cимволов должно уместиться достаточно большое число
	snprintf(value_string, 10, "%hi", value);
	if (strcmp(value_string, parameter_value))
	{
		printf("Значение '%s' параметра '%s' непонятно. ", parameter_value, parameter_name);
		if (*parameter == default_value)
			printf("Установлено значение по умолчанию: %i.\n", default_value);
		else
			printf("Значение не изменено (%hi).\n", *parameter);
		return -2;
	}
	return value;
}
