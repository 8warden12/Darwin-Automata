#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <deque>
using namespace std;

#define DEFAULT_FIRING_THRESHOLD 1000
#define DEFAULT_CONNECTION_WEIGHT 100
#define WEIGHT_MAX 500
#define WEIGHT_MIN -200
#define SUM_MIN 0
#define THRESHOLD_MAX 2000
#define THRESHOLD_MIN 100

void log(const char *thingtowrite)
{
	FILE *f = fopen("log.txt","a");
	fprintf(f,"%s\n",thingtowrite);
	fclose(f);
}

int OUTPUT_TO_LOG = 1;

//-1 == off
// 1 - activation and summation only
//
//




struct connection{//connections between neurons
	
	int weight = DEFAULT_CONNECTION_WEIGHT;//weight of the connection/synapse
	
	int receiverID = 0;
	int emitterID = 0;
	
	
};


enum NEURON_TYPE {
	NORMAL = 0,
	INPUT = 1,
	OUTPUT = 2,
	PATTERN_GENERATOR = 3,
};

struct neuron{// neurons
	
	vector<connection*> inboundConnections;//connections coming into the neuron
	vector<connection*> outboundConnections;//connections sent out by the neuron
	
	int neuronID;
	
	int sum = 0;//Sum of the signals coming into the neuron, neuron will activate if this breaches the threshhold
	int activation = 0;//1.0 if activated, 0.0 if not
	
	bool activated = false; //whether the neuron is activated or not
	
	int timeSinceActivation = -1;
	
	int fireCounter = 0;//number of times the neuron has fired since the last dopamine/antidopamine function
	
	int patternGenSignalDistance = 0;//time between 2 firings if the neuron is a pattern generator
	
	NEURON_TYPE type = NORMAL;
	
	bool used = true;
	
	int firingThreshold = DEFAULT_FIRING_THRESHOLD;
};

neuron *newNeuron();


neuron* newNeuron()
{
	neuron *n = new neuron;
	return n;
}

struct network{// brain networks
	vector<neuron*> inputNeurons;//neurons that receive external input
	vector<neuron*> outputNeurons;//neurons that directly affect ations made by the "organism"
	
		
	vector<neuron*> neurons;//all neurons in the network, regardless of role
		
	vector<int> IDtoIndex;
};

inline int neuronIDtoIndex(int ID, const network &n)
{
	/*bool found = false;
	for(int i = 0; i<n.neurons.size(); ++i)
	{
		if(n.neurons[i]->neuronID == ID)
		{
			return i;
		}
	}
	
	/*if(n.IDtoIndex[ID] == -1)
	{
		exit(0);
	}*/
	
	return ID;
}


void newConnection(int emitterNeuronID, int receiverNeuronID, network &n)// create a new connection between 2 neurons (referenced by their IDs)
{
	connection *c = new connection;
	c->emitterID = emitterNeuronID;
	c->receiverID = receiverNeuronID;
	
	n.neurons[neuronIDtoIndex(emitterNeuronID,n)]->outboundConnections.push_back(c); 
	n.neurons[neuronIDtoIndex(receiverNeuronID,n)]->inboundConnections.push_back(c);
		
}


void newConnection(int emitterNeuronID, int receiverNeuronID, network &n, int weight)// create a new connection between 2 neurons (referenced by their IDs)
{
	connection *c = new connection;
	c->emitterID = emitterNeuronID;
	c->receiverID = receiverNeuronID;
	c->weight = weight;
	
	n.neurons[neuronIDtoIndex(emitterNeuronID,n)]->outboundConnections.push_back(c); 
	n.neurons[neuronIDtoIndex(receiverNeuronID,n)]->inboundConnections.push_back(c);
}



bool connectionExists(int neuron1ID, int neuron2ID, const network& n)//for checking if a connection already exists
{
	for(int i = 0; i<n.neurons[neuronIDtoIndex(neuron1ID,n)]->outboundConnections.size(); ++i)//first checking outbound connections...
	{
		if(n.neurons[neuronIDtoIndex(neuron1ID,n)]->outboundConnections[i]->receiverID == neuron2ID)
		{
			return true;
		}
	}
	
	for(int i = 0; i<n.neurons[neuronIDtoIndex(neuron1ID,n)]->inboundConnections.size(); ++i)//...then inbound ones
	{
		if(n.neurons[neuronIDtoIndex(neuron1ID,n)]->inboundConnections[i]->emitterID == neuron2ID)
		{
			return true;
		}
	}
	
	return false;
}

