#include <iostream>
#include <winsock2.h>
#include <fstream>
#include <sstream>
#include <string>
#include <windows.h>
#include <ctime>
#include <stdlib.h>
#include <time.h>
using namespace std;
struct Servicio{
    char origen[64];
    char fecha[64];
    char turno[64];
    char asientos[64];
};
class Server{
public:
    WSADATA WSAData;
    SOCKET server, client;
    SOCKADDR_IN serverAddr, clientAddr;
    char buffer[1024];
    char userActual[12],date[32];
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
            //guardo incio en el log

            ofstream archivo;
            archivo.open("server.log.txt",ios::out|ios::app);
            if(archivo.fail() ){
                cout<<"no se pudo abrir el archivo";
                exit(1);
            }
            archivo<<"\n"<< horaActual() <<"\n =========================== \n INICIA SERVIDOR \n =========================== \n";
            archivo<<"Socket creado. Puerto de escucha: 5555 \n";
            archivo.close();
        }
    }
    int validarCredenciales(){
        memset(buffer, 0, sizeof(buffer));
        char user[12]="\0",password[12]="\0";int flag=0,receiveCode;
        receiveCode = recv(client, buffer, sizeof(buffer), 0);
        if(receiveCode == SOCKET_ERROR){
            return -1;
        }
        strcpy(user,buffer);
        recv(client, buffer, sizeof(buffer), 0);
        strcpy(password,buffer);

        ifstream credenciales;
        credenciales.open("Credenciales.log",ios::in);
        string lector;
        while(!credenciales.eof() && !flag){
            getline(credenciales,lector,';');

            if(strcmp(user,lector.c_str())==0){
                getline(credenciales,lector,'\n');
                if(strcmp(password,lector.c_str())==0){
                    Enviar("LOGIN_VALID");strcpy(userActual,user);
                    cout<<"User y password correctos!\n";
                    cout<<"-----------------------------";
                    //guardo incio en el log

            ofstream archivo;
            archivo.open("server.log.txt",ios::out|ios::app);
            if(archivo.fail() ){
                cout<<"no se pudo abrir el archivo";
                exit(1);
            }
            archivo<<"\n"<<"\nINICIA SESION: "<<userActual<<"\n"<<horaActual()<<"\n-----------------------------\n";
            archivo.close();
                    cout<<"\nINICIA SESION: "<<userActual<<"\n"<<horaActual()<<"\n-----------------------------\n";

                    string log = ".log.txt";
                    string archivoUsuario = userActual+log;

            ofstream archivoUser;
            archivoUser.open(archivoUsuario,ios::out|ios::app);
            if(archivoUser.fail() ){
                cout<<"no se pudo abrir el archivo";
                exit(1);
            }
            archivoUser<<"\n"<<"\nINICIA SESION: "<<userActual<<"\n"<<horaActual()<<"\n-----------------------------\n";
            archivoUser.close();

                    flag = 1;
                }
            }else{
                getline(credenciales,lector,'\n');
            }
        }
        if(flag==0){
            Enviar("LOGIN_INVALID");
            cout<<"Credenciales incorrectas!"<<endl;
            ofstream archivo;
            archivo.open("server.log.txt",ios::out|ios::app);
            if(archivo.fail() ){
                cout<<"no se pudo abrir el archivo";
                exit(1);
            }
            archivo<<"\n"<<"\nERROR AL INICIAR SESION USUARIO: "<<user<<"\n"<<horaActual()<<"\n-----------------------------\n";
            archivo.close();
        }
        credenciales.close();
        return flag;
    }
    char* Recibir()
    {
        char* temp;
        if(recv(client, buffer, sizeof(buffer), 0) == SOCKET_ERROR)sendCloseMessage();
        temp = buffer;
        return temp;
    }
    void Enviar(const char* data)
    {
        strcpy(this->buffer,data);
        send(client, buffer, sizeof(buffer), 0);
    }
    void CerrarSocket()
    {
        int clientAddrSize = sizeof(clientAddr);
        if(closesocket(client)==0){
            sendCloseMessage();
            cout<<"\n-----------------------------\nCIERRA SESION: "<<userActual<<"\n"<<horaActual()<<"\n-----------------------------\n\n";
            cout << "Cliente desconectado!" << endl << "Esperando conexiones entrantes..."<<endl;
            ofstream archivo;
            archivo.open("server.log.txt",ios::out|ios::app);
            if(archivo.fail() ){
                cout<<"no se pudo abrir el archivo";
                exit(1);
            }
            archivo<<"\n-----------------------------\nCIERRA SESION: "<<userActual<<"\n"<<horaActual()<<"\n-----------------------------\n\n";
            archivo.close();

            }
        if((client = accept(server, (SOCKADDR *)&clientAddr, &clientAddrSize)) != INVALID_SOCKET)
        {
            cout << "Cliente conectado!" << endl;
        }
    }
    void sendCloseMessage(){
        memset(buffer,0,sizeof(buffer));
        strcpy(buffer,"closed");
        send(client, buffer, sizeof(buffer), 0);
    }

    char* horaActual(){
        time_t now = time(0);
        tm* localtm = localtime(&now);
        strftime(date,sizeof(date),"%d-%m-%y_%H:%M:%S",localtm);
        return date;
    }

    int validarAltas(){    /** Devuelve -1 => Si se desconecto el cliente, 0 => Dió de alta servicio si no lo encontro, 1 => Si encontro el servicio**/
        memset(buffer,0,sizeof(buffer));
        string alta="",asientos;int flag=0,receiveCode;
        receiveCode = recv(client, buffer, sizeof(buffer), 0); //Recibe lo de cliente.
        if(receiveCode == SOCKET_ERROR || receiveCode == 0){
            return -1;
        }
        alta = buffer;

        stringstream input_stringstream(alta);
        input_stringstream.seekg(0);
        Servicio micro;

        getline(input_stringstream,alta,';');
        strcpy(micro.origen,alta.c_str());

        getline(input_stringstream,alta,';');
        strcpy(micro.fecha,alta.c_str());

        getline(input_stringstream,alta,';');
        strcpy(micro.turno,alta.c_str());

        strcpy(micro.asientos,"OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO");

        ifstream altas("Altas.bin",ios::out | ios::binary);
        string lector;
        Servicio aux;

        while(!altas.eof() && !flag){
            altas.read((char*)(&aux),sizeof(Servicio));

            if(strcmp(micro.origen,aux.origen)==0 && strcmp(micro.fecha,aux.fecha)==0 && strcmp(micro.turno,aux.turno)==0){
                cout<<"ENVIO DATOS DEL SERVICIO ENCONTRADO\n"<<buffer;
                Enviar("SE ENCONTRO EL SERVICIO SELECCIONADO");

                flag = 1;
                imprimirMatrizMicro(aux.asientos);
            }

        }

        altas.close();
        if(flag==0){
            cout<<"NO ENCONTRO SERVICIO SOLICITADO!\n"<<endl;
        cout<<"EL ALTA DEL SERVICIO A CREAR ES: "<<buffer<<"\n";

        ofstream altaServicio("Altas.bin",ios::out | ios::app | ios::binary);

        altaServicio.write((char *)&micro,sizeof(Servicio));
        cout<<"SE DIO DE ALTA EL SERVICIO: "<<buffer<<"\n";
        ofstream archivo;
            archivo.open("server.log.txt",ios::out|ios::app);
            if(archivo.fail() ){
                cout<<"no se pudo abrir el archivo";
                exit(1);
            }
            archivo<<"\n-----------------------------\nSE DIO DE ALTA EL SERVICIO: "<<buffer<<"\n"<<horaActual()<<"\n-----------------------------\n\n";
            archivo.close();

                    string log = ".log.txt";
                    string archivoUsuario = userActual+log;

            ofstream archivoUser;
            archivoUser.open(archivoUsuario,ios::out|ios::app);
            if(archivoUser.fail() ){
                cout<<"no se pudo abrir el archivo";
                exit(1);
            }
            archivoUser<<"\n-----------------------------\nSE DIO DE ALTA EL SERVICIO: "<<buffer<<"\n"<<horaActual()<<"\n-----------------------------\n\n";
            archivoUser.close();
        Enviar("SE DIO DE ALTA EL SERVICIO\n");
        altaServicio.close();

        }
        return flag;
    }

    void imprimirMatrizMicro(char asientos[]){
        char omnibus[3][20];
        int i = 0,j = 0;
        for(int k = 0; k < 60; k++){
            omnibus[i][j] = asientos[k];
            j++;
            if(k == 19){
                i++;
                j = 0;
            }else if(k == 39){
                i++;
                j = 0;
            }
        }
        cout<<"\n                       1 1 1 1 1 1 1 1 1 1 2\n   | 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0\n -------------------------------------------\n";
        for(i = 0; i < 3; i++){
            if(i == 0)cout<<" A |";
            else if(i == 1)cout<<" B |";
            else if(i == 2)cout<<" C |";
            for(j = 0; j < 20; j++){
                cout<<" "<<omnibus[i][j];
            }
            cout<<"\n";
            if(i == 1)cout<<"   | =======================================\n";
        }
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
    int menuOpcionCliente(int opcion){ /** Devuelve un int del codigo del socket **/
        memset(buffer,0,sizeof(buffer));
        int estado = 0;
        if(opcion == -1 || opcion == 0 || opcion == 4)estado = -1;
        switch(opcion){
        case 1:     /** ALTA DE SERVICIOS **/
            estado = validarAltas();
            break;
        case 2:     /** GESTION DE PASAJES **/
            estado = -1; // <-- Eliminar cuando se implemente la funcionalidad
            break;
        case 3:     /** REGISTRO DE ACTIVIDADES **/
            estado = -1; // <-- Eliminar cuando se implemente la funcionalidad
            break;
        }
        return estado;
    }
};

