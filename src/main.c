#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "cli.h"
#include "logger.h"

static void handle_sigint(int sig)
{
    (void)sig;
    log_write("INFO", "Programa encerrado pelo usuário");
    fprintf(stderr, "\nEncerrado pelo usuário.\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    signal(SIGINT, handle_sigint);
    int result = cli_run(argc, argv);
    return result == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
