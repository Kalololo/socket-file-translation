//
//  2.cpp
//  111
//
//  Created by 李灏然 on 16/11/20.
//  Copyright © 2016年 李灏然. All rights reserved.
//

#include <netinet/in.h>  // sockaddr_in
#include <sys/types.h>   // socket
#include <sys/socket.h>  // socket
#include <stdio.h>       // printf
#include <stdlib.h>      // exit
#include <string.h>      // bzero
#include <unistd.h>      // close
#include <iostream>

#define SERVER_PORT 8000
#define LENGTH_OF_LISTEN_QUEUE 20
#define BUFFER_SIZE 1024
#define FILE_NAME_MAX_SIZE 512
#define FILE_PATH_MAX_SIZE 512
#define PATH_NAME_MAX_SIZE 1024
#define FILE_TRANSLATE_FINSHED_FLAG "FILE_TRANSLATE_FINSHED_FLAG"
#define TRANSLATE_FINSHED_FLAG "TRANSLATE_FINSHED_FLAG"

int main(void)
{
    int k=0;
    // 声明并初始化一个服务器端的socket地址结构
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);
    
    // 创建socket，若成功，返回socket描述符
    int server_socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if(server_socket_fd < 0)
    {
        perror("Create Socket Failed:");
        exit(1);
    }
    int opt = 1;
    setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    // 绑定socket和socket地址结构
    if(-1 == (bind(server_socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr))))
    {
        perror("Server Bind Failed:");
        exit(1);
    }
    
    // socket监听
    if(-1 == (listen(server_socket_fd, LENGTH_OF_LISTEN_QUEUE)))
    {
        perror("Server Listen Failed:");
        exit(1);
    }
    
    while(1)
    {
        // 定义客户端的socket地址结构
        struct sockaddr_in client_addr;
        socklen_t client_addr_length = sizeof(client_addr);
        
        // 接受连接请求，返回一个新的socket(描述符)，这个新socket用于同连接的客户端通信
        // accept函数会把连接到的客户端信息写到client_addr中
        int new_server_socket_fd = accept(server_socket_fd, (struct sockaddr*)&client_addr, &client_addr_length);
        if(new_server_socket_fd < 0)
        {
            perror("Server Accept Failed:");
            break;
        }

        // recv函数接收数据到缓冲区buffer中
        char buffer[BUFFER_SIZE];
        bzero(buffer, BUFFER_SIZE);
        if(recv(new_server_socket_fd, buffer, BUFFER_SIZE, 0) < 0)
        {
            perror("Server Recieve Data Failed:");
            break;
        }
        
        // 然后从buffer(缓冲区)拷贝到file_name中
        char file_name[FILE_NAME_MAX_SIZE+1];
        char path_name[PATH_NAME_MAX_SIZE+1];
        char file_path[FILE_PATH_MAX_SIZE+1];
        bzero(file_name, FILE_NAME_MAX_SIZE+1);
        strncpy(file_name, buffer, strlen(buffer)>FILE_NAME_MAX_SIZE?FILE_NAME_MAX_SIZE:strlen(buffer));
        printf("%s\n", file_name);
        
        while (strcmp(file_name, TRANSLATE_FINSHED_FLAG)!=0)
        {
            // 设置文件储存路径s
            strcpy(file_path, "/Users/lihaoran/Desktop/test2/");
            strcpy(path_name, file_path);
            strcat(path_name, file_name);
            printf("%s\n", path_name);
            
            // 打开文件，准备写入
            FILE *fp = fopen(path_name, "w");
            if(NULL == fp)
            {
                printf("File:\t%s Can Not Open To Write\n", file_name);
                exit(1);
            }
            
            // 从服务器接收数据到buffer中
            // 每接收一段数据，便将其写入文件中，循环直到文件接收完并写完为止
            bzero(buffer, BUFFER_SIZE);
            int length = 0;
            while ((length = recv(new_server_socket_fd, buffer, BUFFER_SIZE, 0)) > 0)
            {
                printf("length=%d\n",length);
                printf("%s\n",buffer);
                if (strcmp(buffer, FILE_TRANSLATE_FINSHED_FLAG)==0)
                {
                    bzero(buffer, BUFFER_SIZE);
                    break;
                }
                if(fwrite(buffer, sizeof(char), length, fp) < length)
                {
                    printf("File:\t%s Write Failed\n", file_name);
                    break;
                }
                bzero(buffer, BUFFER_SIZE);  
            }
            fclose(fp);
            recv(new_server_socket_fd, buffer, BUFFER_SIZE, 0);
            printf("%d\n",strlen(buffer));
            printf("in buffer is:%s\n",buffer);
            bzero(file_name, FILE_NAME_MAX_SIZE+1);
            strncpy(file_name, buffer, strlen(buffer)>FILE_NAME_MAX_SIZE?FILE_NAME_MAX_SIZE:strlen(buffer));
            printf("kkk%s\n", file_name);
        }
        close(new_server_socket_fd);
    }
    // 关闭监听用的socket
    close(server_socket_fd);
    return 0;  
}
