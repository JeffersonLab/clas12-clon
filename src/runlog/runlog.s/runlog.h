
/* runlog.h */

#ifdef  __cplusplus
extern "C" {
#endif

  int runlogMsgInit(const char *expid_, const char *session_, const char *unique_id_, const char *topic_);
  int runlogMsgClose();
  int runlogMsgSend(const char *msg);

#ifdef  __cplusplus
}
#endif
