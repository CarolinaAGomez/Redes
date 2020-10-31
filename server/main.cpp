#include <iostream>
#include <winsock2.h>
#include <fstream>
#include <sstream>
#include <string>
#include <windows.h>
#include <ctime>
#include <stdlib.h>
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

                    string log = ".log.txt";
                    string archivoUsuario = userActual+log;

            ofstream archivoUser;
            archivoUser.open(archivoUsuario,ios::out|ios::app);
            if(archivoUser.fail() ){
                cout<<"no se pudo abrir el archivo";
                exit(1);
            }
            archivoUser<<"\n-----------------------------\nCIERRA SESION: "<<userActual<<"\n"<<horaActual()<<"\n-----------------------------\n\n";
            archivoUser.close();
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
        Servicio aux;

        while(!altas.eof() && !flag){
            altas.read((char*)(&aux),sizeof(Servicio));

            if(strcmp(micro.origen,aux.origen)==0 && strcmp(micro.fecha,aux.fecha)==0 && strcmp(micro.turno,aux.turno)==0){
                cout<<"ENVIO DATOS DEL SERVICIO ENCONTRADO\n"<<buffer;
                Enviar("SE ENCONTRO EL SERVICIO SELECCIONADO");
                Enviar(aux.asientos);

                flag = 1;
            }

        }

        altas.close();
        if(flag==0){
        cout<<"NO ENCONTRO SERVICIO SOLICITADO!\n"<<endl;
        cout<<"EL ALTA DEL SERVICIO A CREAR ES: "<<buffer<<"\n";

        ofstream altaServicio("Altas.bin",ios::out | ios::app | ios::binary);
        altaServicio.write((char *)&micro,sizeof(Servicio));
        altaServicio.close();

        ofstream altaLegible("Legible.txt",ios::out | ios::app);
        altaLegible<<micro.origen<<"-"<<micro.fecha<<"-"<<micro.turno<<"-"<<micro.asientos<<"\n";

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
        Enviar(micro.asientos);

        altaLegible.close();
        }
        return flag;
    }

    void reservarLiberarAsiento(char caracterAsiento,Servicio micro){
        string ubicacion;int fila,columna,flag=0,pos;
        string filas;
        char reservar = 'X';
        fstream file("Altas.bin",ios::out | ios::in | ios::binary);
        Servicio aux;

        while(!file.eof() && !flag){   /** Me recorro el archivo para localizar el servicio y me devuelva los asientos de este **/
            file.read((char*)(&aux),sizeof(Servicio));

            if(strcmp(micro.origen,aux.origen)==0 && strcmp(micro.fecha,aux.fecha)==0 && strcmp(micro.turno,aux.turno)==0){
                Enviar(aux.asientos);
                ubicacion = Recibir();

                stringstream posicion(ubicacion);
                getline(posicion,ubicacion,';');
                fila = atoi(ubicacion.c_str());

                getline(posicion,ubicacion);
                columna = atoi(ubicacion.c_str());

                if(fila == 1)pos=columna-1;
                else if(fila == 2)pos=columna+19;
                else if(fila == 3)pos=columna+39; /** Proceso de conversion de array a matriz de los asientos **/

                if(aux.asientos[pos] == 'X'){
                    Enviar("SEAT_OCCUPIED");
                }else{
                    aux.asientos[pos] = caracterAsiento;
                    Enviar(aux.asientos);
                }
                flag=1;
                file.seekg(file.tellg()-sizeof(aux));
                file.write((char*)&aux,sizeof(aux));

            if (fila == 1){
                filas = "A";
            }else if (fila == 2){
                filas = "B";
            }else{
                filas = "C";
            }
            if (caracterAsiento == reservar){
            ofstream archivo;
            archivo.open("server.log.txt",ios::out|ios::app);
            if(archivo.fail() ){
                cout<<"no se pudo abrir el archivo";
                exit(1);
            }

            archivo<<"\nReserva asiento en "<<micro.origen<<" "<<micro.fecha<<" "<<micro.turno<<"\n";
            archivo<<"Asiento: "<<filas<<""<<ubicacion<<"\n";
            archivo.close();

                    string log = ".log.txt";
                    string archivoUsuario = userActual+log;

            ofstream archivoUser;
            archivoUser.open(archivoUsuario,ios::out|ios::app);
            if(archivoUser.fail() ){
                cout<<"no se pudo abrir el archivo";
                exit(1);
            }

            archivoUser<<"\nReserva asiento en "<<micro.origen<<" "<<micro.fecha<<" "<<micro.turno<<"\n";
            archivoUser<<"Asiento: "<<filas<<""<<ubicacion<<"\n";
            archivoUser.close();
            }else{
                ofstream archivo;
            archivo.open("server.log.txt",ios::out|ios::app);
            if(archivo.fail() ){
                cout<<"no se pudo abrir el archivo";
                exit(1);
            }

            archivo<<"\nLibera asiento en "<<micro.origen<<" "<<micro.fecha<<" "<<micro.turno<<"\n";
            archivo<<"Asiento: "<<filas<<""<<ubicacion<<"\n";
            archivo.close();

                    string log = ".log.txt";
                    string archivoUsuario = userActual+log;

            ofstream archivoUser;
            archivoUser.open(archivoUsuario,ios::out|ios::app);
            if(archivoUser.fail() ){
                cout<<"no se pudo abrir el archivo";
                exit(1);
            }

            archivoUser<<"\nLibera asiento en "<<micro.origen<<" "<<micro.fecha<<" "<<micro.turno<<"\n";
            archivoUser<<"Asiento: "<<filas<<""<<ubicacion<<"\n";
            archivoUser.close();

            }
            }

        }
        file.close();
    }

    int menuOpcionGestionPasajes(){
        memset(buffer,0,sizeof(buffer));
        Servicio recordService;
        string registroServicio="",posicion="";int opcion=0,receiveCode;
        receiveCode = recv(client, buffer, sizeof(buffer), 0);
        if(receiveCode == SOCKET_ERROR || receiveCode == 0){
            //sendCloseMessage();
            return -1;
        }
        registroServicio = buffer;  //Recibe el registro del servicio, desde Reserva() en cliente

        validarDisponibilidadServicio(registroServicio,recordService);
        Enviar(registroServicio.c_str());
        if(registroServicio == "NOT_FOUND")
        {
            return 404;
        }
        memset(buffer,0,sizeof(buffer));
        opcion = atoi(Recibir());   //Se queda esperando la opcion de reservar o liberar un asiento

        switch(opcion){
        case 1:
            reservarLiberarAsiento('X',recordService);    //Opcion 1 se ocupa un asiento
            break;
        case 2:
            reservarLiberarAsiento('O',recordService);    //Opcion 2 se libera un asiento
            break;
        default:
            return -1;
            break;
        }
        return 0;
    }

