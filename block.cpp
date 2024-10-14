#include <iostream>
#include <fstream>
#include <unistd.h>

using namespace std;

int main(int argc, char *argv[]){

   int response;
   string addSite;

   chdir("/etc");

   ofstream sitesToWrite(".blockedSites", ios::app);
   ofstream hosts("hosts", ios::app);
   ifstream sitesToRead(".blockedSites");

   while(true){

      cout << "1-Visualizar sites bloqueados" << endl << "2-Bloquear site" << endl << "3-Sair" << endl << "Escolha uma opção: ";
      cin >> response;

      switch (response){

      case 1:
         system("clear");
         system("cat .blockedSites");
         break;

      case 2:
         system("clear");
         cout << "Digite o domínio do site a ser bloqueado: ";
         cin >> addSite;

         sitesToWrite << "127.0.0.1 " << addSite << endl;
         hosts << "127.0.0.1 " << addSite << endl;

         cout << addSite << " adicionado com sucesso!" << endl;
         break;

      case 3:
         system("clear");
         cout << "Saindo..." << endl;
         return 0;

      default:
         cout << "Comando inválido" << endl;
         break;
      }

   }

}
