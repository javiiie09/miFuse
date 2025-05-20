#ifndef _UTILS_H
#define _UTILS_H

extern char* split_path(const char* path, char* dir_path, char* next_part);

extern char* split_parent_path(const char* path, char* parent_path);

#endif // _UTILS_H