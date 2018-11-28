#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>

#define SL1_BIN_SIZE  2
#define SL2_BIN_SIZE  2
#define SL3_BIN_SIZE  3
#define SL4_BIN_SIZE  3
#define SL5_BIN_SIZE  3
#define SL6_BIN_SIZE  3

#define SL1_BIN_COUNT ((112+SL1_BIN_SIZE-1)/SL1_BIN_SIZE)
#define SL2_BIN_COUNT ((112+SL2_BIN_SIZE-1)/SL2_BIN_SIZE)
#define SL3_BIN_COUNT ((112+SL3_BIN_SIZE-1)/SL3_BIN_SIZE)
#define SL4_BIN_COUNT ((112+SL4_BIN_SIZE-1)/SL4_BIN_SIZE)
#define SL5_BIN_COUNT ((112+SL5_BIN_SIZE-1)/SL5_BIN_SIZE)
#define SL6_BIN_COUNT ((112+SL6_BIN_SIZE-1)/SL6_BIN_SIZE)

static int BIN_SIZE[6] = {SL1_BIN_SIZE, SL2_BIN_SIZE, SL3_BIN_SIZE, SL4_BIN_SIZE, SL5_BIN_SIZE, SL6_BIN_SIZE};
static int BIN_COUNT[6] = {SL1_BIN_COUNT, SL2_BIN_COUNT, SL3_BIN_COUNT, SL4_BIN_COUNT, SL5_BIN_COUNT, SL6_BIN_COUNT};

//const char *fin_name = "/data/TracksDic_n20000000_pcalu.txt";
const char *fin_name = "/data/TracksDic_n20000000_newformat.txt";

FILE *fin;
int nlines = 0;
int nroads = 0;

typedef struct
{
  char w[6];
  char p, n;
  int ftof1b[2];
  int ftof2;
  int pcalu[3];
} road_t;

std::vector<road_t> roads[SL1_BIN_COUNT][SL2_BIN_COUNT][SL3_BIN_COUNT];

std::vector<road_t> road_dict;
std::vector<int> proad_dict;
std::vector<int> nroad_dict;
std::vector<int> ftof1broad_dict[62];
std::vector<int> ftof2road_dict[5];
std::vector<int> pcaluroad_dict[68];

void add_road(road_t new_road)
{
  // Scan road table for existing match 
  int len = roads[new_road.w[0]][new_road.w[1]][new_road.w[2]].size();
  int found = 0;
  for(int i=0;i<len;i++)
  {
    road_t r = roads[new_road.w[0]][new_road.w[1]][new_road.w[2]][i];
    if((r.w[3]!=new_road.w[3]) || (r.w[4]!=new_road.w[4]) || (r.w[5]!=new_road.w[5]))
      continue;

    found = 1;

    r.p |= new_road.p;
    r.n |= new_road.n;
    r.ftof1b[0] |= new_road.ftof1b[0];
    r.ftof1b[1] |= new_road.ftof1b[1];
    r.ftof2 |= new_road.ftof2;
    r.pcalu[0] |= new_road.pcalu[0];
    r.pcalu[1] |= new_road.pcalu[1];
    r.pcalu[2] |= new_road.pcalu[2];

    roads[new_road.w[0]][new_road.w[1]][new_road.w[2]][i] = r;
    break;
  }

  // Add to road table if road doesn't already exist
  if(!found)
  {
    roads[new_road.w[0]][new_road.w[1]][new_road.w[2]].push_back(new_road);
    nroads++;
  }
}

