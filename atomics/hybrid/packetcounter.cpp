#include "packetcounter.h"
void packetcounter::init(double t,...) {
for (int i=0;i<10;i++){
  y[i]=0;
};
Sigma=1e20;
total=0;
}
double packetcounter::ta(double t) {
return Sigma;
}
void packetcounter::dint(double t) {
Sigma=INF;
}
void packetcounter::dext(Event x, double t) {
total++;
y[0]=total;
Sigma=0;
}
Event packetcounter::lambda(double t) {
return Event(&y,0);
}
void packetcounter::exit() {

}
