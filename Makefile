CC = gcc

all: PrimaryLikesServer ParentProcess

PrimaryLikesServer: PrimaryLikesServer.c
	$(CC) -o PrimaryLikesServer PrimaryLikesServer.c

ParentProcess: ParentProcess.c
	$(CC) -o ParentProcess ParentProcess.c

run: all
	./PrimaryLikesServer & ./ParentProcess

clean:
	rm -f PrimaryLikesServer ParentProcess
