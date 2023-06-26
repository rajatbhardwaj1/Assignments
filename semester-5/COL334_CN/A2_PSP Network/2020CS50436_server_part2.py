from __future__ import barry_as_FLUFL
from asyncio.log import logger
from base64 import encode
import chunk
from concurrent.futures import thread
from functools import lru_cache
from glob import glob
from http import client, server
import imp
from lzma import CHECK_NONE
import os
import time 

import socket
from sqlite3 import Connection 
import threading
from xmlrpc.client import Server
from collections import OrderedDict

import logging


logging.basicConfig(filename="stds.log", 
					format='%(asctime)s %(message)s', 
					filemode='w')
lock = threading.Lock()
logger.setLevel(logging.DEBUG) 


PORT_SERVER_UDP = 5504



PORT_SERVER_TCP = 5055

start_time = time.time()



# HEADERS SIZES 

CHUNK_ID_SIZE = 8 

REQUEST_SIZE = 8 

CLIENT_NAME_SIZE = 8 

REQUEST_SIZE_1 = CLIENT_NAME_SIZE + REQUEST_SIZE

CHUNK_SIZE_SIZE = 4 

HEADER_SIZE = CHUNK_ID_SIZE+CHUNK_SIZE_SIZE

ENCODING = 'utf-8'
CHUNK_SIZE = 1024

TOTAL_SIZE = CHUNK_SIZE+HEADER_SIZE

client_port_id = []   #will be a tuple of (conn_tcp , addr_udp)

server_broadcast_sock = [] 

server_broadcast_tcp = [] 

NUM_CLIENTS  = 5

SERVER = '0.0.0.0'

bytes_sent_len = [] 

DISCONNECT_MSG = "!DISCONNECT"

ACKNOWLEDGEMENT_SIZE = 10

ADDR_SERVER_UDP = (SERVER , PORT_SERVER_UDP)
server_UDP = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
server_UDP.bind(ADDR_SERVER_UDP)
server_UDP.settimeout(0.5)

ADDR_SERVER_TCP =   (SERVER , PORT_SERVER_TCP)


server_TCP = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_TCP.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
server_TCP.bind(ADDR_SERVER_TCP)
server_TCP.listen()

duplicate = {} 

all_pack_rec = 0 
all_pack_rec_lis = []
sent_chunk_len = 0
class LRUCache:

    def __init__(self, size):
        self.cache = OrderedDict()
        self.capacity = size

    def get(self , key: int):
        if key not in self.cache:
            return -1
        else:
            self.cache.move_to_end(key)
            return self.cache[key]
        
    def put(self, key: int, value:str):
        self.cache[key] = value
        self.cache.move_to_end(key)
        if len(self.cache) > self.capacity:
            self.cache.popitem(last = False)

lru_cache = LRUCache(NUM_CLIENTS)      

def new_connection():                           #not implemented for udp
    # conn_UDP , addr_UDP = server_UDP.accept()
  
    while True :
        conn_TCP , addr_TCP = server_TCP.accept()


        print(f'TCP connection is active with client [{addr_TCP}]')

        client_port_UDP, addr_UDP = server_UDP.recvfrom(6)
      

        print(f'UDP connection is active with client [{addr_UDP}] ')



        print(f'appending udp and tcp ports to list ....')

        client_port_id.append((conn_TCP, addr_UDP))
        if len(client_port_id) == NUM_CLIENTS:
            
            break
     

def decode_headers(headers):       #return chunk_id , chunk_size
    if(headers[0] == '#'):
        chunk_list_size = int(headers[1:HEADER_SIZE])
        return chunk_list_size
    chunk_id = int(headers[0:CHUNK_ID_SIZE])
    chunk_size = int(headers[CHUNK_ID_SIZE : HEADER_SIZE])
    return chunk_id , chunk_size

def make_header(chunk_id ,  chunk_size ):
    global CHUNK_ID_SIZE , CHUNK_SIZE_SIZE 
    p1 = str(chunk_id).encode()
    p1 += b' '*(CHUNK_ID_SIZE - len(p1))

    p2 = str(chunk_size).encode()
    p2 += b' '*(CHUNK_SIZE_SIZE - len(p2))

    header = p1 + p2 
    return header 


def adjust_data_size(data):
    data += b' '*(CHUNK_SIZE - len(data))
    return data




