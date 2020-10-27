#include <iostream>
#include <winsock2.h>
#include <string>
#include <sstream>
#include <stdio.h>
#include<stdlib.h>
#include <ctime>

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



char* ValidarOrigen(){
int opcion=0;
static char origen[64];

 cout<<"INGRESE EL ORIGEN:\n\n1-BUENOS AIRES\n2-MAR DEL PLATA\n3-SALIR\n";
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
            //return;
                menu();
            break;
       default:
            cout<<"SELECCIONE UNA OPCION CORRECTA\n";
            break;
    }

    }
return origen;

}

char* validarFecha(){
    int dia=0;
    int mes=0;
    int anio=0;
    int fechaCorrecta=0;
    static char date[64];

    while (fechaCorrecta==0){

cout<<"INGRESE EL DIA: ";

cin>>dia;

cout<<"INGRESE MES:";
cin>>mes;

cout<<"INGRESE ANIO: ";
cin>>anio;

if (mes>0 && mes <=12){

    switch(mes){
    //Los meses que tienen 31 dias.

            case  1 :
            case  3 :
            case  5 :
            case  7 :
            case  8 :
            case 10 :
            case 12 :
                if ((dia>0 && dia<=31) && (anio>2019)){
                    fechaCorrecta=1;

                }else{
                  cout<<"EL MES SELECCIONADO TIENE 31 DIAS Y EL ANIO DEBE SER EL ACTUAL O MAYOR\n";
                }

                 break;
        // Los meses que tienen 30 dias.
            case  4:
            case  6 :
            case  9 :
            case 11 :

                if ((dia>0 && dia<31)&& (anio>2019)){
                    fechaCorrecta=1;

                }else {
                    cout<<"EL MES SELECCIONADO TIENE 30 DIAS Y EL ANIO DEBE SER EL ACTUAL O MAYOR\n";
                }
                break;

            // Febrero validando anio bisiesto.

            case 2:

                if(anio % 4 == 0 && (anio % 100 != 0 || anio % 400 == 0))
                      {
                          cout<<"EL ANIO ES BISIESTO Y TIENE 29 DIAS\n";
                          if (( dia > 0 && dia <= 29)  && (anio>2019))
                          {
                            fechaCorrecta = 1;
                          }
                        }

                      else{
                        cout<<"EL ANIO INGRESADO NO ES BISIESTO Y TIENE 28 DIAS Y EL ANIO DEBE SER EL ACTUAL O MAYOR \n";
                        if ((dia>0 && dia<= 28) && (anio>2019)){
                            fechaCorrecta = 1;
                        }
                           }

                    break;
                      }

    }
    if(fechaCorrecta==0){
      cout<<"\nFECHA INCORRECTA - INGRESE NUEVAMENTE LA FECHA:\n";
    }

    }

       if (fechaCorrecta=1){

        ostringstream os;
        os << dia << '/' << mes << '/' << anio;
        string fecha=os.str();
        cout<<"\nFECHA CORRECTA\n";
        fflush(stdin);
        strcpy(date,fecha.c_str());

       }
return date;

}



char* ValidarTurno(){
int opcion1=0;
static char turno[64];

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
                //return;
                menu();
                break;
       default:
            cout<<"SELECCIONE UN OPCION CORRECTA\n";
            break;
    }

    }

 return turno;

}


void menu (){

    int opcion=0;
     cout<<"\n********************************* BIENVENIDOS ********************************* \n\nSELECCIONE UNA OPCION:\n";

     while (opcion<1  || opcion>4){

        cout<<"\n1-ALTA SERVICIO: \n2-GESTIONAR PASAJES:\n3-REGISTRO DE ACTIVIDADES:\n4-CERRAR SESION:\n";
        cin>>opcion;

        switch(opcion){

        case 1:
            cout<<"\nUSTED SELECCIONO LA OPCION ALTAS SERVICIO\n\n";
                Altas();menu();
                break;
        case 2:
            cout<<"\nUSTED SELECCIONO LA OPCION GESTIONAR PASAJES\n";
                gestionarPaquetes(); menu();
                break;
        case 3:
            cout<<"\nUSTED SELECCIONO REGISTRO DE ACTIVIDADES\n";
                //Altas();menu();
                break;
        case 4:
            cout<<"\nUSTED CERRO SESION\n";
            Enviar("salir");
            CerrarSocket();
                break;
       default:
            cout<<"\nSELECCIONE UN OPCION CORRECTA\n";
            break;
        }
        }
    system ("pause");
    system("cls");
    }

void Altas(){
    char origen[64];
    int dia=0;
    int mes=0;
    int anio=0;
    char turno[10];
    char date[15];
    int opcion=0;int opcion1=0;


strcpy(origen,ValidarOrigen());
strcpy(date,validarFecha());
strcpy(turno,ValidarTurno());


fflush(stdin);
char alta[50]="";
strcat(alta,origen);
strcat(alta,";");
strcat(alta,date);
strcat(alta,";");
strcat(alta,turno);
strcat(alta,";");

cout<<"\nEL SERVICIO SELECCIONADO ES:\n\n"<<"ORIGEN: "<<origen<<" - FECHA:"<<date<<" - TURNO:"<<turno<<".\n\n";
Enviar("alta");
Enviar(alta);
strcpy(alta,Recibir());

system ("pause");
system("cls");

return;
}


