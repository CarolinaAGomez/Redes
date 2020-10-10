#include <iostream>
#include <winsock2.h>
#include <string>
using namespace std;

class Client{
public:
    WSADATA WSAData;
    SOCKET server;
    SOCKADDR_IN addr;
    char buffer[1024];
    int isocket;
    Client(){}
    int _init(char* ip, short unsigned int port){
        cout<<"Conectando al servidor..."<<endl<<endl;
        isocket = WSAStartup(MAKEWORD(2,0), &WSAData);
        server = socket(AF_INET, SOCK_STREAM, 0);
        if (server == INVALID_SOCKET) {
            cout<<"socket function failed with error: "<<WSAGetLastError()<<endl;
            WSACleanup();
        }
        addr.sin_addr.s_addr = inet_addr(ip);
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        isocket = connect(server, (SOCKADDR *)&addr, sizeof(addr));
        if(isocket == SOCKET_ERROR){
            cout<<"connect function failed with error: "<<WSAGetLastError()<<endl;
            isocket = closesocket(server);
            if (isocket == SOCKET_ERROR){
                cout<<"closesocket function failed with error: "<<WSAGetLastError()<<endl;
            }
            WSACleanup();
            return -1;
        }else{
            cout << "Conectado al Servidor!" << endl;
            return 0;
        }
        system("pause");system("cls");
    }
    int login(){
        char user[12],password[12];
        cout<<"Ingrese el nombre de usuario: ";
        fflush(stdin);
        cin>>user;
        cout<<"Ingrese contraseña: ";
        fflush(stdin);
        cin>>password;
        Enviar(user);
        strcpy(user,Recibir());
        cout<<"Usuario recibido: "<<user<<endl;
        Enviar(password);
        strcpy(password,Recibir());
        cout<<"Password recibido: "<<password<<endl;
        if(strcmp(user,"")!=0/*user != ""*/ && strcmp(password,"")!=0/*password != ""*/){
            return 0;
        }else{
            return -1;
        }
    }
    void Enviar(char data[])
    {
        int envio;
        //cout<<"Escribe el mensaje a enviar: ";
        //cin>>this->buffer;
        strcpy(this->buffer,data);
        envio = send(server, buffer, sizeof(buffer), 0);
        //memset(buffer, 0, sizeof(buffer));
        //cout << "Mensaje enviado!" << endl;
    }
    char* Recibir()
    {
        char* temp;
        recv(server, buffer, sizeof(buffer), 0);
        temp = buffer;
        //cout << "El servidor dice: " << buffer << endl;
        //memset(buffer, 0, sizeof(buffer));
        return temp;
    }
    void CerrarSocket()
    {
       closesocket(server);
       WSACleanup();
       cout << "Socket cerrado." << endl << endl;
    }
};

int main()
{
    char ip[12]=""; short unsigned int port; int loginSuccess,intentos=3;
    Client *Cliente = new Client();
    do{
        cout<<"Ingrese la IP del servidor para conectarse: ";
        fflush(stdin);
        cin>>ip;
        cout<<"\nIngrese un puerto: ";
        cin>>port;
    }while(Cliente->_init(ip,port)!=0);
        //Cliente->Enviar();
    while(intentos!=0){
        loginSuccess = Cliente->login();
        if(loginSuccess == 0){
            break;
        }else{
            intentos--;
            cout<<"Reintentos restantes: "<<intentos<<endl;
            system("pause");system("cls");
        }
    }
    if(intentos == 0){
        cout<<"Cliente desconectado por varios intentos fallidos."<<endl;
        system("pause");
        Cliente->CerrarSocket();
    }else{
        cout<<"Login successful"<<endl;
        system("pause");
        Cliente->CerrarSocket();
    }
    //system("pause");system("cls");

    //Client *Cliente = new Client(ip,port);

    return 0;
}
