#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <time.h>
#include <algorithm>
#include <string>
#include "network.h"
#include <deque>

#define INBOUND_WEIGHT_SUM_MIN -1

int RANDOM_ACTIVATION = 1;

using namespace std;



void forwardNeuronActivationTime(network &n)
{
	for(int i = 0; i<n.neurons.size(); ++i)
	{
		if(n.neurons[i]->timeSinceActivation > -1)
		{
			n.neurons[i]->timeSinceActivation += 1;
		}
	}
}


void sumProcess(network &n)
{
	neuron *tpNeuron = NULL;
	for(int i = 0; i<n.neurons.size(); ++i)
	{
		if(n.neurons[i]->activated)
		{
			tpNeuron = n.neurons[i];
			for(int x = 0; x<tpNeuron->outboundConnections.size(); ++x)
			{
				n.neurons[neuronIDtoIndex(tpNeuron->outboundConnections[x]->receiverID,n)]->sum += tpNeuron->activation * tpNeuron->outboundConnections[x]->weight;
			}
			
			tpNeuron->activated = false;
			tpNeuron->sum = 0;
			tpNeuron->activation = 0;
			tpNeuron = NULL;
		}
	}	
}

void activationProcess(network &n)
{
	for(int i = 0; i<n.neurons.size(); ++i)
	{
		if(RANDOM_ACTIVATION && n.neurons[i]->type != INPUT)
		{
			if(rand()%n.neurons[i]->firingThreshold < n.neurons[i]->sum)
			{
				n.neurons[i]->timeSinceActivation = 0;//set timeSinceActivation = 0;
				n.neurons[i]->activated = true;
				++n.neurons[i]->fireCounter;
				n.neurons[i]->activation = 1;
			}
		}else{
			if(n.neurons[i]->firingThreshold < n.neurons[i]->sum)
			{
				n.neurons[i]->timeSinceActivation = 0;//set timeSinceActivation = 0;
				n.neurons[i]->activated = true;
				n.neurons[i]->activation = 1;
				++n.neurons[i]->fireCounter;
			}
		}
	}
}



void decayWeights(network &n)
{
	for(int i = 0; i<n.neurons.size(); ++i)
	{
		for(int x = 0; x<n.neurons[i]->outboundConnections.size(); ++x)
		{
			if((n.neurons[i]->type == INPUT || outboundWeightSum(n.neurons[i]) < 0) || outboundWeightSum(n.neurons[neuronIDtoIndex(n.neurons[i]->outboundConnections[x]->receiverID,n)]) < 0)
			{
				break;
			}
			n.neurons[i]->outboundConnections[x]->weight *= 0.99;
		}
		for(int x = 0; x<n.neurons[i]->inboundConnections.size(); ++x)
		{
			if((n.neurons[i]->type == INPUT || inboundWeightSum(n.neurons[i]) < 0) || inboundWeightSum(n.neurons[neuronIDtoIndex(n.neurons[i]->inboundConnections[x]->emitterID,n)]) < 0)
			{
				break;
			}
			//n.neurons[i]->inboundConnections[x]->weight *= 0.95;
		}
	}
}



void networkSynapticPlasticity(network &n)
{
	neuron *activatedNeuron = NULL;
	for(int i = 0; i<n.neurons.size(); ++i)
	{
		if(n.neurons[i]->activated && n.neurons[i]->type != INPUT)
		{
			activatedNeuron = n.neurons[i];
			for(int x = 0; x<activatedNeuron->inboundConnections.size(); ++x)
			{
				switch(n.neurons[neuronIDtoIndex(activatedNeuron->inboundConnections[x]->emitterID,n)]->timeSinceActivation)
				{
					case 1:
						if(!activatedNeuron->inboundConnections[x]->weight + 16 > WEIGHT_MAX)
						activatedNeuron->inboundConnections[x]->weight += 16;
					break;
					
					case 2:
						if(!activatedNeuron->inboundConnections[x]->weight + 8 > WEIGHT_MAX)
						activatedNeuron->inboundConnections[x]->weight += 8;
					break;
					
					case 3:
						if(!activatedNeuron->inboundConnections[x]->weight + 4 > WEIGHT_MAX)
						activatedNeuron->inboundConnections[x]->weight += 4;
					break;
				}
			}
			
			for(int x = 0; x<activatedNeuron->outboundConnections.size(); ++x)
			{
				switch(n.neurons[neuronIDtoIndex(activatedNeuron->outboundConnections[x]->receiverID,n)]->timeSinceActivation)
				{
					case 1:
						if(!activatedNeuron->inboundConnections[x]->weight - 16 > WEIGHT_MAX)
						activatedNeuron->outboundConnections[x]->weight -= 16;
					break;
					
					case 2:
						if(!activatedNeuron->inboundConnections[x]->weight - 8 > WEIGHT_MAX)
						activatedNeuron->outboundConnections[x]->weight -= 8;
					break;
					
					case 3:
						if(!activatedNeuron->inboundConnections[x]->weight - 4 > WEIGHT_MAX)
						activatedNeuron->outboundConnections[x]->weight -= 4;
					break;
				}
			}
		}
		activatedNeuron = NULL;	
	}
}

