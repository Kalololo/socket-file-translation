//
//  client.cpp
//  111
//
//  Created by 李灏然 on 16/11/20.
//  Copyright © 2016年 李灏然. All rights reserved.
//

#include <netinet/in.h>   // sockaddr_in
#include <sys/types.h>    // socket
#include <sys/socket.h>   // socket
#include <stdio.h>        // printf
#include <stdlib.h>       // exit
#include <string.h>       // bzero
#include <unistd.h>       // close
#include <arpa/inet.h>    // inet_pron
#include <dirent.h>
#include "LOG_INCLUDE.h"
#define SERVER_PORT 8000
#define BUFFER_SIZE 1024
#define FILE_NAME_MAX_SIZE 512
#define FILE_PATH_MAX_SIZE 512
#define PATH_NAME_MAX_SIZE 1024
#define TRANSLATE_FINSHED_FLAG "TRANSLATE_FINSHED_FLAG"
#define FILE_TRANSLATE_FINSHED_FLAG "FILE_TRANSLATE_FINSHED_FLAG"

int main()
{
    // 声明并初始化一个客户端的socket地址结构
    struct sockaddr_in client_addr;
    bzero(&client_addr, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = htons(INADDR_ANY);
    client_addr.sin_port = htons(0);
    
    // 创建socket，若成功，返回socket描述符
    int client_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(client_socket_fd < 0)
    {
        perror("Create Socket Failed:");
        exit(1);
    }
    
    // 绑定客户端的socket和客户端的socket地址结构 非必需
    if(-1 == (bind(client_socket_fd, (struct sockaddr*)&client_addr, sizeof(client_addr))))
    {
        perror("Client Bind Failed:");
        exit(1);
    }
    
    // 声明一个服务器端的socket地址结构，并用服务器那边的IP地址及端口对其进行初始化，用于后面的连接
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    if(inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) == 0)
    {
        perror("Server IP Address Error:");
        exit(1);
    }
    server_addr.sin_port = htons(SERVER_PORT);
    socklen_t server_addr_length = sizeof(server_addr);
    
    // 向服务器发起连接，连接成功后client_socket_fd代表了客户端和服务器的一个socket连接
    if(connect(client_socket_fd, (struct sockaddr*)&server_addr, server_addr_length) < 0)
    {
        perror("Can Not Connect To Server IP:");
        exit(0);
    }
    
    int file_pack_num=0;
    char file_path[FILE_NAME_MAX_SIZE+1];
    bzero(file_path, FILE_NAME_MAX_SIZE+1);
    char file_name[FILE_NAME_MAX_SIZE+1];
    char buffer[BUFFER_SIZE];
    char translate_finshed_flag[BUFFER_SIZE+1];
    bzero(translate_finshed_flag, BUFFER_SIZE+1);
    DIR *dir;
    struct    dirent    *ptr;
    printf("Please Input Directory Name On Server:\t");
    scanf("%s", file_path);
    dir = opendir(file_path);
    while((ptr = readdir(dir)) != NULL) ///read the list of this dir
    {
        // 输入文件名 并放到缓冲区buffer中等待发送
        if (ptr->d_type==4) continue;
        
        bzero(file_name, FILE_NAME_MAX_SIZE+1);
        strncpy(file_name, ptr->d_name, strlen(ptr->d_name));
        printf("file name is:%s\n",file_name);//
        
        bzero(buffer, BUFFER_SIZE);
        strncpy(buffer, file_name, strlen(file_name)>BUFFER_SIZE?BUFFER_SIZE:strlen(file_name));
        printf("in buffer is:%s\n",buffer);//
        // 向服务器发送buffer中的数据
        if(send(client_socket_fd, buffer, BUFFER_SIZE, 0) < 0)
        {
            perror("Send File Name Failed:");
            exit(1);
        }
        
        // 设置文件路径
        char path_name[PATH_NAME_MAX_SIZE+1];
        bzero(path_name, PATH_NAME_MAX_SIZE+1);
        strncpy(path_name, file_path, strlen(file_path));
        strcat(path_name, file_name);
        printf("%s\n", path_name);
        // 打开文件并读取文件数据
        FILE *fp = fopen(path_name, "r");
        if(NULL == fp)
        {
            printf("File:%s Not Found\n", file_name);
        }
        else
        {
            bzero(buffer, BUFFER_SIZE);
            int length = 0;
            // 每读取一段数据，便将其发送给客户端，循环直到文件读完为止
            while((length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0)
            {
                file_pack_num++;
                printf("第%d次长度为%d\n",file_pack_num,length);
                if(send(client_socket_fd, buffer, length, 0) < 0)
                {
                    printf("Send File:%s Failed.\n", file_name);
                    break;
                }
                bzero(buffer, BUFFER_SIZE);
            }
            // 关闭文件
            fclose(fp);
            
            send(client_socket_fd, FILE_TRANSLATE_FINSHED_FLAG, strlen(FILE_TRANSLATE_FINSHED_FLAG), 0);
            printf("File:%s Transfer Successful!\n", file_name);
        }
    }
    bzero(buffer, BUFFER_SIZE);
    int length = strlen(TRANSLATE_FINSHED_FLAG);
    strncpy(buffer, TRANSLATE_FINSHED_FLAG, length);
    send(client_socket_fd, buffer, length, 0);
    closedir(dir);
    close(client_socket_fd);
    return 0;
}
