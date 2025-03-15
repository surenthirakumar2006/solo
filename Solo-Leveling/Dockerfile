FROM alpine:latest

RUN apk add --no-cache socat clang

WORKDIR /app/build

COPY solo-leveling.c .
RUN clang -o solo-leveling solo-leveling.c -Wno-implicit-function-declaration -fno-stack-protector
RUN mv solo-leveling /app

WORKDIR /app

RUN rm -rf build
RUN apk del clang 

COPY flag .

RUN chmod +x solo-leveling

EXPOSE 1340

CMD ["socat", "-T60", "TCP-LISTEN:1340,fork,reuseaddr", "EXEC:./solo-leveling,pty,raw,echo=0"]