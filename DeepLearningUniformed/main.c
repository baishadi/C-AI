#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define INPUT_NUM 121
#define HIDDEN_LAYER_NUM 3
#define HIDDEN_LAYER_SIZE 36
#define OUTPUT_NUM 10
#define LEARNING_RATE 0.4

double preferredOutput[OUTPUT_NUM];

struct Sample
{
    double input[INPUT_NUM],output[OUTPUT_NUM];
};

struct InputCell
{
    double value;
} inputLayer[INPUT_NUM];

struct FirstHiddenCell
{
    double bias;
    double weight[INPUT_NUM];
    double value;
} firstHiddenLayer[HIDDEN_LAYER_SIZE];

struct HiddenCell
{
    double bias;
    double weight[HIDDEN_LAYER_SIZE];
    double value;
} hiddenLayers[HIDDEN_LAYER_NUM-1][HIDDEN_LAYER_SIZE];

struct OutputCell
{
    double bias;
    double weight[HIDDEN_LAYER_SIZE];
    double value;
} outputLayer[OUTPUT_NUM];

struct BPData
{
    double dCHiddenBIAS[HIDDEN_LAYER_NUM][HIDDEN_LAYER_SIZE];
    double dCOutputBIAS[OUTPUT_NUM];
    double dCFirstHiddenWeight[HIDDEN_LAYER_SIZE][INPUT_NUM];
    double dCHiddenRestWeight[HIDDEN_LAYER_NUM-1][HIDDEN_LAYER_SIZE][HIDDEN_LAYER_SIZE];
    double dCOutputWeight[OUTPUT_NUM][HIDDEN_LAYER_SIZE];
};

double logistic(double rawValue)
{
    return 1/(1+pow(2.718,-rawValue));
}

void initialize()
{
    for(int i=0; i<HIDDEN_LAYER_SIZE; i++) //1st hidden layer
    {
        firstHiddenLayer[i].bias = (rand()%1000)*1.0/RAND_MAX;
        for(int j=0; j<INPUT_NUM; j++)
        {
            firstHiddenLayer[i].weight[j] = (rand()%1000)*1.0/RAND_MAX;
        }
    }
    for(int i=0; i<HIDDEN_LAYER_NUM-1; i++)//layers
    {
        for(int j=0; j<HIDDEN_LAYER_SIZE; j++)
        {
            hiddenLayers[i][j].bias = (rand()%1000)*1.0/RAND_MAX;
            for(int k=0; k<HIDDEN_LAYER_SIZE; k++)
            {
                hiddenLayers[i][j].weight[k] = (rand()%1000)*1.0/RAND_MAX;
            }
        }
    }
    for(int i=0; i<OUTPUT_NUM; i++)//output
    {
        outputLayer[i].bias = (rand()%1000)*1.0/RAND_MAX;
        for(int j=0; j<HIDDEN_LAYER_SIZE; j++)
        {
            outputLayer[i].weight[j] = (rand()%1000)*1.0/RAND_MAX;
        }
    }
}

void feedForward(struct Sample pic)
{
    for(int i=0; i<INPUT_NUM; i++)//input
    {
        inputLayer[i].value = pic.input[i];
    }
    for(int i=0; i<OUTPUT_NUM; i++)
    {
        preferredOutput[i] = pic.output[i];
    }
    for(int i=0; i<HIDDEN_LAYER_SIZE; i++)//calculate 1st hiddenlayer's value
    {
        firstHiddenLayer[i].value = firstHiddenLayer[i].bias;
        for(int j=0; j<INPUT_NUM; j++)//transverse input
        {
            firstHiddenLayer[i].value += inputLayer[j].value * firstHiddenLayer[i].weight[j];
        }
        firstHiddenLayer[i].value = logistic(firstHiddenLayer[i].value);
    }
    for(int i=0; i<HIDDEN_LAYER_SIZE; i++)//calculate 1st hiddenlayer's value
    {
        hiddenLayers[0][i].value = hiddenLayers[0][i].bias;
        for(int j=0; j<HIDDEN_LAYER_SIZE; j++)//transverse input
        {
            hiddenLayers[0][i].value += firstHiddenLayer[j].value * hiddenLayers[0][i].weight[j];
        }
        hiddenLayers[0][i].value = logistic(hiddenLayers[0][i].value);
    }
    for(int i=1; i<HIDDEN_LAYER_NUM-1; i++)//layers
    {
        for(int j=0; j<HIDDEN_LAYER_SIZE; j++)
        {
            hiddenLayers[i][j].value=hiddenLayers[i][j].bias;
            for(int k=0; k<HIDDEN_LAYER_SIZE; k++)
            {
                hiddenLayers[i][j].value += hiddenLayers[i-1][k].value * hiddenLayers[i][j].weight[k];
            }
            hiddenLayers[i][j].value = logistic(hiddenLayers[i][j].value);
        }
    }
    for(int i=0; i<OUTPUT_NUM; i++)//calculate outputlayer's value
    {
        outputLayer[i].value = outputLayer[i].bias;
        for(int j=0; j<HIDDEN_LAYER_SIZE; j++)//transverse 2nd hiddenlayer
        {
            outputLayer[i].value += hiddenLayers[HIDDEN_LAYER_NUM-2][j].value * outputLayer[i].weight[j];
        }
        outputLayer[i].value = logistic(outputLayer[i].value);
    }
}

