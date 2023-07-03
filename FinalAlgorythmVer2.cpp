#include<cstdio>
#include <stdlib.h>
#include<vector>
#include<algorithm>
#include <cmath>
#include <time.h>

using namespace std;

//******************************************************************************
//Mateusz Tabaszewski
//ver.2. -11.2022
//
//*****************************************************************************


//Parameters
//---------------------------------------------------------------------------
const int NUM_OF_SOL=20;        //population in evolutionary alg.
//propability of changes
const int PROP_OF_LIBS_MUT=80; //%   - propab. of libs muatations
const int PROP_OF_BOOKS_MUT=80; //%  - propab. of books muatations
//the depth of the changes in population
const int MAX_LIBS_CHANGES=100;  //max number of changes in muted lib and
const int MAX_BOOKS_CHANGES=100; //books
const int NUM_OF_ITERATIONS=200; //number of iterations in evolutionary alg.

const double maxTime=4.0*60+40.0; //sec 4:40
//------------------------------------------------------------------------

//Global variables
long long int level; //for method 1 - Quartile 50%
int nBooks, nLibs, nDays,simDays; //input data
int noZerosLibs;                  //number of not used libs


//scores of books and copy
static int book_scores[100000];
static int copy_book_scores[100000];

//library declaration
struct library{
	int T;  //sign up time
	int M;  //rate
	int N;  //nBooks
	long long book_scores_sum;
	bool used;
	vector<int>ID_books;
	vector<int>Scores;
	int ownIndex;
};

//world of libs and copy
static library libs[100000];
static library copy_Libs[100000];

//solutions
vector <library> solution[NUM_OF_SOL];    //table off solutions (table of containers with libs)
vector <library> solutionTmp[NUM_OF_SOL]; //temporary vector
vector <long long int> total_scores;     //scores of solutions

vector<library> bestSol;                 //best solution - container of libs
long long int bestTotalScore=0;          //total score of best solution

//for stoping cmd window
void Pause(void)
{char pause; scanf("%c",&pause);}

//Prototypes of functions

void UpdateScore(library &lib);
//----------------------------------------------------------------------------
//Input
//using global variables
//---------------------------------------------------------------------------


void ReadData(void)
{
  scanf(" %i %i %i", &nBooks, &nLibs, &nDays);

for(int i = 0; i < nBooks; i++){
		scanf(" %i", book_scores + i);
		copy_book_scores[i]=book_scores[i];
}

	for(int i = 0; i < nLibs; ++i){
		int N_books, T_tscan, M_rate,b;

		scanf(" %i %i %i", &N_books, &T_tscan, &M_rate);

		libs[i].T =T_tscan;
		libs[i].M = M_rate;
		libs[i].N= N_books;
		libs[i].book_scores_sum=0L;
		libs[i].used=false;
		libs[i].ownIndex=i;

		for(int j = 0; j < N_books; ++j){

			scanf(" %i", &b);

			libs[i].ID_books.push_back(b);
			libs[i].Scores.push_back(book_scores[b]);
		}
	  copy_Libs[i]=libs[i];
	}

  vector<int> sortedBookS;

  for(long long int i =0;i<nBooks;i++)
   {
	sortedBookS.push_back(book_scores[i]);
   }
   sort (sortedBookS.begin(), sortedBookS.end());
   level=sortedBookS[nBooks/2];
}

//----------------------------------------------------------------------
// Output
//-----------------------------------------------------------------------

void WriteSoluton(vector<library> &bestSol)
{
  int L=bestSol.size();
  printf("%i\n",L);
  for (int i=0;i<L;i++)
   {
	 printf("%i %i\n",bestSol[i].ownIndex,bestSol[i].N);
		for(int j=0;j<bestSol[i].N;j++)
		   printf("%i ",bestSol[i].ID_books[j]);
   printf("\n");
   }
}

//*************************************************************************
// Sorting, evaluation, data operations
//*************************************************************************

 //-------------------------------------------------------------------------
