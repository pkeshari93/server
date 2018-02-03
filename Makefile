#Makefile for program 5
#Author: Prakhar Keshari
all : server
server : server.o
	gcc server.o -o server
server.o : server.c 
	gcc -std=c99 -pedantic -Wall -c server.c
clean :
	rm server server.o