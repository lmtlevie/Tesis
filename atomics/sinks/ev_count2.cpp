#include "ev_count2.h"
#include "pdevslib.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>

void ev_count2::init(double t,...) {
va_list parameters;
va_start(parameters, t);
FName=va_arg(parameters, char*);
char* n_str = va_arg(parameters, char*);
n = (int)std::atof(n_str);
for (int i=0;i<n;i++){
  k[i]=0;
};
emit_port=0;
Sigma=1e20;
}
double ev_count2::ta(double t) {
return Sigma;
}
void ev_count2::dint(double t) {
Sigma=1e20;
}
void ev_count2::dext(Event x, double t) {
k[x.port]=k[x.port]+1;
emit_port=x.port;
Sigma=0;
}
Event ev_count2::lambda(double t) {
return Event(&k[emit_port], emit_port);
}
void ev_count2::exit() {
FILE* FOutput = fopen(FName, "w");
for (int i=0;i<n;i++){
  fprintf(FOutput, "%g\n",k[i]);
  };
fclose(FOutput);

char DataName[256];
char ScriptName[256];
snprintf(DataName, sizeof(DataName), "%s.hist.dat", FName);
snprintf(ScriptName, sizeof(ScriptName), "%s.hist.plt", FName);

FILE* DataOutput = fopen(DataName, "w");
if (DataOutput) {
  for (int i=0;i<n;i++) fprintf(DataOutput, "\"%d\" %g\n", i, k[i]);
  fclose(DataOutput);
}

FILE* ScriptOutput = fopen(ScriptName, "w");
if (ScriptOutput) {
  fprintf(ScriptOutput, "set title 'Measurement histogram'\n");
  fprintf(ScriptOutput, "set xlabel 'measurement result'\n");
  fprintf(ScriptOutput, "set ylabel 'count'\n");
  fprintf(ScriptOutput, "set style data histograms\n");
  fprintf(ScriptOutput, "set style histogram clustered gap 1\n");
  fprintf(ScriptOutput, "set style fill solid 0.75 border -1\n");
  fprintf(ScriptOutput, "set boxwidth 0.6\n");
  fprintf(ScriptOutput, "set yrange [0:*]\n");
  fprintf(ScriptOutput, "plot '%s' using 2:xtic(1) title 'shots'\n", DataName);
  if (getOs()!=WINDOWS) fprintf(ScriptOutput, "pause -1\n");
  else fprintf(ScriptOutput, "pause 3600\n");
  fclose(ScriptOutput);
  if (getOs()!=WINDOWS) spawnProcess("/usr/bin/gnuplot", ScriptName);
  else {
    char buff[300];
    strcpy(buff, ScriptName);
    strcat(buff, " -");
    spawnProcess("../bin/gnuplot/bin/wgnuplot.exe", buff);
  }
}

//putScilabVar("ev(n)",k[0]);
}