// Searching idx of lib with max score
//-------------------------------------------------------------------------

int MaxScore(library *libs,int NLibs)
{
  long long maxVal=0;
  int idx=-1;
  for (int i=0; i<NLibs;i++)
	 if(!libs[i].used && (maxVal<libs[i].book_scores_sum))
	 {
	   maxVal= libs[i].book_scores_sum;
	   idx=i;
	 }
  if(idx>=0)
	libs[idx].used=true;
  return idx;
}
//-------------------------------------------------------------------------
//Updating score in table of scores
//using global var
//-------------------------------------------------------------------------
void UpdateScore(library &lib)
{
  for (int i=0;i<lib.N;i++)
	 book_scores[lib.ID_books[i]]=0;
}
//---------------------------------------------------------------------------
//Change scores in lib after changing of book_scores
//using global vals
//-------------------------------------------------------------------------
void UpdateScoresInLib(library &lib)
{
  for(int i=0;i<lib.N;i++)
	lib.Scores[i]=book_scores[lib.ID_books[i]];
}
//-----------------------------------------------------------------------
// Copy book score
//-----------------------------------------------------------------------

void CopyScore(void)
{
  for(int i = 0; i < nBooks; ++i)
  {
   book_scores[i]=copy_book_scores[i];
  }
}
//-------------------------------------------------------------------------
// Copy libs and book_scores
//------------------------------------------------------------------------
void copyData(void)
{
 for(int i = 0; i < nLibs; ++i)
  {
	libs[i]=copy_Libs[i];
  }
  CopyScore();
}


//-----------------------------------------------------------------------------
//Counting sort  - sorting books in library
//----------------------------------------------------------------------------

//for counting sort
static int booksScoresInLib[100000];  //output scores  sorted order
static int booksIDInLib[100000];      //output ID's  sorted order
static int bookScore[1001];           //counts


void SortBooksScore(library &lib,int *bookScore,int *booksScoresInLib,int *booksIDInLib)
{
  //largest element of the library (best score)
  int max = lib.Scores[0];
  for (int i = 1; i < lib.N; i++) {
	if (lib.Scores[i] > max)
	   max = lib.Scores[i];
  }

  // Initialize count array with all zeros.
  for (int i = 0; i <= max; ++i) {
	bookScore[i] = 0;
  }

  // Store the count of each element
  for (int i = 0; i < lib.N; i++) {
	bookScore[lib.Scores[i]]++;
  }

  //Calculation of cummulative count
  for (int i = 1; i <= max; i++) {
	bookScore[i] += bookScore[i - 1];
  }

  // Identyfication of index of each element of the original array in count array, and
  // place the element in temp. array of bookScores and books IDs
  for (int i = lib.N - 1; i >= 0; i--) {
	booksScoresInLib[bookScore[lib.Scores[i]] - 1] = lib.Scores[i];
	booksIDInLib[bookScore[lib.Scores[i]] - 1]=lib.ID_books[i];
	bookScore[lib.Scores[i]]--;
  }

  // Copy the sorted elements input array of scores and ID books
  int j=lib.N-1;
  for (int i = 0; i < lib.N; i++,j--) {
   lib.Scores[i]= booksScoresInLib[j];
   lib.ID_books[i]=booksIDInLib[j];
  }
}

//-------------------------------------------------------------------------
//Test function - printing libs
//------------------------------------------------------------------
void Test(library *libs,int from,int to)
{
  for (int i=from;i<to;i++)
  {
   printf("[lib =%d ",i);
   for (int j=0;j<libs[i].ID_books.size();j++)
	printf("(%d,%d); ",libs[i].ID_books[j],libs[i].Scores[j]);
	printf("] = total %lli\n",libs[i].book_scores_sum);
  }
  Pause();
}

