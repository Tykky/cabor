FROM gcc:latest

RUN apt-get update && apt-get install -y git cmake gcc

WORKDIR /app

RUN git clone https://github.com/Tykky/cabor /app
RUN chmod +x build.sh && chmod +x cabor.sh && ./build.sh && ./cabor.sh --server

CMD ["bash", "-c", "git pull && ./build.sh && ./cabor.sh --server"]
