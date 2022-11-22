// Initialize a client socket.
int init(int argc, char *argv[]){

  int sock;
  if(argc != 5 || strcmp(argv[1], "-h") || strcmp(argv[3], "-p")){
    printf("unvalid command line\n");
    printf("./client -h <address> -p <port>\n");
    exit(1);
  }

  // client socket initialization
  if ((sock = socket(AF_INET,SOCK_STREAM, 0)) == -1){
    perror("sock making error\n");
    return False;
  }
  
  // serv_addr setting
  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serv_addr.sin_port = htons(8080);
  
  // connection test
  if(connect(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1){
    printf("My connect error in client.c\n");
    return False;
  }
  return sock; // return initialized socket
}

void ls(){
  char* load = (char*) malloc(sizeof(char)* 256);
  DIR* dir = NULL;
  struct dirent* file_list = NULL;
  getcwd(load, 256);
  
  if (NULL == (dir = opendir(load))){
        perror("lserror");
        exit(1);
    }

    while(NULL != (file_list = readdir(dir))){
        printf("%s, ", file_list->d_name);
    }
    printf("\n");

}

// Change the current directory to target directory.
void changedir(const char* name){ 
  int tmp = chdir(name);
  if (tmp != 0){
    perror("changedir error");
    exit(1);
  }
  return;
}

// Input : socket & file_name you want to upload to server
// Return : if the socket success to upload the accesible file, return "True". Otherwise, return "False".
int upload(int sock, const char* file_name){ //command: upload:1, download:2, terminate:3
    //changedir("../client_files");
    //ls();

    if (access(file_name, F_OK) == -1){
        return False;
    }                          

    //function type send
    char type_buf[256];
    type_buf[0] = 1;
    send(sock, type_buf, sizeof(type_buf), 0);

    //file_name send
    send(sock, file_name, sizeof(type_buf), 0);

    //size check
    int count = 0;
    char size_buf[1];
    FILE* count_file = fopen(file_name, "rb");
    while (feof(count_file) == 0){
    fread(size_buf, sizeof(char), 1, count_file);
    count++;
    size_buf[1] = {0};
    }
    fclose(count_file);
    //printf("Count: %d\n", count);

    //size send
    int count_arr[256];
    count_arr[0] = count;
    send(sock, count_arr, sizeof(count_arr), 0);

    //file send
    char data_buf[count];
    FILE* sending_file = fopen(file_name, "rb");
    fread(data_buf, sizeof(char), count, sending_file);
    send(sock, data_buf, sizeof(data_buf), 0);
    
    fclose(sending_file);
    
    //file del
    int rem_res = remove(file_name);
    if (rem_res == -1){
    perror("uploaded file removing failed");
    exit(1);
    }
    
    
    return True;

}



// Input : socket & file_name you want to download from server
// Return : if the socket success to download the accessible file, return "True". Otherwise, return "False".
int download(int sock, const char* file_name){
    //dir change
    //changedir("../client_files");

    //type send: download
    char type_buf[256];
    type_buf[0] = 2;
    send(sock, type_buf, sizeof(type_buf), 0);

    //name send
    send(sock, file_name, sizeof(type_buf), 0);

    //existence recv, return
    char exist_buf[256];
    recv(sock, exist_buf, sizeof(exist_buf), 0);
    if (exist_buf[0] == 0){
        return False;
    }

    //size recv
    int count_buf[256];
    recv(sock, count_buf, sizeof(count_buf), 0);

    //file recv
    char file_buf[256];
    FILE *down_file = fopen(file_name, "wb");
    for (int i = 0; i < count_buf[0]; i++){
        recv(sock, file_buf, sizeof(file_buf), 0);
        fputs(file_buf, down_file);
        file_buf[256] = {0, };
    }

    return True;
    
}

// Terminate function should send the terminate signal to server.
void terminate(int sock){      

    //send terminate signal
    char type_buf[256];
    type_buf[0] = 3;
    send(sock, type_buf, sizeof(type_buf), 0);

    //socket close
    close(sock);


}
