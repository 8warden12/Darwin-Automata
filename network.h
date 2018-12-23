#ifndef NETWORK_H
#define NETWORK_H

#include <vector>
#include "network.c"


using namespace std;

#define DEFAULT_FIRING_THRESHOLD 1000
#define DEFAULT_CONNECTION_WEIGHT 100
#define WEIGHT_MAX 500
#define WEIGHT_MIN -200
#define SUM_MIN 0
#define THRESHOLD_MAX 2000
#define THRESHOLD_MIN 100

struct connection;
enum NEURON_TYPE;
struct neuron;
neuron *newNeuron();
struct network;
void newConnection(int emitterNeuronID,int receiverNeuronID, network &n);
void newConnection(int emitterNeuronID,int receiverNeuronID, network &n, int weight);
bool connectionExists(int neuron1ID, int neuron2ID, const network& n);
void deleteConnection(int emitterNeuronID, int receiverNeuronID, network &n);
void trimWeights(network &n);
network newNetwork(int neurons, int inputNeurons, int outputNeurons, int patternGenNeurons);
network newNetworkJustNeuronsWithRoles(int neurons,vector<int> roles);
void trimSum(network &n);
int inboundWeightSum(neuron *n);
int outboundWeightSum(neuron *n);
void insertInputVals(network &n, vector<int> inputVals);
void trimThreshold(network &n);
void displayNetworkConnectionData(const network &n);
void displayOnOffData(network &n);
void fancyFiringGraphicalDisplay(network &n);
network networkcpy(const network &source);
void saveNetwork(network n, const char* filename);
network loadNetwork(const char* filename);
void networkDelete(network &n);

#define MAX_MUTATION_NUM 20

#define CONNECTION_REARRANGEMENT_CHANCE -2
#define WEIGHT_MOD_CHANCE 2
#define THRESHOLD_MOD_CHANCE 2
#define NEW_CONNECTION_CHANCE -2
#define DELETE_CONNECTION_CHANCE -2

#define MAX_WEIGHT_MOD 100

#define MAX_THRESHOLD_MOD 100

#define NEW_NEURON_CHANCE 2
#define DELETE_NEURON_CHANCE 2 



/*#define MAX_MUTATION_NUM 20

#define CONNECTION_REARRANGEMENT_CHANCE 20
#define WEIGHT_MOD_CHANCE 5
#define THRESHOLD_MOD_CHANCE 5
#define NEW_CONNECTION_CHANCE 5
#define DELETE_CONNECTION_CHANCE 5

#define MAX_WEIGHT_MOD 100

#define MAX_THRESHOLD_MOD 100

#define NEW_NEURON_CHANCE 20
#define DELETE_NEURON_CHANCE 20 */

#endif
