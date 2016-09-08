// HopfieldNN.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "my_matrix.h"
#include <iostream>
#include <vector>

#include "Utils.h"

using std::cout;
using std::endl;
using std::vector;


float sign(float x)
{
	if (x > 0.0f) 
		return 1.0f;
	else if (x == 0.0f) return 0.0f;

	return -1.0f;
}

matrix sign(matrix &m)
{
	for (int i = 0; i < m.NumRows(); i++)
		for (int j = 0; j < m.NumColumns(); j++)
			m(i, j) = sign(m(i, j));
	
	return m;
}

float max_fundamental_memories(float n)
{
	return n / (4 * log(n));
}

matrix& identity_NN(matrix& m)
{
	if (m.NumColumns() != m.NumRows()) return m; // ?? 

	for (int i = 0; i < m.NumRows(); i++)
		for (int j = 0; j < m.NumRows(); j++)
			m(i, j) = (i == j ? 1.0f : 0.0f);

	return m;
}

void Hopfield_Network_Test()
{
	// input states to memorize
	float number_of_states = 2.0f;
	matrix s1_(3, 1);
	matrix s2_(3, 1);

	// input state initialization
	s1_(0, 0) = 1;
	s1_(1, 0) = 1;
	s1_(2, 0) = 1;

	s2_(0, 0) = -1;
	s2_(1, 0) = -1;
	s2_(2, 0) = -1;

	// 3 x 3 identity matrix
	matrix ident33_(3, 3);
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			ident33_(i, j) = (i == j ? 1.0f : 0.0f);

	// create the transpositions of the state matrices 
	matrix s1_T_ = s1_;
	s1_T_.transpose();

	matrix s2_T_ = s2_;
	s2_T_.transpose();

	// compute the weight matrix
	matrix weight_matrix_ = s1_ * s1_T_ + s2_ * s2_T_ - ident33_ * number_of_states;

	// create a theshold matrix
	matrix threshold_(3, 1);
	threshold_(0, 0) = 0.0f;
	threshold_(1, 0) = 0.0f;
	threshold_(2, 0) = 0.0f;

	// print what happened
	cout << "Input States: " << endl;

	cout << "s1_" << endl;
	s1_.print();
	cout << endl;

	cout << "s2_" << endl;
	s2_.print();
	cout << endl;

	cout << "Weight Matrix: " << endl;
	weight_matrix_.print();

	cout << endl;

	// test the output with the states as the probes
	matrix Y1_ = sign(weight_matrix_ * s1_ - threshold_);

	matrix Y2_ = sign(weight_matrix_ * s2_ - threshold_);

	// print what happened
	cout << "printing the output" << endl;

	cout << "Y1_" << endl;
	Y1_.print();

	cout << endl;

	cout << "Y2_" << endl;
	Y2_.print();

	cout << endl;
}


void Hopfield_Network_Test_2()
{
	float number_of_states = 2.0f;

	int matrix_dimension = 80;
	cout << "Testing large Hopfield Network with matrix size: " << matrix_dimension << endl;

	float max_memories = 2*max_fundamental_memories((float)matrix_dimension);

	cout << "Expecting to store: " << (int)max_memories << " fundamental memories" << endl;

	matrix *inputs = new matrix[(int)max_memories];
	matrix *transpose_inputs = new matrix[(int)max_memories];
	
	int num_memories_stored = 0;
	cout << "creating random states ..." << endl;
	for (int i = 0; i < (int)max_memories; i++)
	{
		matrix state(matrix_dimension, 1);
		for (int j = 0; j < matrix_dimension; j++)
		{
			float rand_decision = RandomFloat(0, 20);

			if (rand_decision >= 10.0f)
				state(j, 0) = 1.0f;
			else state(j, 0) = -1.0f;
		}

		bool add_state = true;
		for (int p = 0; p < num_memories_stored; p++)
		{
			if (state.IsEqual(inputs[p])) // 
			{
				i--; // repeat the step
				add_state = false; // don't add this state
				break; // no need to continue search
			}
		}

		if (add_state)
		{
			matrix state_T;
			state_T = state;
			state_T.transpose();


			inputs[num_memories_stored] = state;
			transpose_inputs[num_memories_stored] = state_T;
			num_memories_stored++;
		}
			

	}

	matrix threshold_matrix(matrix_dimension, 1);

	matrix weight_matrix_(matrix_dimension, matrix_dimension);

	cout << "Storing Memories ..." << endl;

	for (int i = 0; i < num_memories_stored; i++)
	{
		weight_matrix_ = weight_matrix_ + inputs[i] * transpose_inputs[i];
	}

	matrix ident_NN_(matrix_dimension, matrix_dimension);

	identity_NN(ident_NN_);

	weight_matrix_ = weight_matrix_ - ident_NN_ * max_memories;

	cout << "Testing for memory retrieval ..." << endl;

	int count = 0;
	for (int p = 0; p < num_memories_stored; p++)
	{
		matrix Y = sign(weight_matrix_ * inputs[p] - threshold_matrix);

		if (Y.IsEqual(inputs[p]))
			count++;


	}

	cout << "count of retrieved memories: " << count << endl;

	delete [] inputs;
	delete[] transpose_inputs;
}

int main(int argc, char* argv[])
{
	Hopfield_Network_Test();

	Hopfield_Network_Test_2();

	return 0;
}