int main()
{
    Server *Servidor = new Server();
    int servicio=0,isLoginValid = 0,intentos=0,opcionCliente=0,clientAddrSize = sizeof(Servidor->clientAddr);

    while(true){
        if(intentos < 3 && isLoginValid != 1){ /** Validacion de los tres intentos de login**/
            isLoginValid = Servidor->validarCredenciales();
            intentos++;
        }
        if(isLoginValid==1 && intentos <= 3) /** Si las credenciales son validas puede continuar con el resto del programa**/
        {
          opcionCliente=atoi(Servidor->Recibir());

          servicio= Servidor->menuOpcionCliente(opcionCliente);

            if(servicio == SOCKET_ERROR){ /** SOCKET_ERROR => -1 **/ /** Valida si el cliente se desconecto del servidor, 0 si se desconecto normalmente y -1 desconexion forzada**/
                //Servidor->CerrarSocket();
                Servidor->CerrarSocket();
                intentos = 0;isLoginValid = 0;
            }
        }
        if(isLoginValid==-1 || intentos == 3){  /** Si por inactividad del cliente o las credenciales son invalidas se desconecta **/
            Servidor->sendCloseMessage();
            /*Servidor->CerrarSocket();
            intentos = 0;*/
            if(closesocket(Servidor->client)==0){
            //cout<<"\n-----------------------------\nCIERRA SESION: "<<Servidor->userActual<<"\n-----------------------------\n\n";
            cout << "\nCliente desconectado !" << endl << "Esperando conexiones entrantes.."<<endl;
            ofstream archivo;
            archivo.open("server.log.txt",ios::out|ios::app);
            if(archivo.fail() ){
                cout<<"no se pudo abrir el archivo";
                exit(1);
            }
            archivo<<"\n-----------------------------\nCIERRA SOCKET: "<<Servidor->horaActual()<<"\n-----------------------------\n\n";
            archivo.close();
            }
            if((Servidor->client = accept(Servidor->server, (SOCKADDR *)&Servidor->clientAddr, &clientAddrSize)) != INVALID_SOCKET)
            {
                cout << "Cliente conectado!" << endl;
                intentos = 0;
            }
        }
    }
}
