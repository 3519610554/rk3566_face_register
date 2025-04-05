# import socket
# import json

# def read_hand(read_data):
#     json_data = json.loads(read_data)
#     return json_data

# def server():
#     # 创建服务器套接字
#     server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#     server_socket.bind(('10.34.45.164', 8081))  # 绑定到指定地址和端口
#     server_socket.listen(1)  # 设置监听队列大小

#     print("等待客户端连接...")
#     client_socket, addr = server_socket.accept()  # 接受客户端连接
#     print(f"已连接到: {addr}")

#     # 接收文件名
#     filename = client_socket.recv(1024).decode('utf-8')
#     file_data = read_hand(filename)
#     print(f"文件名: {file_data}")
#     with open(filename, 'wb') as f:
#         print("开始接收文件...")
#         while True:
#             data = client_socket.recv(1024)
#             if not data:
#                 break  # 数据接收完毕
#             f.write(data)  # 将数据写入文件
#     print(f"文件 {file_data['Name']} 接收完毕！")

#     client_socket.close()  # 关闭客户端连接
#     server_socket.close()  # 关闭服务器

# if __name__ == '__main__':
#     server()

# import socket
# import threading
# import os
# from tqdm import tqdm  # 导入进度条模块

# # 接收文件的线程函数
# def receive_file_part(client_socket, part_num, filename, part_size, total_size):
#     with open(f"{filename}.part{part_num}", 'wb') as f:
#         print(f"线程 {part_num} 开始接收...")
#         total_received = 0
#         with tqdm(total=part_size, unit='B', unit_scale=True, desc=f"Part {part_num}") as pbar:
#             while True:
#                 data = client_socket.recv(part_size)
#                 if not data:
#                     break  # 数据接收完毕
#                 f.write(data)  # 将数据写入文件部分
#                 total_received += len(data)
#                 pbar.update(len(data))  # 更新进度条
#         print(f"线程 {part_num} 接收完毕！")

# # 合并所有文件部分
# def merge_file_parts(filename, total_parts):
#     with open(filename, 'wb') as f:
#         for i in range(total_parts):
#             with open(f"{filename}.part{i}", 'rb') as part_file:
#                 f.write(part_file.read())
#             # 删除每部分文件
#             os.remove(f"{filename}.part{i}")
#     print(f"文件 {filename} 合并完毕！")

# def server():
#     # 创建服务器套接字
#     server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#     server_socket.bind(('10.34.45.164', 8081))  # 绑定到指定地址和端口
#     server_socket.listen(1)  # 设置监听队列大小

#     print("等待客户端连接...")
#     client_socket, addr = server_socket.accept()  # 接受客户端连接
#     print(f"已连接到: {addr}")

#     # 接收文件名
#     filename = client_socket.recv(1024).decode('utf-8')
#     print(f"文件名: {filename}")

#     # 接收文件的总大小和分块信息（假设已经传输了这些信息）
#     total_parts = int(client_socket.recv(1024).decode('utf-8'))  # 接收总分块数
#     part_size = int(client_socket.recv(1024).decode('utf-8'))  # 每块大小
#     total_size = part_size * total_parts  # 总文件大小

#     # 创建多个线程来接收文件部分
#     threads = []
#     for part_num in range(total_parts):
#         thread = threading.Thread(target=receive_file_part, args=(client_socket, part_num, filename, part_size, total_size))
#         threads.append(thread)
#         thread.start()

#     # 等待所有线程完成
#     for thread in threads:
#         thread.join()

#     # 合并所有部分文件
#     merge_file_parts(filename, total_parts)

#     client_socket.close()  # 关闭客户端连接
#     server_socket.close()  # 关闭服务器

# if __name__ == '__main__':
#     server()


# import socket
# import os

# # 服务器配置
# SERVER_IP = "0.0.0.0"  # 监听所有 IP 地址
# SERVER_PORT = 8081
# BUFFER_SIZE = 4096  # UDP 数据包大小
# FILE_NAME = "received_file.img"

# # 创建 UDP 套接字
# server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
# server_socket.bind((SERVER_IP, SERVER_PORT))

# print(f"服务器已启动，监听 {SERVER_IP}:{SERVER_PORT}...")

# received_data = {}
# expected_seq = 0  # 期望的包序号

# while True:
#     data, client_addr = server_socket.recvfrom(BUFFER_SIZE)
    