void validarDisponibilidadServicio(string &registroServicio, Servicio &service){
    int flag=0;

    stringstream input_stringstream(registroServicio);
    Servicio micro;

    getline(input_stringstream,registroServicio,';');
    strcpy(micro.origen,registroServicio.c_str());

    getline(input_stringstream,registroServicio,';');
    strcpy(micro.fecha,registroServicio.c_str());

    getline(input_stringstream,registroServicio);
    strcpy(micro.turno,registroServicio.c_str());
    Servicio aux;

    fstream file("Altas.bin",ios::out | ios::in | ios::binary);
    while(!file.eof() && !flag)
    {
        file.read((char*)(&aux),sizeof(Servicio));
        if(strcmp(micro.origen,aux.origen)==0 && strcmp(micro.fecha,aux.fecha)==0 && strcmp(micro.turno,aux.turno)==0)
        {
            flag = 1;
            strcpy(service.origen,aux.origen);
            strcpy(service.fecha,aux.fecha);
            strcpy(service.turno,aux.turno);
        }
    }
    if(flag == 0)
    {
        registroServicio = "NOT_FOUND";
    }
}

    int busquedas(){

        memset(buffer,0,sizeof(buffer));
        string dato="",asientos;int flag=0,receiveCode;
        receiveCode = recv(client, buffer, sizeof(buffer), 0); //Recibe lo de cliente.
        if(receiveCode == SOCKET_ERROR || receiveCode == 0){
            return -1;
        }
        dato = buffer;
        Servicio micro; // crea una estructura
        strcpy(micro.origen,dato.c_str());

        ifstream altas("Altas.bin",ios::out | ios::binary);
        Servicio aux;

        altas.read((char*)(&aux),sizeof(Servicio));
        while(!altas.eof()){

            //cout<<"micro origen"<<micro.origen<<" "<<micro.fecha<<" "<<micro.turno<<"\n";
            //cout<<aux.origen<<" "<<aux.fecha<<" "<<aux.turno<<"\n\n";
            if((strcmp(micro.origen,aux.origen)==0) || (strcmp(micro.origen,aux.fecha)==0) ||(strcmp(micro.origen,aux.turno))==0){
                char data[64]="";
                strcat(data,aux.origen);
                strcat(data,"-");
                strcat(data,aux.fecha);
                strcat(data,"-");
                strcat(data,aux.turno);

             ofstream archivo;
            archivo.open("server.log.txt",ios::out|ios::app);
            if(archivo.fail() ){
                cout<<"no se pudo abrir el archivo";
                exit(1);
            }
            archivo<<"El usuario consulto por "<<micro.origen<<" "<<micro.fecha<<" "<<micro.turno<<"\n";
            archivo<<aux.origen<<" "<<aux.fecha<<" "<<aux.turno<<"\n\n";
            archivo.close();

                    string log = ".log.txt";
                    string archivoUsuario = userActual+log;

            ofstream archivoUser;
            archivoUser.open(archivoUsuario,ios::out|ios::app);
            if(archivoUser.fail() ){
                cout<<"no se pudo abrir el archivo";
                exit(1);
            }
            archivoUser<<"El usuario consulto por "<<micro.origen<<" "<<micro.fecha<<" "<<micro.turno<<"\n";
            archivoUser<<aux.origen<<" "<<aux.fecha<<" "<<aux.turno<<"\n\n";
            archivoUser.close();

                cout<<"ENVIO DATOS DEL SERVICIO ENCONTRADO: "<<data<<"\n";
                Enviar(data);
                flag=1;
            }
            altas.read((char*)(&aux),sizeof(Servicio));
        }

        altas.close();
        if (flag==1){
            cout<<"\n\n";
            Enviar("NO HAY MAS DATOS PARA MOSTRAR");
        }
        if(flag==0){
            cout<<"NO SE ENCONTRO LA OPCION SOLICITADA!\n"<<endl;

        Enviar("NO SE ENCONTRO LA OPCION SOLICITADA");

        }
        return flag;
    }


