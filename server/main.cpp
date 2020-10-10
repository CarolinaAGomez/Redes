#include <iostream>
#include <winsock2.h>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

class Server{
public:
    WSADATA WSAData;
    SOCKET server, client;
    SOCKADDR_IN serverAddr, clientAddr;
    char buffer[1024];
    Server()
    {
        WSAStartup(MAKEWORD(2,0), &WSAData);
        server = socket(AF_INET, SOCK_STREAM, 0);

        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(5555);

        bind(server, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
        listen(server, 0);

        cout << "Escuchando para conexiones entrantes." << endl;
        int clientAddrSize = sizeof(clientAddr);
        if((client = accept(server, (SOCKADDR *)&clientAddr, &clientAddrSize)) != INVALID_SOCKET)
        {
            cout << "Cliente conectado!\n" << endl;
        }
    }
    bool validarUser(){
        char* user;bool flag=false;
        user = Recibir();
        cout<<"Usuario recibido"<<endl;
        ifstream credenciales;
        credenciales.open("Credenciales.log",ios::in);
        string lector;
        //stringstream ss;
        while(!credenciales.eof() && !flag){
            getline(credenciales,lector,';');
            //strcpy(user,lector.c_str());
            if(strcmp(user,lector.c_str())==0){
                Enviar(user);
                flag = true;
                cout<<"User correcto!"<<endl;
            }

            getline(credenciales,lector,'\n');
            //strcpy(password,lector.c_str());
        }
        if(!flag){
            cout<<"User incorrecto!"<<endl;
            strcpy(user,"");
            Enviar(user);
        }
        credenciales.close();
        return flag;
    }
    bool validarPassword(){
        char* password;bool flag=false;
        password = Recibir();
        cout<<"Password recibido"<<endl;
        ifstream credenciales;
        credenciales.open("Credenciales.log",ios::in);
        string lector;
        //stringstream ss;
        while(!credenciales.eof() && !flag){
            getline(credenciales,lector,';');

            getline(credenciales,lector,'\n');
            //strcpy(password,lector.c_str());
            if(strcmp(password,lector.c_str())==0){
                Enviar(password);
                flag=true;
                cout<<"Password correcto!\n"<<endl;
            }
        }
        if(!flag){
            cout<<"Password incorrecto!\n"<<endl;
            strcpy(password,"");
            Enviar(password);
        }
        credenciales.close();
        return flag;
    }
    char* Recibir()
    {
        char* temp;
        recv(client, buffer, sizeof(buffer), 0);
        temp = buffer;
        //cout << "El cliente dice: " << buffer << endl;
        //memset(buffer, 0, sizeof(buffer));
        return temp;
    }
    void Enviar(char data[])
    {
        int envio;
        //cout<<"Escribe el mensaje a enviar: ";
        //cin>>this->buffer;
        envio = send(client, buffer, sizeof(buffer), 0);
        //memset(buffer, 0, sizeof(buffer));
        cout << "Mensaje enviado! " << envio << endl;
    }
    void CerrarSocket()
    {
        closesocket(client);
        cout << "Socket cerrado, cliente desconectado." << endl;
    }
};


int main()
{
    Server *Servidor = new Server();
    bool isPasswordValid = false, isUserValid = false; int cod,intentos=0,clientAddrSize = sizeof(Servidor->clientAddr);
    //Servidor->Recibir();
    while(true){
        if(intentos < 3){
            isUserValid = Servidor->validarUser();
            isPasswordValid = Servidor->validarPassword();
            intentos++;
        }
        //memset(Servidor->buffer,0,sizeof(Servidor->buffer));
        if(isUserValid && isPasswordValid && intentos <= 3)
        {
            //Servidor->Enviar();
            cod = recv(Servidor->client,Servidor->buffer,1024,0);
            if(cod == 0 || cod == -1){
                cout << "Cliente desconectado!" << endl << "Esperando conexiones entrantes..."<<endl;
                if((Servidor->client = accept(Servidor->server, (SOCKADDR *)&Servidor->clientAddr, &clientAddrSize)) != INVALID_SOCKET)
                {
                    cout << "Cliente conectado!" << endl;
                    intentos = 0;
                    break;
                }
            }
        }
        if(intentos == 3){
            cout << "Cliente desconectado!" << endl << "Esperando conexiones entrantes..."<<endl;
            if((Servidor->client = accept(Servidor->server, (SOCKADDR *)&Servidor->clientAddr, &clientAddrSize)) != INVALID_SOCKET)
            {
                cout << "Cliente conectado!" << endl;
                intentos = 0;
            }
        }
    };
}
