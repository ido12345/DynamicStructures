CC = gcc
FLAGS = -Wall -Wextra -std=c23 -g

.PHONY: all
all: DynamicArray/DynamicArrayExample.exe 
all: DynamicHashmap/DynamicHashmapExample.exe 
all: DynamicHeap/DynamicHeapExample.exe 
all: DynamicStack/DynamicStackExample.exe 
all: DynamicQueue/DynamicQueueExample.exe 
all: DynamicBinaryTree/DynamicBinaryTreeExample.exe 
all: DynamicLinkedList/DynamicLinkedListExample.exe

DynamicArray/DynamicArrayExample.exe: DynamicArray/DynamicArrayExample.c
	$(CC) $< -o $@ $(FLAGS)

DynamicHashmap/DynamicHashmapExample.exe: DynamicHashmap/DynamicHashmapExample.c
	$(CC) $< -o $@ $(FLAGS)

DynamicHeap/DynamicHeapExample.exe: DynamicHeap/DynamicHeapExample.c
	$(CC) $< -o $@ $(FLAGS)

DynamicStack/DynamicStackExample.exe: DynamicStack/DynamicStackExample.c
	$(CC) $< -o $@ $(FLAGS)

DynamicQueue/DynamicQueueExample.exe: DynamicQueue/DynamicQueueExample.c
	$(CC) $< -o $@ $(FLAGS)

DynamicBinaryTree/DynamicBinaryTreeExample.exe: DynamicBinaryTree/DynamicBinaryTreeExample.c
	$(CC) $< -o $@ $(FLAGS)

DynamicLinkedList/DynamicLinkedListExample.exe: DynamicLinkedList/DynamicLinkedListExample.c
	$(CC) $< -o $@ $(FLAGS)