#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "installer.h"
#include "package_manager.h"
#include "logger.h"

int install_package(const char *name, const char *cmd, int verbose)
{
    char full_cmd[512];
    snprintf(full_cmd, sizeof(full_cmd), "%s 2>&1", cmd);

    if (verbose)
        printf("==> Instalando: %s\n", name);

    FILE *fp = popen(full_cmd, "r");
    if (!fp) {
        perror("install_package: popen");
        return -1;
    }

    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        if (verbose)
            printf("%s", line);
    }

    int status = pclose(fp);
    int result = (status == 0) ? 0 : -1;
    log_write(result == 0 ? "SUCCESS" : "ERROR", name);
    return result;
}

int run_installer(int verbose)
{
    char names[50][100];
    char cmds[50][255];

    int total = pm_get_installer_cmds(names, cmds, 50);
    if (total == 0) {
        printf("Nenhum pacote cadastrado.\n");
        return 0;
    }

    int successes = 0;
    int failures = 0;

    for (int i = 0; i < total; i++) {
        int result = install_package(names[i], cmds[i], verbose);
        if (result == 0)
            successes++;
        else
            failures++;
    }

    if (!verbose)
        printf("Download dos pacotes foram concluídos com %d falhas e %d sucessos.\n", failures, successes);

    return 0;
}
