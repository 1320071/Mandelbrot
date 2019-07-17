#include "Creation.h"

Creation::Creation()
{

}

Creation::~Creation()
{

}

int Creation::ComputeThreads(size_t t)
{
	// size_t is the type returned by thread::hardware_concurrency() 
	// Changing this type from int doesn't really do much but makes the compiler complain less
	size_t i;

	// https://stackoverflow.com/questions/10673585/start-thread-with-member-function	
	for (i = 0; i < t; i++)
	{
		threadMut.push_back(new mutex);
		threadComplete.push_back(false);
		threadVec.push_back(thread([this, i] { this->ComputeSlice(i); }));
	}
	for (i = 0; i < t; i++)
	{
		if (threadVec[i].joinable())
			threadVec[i].join();
	}

	// Threaded as it runs alongside the above
	WriteSlices("MandelbrotPic.tga", t);

	// We will just work our way back through the vector
	for (i = 0; i < t; i++) 
	{
		// Ensures variables are deallocated
		threadVec.pop_back();
		threadMut.pop_back();
		threadComplete.pop_back();
	}

	return 0;
}

void Creation::ComputeSlice(size_t tId)
{
	int y = tId * sliceSize;
	int endY = y + sliceSize;

	threadMut[tId]->lock();
	for (y; y < endY; ++y)
	{
		for (int x = 0; x < WIDTH; ++x)
		{
			complex<double> c(left + (x * (right - left) / WIDTH),
				top + (y * (bottom - top) / HEIGHT));

			complex<double> z(0.0, 0.0);

			// Iterate z = z^2 + c until z moves more 2than 2 units
			// away from (0, 0), or we've iterated too many times.
			int iterations = 0;
			while (abs(z) < 2.0 && iterations < MAX_ITERATIONS)
			{
				z = (z * z) + c;

				++iterations;
			}

			if (iterations == MAX_ITERATIONS) // Different threads
			{
				// z didn't escape from the circle.
				// This point is in the Mandelbrot set.
				image[y][x] = 0x000000; // black like my soul
			}
			else
			{
				// z escaped within less than MAX_ITERATIONS
				// iterations. This point isn't in the set.
				// image[y][x] = 0xFFFFFF; // WHITE
				image[y][x] = 0x42f4a1; // NICE GREEN
			}
		}
	}

	threadMut[tId]->unlock();
	threadComplete[tId] = true;
	
	return;
}

/* FOR TESTING 
// Write the image to a TGA file with the given name.
// Format specification: http://www.gamers.org/dEngine/quake3/TGA.txt
void Creation::WriteTGA(const char* filename) 
{
	ofstream outfile(filename, ofstream::binary);	

	uint8_t header[18] = {
		0, // no image ID
		0, // no colour map
		2, // uncompressed 24-bit image
		0, 0, 0, 0, 0, // empty colour map specification
		0, 0, // X origin
		0, 0, // Y origin
		WIDTH & 0xFF, (WIDTH >> 8) & 0xFF, // width
		HEIGHT & 0xFF, (HEIGHT >> 8) & 0xFF, // height
		24, // bits per pixel
		0, // image descriptor
	};

	outfile.write((const char *)header, 18);

	for (int y = 0; y < HEIGHT; ++y)
	{
		for (int x = 0; x < WIDTH; ++x)
		{
			uint8_t pixel[3] = {
				image[y][x] & 0xFF, // blue channel
				(image[y][x] >> 8) & 0xFF, // green channel
				(image[y][x] >> 16) & 0xFF, // red channel
			};
			outfile.write((const char *)pixel, 3);
		}
	}

	outfile.close();

	if (!outfile)
	{
		// An error has occurred at some point since we opened the file.
		cout << "Error writing to " << filename << endl;
		exit(1);
	}
}
*/

void Creation::WriteSlices(const char* filename, size_t t)
{
	ofstream outfile(filename, ofstream::binary);

	uint8_t header[18] = {
		0, // no image ID
		0, // no colour map
		2, // uncompressed 24-bit image
		0, 0, 0, 0, 0, // empty colour map specification
		0, 0, // X origin
		0, 0, // Y origin
		WIDTH & 0xFF, (WIDTH >> 8) & 0xFF, // width
		HEIGHT & 0xFF, (HEIGHT >> 8) & 0xFF, // height
		24, // bits per pixel
		0, // image descriptor
	};
	
	outfile.write((const char *)header, 18);

	for (size_t i = 0; i < t; i++)
	{
		int y = i * sliceSize;
		int endY = y + sliceSize;

		if (!threadComplete[i])
		{
			unique_lock<mutex> uniqueLock(*threadMut[i]); // If the thread has not been computed yet, wait
		}

		for (y; y < endY; ++y)
		{
			for (int x = 0; x < WIDTH; ++x)
			{
				uint8_t pixel[3] = {
					image[y][x] & 0xFF, // blue channel
					(image[y][x] >> 8) & 0xFF, // green channel
					(image[y][x] >> 16) & 0xFF, // red channel
				};
				outfile.write((const char *)pixel, 3);
			}
		}
	}
	
	outfile.close();

	if (!outfile)
	{
		// An error has occurred at some point since we opened the file.
		cout << "Error writing to " << filename << endl;
		exit(1);
	}
}

void Creation::SetSliceSize(size_t t) // How many threads to be running at the same time
{
	sliceSize = HEIGHT / t; // Divides image into even slices according to max threads
	return;
}