void networkSynapticPlasticityFireCounter(network &n)
{
	
	for(int i = 0; i<n.neurons.size(); ++i)
	{
		
	}
	
	
}


void decayNetworkConstant(network *n, int decay)
{
	for(int i = 0; i<n->neurons.size(); ++i)
	{
		n->neurons[i]->sum =- decay;
	}
}

void decayNetwork(network &n, float decay)
{
	for(int i = 0; i<n.neurons.size(); ++i)
	{
		n.neurons[i]->sum *= decay;
	}
}

void process(network &n)//grrr
{
	forwardNeuronActivationTime(n);
	trimSum(n);
	decayNetwork(n,0.8);	
	sumProcess(n);
	activationProcess(n);
	trimSum(n);
}

void dopamine(network &n)
{
	for(int i = 0; i<n.neurons.size(); ++i)
	{
		if(n.neurons[i]->type != OUTPUT)
		{
			if(!(n.neurons[i]->firingThreshold - n.neurons[i]->fireCounter * 10 < THRESHOLD_MIN))
			{
				n.neurons[i]->firingThreshold -= n.neurons[i]->fireCounter * 10; 
			}
		}
		n.neurons[i]->fireCounter = 0;
	}
}

void antidopamine(network &n)
{
	for(int i = 0; i<n.neurons.size(); ++i)
	{
		if(n.neurons[i]->type != OUTPUT)
		{
			if(!(n.neurons[i]->firingThreshold + n.neurons[i]->fireCounter * 10 > THRESHOLD_MAX))
			{
				n.neurons[i]->firingThreshold += n.neurons[i]->fireCounter * 10; 
			}
		}
		n.neurons[i]->fireCounter = 0;
	}
}


struct gridbug{
	int gridbugX = 4;
	int gridbugY = 4;	
	int eaten = 0;
	int hunger = 0;
	int timeAlive = 0;
	int generation = 0;
	int colorID = 0;
	string speciesID;
		
	network brain;//the best part

	//gridbug(const gridbug& other);
	
	/*gridbug& operator=(const gridbug &other){
		this->gridbugX = 4;
		this->gridbugY = 4;
		this->eaten = 0;
		this->hunger = 0;
		
		this->brain = other.brain;
		
		return *this;
		
	}*/

};


gridbug newBug(int neurons, int inputNeurons, int outputNeurons, int patternGenNeurons)
{
	gridbug g;
	g.brain = newNetwork(neurons,inputNeurons,outputNeurons,patternGenNeurons);
	
	
	return g;
}

gridbug newBug(const network n)
{
	gridbug g;
	g.brain = networkcpy(n);
	return g;
}

