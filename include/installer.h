#ifndef INSTALLER_H
#define INSTALLER_H

int install_package(const char *name, const char *cmd, int verbose);
int run_installer(int verbose);

#endif