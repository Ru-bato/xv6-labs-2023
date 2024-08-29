#include "user/user.h"

int
main(int argc, char *argv[])
{
  if(argc < 2){
    fprintf(2, "Usage: sleep argument should not be null\n"); // 2 refers to stderr
    exit(1);
  }
  else if(argc > 2){
    fprintf(2, "Usage: sleep argument should not be more than one");
    exit(1);
  }
  else{
    int tick = atoi(argv[1]);
    if(tick < 0)
        tick = 0; // do not exit if tick is invalid
    sleep(tick);
    exit(0); // done
  }
}
