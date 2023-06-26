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

server_UDP_random_broadcast = [] 

NUM_CLIENTS  = 5

SERVER = '0.0.0.0'

bytes_sent_len = [] 

DISCONNECT_MSG = "!DISCONNECT"


ADDR_SERVER_UDP = (SERVER , PORT_SERVER_UDP)
server_UDP = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
server_UDP.bind(ADDR_SERVER_UDP)


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

lru_cache = LRUCache(5)      

def new_connection():                           #not implemented for udp
    # conn_UDP , addr_UDP = server_UDP.accept()
  
    while True :
        conn_TCP , addr_TCP = server_TCP.accept()


        print(f'TCP connection is active with client [{addr_TCP}]')

        client_port_UDP, addr_UDP = server_UDP.recvfrom(6)
      

        print(f'UDP connection is active with client [{addr_UDP}] ')



        print(f'appending udp and tcp ports to list ....')

        client_port_id.append((conn_TCP, addr_UDP))
        server_UDP_random = socket.socket(socket.AF_INET , socket.SOCK_DGRAM)
        server_UDP_random_broadcast.append(server_UDP_random)


        if len(client_port_id) == NUM_CLIENTS:
            print(client_port_id)
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

def send_chunk(chunk , conn):
    if chunk[0] != '#' :
        if chunk[0]%1000 == 0 :
            logger.info(f'sending chunk {chunk[0]} to client {conn.getpeername()} ')
            # logger.info(f'Time taken by the code is {time.time() - start_time}')
    conn.send(chunk[1])
    


def distribute_file_to_clients(filename):
    chunk_list = splitfile(filename)
    for i in range(len(chunk_list)):
        if i%1000 == 0  :
            print(f'Sending chunk #{i} to client #{i%NUM_CLIENTS}')
        send_chunk(chunk_list[i], client_port_id[i%NUM_CLIENTS][0])
    
#udp ports to recieve chunk request from the clients 


def setup_broadcast():
    for i in range(NUM_CLIENTS):
        #udp socket init

        print('setting up broadcast....')
        server_UDP = socket.socket(socket.AF_INET , socket.SOCK_DGRAM)
        server_UDP.bind((SERVER , 7000+i))
        server_cnct_msg_udp = 'HELLO!'  #sending hi to server to save addr
        server_cnct_msg_udp = server_cnct_msg_udp.encode(ENCODING)
        server_UDP.sendto(server_cnct_msg_udp,client_port_id[0][1])
        print(f'The server port# {server_UDP.getsockname()[1]} is connected(udp) and ready for broadcast!')
        server_broadcast_sock.append(server_UDP)


def init_tcp_ports_broadcast():
    for i in range(NUM_CLIENTS):
        all_pack_rec_lis.append(False)
        ADDR_SERVER_TCP = (SERVER , i+9000)
        server_TCP = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_TCP.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server_TCP.bind(ADDR_SERVER_TCP)
        server_TCP.listen()
        server_TCP.settimeout(5)
        server_broadcast_tcp.append(server_TCP)
        print(f'tcp broadcast reply receiver #{i} initialised !')


def broadcast_thread(enc_req , id ):
    server_UDP_random_broadcast[id].sendto(enc_req ,client_port_id[id][1])


def broadcast(request , udp_client_port):

    h1 = str(request).encode('utf-8')

    h1 += b' '*(REQUEST_SIZE - len(h1))

    h2 = str(udp_client_port).encode('utf-8')

    h2 += b' '*(CLIENT_NAME_SIZE - len(h2))

    enc_req = h1 + h2 

    server_UDP_random = socket.socket(socket.AF_INET , socket.SOCK_DGRAM)
    for id in range(NUM_CLIENTS):
        if request != '#':
            if request%1000 == 0:
                pass
                # print(f'requesting chunk #{request} from client #{id}')
        threadtemp = threading.Thread(target= broadcast_thread , args= (enc_req, id))
        threadtemp.start()


#server tcp ports to recieve chunks from the clients 

