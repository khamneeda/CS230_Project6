int main(int argc, char *argv[]){

  int serv_sock; // server socket
  int clnt_sock; // client socket
  if(argc != 3 || strcmp(argv[1], "-p")){
    printf("invalid command line\n");
    printf("./server -p <port>\n");
    exit(1);
  }
  
  if((serv_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    printf("socket error\n");
    exit(1);
  }
  
  // Bind the server socket .
  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(8080);
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  //serv_addr.sin_zero = 0;
  
  if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1){
    printf("bind error\n");/
    exit(1);
  }

  // Set the server socket to listening mode.
  printf("Server start: listen on port %s\n", argv[2]);
  if(listen(serv_sock, 50) == -1){ 
    printf("listen error\n");
    exit(1);
  }

  // Accept client sockets if they request to accept.
  struct sockaddr_in clnt_addr;
  socklen_t clnt_addr_size = sizeof(clnt_addr);
  clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_addr, &clnt_addr_size);
  
  if(clnt_sock == -1){
    printf("accept error\n");
    exit(1);
  }

    //dir_change
    changedir("server_files");

    //Listening...
    while(True){

    //file_type recv
    char type_buf[256];
    recv(clnt_sock, type_buf, sizeof(type_buf), 0);

    // upload   file_type check
    if(type_buf[0] == 1){

    //file_name recv
    char name_buf[256];
    recv(clnt_sock, name_buf, sizeof(name_buf), 0);

    //file_size recv    
    int count_buf[256];
    recv(clnt_sock, count_buf, sizeof(count_buf), 0);
    //printf("Received count: %d\n", count_buf[0] - 1);

    //file write
    char file_buf[count_buf[0]];
    
    FILE *up_file = fopen(name_buf, "wb");
    recv(clnt_sock, file_buf, sizeof(file_buf), 0);
    fputs(file_buf, up_file);
    
    fclose(up_file);



    }






    // download         
    else if(type_buf[0] == 2){

    //name recv
    char name_buf[256];
    recv(clnt_sock, name_buf, sizeof(name_buf), 0);

    //exist_check
    char exist_buf[256];
    if (access(name_buf, F_OK) == -1){
        exist_buf[0] = 0; //False
    } else {
        exist_buf[0] = 1; //True
    }
    send(clnt_sock, exist_buf, sizeof(exist_buf), 0);

    //size_check
    int count = 0;
    char size_buf[256];
    FILE* count_file = fopen(name_buf, "rb");
    while (feof(count_file) == 0){
    fread(size_buf, sizeof(char), 256, count_file);       
    count++;
    size_buf[256];
    }
    fclose(count_file);

    //size_send
    int count_arr[256];
    count_arr[0] = count;
    send(clnt_sock, count_arr, sizeof(count_arr), 0);

    //file_send
    char data_buf[256];
    FILE* sending_file = fopen(name_buf, "rb");
    for (int i = 0; i < count; i++){
    fread(data_buf, sizeof(char), 256, sending_file);
    send(clnt_sock, data_buf, sizeof(data_buf), 0);
    data_buf[256] = {0, };
    }
    
    fclose(sending_file);

    //file_del
    int rem_res = remove(name_buf);
    if (rem_res == -1){
        perror("upload removing failed");
        exit(1);
    }
    }


    // Terminate
    else if(type_buf[0] == 3){

    //socket close
    close(serv_sock);
    close(clnt_sock);

    }
    //else exit(1);
  }

}