#     # 解析数据包
#     seq_num = int.from_bytes(data[:4], 'big')  # 前 4 个字节是包序号
#     file_data = data[4:]  # 剩下的是文件数据

#     # 发送 ACK 确认
#     server_socket.sendto(seq_num.to_bytes(4, 'big'), client_addr)

#     if seq_num == -1:  # 结束标志
#         break

#     print(f"收到数据包 {seq_num}")

#     # 只接受按顺序的包
#     received_data[seq_num] = file_data

# # 保存文件
# with open(FILE_NAME, "wb") as f:
#     for i in sorted(received_data.keys()):
#         f.write(received_data[i])

# print(f"文件 {FILE_NAME} 接收完成！")
# server_socket.close()

# import socket
# import threading
# import queue
# import os

# # 服务器配置
# SERVER_IP = "0.0.0.0"
# SERVER_PORT = 8081
# BUFFER_SIZE = 4096  # 每次最大接收 4KB
# FILE_NAME = "received_file.img"

# # 创建 UDP 套接字
# server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
# server_socket.bind((SERVER_IP, SERVER_PORT))

# # 队列用于存储接收到的数据
# data_queue = queue.PriorityQueue()  # 使用优先级队列（保证有序）

# # 记录已收到的数据块
# received_data = {}
# expected_seq = 0  # 期望的下一个包序号
# is_receiving = True  # 控制写入线程的标志

# print(f"服务器启动，监听 {SERVER_IP}:{SERVER_PORT}...")


# # **1. 接收数据线程**
# def receive_data():
#     global expected_seq, is_receiving

#     while True:
#         data, client_addr = server_socket.recvfrom(BUFFER_SIZE + 4)  # 4 字节序号 + 文件数据
#         seq_num = int.from_bytes(data[:4], 'big')  # 解析包序号
#         file_data = data[4:]

#         # 发送 ACK 确认
#         server_socket.sendto(seq_num.to_bytes(4, 'big'), client_addr)

#         if seq_num == -1:  # 结束标志
#             is_receiving = False  # 停止写入线程
#             break

#         print(f"收到数据包 {seq_num}, 长度 {len(file_data)}")

#         # 将数据存入队列（保证数据有序）
#         data_queue.put((seq_num, file_data))


# # **2. 文件写入线程**
# def write_to_file():
#     global expected_seq

#     with open(FILE_NAME, "wb") as f:
#         while is_receiving or not data_queue.empty():
#             try:
#                 seq_num, file_data = data_queue.get(timeout=1)  # 1 秒超时
#                 if seq_num == expected_seq:  # 按顺序写入
#                     f.write(file_data)
#                     expected_seq += 1
#                 else:
#                     # 如果顺序不对，重新放回队列（等待正确顺序）
#                     data_queue.put((seq_num, file_data))
#             except queue.Empty:
#                 continue

#     print(f"文件 {FILE_NAME} 接收完成！")


# # **启动线程**
# recv_thread = threading.Thread(target=receive_data)
# write_thread = threading.Thread(target=write_to_file)

# recv_thread.start()
# write_thread.start()

# recv_thread.join()
# write_thread.join()

# server_socket.close()


# import socket
# import threading
# import queue
# import os

# # 服务器配置
# SERVER_IP = "0.0.0.0"
# SERVER_PORT = 8081
# BUFFER_SIZE = 4096  # 每次最大接收 4KB
# FILE_NAME = "received_file.img"
# NUM_RECEIVER_THREADS = 4  # 设定接收线程数量

# # 创建 UDP 套接字
# server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
# server_socket.bind((SERVER_IP, SERVER_PORT))

# # 用于存储接收到的数据的优先级队列（保证有序）
# data_queue = queue.PriorityQueue()

# # 记录接收状态
# is_receiving = True
# expected_seq = 0  # 期望的下一个包序号
# client_addr = None  # 记录客户端地址

# print(f"服务器启动，监听 {SERVER_IP}:{SERVER_PORT}...")


# # **1. 多个接收数据线程**
# def receive_data():
#     global client_addr, is_receiving

#     while is_receiving:
#         try:
#             data, addr = server_socket.recvfrom(BUFFER_SIZE + 4)  # 4 字节序号 + 文件数据
#             seq_num = int.from_bytes(data[:4], 'big')  # 解析包序号
#             file_data = data[4:]

