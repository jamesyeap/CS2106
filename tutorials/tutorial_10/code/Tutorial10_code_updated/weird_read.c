#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void a();
void b();
void c();

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;
  // a();
  // b();
  c();
}

void a() {
  char next_char;
  // Opens alice.txt
  int fd = open("alice.txt", O_RDONLY);

  // Create new child process
  fork();

  // Both processes read and print one character from the file
  for (int i = 0; i < 50; i++) {
    read(fd, &next_char, 1);
    printf("%c", next_char);
    usleep(rand() % 1000);
  }

  close(fd);
}

void b() {
  char next_char;
  fork();

  // We open the file AFTER the fork here
  int fd = open("alice.txt", O_RDONLY);

  // Both processes read and print one character from the file
  for (int i = 0; i < 50; i++) {
    read(fd, &next_char, 1);
    printf("%c", next_char);
    usleep(rand() % 1000);
  }

  close(fd);
}

char buf[50];

void c() {
  char next_char;
  FILE *fp = fopen("alice.txt", "r");
  setbuffer(fp, buf, 50);

  // remove for c) part i.
  fread(&next_char, 1, 1, fp);
  printf("%c", next_char);

  fork();
  for (int i = 0; i < 49; i++) {
    fread(&next_char, 1, 1, fp);
    printf("%c", next_char);
    usleep(rand() % 1000);
  }

  fclose(fp);
}
