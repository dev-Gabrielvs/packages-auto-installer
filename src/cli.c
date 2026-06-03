#include <stdio.h>
#include <getopt.h>
#include "installer.h"
#include "package_manager.h"
#include <string.h>

struct option long_options[] = {
    {"help", no_argument, 0, 'h'},
    {"list-packages", no_argument, 0, 'l'},
    {"add-package", no_argument, 0, 'a'},
    {"update-package", no_argument, 0, 'u'},
    {"delete-package", no_argument, 0, 'd'},
    {"install-packages", no_argument, 0, 'i'},
    {"install-packages-verbose", no_argument, 0, 'v'},
    {0, 0, 0, 0}
};

int help_info ();
int add_package_cli ();
int update_package_cli ();
int delete_package_cli ();

// --------------------------------------------------- Função principal do CLI ---------------------------------------------------

int cli_run (int argc, char *argv[]) {
    int option_index = 0;
    int c;

    while ((c = getopt_long(argc, argv, "hlaudiv", long_options, &option_index)) != -1) {
        switch (c) {
            case 'h':
                help_info();
                return 0;
            case 'l':
                pm_list();
                return 0;
            case 'a':
                add_package_cli();
                return 0;
            case 'u':
                update_package_cli();
                return 0;
            case 'd':
                delete_package_cli();
                return 0;
            case 'i':
                return run_installer(0);
            case 'v':
                return run_installer(1);
            default:
                fprintf(stderr, "Usage: %s [options]\n", argv[0]);
                return 1;
        }
    }


    return 0;
}

// --------------------------------------------------- Helpers CLI ---------------------------------------------------

int help_info () {
    printf("Usage: [options]\n");
    printf("Options:\n");
    printf("  -h, --help     Show this help message\n");
    printf("  -l, --list-packages  List available packages\n");
    printf("  -a, --add-package  Add a new package\n");
    printf("  -u, --update-package  Update an existing package\n");
    printf("  -d, --delete-package  Delete a package\n");
    printf("  -i, --install-packages  Install packages\n");
    printf("  -v, --install-packages-verbose  Install packages with verbose output\n");
    return 0;
}

int add_package_cli () {
    char name[100], description[255], installer_cmd[255];

    printf("Digite o nome do pacote: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0; 

    printf("Digite a descrição do pacote: ");
    fgets(description, sizeof(description), stdin);
    description[strcspn(description, "\n")] = 0; 

    printf("Digite o comando de instalação do pacote: ");
    fgets(installer_cmd, sizeof(installer_cmd), stdin);
    installer_cmd[strcspn(installer_cmd, "\n")] = 0;

    pm_add(name, description, installer_cmd);

    return 0;
}

int update_package_cli () {
    int id;
    char new_name[100], new_description[255], new_installer_cmd[255];

    printf("Digite o ID do pacote a ser atualizado: ");
    scanf("%d", &id);
    getchar();

    printf("Digite o novo nome do pacote: ");
    fgets(new_name, sizeof(new_name), stdin);
    new_name[strcspn(new_name, "\n")] = 0;

    printf("Digite a nova descrição do pacote: ");
    fgets(new_description, sizeof(new_description), stdin);
    new_description[strcspn(new_description, "\n")] = 0;

    printf("Digite o novo comando de instalação do pacote: ");
    fgets(new_installer_cmd, sizeof(new_installer_cmd), stdin);
    new_installer_cmd[strcspn(new_installer_cmd, "\n")] = 0;

    if (pm_update(id, new_name, new_description, new_installer_cmd) == -1) {
        fprintf(stderr, "Erro: pacote com ID %d não encontrado.\n", id);
        return 1;
    }

    printf("Pacote %d atualizado com sucesso.\n", id);
    return 0;
}

int delete_package_cli () {
    int id;
    printf("Digite o ID do pacote a ser removido: ");
    scanf("%d", &id);
    getchar();

    if (pm_remove(id) == -1) {
        fprintf(stderr, "Erro: pacote com ID %d não encontrado.\n", id);
        return 1;
    }

    printf("Pacote %d removido com sucesso.\n", id);
    return 0;
}