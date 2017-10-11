
/* epicsutil.h */


// max entries in file
#define MAX_ENTRY 5000

// max number of elements in epics message
#define MAX_ELEM 1000

// max number of PVS
#define MAX_PVS 100


#ifdef  __cplusplus
extern "C" {
#endif

typedef struct epics_struct
{
  char *name;
  char *chan;
  char *get;
} EPICS_CONFIG;

int getepics(int nPvs, char *canames[MAX_ENTRY], float *valfloats);

int getepicsvector(char *caname, float *valfloats);

int insert_msg(const char *name, const char *facility, const char *process, const char *msgclass, 
	       int severity, const char *status, int code, const char *text);

int epics_msg_sender_init(char *application, char *unique_id);

int epics_msg_send(const char *caname, const char *catype, int nelem, void *data);

int epics_msg_receiver_init(char *application);

#ifdef  __cplusplus
}
#endif