int gestionarPaquetes(){

int opcion=0;

while (opcion<1  || opcion>=3){

        cout<<"\n1-BUSQUEDA DE SERVICIO: \n2-RESERVAR PASAJES: \n3-VOLVER AL MENU:\n";
        cin>>opcion;


        switch(opcion){

         case 1:
            cout<<"BUSQUEDA DE SERVICIO\n";
            busquedadeservicio();
            break;
        case 2:
            cout<<"USTED SELECCIONO LA OPCION RESERVAR PASAJES\n";
                //ReservaPasaje();
            break;
        case 3:
            cout<<"USTED SELECCIONO VOLVER AL MENU\n";
            menu();
            break;

       default:
        {
            cout<<"SELECCIONE UN OPCION CORRECTA\n";

        }


}
}
}


int busquedadeservicio(){

int opcion=0;
while (opcion<1  || opcion>7){

        cout<<"\nSELECCIONE UNA OPCION:\n";

        cout<<"\n1-BUSQUEDA POR ORIGEN: \n2-BUSQUEDA POR FECHA: \n3-BUSQUEDA POR TURNO: \n4-BUSQUEDA POR ORIGEN Y FECHA: \n5-BUSQUEDA POR ORIGEN Y TURNO:\n6-LISTAR TODOS LOS SERVICIOS:\n7-VOLVER AL MENU ANTERIOR\n";
        cin>>opcion;


        switch(opcion){

         case 1:
            cout<<"USTED HA SELECCIONADO LA OPCION BUSQUEDA POR ORIGEN\n\n";
                busquedaPorOrigen();
                break;
        case 2:
            cout<<"USTED HA SELECCIONADO LA OPCION BUSQUEDA POR FECHA\n";
            busquedaPorFecha();
            break;

        case 3:
            cout<<"USTED HA SELECCIONADO LA OPCION  BUSQUEDA POR TURNO\n";
            busquedaPorTurno();
            break;
        case 4:
            cout<<"USTED HA SELECCIONADO LA OPCION BUSQUEDA POR ORIGEN Y FECHA\n";
            BusquedaOrigenYFecha();
            break;
        case 5:
            cout<<"USTED HA SELECCIONADO LA OPCION BUSQUEDA POR ORIGEN Y TURNO\n";
             BusquedaOrigenYTurno();
                break;
        case 6:
            cout<<"LISTAR TODOS LOS SERVICIOS\n";
             listarServicios();
                break;
        case 7:

                gestionarPaquetes();
                break;

       default:
        {
            cout<<"SELECCIONE UN OPCION CORRECTA\n";

        }
    }

    }
}

int busquedaPorOrigen(){
int opcion=0;
char origen[64]="";
int env=0;

strcpy(origen,ValidarOrigen());


env=Enviar("validar");
env = Enviar(origen);
while(!(strcmp(origen,"NO HAY MAS DATOS PARA MOSTRAR")==0)&& !(strcmp(origen,"NO SE ENCONTRO LA OPCION SOLICITADA")==0)){
strcpy(origen,Recibir());
}
system("pause");
busquedadeservicio();

}

int busquedaPorFecha(){

char date[20];

strcpy(date,validarFecha());
int env=0;


env=Enviar("validar");
cout<<" LA FECHA ES:"<<date<<"\n\n";
env = Enviar(date);
while(!(strcmp(date,"NO HAY MAS DATOS PARA MOSTRAR")==0) && !(strcmp(date,"NO SE ENCONTRO LA OPCION SOLICITADA")==0)){
strcpy(date,Recibir());
}
system("pause");
busquedadeservicio();

}


int busquedaPorTurno(){

char turno[64];
int env=0;

strcpy(turno,ValidarTurno());
cout<<"EL TURNO ES"<<turno<<"\n\n";

env=Enviar("validar");
env = Enviar(turno);
while(!(strcmp(turno,"NO HAY MAS DATOS PARA MOSTRAR")==0) && !(strcmp(turno,"NO SE ENCONTRO LA OPCION SOLICITADA")==0)){
strcpy(turno,Recibir());

}
system("pause");
busquedadeservicio();

env = Enviar("busquedaporturno");

}
int BusquedaOrigenYFecha(){

char origen[64]="";
char fecha[64]="";
char busqueda[64]="";
int env=0;

strcpy(origen,ValidarOrigen());
strcpy(fecha,validarFecha());

strcat(busqueda,origen);
strcat(busqueda,";");
strcat(busqueda,fecha);

env=Enviar("validardos");
env = Enviar(busqueda);
while(!(strcmp(busqueda,"NO HAY MAS DATOS PARA MOSTRAR")==0) && !(strcmp(busqueda,"NO SE ENCONTRO LA OPCION SOLICITADA")==0)){
strcpy(busqueda,Recibir());
}
system("pause");
busquedadeservicio();


}

int BusquedaOrigenYTurno(){

char origen[64]="";
char turno[64]="";
char busqueda[64]="";
int env=0;

strcpy(origen,ValidarOrigen());
strcpy(turno,ValidarTurno());

strcat(busqueda,origen);
strcat(busqueda,";");
strcat(busqueda,turno);

env=Enviar("validardos");
env = Enviar(busqueda);
while(!(strcmp(busqueda,"NO HAY MAS DATOS PARA MOSTRAR")==0) && !(strcmp(busqueda,"NO SE ENCONTRO LA OPCION SOLICITADA")==0)){
strcpy(busqueda,Recibir());
}
system("pause");
busquedadeservicio();


}

listarServicios(){
char busqueda[64]="";
int env=0;

env=Enviar("listar");
while(!(strcmp(busqueda,"NO HAY MAS DATOS PARA MOSTRAR")==0)){
strcpy(busqueda,Recibir());
}
system("pause");
busquedadeservicio();

}

};

int main()
{
    char ip[12]=""; short unsigned int port; int loginSuccess,intentos=3;
    Client *Cliente = new Client();
    do{
        cout<<"Ingrese la IP del servidor para conectarse: ";
//        fflush(stdin);
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
