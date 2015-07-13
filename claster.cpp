// Подключение необходимых заголовочных файлов
#include "mpi.h"
#include <stdio.h>
#include <math.h>

// Главная функция программы (выполняется всеми процессами)
int main(int argc, char **argv)
{
    // Объявление переменных
    int done = 0, myid, numprocs, i;
	unsigned int n;
    double PI25DT = 3.141592653589793238462643;
    double mypi, pi, h, sum, x;
    double startwtime = 0.0, endwtime;
    int namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
 
    // Инициализация подсистемы MPI
    MPI_Init(&argc, &argv);
    // Получить общее число процессов numprocs в рамках задачи)
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    // Получить номер myid текущего процесса 
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);
    MPI_Get_processor_name(processor_name,&namelen);
 
    // Вывод номера процесса в общем пуле
    fprintf(stdout, "Process %d of %d is on %s\n", myid,numprocs,processor_name);
    fflush(stdout);
 
  //for(n = 5000000;n<=1000000000; n=n*10)
  //{
        // количество интервалов
        if(myid==0)
        {
			n=2000000000;
			fprintf(stdout, "number of intervals: %d million ", n/1000000 );
            fflush(stdout);
			
            //if(scanf("%d",&n) != 1)
            //{
            //    fprintf(stdout, "No number entered; quitting\n");
            //    n = 0;
            //}
            startwtime = MPI_Wtime();
        }

		// Рассылка количества интервалов всем процессам (в том числе и себе)
        MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
        if(n==0)
            done = 1;
        else
        {
            h = 1.0 / (double) n;
            sum = 0.0;
  // Расчет площадей прямоугольников, закрепленных за процессом
            for(i = myid + 1 ; (i <= n) ; i += numprocs)
            {
                x = h * ((double)i - 0.5);
                sum += (4.0/(1.0 + x*x));
            }
            mypi = h * sum;
 
 // Передача 0-му процессу и итоговое суммирование частных сумм
 // (mypi)процессов в переменной pi    
            MPI_Reduce(&mypi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
 
            // Если это главный процесс, вывод полученного результата
            if(myid==0)
            {
                printf("PI is approximately %.16f, Error is %.16f\n", pi, fabs(pi - PI25DT));
                endwtime = MPI_Wtime();
                printf("wall clock time = %f\n", endwtime-startwtime);
                fflush(stdout);
            }
       }
    // Освобождение подсистемы MPI
    MPI_Finalize();
    return 0;
}	 
