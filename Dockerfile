FROM gcc


RUN mkdir -p /home/user/main


WORKDIR /home/user/main


COPY . /home/user/main


RUN apt-get update 


RUN gcc 4task.c -pthread 