double cost()
{
    double cost=0,difference;
    for(int i=0; i<OUTPUT_NUM; i++)
    {
        difference = (outputLayer[i].value-preferredOutput[i]);
        cost+= difference*difference;
    }
    return cost/2;
}


struct BPData backPropagation()
{
    struct BPData data;
    memset(&data,0x0,sizeof(data));
    for(int i=0; i<OUTPUT_NUM; i++)
    {
        data.dCOutputBIAS[i]= (outputLayer[i].value-preferredOutput[i])*(1-outputLayer[i].value) * outputLayer[i].value;
        for(int j=0; j<HIDDEN_LAYER_SIZE; j++)
        {
            data.dCOutputWeight[i][j] =
                data.dCOutputBIAS[i] * hiddenLayers[HIDDEN_LAYER_NUM-2][j].value;
            data.dCHiddenBIAS[HIDDEN_LAYER_NUM-1][j] += data.dCOutputBIAS[i] * outputLayer[i].weight[j]
                    * hiddenLayers[HIDDEN_LAYER_NUM-2][j].value * (1-hiddenLayers[HIDDEN_LAYER_NUM-2][j].value);
        }
    }
    for(int i=HIDDEN_LAYER_NUM-2; i>0; i--)
    {
        for(int j=0; j<HIDDEN_LAYER_SIZE; j++)
        {
            for(int k=0; k<HIDDEN_LAYER_SIZE; k++)
            {
                data.dCHiddenRestWeight[i][j][k] = data.dCHiddenBIAS[i+1][j] * hiddenLayers[i-1][k].value;
                data.dCHiddenBIAS[i][k] += data.dCHiddenBIAS[i+1][j] * hiddenLayers[i][j].weight[k]
                                             * hiddenLayers[i-1][k].value * (1-hiddenLayers[i-1][k].value);
            }
        }
    }
    for(int i=0; i<HIDDEN_LAYER_SIZE; i++)
    {
        for(int j=0; j<HIDDEN_LAYER_SIZE; j++)
        {
            data.dCHiddenRestWeight[0][i][j] = data.dCHiddenBIAS[1][i] * firstHiddenLayer[j].value;
            data.dCHiddenBIAS[0][j] += data.dCHiddenBIAS[1][i] * hiddenLayers[0][i].weight[j]
                                       * firstHiddenLayer[j].value * (1-firstHiddenLayer[j].value);
        }
    }
    for(int i=0; i<HIDDEN_LAYER_SIZE; i++)
    {
        for(int j=0; j<INPUT_NUM; j++)
        {
            data.dCFirstHiddenWeight[i][j] = data.dCHiddenBIAS[0][i] * inputLayer[j].value;
        }
    }
    return data;
}
double correct,count;
void learn(int dataNum,struct Sample *pics)
{
    struct BPData total,temp;
    double max=0;
    int index=0;
    memset(&total,0x00,sizeof(total));
    for(int n=0; n<dataNum; n++)
    {
        feedForward(pics[n]);
        temp = backPropagation();
        for(int i=0; i<OUTPUT_NUM; i++)
        {
            if(outputLayer[i].value>max)
            {
                index=i;
                max=outputLayer[i].value;
            }
        }
        count++;
        if(preferredOutput[index]==1)
        {
            correct+=1;
        }
        for(int i=0; i<HIDDEN_LAYER_NUM; i++)
        {
            for(int j=0; j<HIDDEN_LAYER_SIZE; j++)
            {
                total.dCHiddenBIAS[i][j] += temp.dCHiddenBIAS[i][j];
            }
        }
        for(int i=0; i<OUTPUT_NUM; i++)
        {
            total.dCOutputBIAS[i] += temp.dCOutputBIAS[i];
            for(int j=0; j<HIDDEN_LAYER_SIZE; j++)
            {
                total.dCOutputWeight[i][j] += temp.dCOutputWeight[i][j];
            }
        }
        for(int i=0; i<HIDDEN_LAYER_SIZE; i++)
        {
            for(int j=0; j<INPUT_NUM; j++)
            {
                total.dCFirstHiddenWeight[i][j] += temp.dCFirstHiddenWeight[i][j];
            }
        }
        for(int i=0; i<HIDDEN_LAYER_NUM-1; i++)
        {
            for(int j=0; j<HIDDEN_LAYER_SIZE; j++)
            {
                for(int k=0; k<HIDDEN_LAYER_SIZE; k++)
                {
                    total.dCHiddenRestWeight[i][j][k] += temp.dCHiddenRestWeight[i][j][k];
                }
            }
        }
    }
    for(int i=0; i<HIDDEN_LAYER_NUM-1; i++)
    {
        for(int j=0; j<HIDDEN_LAYER_SIZE; j++)
        {
            hiddenLayers[i][j].bias -= total.dCHiddenBIAS[i+1][j] * LEARNING_RATE;
        }
    }
    for(int i=0; i<OUTPUT_NUM; i++)
    {
        outputLayer[i].bias -= total.dCOutputBIAS[i] * LEARNING_RATE;
        for(int j=0; j<HIDDEN_LAYER_SIZE; j++)
        {
            outputLayer[i].weight[j] -= total.dCOutputWeight[i][j] * LEARNING_RATE;
        }
    }
    for(int i=0; i<HIDDEN_LAYER_SIZE; i++)
    {
        firstHiddenLayer[i].bias -= total.dCHiddenBIAS[0][i] * LEARNING_RATE;
        for(int j=0; j<INPUT_NUM; j++)
        {
            firstHiddenLayer[i].weight[j] -= total.dCFirstHiddenWeight[i][j] * LEARNING_RATE;
        }
    }
    for(int i=0; i<HIDDEN_LAYER_NUM-1; i++)
    {
        for(int j=0; j<HIDDEN_LAYER_SIZE; j++)
        {
            for(int k=0; k<HIDDEN_LAYER_SIZE; k++)
            {
                hiddenLayers[i][j].weight[k] -= total.dCHiddenRestWeight[i][j][k] * LEARNING_RATE;
            }
        }
    }
}

