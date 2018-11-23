/*  settings.c

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

int change_settings_file(struct options_array *options_arrayPtr, FILE *settings, char *settings_filename);
int repair_settings_file(struct options_array *options_arrayPtr, FILE *settings, char *settings_filename);
int next_element(struct options_array **options_arrayPtr, const char *option, const short int value);
int search_in_section(struct options_array *sectionPtr, FILE *settings, FILE *tmp);

int set_options(FILE *settings, char *settings_filename, short int *window_height, short int *window_width, short int *fullscreen, short int *refresh_rate, short int *fps, short int *hw_accel, short int *vsync, short int *sdl_12, short int *size_labyrinth_length, short int *size_labyrinth_width, short int *no_walls_removing, short int *rivals, short int *holes, short int *trap_time, short int *hole_time, short int *speed, short int *turn_speed, short int *debug, short int *visual, short int *result)
{
	//Если файл ещё не открыт - производится открытие существующего файла по умолчанию
	//либо создание нового файла с настройками по умолчанию
	if (!settings)
	{
		int string_lenght_1=strlen("Нет могу открыть файл настроек!\n ");
		int string_lenght_2=strlen((DEFAULT_SETTINGS_FILE));
		char *error_message=calloc(string_lenght_1+string_lenght_2+1, 1);
		snprintf(error_message, string_lenght_1+string_lenght_2+1, "Нет могу открыть файл настроек!\n %s", DEFAULT_SETTINGS_FILE);
		if (!(settings = fopen(DEFAULT_SETTINGS_FILE, "r")))
		{
			perror(error_message);
			printf("Создаю новый... (\"%s\")\n\n", DEFAULT_SETTINGS_FILE);
			if (change_settings_file(NULL, settings, settings_filename))
				return 1;
		}
		free(error_message);
		error_message=NULL;
	}
	if (!settings)
	{
		if (!(settings = fopen(DEFAULT_SETTINGS_FILE, "r")))
		{
			perror("Проблема по созданию файла настроек\nПроверьте права доступа к каталогу с настройками игры и свободное место на диске с разделом, содержащим этот каталог\n");
			return 1;
		}
	}
	//Если файл пуст (размер 0 байт) - он будет полностью восстановлен со значениями по умолчанию
	char *temp_string_1=calloc(100, 1);
	fgets(temp_string_1, 100, settings);
	if (feof(settings))
	{
		if (change_settings_file(NULL, settings, settings_filename))
			return 1;
		if (!(settings = fopen(settings_filename, "r")))
		{
			perror("Проблема по созданию файла настроек\nПроверьте права доступа к каталогу с настройками игры и свободное место на диске с разделом, содержащим этот каталог\n");
			return 1;
		}
	}
	free(temp_string_1);
	if (fseek(settings, 0, SEEK_SET))
	{
		perror("Не получается восстановить файл настроек");
		return 1;
	}
	struct options_array *options_arrayPtr=NULL;
	while (!feof(settings))
	{
		int success=0;
		//Максимальный размер считываемой строки - 100 символов
		char *option=calloc(100, 1);
		char *temp_string_1=calloc(100, 1);
		char *temp_string_2=calloc(100, 1);
		if (!option || !temp_string_1 || !temp_string_2)
		{
			puts("Проблема с памятью во время чтения файла настроек");
			fclose(settings);
			return 2;
		}
		short int value=NULL;
		fgets(temp_string_1, 100, settings);
		//FIXME: значение опции может быть изменено только один раз за вызов функции change_settings_file()
		//(повторяющиеся опции перезаписываются, в расчёт берётся последняя)
		
		//Если строка начинается с одного из этих симовлов
		//(комментарий/пустая строка с переходом на следующую строку/начало новой секции)- она пропускается
		if (strspn(temp_string_1, "#\n["))
			continue;
		success=sscanf(temp_string_1, "%s = %s", option, temp_string_2);
		if (success == 1)
		{
			int string_lenght=0;
			string_lenght=strlen(temp_string_1);
			int before_equal_sign=strcspn(temp_string_1, "=");
			//Если в строке отсутствует знак равенства - строка будет закомментирована
			//(функции change_settings_file() будет передано первое слово строки и значение параметра "-2")
			if (before_equal_sign == string_lenght)
			{
				before_equal_sign=strcspn(temp_string_1, " ");
				memset(option, 0, 100);
				strncpy(option, temp_string_1, before_equal_sign);
				if (next_element(&options_arrayPtr, option, -2))
					return 2;
				continue;
			}
			memset(option, 0, 100);
			strncpy(option, temp_string_1, before_equal_sign);
			strncpy(temp_string_2, &temp_string_1[before_equal_sign+1], string_lenght);
			//Если в названии настройки присутствует знак пробела - он будет заменён на признак конца строки
			//В противном случае признак конца строки устанавливается в конце строковаго массива, на всякий случай
			//(пробел в названиях разрешён только в конце строки)
			before_equal_sign=strcspn(option, " ");
			if (before_equal_sign < 100)
				option[before_equal_sign]='\000';
			else
				option[99]='\000';
			success=2;
			printf("Ptr: %i Lenght: %i\n", before_equal_sign, string_lenght);
		}
		if (success == 2)
		{
			//Защита от значений, превышающих размер short int, а также от нечисловых символов
			long int temp_int_2;
			char *tile_string[100]={""};
			temp_int_2=strtol(temp_string_2, tile_string, 10);
			printf("Int: %li String: %s Tile: %s Equal: %i\n", temp_int_2, temp_string_2, *tile_string, temp_string_2 == *tile_string);
			if ((!temp_int_2) && (temp_string_2 == *tile_string))
			{
				//Если преобразование строки в число не удалось - настройке присваивается
				//значение равное "-1", которое будет при дальнейшем разборе заменено на значение по умолчанию
				value=-1;
				if (next_element(&options_arrayPtr, option, value))
					return 2;
			}
			else if (temp_int_2)
				value=temp_int_2;
			if (value != -1)
			{
				//Если в файле настроек записано дробное число/мусор -
				//такие значения будут усечены до целых чисел/исправлены на нули или значения по умолчанию
				char value_string[10]; //в 10 cимволов должно уместиться достаточно большое число
				snprintf(value_string, 10, "%hi;", value);
				if (strcmp(value_string, temp_string_2))
				{
					if (next_element(&options_arrayPtr, option, value))
						return 2;
				}
			}
			printf("%s = %hi\n", option, value);
			//Размеры окна будут проверены и при необходимости изменены при инициализации SDL
			if (!strcmp("Height", option))
			{
				if (value < 0)
				{
					value=DEFAULT_RES_Y;
					if (next_element(&options_arrayPtr, option, value))
						return 2;
				}
				*window_height=value;
			}
			else if (!strcmp("Width", option))
			{
				if (value < 0)
				{
					value=DEFAULT_RES_X;
					if (next_element(&options_arrayPtr, option, value))
						return 2;
				}
				*window_width=value;
			}
			//Если значение Fullscreen меньше нуля - будет установлен оконный режим
			//(значение измениьтся на "0")
			//Иначе, если значение больше "1" - будет установлен полноэкранный режим
			//(значение измениьтся на "1")
			else if (!strcmp("Fullscreen", option))
			{
				if (value < 0)
				{
					value=0;
					if (next_element(&options_arrayPtr, option, value))
						return 2;
				}
				if (value > 1)
				{
					value=1;
					if (next_element(&options_arrayPtr, option, value))
						return 2;
				}
				*fullscreen=value;
			}
			//От частоты обновления на данный момент зависит только значение "FPS"
			//По умолчанию обе величины устанавливаются в отрицательную сторону ("-1"),
			//при инициализации SDL подстраиваясь к частоте обновления экрана
			//Значение "FPS" может быть установлено вручную, но не может быть меньше "1"
			else if (!strcmp("Refresh_rate", option))
				*refresh_rate=value; //будет проверено в дальнейшем с помощью SDL
			else if (!strcmp("FPS", option))
			{
				if (!value || value < -2)
				{
					value = -1;
					if (next_element(&options_arrayPtr, option, value))
						return 2;
				}
				*fps=value;
			}
			//Значение "Hardware_Acceleration" указывает на необходимость
			//инициализации SDL с задействованием аппаратного ускорения
			//Отрицательные значения (кроме "-1") будут переписаны на "0" (отключение аппаратного ускорения)
			//Положительные значения (кроме "1") будут переписаны на "1" (включение аппаратного ускорения)
			else if (!strcmp("Hardware_Acceleration", option))
			{
				if (value < -2)
				{
					value=0;
					if (next_element(&options_arrayPtr, option, value))
						return 2;
				}
				if (value == -1)
				{
					value=1; //значение по умолчанию
					if (next_element(&options_arrayPtr, option, value))
						return 2;
				}
				if (value > 1)
				{
					value=1;
					if (next_element(&options_arrayPtr, option, value))
						return 2;
				}
				*hw_accel=value;
			}
			//Значение "VSync" указывает на необходимость инициализации SDL
			//с задействованием режима синхронизации вертикального обновления экрана
			//Отрицательные значения (кроме "-1") будут переписаны на "0" (отключение синхронизации)
			//Положительные значения (кроме "1") будут переписаны на "1" (включение синхронизации)
			else if (!strcmp("VSync", option))
			{
				if (value < -2)
				{
					value=0;
					if (next_element(&options_arrayPtr, option, value))
						return 2;
				}
				if (value == -1)
				{
					value=1; //значение по умолчанию
					if (next_element(&options_arrayPtr, option, value))
						return 2;
				}
				if (value > 1)
				{
					value=1;
					if (next_element(&options_arrayPtr, option, value))
						return 2;
				}
				*vsync=value;
			}
			//Параметр "SDL_12_style" указывает на необходимость использования программного ускорения
			//с помощью API SDL 1.2 (которое устарело и в котором отстствуют
			//некоторые функции для корректной отрисовки по имеющемуся на данный момент алгоритму,
			//но которое может лучше работать на старых машинах/системах)
			//TODO: проверить, так ли это на самом деле
			//Отрицательные значения (включая "-1") будут переписаны на "0" (использование API SDL 2.x)
			//Положительные значения (кроме "1") будут переписаны на "1" (использование API SDL 1.2)
			
			//Этот параметр несовместим с параметрами "Hardware_Acceleration" и "VSync",
			//которые используются в API SDL 2.x
			//Если значение хотя бы одного из них будет равно "1",
			//и значение параметра "SDL_12_style" также будет установлено в "1" -
			//последний имеет приоритет, значения несовместимых параметров будут сброшены в состояние "0"
			
			else if (!strcmp("SDL_12_style", option))
			{
				if (value < 0)
				{
					value=0; //значение по умолчанию
					if (next_element(&options_arrayPtr, option, value))
						return 2;
				}
				if (value > 1)
				{
					value=1;
					if (next_element(&options_arrayPtr, option, value))
						return 2;
					if (*hw_accel)
					{
						if (next_element(&options_arrayPtr, "Hardware_Acceleration", 0))
							return 2;
					}
					if (*vsync)
					{
						if (next_element(&options_arrayPtr, "VSync", 0))
							return 2;
					}
				}
				*sdl_12=value;
			}
			//Размеры лабиринта могут варьироваться в пределах
			//от DEFAULT_MIN_LENGTH/DEFAULT_MIN_WIDTH до DEFAULT_MAX_LENGTH/DEFAULT_MAX_WIDTH
			//(по умолчанию - от 5 до 1000 клеток)
			//Значения большие, чем максимально возможные, будут приравнены к максимально возможным
			//Значения меньше минимально возможных будут приравнены к минимально возможным
			//Отрицательные значения будут приравнены к значениям по умолчанию
			else if (!strcmp("Labyrinth_length", option))
			{
				if (value > DEFAULT_MAX_LENGTH)
				{
					value=DEFAULT_MAX_LENGTH;
					if (next_element(&options_arrayPtr, option, value))
							return 2;
				}
				if (value < DEFAULT_MIN_LENGTH && value > 0)
				{
					value=DEFAULT_MIN_LENGTH;
					if (next_element(&options_arrayPtr, option, value))
							return 2;
				}
				if (value < 0)
				{
					value=DEFAULT_LENGTH;
					if (next_element(&options_arrayPtr, option, value))
							return 2;
				}
				*size_labyrinth_length=value+2;
			}
			else if (!strcmp("Labyrinth_width", option))
			{
				if (value > DEFAULT_MAX_WIDTH)
				{
					value=DEFAULT_MAX_WIDTH;
					if (next_element(&options_arrayPtr, option, value))
							return 2;
				}
				if (value < DEFAULT_MIN_WIDTH && value > 0)
				{
					value=DEFAULT_MIN_WIDTH;
					if (next_element(&options_arrayPtr, option, value))
							return 2;
				}
				if (value < 0)
				{
					value=DEFAULT_WIDTH;
					if (next_element(&options_arrayPtr, option, value))
							return 2;
				}
				*size_labyrinth_width=value+2;
			}
			//Параметр "Not_Remove_Walls" указывает на необходимость
			//сохранения "правильного" (нецикличного) лабиринта (при значении равном "1")
			//Все значения (кроме "1") будут переписаны на "0" (удаление случайных стен внутри лабиринта при его создании)
			else if (!strcmp("Not_Remove_Walls", option))
			{
				if (value != 1 && value != 0)
				{
					value=0;
					if (next_element(&options_arrayPtr, option, value))
						return 2;
				}
				*no_walls_removing=value;
			}
			//Параметр "Rivals" указывает общее на количество соперников, участвующих в игре (включая игрока)
			//FIXME: Максимальное количество участников пока не ограничено
			//Значения меньше нуля будут изменены на значение по умолчанию
			else if (!strcmp("Rivals", option))
			{
				if (value < 0)
				{
					value=RIVALS;
					if (next_element(&options_arrayPtr, option, value))
						return 2;
				}
				*rivals=value;
			}
			//Параметр "Pits" указывает на количество дыр в полу
			//TODO: пока все дыры циклически связаны между собой
			//TODO: надо сделать возможность создания нескольких несвязанных между собой пар дыр
			//Пока дыры связаны циклически - проверка на их чётное количество не производится
			//Значения меньше нуля будут изменены на значение по умолчанию
			//FIXME: максимальное количество пока не ограничено!
			//TODO: Пока не найден способ без особых накладных расходов перемещать участника
			//в случайное место лабиринта - количество дыр должно быть или больше 1, или равно 0
			else if (!strcmp("Pits", option))
			{
				if (value < 0)
				{
					value=HOLES;
					if (next_element(&options_arrayPtr, option, value))
						return 2;
				}
				if (value == 1) //см. TODO выше
				{
					value=2;
					if (next_element(&options_arrayPtr, option, value))
						return 2;
				}
				*holes=value;
			}
			//Время, проведённое в ловушке или потраченное при перемещении между дырами
			//в файле настройки указывается в секундах,
			//поскольку SDL API воспринимает миллисекунды - производится умножение на 1000
			//Ограничение в 30 секунд связано с максимально возможным значением переменной типа short int - 32767
			//Отрицательные значения будут заменены на значения по умолчанию
			else if (!strcmp("Time_in_trap", option))
			{
				if (value < 0 || value > 30)
				{
					value=TIME_IN_TRAP;
					if (next_element(&options_arrayPtr, option, value))
						return 2;
				}
				*trap_time=value*1000;
			}
			else if (!strcmp("Time_between_holes", option))
			{
				if (value < 0 || value > 30)
				{
					value=TIME_BETWEEN_HOLES;
					if (next_element(&options_arrayPtr, option, value))
						return 2;
				}
				*hole_time=value*1000;
			}
			//Параметры скорости влияют на задержку ввода (отзывчивость)
			//от игрока команд движения (перемещения между клетками и поворота)
			//TODO: ограничений на максимальное значение пока нет
			//Отрицательные значения будут заменены на значения по умолчанию
			else if (!strcmp("Speed_move", option))
			{
				if (value < 0)
				{
					value=SPEED_MOVE;
					if (next_element(&options_arrayPtr, option, value))
						return 2;
				}
				*speed=value;
			}
			else if (!strcmp("Speed_turn", option))
			{
				if (value < 0)
				{
					value=SPEED_TURN;
					if (next_element(&options_arrayPtr, option, value))
						return 2;
				}
				*turn_speed=value;
			}
			//В отношении параметров показа лабиринта во время игры/визуализации построения лабиринта/
			//показа готовой версии лабиринта сразу же после его построения -
			//любые значения, кроме "1" будут изменены на значения по умолчанию ("0" - не показывать)
			else if (!strcmp("Visual", option))
			{
				if (value != 1 && value != 0)
				{
					value=0;
					if (next_element(&options_arrayPtr, option, value))
						return 2;
				}
				*debug=value;
			}
			else if (!strcmp("Show_Building", option))
			{
				if (value != 1 && value != 0)
				{
					value=0;
					if (next_element(&options_arrayPtr, option, value))
						return 2;
				}
				*visual=value;
			}
			else if (!strcmp("Show_Finished", option))
			{
				if (value != 1 && value != 0)
				{
					value=0;
					if (next_element(&options_arrayPtr, option, value))
						return 2;
				}
				*result=value;
			}
			//Если считанная строка не относится ни к одной из настроек
			//(в том числе из-за грамматических или синтиксичексих ошибок) - она будет закомментирована
			else
			{
				if (next_element(&options_arrayPtr, option, -2))
					return 2;
			}
		}
		free(option);
		free(temp_string_1);
		free(temp_string_2);
		option=NULL;
		temp_string_1=NULL;
		temp_string_2=NULL;
	}
	//При изменении каких-либо настроек их данные (название и новое значение) будут записаны в файл настроек
	//Тоже самое будет сделано, если обнаружится нечитаемая строка/несчитанная настройка
	//(она будет либо закомментирована, либо восстановлена)
	if (options_arrayPtr)
	{
		if (change_settings_file(options_arrayPtr, settings, settings_filename))
		{
			perror("Файл настроек повреждён.\nВосстановление не удалось");
			return 1;
		}
	}
	else
	{
		if (fclose(settings))
		{
			perror("Не получается записать измененния в файл настроек");
			puts("Проверьте права доступа к каталогу и файлу с настройками игры и свободное место на диске с разделом, содержащим этот каталог");
			return 1;
		}
	}
	return 0;
	//Проверка корректности загруженных из файла параметров
	printf("window_height %hi\nwindow_width %hi\nfullscreen %hi\nrefresh_rate %hi\nfps %hi\nhw_accel %hi\nvsync %hi\nsdl_12 %hi\nsize_labyrinth_length %hi\nsize_labyrinth_width %hi\nno_walls_removing %hi\nrivals %hi\nholes %hi\ntrap_time %hi\nhole_time %hi\nspeed %hi\nturn_speed %hi\ndebug %hi\nvisual %hi\nresult %hi\n", *window_height, *window_width, *fullscreen, *refresh_rate, *fps, *hw_accel, *vsync, *sdl_12, *size_labyrinth_length, *size_labyrinth_width, *no_walls_removing, *rivals, *holes, *trap_time, *hole_time, *speed, *turn_speed, *debug, *visual, *result);
	//~ return 1;
}

//Получаемые параметры: список (связанный список/массив структур) настроек для изменения
//(название, новое значение, указатель на следующий элемент) и указатель на файл настроек
int change_settings_file(struct options_array *options_arrayPtr, FILE *settings, char *settings_filename)
{	
	//Если список настроек для изменения пуст (указатель ссылается на NULL) -
	//требуется создать новый/полностью перезаписать существующий файл настроек
	if (!options_arrayPtr)
	{
		//Определение массивов строк и значений по умолчанию (а также комментариев), которые содеражатся в файле настроек
		//TODO: проверить и изменить перед релизом значения по умолчанию "SDL_12_style", секции "[DEBUG]" и пр.
		#define SECTION_NUMBERS 3 //количество секций в файле настроек
		char *sections[]={"[VIDEO]", "[GAME]", "[DEBUG]"};
		char *video_options[]={"Height", "Width", "Fullscreen", "Refresh_rate", "FPS", "Hardware_Acceleration", "VSync", "SDL_12_style"};
		short int video_values[]={DEFAULT_RES_Y, DEFAULT_RES_X, 0, -1, -1, 1, 1, 0};
		char *video_comments[]={"#Resolution\n#Only supported by adapter and monitor!\n",
								"", "", "",
								"#FPS is usually set to adaptor/monitor refresh rate\n#but may be changed for slowdown/acceleration of internal computing\n#If this value is \"-1\", it will be set to the refresh rate of the screen\n",
								
								"#Either 0 (no acceleration/VSync) or 1 (with acceleration/VSync)\n", "",
								
								"#Old version of software acceleration without VSync and with some bugs\n#Set to 1 for this or 0 for new version\n#If this option is set to 1, then \"Hardware_Acceleration\" and \"VSync\"\n#options will be ignored\n"};
		
		char *game_options[]={"Labyrinth_length", "Labyrinth_width", "Not_Remove_Walls", "Rivals", "Pits", "Time_in_trap", "Time_between_holes", "Speed_move", "Speed_turn"};
		short int game_values[]={DEFAULT_LENGTH, DEFAULT_WIDTH, 0, RIVALS, HOLES, TIME_IN_TRAP/1000, TIME_BETWEEN_HOLES/1000, SPEED_MOVE, SPEED_TURN};
		char *game_comments[]={"#Something between 5-1000\n", "",
			
								"#Either 1 (no walls will be removing) or 0\n#(some walls will be remove that cause to create potentially cycling labyrinth)\n",
								"#Total number of rivals (including player)\n",
								"#Total number of pits in floor\n#(for fast travel to far end of labyrinth. Or, maybe to neighbour cell)\n#Either 0 (no pits at all), either something greater 1\n",
								"#Time in seconds, no more than 30\n", "",
								
								"#Time in milliseconds (lesser is faster)\n", ""};
		
		char *debug_options[]={"Visual", "Show_Building", "Show_Finished"};
		unsigned short int debug_values[]={0, 0, 0};
		char *debug_comments[]={"#Show labyrinth in terminal/console\n#For developing/debugging or cheating only\n",
								
								"#Show process of creating skeleton of labyrinth\n",
								
								"#Show the final version of skeleton of labyrinth\n#before the game start\n"};
		
		if (settings)
			fclose(settings);
		int string_lenght_1=strlen("Не получается создать файл настроек ");
		int string_lenght_2=strlen(settings_filename);
		char *error_message=calloc(string_lenght_1+string_lenght_2+1, 1);
		snprintf(error_message, string_lenght_1+string_lenght_2+1, "Не получается создать файл настроек %s", settings_filename);
		if (!(settings = fopen(settings_filename, "w")))
		{
			perror(error_message);
			puts("Проверьте права доступа к каталогу с настройками игры и свободное место на диске с разделом, содержащим этот каталог");
			free(error_message);
			error_message=NULL;
			return 1;
		}
		free(error_message);
		error_message=NULL;
		fprintf(settings, "%s", "#Comments are beginning with number sign '#'\n");
		fprintf(settings, "%s", "#Section titles has highlited by brakets '[]'\n");
		fprintf(settings, "%s", "#Options and values are separated by an equal sign '='\n\n");
		for (int i = 0; i < SECTION_NUMBERS; i++)
		{
			fprintf(settings, "%s\n", sections[i]);
			switch (i)
			{
				case 0:
					for (int j = 0; j < sizeof(video_values)/sizeof(short int); j++)
					{
						fprintf(settings, "%s%s = %hi;\n", video_comments[j], video_options[j], video_values[j]);
					}
					break;
				case 1:
					for (int j = 0; j < sizeof(game_values)/sizeof(short int); j++)
					{
						fprintf(settings, "%s%s = %hi;\n", game_comments[j], game_options[j], game_values[j]);
					}
					break;
				case 2:
					for (int j = 0; j < sizeof(debug_values)/sizeof(short int); j++)
					{
						fprintf(settings, "%s%s = %hi;\n", debug_comments[j], debug_options[j], debug_values[j]);
					}
					break;
				default:
					printf("Не найдено настроек для секции %s!", sections[i]);
					break;
			}
			fprintf(settings, "\n");
		}
		if (fclose(settings))
		{
			perror("Не получается создать файл настроек");
			puts("Проверьте права доступа к каталогу с настройками игры и свободное место на диске с разделом, содержащим этот каталог");
			return 1;
		}
	}
	if (options_arrayPtr)
	{
		if (settings)
			fclose(settings);
		//Удаление предыдущей резервной копии и создание новой из имеющегося файла настроек
		char *backup_filename=calloc(strlen(settings_filename)+strlen(".bak")+1,1);
		snprintf(backup_filename, strlen(settings_filename)+strlen(".bak")+1, "%s.bak", settings_filename);
		
		int string_lenght_1=strlen("Ошибка удаления предыдущей резервной копии\n ");
		int string_lenght_2=strlen(backup_filename);
		char *error_message=calloc(string_lenght_1+string_lenght_2+1, 1);
		snprintf(error_message, string_lenght_1+string_lenght_2+1, "Ошибка удаления предыдущей резервной копии\n %s", backup_filename);
		
		if (remove(backup_filename))
		{
			perror(error_message);
		}
		free(error_message);
		error_message=NULL;
		puts("Создание резервной копии файла настроек");
		if (rename(settings_filename, backup_filename))
		{
			perror("Не могу сохранить резервную копию файла настроек");
			puts("Проверьте права доступа к каталогу с настройками игры и свободное место на диске с разделом, содержащим этот каталог");
			return 1;
		}
		if (!(settings = fopen(backup_filename, "r")))
		{
			perror("Не получается записать измененния в файл настроек");
			puts("Проверьте права доступа к каталогу и файлу с настройками игры и свободное место на диске с разделом, содержащим этот каталог");
			return 1;
		}
		FILE *tmp;
		if (!(tmp = fopen(settings_filename, "w")))
		{
			perror("Не получается записать измененния в файл настроек");
			puts("Проверьте права доступа к каталогу и файлу с настройками игры и свободное место на диске с разделом, содержащим этот каталог");
			fclose(settings);
			return 1;
		}
		do
		{
			char temp_string_1[100]={""};
			char temp_string_2[50]={""};
			struct options_array *last_options_arrayPtr=options_arrayPtr;
			struct options_array *previous_options_arrayPtr=options_arrayPtr;
			fgets(temp_string_1, 100, settings);
			//Если в исходном файле присутствует несколько пустых строк подряд -
			//они будут удалены (останется только одна)
			int ch=fgetc(settings);
			if (!strcmp(temp_string_1, "\n") && (ch == 10 || ch == 13))
			{
				ungetc(ch, settings);
				continue;
			}
			else
				ungetc(ch, settings);
			//Комментарии, названия секций и пустые строки сразу же записываются в файл,
			//без дальнейшей обработки
			if (strspn(temp_string_1, "#\n["))
			{
				fputs(temp_string_1, tmp);
				continue;
			}
			sscanf(temp_string_1, "%s", temp_string_2);
			int success=0;
			while (last_options_arrayPtr)
			{
				int string_lenght=strlen(last_options_arrayPtr->change_option);
				//Если последний символ проверяемой строки содержит знак перевода строки - он не будет учитываться при сравнении
				if (last_options_arrayPtr->change_option[string_lenght-1] == '\n')
					string_lenght--;
				if (!strncmp(last_options_arrayPtr->change_option, temp_string_2, string_lenght))
				{
					if (last_options_arrayPtr == options_arrayPtr)
						options_arrayPtr=options_arrayPtr->next_option;
					previous_options_arrayPtr->next_option=last_options_arrayPtr->next_option;
					success=1;
					break;
				}
				if (previous_options_arrayPtr != last_options_arrayPtr)
					previous_options_arrayPtr=last_options_arrayPtr;
				last_options_arrayPtr=last_options_arrayPtr->next_option;
			}
			if (!success)
				fputs(temp_string_1, tmp);
			if (success)
			{
				//Если в качестве значения передано "-2" - строка будет закомментирована
				//(первым символом в строке будет "#")
				if (last_options_arrayPtr->change_value == -2)
					fprintf(tmp, "#%s", temp_string_1);
				else
					fprintf(tmp, "%s = %hi;\n", last_options_arrayPtr->change_option, last_options_arrayPtr->change_value);
				free(last_options_arrayPtr);
			}
		} while (!feof(settings));
		if (options_arrayPtr)
		{
			printf("\nНе найдено: (%p)\n", options_arrayPtr);
			struct options_array *last_options_arrayPtr=options_arrayPtr;
			while (last_options_arrayPtr)
			{
				printf("%s = %hi (%p)\n", last_options_arrayPtr->change_option, last_options_arrayPtr->change_value, last_options_arrayPtr->next_option);
				last_options_arrayPtr=last_options_arrayPtr->next_option;
			}
		}
		puts("");
		if (fclose(tmp))
		{
			perror("Не получается записать измененния в файл настроек");
			puts("Проверьте права доступа к каталогу и файлу с настройками игры и свободное место на диске с разделом, содержащим этот каталог");
			return 1;
		}
		//Если в списке настроек остались какие-то незаписанные настройки,
		//значит они по каким-то причинам (закомментированы/удалены/записаны с ошибками и пр.) не были найдены в файле настроек
		//В этом случае производится попытка восстановления данных настроек с их дозаписью
		//либо под соответствующей закомментированной настройкой, либо в конце соответствующей секции (разделе файла настроек)
		if (options_arrayPtr)
		{
			puts("Файл настроек повреждён. Попытка восстановления...");
			repair_settings_file(options_arrayPtr, settings, settings_filename);
		}
		else
		{
			if (fclose(settings))
			{
				perror("Не получается записать измененния в файл настроек");
				puts("Проверьте права доступа к каталогу и файлу с настройками игры и свободное место на диске с разделом, содержащим этот каталог");
				return 1;
			}
		}
	}
	return 0;
}

//Если в списке настроек остались какие-то незаписанные настройки,
//значит они по каким-то причинам (закомментированы/удалены/записаны с ошибками и пр.) не были найдены в файле настроек
//В этом случае производится попытка восстановления данных настроек с их дозаписью
//либо под соответствующей закомментированной настройкой, либо в конце соответствующей секции (разделе файла настроек)
int repair_settings_file(struct options_array *options_arrayPtr, FILE *settings, char *settings_filename)
{
	//В случае восстановления будет создана ещё одна резервная копия (для имеющейся резервной копии)
	if (settings)
		fclose(settings);
	char *backup_filename=calloc(strlen(settings_filename)+strlen(".bak")+1,1);
	char *backup2_filename=calloc(strlen(settings_filename)+strlen(".bak_old")+1,1);
	snprintf(backup_filename, strlen(settings_filename)+strlen(".bak")+1, "%s.bak", settings_filename);
	snprintf(backup2_filename, strlen(settings_filename)+strlen(".bak_old")+1, "%s.bak_old", settings_filename);
	
	int string_lenght_1=strlen("Ошибка удаления предыдущей резервной копии\n ");
	int string_lenght_2=strlen(backup2_filename);
	char *error_message=calloc(string_lenght_1+string_lenght_2+1, 1);
	snprintf(error_message, string_lenght_1+string_lenght_2+1, "Ошибка удаления предыдущей резервной копии\n %s", backup2_filename);
		
	if (remove(backup2_filename))
	{
		perror(error_message);
	}
	free(error_message);
	error_message=NULL;
	puts("Создание резервной копии для резервной копии файла настроек");
	if (rename(backup_filename, backup2_filename))
	{
		perror("Не получается сохранить предыдущую резервную копию файла настроек");
		puts("Проверьте права доступа к каталогу с настройками игры и свободное место на диске с разделом, содержащим этот каталог");
		return 1;
	}
	//~ if (remove("settings.ini.bak"))
	//~ {
		//~ perror("Ошибка удаления резервной копии\n settings.ini.bak");
	//~ }
	puts("Создание резервной копии файла настроек");
	if (rename(settings_filename, backup_filename))
	{
		perror("Не получается сохранить резервную копию файла настроек");
		puts("Проверьте права доступа к каталогу и файлу с настройками игры и свободное место на диске с разделом, содержащим этот каталог");
		return 1;
	}
	if (!(settings = fopen(backup_filename, "r")))
	{
		perror("Не получается восстановить файл настроек");
		puts("Проверьте права доступа к каталогу и файлу с настройками игры и свободное место на диске с разделом, содержащим этот каталог");
		return 1;
	}
	FILE *tmp;
	if (!(tmp = fopen(settings_filename, "w")))
	{
		perror("Не получается восстановить файл настроек");
		puts("Проверьте права доступа к каталогу и файлу с настройками игры и свободное место на диске с разделом, содержащим этот каталог");
		fclose(settings);
		return 1;
	}
	char *video_options[]={"Height", "Width", "Fullscreen", "Refresh_rate", "FPS", "Hardware_Acceleration", "VSync", "SDL_12_style", ""};
	char *game_options[]={"Labyrinth_length", "Labyrinth_width", "Not_Remove_Walls", "Rivals", "Holes", "Time_in_trap", "Time_between_holes", "Speed_move", "Speed_turn", ""};
	char *debug_options[]={"Visual", "Show_Building", "Show_Finished", ""};
	//Производится считывание настроек для восстановления и присваивание их соответствующим спискам
	//(по секциям/разделам в файле настроек)
	struct options_array *last_options_arrayPtr=options_arrayPtr;
	struct options_array *video_options_arrayPtr=NULL;
	struct options_array *game_options_arrayPtr=NULL;
	struct options_array *debug_options_arrayPtr=NULL;
	while (last_options_arrayPtr)
	{
		int count=0;
		while (strcmp(video_options[count], ""))
		{
			if (!strcmp(video_options[count], last_options_arrayPtr->change_option))
				next_element(&video_options_arrayPtr, last_options_arrayPtr->change_option, last_options_arrayPtr->change_value);
			count++;
		}
		count=0;
		while (strcmp(game_options[count], ""))
		{
			if (!strcmp(game_options[count], last_options_arrayPtr->change_option))
				next_element(&game_options_arrayPtr, last_options_arrayPtr->change_option, last_options_arrayPtr->change_value);
			count++;
		}
		count=0;
		while (strcmp(debug_options[count], ""))
		{
			if (!strcmp(debug_options[count], last_options_arrayPtr->change_option))
				next_element(&debug_options_arrayPtr, last_options_arrayPtr->change_option, last_options_arrayPtr->change_value);
			count++;
		}
		last_options_arrayPtr=last_options_arrayPtr->next_option;
	}
	while (!feof(settings))
	{
		char temp_string_1[100]={""};
		fgets(temp_string_1, 100, settings);
		//Во время чтения исходного файла настроек все пустые строки и комментарии
		//сразу же переносятся в новый файл без изменений
		if (strspn(temp_string_1, "#\n"))
		{
			fputs(temp_string_1, tmp);
			continue;
		}
		//Внутри разделов производится поиск соответствующих настроек
		//Когда они все будут найдены - оставшаяся непросмотренная часть раздела
		//(если она есть) переносится без изменений, пока не будет достигнут следующий раздел или конец файла
		if (strspn(temp_string_1, "["))
		{
			fputs(temp_string_1, tmp);
			if (!strncmp(temp_string_1, "[VIDEO]", 7))
				search_in_section(video_options_arrayPtr, settings, tmp);
			if (!strncmp(temp_string_1, "[GAME]", 6))
				search_in_section(game_options_arrayPtr, settings, tmp);
			if (!strncmp(temp_string_1, "[DEBUG]", 7))
				search_in_section(debug_options_arrayPtr, settings, tmp);
			continue;
		}
		fputs(temp_string_1, tmp);
	}
	if (fclose(tmp))
	{
		perror("Не получается восстановить файл настроек");
		puts("Проверьте права доступа к каталогу и файлу с настройками игры и свободное место на диске с разделом, содержащим этот каталог");
		return 1;
	}
	if (fclose(settings))
	{
		perror("Не получается восстановить файл настроек");
		puts("Проверьте права доступа к каталогу си файлу  настройками игры и свободное место на диске с разделом, содержащим этот каталог");
		return 1;
	}
	return 0;
}

//Функция для добавления названий и значений в связанный список (массив структур) настроек для изменения/восстановления
//FIXME: значение опции может быть изменено только один раз за вызов функции
//(повторяющиеся опции перезаписываются, в расчёт берётся последняя)
int next_element(struct options_array **options_arrayPtr, const char *option, const short int value)
{
	struct options_array *new_options_arrayPtr=NULL;
	if (!(new_options_arrayPtr=calloc(1, sizeof(struct options_array))))
	{
		perror("Файл настроек повреждён.\nВосстановление не удалось из-за проблем с памятью");
		return 2;
	}
	strcpy(new_options_arrayPtr->change_option, option);
	new_options_arrayPtr->change_value=value;
	new_options_arrayPtr->next_option=NULL;
	if (!(*options_arrayPtr))
		*options_arrayPtr=new_options_arrayPtr;
	else
	{
		struct options_array *last_options_arrayPtr=NULL;
		last_options_arrayPtr=*options_arrayPtr;
		//Если настройка уже присутствует в массиве, то её значение заменяется на вновь полученное
		if (!strcmp(last_options_arrayPtr->change_option, new_options_arrayPtr->change_option))
		{
			last_options_arrayPtr->change_value=new_options_arrayPtr->change_value;
			return 0;
		}
		while (last_options_arrayPtr->next_option)
		{
			last_options_arrayPtr=last_options_arrayPtr->next_option;
			//Если настройка уже присутствует в массиве, то её значение заменяется на вновь полученное
			if (!strcmp(last_options_arrayPtr->change_option, new_options_arrayPtr->change_option))
			{
				last_options_arrayPtr->change_value=new_options_arrayPtr->change_value;
				return 0;
			}
		}
		last_options_arrayPtr->next_option=new_options_arrayPtr;
	}
	return 0;
}

//Функция поиска места для записи настроек, подлежещих восстановлению, в файле настроек
//Запись осуществялется либо под уже имеющейся, но закомментированной настройкой,
//либо в самом конце текущего раздела (секции) настроек
int search_in_section(struct options_array *sectionPtr, FILE *settings, FILE *tmp)
{
	char temp_string_1[100]={""};
	fpos_t position;
	struct options_array *last_options_arrayPtr=NULL;
	while (sectionPtr)
	{
		fgetpos(settings, &position);
		fgets(temp_string_1, 100, settings);
		if (temp_string_1[0] == '#')
		{
			char temp_string_11[100]={""};
			char temp_string_12[100]={""};
			char temp_string_13[100]={""};
			char temp_string_14[100]={""};
			int success=0;
			//Попытка вычленить из комментария строку, содержащую следующие варианты:
			//"Настройка Значение"
			//"Настройка = Значение"
			//Настройка = Значение ;"
			//В случае совпадения с одним их вариантов первое слово строки (со второго символа -
			//без знака '#') сравнивается со списком настроек для восстановления
			//Если совпадения найдены - будет записан изменённый (восстановленный) вариант, если нет - считанный из файла
			success=sscanf(temp_string_1, "%s %s %s %s", temp_string_11, temp_string_12, temp_string_13, temp_string_14);
			if ((success < 3) \
			|| ((success == 3) && !(strcmp(temp_string_12, "="))) \
			|| ((success == 4) && !(strcmp(temp_string_12, "=")) && !(strcmp(temp_string_14, ";"))))
			{
				struct options_array *previous_options_arrayPtr=sectionPtr;
				last_options_arrayPtr=sectionPtr;
				while (last_options_arrayPtr)
				{
					int string_lenght=strlen(last_options_arrayPtr->change_option);
					if (!strncmp(last_options_arrayPtr->change_option, &temp_string_11[1], string_lenght))
					{
						fputs(temp_string_1, tmp);
						fprintf(tmp, "%s = %hi;\n", last_options_arrayPtr->change_option, last_options_arrayPtr->change_value);
						printf("Восстанавливаю параметр \"%s\"...\n", last_options_arrayPtr->change_option);
						if (last_options_arrayPtr == sectionPtr)
							sectionPtr=sectionPtr->next_option;
						previous_options_arrayPtr->next_option=last_options_arrayPtr->next_option;
						free(last_options_arrayPtr);
						temp_string_1[0]='\000'; //для исключения дублирования строки при её выводе ниже
						break;
					}
					if (previous_options_arrayPtr != last_options_arrayPtr)
						previous_options_arrayPtr=last_options_arrayPtr;
					last_options_arrayPtr=last_options_arrayPtr->next_option;
				}
				//На случай, если условие будет верным, но совпадений не будет найдено
				fputs(temp_string_1, tmp);
			}
			//Если считанная строка не подходит ни под один из вариантов - она будет перенесена без изменений
			else
				fputs(temp_string_1, tmp);
		}
		//Проверка на конец раздела и начало нового
		//Если после пустой строки начинается новый раздел -
		//все невосстановленные настройки для данного раздела будут записаны сюда
		else if (!(strcmp(temp_string_1, "\n")))
		{
			fgetpos(settings, &position);
			fgets(temp_string_1, 100, settings);
			if ((temp_string_1[0] == '\010') || (temp_string_1[0] == '\013') || (temp_string_1[0] == '['))
			{
				while (sectionPtr)
				{
					fprintf(tmp, "%s = %hi;\n", sectionPtr->change_option, sectionPtr->change_value);
					printf("Восстанавливаю параметр \"%s\"...\n", sectionPtr->change_option);
					sectionPtr=sectionPtr->next_option;
				}
				fputs("\n", tmp); //добавление пустой строки для 
			}
			//Восстановление позиции для последующего корректного считывания названия следующего раздела
			fsetpos(settings, &position);
		}
		//Проверка на конец раздела и начало нового
		//(все невосстановленные настройки для данного раздела будут записаны сюда)
		else if (temp_string_1[0] == '[')
		{
			while (sectionPtr)
			{
				fprintf(tmp, "%s = %hi;\n", sectionPtr->change_option, sectionPtr->change_value);
				printf("Восстанавливаю параметр \"%s\"...\n", sectionPtr->change_option);
				sectionPtr=sectionPtr->next_option;
			}
			//Восстановление позиции для последующего корректного считывания названия следующего раздела
			fsetpos(settings, &position);
			break;
		}
		//Все остальные строки переносятся без изменений
		else
			fputs(temp_string_1, tmp);
	}
	return 0;
}
