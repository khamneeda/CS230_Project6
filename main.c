int main (int argc, char *argv[])
{  
      int sock = init(argc, argv);
      changedir("client_files");
      ls();
      if (upload(sock, "image1.jpg") == True)
        printf("send success\n");
      
      changedir("../server_files");
      changedir("../client_files");

      if (download(sock, "image2.jpg") == True)
        printf("recv success\n");
      changedir("../server_files");
      ls();
  
}