void test(struct Sample pic)
{
    double max=0;
    int index=0;
    feedForward(pic);
    printf("cost:%.12f\n",cost());
    for(int i=0; i<OUTPUT_NUM; i++)
    {
        if(outputLayer[i].value>max)
        {
            index=i;
            max=outputLayer[i].value;
        }
        printf(" %d:%.12f\n",i,outputLayer[i].value);
    }
    printf("G:%d\n",index);
}

struct Sample inputSample()
{
    struct Sample result;
    for(int i=0; i<INPUT_NUM; i++)
    {
        scanf("%lf",&result.input[i]);
    }
    for(int i=0; i<OUTPUT_NUM; i++)
    {
        scanf("%lf",&result.output[i]);
    }
    return result;
}

void printSample(struct Sample sample)
{
    for(int i=0; i<INPUT_NUM; i++)
    {
        printf("%.2f ",sample.input[i]);
    }
    printf("\n\n");
    for(int i=0; i<OUTPUT_NUM; i++)
    {
        printf("%.2f ",sample.output[i]);
    }
    printf("\n\n");
}

struct Sample pic[1190],tempPic;

int main()
{
    int m,n,tempi;
    srand((unsigned)time(NULL));
    initialize();
    for(int i=0; i<1190; i++)
    {
        pic[i]=inputSample();
    }
    scanf("%d%d",&m,&n);
    for(int t=0; t<n; t++)
    {
        for(int i=0; i<m; i++)
        {
            tempi = rand()%(1190-m)+m-1;
            tempPic = pic[i];
            pic[i] = pic[tempi];
            pic[tempi] = tempPic;
        }
        if(!(t%100000))
        {
            printSample(pic[0]);
            test(pic[0]);
            printf("Correctness:%.10f\n",(correct/count));
            correct=0;
            count=0;
        }
        learn(m,pic);
    }
    return 0;
}
