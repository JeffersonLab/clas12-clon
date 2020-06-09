
/* ecalview.h */

typedef struct drawobject *DrawObjectPtr;
typedef struct drawobject
{
  TObject *obj;
  char name[128];
  double energy;

} DrawObject;


#define NDOBJ 500
