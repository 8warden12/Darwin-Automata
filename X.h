#ifndef X_H
#define X_H

#include <vector>
#include "network.h"

#include "X.c"





using namespace std;

void forwardNeuronActivationTime(network &n);
void sumProcess(network &n);
void activationProcess(network &n);
void decayWeights(network &n);
void networkSynapticPlasticity(network &n);
void decayNetworkConstant(network *n, int decay);
void decayNetwork(network &n, float decay);
void process(network &n);
void dopamine(network &n);


void antidopamine(network &n);

struct gridbug;

gridbug newBug(int neurons, int inputNeurons, int outputNeurons, int patternGenNeurons);
gridbug newBug(network n);
void getInputsFromEnvironment(char world[10][10], gridbug &g);
void fillworld(char world[10][10]);
int runBug(gridbug &g, int cycles);
bool betterbug(gridbug g, gridbug h);
gridbug findBestBug(vector<gridbug> g);
void tempDisplay(const char world[10][10], gridbug &g);
void spawnFood(char world[10][10]);
vector<gridbug> mutateBug(gridbug n, const int offspringNum);

#endif
