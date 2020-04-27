# private_netdisk
## 项目简介
利用UDT库实现私有网盘，场景中由三种角色：server、client\client-qt、client\netdisk。
netdisk为位于局域网A的私有云存储，client-qt为位于局域网B的图形客户端。
client-qt与netdisk处于不同的局域网，netdisk首先建立与server的连接，在client-qt连接到server后，借助UDP Through技术实现client-qt与netdisk的连接。
# 环境配置
## Qt编译环境（略）client-qt依赖
## UDT库安装（略）
## mysql(mariadb)安装
```
yum -y install mariadb mysql-server mysql-devel
```
# 源码编译
项目根目录执行make

# 运行
1. ./server
2. ./netdisk
3. client-qt run


