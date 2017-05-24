#include "includes_macros.h"

int parameters(char *args[], int count, int *length, int *width, int *visual, int *no_walls_removing, int *show_result)
{
	void help(char *args[0], int help_size);
	int parameter_count=1;
		while (parameter_count < count)
		{
			int check=1;
			//Проверка, желает ли пользователь установить свою длину лабиринта
			if (!strcmp("-l", args[parameter_count]) || !strcmp("--length", args[parameter_count]))
			{
				//Проверка наличия непосредственно значения длины, если оно отсутствует - установка значения по умолчанию и переход к следующему параметру
				if ((parameter_count+1) >= count)
				{
					printf ("Параметр '%s' пуст. Установлено значение по умолчанию: %i.\n", args[parameter_count], DEFAULT_LENGTH);
					parameter_count++;
					continue;
				}
				//Если символы значения длины содержат посторонние символы - установка значения по умолчанию и переход к следующему параметру
				for (int i = 0; i < strlen(args[parameter_count+1]); i++)
				{
					if (!isdigit(args[parameter_count+1][i]) && check)
					{
						printf ("Значение \'%s\' параметра '%s' непонятно. Установлено значение по умолчанию: %i.\n", args[parameter_count+1], args[parameter_count], DEFAULT_LENGTH);
						check=0;
					}
				}
				//Если введённое значение содержит больше четырёх цифр (для 1000) - присваивание максимально возможного значения и переход к следующему параметру
				if (check && (strlen(args[parameter_count+1]) > 5))
				{
					printf ("Параметр '%s' содержит очень большое число (\'%s\').\nУстановлено максимально возможное значение: %i.\n", args[parameter_count], args[parameter_count+1], DEFAULT_MAX_LENGTH);
					*length=DEFAULT_MAX_LENGTH;
					parameter_count++;
					continue;
				}
				//Преобразование значения параметра и присваивание его переменной size_labyrinth_length и переход к следующему параметру
				//Если значение больше определённого максимально возможного - присваивание максимально возможного значения
				//Если значение меньше определённого минимально возможного - присваивание минимально возможного значения
				if (check)
				{
					*length=atoi(args[parameter_count+1]);
					if (*length > DEFAULT_MAX_LENGTH)
					{
						printf ("Параметр '%s' содержит очень большое число (\'%s\').\nУстановлено максимально возможное значение: %i.\n", args[parameter_count], args[parameter_count+1], DEFAULT_MAX_LENGTH);
						*length=DEFAULT_MAX_LENGTH;
					}
					if (*length < DEFAULT_MIN_LENGTH)
					{
						printf ("Параметр '%s' содержит очень маленькое число (\'%s\').\nУстановлено минимально возможное значение: %i.\n", args[parameter_count], args[parameter_count+1], DEFAULT_MIN_LENGTH);
						*length=DEFAULT_MIN_LENGTH;
					}
				}
				parameter_count++;
			}
			
			//Проверка, желает ли пользователь установить свою ширину лабиринта
			else if (!strcmp("-w", args[parameter_count]) || !strcmp("--width", args[parameter_count]))
			{
				//Проверка наличия непосредственно значения ширины, если оно отсутствует - установка значения по умолчанию и переход к следующему параметру
				if ((parameter_count+1) >= count)
				{
					printf ("Параметр '%s' пуст. Установлено значение по умолчанию: %i.\n", args[parameter_count], DEFAULT_WIDTH);
					parameter_count++;
					continue;
				}
				//Если символы значения ширины содержат посторонние символы - установка значения по умолчанию и переход к следующему параметру
				for (int i = 0; i < strlen(args[parameter_count+1]); i++)
				{
					if (!isdigit(args[parameter_count+1][i]) && check)
					{
						printf ("Значение \'%s\' параметра '%s' непонятно. Установлено значение по умолчанию: %i.\n", args[parameter_count+1], args[parameter_count], DEFAULT_WIDTH);
						check=0;
					}
				}
				//Если введённое значение содержит больше четырёх цифр (для 1000) - присваивание максимально возможного значения и переход к следующему параметру
				if (check && (strlen(args[parameter_count+1]) > 5))
				{
					printf ("Параметр '%s' содержит очень большое число (\'%s\').\nУстановлено максимально возможное значение: %i.\n", args[parameter_count], args[parameter_count+1], DEFAULT_MAX_WIDTH);
					*width=DEFAULT_MAX_WIDTH;
					parameter_count++;
					continue;
				}
				//Преобразование значения параметра и присваивание его переменной size_labyrinth_width и переход к следующему параметру
				//Если значение больше определённого максимально возможного - присваивание максимально возможного значения
				//Если значение меньше определённого минимально возможного - присваивание минимально возможного значения
				if (check)
				{
					*width=atoi(args[parameter_count+1]);
					if (*width > DEFAULT_MAX_WIDTH)
					{
						printf ("Параметр '%s' содержит очень большое число (\'%s\').\nУстановлено максимально возможное значение: %i.\n", args[parameter_count], args[parameter_count+1], DEFAULT_MAX_WIDTH);
						*width=DEFAULT_MAX_WIDTH;
					}
					if (*width < DEFAULT_MIN_WIDTH)
					{
						printf ("Параметр '%s' содержит очень маленькое число (\'%s\').\nУстановлено минимально возможное значение: %i.\n", args[parameter_count], args[parameter_count+1], DEFAULT_MIN_WIDTH);
						*width=DEFAULT_MIN_WIDTH;
					}
				}
				parameter_count++;
			}
			//Проверка, желает ли пользователь вдля целей отладки
			//видеть процесс создания скелета лабиринта (по умолчанию -да), или нет (путём передачи данного параметра)
			//и переход к следующему параметру (если есть)
			else if (!strcmp("-v", args[parameter_count]) || !strcmp("--visual", args[parameter_count]))
			{
				*visual=1;
				parameter_count++;
			}
			//Проверка, желает ли пользователь оставить удаление стен в лабиринте для создания цикличного лабиринта
			//(по умолчанию - да), или нет (путём передачи данного параметра)
			//и переход к следующему параметру (если есть)
			else if (!strcmp("-nwr", args[parameter_count]) || !strcmp("--no-walls-removing", args[parameter_count]))
			{
				*no_walls_removing=1;
				parameter_count++;
			}
			//Проверка, желает ли пользователь видеть окончательный результат работы программы
			//(на данный момент - вывод скелета лабиринта), путём передачи данного параметра, или нет (по умолчанию)
			//и переход к следующему параметру (если есть)
			else if (!strcmp("-r", args[parameter_count]) || !strcmp("--result", args[parameter_count]))
			{
				*show_result=1;
				parameter_count++;
			}
			
			//Проверка на запрос вызова справки и передача в основную программу сигнала о завершении работы
			else if (!strcmp("-h", args[parameter_count]) || !strcmp("--help", args[parameter_count]))
			{
				help(&args[0], strlen(args[parameter_count]));
				return 1;
			}
			
			//Если в качестве параметра передано что-то иное (например, значение для предыдущего параметра) - переход к следующему параметру (если он есть)
			else
			{
				parameter_count++;
			}
			
			
		}
		
	return 0;
}