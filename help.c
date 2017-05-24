#include <stdio.h>
#include <string.h>

void help(char *args[0], int help_size)
{
	int line_length=strlen(args[0]);
	int indent=0;
	
	//Вывод форматированной краткой справки (для параметра "-h")
	char *help_strings[]={"[-l ЗНАЧЕНИЕ|--length ЗНАЧЕНИЕ]",
						"[-w ЗНАЧЕНИЕ|--width ЗНАЧЕНИЕ]",
						"[-v|--visual]",
						"[-nwr|--no-wall-removing]",
						"[-r|--result]",
						"[-h|--help]"};
	printf("\n  Использование: \n\n%s", args[0]);
	//Проверка возможности использования отступа для новых строк шириной в название программы
	//Если название программы и строка помощи больше, чем 80 (ширина термина+пробел) - устанавливается значение по умолчанию (10)
	//Последнюю строку массива ("[-h|--help]") не обрабатываем - она будет выводиться в отдельной строке
	for (int i = 0; i < 5; i++)
	{
		if (line_length+strlen(help_strings[i]) < 79)
			indent=line_length+1;
		else
			indent=10;
	}
	
	for (int i = 0; i < 6; i++) //FIXME: задать конечное значение переменной - количеством элементов (строк) в массиве
	{
		//Для вывода последней строки помощи ("[-h|--help]") - отдельная строка
		if (i==5)
		{
			printf("\n\n%s", args[0]);
		}
		//Если название программы и первой строки справки меньше 80 (ширина терминала+пробел; задать иначе (узнать из окружения)?),
		//то первая строка справки выводится в той же строке
		//FIXME: неверно учитывается размер кириллических символов!
		line_length=line_length+strlen(help_strings[i]);
		if (line_length < 79)
		{
			printf(" %s", help_strings[i]);
		}
		//Иначе - производится перевод на новую строку с отступом в 10 пробелов и устанавливается признак перевода строки
		else
		{
			printf("\n%*s%s", indent, " ", help_strings[i]);
			line_length=strlen(help_strings[i]);
		}
	}
	printf("\n\n");

	//Если указан параметр "--help" - дополнительно выводится полная справка
	if (help_size > 3)
	{
		FILE *help_file=NULL;
		help_file=fopen("help.txt", "r");
		if (help_file == NULL)
		{
			puts("Файл справки отсутствует или повреждён");
			return;
		}
		//char printhelp[200];
		while (!feof(help_file) && !ferror(help_file))
		{
			char printhelp[200]={'\000'};
			fgets(printhelp, 200, help_file);
			printf("%s", printhelp);
		}
		fclose(help_file);
		printf("\n\n");
	}
	return;
}