//---------------------------------------------------------------------
// Actual total score for solution computing
//---------------------------------------------------------------------

 long long int computeTotalScore(vector <library> &sol)
 {
 long long totalScore=0;
 for (int i=0;i<sol.size();i++)
   {
	 for(int j=0;j<sol[i].N;j++)
	  totalScore+=sol[i].Scores[j];
   }
	return totalScore;
 }


//*******************************************************************
// Greedy algorythm
//*******************************************************************
//--------------------------------------------------------------------
//Calculation of capacity and scores functions
//-------------------------------------------------------------------
long long int CalcCapacity(library &lib,int remTime)
{
  if(lib.used) return 0;
  long long int  capacity=(long long int)lib.M*(remTime-lib.T);
  if(capacity>lib.N)
	  capacity=lib.N;
  if(capacity<=0)
	 {
	  lib.book_scores_sum=0;
	  return 0;
	 }
  return capacity;
}
//------------------------------------------------------------------
//Method 1 - penalty for low scores of books (quartile 50% - below median)
//sum of scores after corection/T
//Using glbal variable - level
//-----------------------------------------------------------------
long long int Method1(library &lib,long long int capacity)
{
  lib.book_scores_sum=0;
  long long int count=0;
  for(long long int i=0;i<capacity;i++)
  {
  if(lib.Scores[i]<level && lib.Scores[i]!=0) count++;
  lib.book_scores_sum+=lib.Scores[i];
  }

  return (lib.book_scores_sum-count*level/2)/lib.T;
}
//-----------------------------------------------------------------------
//Method 2 - sum of scores /T
//-----------------------------------------------------------------------
long long int Method2(library &lib,long long int capacity)
{
   lib.book_scores_sum=0;

  for(long long int i=0;i<capacity;i++)
	{lib.book_scores_sum+=lib.Scores[i];}
  return(lib.book_scores_sum/lib.T);
}
//-----------------------------------------------------------------------
//Method 3 - sum of scores / sqrt(T)
//------------------------------------------------------------------------
 long long int Method3(library &lib,long long int capacity)
{
   lib.book_scores_sum=0;

  for(long long int i=0;i<capacity;i++)
	{lib.book_scores_sum+=lib.Scores[i];}
  return(lib.book_scores_sum/sqrt(lib.T));
}

//----------------------------------------------------------------------
//Calculation of measure of quality (value) a lib
//----------------------------------------------------------------------
int LibMeasureCalac(library &lib,int remTime,
	long long int (*Method)(library &,long long int))
{

  if(lib.used) return 0;
  long long int  capacity=CalcCapacity(lib,remTime);
  if(capacity==0) return 0;

  lib.book_scores_sum=Method(lib,capacity);
  lib.N=capacity;
  return 1;
}
//-----------------------------------------------------------------
// Calculation of real score of books in lib. for rem. time
//  for oryginal book scores and actualization of lib.Scores
//-----------------------------------------------------------------
long long int LibScoreCalac(library &lib,int *book_scores,int remTime)
{

  long long int  capacity=CalcCapacity(lib,remTime);

  lib.book_scores_sum=0;

  for(long long int i=0;i<capacity;i++)
  {
  lib.book_scores_sum+=book_scores[lib.ID_books[i]];
  lib.Scores[i]=book_scores[lib.ID_books[i]];
  }

  lib.N=capacity;
  UpdateScore(lib); //updating score in rest of libs
  return lib.book_scores_sum;
}


