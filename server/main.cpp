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

        DWORD timeout = 120 * 1000;
        setsockopt(server, SOL_SOCKET, SO_RCVTIMEO, (const char *) &timeout, sizeof timeout);

        bind(server, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
        listen(server, 0);

        cout << "Escuchando para conexiones entrantes." << endl;
        int clientAddrSize = sizeof(clientAddr);
        if((client = accept(server, (SOCKADDR *)&clientAddr, &clientAddrSize)) != INVALID_SOCKET)
        {
            cout << "Cliente conectado!\n" << endl;
        }
    }
    int validarUser(){
        char* user;int flag=0,receiveCode;
        receiveCode = recv(client, buffer, sizeof(buffer), 0);
        if(receiveCode == SOCKET_ERROR){
            return -1;
        }
        user = buffer;
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
                flag = 1;
                cout<<"User correcto!"<<endl;
            }

            getline(credenciales,lector,'\n');
            //strcpy(password,lector.c_str());
        }
        if(flag==0){
            cout<<"User incorrecto!"<<endl;
            strcpy(user,"");
            Enviar(user);
        }
        credenciales.close();
        return flag;
    }
    int validarPassword(){
        char* password;int flag=0,receiveCode;
        receiveCode = recv(client, buffer, sizeof(buffer), 0);
        if(receiveCode == SOCKET_ERROR){
            return -1;
        }
        password = buffer;
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
                flag=1;
                cout<<"Password correcto!\n"<<endl;
            }
        }
        if(flag==0){
            cout<<"Password incorrecto!\n"<<endl;
            strcpy(password,"");
            Enviar(password);
        }
        credenciales.close();
        return flag;
    }
    char* Recibir()
    {
        char* temp; int receiveCode;
        receiveCode = recv(client, buffer, sizeof(buffer), 0);
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
        strcpy(this->buffer,data);
        envio = send(client, buffer, sizeof(buffer), 0);
        //memset(buffer, 0, sizeof(buffer));
        cout << "Mensaje enviado! " << endl;
    }
    void CerrarSocket()
    {
        closesocket(client);
        cout << "Socket cerrado, cliente desconectado." << endl;
    }
    void sendCloseMessage(){
        strcpy(buffer,"closed");
        send(client, buffer, sizeof(buffer), 0);
    }
};


int main()
{
    Server *Servidor = new Server();
    int isPasswordValid = 0, isUserValid = 0,cod,intentos=0,clientAddrSize = sizeof(Servidor->clientAddr);
    //Servidor->Recibir();
    while(true){
        if(intentos < 3){ /** Validacion de los tres intentos de login**/
            isUserValid = Servidor->validarUser();
            if(isUserValid == -1){
                isPasswordValid = -1;
            }else{
                isPasswordValid = Servidor->validarPassword();
            }
            intentos++;
        }
        //memset(Servidor->buffer,0,sizeof(Servidor->buffer));
        if(isUserValid==1 && isPasswordValid==1 && intentos <= 3) /** Si las credenciales son validas puede continuar con el resto del programa**/
        {
            //Servidor->Enviar();
            cod = recv(Servidor->client,Servidor->buffer,1024,0);
            if(cod == 0 || cod == -1){ /** SOCKET_ERROR => -1**/
                //Servidor->CerrarSocket();
                if(closesocket(Servidor->client)==0){
                    cout << "Cliente desconectado!" << endl << "Esperando conexiones entrantes..."<<endl;}
                if((Servidor->client = accept(Servidor->server, (SOCKADDR *)&Servidor->clientAddr, &clientAddrSize)) != INVALID_SOCKET)
                {
                    cout << "Cliente conectado!" << endl;
                    intentos = 0;
                }
            }
        }
        if(isUserValid==-1 || isPasswordValid==-1 || intentos == 3){  /** Si por inactividad del cliente o las credenciales son invalidas se desconecta **/
            Servidor->sendCloseMessage();
            if(closesocket(Servidor->client)==0){
            cout << "Cliente desconectado por inactividad!" << endl << "Esperando conexiones entrantes..."<<endl;}
            if((Servidor->client = accept(Servidor->server, (SOCKADDR *)&Servidor->clientAddr, &clientAddrSize)) != INVALID_SOCKET)
            {
                cout << "Cliente conectado!" << endl;
                intentos = 0;
            }
        }
    }
}
