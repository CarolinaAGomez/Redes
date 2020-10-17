#include <iostream>
#include <winsock2.h>
#include <fstream>
#include <sstream>
#include <string>
#include <windows.h>
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
    int validarCredenciales(){
        memset(buffer, 0, sizeof(buffer));
        char user[12]="\0",password[12]="\0";int flag=0,receiveCode;
        receiveCode = recv(client, buffer, sizeof(buffer), 0);
        if(receiveCode == SOCKET_ERROR){
            return -1;
        }
        strcpy(user,buffer);
        //user = buffer;
        recv(client, buffer, sizeof(buffer), 0);
        //password = buffer;
        strcpy(password,buffer);
        //cout<<"Usuario y contrase�a recibidos"<<endl;
        ifstream credenciales;
        credenciales.open("Credenciales.log",ios::in);
        string lector;
        //stringstream ss;
        while(!credenciales.eof() && !flag){
            getline(credenciales,lector,';');
            //strcpy(user,lector.c_str());
            if(strcmp(user,lector.c_str())==0){
                getline(credenciales,lector,'\n');
                if(strcmp(password,lector.c_str())==0){
                    Enviar("LOGIN_VALID");
                    cout<<"User y password correctos!\n";
                    flag = 1;
                }
            }else{
                getline(credenciales,lector,'\n');
            }
        }
        if(flag==0){
            Enviar("LOGIN_INVALID");
            cout<<"Credenciales incorrectas!"<<endl;
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
    void Enviar(const char* data)
    {
        int envio;
        //cout<<"Escribe el mensaje a enviar: ";
        //cin>>this->buffer;
        strcpy(this->buffer,data);
        envio = send(client, buffer, sizeof(buffer), 0);
        //memset(buffer, 0, sizeof(buffer));
        //cout << "Mensaje enviado! " << endl;
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




 int validarAltas(){    /** Devuelve -1 => Si se desconecto el cliente, 0 => Di� de alta servicio si no lo encontro, 1 => Si encontro el servicio**/
        memset(buffer,0,sizeof(buffer));
        char* alta;int flag=0,receiveCode;
        receiveCode = recv(client, buffer, sizeof(buffer), 0); //Recibe lo de cliente.
        if(receiveCode == SOCKET_ERROR || receiveCode == 0){
            return -1;
        }
        alta = buffer;

        ifstream altas;
        altas.open("Altas.txt",ios::in);
        string lector;
        while(!altas.eof() && !flag){
            getline(altas,lector,'\n');
            //strcpy(user,lector.c_str());

            if(strcmp(alta,lector.c_str())==0){
            cout<<"ENVIO DATOS DEL SERVICIO ENCONTRADO\n"<<alta;
            Enviar("SE ENCONTRO EL SERVICIO SELECCIONADO");

            flag = 1;

            }

        }
        if(flag==0){
            cout<<"NO ENCONTRO SERVICIO SOLICITADO!\n"<<endl;
           // strcpy(alta,"NO ENCONTRO SERVICIO SOLICITADO, SE CREARA SERVICIO\n");
            alta = buffer;
        cout<<"EL ALTA DEL SERVICIO A CREAR ES: "<<alta<<"\n";

        ofstream altas;
        altas.open("Altas.txt",ios::app);
        altas<<"\n"<<alta;
        cout<<"SE DIO DE ALTA EL SERVICIO: "<<alta<<"\n";
        Enviar("SE DIO DE ALTA EL SERVICIO\n");

        }
        altas.close();
        return flag;
    }
/*
int ValidarTurno(){

 char* turno;int flag=0,receiveCode;
        receiveCode = recv(client, buffer, sizeof(buffer), 0); //Recibe lo de cliente.
        if(receiveCode == SOCKET_ERROR){
            return -1;
        }

        turno = buffer;
        cout<<"TURNO:"<<turno<<"\n"<<endl;
        ifstream altas;
        altas.open("Altas.txt",ios::in);
        string lector;

        while(!altas.eof() && !flag){
            getline(altas,lector,';');
            //strcpy(user,lector.c_str());
 cout<<"turno date es:"<<turno<<"turno lector es:"<<lector<<"\n";
            if(strcmp(turno,lector.c_str())==0){

                Enviar(turno);
                cout<<"ENVIA TURNO\n";
                cout<<"TURNO EXISTE\n"<<endl;
                flag = 1;

            }

            //getline(altas,lector,';');

            //strcpy(password,lector.c_str());
        }
        if(flag==0){
            cout<<"NO ENCONTRO TURNO!\n"<<endl;
            strcpy(turno,"NO ENCONTRO TURNO");
            Enviar(turno);
        }
        altas.close();
        return flag;
}



*/
/*
 int crearServicio(){
     cout<<"entro a crearservicio\n";

        char* alta;int flag=0,receiveCode;
        receiveCode = recv(client, buffer, sizeof(buffer), 0); //Recibe lo de cliente.
        if(receiveCode == SOCKET_ERROR){
            return -1;
        }

        alta = buffer;
        cout<<"ALTA DE CREAR SERVICIO ES"<<alta;

        ofstream altas;
        altas.open("Altas.txt",ios::in);
        altas<<alta;
        cout<<"SE DIO DE ALTA\n";

 }
 */
};


int main()
{
    Server *Servidor = new Server();
    int isPasswordValid = 0, isUserValid = 0,isLoginValid = 0,cod,intentos=0,clientAddrSize = sizeof(Servidor->clientAddr);

    int servicio=0;

    //Servidor->Recibir();
    while(true){
        if(intentos < 3 && isLoginValid != 1){ /** Validacion de los tres intentos de login**/
            /*isUserValid = Servidor->validarUser(); //solo envia
            if(isUserValid == -1){   //-1 Es error.
                isPasswordValid = -1;  // le da error al password.
            }else{
                isPasswordValid = Servidor->validarPassword();// Si es correcta valida la pass
            }*/
            isLoginValid = Servidor->validarCredenciales();
            intentos++;
        }
        //memset(Servidor->buffer,0,sizeof(Servidor->buffer));
        if(isLoginValid==1 && intentos <= 3) /** Si las credenciales son validas puede continuar con el resto del programa**/
        {
            //Servidor->Enviar();
          servicio= Servidor->validarAltas();
          //cod = recv(Servidor->client,Servidor->buffer,1024,0); // Si es correcto que reciba --LO SAQUE CARO

            if(servicio == SOCKET_ERROR){ /** SOCKET_ERROR => -1 **/ /** Valida si el cliente se desconecto del servidor, 0 si se desconecto normalmente y -1 desconexion forzada**/
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
        if(isLoginValid==-1 || intentos == 3){  /** Si por inactividad del cliente o las credenciales son invalidas se desconecta **/
            Servidor->sendCloseMessage();
            if(closesocket(Servidor->client)==0){
            cout << "Cliente desconectado !" << endl << "Esperando conexiones entrantes.."<<endl;}
            if((Servidor->client = accept(Servidor->server, (SOCKADDR *)&Servidor->clientAddr, &clientAddrSize)) != INVALID_SOCKET)
            {
                cout << "Cliente conectado!" << endl;
                intentos = 0;
            }
        }
    }



}
