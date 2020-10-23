#include <iostream>
#include <winsock2.h>
#include <string>
#include <sstream>

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
        char user[12]="\0",password[12]="\0",mensaje[12]="\0";
        cout<<"Ingrese el nombre de usuario: ";
        fflush(stdin);
        cin>>user;
        cout<<"Ingrese contraseña: ";
        fflush(stdin);
        cin>>password;
        if(checkConnectivity()== -1) return -1;
        Enviar(user);
        Enviar(password);
        strcpy(mensaje,Recibir());
        if(strcmp(mensaje,"LOGIN_VALID")==0){
            return 0;
        }else{
            return 1;
        }
    }
    int checkConnectivity(){ /** Sirve para comprobar si el servidor esta apto para recibir y enviar mensajes**/
        int isConnected = 0;
        u_long iMode= 1;
        ioctlsocket(server,FIONBIO,&iMode); //Funcion que convierte la conexion en blocking-sockets a non-blocking sockets
        memset(buffer, 0, sizeof(buffer));
        recv(server, buffer, sizeof(buffer), 0);
        if(strcmp(buffer,"closed")==0){
            isConnected = -1;
        }iMode = 0;
        ioctlsocket(server,FIONBIO,&iMode); //Y viceversa
        return isConnected;
    }
    int Enviar(const char* data){
        int envio;
        //cout<<"Escribe el mensaje a enviar: ";
        //cin>>this->buffer;
        strcpy(this->buffer,data);
        envio = send(server, buffer, sizeof(buffer), 0);
        //memset(buffer, 0, sizeof(buffer));
        //cout << "Mensaje enviado!" << endl;
        return envio;
    }
    char* Recibir(){
        char* temp;
        recv(server, buffer, sizeof(buffer), 0);
        temp = buffer;
        //cout << "El servidor dice: " << buffer << endl;
        //memset(buffer, 0, sizeof(buffer));
        cout<<"MENSAJE RECIBIDO: "<<temp<<"\n";
        return temp;
    }
    void CerrarSocket(){
       closesocket(server);
       WSACleanup();
       cout << "Socket cerrado." << endl << endl;
    }

    void menu (){

    int opcion=0;
     cout<<"\n********************************* BIENVENIDOS ********************************* \nSELECCIONE UNA OPCION: \n";
     while (opcion<1  || opcion>4){

        cout<<"\n1-ALTA SERVICIO: \n2-GESTIONAR PASAJES:\n3-REGISTRO DE ACTIVIDADES:\n4-CERRAR SESION:\n";
        cin>>opcion;

        switch(opcion){

         case 1:
            cout<<"USTED SELECCIONO LA OPCION ALTAS\n\n";
                Altas();menu();
                break;
        case 2:
            cout<<"USTED SELECCIONO LA OPCION GESTIONAR PAQUETES\n";
                //Gestionar paquetes();opcion=0;
                break;
        case 3:
            cout<<"USTED SELECCIONO REGISTRO DE ACTIVIDADES\n";
                //Altas();opcion=0;
                break;
        case 4:
            cout<<"CERRAR SESION\n";
                CerrarSocket();
                break;
       default:
            cout<<"SELECCIONE UN OPCION CORRECTA\n";
            break;
        }
        }
    }

void Altas(){
    char origen[15];
    int dia;
    int mes;
    int anio;
    char turno[10];
    int opcion=0;int opcion1=0;

    cout<<"INGRESE EL ORIGEN:\n\n1-Buenos Aires\n2-Mar del plata\n3-SALIR\n";
    while (opcion <1 || opcion>3){
    cin>>opcion;

    switch(opcion){

    case 1:
    cout<<"USTED HA SELECCIONADO ORIGEN BUENOS AIRES\n\n";
        strcpy(origen,"Buenos Aires");
            break;
        case 2:
            cout<<"USTED HA SELECCIONADO ORIGEN MAR DEL PLATA\n\n";
               strcpy(origen,"Mar del Plata");
                break;
        case 3:
            cout<<"SALIR \n";
                system("cls");
                return;
                //menu();
                break;
       default:
            cout<<"SELECCIONE UNA OPCION CORRECTA\n";
            break;
    }

    }
fflush(stdin);
cout<<"INGRESE EL DIA: ";
cin>>dia;
cout<<"INGRESE MES: ";
cin>>mes;
cout<<"INGRESE ANIO: ";
cin>>anio;
ostringstream os;
os << dia << '/' << mes << '/' << anio;
//cout << os.str();
string fecha=os.str();
cout<<"LA FECHA ES: "<<fecha<<"\n";
fflush(stdin);

char date[15];
strcpy(date,fecha.c_str());

cout<<"\nINGRESE EL TURNO:\n1-MANANA\n2-TARDE\n3-NOCHE\n4-SALIR\n";
while (opcion1 <1 || opcion1>4){
    cin>>opcion1;

    switch(opcion1){

    case 1:
    cout<<"USTED HA SELECCIONADO TURNO MANANA\n";
        strcpy(turno,"maniana");
            break;
        case 2:
            cout<<"USTED HA SELECCIONADO TURNO TARDE\n";
               strcpy(turno,"tarde");
                break;
        case 3:
            cout<<"USTED HA SELECCIONADO TURNO NOCHE\n";
               strcpy(turno,"noche");
                break;
        case 4:
            cout<<"SALIR \n";
                system("cls");
                return;
                //menu();
                break;
       default:
            cout<<"SELECCIONE UN OPCION CORRECTA\n";
            break;
    }

    }
fflush(stdin);
char alta[50]="";
strcat(alta,origen);
strcat(alta,";");
strcat(alta,date);
strcat(alta,";");
strcat(alta,turno);
strcat(alta,";");

cout<<"\nEL SERVICIO SELECCIONADO ES:\n"<<"ORIGEN: "<<origen<<" - FECHA:"<<date<<" - TURNO:"<<turno<<".\n\n";

Enviar(alta);
strcpy(alta,Recibir());

system ("pause");
system("cls");
//menu();
return;
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
        if(loginSuccess == 0 || loginSuccess == -1){
            break;
        }else if(loginSuccess == 1){
            intentos--;
            cout<<"Reintentos restantes: "<<intentos<<endl;
            system("pause");system("cls");
        }
    }
    if(intentos == 0){
        cout<<"Cliente desconectado por varios intentos fallidos."<<endl;
        system("pause");
        Cliente->CerrarSocket();
    }else if(intentos > 0 && loginSuccess == 0){
        cout<<"Login successful"<<endl;
        Cliente->menu();
        system ("pause");
    }else if(loginSuccess == -1){
        cout<<"Cliente desconectado por inactividad."<<endl;
        Cliente->CerrarSocket();
    }
    return 0;
}