int read_road(road_t *r)
{
  int ret, w[36], charge, ftof1b = 0, ftof2 = 0, pcalu = 0;
  int iw[6], bit, idx;
  float mom, tet, phi, vz;
  char buf[1000];
 
  while(fgets(buf, sizeof(buf), fin))
  {
    if(buf[0] == '#')
    {
      printf("skip comment...\n");
      continue;
    }

    ret = sscanf(buf,"%d %f %f %f"\
                     " %d %d %d %d %d %d"\
                     " %d %d %d %d %d %d"\
                     " %d %d %d %d %d %d"\
                     " %d %d %d %d %d %d"\
                     " %d %d %d %d %d %d"\
                     " %d %d %d %d %d %d"\
                     " %d %f %d %d",
				 &charge,&mom,&tet,&phi,
				 &w[0],&w[1],&w[2],&w[3],&w[4],&w[5],&w[6],&w[7],&w[8],&w[9],&w[10],&w[11],
				 &w[12],&w[13],&w[14],&w[15],&w[16],&w[17],&w[18],&w[19],&w[20],&w[21],&w[22],&w[23],
				 &w[24],&w[25],&w[26],&w[27],&w[28],&w[29],&w[30],&w[31],&w[32],&w[33],&w[34],&w[35],
         &ftof1b, &vz,&ftof2, &pcalu);

    if(ret!=44 && ret!=42)
    {
      printf("skip invalid line (ret=%d)...\n", ret);
      continue;
    }

    if(w[0]<=0 || w[6]<=0 || w[12]<=0 || w[18]<=0 || w[24]<=0 || w[30]<=0)
      continue;

    if(!(nlines % 100000))
      printf("lines = %u, nroads = %u\n", nlines, nroads);

    for(int i=0; i<6; i++)
      r->w[i] = (w[i*6]-1) / BIN_SIZE[i];

    if(charge>0) r->p = 1;
    else         r->p = 0;

    if(charge<0) r->n = 1;
    else         r->n = 0;

    ftof1b--;
    idx = ftof1b/32;
    bit = ftof1b%32;
    r->ftof1b[0] = 0;
    r->ftof1b[1] = 0;
    if(ftof1b>=0)
      r->ftof1b[idx] = 1<<bit;

    ftof2--;
    r->ftof2 = 0;
    if(ftof2>=0)
      r->ftof2 = 1<<ftof2;

    pcalu--;
    idx = pcalu/32;
    bit = pcalu%32;
    r->pcalu[0] = 0;
    r->pcalu[1] = 0;
    r->pcalu[2] = 0;
    if(pcalu>=0)
      r->pcalu[idx] = 1<<bit;

    nlines++;

    return 1;
  }
  return 0;
}

void build_dictionaries()
{
  for(int sl1=0;sl1<SL1_BIN_COUNT;sl1++)
  for(int sl2=0;sl2<SL2_BIN_COUNT;sl2++)
  for(int sl3=0;sl3<SL3_BIN_COUNT;sl3++)
  {
    std::vector<road_t> road_vec = roads[sl1][sl2][sl3];
    int len = road_vec.size();

    for(int i=0;i<len;i++)
    {
      road_t r = road_vec[i];

      road_dict.push_back(r);
      int pos = road_dict.size()-1;

      if(r.p) proad_dict.push_back(pos);
      if(r.n) nroad_dict.push_back(pos);

      for(int j=0;j<62;j++)
      {
        int idx = j/32;
        int bit = j%32;
        if(r.ftof1b[idx] & (1<<bit))
          ftof1broad_dict[j].push_back(pos);
      }

      for(int j=0;j<5;j++)
      {
        if(r.ftof2 & (1<<j))
          ftof2road_dict[j].push_back(pos);
      }

      for(int j=0;j<68;j++)
      {
        int idx = j/32;
        int bit = j%32;
        if(r.pcalu[idx] & (1<<bit))
          pcaluroad_dict[j].push_back(pos);
      }
    }
  }

  printf("nroads = %d, npositives = %d, nnegatives = %d\n",
      road_dict.size(),
      nroad_dict.size(),
      proad_dict.size()
    );

  for(int j=0;j<62;j++)
  {
    printf("nftof1b%2d = %5d ", j, ftof1broad_dict[j].size());
    if(!(j % 4))
      printf("\n");
  }
  printf("\n");

  for(int j=0;j<5;j++)
  {
    printf("nftof2%2d = %5d ", j, ftof2road_dict[j].size());
    if(!(j % 4))
      printf("\n");
  }
  printf("\n");

  for(int j=0;j<68;j++)
  {
    printf("npcalu%2d = %5d ", j, pcaluroad_dict[j].size());
    if(!(j % 4))
      printf("\n");
  }
  printf("\n");
}