#encoding the file and making 1kb chunks  
def splitfile(filename):

    with open(filename,"r") as f:
        text = f.readlines()[0]
        logger.info(f'Total file length = {len(text)}')
        text = text.encode(ENCODING)
        
        logger.info(f'Total encoded file length = {len(text)}')

        length = len(text)
        chunk_list = []
        num_chunks = length // CHUNK_SIZE
        i = 0 
        if length % CHUNK_SIZE > 0 :
            i += 1 
        logger.info(f'The total number of chunks = {num_chunks+ i }')
        testing = b''
        for chunk_id in range(0 , num_chunks):
            header = make_header(chunk_id , CHUNK_SIZE )
      
            data = text[chunk_id*CHUNK_SIZE: (chunk_id+1)*CHUNK_SIZE]
            chunk_list.append((chunk_id, header + data ))
            # testing += data 
        if(num_chunks*CHUNK_SIZE < length):
            header = make_header(num_chunks , length - num_chunks*CHUNK_SIZE)
            # testing += text[num_chunks*CHUNK_SIZE: length] 

            data = adjust_data_size( text[num_chunks*CHUNK_SIZE: length] )
            
            chunk_list.append((num_chunks, header + data ))
        # logger.info(f'splitting size = {len(testing.decode())}')
        for i in range(NUM_CLIENTS):
            data = '#'.encode() 
            data += str(len(chunk_list) - i ).encode()
            data += b' '*(HEADER_SIZE - len(data))
            chunk_list.append(('#', data))
        return chunk_list

def recieve_ack(chunk_id , conn ):    #ack for initial sending ...
    ack , addr  = server_UDP.recvfrom(ACKNOWLEDGEMENT_SIZE )    
    if addr == conn and chunk_id == int(ack.decode())  :
        # logger.info(f'Correct Ack from client recieved for chunk #{chunk_id}')


        return True 
    else :
        # logger.info(f'Incorrect ack ! resending packet #{chunk_id} again')
        return False 


def send_chunk(chunk , conn , chunk_id):
    logger.info(f'sending chunk {chunk[0]} to client {conn} ')
    while True : 
        server_UDP.sendto(chunk[1] , conn)
        try :
            # logger.info(f'Acknowledgement received ... checking if the acknowledgement is correct!')
            if recieve_ack(chunk_id , conn):
                break

            break 
        except socket.timeout:
            # logger.info(f'The acknowledgement from the client #{conn[1]} not recieved. Resending chunk #{chunk[0]}')
            pass

    
def distribute_file_to_clients(filename):
    chunk_list = splitfile(filename)
    for i in range(len(chunk_list)):
        print(f'Sending chunk #{i} to client #{i%NUM_CLIENTS}')
        send_chunk(chunk_list[i], client_port_id[i%NUM_CLIENTS][1], i )#sending via udp 


# server ports to recieve requested chunks 

def setup_udp_receive_chunks():
    for i in range(NUM_CLIENTS):
        #udp sockets for recieving requested chunks from the clients

        print('setting up udp server ports....')
        server_UDP = socket.socket(socket.AF_INET , socket.SOCK_DGRAM)
        server_UDP.bind((SERVER , 7000+i))
        server_UDP.settimeout(5)
        server_cnct_msg_udp = 'HELLO!'  #sending hi to server to save addr
        server_cnct_msg_udp = server_cnct_msg_udp.encode(ENCODING)
        server_UDP.sendto(server_cnct_msg_udp,client_port_id[0][1])
        print(f'The server port# {server_UDP.getsockname()[1]} is connected(udp) and ready to receive requested chunks!')
        server_broadcast_sock.append(server_UDP)

# ports to receive requests from the client. 

def init_tcp_ports_broadcast():
    for i in range(NUM_CLIENTS):
        all_pack_rec_lis.append(False)
        ADDR_SERVER_TCP = (SERVER , i+9000)
        server_TCP = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_TCP.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server_TCP.bind(ADDR_SERVER_TCP)
        server_TCP.listen()
        server_broadcast_tcp.append(server_TCP)
        print(f'tcp broadcasting server #{i} initialised !')


#broadcasting request to all clients using tcp ports 

#code.....


def broadcast(request , udp_client_port):

    h1 = str(request).encode('utf-8')

    h1 += b' '*(REQUEST_SIZE - len(h1))

    h2 = str(udp_client_port).encode('utf-8')

    h2 += b' '*(CLIENT_NAME_SIZE - len(h2))

    enc_req = h1 + h2 


    for id in range(NUM_CLIENTS):
        server_TCP_random = socket.socket(socket.AF_INET , socket.SOCK_STREAM)
        ADDR_client_TCP=   (SERVER , 4000+id)

        server_TCP_random.connect(ADDR_client_TCP)
        # print(f'requesting chunk #{request} from client #{id}')
        server_TCP_random.send(enc_req)
        server_TCP_random.close()

