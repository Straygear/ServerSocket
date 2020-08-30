#include "SERVER.h"
#include "Database.h"
int main()
{

   /* Database b;
    b.connectDatabase();
    b.insertPerson();*/

    SERVER S;

    S.initialiseSocket();
    S.createSocket();
    S.bindSocket();
    S.listenSocket();
    S.connectMultipleClients();
   // doit = S.recieveSendData();
    S.disconnectServer();

    return 0;
}