void deleteConnection(int emitterNeuronID, int receiverNeuronID, network &n)
{
	bool done = false;
	int fe = 0;
	int se = 0;
	
	if(connectionExists(emitterNeuronID,receiverNeuronID,n))
	{
		for(int i = 0; i<n.neurons[neuronIDtoIndex(emitterNeuronID,n)]->outboundConnections.size(); ++i)
		{
			if(n.neurons[neuronIDtoIndex(emitterNeuronID,n)]->outboundConnections[i]->receiverID == receiverNeuronID)
			{
				fe = i;
				break;
			}
		}
		
		for(int i = 0; i<n.neurons[neuronIDtoIndex(receiverNeuronID,n)]->inboundConnections.size(); ++i)
		{
			if(n.neurons[neuronIDtoIndex(receiverNeuronID,n)]->inboundConnections[i]->emitterID == emitterNeuronID)
			{
				se = i;
				break;
			}
		}
		
		n.neurons[neuronIDtoIndex(emitterNeuronID,n)]->outboundConnections.erase(n.neurons[neuronIDtoIndex(emitterNeuronID,n)]->outboundConnections.begin() + fe);
		
		
		
		//TIL: erase will destroy pointers, so only one of these erases are needed.
		
		n.neurons[neuronIDtoIndex(receiverNeuronID,n)]->inboundConnections.erase(n.neurons[neuronIDtoIndex(receiverNeuronID,n)]->inboundConnections.begin() + se);
	}
	
}



void trimWeights(network &n)
{
	for(int i = 0; i<n.neurons.size(); ++i)
	{
		for(int x = 0; x<n.neurons[i]->outboundConnections.size(); ++x)
		{
			if(n.neurons[i]->outboundConnections[x]->weight > WEIGHT_MAX)
			{
				n.neurons[i]->outboundConnections[x]->weight = WEIGHT_MAX;
			}else if(n.neurons[i]->outboundConnections[x]->weight < WEIGHT_MIN)
			{
				n.neurons[i]->outboundConnections[x]->weight = WEIGHT_MIN;
			}
		}
	}
}

network newNetwork(int neurons, int inputNeurons, int outputNeurons, int patternGenNeurons)//returns a pointer to the network
{
	/*if(inputNeurons + outputNeurons + patternGenNeurons > neurons)
	{
		return;
	}*/
	
	
	
	network n;
	
	for(int i = 0; i<neurons; ++i)
	{
		n.IDtoIndex.push_back(0);
	}
	
	for(int i = 0; i<neurons; ++i)//put neurons in the network
	{
		n.neurons.push_back(newNeuron());
		n.neurons[i]->neuronID = i;
		n.IDtoIndex[i] = i;
	}
	
	for(int i = 0; i<inputNeurons; ++i)
	{
		n.inputNeurons.push_back(n.neurons[i]);//the first neurons become inputNeurons
		n.neurons[i]->type = INPUT;
	}
	
	for(int i = 0; i<outputNeurons; ++i)
	{
		n.outputNeurons.push_back(n.neurons[neurons-(i+1)]);//the last neurons become outputNeurons
		n.neurons[neurons-(i+1)]->type = OUTPUT;
	}
	
	for(int i = 0, i2 = 0; i<n.neurons.size(), i2<patternGenNeurons; ++i)
	{
		if(n.neurons[i]->type == NORMAL)
		{
			n.neurons[i]->type = PATTERN_GENERATOR;
			if(++i2 >= patternGenNeurons)
			{
				break;
			}
		}
	}
	
	deque<int> nottaken;
	
	int connections = 0;//connections that a neuron will have
	int inboundConnectionNum = 0;
	int outboundConnectionNum = 0;
	
	int otherNeuronID = 0;//ID of the other neuron
	
	for(int i = 0; i<n.neurons.size(); ++i)
	{
		nottaken.clear();
		connections = rand() % (neurons - neurons/2) + neurons/3;//max connections = n/2 NOTE in later versions, connection pruning will be introduced, and this will be uncessesaey

		for(int x = 0; x<n.neurons.size(); ++x)
		{
			if(!connectionExists(i,x,n) && i != x)
			nottaken.push_back(x);
		}
		
		random_shuffle(nottaken.begin(),nottaken.end());
		
		inboundConnectionNum = (nottaken.size()-1)/2;
		outboundConnectionNum = (nottaken.size()-1)/2;
		
		if(n.neurons[i]->type == INPUT)
		{
			inboundConnectionNum = 0;
		}
		
		while(n.neurons[i]->outboundConnections.size() < outboundConnectionNum)
		{
			newConnection(i,nottaken[0],n);
			nottaken.pop_front();
		}
	
		while(n.neurons[i]->inboundConnections.size() < inboundConnectionNum)
		{
			newConnection(nottaken[0],i,n);
			nottaken.pop_front();			
		}
		
	}
	
	for(int i = 0; i<n.inputNeurons.size(); ++i)
	{
		for(int x = 0; x<n.inputNeurons[i]->inboundConnections.size(); ++x)
		{
			deleteConnection(n.inputNeurons[i]->inboundConnections[x]->emitterID,n.inputNeurons[i]->inboundConnections[x]->receiverID,n);
		}
	}
	

	return n;

}