# server recieving the requested(broadcasted) chunk 
def accept_udp(id):
    global lock , duplicate, all_pack_rec, all_pack_rec_lis
    
    while True :
        if(all_pack_rec == NUM_CLIENTS ):
            logger.info(f'closing server tcp acceptor #{id} !')
            break
        

        # conn_TCP , addr_TCP = server_broadcast_tcp[id].accept()
        # msg = conn_TCP.recv(TOTAL_SIZE)
        try:
            msg, addr_UDP = server_broadcast_sock[id].recvfrom(TOTAL_SIZE)
        except socket.timeout:
            break
            

        header = msg[0:HEADER_SIZE]
        header = header.decode()
        chunk_id , chunk_size = decode_headers(header)
        

        # print(f'chunk #{chunk_id} recieved by tcp server #{id}')
        data = msg[HEADER_SIZE:]

        lru_cache.put(chunk_id , data[:chunk_size])
        

        # print(f'Requested chunk of #{chunk_id} received by server from {addr_UDP[1]}')
        
TCP_threads = []



#code from here .................................................
def send_reqeuested_chunk(id):
    #recieving request
    global lock , lru_cache , server_broadcast_sock, sent_from_cache, all_pack_rec, all_pack_rec_lis

    connected = True
    while connected:

        logger.info(f'Taking request from client #{id}')

        conn_TCP , addr_TCP = server_broadcast_tcp[id].accept()
        request = conn_TCP.recv(REQUEST_SIZE)

        # request, addr_UDP = server_broadcast_sock[id].recvfrom(REQUEST_SIZE)
        request = int(request.decode())
        logger.info(f'client #{id} requested chunk #{request} from the server')

        if(request == -1):
            all_pack_rec += 1 
            all_pack_rec_lis[id] = True 
            logger.info(f'clients with complete packets = {all_pack_rec}')
            if all_pack_rec == NUM_CLIENTS:
                broadcast(-1 , addr_TCP[1] - 9000)
            connected = False 
            break
        else :
            
            i = lru_cache.get(request)
            
            if(i == -1):
                
                logger.info(f'server does not have the requested chunk #{request} broadcasting...')
                break_loop = False 

                while break_loop == False  :
                    break_loop =True 
               
                    broadcast(request , addr_TCP[1] - 9000)
                    timeout = time.time() + 5
                    while lru_cache.get(request) == -1  :
                        if time.time() > timeout:  #code this 
                            #broadcast(request , addr_TCP[1] - 9000)
                            print(f'client #{id} Re requesting chunk #{request}')
                            break_loop = False 
                            break 
                        pass
                   
                     
                i = lru_cache.get(request)
                chunk = i  
                if type(i) == int :
                    print(i)
                header = make_header(request , len(chunk) )
                
                send_chunk = header + chunk 

                #need to send the requested chunk to the udp

                # ADDR_client_TCP=   (SERVER , 4000+id)
                # server_TCP_random = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                # server_TCP_random.connect(ADDR_client_TCP)
                # server_TCP_random.send(send_chunk)
                server_UDP_random = socket.socket(socket.AF_INET , socket.SOCK_DGRAM)
                server_UDP_random.sendto(send_chunk ,client_port_id[id][1])



                
            else : 
                chunk = i     
                header = make_header(request , len(chunk) )
                
                send_chunk = header + chunk 
                # ADDR_client_TCP=   (SERVER ,4000+id)
                # server_TCP_random = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                # server_TCP_random.connect(ADDR_client_TCP)
                # server_TCP_random.send(send_chunk)
                server_UDP_random = socket.socket(socket.AF_INET , socket.SOCK_DGRAM)
                server_UDP_random.sendto(send_chunk ,client_port_id[id][1])





TCP_threads = []


def handle_client_chunks():
    print(f'init hearing')
    for id in range (NUM_CLIENTS):
        thread = threading.Thread(target=accept_udp, args=(id,))
        thread.start()
        TCP_threads.append(thread)
                  
HCR_threads = [] 

def handle_client_request():
    for id in range(NUM_CLIENTS):
        thread = threading.Thread(target=send_reqeuested_chunk, args=(id,))
        thread.start()
      

new_connection()

distribute_file_to_clients('OneDrive_1_7-9-2022/A2_small_file.txt')
# os.remove('OneDrive_1_7-9-2022/A2_small_file.txt')

setup_udp_receive_chunks()
init_tcp_ports_broadcast()

handle_client_request()         
handle_client_chunks()

for x in TCP_threads:
    x.join()

end_time = time.time() 
logger.info(f'Total time taken by the code = {end_time - start_time}')
