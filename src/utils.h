

typedef struct {
  int Hours;
  int Minutes;
  int Seconds;
  int MilliSeconds;
} STime;

void timeFromInt ( STime *, int );
int  intFromTime ( STime * );
