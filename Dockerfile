# 使用Ubuntu作为基础镜像
FROM ubuntu:latest

# 更新包列表并安装必要的软件包
RUN apt-get update && apt-get install -y \
    build-essential \
    linux-headers-generic \
    gdb \
    make \
    xmake \
    cmake \
    gcc \
    git \
    vim \
    sudo \
    kmod

# 创建一个用户以避免使用root用户
RUN useradd -ms /bin/bash wps && echo "wps:wps" | chpasswd && adduser wps sudo

# 切换到非root用户
USER wps
WORKDIR /home/wps

# 将simplefs.ko复制到容器中
#COPY simplefs.ko /home/wps/simplefs.ko

# 设置工作目录
#WORKDIR /home/wps

# 提供一个入口点以便于调试
CMD ["bash"]