network newNetworkJustNeuronsWithRoles(int neurons,vector<int> roles)//returns a pointer to the network
{
	network n;
	
	for(int i = 0; i<neurons; ++i)//put neurons in the network
	{
		n.IDtoIndex.push_back(0);
	}
	
	for(int i = 0; i<neurons; ++i)//put neurons in the network
	{
		n.neurons.push_back(newNeuron());
		n.neurons[i]->type = (NEURON_TYPE)roles[i];
		n.neurons[i]->neuronID = i;
		n.IDtoIndex[i] = i;
	}
	
	return n;
	
}

//if sum exceeds a limit, it will go back to a limit
void trimSum(network &n)//chinese food??
{
	for(int i = 0; i<n.neurons.size(); ++i)
	{
		if(n.neurons[i]->sum > DEFAULT_FIRING_THRESHOLD)
		{
			n.neurons[i]->sum = DEFAULT_FIRING_THRESHOLD;
		}
		
		if(n.neurons[i]->sum < SUM_MIN)
		{
			n.neurons[i]->sum = SUM_MIN;
		}
	}
}


int inboundWeightSum(neuron *n)
{
	int s = 0;
	
	for(int i = 0; i<n->inboundConnections.size(); ++i)
	{
		s += n->inboundConnections[i]->weight;
	}
	
	return s;
}

int outboundWeightSum(neuron *n)
{
	int s = 0;
	
	for(int i = 0; i<n->outboundConnections.size(); ++i)
	{
		s += n->outboundConnections[i]->weight;
	}
	
	return s;
}


/*void insertInputVals(network *n, vector<int> inputVals)
{
	if(inputVals.size() != n->inputNeurons.size())//exit if input size differs
	{//printf("sizedifferes");
		return;
	}
	
	for(int i = 0; i<n->inputNeurons.size(); ++i)//assigning activations from inputVals
	{
		n->inputNeurons[i]->activated = inputVals[i];
		n->inputNeurons[i]->activation = inputVals[i];//bool to float
		
		if(n->inputNeurons[i]->activated && n->inputNeurons[i]->timeSinceActivation == -1)
		{
			n->inputNeurons[i]->timeSinceActivation = 0;
		}
		
//		n->inputNeurons[i]->sum = inputVals[i];
	}
}*/

void insertInputVals(network &n, vector<int> inputVals)
{
	if(inputVals.size() != n.inputNeurons.size())//exit if input size differs
	{//printf("sizedifferes");
		return;
	}
	for(int i = 0; i<n.inputNeurons.size(); ++i)//assigning activations from inputVals
	{
		n.inputNeurons[i]->activated = inputVals[i];
		n.inputNeurons[i]->activation = inputVals[i];//bool to float
		
		if(n.inputNeurons[i]->activated && n.inputNeurons[i]->timeSinceActivation == -1)
		{
			n.inputNeurons[i]->timeSinceActivation = 0;
		}
		
//		n->inputNeurons[i]->sum = inputVals[i];
	}
}

