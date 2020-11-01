#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

using namespace std;

struct Servicio{
    char origen[64];
    char fecha[64];
    char turno[64];
    char asientos[64];
};

void lectura();
void escritura();

int main()
{
    escritura();
    lectura();
}

void lectura(){
    ofstream altaLegible("Legible.txt",ios::out | ios::app);
    ifstream lecturaLegible("Legible.txt",ios::in | ios::app);
    string lector;

    getline(lecturaLegible,lector,';');
    while(!lecturaLegible.eof()){
        cout<<lector<<"\n";
        getline(lecturaLegible,lector,';');
    }
    altaLegible.close();
    lecturaLegible.close();
}

void escritura(){
    ifstream altas("C:\\Users\\magostino\\Documents\\Github\\Redes\\server\\Altas.bin",ios::out | ios::binary);
    ofstream altaLegible("Legible.txt",ios::out | ios::trunc);

    Servicio aux;
    altas.read((char*)(&aux),sizeof(aux));

    while(!altas.eof() ){
        altaLegible<<"Origen:" <<aux.origen<<"- Fecha:" <<aux.fecha<<"- Turno:" <<aux.turno<<"- Ascientos:" <<aux.asientos<<";\n";
        altas.read((char*)(&aux),sizeof(Servicio));
    }
    altas.close();
}
