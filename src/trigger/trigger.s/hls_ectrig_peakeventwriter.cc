
/* hls_ectrig_peakeventwriter.cc - writes peaks to the event */

/*
getting peaks [and clusters] written to events. I attached the event builder code I use for peaks and clusters to demonstrate
the interface to the trigger and event stream. In both functions there are two types common type that interface to the readout system:

    hls::stream<trig_t> &trig_stream
         Trigger stream - there is one word to read this stream for each trigger the VTP receives.
            trig_t.t_start  is the 4ns timestamp the VTP readout window starts processing from
            trig_t.t_stop   is the 4ns timestamp the VTP readout window stops processing from
            These timestamps are 11bits, so that's 8192ns of time that constantly wraps around

    hls::stream<eventdata_t> &event_stream
         For each trigger you 32bit data words into event_stream:
            eventdata_t.data - the 32bit data words you want to go into the event
            eventdata_t.end  - a flag that must be set 1 with a dummy value on eventdata_t.data to indicate the end of event

Ben
*/

#include <ap_int.h>
#include <ap_fixed.h>
#include <hls_stream.h>

#define ECPEAK_TAG          0x4
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
  ap_uint<9>  coord;
  ap_uint<2>  view;
  ap_uint<9>  next_addr;
  ap_uint<1>  next_valid;
} cambuf_ram_t;

typedef struct
{
  ap_uint<9>  cambuf_addr;
  ap_uint<1>  valid;
} camidx_ram_t;

void
hls_ectrig_peakeventwriter(
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
            eventdata.data(30,27)  = ECPEAK_TAG;
            eventdata.data(26,26)  = inst;
            eventdata.data(25,24)  = cambuf_ram_val.view;
            eventdata.data(23,15)  = cambuf_ram_val.coord;
            eventdata.data(14,2)   = cambuf_ram_val.energy;
            eventdata.data(1,0)    = 0;
            event_stream.write(eventdata);

            eventdata.end = 0;
            eventdata.data(31,31)  = 0;
            eventdata.data(30,11)  = 0;
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