int busquedasConDosEntradas(){

  memset(buffer,0,sizeof(buffer));
        string dato="",asientos;int flag=0,receiveCode;
        receiveCode = recv(client, buffer, sizeof(buffer), 0); //Recibe lo de cliente.
        if(receiveCode == SOCKET_ERROR || receiveCode == 0){
            return -1;
        }
        dato = buffer;

        stringstream input_stringstream(dato);
        input_stringstream.seekg(0);
        Servicio micro;

        getline(input_stringstream,dato,';');
        strcpy(micro.origen,dato.c_str());

        getline(input_stringstream,dato,' ');
        strcpy(micro.fecha,dato.c_str());

        ifstream altas("Altas.bin",ios::out | ios::binary);
        Servicio aux;

        altas.read((char*)(&aux),sizeof(Servicio));
        while(!altas.eof()){

            //cout<<"micro origen"<<micro.origen<<" "<<micro.fecha<<" "<<micro.turno<<"\n";
            //cout<<aux.origen<<" "<<aux.fecha<<" "<<aux.turno<<"\n\n";
            if((strcmp(micro.origen,aux.origen)==0) && (strcmp(micro.fecha,aux.fecha)==0) || (strcmp(micro.origen,aux.origen)==0) && (strcmp(micro.fecha,aux.turno)==0)){
                char data[64]="";
                strcat(data,aux.origen);
                strcat(data,"-");
                strcat(data,aux.fecha);
                strcat(data,"-");
                strcat(data,aux.turno);

                ofstream archivo;
            archivo.open("server.log.txt",ios::out|ios::app);
            if(archivo.fail() ){
                cout<<"no se pudo abrir el archivo";
                exit(1);
            }
            archivo<<"El usuario consulto por "<<micro.origen<<" "<<micro.fecha<<" "<<micro.turno<<"\n";
            archivo<<micro.origen<<" "<<micro.fecha<<" "<<micro.turno<<"\n\n";
            archivo.close();

                    string log = ".log.txt";
                    string archivoUsuario = userActual+log;

            ofstream archivoUser;
            archivoUser.open(archivoUsuario,ios::out|ios::app);
            if(archivoUser.fail() ){
                cout<<"no se pudo abrir el archivo";
                exit(1);
            }
            archivoUser<<"El usuario consulto por "<<micro.origen<<" "<<micro.fecha<<" "<<micro.turno<<"\n";
            archivoUser<<micro.origen<<" "<<micro.fecha<<" "<<micro.turno<<"\n\n";
            archivoUser.close();

                cout<<"ENVIO DATOS DEL SERVICIO ENCONTRADO: "<<data<<"\n";
                Enviar(data);
                flag=1;
            }
            altas.read((char*)(&aux),sizeof(Servicio));
        }
        altas.close();
        if (flag==1){
            cout<<"\n\n";
            Enviar("NO HAY MAS DATOS PARA MOSTRAR");
        }
        if(flag==0){
            cout<<"NO SE ENCONTRO LA OPCION SOLICITADA!\n"<<endl;
            Enviar("NO SE ENCONTRO LA OPCION SOLICITADA");
        }
        return flag;
    }

