# IP QoS Helper
## Introduction

Class  ```IPQoSHelper``` 用于辅助设置socket的一些网络特征，包括三个主要函数
- ```int set_and_check_ip_tos(int sock, int tos)``` 用于设置IP头的ToS特性，ToS是一个8-bit的包头位置，用于网络设备（交换机）上的QoS调度
- ```int set_and_check_ip_priority(int sock, int prio)``` 用于设置sk_buf->priority特性，该优先级作用于协议栈中数据包的队列排队问题，用于协议栈上的流量控制，实现QoS调度目的。
- ```int set_socket_reuse(int sock)``` 用于设置端口复用，避免socket不及时释放而建链失败。


## Usage
```ip_qos_helper.h``` 可以同时支持C和C++语言，可以直接拿来使用

C语言环境下:
```c
#include "ip_qos_helper.h"

// ...
int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
// ...

set_and_check_ip_tos(sock, IPTOS_LOWDELAY);
set_and_check_ip_priority(sock, 4);
set_socket_reuse(sock);

//...
```

C++语言环境下:
```c
#include "ip_qos_helper.h"

// ...
int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
// ...

newplan::IPQoSHelper::set_and_check_ip_tos(sock, IPTOS_LOWDELAY);
newplan::IPQoSHelper::set_and_check_ip_priority(sock, 4);
newplan::IPQoSHelper::set_socket_reuse(sock);

//...
```

## Verification
运行服务端：
```shell
~/p/t/n/server> gcc server.c -o server
~/p/t/n/server> ./server
[INFO] Having modified IP ToS for socket: 3 from 0x0(old) to 0x10(new)
[INFO] Having modified IP priority for socket: 3 from 6(old) to 4(new)
```
运行客户端：
```shell
~/p/t/n/server> gcc client.c -o client
~/p/t/n/server> ./client
[INFO] Having modified IP ToS for socket: 3 from 0x0(old) to 0x10(new)
[INFO] Having modified IP priority for socket: 3 from 6(old) to 4(new)
Message form server: [0] Hello World!
Message form server: [1] Hello World!
Message form server: [2] Hello World!
Message form server: [3] Hello World!
Message form server: [4] Hello World!
Message form server: [5] Hello World!
...
```
使用tcpdump抓包查看
```shell
~ sudo tcpdump -i lo tcp port 1234  -nnvvS
14:22:04.860846 IP (tos 0x10, ttl 64, id 64125, offset 0, flags [DF], proto TCP (6), length 52)
    127.0.0.1.57080 > 127.0.0.1.1234: Flags [.], cksum 0xfe28 (incorrect -> 0xe2a2), seq 3884029930, ack 3737377325, win 49, options [nop,nop,TS val 3821099293 ecr 3821099293], length 0
14:22:05.861018 IP (tos 0x10, ttl 64, id 47800, offset 0, flags [DF], proto TCP (6), length 180)
    127.0.0.1.1234 > 127.0.0.1.57080: Flags [P.], cksum 0xfea8 (incorrect -> 0xd3f2), seq 3737377325:3737377453, ack 3884029930, win 43, options [nop,nop,TS val 3821100293 ecr 3821099293], length 128
14:22:05.861042 IP (tos 0x10, ttl 64, id 64126, offset 0, flags [DF], proto TCP (6), length 52)
    127.0.0.1.57080 > 127.0.0.1.1234: Flags [.], cksum 0xfe28 (incorrect -> 0xda51), seq 3884029930, ack 3737377453, win 50, options [nop,nop,TS val 3821100293 ecr 3821100293], length 0
```
看到<font color='red'> tos 0x10 </font>，表明设置成功

<font color='red'> Note</font>: 默认的排队规则使用 ```pfifo_fast``` 使用三个规则，low_delay放入快速队列中，其他数据放入别的队列中。

## Reference
[Linux traffic control](https://tonydeng.github.io/sdn-handbook/linux/tc.html)