void trimThreshold(network &n)
{
	for(int i = 0; i<n.neurons.size(); ++i)
	{
		if(n.neurons[i]->firingThreshold > THRESHOLD_MAX)
		{
			n.neurons[i]->firingThreshold = THRESHOLD_MAX;
		}else if(n.neurons[i]->firingThreshold < THRESHOLD_MIN)
		{
			n.neurons[i]->firingThreshold = THRESHOLD_MIN;
		}
	}
}

void displayNetworkConnectionData(const network &n)//for debugging, display neuron connection values, etc.
{
	for(int i = 0; i<n.neurons.size(); ++i)
	{
		printf("Neuron at index %d, ID %d\n",i,n.neurons[i]->neuronID);
		
		printf("Inbound connections: \n");
		for(int x = 0; x<n.neurons[i]->inboundConnections.size(); ++x)
		{
			printf("From %d, weight %d\n",n.neurons[i]->inboundConnections[x]->emitterID,n.neurons[i]->inboundConnections[x]->weight);
		}
		
		printf("\nOutbound connections: \n");
		for(int x = 0; x<n.neurons[i]->outboundConnections.size(); ++x)
		{
			printf("To %d, weight %d\n",n.neurons[i]->outboundConnections[x]->receiverID,n.neurons[i]->outboundConnections[x]->weight);
		}
		printf("\n\n");
	}
}

void displayOnOffData(network &n)
{
	for(int i = 0; i<n.neurons.size(); ++i)
	{
		printf("neuron index %d, ID %d",i,n.neurons[i]->neuronID);
		printf("  Sum: %d TSA: %d Activation: %d Threshold: %d FireCounter: %d\n",n.neurons[i]->sum,n.neurons[i]->timeSinceActivation, n.neurons[i]->activation,n.neurons[i]->firingThreshold,n.neurons[i]->fireCounter);
	}
}

void fancyFiringGraphicalDisplay(network &n)
{
	for(int i = 0; i<n.neurons.size(); ++i)
	{
		if(n.neurons[i]->activated)
		{
			printf("1");
		}else{
			printf("0");
		}
	}
	printf("\n");
	
	for(int i = 0; i<n.neurons.size(); ++i)
	{
		printf("%d",n.neurons[i]->type);
	}
	
}

void insertNeuron(network &n)
{
	neuron *o = new neuron;
	n.neurons.push_back(o);
	
	o->neuronID = 0;
		
	for(int i = 0; i<n.neurons.size(); ++i)
	{
		if(o->neuronID < n.neurons[i]->neuronID)
		{
			o->neuronID = n.neurons[i]->neuronID;
		}
	}
	++o->neuronID;
	printf("ID: %d\n",o->neuronID);
	deque<int> nottaken;
	
	
	
	int inb = (nottaken.size()-1)/2;
	int outb = (nottaken.size()-1)/2;
	
	for(int i = 0; i<n.neurons.size()-1; ++i)
	{
		nottaken.push_back(n.neurons[i]->neuronID);
	}
	
	for(int i = 0; i<inb; ++i)
	{
		newConnection(nottaken[0],o->neuronID,n);
		nottaken.pop_front();
	}
	
	for(int i = 0; i<outb; ++i)
	{
		newConnection(o->neuronID,nottaken[0],n);
		nottaken.pop_front();
	}
	
	
	
	
}

void deleteNeuron(int neuronID, network &n)
{
	int c = 0;
	redux:
	
	while(n.neurons[neuronIDtoIndex(neuronID,n)]->outboundConnections.size() != 0)
	{
		c = n.neurons[neuronIDtoIndex(neuronID,n)]->outboundConnections.size();
		for(int i = 0; i<c; ++i)
		{
			deleteConnection(neuronID,n.neurons[neuronIDtoIndex(neuronID,n)]->outboundConnections[i]->receiverID,n);
			--c;
		}
	}
	
	while(n.neurons[neuronIDtoIndex(neuronID,n)]->inboundConnections.size() != 0)
	{
		c = n.neurons[neuronIDtoIndex(neuronID,n)]->inboundConnections.size();
		for(int i = 0; i<c; ++i)
		{
			deleteConnection(n.neurons[neuronIDtoIndex(neuronID,n)]->inboundConnections[i]->emitterID,neuronID,n);
			--c;
		}
	}
	
	
	
	n.neurons[neuronID]->used = false;
	
	/*n.IDtoIndex.pop_back();
	
	for(int i = 0; i<n.neurons.size(); ++i)
	{
		n.IDtoIndex[n.neurons[i]->neuronID] = i;		
	}*/
	
}