char tempArena[10][10] = {
	{' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
	{' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},	
	{' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},	
	{' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},	
	{' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},	
	{' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},	
	{' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},	
	{' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},	
	{' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},	
	{' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},	
};

void getInputsFromEnvironment(char world[10][10], gridbug &g)
{
	vector<int> tempInputs;
	
	if(world[g.gridbugY-1][g.gridbugX] == ' ')
	{
		tempInputs.push_back(1);
	}else if(world[g.gridbugY-1][g.gridbugX] == ','){
		tempInputs.push_back(8);
	}else{
		tempInputs.push_back(1);
	}
	
	if(world[g.gridbugY][g.gridbugX-1] == ' ')
	{
		tempInputs.push_back(1);
	}else if(world[g.gridbugY][g.gridbugX-1] == ','){
		tempInputs.push_back(8);
	}else{
		tempInputs.push_back(1);
	}
	
	if(world[g.gridbugY+1][g.gridbugX] == ' ')
	{
		tempInputs.push_back(1);
	}else if(world[g.gridbugY+1][g.gridbugX] == ','){
		tempInputs.push_back(8);
	}else{
		tempInputs.push_back(1);
	}
	
	if(world[g.gridbugY][g.gridbugX+1] == ' ')
	{
		tempInputs.push_back(1);
	}else if(world[g.gridbugY][g.gridbugX+1] == ','){
		tempInputs.push_back(8);
	}else{
		tempInputs.push_back(1);
	}
	
	insertInputVals(g.brain,tempInputs);
}


void fillWorld(char world[10][10])
{
	for(int i = 0; i<10; ++i)
	{
		for(int x = 0; x<10; ++x)
		{
			world[i][x] = ',';
		}
	}
}

int runBug(gridbug &g, int cycles)
{
	g.gridbugX = 4;
	g.gridbugY = 4;
	int eaten = 0;
	
	char world[10][10];
	
	for(int i = 0; i<10; ++i)
	for(int x = 0; x<10; ++x)
	world[i][x] = ',';
	
	 
	for(int i = 0; i<cycles; ++i)
	{
		
		//spawnFoodTemp();
		getInputsFromEnvironment(world,g);
		
	
		process(g.brain);		
		
		if(g.brain.outputNeurons[0]->activated)
		{
			if(g.gridbugY > 0)
			{
				g.gridbugY--;		
			}
		}
		if(g.brain.outputNeurons[1]->activated)
		{
			if(g.gridbugY < 9)
			{
				g.gridbugY++;		
			}
		}
		
		if(g.brain.outputNeurons[2]->activated)
		{
			if(g.gridbugX > 0)
			{
				g.gridbugX--;		
			}
		}
		
		if(g.brain.outputNeurons[3]->activated)
		{
			if(g.gridbugX < 9)
			{
				g.gridbugX++;		
			}
		}
		 
		if(world[g.gridbugY][g.gridbugX] == ',')
		{
			world[g.gridbugY][g.gridbugX] = ' ';
			++eaten;
		}
	}
	return eaten;
}

bool betterbug(gridbug g, gridbug h)
{
	if(g.eaten > h.eaten)
		return true;
	else
		return false;
}

#define TIMES_TO_RUN 20
#define STEPS_TO_RUN 1000


gridbug findBestBug(vector<gridbug> g)
{
	vector<int> totalNetworkEaten;
	for(int i = 0; i<g.size(); ++i)
	{
		totalNetworkEaten.push_back(0);
	}
	
	for(int i = 0; i<g.size(); ++i)
	{
		//for(int x = 0; x<TIMES_TO_RUN; ++x)
		//totalNetworkEaten[i] += runBug(g[i],STEPS_TO_RUN);
		
		//g[i].eaten = totalNetworkEaten[i]/TIMES_TO_RUN;
		
		g[i].eaten = runBug(g[i],STEPS_TO_RUN);
	}
	
	
	
	int highestEaten = 0, highIndex = 0;
	
	for(int i = 0; i<g.size(); ++i)
	if(g[i].eaten > highestEaten)
	{
		highestEaten = g[i].eaten;
		highIndex = i;
	}
	
	
//	gridbug best;
//	best.brain = networkcpy(g[highIndex].brain);
	for(int i = 0; i<g.size(); ++i)
	//networkDelete(g[i].brain);
//	g.clear();
	
	
	return g[highIndex];	
	
	
}


void tempDisplay(const char world[10][10], gridbug &g)
{
	for(int i = 0; i<10; ++i)
	{
		for(int x = 0; x<10; ++x)
		{
			if(i == g.gridbugY && x == g.gridbugX)
			{
				printf("X");
			}else{
				printf("%c",world[i][x]);
			}
		}
		
		printf("\n");
	}
	
	printf("Eaten: %d\nHunger: %d\n",g.eaten,g.hunger);
}

void spawnFood(char world[10][10])
{
	for(int i = 0; i<10; ++i)
	{
		for(int x = 0; x<10; ++x)
		{
			if(rand()%100 == 1)
			{
				world[i][x] = ',';
			}
		}
	}
}


vector<gridbug> mutateBug(gridbug n, const int offspringNum)
{
	vector<network> offspring;
	
	int mutationNum = 0;
	int onn = 0;
	
	
	for(int i = 0; i<offspringNum; ++i)
	{	
		network mod = networkcpy(n.brain);
		
		
		nra:
		if(rand()%NEW_NEURON_CHANCE == 1)
		{
			log("NEW");
			neuron *nu = newNeuron();
			
			int maxID = 0;
			
		/*	
			
			
			
			
						
			mod.IDtoIndex.push_back(0);
					
			mod.IDtoIndex[maxID+1] = mod.neurons.size();*/
			
			for(int x = 0; x < mod.neurons.size(); ++x)
			{
				if(mod.neurons[x]->neuronID > maxID)
				{
					maxID = mod.neurons[x]->neuronID;
				}
			}
			
			nu->neuronID = maxID+1;
			mod.neurons.push_back(nu);
			
			int ind = mod.neurons.size()-1;
			
			deque<int> nottaken;
			
			int connections = 0;//connections that a neuron will have
			int inboundConnectionNum = 0;
			int outboundConnectionNum = 0;
			
			int otherNeuronID = 0;//ID of the other neuron
			
			for(int z = 0; z<mod.neurons.size(); ++z)
			{
				//connections = rand() % (mod.neurons.size() - mod.neurons.size()/2) + mod.neurons.size()/3;
				
				for(int x = 0; x<mod.neurons.size(); ++x)
				{
					if(!connectionExists(mod.neurons[ind]->neuronID,mod.neurons[x]->neuronID,mod) && ind != x)
					nottaken.push_back(x);
				}
				
				random_shuffle(nottaken.begin(),nottaken.end());
				
				inboundConnectionNum = (nottaken.size()-1)/2;
				outboundConnectionNum = (nottaken.size()-1)/2;
				
				
				while(mod.neurons[ind]->outboundConnections.size() < outboundConnectionNum)
				{
					newConnection(mod.neurons[ind]->neuronID,nottaken[0],mod);
					nottaken.pop_front();
				}
				
				while(nu->inboundConnections.size() < inboundConnectionNum)
				{
					newConnection(mod.neurons[nottaken[0]]->neuronID,mod.neurons[ind]->neuronID,mod);
					nottaken.pop_front();			
				}
				
			}
			
		}
		
		
		if(rand()%DELETE_NEURON_CHANCE == 1 && mod.neurons.size() > mod.inputNeurons.size() + mod.outputNeurons.size())
		{
			int ind = 0;
			log("DELETE");
			while(mod.neurons[ind]->type == INPUT || mod.neurons[ind]->type == OUTPUT)
			{
				ind = rand()%mod.neurons.size();
			}				
			
			deleteNeuron(mod.neurons[ind]->neuronID,mod);
		/*	for(int x = 0; x<mod.neurons[ind]->outboundConnections.size(); ++x)
			{
				deleteConnection(mod.neurons[ind]->neuronID,mod.neurons[ind]->outboundConnections[x]->receiverID,mod);
			}
			
			for(int x = 0; x<mod.neurons[ind]->inboundConnections.size(); ++x)
			{
				deleteConnection(mod.neurons[ind]->inboundConnections[x]->emitterID,mod.neurons[ind]->neuronID,mod);
			}
			
			mod.neurons.erase(mod.neurons.begin()+ind);*/
			
			

		}
		
		if(rand()%3 == 1)
		goto nra;
		
		
		
		for(int x = 0; x<mod.neurons.size(); ++x)
		{
			for(int z = 0; z<mod.neurons[x]->outboundConnections.size(); ++z)
			{
				if(rand()%WEIGHT_MOD_CHANCE == 1)
				{
					mod.neurons[x]->outboundConnections[z]->weight += rand()%(MAX_WEIGHT_MOD * 2) - MAX_WEIGHT_MOD;
				}
				
				
				if(rand()%DELETE_CONNECTION_CHANCE == 1)
				{
					onn = rand()%mod.neurons[x]->outboundConnections.size();
					
					if(connectionExists(mod.neurons[x]->outboundConnections[onn]->receiverID,mod.neurons[x]->neuronID,mod))
					{
						deleteConnection(mod.neurons[x]->neuronID,mod.neurons[x]->outboundConnections[onn]->receiverID,mod);
					}
					
				}
				
				
				if(rand()%NEW_CONNECTION_CHANCE == 1)
				{
					onn = rand()%mod.neurons.size();
					if(!connectionExists(mod.neurons[x]->neuronID,mod.neurons[onn]->neuronID,mod) && onn != x)
					{
						if(rand()% 2 == 1)
						{
							newConnection(mod.neurons[onn]->neuronID,mod.neurons[x]->neuronID,mod);
						}else{
							newConnection(mod.neurons[x]->neuronID,mod.neurons[onn]->neuronID,mod);
						}
					}
				}
				
				
			}
			
			if(rand()%THRESHOLD_MOD_CHANCE == 1)
			{
				mod.neurons[x]->firingThreshold += rand()%(MAX_THRESHOLD_MOD * 2) - MAX_THRESHOLD_MOD;
			}
			
			
		}
		
		
		
		
		offspring.push_back(mod);
		
		
		
	}
	
	vector<gridbug> bugs;
	
	
	for(int i = 0; i<offspring.size(); ++i)
	{
		bugs.push_back(newBug(offspring[i]));
	}
	
	for(int i = 0; i<bugs.size(); ++i)
	{
		bugs[i].generation = n.generation + 1;	
	}
	
	gridbug h = newBug(n.brain);
	h.generation = n.generation + 1;
	bugs.push_back(h);
	return bugs;
}
