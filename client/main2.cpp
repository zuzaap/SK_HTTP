#include <Python.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#pragma comment(lib,"ws2_32.lib")
std::string client(char *address, int *port, char *request){
    //socket init
    int Init;
    WSADATA wsaData;

    //socket to send data
    SOCKET SendSocket = INVALID_SOCKET;
    int domain = AF_INET;
    int type = SOCK_STREAM;
    int protocol = IPPROTO_TCP;


    //setting server addr
    sockaddr_in ServAddr;
    unsigned short Port = (unsigned short) port;

    //server connection
    int nConnect;

    //sending
    char *Buf = request;

    int nBytes;

    //Socket initialization
    Init = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (Init != 0) {
        return "ERROR";
        exit(1);
    }

    //creating a socket to send data
    SendSocket = socket(domain, type, protocol);

    //setting the server addr
    ServAddr.sin_family = AF_INET;
    ServAddr.sin_port = htons(Port);
    InetPton(AF_INET, address, &ServAddr.sin_addr.s_addr);


    //connection with server
    nConnect = connect(SendSocket, (struct sockaddr *) &ServAddr, sizeof(struct sockaddr));
    if (nConnect < 0) {
        return "ERROR";
        exit(1);
    }

    //sending datagram/something
    Init = sendto(SendSocket, Buf, strlen(Buf), 0, (SOCKADDR *) &ServAddr, sizeof(ServAddr));
    if (Init == SOCKET_ERROR) {
        closesocket(SendSocket);
        WSACleanup();
        return "";
    }

    char recvBuf[1024];
    std::string ss;

    do {
        for(int i=0; i< sizeof(recvBuf); i++)
            recvBuf[i] = '\0';
        nBytes = recv(SendSocket, recvBuf, 1024, 0);
        if (nBytes > 0){
            ss += recvBuf;
        }
        else if (nBytes == 0)
            break;
        else
            return "ERRRRROR";
    } while (nBytes > 0);



    //closing socket
    closesocket(SendSocket);

    //cleaning up
    WSACleanup();


    return ss;
}

static PyObject *method_client(PyObject *self, PyObject *args) {
    char *address, *request = NULL;
    int *port;

    /* Parse arguments */
    if(!PyArg_ParseTuple(args, "sls", &address, &port, &request)) {
        return NULL;
    }



    std::string response = client(address, port, request);
    return Py_BuildValue("y",response);
}


static PyMethodDef ClientMethods[] = {
        {"client", method_client, METH_VARARGS, "Python interface for client"},
        {NULL, NULL, 0, NULL}
};


static struct PyModuleDef clientmodule = {
        PyModuleDef_HEAD_INIT,
        "client",
        "Python interface for the client",
        -1,
        ClientMethods
};

PyMODINIT_FUNC PyInit_client(void) {
    return PyModule_Create(&clientmodule);
}