void saveNetwork(network n, const char* filename)
{
	FILE *f = fopen(filename,"w");

	fprintf(f,"%d %d %d\n",n.inputNeurons.size(),n.outputNeurons.size(),n.neurons.size());
	for(int i = 0; i<n.neurons.size(); ++i)
	{
		fprintf(f,"%d ",n.neurons[i]->type);
	}
	
	fprintf(f,"\n");
	
	for(int i = 0; i<n.neurons.size(); ++i)
	{
		fprintf(f,"%d ",n.neurons[i]->neuronID);
	}
	
	fprintf(f,"\n");
	
	for(int i = 0; i<n.neurons.size(); ++i)
	{
		fprintf(f,"%d %d %d %d %d %d\n",i,n.neurons[i]->neuronID,n.neurons[i]->type,n.neurons[i]->inboundConnections.size(),n.neurons[i]->outboundConnections.size(),n.neurons[i]->firingThreshold);	
		
		for(int x = 0; x<n.neurons[i]->inboundConnections.size(); ++x)
		{
			fprintf(f,"%d %d;",n.neurons[i]->inboundConnections[x]->emitterID,n.neurons[i]->inboundConnections[x]->weight);
		}
		
		fprintf(f,"\n");
		
		for(int x = 0; x<n.neurons[i]->outboundConnections.size(); ++x)
		{
			fprintf(f,"%d %d;",n.neurons[i]->outboundConnections[x]->receiverID,n.neurons[i]->outboundConnections[x]->weight);
		}
		
		fprintf(f,"\n");
		
	}
	
	fclose(f);
	

	
}

network loadNetwork(const char* filename)
{
	FILE *f = fopen(filename,"r");
	
	int tempchar = ' ';
	
	int inputNeuronNum = 0;
	int outputNeuronNum = 0;
	int totalNeurons = 0;
	
	int inboundConnectionNum = 0;
	int outboundConnectionNum = 0;
	int neuronThreshold = 0;
	int otherNeuronIndex = 0;
	
	int otherNeuronID = 0;
	
	int tempInt = 0;
	
	vector<int> typelist;	
	
	fscanf(f,"%d",&inputNeuronNum);
	fscanf(f,"%d",&outputNeuronNum);
	fscanf(f,"%d",&totalNeurons);
	
	for(int i = 0; i<totalNeurons; ++i)
	{
		fscanf(f,"%d",&tempInt);
		typelist.push_back(tempInt);
	}
	
	
	
	network n = newNetworkJustNeuronsWithRoles(totalNeurons,typelist);
	
	
	for(int i = 0; i<totalNeurons; ++i)
	{
		fscanf(f,"%d",&tempInt);
		typelist.push_back(tempInt);
	}
	
	for(int i = 0; i<n.neurons.size(); ++i)
	{
		n.neurons[i]->neuronID = typelist[i];
		n.IDtoIndex.push_back(typelist[i]);
	}
	
	for(int i = 0; i<totalNeurons; ++i)
	{
		//fprintf(f,"%d %d %d %d %d\n",i,n->neurons[i]->type,n->neurons[i]->inboundConnections.size(),n->neurons[i]->outboundConnections.size(),n->neurons[i]->firingThreshold);	
		
		if(n.neurons[i]->type == INPUT)
		{
			n.inputNeurons.push_back(n.neurons[i]);
		}else if(n.neurons[i]->type == OUTPUT)
		{
			n.outputNeurons.push_back(n.neurons[i]);
		}
		
		fscanf(f,"%d",&tempInt);
		fscanf(f,"%d",&otherNeuronID);
		fscanf(f,"%d",&tempInt);
		fscanf(f,"%d",&inboundConnectionNum);
		fscanf(f,"%d",&outboundConnectionNum);
		fscanf(f,"%d",&neuronThreshold);
		
		n.neurons[i]->firingThreshold = neuronThreshold;
		
		
		
		for(int x = 0; x<inboundConnectionNum; ++x)
		{
		
			fscanf(f,"%d",&otherNeuronIndex);
			fscanf(f,"%d",&tempInt);//weight
			//fprintf(f,"%d %d;",n->neurons[i]->inboundConnections[x]->emitterIndex,n->neurons[i]->inboundConnections[x]->weight);
			
			if(!connectionExists(n.neurons[i]->neuronID,n.neurons[otherNeuronIndex]->neuronID,n))
			{
				newConnection(n.neurons[otherNeuronIndex]->neuronID,n.neurons[i]->neuronID,n,tempInt);
			}
			
			
			
			fscanf(f,"%c",&tempchar);
		}
		
		
		for(int x = 0; x<outboundConnectionNum; ++x)
		{
			
			fscanf(f,"%d",&otherNeuronIndex);
			fscanf(f,"%d",&tempInt);//weight
			//fprintf(f,"%d %d;",n->neurons[i]->inboundConnections[x]->emitterIndex,n->neurons[i]->inboundConnections[x]->weight);
		//	printf("W1\n");
			if(!connectionExists(n.neurons[i]->neuronID,n.neurons[otherNeuronIndex]->neuronID,n))
			{
				newConnection(n.neurons[i]->neuronID,n.neurons[otherNeuronIndex]->neuronID,n,tempInt);
			}
			//printf("W2\n");
			
			fscanf(f,"%c",&tempchar);
			
		}
		
					
	}
	fclose(f);
	
	
	return n;
}

