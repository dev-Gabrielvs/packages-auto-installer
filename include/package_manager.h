#ifndef PACKAGE_MANAGER_H
#define PACKAGE_MANAGER_H

#define PACKAGES_FILE "db/packages.json"

// CREATE
int pm_add(const char *name, const char *description, const char *installer_cmd);

// READ
void pm_list(void);
int pm_find(const char *name); // retorna id ou -1
int pm_get_installer_cmds(char names[][100], char cmds[][255], int max);

// UPDATE
int pm_update(int id, const char *new_name, const char *new_description, const char *new_installer_cmd);

// DELETE
int pm_remove(int id);

#endif