//**************************************************************************
//Evolution algorithm
//************************************************************************
//-----------------------------------------------------------------------
//swap two libs
//-----------------------------------------------------------------------
 void SwapLibs(library &l1,library &l2)
 {
   library tmp=l1;
   l1=l2;
   l2=tmp;
 }
 //----------------------------------------------------------------------
 // Reseting libs in solution
 //----------------------------------------------------------------------
 void ResetLibs(vector<library>&oneSolution)
 {
   int size=oneSolution.size();
   for(int i=0;i<size;i++)
	{
		oneSolution[i].N=oneSolution[i].ID_books.size();
		oneSolution[i].used=false;
    }
 }

 //----------------------------------------------------------------------
 // Swap two books
 //----------------------------------------------------------------------
 void SwapBooks(int &b1, int &b2)
 {
   int tmp=b1;
   b1=b2;
   b2=tmp;
 }
 //-----------------------------------------------------------------------
 // Recalculation of solution
 // using global table book_scores
 //-----------------------------------------------------------------------

 long long int RecalculateSolution(vector<library>&oneSolution, int nDays)
 {
	//oryginal scores
	CopyScore();
	//recalculating scores and update scores in libs
	int simDays=nDays;
	long long int sumOfScores=0;

	for (int i=0;i<oneSolution.size();i++)
	{
	   sumOfScores+=LibScoreCalac(oneSolution[i],book_scores,simDays);
	   simDays-=oneSolution[i].T;
	   if(simDays<=0) break;
	}

	 for(int i=0;i<nLibs;i++)
	  {
		if(!libs[i].used)
		 {
		  UpdateScoresInLib(libs[i]);
		 }
	  }
	return sumOfScores;
 }
 //---------------------------------------------------------------------
 //  Mutations in solution
 //
 //-------------------------------------------------------------------
 void Mutation (vector<library>&oneSolution, int maxLibsMut,
			   int maxBooksMut,int propOfLibsMutations,int  propOfBooksMutations )
 {
	//libs mutations
	int rand1,rand2;
	int size=oneSolution.size(),bookSize;
	if((rand()%100)<propOfLibsMutations)
	 {
	   //number of mutation in libs
	   int mutCounts=rand()%maxLibsMut+1;
	   for(int j=0;j<mutCounts;j++)
		{
		  rand1=rand()%size; rand2=rand()%size;
		  if(rand1!=rand2)
		   SwapLibs(oneSolution[rand1],oneSolution[rand2]);
		}

	 }
	 //reset of libs
		ResetLibs(oneSolution);

	//books mutations
	for (int i=0;i<size;i++)
	{
	   if((rand()%100)<propOfBooksMutations)
	   {
		 bookSize=oneSolution[i].ID_books.size();
		 int mutCounts=rand()%maxBooksMut+1;
		 for(int j=0;j<mutCounts;j++)
		{
		  rand1=rand()%bookSize; rand2=rand()%bookSize;
		  if(rand1!=rand2)
		   SwapBooks(oneSolution[i].ID_books[rand1],oneSolution[i].ID_books[rand2]);
		}
	   }
	}
 }

 //--------------------------------------------------------------------------
 //Creation of population 0
 //copy and mutation of greedy solution
 //fisrt 3 elements are solution of greedy algorythm. Rest is mutated copy of this
 //--------------------------------------------------------------------------

 void CreateStartPopulation(int Ninput,int Nmax,vector<library> *population)
 {
   int k=0;

   for (int i=Ninput;i<Nmax;i++)
	{
	  population[i]=population[k];  //solutions from greedy - index k
	  Mutation(population[i],MAX_LIBS_CHANGES,MAX_BOOKS_CHANGES,PROP_OF_LIBS_MUT,PROP_OF_BOOKS_MUT);
	  total_scores.push_back(RecalculateSolution(population[i],nDays));

	  k++;
	  if(k==Ninput) k=0;
	}
 }
