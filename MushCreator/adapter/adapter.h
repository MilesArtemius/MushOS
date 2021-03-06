#ifndef MUSHFS_WINDOWS_H
#define MUSHFS_WINDOWS_H

#include "../../MushLib/generic.h"

void create_fs();

int read_external(char* name, byte** container);
int insert_header(char* name);

void write_at(int pos, byte info);
byte read_at(int pos);

long get_drive_size();


#endif //MUSHFS_WINDOWS_H
