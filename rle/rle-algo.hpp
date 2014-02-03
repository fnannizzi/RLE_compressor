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
		delete m_Data;
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
	if ((*temp) == (*(temp + 1))){
		positiveRun = true;
	}
	

	//std::cout << "Input size = " << inSize << std::endl;

	for (int character = 1; character < inSize; character++){
		if (positiveRun){ // in a positive run
			if ((*temp) == (*(temp + 1))){ // continuing the positive run
				//std::cout << "MATCH " << (*temp) << " " << (*(temp + 1)) << std::endl;

				if (runSize == maxRunSize){ // need to record run before exceeding maxRunSize
					//std::cout << "Reached maxRunSize = " << maxRunSize << std::endl;
					m_Data[m_Size] = (T)runSize;
					m_Size++;
					m_Data[m_Size] = (T)(*temp);
					m_Size++;
					runSize = 0;
				}
				runSize++;

				if (character == (inSize - 1)){ // reached the end of the input and need to write
					//std::cout << "Reached end of input on a positive run" << std::endl;
					m_Data[m_Size] = (T)runSize;
					m_Size++;
					m_Data[m_Size] = (T)(*temp);
					m_Size++;
					runSize = 0;
				}
			}
			else { // positive run is ending
				//std::cout << "NO MATCH " << (*temp) << " " << (*(temp + 1)) << std::endl;
				m_Data[m_Size] = (T)runSize;
				m_Size++;
				m_Data[m_Size] = (T)(*temp);
				m_Size++;
				//std::cout << "PRINT " << m_Data[m_Size - 2] << " " << m_Data[m_Size - 1] << std::endl;
				runSize = 1;
				positiveRun = false;

				if (character == (inSize - 1)){ // reached the end of the input and need to write
					//std::cout << "Reached end of input ending a positive run" << std::endl;
					m_Data[m_Size] = (T)runSize;
					m_Size++;
					m_Data[m_Size] = (T)(*(temp + 1));
					m_Size++;
					runSize = 0;
				}
			}
		}
		else { // in a negative run
			if ((*temp) == (*(temp + 1))){ // negative run is ending
				//std::cout << "MATCH " << (*temp) << " " << (*(temp + 1)) << std::endl;
				//std::cout << "BACKTRACKING because negative run is ending" << std::endl;
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
					//std::cout << "Reached end of input ending a negative run" << std::endl;
					m_Data[m_Size] = (T)runSize;
					m_Size++;
					m_Data[m_Size] = (T)(*temp);
					m_Size++;
				}
			}
			else { // continuing the negative run
				//std::cout << "NO MATCH " << (*temp) << " " << (*(temp + 1)) << std::endl;
				runSize++;

				if (runSize == maxRunSize){ // need to record run before exceeding maxRunSize
					if ((*(temp + 1)) == (*(temp + 2))){ // negative run is ending soon, and our runSize is too long
						//std::cout << "BACKTRACKING because maxRunSize reached and negative run ending" << std::endl;
						runSize--;
						m_Data[m_Size] = (T)((-1)*runSize);
						m_Size++;
						for (int backtrack = runSize; backtrack > 0; backtrack--){
							m_Data[m_Size] = (T)(*(temp - backtrack));
							m_Size++;
						}
					}
					else { // negative run is going to continue a little longer
						//std::cout << "BACKTRACKING because maxRunSize reached" << std::endl;
						m_Data[m_Size] = (T)((-1)*runSize);
						m_Size++;
						for (int backtrack = runSize; backtrack > 0; backtrack--){
							m_Data[m_Size] = (T)(*(temp - backtrack));
							m_Size++;
						}
						++temp;
					}
					runSize = 1;
				}

				if (character == (inSize - 1)){ // reached the end of the input and need to write
					//std::cout << "Reached end of input on a negative run" << std::endl;
					m_Data[m_Size] = (T)((-1)*runSize);
					m_Size++;
					for (int backtrack = runSize; backtrack > 0; backtrack--){
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

	while (m_Size < outSize){
		if ((*runStart) > 0){
			positiveRun = true;
			runSize = *runStart;
		}
		else {
			positiveRun = false;
			runSize = (-1)*(*runStart);
		}
		for (int i = 0; i < runSize; i++){
			m_Data[m_Size] = *runTrack;
			m_Size++;
			if (!positiveRun){
				++runTrack;
			}
			//std::cout << "PRINT " << m_Data[m_Size - 1] << std::endl;
		}
		if (positiveRun){
			++runTrack;
		}
		runStart = runTrack;
		++runTrack;
		//std::cout << "RUNTRACK " << (*runStart) << " " << (*runTrack) << std::endl;
	}

}