//---------------------------------------------------------------------------
//Creation of new population - all solutions can be mutated
//---------------------------------------------------------------------------

 void CreateNewPopulation(int Nmax,vector<library> *population)
 {
	total_scores.clear();
	for (int i=0;i<Nmax;i++)
	{
	 Mutation(population[i],MAX_LIBS_CHANGES,MAX_BOOKS_CHANGES,PROP_OF_LIBS_MUT,PROP_OF_BOOKS_MUT);
	 total_scores.push_back(RecalculateSolution(population[i],nDays));
	}
 }

 //--------------------------------------------------------------------------
 //  Find winner - the best pop. in iteration of tournament
 //-------------------------------------------------------------------------

 int GetBest(vector<library> *InputPopulation,vector<int>idx)
 {
  int best=0;
  long long int val=0,valMax=0;
  for (int i=0;i<idx.size();i++)
   {
	 val= computeTotalScore(InputPopulation[idx[i]]);
	 if(val>valMax)
	 {
	   valMax=val;
	   best=i;
	 }
   }
   return best;
 }
 //----------------------------------------------------------------------------
 //   Tournament selection
 //----------------------------------------------------------------------------

 void Tournament(vector<library> *InputPopulation,vector<library> *OutputPopulation,int Nmax)
 {
   int numOfcomp=Nmax/3;  //1/3 of population chosen as competitors
   vector <int> idx;
   for (int i=0;i<Nmax;i++)
   {
	 idx.clear();
	 for(int j=0;j<numOfcomp;j++)
	   {
		 int r=rand()%Nmax;
		 idx.push_back(r);
	   }
	  int winner=GetBest(InputPopulation,idx);
	  OutputPopulation[i]=InputPopulation[winner];
	  total_scores[i]=total_scores[winner];
   }
 }
//********************************************************************************


int main(int argc, char** argv){

time_t beginTime, endTime;
time(&beginTime);

//Input data ------------------------------------------------------------

	ReadData();

//Greedy algorythm --------------------------------------------------------

simDays=nDays; //days of simulation

//evaluation functions table
long long int (*MethodTable[])(library &,long long int)={Method1,Method2,Method3};

//three competitor methods of evaluations for greedy

for (int method=0;method<3;method++)
{
while(simDays>0)
 {
	noZerosLibs=0;
	for (int i=0;i<nLibs;i++)
	{
	  SortBooksScore(libs[i],bookScore,booksScoresInLib,booksIDInLib);
	  noZerosLibs+= LibMeasureCalac(libs[i],simDays,MethodTable[method]);
	}
	if(!noZerosLibs) break;

	//max score for libs
	int idx=MaxScore(libs,nLibs);
	if(idx<0) break;
	//add solution
	if(libs[idx].T<=simDays)
	  { solution[method].push_back(libs[idx]);}
	else
	 break;
	//udate scores in input data
	UpdateScore(libs[idx]);
	//update scores in libs  - new data in books_scores
	 for(int i=0;i<nLibs;i++)
	  {
		if(!libs[i].used)
		 {UpdateScoresInLib(libs[i]);}
	  }

	simDays-=libs[idx].T;
 }//while


   total_scores.push_back(computeTotalScore(solution[method])); // S_total
   // print total scores aftewr greedy
  // printf("Total score %lli\n",total_scores[method]);

 copyData();
 simDays=nDays;
}//for

//end of greedy -------------------------------------------------------




//evolutionary algorythm ----------------------------------------------

//population 0
CreateStartPopulation(3,NUM_OF_SOL,solution);



long long int idx;  //index of best solution

//Evolution
bestTotalScore=0;
for(int step=0; step<=NUM_OF_ITERATIONS;step++)
 {
  //Find and remember the best solution
   idx=0;
   for (int i=0;i<NUM_OF_SOL;i++)
   {if(total_scores[idx]<total_scores[i]) idx=i;}

   if(bestTotalScore<total_scores[idx])  bestSol=solution[idx];
   bestTotalScore=computeTotalScore(bestSol);

   if(step==NUM_OF_ITERATIONS) break;
   time(&endTime);

   if(difftime(endTime,beginTime)>maxTime) break;

 //tournament selection
  Tournament(solution,solutionTmp,NUM_OF_SOL);
 //calculation of score and
 for (int i=0;i<NUM_OF_SOL;i++)
	 {total_scores[i]=computeTotalScore(solutionTmp[i]);
	  solution[i]=solutionTmp[i]; }

//Mutations and creation new population of solution
	 CreateNewPopulation(NUM_OF_SOL,solution);

 }//for



	 WriteSoluton(bestSol);


	return 0;
}
