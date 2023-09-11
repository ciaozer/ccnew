#include "build.h"
#include "heuristic.h"
#include "data_structure.h"
#include"parse.h"
#include<signal.h>

void interrupt(int sig)
{
	if (check_answer() == 1)
		cout << "c verified" << endl;
	print_best_solution();
	free_memory();
	exit(10);
}

int main(int argc, char* argv[])
{
    signal(SIGTERM, interrupt);
	times(&start);
	parse_args(argc, argv);

    return 0;
}
