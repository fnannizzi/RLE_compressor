#pragma once
#include <ostream>

// RLE Compression/Decompression
template <typename T>
struct RLE
{
	// Memory which stores either compressed or decompressed data
	T* m_Data;
	// Number of elements of type T that data is pointing to
	int m_Size;

	RLE()
	: m_Data(nullptr)
	, m_Size(0)
	{ }

	~RLE()
	{
		delete[] m_Data;
	}

	
	// Compresses input data and stores it in m_Data
	void Compress(const T* input, int inSize);

	// Decompresses input data and stores it in m_Data
	void Decompress(const T* input, int inSize, int outSize);

	// Outputs m_Data
	friend std::ostream& operator<< (std::ostream& stream, const RLE<T>& rhs)
	{
		for (int i = 0; i < rhs.m_Size; i++)
		{
			stream << rhs.m_Data[i];
		}
		return stream;
	}

	// Shenanigans to get the maximum run size
	int MaxRunSize();

	// Tired of writing m_Size++ over and over
	void WritePositiveData(int runSize, char value)
	{
		m_Data[m_Size] = (T)runSize;
		m_Size++;
		m_Data[m_Size] = (T)(value);
		m_Size++;
	}
};

template <typename T>
void RLE<T>::Compress(const T* input, int inSize)
{
	delete m_Data;
	m_Size = 0;
	m_Data = new T[2 * inSize];
	int maxRunSize = MaxRunSize();
	const T* temp = input;
	int runSize = 1;
	bool positiveRun = false;

	if ((*temp) == (*(temp + 1)))
	{
		positiveRun = true;
	}

	for (int character = 1; character < inSize; character++)
	{
		if (positiveRun)
		{ // in a positive run
			if ((*temp) == (*(temp + 1)))
			{ // continuing the positive run

				if (runSize == maxRunSize)
				{ // need to record run before exceeding maxRunSize
					WritePositiveData(runSize, (*temp));
					runSize = 0;
				}
				runSize++;

				if (character == (inSize - 1))
				{ // reached the end of the input and need to write
					WritePositiveData(runSize, (*temp));
					runSize = 0;
				}
			}
			// positive run is ending
			else 
			{
				WritePositiveData(runSize, (*temp));
				runSize = 1;

				// another positive run is beginning
				if ((*(temp + 1)) != (*(temp + 2)))
				{ 
					positiveRun = false;
				}

				// reached the end of the input and need to write
				if (character == (inSize - 1))
				{ 
					WritePositiveData(runSize, (*(temp + 1)));
					runSize = 0;
				}
			}
		}
		else { // in a negative run
			if ((*temp) == (*(temp + 1))){ // negative run is ending
				runSize--;
				m_Data[m_Size] = (T)((-1)*runSize);
				m_Size++;
				for (int backtrack = runSize; backtrack > 0; backtrack--){
					m_Data[m_Size] = (T)(*(temp - backtrack));
					m_Size++;
				}

				positiveRun = true;
				runSize = 2;

				if (character == (inSize - 1)){ // reached the end of the input and need to write
					WritePositiveData(runSize, (*temp));
				}
			}
			// continuing the negative run
			else 
			{ 
				runSize++;

				// need to record run before exceeding maxRunSize
				if (runSize == maxRunSize)
				{ 
					// negative run is ending soon, and our runSize is too long
					if ((*(temp + 1)) == (*(temp + 2)))
					{ 
						runSize--;
						m_Data[m_Size] = (T)((-1)*runSize);
						m_Size++;
						for (int backtrack = runSize; backtrack > 0; backtrack--)
						{
							m_Data[m_Size] = (T)(*(temp - backtrack));
							m_Size++;
						}
					}
					// negative run is going to continue a little longer
					else 
					{ 
						m_Data[m_Size] = (T)((-1)*runSize);
						m_Size++;
						for (int backtrack = runSize; backtrack > 0; backtrack--)
						{
							m_Data[m_Size] = (T)(*(temp - backtrack));
							m_Size++;
						}
						++temp;
					}
					runSize = 1;
				}

				// reached the end of the input and need to write
				if (character == (inSize - 1))
				{ 
					m_Data[m_Size] = (T)((-1)*runSize);
					m_Size++;
					for (int backtrack = (runSize - 2); backtrack > (-2); backtrack--)
					{
						m_Data[m_Size] = (T)(*(temp - backtrack));
						m_Size++;
					}
				}
			}
		}
		++temp;
	}
}

template <typename T>
void RLE<T>::Decompress(const T* input, int inSize, int outSize)
{
	delete m_Data;
	m_Size = 0;
	m_Data = new T[outSize];
	const T* runStart = input;
	const T* runTrack = (input + 1);
	int runSize;
	bool positiveRun;

	while (runStart < (input + inSize)) 
	{
		// beginning a positive run
		if ((*runStart) > 0)
		{ 
			positiveRun = true;
			runSize = (*runStart);
		}
		// beginning a negative run
		else 
		{
			positiveRun = false;
			runSize = (-1)*(*runStart);
		}
		for (int i = 0; i < runSize; i++)
		{ 
			m_Data[m_Size] = *runTrack;
			m_Size++;

			// if in a negative run, increment the pointer to the next character
			if (!positiveRun)
			{
				++runTrack;
			}
		}
		// if we just finished a positive run, we need to increment the pointer to the beginning of the next run
		if (positiveRun)
		{ 
			++runTrack;
		}
		runStart = runTrack; // reset the start of the run
		++runTrack; // reset the pointer to the first character
	}
}
