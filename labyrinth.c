#include "includes_macros.h"

int parameters(char *args[], int count, int *length, int *width, int *visual, int *no_walls_removing, int *show_result);
int labyrinth_generation(int *labyrinth, int const size_labyrinth_length, int const size_labyrinth_width, int const visual, int const no_walls_removing, int const result, int const rivals);
int sdl_test();

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
	int size_labyrinth_length=DEFAULT_LENGTH; //длина - задаётся в настройках
	int size_labyrinth_width=DEFAULT_WIDTH; //ширина - задаётся в настройках
	int visual=0; //для отладки - выводить скелет лабиринта (по умолчанию - нет) или да
	int no_walls_removing=0; //убирать случайные стены в лабиринте (по умолчанию - да) или нет
	int result=0; //Отображать окончательный результат или нет (по умолчанию - нет)
	int rivals=RIVALS; //Количество соперников в лабиринте (включая игрока!)
	//Просмотр параметров запуска, если они есть
	//Если среди параметров есть требующие завершения работы после их обработки
	//(например, вызов справки) - производится проверка значения возврата из функции
	//и, если требуется, завершение работы программы
	if (argc > 1)
	{
		if (parameters(argv, argc, &size_labyrinth_length, &size_labyrinth_width, &visual, &no_walls_removing, &result))
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
	if (labyrinth_generation(labyrinth, size_labyrinth_length, size_labyrinth_width, visual, no_walls_removing, result, rivals))
	{
		puts("Недостаточно свободной памяти для генерации лабиринта");
		return 1;
	}
	
	//FIXME: изменить имя функции!
	if (sdl_test())
		return 1;

	//labyrinth={0};
	memset(labyrinth, 0, sizeof(*labyrinth)*size_labyrinth_width*size_labyrinth_length);
	free(labyrinth);
	labyrinth=NULL;
	return 0;
}
