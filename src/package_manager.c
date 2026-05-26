#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../include/cJSON.h"
#include "../include/package_manager.h"

static cJSON *load_file(void)
{
    FILE *f = fopen(PACKAGES_FILE, "r");
    if (!f)
    {
        cJSON *root = cJSON_CreateObject();
        cJSON_AddArrayToObject(root, "packages");
        return root;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    char *buf = malloc(size + 1);
    fread(buf, 1, size, f);
    buf[size] = '\0';
    fclose(f);

    cJSON *root = cJSON_Parse(buf);
    free(buf);
    return root;
}

// Serializa e salva o JSON no arquivo
static void save_file(cJSON *root)
{
    char *str = cJSON_Print(root);
    FILE *f = fopen(PACKAGES_FILE, "w");
    if (!f) {
        perror("save_file: fopen");
        free(str);
        return;
    }
    fprintf(f, "%s\n", str);
    fclose(f);
    free(str);
}

// Gera próximo id como max(id existente) + 1
static int next_id(cJSON *packages)
{
    int max = 0;
    cJSON *pkg;
    cJSON_ArrayForEach(pkg, packages)
    {
        cJSON *id = cJSON_GetObjectItem(pkg, "id");
        if (id && id->valueint > max)
            max = id->valueint;
    }
    return max + 1;
}

// CREATE
int pm_add(const char *name, const char *description, const char *installer_cmd)
{
    cJSON *root = load_file();
    cJSON *packages = cJSON_GetObjectItem(root, "packages");

    cJSON *pkg = cJSON_CreateObject();
    cJSON_AddNumberToObject(pkg, "id", next_id(packages));
    cJSON_AddStringToObject(pkg, "name", name);
    cJSON_AddStringToObject(pkg, "description", description);
    cJSON_AddStringToObject(pkg, "installer-cmd", installer_cmd);
    cJSON_AddItemToArray(packages, pkg);

    save_file(root);
    cJSON_Delete(root);
    return 0;
}

// READ
void pm_list(void)
{
    cJSON *root = load_file();
    cJSON *packages = cJSON_GetObjectItem(root, "packages");
    cJSON *pkg;

    cJSON_ArrayForEach(pkg, packages)
    {
        cJSON *id   = cJSON_GetObjectItem(pkg, "id");
        cJSON *name = cJSON_GetObjectItem(pkg, "name");
        cJSON *desc = cJSON_GetObjectItem(pkg, "description");
        printf("[%d] %s — %s\n",
            id   ? id->valueint          : -1,
            name ? name->valuestring     : "?",
            desc ? desc->valuestring     : "?");
    }

    cJSON_Delete(root);
}

// UPDATE
int pm_update(int id, const char *new_name, const char *new_description, const char *new_installer_cmd)
{
    cJSON *root     = load_file();
    cJSON *packages = cJSON_GetObjectItem(root, "packages");
    cJSON *pkg;

    cJSON_ArrayForEach(pkg, packages)
    {
        cJSON *pkg_id = cJSON_GetObjectItem(pkg, "id");
        if (pkg_id && pkg_id->valueint == id)
        {
            cJSON_ReplaceItemInObject(pkg, "name",          cJSON_CreateString(new_name));
            cJSON_ReplaceItemInObject(pkg, "description",   cJSON_CreateString(new_description));
            cJSON_ReplaceItemInObject(pkg, "installer-cmd", cJSON_CreateString(new_installer_cmd));
            save_file(root);
            cJSON_Delete(root);
            return 0;
        }
    }

    cJSON_Delete(root);
    return -1; // não encontrado
}

// GET INSTALLER CMDS
int pm_get_installer_cmds(char names[][100], char cmds[][255], int max)
{
    cJSON *root = load_file();
    cJSON *packages = cJSON_GetObjectItem(root, "packages");
    int count = 0;
    cJSON *pkg;

    cJSON_ArrayForEach(pkg, packages)
    {
        if (count >= max) break;
        cJSON *name = cJSON_GetObjectItem(pkg, "name");
        cJSON *cmd  = cJSON_GetObjectItem(pkg, "installer-cmd");
        if (name && cmd) {
            strncpy(names[count], name->valuestring, 99);
            names[count][99] = '\0';
            strncpy(cmds[count], cmd->valuestring, 254);
            cmds[count][254] = '\0';
            count++;
        }
    }

    cJSON_Delete(root);
    return count;
}

// DELETE
int pm_remove(int id)
{
    cJSON *root = load_file();
    cJSON *packages = cJSON_GetObjectItem(root, "packages");
    int i = 0;
    cJSON *pkg;

    cJSON_ArrayForEach(pkg, packages)
    {
        cJSON *pkg_id = cJSON_GetObjectItem(pkg, "id");
        if (pkg_id && pkg_id->valueint == id)
        {
            cJSON_DeleteItemFromArray(packages, i);
            save_file(root);
            cJSON_Delete(root);
            return 0;
        }
        i++;
    }

    cJSON_Delete(root);
    return -1; // não encontrado
}