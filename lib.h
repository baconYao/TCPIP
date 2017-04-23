void error(char *);                                 //顯示error msg並結束程式
float calculate_throughput(int, int, float);        //計算throughput
void check_parameters_number(int, int, char **);    //檢查參數是否正確
int read_in(int, char *, int);                      //讀取data(recv function)
int create_socket();                                 //建立socket
void bind_to_port(int , int);                       //幫定埠號