void write_roads_c()
{
  FILE *f;
  //
  // roaddict.h
  //
  f = fopen("roaddict.h","w");
  fprintf(f, "/* roaddict.h */\n\n");
  fprintf(f, "#define NROADS %d\n", road_dict.size());

  fprintf(f, "static int roaddict[NROADS][6] = {\n");
  for(int i=0;i<road_dict.size();i++)
  {
    fprintf(f,"    { %3d, %3d, %3d, %3d, %3d, %3d},\n",
        road_dict[i].w[0],road_dict[i].w[1],road_dict[i].w[2],
        road_dict[i].w[3],road_dict[i].w[4],road_dict[i].w[5]
      );
  }
  fprintf(f, "  };\n");
  fclose(f);

  //
  // roadchargedict.h
  //
  f = fopen("roadchargedict.h","w");
  fprintf(f, "/* roadchargedict.h */\n\n");
  fprintf(f, "#define NPOSITIVES %d\n", proad_dict.size());
  fprintf(f, "#define NNEGATIVES %d\n\n", nroad_dict.size());

  fprintf(f, "static int roadpositivedict[NPOSITIVES] = {\n");
  for(int i=0;i<proad_dict.size();i++)
    fprintf(f,"    %d,\n", proad_dict[i]);
  fprintf(f, "  };\n\n");

  fprintf(f, "static int roadnegativedict[NNEGATIVES] = {\n");
  for(int i=0;i<nroad_dict.size();i++)
    fprintf(f,"    %d,\n", nroad_dict[i]);
  fprintf(f, "  };\n");

  fclose(f);

  //
  // roadchargedict.h
  //
  f = fopen("roadftofdict.h","w");
  fprintf(f, "/* roadftofdict.h */\n\n");
  for(int i=0;i<62;i++)
    fprintf(f,"#define NFTOF%02d %4d\n",i,ftof1broad_dict[i].size());
  fprintf(f,"\n");

  fprintf(f,"static int nroadftof[] = {\n");
  for(int i=0;i<62;i++)
    fprintf(f,"  NFTOF%02d,\n",i);
  fprintf(f,"  };\n\n");

  for(int i=0;i<62;i++)
  {
    fprintf(f,"\n");
    fprintf(f,"static int roadftofdict%02d[NFTOF%02d] = {\n",i,i,ftof1broad_dict[i].size());
    for(int j=0;j<ftof1broad_dict[i].size();j++)
      fprintf(f,"    %d,\n", ftof1broad_dict[i][j]);
    fprintf(f,"  };\n");
  }

  fprintf(f, "\n");
  fprintf(f, "static int *roadftof[] = {\n");
  for(int i=0;i<62;i++)
    fprintf(f,"    roadftofdict%02d,\n",i);
  fprintf(f,"\n};\n");

  fclose(f);
}