network networkcpy(const network &source)
{
	network n;
	
	
	for(int i = 0; i<source.neurons.size(); ++i)
	{
		n.neurons.push_back(newNeuron());
		n.neurons[i]->neuronID = source.neurons[i]->neuronID;
		
		if(source.neurons[i]->outboundConnections.size() == 0 && source.neurons[i]->inboundConnections.size() == 0)
		{
			n.neurons[i]->used = false;
		}
		
	}
		
	
	
	for(int i = 0; i<source.neurons.size(); ++i)
	{
		
		n.neurons[i]->type = source.neurons[i]->type;
		if(n.neurons[i]->type == INPUT)
			n.inputNeurons.push_back(n.neurons[i]);
			
		if(n.neurons[i]->type == OUTPUT)
			n.outputNeurons.push_back(n.neurons[i]);
		
			
		n.neurons[i]->firingThreshold = source.neurons[i]->firingThreshold;
		
		
		for(int x = 0; x<source.neurons[i]->outboundConnections.size(); ++x)
			if(!connectionExists(source.neurons[i]->neuronID,source.neurons[i]->outboundConnections[x]->receiverID,n))
				newConnection(source.neurons[i]->neuronID,source.neurons[i]->outboundConnections[x]->receiverID,n,source.neurons[i]->outboundConnections[x]->weight);
				
		for(int x = 0; x<source.neurons[i]->inboundConnections.size(); ++x)
			if(!connectionExists(source.neurons[i]->neuronID,source.neurons[i]->inboundConnections[x]->emitterID,n))
				newConnection(source.neurons[i]->inboundConnections[x]->emitterID,source.neurons[i]->neuronID,n,source.neurons[i]->inboundConnections[x]->weight);		
	
	}
	
	return n;
	
}


void networkDelete(network &n)
{
	int d = n.neurons.size();
	
	int is = 0;
	int os = 0;
	for(int i = 0; i<d; ++i)
	{
		os = n.neurons[i]->outboundConnections.size();
		
		
		while(n.neurons[i]->outboundConnections.size() != 0)
		{
			for(int x = 0; x<os; ++x)
			{
				deleteConnection(n.neurons[i]->neuronID,n.neurons[i]->outboundConnections[x]->receiverID,n);
				--os;
			}
		}
		
		os = n.neurons[i]->inboundConnections.size();
		
		while(n.neurons[i]->inboundConnections.size() != 0)
		{
			for(int x = 0; x<os; ++x)
			{
				deleteConnection(n.neurons[i]->inboundConnections[x]->emitterID,n.neurons[i]->neuronID,n);
				--os;
			}
		}
	}
	
	
	
	for(int i = 0; i<d; ++i)
	{
		n.neurons[i]->inboundConnections.clear();
    	n.neurons[i]->outboundConnections.clear();
		delete n.neurons[i];
		n.neurons[i] = NULL;
	}
	
	n.neurons.clear();
}