def accept_tcp(id):
    global lock , server_broadcast_tcp, duplicate, all_pack_rec, all_pack_rec_lis
    
    while True :
        
        try:
            conn_TCP , addr_TCP = server_broadcast_tcp[id].accept()
        except socket.timeout:
            break
        msg = conn_TCP.recv(TOTAL_SIZE)
        header = msg[0:HEADER_SIZE]
        header = header.decode()
        chunk_id , chunk_size = decode_headers(header)
        # if(chunk_id == -1 ):
        #     logger.info(f'closing server tcp acceptor #{id} !')
        #     break
        # if chunk_id != '#':
        #     if chunk_id %1000 == 0: 

                # print(f'chunk #{chunk_id} recieved by tcp server #{id}')
        data = msg[HEADER_SIZE:]

        lru_cache.put(chunk_id , data[:chunk_size])
        
        # if chunk_id %1000 == 0 :
        #     print(f'Requested chunk of #{chunk_id} received by server from {addr_TCP[1]}')
        

                
HCC_threads = [] 



sent_from_cache  = 0 



def send_reqeuested_chunk(id):
    #recieving request
    global lock , lru_cache , server_broadcast_sock, sent_from_cache, all_pack_rec, all_pack_rec_lis
    

    connected = True
    while connected:

        #logger.info(f'Taking request from client #{id}')

                    
        request, addr_UDP = server_broadcast_sock[id].recvfrom(REQUEST_SIZE)
        request = int(request.decode())
        # if request%1000 == 0:
            ##logger.info(f'client #{id} requested chunk #{request} from the server and time taken for code is {time.time() - start_time}')

        if(request == -1):
            all_pack_rec += 1 
            all_pack_rec_lis[id] = True 
            # logger.info(f'clients with complete packets = {all_pack_rec}')
            if all_pack_rec == NUM_CLIENTS:
                broadcast(-1 , addr_UDP[1] - 6000)
            connected = False 
            break
        else :
            
            i = lru_cache.get(request)
            
            
            if(i == -1):
                
                ##logger.info(f'server does not have the requested chunk #{request} broadcasting...')
                reloop = True 

                while reloop :
                    reloop = False 
                    
                    broadcast(request , addr_UDP[1] - 6000)
                    timeout = time.time() + 5
                    while lru_cache.get(request) == -1  :
                        if time.time() > timeout:
                            reloop = True 
                            break
                        pass
                    
                i = lru_cache.get(request)
                chunk = i  
                header = make_header(request , len(chunk) )
                
                send_chunk = header + chunk 

                #need to send the requested chunk to the tcp 

                ADDR_client_TCP=   (SERVER , 4000+id)
                server_TCP_random = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                server_TCP_random.connect(ADDR_client_TCP)
                server_TCP_random.send(send_chunk)
                server_TCP_random.close()
                
            else : 
                chunk = i     
                header = make_header(request , len(chunk) )
                
                send_chunk = header + chunk 
                ADDR_client_TCP=   (SERVER ,4000+id)
                server_TCP_random = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                server_TCP_random.connect(ADDR_client_TCP)
                server_TCP_random.send(send_chunk)
                server_TCP_random.close()





TCP_threads = []

def handle_client_chunks():
    print(f'init hearing')
    for id in range (NUM_CLIENTS):
        thread = threading.Thread(target=accept_tcp, args=(id,))
        thread.start()
        TCP_threads.append(thread)
                  

HCR_threads = [] 
def handle_client_request():
    for id in range(NUM_CLIENTS):
        thread = threading.Thread(target=send_reqeuested_chunk, args=(id,))
        thread.start()
      
    

new_connection()

# update the location of the file 
distribute_file_to_clients('OneDrive_1_7-9-2022/A2_small_file.txt')
# os.remove('OneDrive_1_7-9-2022/A2_small_file.txt')
init_tcp_ports_broadcast()


setup_broadcast()
handle_client_request()         
handle_client_chunks()

for x in HCR_threads:
    x.join()
for x in TCP_threads:
    x.join()


end_time = time.time() 

logger.info(f'Total time taken by the code = {end_time - start_time} seconds')


