#ifndef _H_FILERPL_
#define _H_FILERPL_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FILERPL_ERROR_PARAMETER		-11
#define FILERPL_ERROR_MALLOC		-21
#define FILERPL_ERROR_REALLOC		-22
#define FILERPL_ERROR_OPEN		-31
#define FILERPL_ERROR_FSTAT		-32
#define FILERPL_ERROR_READ		-33
#define FILERPL_ERROR_MMAP		-34
#define FILERPL_ERROR_WRITE		-35
#define FILERPL_ERROR_MAP_INVALID_1	-101
#define FILERPL_ERROR_MAP_INVALID_2	-102
#define FILERPL_ERROR_MAP_INVALID_3	-103
#define FILERPL_ERROR_MAP_INVALID_4	-104
#define FILERPL_ERROR_MAP_INVALID_5	-105
#define FILERPL_ERROR_MAP_INVALID_6	-106


int filerpl( char *tpl_pathfilename , char *map_pathfilename , char *ins_pathfilename );

int strrpl( char **pp_buf , int *p_buf_len , int *p_buf_size , char *key , int key_len , char *value , int value_len );

int LoadRplTemplateFile( char *tpl_pathfilename , char **pp_tpl_buf , int *p_tpl_buf_len , int *p_tpl_buf_size );
int LoadRplMappingFile( char *map_pathfilename , char **pp_map_buf , int *p_map_buf_len , int *p_map_buf_size );
int ConvertRplBuffer( char **pp_ins_buf , int *p_ins_buf_len , int *p_ins_buf_size , char *p_map_buf , int map_buf_len );
int DumpRplInstanceFile( char *p_ins_buf , int ins_buf_len , char *ins_pathfilename );

int FreeRplBuffer( char **pp_buf , int *p_buf_len , int *p_buf_size );

#ifdef __cplusplus
}
#endif

#endif