#             # 记录客户端地址
#             if client_addr is None:
#                 client_addr = addr

#             # 发送 ACK 确认
#             server_socket.sendto(seq_num.to_bytes(4, 'big'), client_addr)

#             if seq_num == -1:  # 结束标志
#                 is_receiving = False
#                 break

#             print(f"[线程 {threading.current_thread().name}] 收到数据包 {seq_num}, 长度 {len(file_data)}")
            
#             # 加入队列，保证数据按序存储
#             data_queue.put((seq_num, file_data))
#         except Exception as e:
#             print(f"接收线程错误: {e}")


# # **2. 单独的文件写入线程**
# def write_to_file():
#     global expected_seq

#     with open(FILE_NAME, "wb") as f:
#         while is_receiving or not data_queue.empty():
#             try:
#                 seq_num, file_data = data_queue.get(timeout=1)  # 1 秒超时
#                 if seq_num == expected_seq:  # 按顺序写入
#                     f.write(file_data)
#                     expected_seq += 1
#                 else:
#                     # 如果顺序不对，重新放回队列
#                     data_queue.put((seq_num, file_data))
#             except queue.Empty:
#                 continue

#     print(f"文件 {FILE_NAME} 接收完成！")


# # **启动多个接收线程**
# recv_threads = []
# for _ in range(NUM_RECEIVER_THREADS):
#     thread = threading.Thread(target=receive_data, name=f"Receiver-{_}")
#     recv_threads.append(thread)
#     thread.start()

# # 启动写入线程
# write_thread = threading.Thread(target=write_to_file)
# write_thread.start()

# # 等待所有接收线程完成
# for thread in recv_threads:
#     thread.join()

# # 等待写入线程完成
# write_thread.join()

# server_socket.close()
# print("服务器已关闭")


# import socket
# import threading
# import os

# SERVER_IP = "0.0.0.0"  # 监听所有 IP
# SERVER_PORT = 8081
# BUFFER_SIZE = 4096  # 4KB 缓冲区
# SAVE_PATH = "received_file"  # 文件保存路径

# # 线程函数：接收文件块
# def receive_file(client_socket, filename, total_size):
#     file_path = os.path.join(SAVE_PATH, filename)
#     with open(file_path, "wb") as f:
#         received_size = 0
#         while received_size < total_size:
#             data = client_socket.recv(BUFFER_SIZE)
#             if not data:
#                 break
#             f.write(data)
#             received_size += len(data)
#             print(f"\r接收中: {received_size}/{total_size} 字节", end="")
#     print(f"\n文件 {filename} 接收完成！")
#     client_socket.close()

# def server():
#     os.makedirs(SAVE_PATH, exist_ok=True)  # 创建保存目录
#     server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#     server_socket.bind((SERVER_IP, SERVER_PORT))
#     server_socket.listen(5)  # 允许最多 5 个客户端同时连接

#     print(f"TCP 服务器启动，监听 {SERVER_IP}:{SERVER_PORT}...")

#     while True:
#         client_socket, addr = server_socket.accept()
#         print(f"客户端 {addr} 连接成功！")

#         # 先接收文件名和大小
#         filename = client_socket.recv(1024).decode('utf-8')
#         total_size = int(client_socket.recv(1024).decode('utf-8'))
#         print(f"接收文件: {filename}, 大小: {total_size} 字节")

#         # 启动线程接收文件
#         threading.Thread(target=receive_file, args=(client_socket, filename, total_size)).start()

# if __name__ == "__main__":
#     server()

# import socket
# import threading
# import queue
# import os

# SERVER_IP = "0.0.0.0"
# SERVER_PORT = 8081
# BUFFER_SIZE = (1024*1024)  # 4KB 缓冲区
# SAVE_PATH = "received_file"

# # 接收数据的线程
# def receive_data(client_socket, data_queue, total_size):
#     received_size = 0
#     while received_size < total_size:
#         data = client_socket.recv(BUFFER_SIZE)
#         if not data:
#             break
#         data_queue.put(data)  # 将数据放入队列
#         received_size += len(data)
#         print(f"\r接收中: {received_size}/{total_size} 字节", end="")
#     data_queue.put(None)  # 结束标志
#     client_socket.close()

