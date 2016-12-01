//
//  GET_FILE.h
//  client
//
//  Created by 李灏然 on 16/11/27.
//  Copyright © 2016年 李灏然. All rights reserved.
//

#ifndef GET_FILE_h
#define GET_FILE_h



#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>


struct    dirent    *ptr;

DIR *get_file(const char *filepath)
{
    DIR    *dir;
    dir = opendir(filepath); ///open the dir
    return dir;
}

#endif /* GET_FILE_h */