void write_roads_hdl()
{
  FILE *f = fopen("roadfinder.vhd", "wt");
  const char *lut_mult5 = "E880800080000000";
  const char *lut_or    = "FFFFFFFFFFFFFFFE";

  fprintf(f,
      "library ieee;\n"\
      "use ieee.std_logic_1164.all;\n"\
      "use ieee.std_logic_misc.all;\n"\
      "\n"\
      "library utils;\n"\
      "use utils.utils_pkg.all;\n"\
      "\n"\
      "library unisim;\n"\
      "use unisim.vcomponents.all;\n"\
      "\n"\
      "entity roadfinder is\n"\
      "  port(\n"\
      "    SEGS_SL1         : in std_logic_vector(111 downto 0);\n"\
      "    SEGS_SL2         : in std_logic_vector(111 downto 0);\n"\
      "    SEGS_SL3         : in std_logic_vector(111 downto 0);\n"\
      "    SEGS_SL4         : in std_logic_vector(111 downto 0);\n"\
      "    SEGS_SL5         : in std_logic_vector(111 downto 0);\n"\
      "    SEGS_SL6         : in std_logic_vector(111 downto 0);\n"\
      "    ROAD_VALID       : out std_logic;\n"\
      "    P_ROAD_VALID     : out std_logic;\n"\
      "    N_ROAD_VALID     : out std_logic;\n"\
      "    FTOF_ROAD_VALID  : out std_logic_vector(61 downto 0)\n"\
      "  );\n"\
      "end roadfinder;\n"\
      "\n"\
      "architecture synthesis of roadfinder is\n"
    );
  for(int i=0;i<6;i++)
    fprintf(f, "  signal W%d     : std_logic_vector(%d downto 0);\n", i+1, BIN_COUNT[i]-1);

  fprintf(f, "  signal O_P_OR : std_logic;\n");
  fprintf(f, "  signal O_N_OR : std_logic;\n");
  
  fprintf(f,
      "  signal O     : std_logic_vector(0 to %d);\n", road_dict.size()-1
    );

  fprintf(f,
      "  signal O_P   : std_logic_vector(0 to %d);\n", (proad_dict.size()+5)/6-1
    );

  fprintf(f,
      "  signal O_N   : std_logic_vector(0 to %d);\n", (nroad_dict.size()+5)/6-1
    );

  for(int i=0;i<62;i++)
  {
    if(ftof1broad_dict[i].size() > 0)
    {
      fprintf(f,
        "  signal O_FTOF%-2d    : std_logic_vector(0 to %d);\n",i,(ftof1broad_dict[i].size()+5)/6-1
      );
    }
  }

  fprintf(f,
      "begin\n"\
      "\n"\
      "  ROAD_VALID   <= O_P_OR or O_N_OR;\n"\
      "  P_ROAD_VALID <= O_P_OR;\n"\
      "  N_ROAD_VALID <= O_N_OR;\n"\
      "\n"\
      "  O_P_OR <= or_reduce(O_P);\n"\
      "  O_N_OR <= or_reduce(O_N);\n"
    );

  for(int i=0;i<62;i++)
  {
    if(ftof1broad_dict[i].size() > 0)
    {
      fprintf(f,
        "  FTOF_ROAD_VALID(%d) <= or_reduce(O_FTOF%d);\n", i, i
      );
    }
    else
    {
      fprintf(f,
        "  FTOF_ROAD_VALID(%d) <= '0';\n", i
      );
    }
  }

  for(int i=0; i<6;i++)
  {
    for(int j=0;j<BIN_COUNT[i];j++)
    {
      int sl_max = BIN_SIZE[i]*j+BIN_SIZE[i]-1;
      int sl_min = BIN_SIZE[i]*j;
      if(sl_max > 111) sl_max = 111;
      fprintf(f,"  W%d(%d) <= or_reduce(SEGS_SL%d(%d downto %d));\n", i+1, j, i+1, sl_max, sl_min);
    }
  }

  fprintf(f,
      "\n"\
      "--------------------------------------------------------------------------------\n"\
      "-- ROAD table                                                                 --\n"\
      "--------------------------------------------------------------------------------\n"\
      "\n"
    );
  
  for(int i=0;i<road_dict.size();i++)
  {
    fprintf(f,
      "  LUT6_inst%d: LUT6 generic map(INIT=>x\"%s\") "\
      "port map(O=>O(%d),I0=>W1(%d),I1=>W2(%d),I2=>W3(%d),I3=>W4(%d),I4=>W5(%d),I5=>W6(%d));\n",
       i, lut_mult5, i,
       road_dict[i].w[0],road_dict[i].w[1],road_dict[i].w[2],
       road_dict[i].w[3],road_dict[i].w[4],road_dict[i].w[5]
    );
  }

  fprintf(f,
      "\n"\
      "\n"\
      "--------------------------------------------------------------------------------\n"\
      "-- Positive ROAD table                                                        --\n"\
      "--------------------------------------------------------------------------------\n"\
      "\n"
    );

  for(int i=0;i<(proad_dict.size()+5)/6;i++)
  {
    fprintf(f,
      "  LUT6_inst_p%d: LUT6 generic map(INIT=>x\"%s\") "\
      "port map(O=>O_P(%d)",
       i, lut_or, i);
    
    for(int j=0;j<6;j++)
    {
      if(i*6+j<proad_dict.size())
        fprintf(f, ",I%d=>O(%d)", j, proad_dict[i*6+j]);
      else
        fprintf(f, ",I%d=>'0'", j);
    }    
    fprintf(f, ");\n");
  }

  fprintf(f,
      "\n"\
      "\n"\
      "--------------------------------------------------------------------------------\n"\
      "-- Negative ROAD table                                                        --\n"\
      "--------------------------------------------------------------------------------\n"\
      "\n"
    );

  for(int i=0;i<(nroad_dict.size()+5)/6;i++)
  {
    fprintf(f,
      "  LUT6_inst_n%d: LUT6 generic map(INIT=>x\"%s\") "\
      "port map(O=>O_N(%d)",
       i, lut_or, i);
    
    for(int j=0;j<6;j++)
    {
      if(i*6+j<nroad_dict.size())
        fprintf(f, ",I%d=>O(%d)", j, nroad_dict[i*6+j]);
      else
        fprintf(f, ",I%d=>'0'", j);
    }    
    fprintf(f, ");\n");
  }

  fprintf(f,
      "\n"\
      "\n"\
      "--------------------------------------------------------------------------------\n"\
      "-- FTOF ROAD table                                                            --\n"\
      "--------------------------------------------------------------------------------\n"\
      "\n"
    );

  for(int i=0;i<62;i++)
  {
    fprintf(f,
      "  -- FTOF%d\n", i
      );

    for(int j=0;j<(ftof1broad_dict[i].size()+5)/6;j++)
    {
      fprintf(f,
        "  LUT6_inst_ftof%d_%d: LUT6 generic map(INIT=>x\"%s\") "\
        "port map(O=>O_FTOF%d(%d)",
         i, j, lut_or, i, j);
    
      for(int k=0;k<6;k++)
      {
        if(j*6+k<ftof1broad_dict[i].size())
          fprintf(f, ",I%d=>O(%d)", k, ftof1broad_dict[i][j*6+k]);
        else
          fprintf(f, ",I%d=>'0'", k);
      }    
      fprintf(f, ");\n");
    }
      fprintf(f, "\n");
  }

  fprintf(f, "\n");
  fprintf(f, "end synthesis;\n\n");
  
  fclose(f);
}

int main(int argc, char *argv[])
{
  road_t road;

  if(!(fin = fopen(fin_name, "r")))
  {
    printf("Failed to open: %s\n", fin_name);
    return -1;
  }

  while(read_road(&road))
    add_road(road);

  build_dictionaries();

  write_roads_hdl();
  write_roads_c();

  fclose(fin);
  return 0;
}