# # 写入数据的线程
# def write_to_file(filename, data_queue):
#     file_path = os.path.join(SAVE_PATH, filename)
#     with open(file_path, "wb") as f:
#         while True:
#             data = data_queue.get()
#             if data is None:
#                 break  # 结束标志
#             f.write(data)
#     print(f"\n文件 {filename} 接收完成！")

# def server():
#     os.makedirs(SAVE_PATH, exist_ok=True)  # 确保目录存在
#     server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#     server_socket.bind((SERVER_IP, SERVER_PORT))
#     server_socket.listen(5)  # 允许最多 5 个客户端同时连接

#     print(f"TCP 服务器启动，监听 {SERVER_IP}:{SERVER_PORT}...")

#     while True:
#         client_socket, addr = server_socket.accept()
#         print(f"客户端 {addr} 连接成功！")

#         # 先接收文件名和大小
#         filename = client_socket.recv(1024).decode('utf-8')
#         total_size = int(client_socket.recv(1024).decode('utf-8'))
#         print(f"接收文件: {filename}, 大小: {total_size} 字节")

#         data_queue = queue.Queue()

#         # 启动接收线程
#         recv_thread = threading.Thread(target=receive_data, args=(client_socket, data_queue, total_size))
#         recv_thread.start()

#         # 启动写入线程
#         write_thread = threading.Thread(target=write_to_file, args=(filename, data_queue))
#         write_thread.start()

# if __name__ == "__main__":
#     server()


import socket
import threading
import os
import struct  # 用于解析二进制数据
from datetime import datetime 
import queue
import json

SERVER_IP = "0.0.0.0"
SERVER_PORT = 8081
BUFFER_SIZE = (1024 * 1024)
SAVE_PATH = "received_file"

# 解析客户端发送的元数据
def recv_exact(sock, size):
    """确保完整接收指定大小的字节"""
    data = b""
    while len(data) < size:
        packet = sock.recv(size - len(data))
        if not packet:
            raise ConnectionError("连接中断")
        data += packet
    return data

def handle_client(client_socket):
    try:
        # **固定头部大小 260 字节 (256 文件名 + 4 字节 part_id + 8 字节 total_size)**
        header_size = 272
        header_data = recv_exact(client_socket, header_size)

        # **解析头部数据**
        filename, part_id, total_size = struct.unpack("256sIq", header_data)
        filename = filename.decode('utf-8').rstrip('\x00')  # 去除填充的空字符
        print(f"接收文件: {filename}, 分片 {part_id}, 大小: {total_size} 字节")

        strat_time = datetime.now().timestamp()
        # **开始接收文件数据**
        file_path = os.path.join(SAVE_PATH, f"{filename}.part{part_id}")
        with open(file_path, "wb") as f:
            received_size = 0
            while received_size < total_size:
                data = client_socket.recv(BUFFER_SIZE)
                if not data:
                    break
                f.write(data)
                received_size += len(data)
                print(f"\r线程 {part_id} 接收中: {received_size}/{total_size} 字节", end="")
        print(f"\n线程 {part_id} 接收完成！时间: {datetime.now().timestamp()-strat_time}")
    except Exception as e:
        print(f"处理客户端异常: {e}")
    finally:
        client_socket.close()

files = queue.Queue()

def write_file_thread(filename):
    with open(filename, "wb") as f:
        while True:
            if not files.empty():
                data = files.get()
                if data is None:
                    break
                f.write(data)
            else:
                continue

def server():
    os.makedirs(SAVE_PATH, exist_ok=True)
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((SERVER_IP, SERVER_PORT))
    server_socket.listen(5)

    print(f"TCP 服务器启动，监听 {SERVER_IP}:{SERVER_PORT}...")
    client_socket, addr = server_socket.accept()
    print(f"客户端 {addr} 连接成功！")
    json_hand = json.loads(client_socket.recv(1024).decode('utf-8'))
    filename = json_hand['Name']
    file_size = json_hand['FileSize']
    threading.Thread(target=write_file_thread, args=(filename)).start()
    size_index = 0
    while True:
        if size_index < file_size:
            files.put(server_socket.recv(BUFFER_SIZE))
            size_index += BUFFER_SIZE
        else:
            files.put(None)
            break
    
    server_socket.close()
    print("接收完成")
    # while True:
        
    #     threading.Thread(target=handle_client, args=(client_socket,)).start()

if __name__ == "__main__":
    server()
