#include <iostream>
#include <winsock2.h>
#include <fstream>
#include <sstream>
#include <string>
#include <windows.h>
#include <ctime>
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
    char userActual[12];
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
        //cout<<"Usuario y contraseña recibidos"<<endl;
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
                    Enviar("LOGIN_VALID");strcpy(userActual,user);
                    cout<<"User y password correctos!\n";
                    cout<<"-----------------------------";
                    time_t now = time(0);
                    tm* localtm = localtime(&now);
                    cout<<"\nINICIA SESION: "<<userActual<<"\n"<<asctime(localtm)<<"-----------------------------\n";
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
        char* temp;
        recv(client, buffer, sizeof(buffer), 0);
        temp = buffer;
        //cout << "El cliente dice: " << buffer << endl;
        //memset(buffer, 0, sizeof(buffer));
        return temp;
    }
    void Enviar(const char* data)
    {
        //cout<<"Escribe el mensaje a enviar: ";
        //cin>>this->buffer;
        strcpy(this->buffer,data);
        send(client, buffer, sizeof(buffer), 0);
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

 int validarAltas(){    /** Devuelve -1 => Si se desconecto el cliente, 0 => Dió de alta servicio si no lo encontro, 1 => Si encontro el servicio**/
        memset(buffer,0,sizeof(buffer));
        string alta="",asientos;int flag=0,receiveCode;
        receiveCode = recv(client, buffer, sizeof(buffer), 0); //Recibe lo de cliente.
        if(receiveCode == SOCKET_ERROR || receiveCode == 0){
            return -1;
        }
        alta = buffer;
        //memset(alta,0,sizeof(alta));
        //strcpy(alta,buffer);

        stringstream input_stringstream(alta);// Toma la cadena que hay en alta.
        input_stringstream.seekg(0); //Lo posiciona en la primera ubicacion.
        Servicio micro; // crea una estructura
        //micro=initialize(micro);
        getline(input_stringstream,alta,';');
        strcpy(micro.origen,alta.c_str());

        getline(input_stringstream,alta,';');
        strcpy(micro.fecha,alta.c_str());

        getline(input_stringstream,alta,';');
        strcpy(micro.turno,alta.c_str());

        //getline(input_stringstream,alta,'\n');
        //strcpy()
        //asientos = "OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO\n";
        strcpy(micro.asientos,"OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO");

        ifstream altas("Altas.bin",ios::out | ios::binary);
        string lector;
        Servicio aux;
        //aux=initialize(aux);
         if (altas == NULL){
            cout<<"entro a alta\n";
            ofstream altaServicio("Altas.bin",ios::out | ios::app | ios::binary);
            altaServicio.write((char *)&micro,sizeof(Servicio));
            ofstream altaLegible("Legible.txt",ios::out | ios::app);
            altaLegible<<micro.origen<<"-"<<micro.fecha<<"-"<<micro.turno<<"\n";

            }
       altas.seekg(0,altas.beg);
        //altas.read((char*)(&aux),sizeof(Servicio));
        while(!altas.eof() && !flag){
            /*getline(altas,lector,'\n');
            //strcpy(user,lector.c_str());

            if(strcmp(alta,lector.c_str())==0){
            cout<<"ENVIO DATOS DEL SERVICIO ENCONTRADO\n"<<alta;
            Enviar("SE ENCONTRO EL SERVICIO SELECCIONADO");

            flag = 1;

            }*/


            cout<<"ENTROOOOOOOOOOOO\n";
            altas.read((char*)(&aux),sizeof(Servicio));
            cout<<"micro origen"<<micro.origen<<" "<<micro.fecha<<" "<<micro.turno<<"\n";
            cout<<aux.origen<<" "<<aux.fecha<<" "<<aux.turno<<"\n\n";
            //system("pause");

            if(strcmp(micro.origen,aux.origen)==0 && strcmp(micro.fecha,aux.fecha)==0 && strcmp(micro.turno,aux.turno)==0){
                cout<<"ENVIO DATOS DEL SERVICIO ENCONTRADO\n"<<buffer;
                Enviar("SE ENCONTRO EL SERVICIO SELECCIONADO");

                flag = 1;
            }
            //altas.read((char*)(&aux),sizeof(Servicio));

        }

        altas.close();
        if(flag==0){
            cout<<"NO ENCONTRO SERVICIO SOLICITADO!\n"<<endl;
           // strcpy(alta,"NO ENCONTRO SERVICIO SOLICITADO, SE CREARA SERVICIO\n");
            //alta = buffer;
            //strcpy(alta,buffer);
        cout<<"EL ALTA DEL SERVICIO A CREAR ES: "<<buffer<<"\n";

        ofstream altaServicio("Altas.bin",ios::out | ios::app | ios::binary);
        ofstream altaLegible("Legible.txt",ios::out | ios::app);

        //altaServicio<<"\n"<<alta<<";OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO";
        //altaServicio.write((char*)(&alta),strlen(alta));
        altaServicio.write((char *)&micro,sizeof(Servicio));

        altaLegible<<micro.origen<<"-"<<micro.fecha<<"-"<<micro.turno<<"\n";
        cout<<"SE DIO DE ALTA EL SERVICIO: "<<buffer<<"\n";
        Enviar("SE DIO DE ALTA EL SERVICIO\n");
        altaServicio.close();
        }
        return flag;
    }

    bool compare(Servicio s1,Servicio s2){
        //if(s1.origen == s2.origen && s1.fecha == s2.fecha && s1.turno == s2.turno){
        if(strcmp(s1.origen,s2.origen)==0 && strcmp(s1.fecha,s2.fecha)==0 && strcmp(s1.turno,s2.turno)==0){
            return true;
        }
        return false;
    }
    Servicio initialize(Servicio s){
        /*s.origen = (char*)"\0";
        s.fecha = (char*)"\0";
        s.turno = (char*)"\0";
        s.asientos = (char*)"\0";*/
        /*for(int i = 0; i < (int)(sizeof(s.origen)/sizeof(s.origen[0])); i++)
            s.origen[i] = '\0';
        for(int i = 0; i < (int)(sizeof(s.fecha)/sizeof(s.fecha[0])); i++)
            s.fecha[i] = '\0';
        for(int i = 0; i < (int)(sizeof(s.turno)/sizeof(s.turno[0])); i++)
            s.turno[i] = '\0';*/
        memset(s.origen,0,sizeof(s.origen));
        memset(s.fecha,0,sizeof(s.fecha));
        memset(s.turno,0,sizeof(s.turno));
        memset(s.asientos,0,sizeof(s.asientos));
        return s;
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
 int busquedas(){

  memset(buffer,0,sizeof(buffer));
        string dato="",asientos;int flag=0,receiveCode;
        receiveCode = recv(client, buffer, sizeof(buffer), 0); //Recibe lo de cliente.
        if(receiveCode == SOCKET_ERROR || receiveCode == 0){
            return -1;
        }
        dato = buffer;
        //memset(alta,0,sizeof(alta));
        //strcpy(alta,buffer);

        //stringstream input_stringstream(alta);// Toma la cadena que hay en alta.
        //input_stringstream.seekg(0); //Lo posiciona en la primera ubicacion.
        Servicio micro; // crea una estructura
        strcpy(micro.origen,dato.c_str());


        ifstream altas("Altas.bin",ios::out | ios::binary);
        //string lector;
        Servicio aux;
        //aux=initialize(aux);
        altas.read((char*)(&aux),sizeof(Servicio));
        while(!altas.eof()){
            /*getline(altas,lector,'\n');
            //strcpy(user,lector.c_str());

            if(strcmp(alta,lector.c_str())==0){
            cout<<"ENVIO DATOS DEL SERVICIO ENCONTRADO\n"<<alta;
            Enviar("SE ENCONTRO EL SERVICIO SELECCIONADO");

            flag = 1;

            }*/
        //altas.read((char*)(&aux),sizeof(Servicio));
            cout<<"micro origen"<<micro.origen<<" "<<micro.fecha<<" "<<micro.turno<<"\n";
            cout<<aux.origen<<" "<<aux.fecha<<" "<<aux.turno<<"\n\n";
            if((strcmp(micro.origen,aux.origen)==0) || (strcmp(micro.origen,aux.fecha)==0) ||(strcmp(micro.origen,aux.turno))==0){
                char data[64]="";
                strcat(data,aux.origen);
                strcat(data,"-");
                strcat(data,aux.fecha);
                strcat(data,"-");
                strcat(data,aux.turno);


                cout<<"ENVIO DATOS DEL SERVICIO ENCONTRADO\n";
                Enviar(data);
                flag=1;


            }
            altas.read((char*)(&aux),sizeof(Servicio));

        }

        altas.close();
        if (flag==1){
            Enviar("NO HAY MAS DATOS PARA MOSTRAR");
        }
        if(flag==0){
            cout<<"NO SE ENCONTRO LA OPCION SOLICITADA!\n"<<endl;
           // strcpy(alta,"NO ENCONTRO SERVICIO SOLICITADO, SE CREARA SERVICIO\n");
            //alta = buffer;
            //strcpy(alta,buffer);
        //cout<<"EL ALTA DEL SERVICIO A CREAR ES: "<<buffer<<"\n";

        //ofstream altaServicio("Altas.bin",ios::out | ios::app | ios::binary);

        //altaServicio<<"\n"<<alta<<";OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO";
        //altaServicio.write((char*)(&alta),strlen(alta));
        //altaServicio.write((char *)&micro,sizeof(Servicio));
        //cout<<"SE DIO DE ALTA EL SERVICIO: "<<buffer<<"\n";
        Enviar("NO SE ENCONTRO LA OPCION SOLICITADA");
        //altaServicio.close();
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
        //memset(alta,0,sizeof(alta));
        //strcpy(dato,buffer);

        stringstream input_stringstream(dato);// Toma la cadena que hay en alta.
        input_stringstream.seekg(0); //Lo posiciona en la primera ubicacion.
        Servicio micro; // crea una estructura


        getline(input_stringstream,dato,';');
        strcpy(micro.origen,dato.c_str());

        getline(input_stringstream,dato,' ');
        strcpy(micro.fecha,dato.c_str());



        ifstream altas("Altas.bin",ios::out | ios::binary);
        //string lector;
        Servicio aux;
        //aux=initialize(aux);
        altas.read((char*)(&aux),sizeof(Servicio));
        while(!altas.eof()){
            /*getline(altas,lector,'\n');
            //strcpy(user,lector.c_str());

            if(strcmp(alta,lector.c_str())==0){
            cout<<"ENVIO DATOS DEL SERVICIO ENCONTRADO\n"<<alta;
            Enviar("SE ENCONTRO EL SERVICIO SELECCIONADO");

            flag = 1;

            }*/
        //altas.read((char*)(&aux),sizeof(Servicio));
            cout<<"micro origen"<<micro.origen<<" "<<micro.fecha<<" "<<micro.turno<<"\n";
            cout<<aux.origen<<" "<<aux.fecha<<" "<<aux.turno<<"\n\n";
            if((strcmp(micro.origen,aux.origen)==0) && (strcmp(micro.fecha,aux.fecha)==0) || (strcmp(micro.origen,aux.origen)==0) && (strcmp(micro.fecha,aux.turno)==0)){
                char data[64]="";
                strcat(data,aux.origen);
                strcat(data,"-");
                strcat(data,aux.fecha);
                strcat(data,"-");
                strcat(data,aux.turno);


                cout<<"ENVIO DATOS DEL SERVICIO ENCONTRADO\n";
                Enviar(data);
                flag=1;


            }
            altas.read((char*)(&aux),sizeof(Servicio));

        }

        altas.close();
        if (flag==1){
            Enviar("NO HAY MAS DATOS PARA MOSTRAR");
        }
        if(flag==0){
            cout<<"NO SE ENCONTRO LA OPCION SOLICITADA!\n"<<endl;
           // strcpy(alta,"NO ENCONTRO SERVICIO SOLICITADO, SE CREARA SERVICIO\n");
            //alta = buffer;
            //strcpy(alta,buffer);
        //cout<<"EL ALTA DEL SERVICIO A CREAR ES: "<<buffer<<"\n";

        //ofstream altaServicio("Altas.bin",ios::out | ios::app | ios::binary);

        //altaServicio<<"\n"<<alta<<";OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO";
        //altaServicio.write((char*)(&alta),strlen(alta));
        //altaServicio.write((char *)&micro,sizeof(Servicio));
        //cout<<"SE DIO DE ALTA EL SERVICIO: "<<buffer<<"\n";
        Enviar("NO SE ENCONTRO LA OPCION SOLICITADA");
        //altaServicio.close();
        }

        return flag;

    }

int listarServicios(){

        int flag=0;

        memset(buffer,0,sizeof(buffer));
        Servicio micro;


        ifstream altas("Altas.bin",ios::out | ios::binary);
        //string lector;
        Servicio aux;
        //aux=initialize(aux);
        altas.read((char*)(&aux),sizeof(Servicio));

        while(!altas.eof()){

        //altas.read((char*)(&aux),sizeof(Servicio));

                char data[64]="";
                strcat(data,aux.origen);
                strcat(data,"-");
                strcat(data,aux.fecha);
                strcat(data,"-");
                strcat(data,aux.turno);


                cout<<"ENVIO DATOS DEL SERVICIO ENCONTRADO\n";
                Enviar(data);


            altas.read((char*)(&aux),sizeof(Servicio));

        }

        altas.close();
            Enviar("NO HAY MAS DATOS PARA MOSTRAR");


        return flag;

    }


};

int main()
{
    Server *Servidor = new Server();
    int servicio=0,isLoginValid = 0,intentos=0,clientAddrSize = sizeof(Servidor->clientAddr);

    while(true){
        if(intentos < 3 && isLoginValid != 1){ /** Validacion de los tres intentos de login**/
            isLoginValid = Servidor->validarCredenciales();
            intentos++;
        }
        if(isLoginValid==1 && intentos <= 3) /** Si las credenciales son validas puede continuar con el resto del programa**/
        {
        //servicio= Servidor->validarAltas();
         servicio= Servidor->busquedas();
        //Servidor->busquedasConDosEntradas();

        //servicio=Servidor->listarServicios(); POR AHORA PARA LISTAR HAY QUE COLOCAR UN SYSTEM PASUSE PORQUE SE GENERA UN BUCLE
        //system("pause");


          //cod = recv(Servidor->client,Servidor->buffer,1024,0); // Si es correcto que reciba --LO SAQUE CARO
            if(servicio == SOCKET_ERROR){ /** SOCKET_ERROR => -1 **/ /** Valida si el cliente se desconecto del servidor, 0 si se desconecto normalmente y -1 desconexion forzada**/
                //Servidor->CerrarSocket();
                if(closesocket(Servidor->client)==0){
                    cout<<"CIERRA SESION: "<<Servidor->userActual<<"\n-----------------------------\n\n";
                    cout << "Cliente desconectado!" << endl << "Esperando conexiones entrantes..."<<endl;}
                if((Servidor->client = accept(Servidor->server, (SOCKADDR *)&Servidor->clientAddr, &clientAddrSize)) != INVALID_SOCKET)
                {
                    cout << "Cliente conectado!" << endl;
                    intentos = 0;isLoginValid = 0;
                }
            }
        }
        if(isLoginValid==-1 || intentos == 3){  /** Si por inactividad del cliente o las credenciales son invalidas se desconecta **/
            Servidor->sendCloseMessage();
            if(closesocket(Servidor->client)==0){
            cout<<"\n-----------------------------\nCIERRA SESION: "<<Servidor->userActual<<"\n-----------------------------\n\n";
            cout << "Cliente desconectado !" << endl << "Esperando conexiones entrantes.."<<endl;}
            if((Servidor->client = accept(Servidor->server, (SOCKADDR *)&Servidor->clientAddr, &clientAddrSize)) != INVALID_SOCKET)
            {
                cout << "Cliente conectado!" << endl;
                intentos = 0;
            }
        }
    }
}
