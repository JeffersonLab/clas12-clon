
/* hls_ectrig_clustereventwriter.cc - writes clusters to the event */

#include <ap_int.h>
#include <ap_fixed.h>
#include <hls_stream.h>

#define ECCLUSTER_TAG       0x5
#define MAX_BIN_SCAN_DEPTH  32

typedef struct
{
  ap_uint<11> t_start;
  ap_uint<11> t_stop;
} trig_t;

typedef struct
{
  ap_uint<32> data;
  ap_uint<1>  end;
} eventdata_t;

typedef struct
{
  ap_uint<5>  t_low;
  ap_uint<13>  energy;
  ap_uint<9>  x;
  ap_uint<9>  y;
  ap_uint<9>  next_addr;
  ap_uint<1>  next_valid;
} cambuf_ram_t;

typedef struct
{
  ap_uint<9>  cambuf_addr;
  ap_uint<1>  valid;
} camidx_ram_t;

void
hls_ectrig_clustereventwriter(
    ap_uint<1> inst,
    hls::stream<trig_t> &trig_stream,
    hls::stream<eventdata_t> &event_stream,
    cambuf_ram_t cambuf_ram[512],
    camidx_ram_t camidx_ram[64])
{
  ap_uint<6> idx_addr;
  eventdata_t eventdata;

  while(1)
  {
    trig_t trig = trig_stream.read();
    ap_uint<11> w = (trig.t_stop - trig.t_start);
    ap_uint<6> idx_addr = trig.t_start.range(10,5);

    do
    {
      camidx_ram_t camidx_ram_val = camidx_ram[idx_addr];
      ap_uint<9> next_addr = camidx_ram_val.cambuf_addr;

      if(camidx_ram_val.valid)
      {
        for(ap_uint<10> cnt = 0; cnt < MAX_BIN_SCAN_DEPTH; cnt++)
        {
          cambuf_ram_t cambuf_ram_val = cambuf_ram[next_addr];
          ap_uint<11> t = (idx_addr, cambuf_ram_val.t_low) - trig.t_start;

          if(t < w)
          {
            eventdata.end = 0;
            eventdata.data(31,31)  = 1;
            eventdata.data(30,27)  = ECCLUSTER_TAG;
            eventdata.data(26,26)  = inst;

            eventdata.data(25,17)  = cambuf_ram_val.y;
            eventdata.data(16,8)  = cambuf_ram_val.x;
            eventdata.data(7,0)    = 0;
            event_stream.write(eventdata);

            eventdata.end = 0;
            eventdata.data(31,31)  = 0;
            eventdata.data(30,30)  = 0;
            eventdata.data(29,17)  = cambuf_ram_val.energy;
            eventdata.data(16,11)  = 0;
            eventdata.data(10,0)  = t;
            event_stream.write(eventdata);
          }

          next_addr = cambuf_ram_val.next_addr;

          if(!cambuf_ram_val.next_valid)
            break;
        }
      }
    }  while(idx_addr++ != trig.t_stop.range(10,5));

    eventdata.data = 0xFFFFFFFF;
    eventdata.end = 1;
    event_stream.write(eventdata);
  }
}