int listarServicios(){
        int flag=0;
        memset(buffer,0,sizeof(buffer));

        ifstream altas("Altas.bin",ios::out | ios::binary);
        Servicio aux;
        altas.read((char*)(&aux),sizeof(Servicio));

        while(!altas.eof()){

                char data[64]="";
                strcat(data,aux.origen);
                strcat(data,"-");
                strcat(data,aux.fecha);
                strcat(data,"-");
                strcat(data,aux.turno);

                cout<<"ENVIO DATOS DEL SERVICIO ENCONTRADO: "<<data<<"\n";
                Enviar(data);
                altas.read((char*)(&aux),sizeof(Servicio));

        }

        ofstream archivo;
            archivo.open("server.log.txt",ios::out|ios::app);
            if(archivo.fail() ){
                cout<<"no se pudo abrir el archivo";
                exit(1);
            }
            archivo<<"\n"<<"\nEL CLIENTE SOLICITO VER LISTA DE SERVICIOS";
            archivo.close();

                    string log = ".log.txt";
                    string archivoUsuario = userActual+log;

            ofstream archivoUser;
            archivoUser.open(archivoUsuario,ios::out|ios::app);
            if(archivoUser.fail() ){
                cout<<"no se pudo abrir el archivo";
                exit(1);
            }
            archivoUser<<"\n"<<"\nEL CLIENTE SOLICITO VER LISTA DE SERVICIOS";
            archivoUser.close();

            altas.close();
            cout<<"\n\n";
            Enviar("NO HAY MAS DATOS PARA MOSTRAR");

        return flag;

    }

    int menuOpcionCliente(int opcion){ /** Devuelve un int del codigo del socket **/
        memset(buffer,0,sizeof(buffer));
        int estado = 0;
        if(opcion == -1 || opcion == 0 || opcion == 4)estado = -1;
        switch(opcion){
        case 1:     /** ALTA DE SERVICIOS **/
            estado = validarAltas();
            break;
        case 2:     /** GESTION DE PASAJES **/
            estado = menuOpcionGestionPasajes();//-1; // <-- Eliminar cuando se implemente la funcionalidad
            break;
        case 3:     /** REGISTRO DE ACTIVIDADES **/
            estado = -1; // <-- Eliminar cuando se implemente la funcionalidad
            break;
        case 4:     /** LISTAR SEGUN OPCION **/
            estado = busquedas();
            break;
        case 5:     /**LISTAR SEGUN ENTRADA DE ORIGEN Y FECHA U ORIGEN Y TURNO  **/
            estado = busquedasConDosEntradas();
            break;
        case 6:     /** LISTAR TODOS LOS SERVICIOS **/
            estado = listarServicios();
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
