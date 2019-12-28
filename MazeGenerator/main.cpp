#include <iostream>
#include <vector>
#include <cassert>
#include <random> // for std::mt19937
#include <ctime> // for std::time
#include <algorithm>
#include "lodepng.h"
#include <string>


std::string intToString(int value)
{
	int multiplier{ 1 };
	int num{ 1 };

	while (value / (multiplier * 10) >= 1)
	{
		multiplier *= 10;
		++num;
	}


	std::cout << "multiplier: " << multiplier << "\n";
	std::cout << "num: " << num << "\n";

	int tempVal{};
	int tempDiv{};
	std::string temp{};

	for (int i = 0; i < num; ++i)
	{
		tempDiv = value / (multiplier * 10);
		tempVal = (value - (multiplier * 10 * tempDiv))/ multiplier;
		std::cout << tempVal << "\n";
		multiplier /= 10;
		temp += (char)(tempVal + 48);

	}

	std::cout << temp << "\n";

	return temp;

}


void encodeOneStep(const char* filename, std::vector<unsigned char>& image, unsigned width, unsigned height);


std::mt19937 mersenne(static_cast<unsigned int>(std::time(nullptr)));

unsigned int getRandomInteger(int min, int max)
{
	//static std::mt19937 mersenne(static_cast<unsigned int>(std::time(nullptr)));
	std::uniform_int_distribution<> location(min, max);

	return location(mersenne);
}





struct Coordinates
{
	int x;
	int y;
};


class Maze
{
private:
	int m_quantity;
	int m_width;
	int m_height;
	int m_emptySpace;
	int m_numOfIsolatedEmptySpaces;
	float m_loopFactor;
	std::vector<bool> m_mazeTiles;
	std::vector<int> m_mazeConnectedEmptySpace;

	void DecrementEmptySpace()
	{
		--m_emptySpace;
	}


public:

	Maze(int width, int height, float loopFactor, int quantity) : m_width(width), m_height(height), m_emptySpace((width-2)*(height-2)), m_numOfIsolatedEmptySpaces(0), m_loopFactor(loopFactor), m_quantity(quantity)
	{
		
		m_mazeTiles.reserve(height*width * 2);
		m_mazeConnectedEmptySpace.reserve(height*width*2);

		for (int i = 1; i <= quantity; ++i)
		{
			std::string temp = intToString(i);
			temp = "maze" + temp + ".png";

			m_mazeTiles.clear();
			m_mazeConnectedEmptySpace.clear();

			m_mazeTiles.resize(height*width);
			m_mazeConnectedEmptySpace.resize(height*width);

			DrawRandomWallTilesStage1();
			//DrawMazeToPng("maze_phase1.png");
			createBorders();
			//DrawMazeToPng("maze_phase2.png");
			freeEncircledSingleWhiteSpaces();
			connectLoneWallTiles();
			//DrawMazeToPng("maze_phase3.png");
			calculateConnectedEmptySpaces();
			connectAllEmptySpace();
			//DrawMazeToPng("maze_phase4.png");
			addRandomWallTilesStage2();
			//DrawMazeToPng("maze_phase5.png");
			calculateStartAndExitLocation();
			//Draw();
			DrawMazeToPng(temp.c_str());
		}

		//createBorders();
	}


	void createBorders()
	{
		std::cout << "Creating maze borders...\n";

		m_mazeTiles[0] = 1;

		for (int i = 1; i < m_mazeTiles.size(); ++i)
		{

			/*if (i < m_width || i >  (m_width * (m_height - 1) - 1))
				m_mazeTiles[i] = 1;

			if (i % m_width == 0)
				m_mazeTiles[i] = 1;

			if (i % m_width == 0)
				if (i > 0)
					m_mazeTiles[i - 1] = 1;
			*/

			if ((i < m_width || i >(m_width * (m_height - 1) - 1)) ||
				(i % m_width == 0))
			{
				m_mazeTiles[i] = 1;
				m_mazeTiles[i - 1] = 1;
			}


		}
		std::cout << "DONE\n";
	}




	int max(int a, int b)
	{
		return (a > b ? a : b);
	}



	int tileCoordinate(int x, int y)
	{
		if ((x < m_width) && (y < m_height))
		{
			//std::cout << "y = " << y << ", x = " << x << "\n";
			//std::cout << "Index: " << (y * m_height + x) << "\n";

			return (y * m_width + x);
		}
		else
		{
			assert(false && "Invalid tile coordinate!");
		}
		
		return 0;

	}



	void DrawWallAtPoint(int x, int y)
	{
		m_mazeTiles[tileCoordinate(x, y)] = 1;
		//std::cout << "drawing wall at (" << x << ", " << y << ")\n";
		DecrementEmptySpace();

		for (int i = -1; i < 1; ++i)
		{
			for (int j = -1; j < 1; ++j)
			{
				if (x + i >= 0 && x + i <= m_width - 2 && y + j >= 0 && y + j <= m_height - 2)
				{


					bool temp = getRandomInteger(0, 1);

					if (!(m_mazeTiles[tileCoordinate(x + i + 1, y + j + 1)]) && (m_mazeTiles[tileCoordinate(x + i, y + j + 1)]) &&
						(m_mazeTiles[tileCoordinate(x + i + 1, y + j)]) && !(m_mazeTiles[tileCoordinate(x + i, y + j)]))
					{

						if (temp)
						{
							//checkValidWallLocation
							if (validWallLocation(x + i + 1, y + j + 1))
							{
								//draw again
								//std::cout << "drawing extra wall\n";
								DrawWallAtPoint(x + i + 1, y + j + 1);

							}
							else if (validWallLocation(x + i, y + j))
							{
								//draw again
								//std::cout << "drawing extra wall\n";
								DrawWallAtPoint(x + i, y + j);

							}


						}
						else
						{

							if (validWallLocation(x + i, y + j))
							{
								//draw again
								//std::cout << "drawing extra wall\n";
								DrawWallAtPoint(x + i, y + j);

							}
							else if (validWallLocation(x + i + 1, y + j + 1))
							{
								//draw again
								//std::cout << "drawing extra wall\n";
								DrawWallAtPoint(x + i + 1, y + j + 1);

							}



						}

						/*
						if (checkValidWallLocation(x + i + 1, y + j + 1))
						{
							//draw again
							std::cout << "drawing extra wall\n";
							DrawWallAtPoint(x + i + 1, y + j + 1);

						}
						else if (checkValidWallLocation(x + i, y + j))
						{
							//draw again
							std::cout << "drawing extra wall\n";
							DrawWallAtPoint(x + i, y + j);

						}*/
						//else
						//{
						//	assert(false && "ERROR: no valid wall locations for wb,bw formation");
						//}


					}
					else if ((m_mazeTiles[tileCoordinate(x + i + 1, y + j + 1)]) && !(m_mazeTiles[tileCoordinate(x + i, y + j + 1)]) &&
						!(m_mazeTiles[tileCoordinate(x + i + 1, y + j)]) && (m_mazeTiles[tileCoordinate(x + i, y + j)]))
					{


						if (temp)
						{

							if (validWallLocation(x + i, y + j + 1))
							{
								//draw again
								//std::cout << "drawing extra wall\n";
								DrawWallAtPoint(x + i, y + j + 1);

							}
							else if (validWallLocation(x + i + 1, y + j))
							{
								//draw again
								//std::cout << "drawing extra wall\n";
								DrawWallAtPoint(x + i + 1, y + j);
							}

						}
						else
						{

							if (validWallLocation(x + i + 1, y + j))
							{
								//draw again
								//std::cout << "drawing extra wall\n";
								DrawWallAtPoint(x + i + 1, y + j);

							}
							else if (validWallLocation(x + i, y + j + 1))
							{
								//draw again
								//std::cout << "drawing extra wall\n";
								DrawWallAtPoint(x + i, y + j + 1);
							}

						}

						/*
						if (checkValidWallLocation(x + i, y + j + 1))
						{
							//draw again
							std::cout << "drawing extra wall\n";
							DrawWallAtPoint(x + i, y + j + 1);

						}
						else if (checkValidWallLocation(x + i + 1, y + j))
						{
							//draw again
							std::cout << "drawing extra wall\n";
							DrawWallAtPoint(x + i + 1, y + j);
						}*/

						//else
						//{
						//	assert(false && "ERROR: no valid wall locations for wb,bw formation");
						//}



					}

				}
			}
		}


	}


	//this function returns true if:
	//all of blocks of 2x2 area are empty (=0)
	bool check2x2EmptyOnly(int x, int y)	//(x, y) is the upper left coordinate of the 2x2 area
	{
		//note that x should be m_width - 1 at max
		//and y should be m_height - 1 at max

		assert(!(x == m_width || y == m_height) && "ERROR: coordinate should be one less than maximum.");

		return (!m_mazeTiles[tileCoordinate(x, y)] && !m_mazeTiles[tileCoordinate(x + 1, y)] &&
				!m_mazeTiles[tileCoordinate(x, y + 1)] && !m_mazeTiles[tileCoordinate(x + 1, y + 1)]);
/*
		if (m_mazeTiles[tileCoordinate(x, y)] == 1)
			++numOfWalls;
		if (m_mazeTiles[tileCoordinate(x + 1, y)] == 1)
			++numOfWalls;
		if (m_mazeTiles[tileCoordinate(x + 1, y + 1)] == 1)
			++numOfWalls;
		if (m_mazeTiles[tileCoordinate(x + 1, y + 1)] == 1)
			++numOfWalls;
	
		return !(numOfWalls == 0 || numOfWalls == 4);
		*/
	}



	bool checkValidWallLocation(int x, int y)
	{

		return ((check2x2LessThan3walls(x - 1, y - 1) && check2x2LessThan3walls(x, y - 1) && check2x2LessThan3walls(x - 1, y) && check2x2LessThan3walls(x, y)) && validWallLocation(x, y));

	}


	bool checkValidWallLocationB(int x, int y)
	{

		for (int i = -1; i < 1; ++i)
		{
			for (int j = -1; j < 1; ++j)
			{
				if (x + i >= 0 && x + i <= m_width - 2 && y + j >= 0 && y + j <= m_height - 2)
				{

					if (!(m_mazeTiles[tileCoordinate(x + i + 1, y + j + 1)]) && (m_mazeTiles[tileCoordinate(x + i, y + j + 1)]) &&
						(m_mazeTiles[tileCoordinate(x + i + 1, y + j)]) && !(m_mazeTiles[tileCoordinate(x + i, y + j)]))
					{

						return false;

					}
					else if ((m_mazeTiles[tileCoordinate(x + i + 1, y + j + 1)]) && !(m_mazeTiles[tileCoordinate(x + i, y + j + 1)]) &&
						!(m_mazeTiles[tileCoordinate(x + i + 1, y + j)]) && (m_mazeTiles[tileCoordinate(x + i, y + j)]))
					{

						return false;
					}

				}
			}
		}


		return ((check2x2LessThan3walls(x - 1, y - 1) && check2x2LessThan3walls(x, y - 1) && check2x2LessThan3walls(x - 1, y) && check2x2LessThan3walls(x, y)));

	}


	bool checkValidWallLocationC(int x, int y)
	{
		int numOfWalls{};

		for (int i = -1; i < 1; ++i)
		{
			for (int j = -1; j < 1; ++j)
			{
				numOfWalls = 0;

				for (int ii = 0; ii < 2; ++ii)
				{

					for (int jj = 0; jj < 2; ++jj)
					{
						
						if (m_mazeTiles[tileCoordinate(x + i + ii, y + j + jj)])
							++numOfWalls;
							


					}
				}

				if (numOfWalls + 1 > 3)
					return false;
			}

		}


		if (m_mazeTiles[tileCoordinate(x - 1, y - 1)] && !m_mazeTiles[tileCoordinate(x - 1, y)] &&
			!m_mazeTiles[tileCoordinate(x, y - 1)])
			return false;
		if (m_mazeTiles[tileCoordinate(x + 1, y - 1)] && !m_mazeTiles[tileCoordinate(x + 1, y)] &&
			!m_mazeTiles[tileCoordinate(x, y - 1)])
			return false;
		if (m_mazeTiles[tileCoordinate(x - 1, y + 1)] && !m_mazeTiles[tileCoordinate(x - 1, y)] &&
			!m_mazeTiles[tileCoordinate(x, y + 1)])
			return false;
		if (m_mazeTiles[tileCoordinate(x + 1, y + 1)] && !m_mazeTiles[tileCoordinate(x + 1, y)] &&
			!m_mazeTiles[tileCoordinate(x, y + 1)])
			return false;


		return true;


	}


	bool check2x2LessThan3walls(int x, int y)
	{

		int temp{};

		for (int i = 0; i < 2; ++i)
		{
			for (int j = 0; j < 2; ++j)
			{
				//std::cout << "Does this fail: " << (x + i) << ", " << (y + j) << "?\n";
				if ((x + i) >= 0 && (x + i) <= (m_width - 2) && (y + j) >= 0 && (y + j) <= (m_height - 2))
				{
					if (m_mazeTiles[tileCoordinate(x + i, y + j)])
						++temp;
				}
				else
				{
					//std::cout << "Ignored x: " << (x + i) << ", y: " << (y + j) << ".\n";
				}
			}

		}


		return (temp < 3);
	}







	//For the first part of adding walls, they must not be added on (x, y) locations where both x and y are (pair + 1)  number.
	bool validWallLocation(int x, int y)
	{
		return !(((x + 1) % 2 == 0) && ((y + 1) % 2 == 0));
	}



	void DrawRandomWallTilesStage1()
	{

		std::cout << "Adding base wall tiles...\n";
		

		for (int i = 0; i < m_width - 1; ++i)
		{

			for (int j = 0; j < m_height - 1; ++j)
			{

				if (check2x2EmptyOnly(i, j))
				{

					bool validLocations[2][2]{};		//[y][x]
					int temp{};


					for (int ii = 0; ii < 2; ++ii)
					{
						for (int jj = 0; jj < 2; ++jj)
						{

							//if (check2x2LessThan3walls(i + ii - 1, j + jj - 1) && check2x2LessThan3walls(i + ii, j + jj - 1) && check2x2LessThan3walls(i + ii - 1, j + jj) && check2x2LessThan3walls(i + ii, j + jj))
							//{
								if (validWallLocation(i + ii, j + jj))
								{
									validLocations[jj][ii] = 1;
									++temp;
								}

							//}



						}

					}

					//std::cout << "validWallLocations: " << temp << "\n";

					if (temp > 0)
					{
						int wallLocation = getRandomInteger(0, temp - 1);
						//std::cout << "Random number: " << wallLocation << "\n";
						temp = 0;

						for (int ii = 0; ii < 2; ++ii)
						{
							for (int jj = 0; jj < 2; ++jj)
							{

								if (validLocations[jj][ii])
								{
									if (wallLocation == temp)
									{
										//std::cout << "x: " << i + ii << ", y = " << j + jj << "\n";
										DrawWallAtPoint(i + ii, j + jj);
										ii = 2;
										jj = 2;
									}
									else
										++temp;
								}

							}
						}

						//Draw();
					}

				}


			}

		}

		std::cout << "DONE\n";

	}



	//this function will open up
	// X X X
	// X   X
	// X X X
	//type of formations
	//by removing either top, bottom, left or right wall tile
	void freeEncircledSingleWhiteSpaces()
	{
		std::cout << "Freeing single empty spaces...\n";

		for (int i = 1; i < m_width - 1; ++i)
		{
			for (int j = 1; j < m_height - 1; ++j)
			{

				if (m_mazeTiles[tileCoordinate(i - 1, j - 1)] && m_mazeTiles[tileCoordinate(i, j - 1)] && m_mazeTiles[tileCoordinate(i + 1, j - 1)] &&
					m_mazeTiles[tileCoordinate(i - 1, j)] && m_mazeTiles[tileCoordinate(i + 1, j)] &&
					m_mazeTiles[tileCoordinate(i - 1, j + 1)] && m_mazeTiles[tileCoordinate(i, j + 1)] && m_mazeTiles[tileCoordinate(i + 1, j + 1)])
				{

					//next, choose which block to remove


					bool tempUpDownOrRightLeft = getRandomInteger(0, 1);
					bool temp = getRandomInteger(0, 1);

					
					if (tempUpDownOrRightLeft) //up down
					{
						if (temp) //up
						{
							if (j - 1 != 0)
							{
								m_mazeTiles[tileCoordinate(i, j - 1)] = 0;
							}
							else
							{
								m_mazeTiles[tileCoordinate(i, j + 1)] = 0;
							}


						}
						else	//down
						{
							if (j + 1 != m_height - 1)
							{
								m_mazeTiles[tileCoordinate(i, j + 1)] = 0;
							}
							else
							{
								m_mazeTiles[tileCoordinate(i, j - 1)] = 0;
							}


						}
					}
					else	// right left
					{
						if (temp) //right
						{

							if (i + 1 != m_width - 1)
							{
								m_mazeTiles[tileCoordinate(i + 1, j)] = 0;
							}
							else
							{
								m_mazeTiles[tileCoordinate(i - 1, j)] = 0;
							}


						}
						else	//left
						{
							if (i - 1 != 0)
							{
								m_mazeTiles[tileCoordinate(i - 1, j)] = 0;
							}
							else
							{
								m_mazeTiles[tileCoordinate(i + 1, j)] = 0;
							}


						}
					}








				}

			}
		}
		std::cout << "DONE\n";
	}


	//this function connects
	// 0 0 0
	// 0 X 0
	// 0 0 0
	//types of formations (where X is wall and 0 is empty space)
	//to walls next to it.
	void connectLoneWallTiles()
	{
		std::cout << "Connecting lone wall tiles...\n";

		for (int i = 2; i < m_width - 2; ++i)
		{
			for (int j = 2; j < m_height - 2; ++j)
			{


				if (!m_mazeTiles[tileCoordinate(i - 1, j - 1)] && !m_mazeTiles[tileCoordinate(i, j - 1)] && !m_mazeTiles[tileCoordinate(i + 1, j - 1)] &&
					!m_mazeTiles[tileCoordinate(i - 1, j)] && !m_mazeTiles[tileCoordinate(i + 1, j)] &&
					!m_mazeTiles[tileCoordinate(i - 1, j + 1)] && !m_mazeTiles[tileCoordinate(i, j + 1)] && !m_mazeTiles[tileCoordinate(i + 1, j + 1)])
				{

					int randomInt = getRandomInteger(0, 3);

					if (randomInt == 0) //up
					{

						m_mazeTiles[tileCoordinate(i, j - 1)] = 1;

						if (m_mazeTiles[tileCoordinate(i, j - 2)] == 0)
							m_mazeTiles[tileCoordinate(i, j - 2)] = 1;

					}
					else if (randomInt == 1) //down
					{

						m_mazeTiles[tileCoordinate(i, j + 1)] = 1;

						if (m_mazeTiles[tileCoordinate(i, j + 2)] == 0)
							m_mazeTiles[tileCoordinate(i, j + 2)] = 1;

					}
					else if (randomInt == 2) //right
					{

						m_mazeTiles[tileCoordinate(i + 1, j)] = 1;

						if (m_mazeTiles[tileCoordinate(i + 2, j)] == 0)
							m_mazeTiles[tileCoordinate(i + 2, j)] = 1;

					}
					else if (randomInt == 3) //left
					{

						m_mazeTiles[tileCoordinate(i - 1, j)] = 1;

						if (m_mazeTiles[tileCoordinate(i - 2, j)] == 0)
							m_mazeTiles[tileCoordinate(i - 2, j)] = 1;

					}
					else
					{
						assert(false && "ERROR! You shouldn't see this lawl");
					}



				}


			}

		}
		std::cout << "DONE\n";
	}


	//this function will calculate which empty spaces are connected to each other.
	//every empty space of the maze should be connected with every other empty space!
	int calculateConnectedEmptySpaces()
	{

		m_numOfIsolatedEmptySpaces = 0;
		m_mazeConnectedEmptySpace.clear();

		m_mazeConnectedEmptySpace.resize(m_height*m_width);

		int target{};
		int newTarget{};

		int newProgress{};
		int currentProgress{};


		std::cout << "Calculating isolated empty space regions...\n";

		for (int j = 1; j < m_height - 1; ++j)
		{
			for (int i = 1; i < m_width - 1; ++i)
			{


				newProgress = static_cast<int>(static_cast<float>(100 * tileCoordinate(i, j) / tileCoordinate(m_width - 1, m_height - 1)));

				if (newProgress > currentProgress)
				{
					std::cout << "Calculating isolated empty space regions... (" << newProgress << "%)\n";
					currentProgress = newProgress;
				}






				if (!m_mazeTiles[tileCoordinate(i, j)])
				{


					if (m_mazeConnectedEmptySpace[tileCoordinate(i - 1, j)] != 0)
					{
						m_mazeConnectedEmptySpace[tileCoordinate(i, j)] = m_mazeConnectedEmptySpace[tileCoordinate(i - 1, j)];

						if (m_mazeConnectedEmptySpace[tileCoordinate(i, j - 1)] != 0 && m_mazeConnectedEmptySpace[tileCoordinate(i, j)] != m_mazeConnectedEmptySpace[tileCoordinate(i, j - 1)])
						{

							//target = 0;
							//newTarget = 0;

							if (m_mazeConnectedEmptySpace[tileCoordinate(i, j)] < m_mazeConnectedEmptySpace[tileCoordinate(i, j - 1)])
							{
								target = m_mazeConnectedEmptySpace[tileCoordinate(i, j - 1)];
								newTarget = m_mazeConnectedEmptySpace[tileCoordinate(i, j)];

							}
							else
							{
								newTarget = m_mazeConnectedEmptySpace[tileCoordinate(i, j - 1)];
								target = m_mazeConnectedEmptySpace[tileCoordinate(i, j)];
							}

							--m_numOfIsolatedEmptySpaces;

							for (int jj = 1; jj < m_height - 1; ++jj)
							{
								for (int ii = 1; ii < m_width - 1; ++ii)
								{

									if (m_mazeConnectedEmptySpace[tileCoordinate(ii, jj)] == target)
										m_mazeConnectedEmptySpace[tileCoordinate(ii, jj)] = newTarget;
									else if (m_mazeConnectedEmptySpace[tileCoordinate(ii, jj)] > target)
										m_mazeConnectedEmptySpace[tileCoordinate(ii, jj)] -= 1;

								}
							}


						}

					}
					else if (m_mazeConnectedEmptySpace[tileCoordinate(i, j - 1)] != 0)
					{

						m_mazeConnectedEmptySpace[tileCoordinate(i, j)] = m_mazeConnectedEmptySpace[tileCoordinate(i, j - 1)];


					}
					else
					{
						++m_numOfIsolatedEmptySpaces;
						m_mazeConnectedEmptySpace[tileCoordinate(i, j)] = m_numOfIsolatedEmptySpaces;
					}

				}


			}
		}
		std::cout << "DONE\n";

		return m_numOfIsolatedEmptySpaces;
	}



	void connectAllEmptySpace()
	{
		std::cout << "Connecting all empty space together...\n";
		//struct Coordinates
		//{
		//	int x;
		//	int y;
		//};

		std::vector<Coordinates> coords;

		std::vector<Coordinates> nearbyCoords{
				{-2, 0},
				{2, 0},
				{0, -2},
				{0, 2}
		};


		for (int k = m_numOfIsolatedEmptySpaces; k > 1; --k)
		{

			for (int i = 1; i < m_width - 1; ++i)
			{
				for (int j = 1; j < m_height - 1; ++j)
				{

					if (m_mazeConnectedEmptySpace[tileCoordinate(i, j)] == k)
					{

						coords.push_back({ i, j });

					}
				}
			}

			//std::mt19937 mersenne2(static_cast<unsigned int>(std::time(nullptr)));
			std::shuffle(std::begin(coords), std::end(coords), mersenne);
			std::shuffle(std::begin(nearbyCoords), std::end(nearbyCoords), mersenne);

			for (int i = 0; i < coords.size(); ++i)
			{
				for (int j = 0; j < nearbyCoords.size(); ++j)
				{
					if (coords[i].x + nearbyCoords[j].x >= 1 && coords[i].x + nearbyCoords[j].x <= m_width - 2 &&
						coords[i].y + nearbyCoords[j].y >= 1 && coords[i].y + nearbyCoords[j].y <= m_height - 2)
					{
						if (m_mazeConnectedEmptySpace[tileCoordinate(coords[i].x + nearbyCoords[j].x, coords[i].y + nearbyCoords[j].y)] < k &&
							m_mazeConnectedEmptySpace[tileCoordinate(coords[i].x + nearbyCoords[j].x, coords[i].y + nearbyCoords[j].y)] > 0)
						{
							int newValue = m_mazeConnectedEmptySpace[tileCoordinate(coords[i].x + nearbyCoords[j].x, coords[i].y + nearbyCoords[j].y)];

							m_mazeConnectedEmptySpace[tileCoordinate(coords[i].x + (nearbyCoords[j].x / 2), coords[i].y + (nearbyCoords[j].y / 2))] = newValue;
							m_mazeTiles[tileCoordinate(coords[i].x + (nearbyCoords[j].x / 2), coords[i].y + (nearbyCoords[j].y / 2))] = 0;

							//std::cout << k << " connected to " << newValue << " at (" << coords[i].x + (nearbyCoords[j].x / 2) << ", " << coords[i].y + (nearbyCoords[j].y / 2) << ")\n";
							//std::cout << "x1: " << coords[i].x << ", y1: " << coords[i].y << "\n";
							//std::cout << "x2: " << (nearbyCoords[j].x / 2) << ", y2: " << (nearbyCoords[j].y / 2) << "\n";

							for (int ii = 0; ii < coords.size(); ++ii)
							{
								m_mazeConnectedEmptySpace[tileCoordinate(coords[ii].x, coords[ii].y)] = newValue;
							}


							coords.clear();
							j = nearbyCoords.size();
							i = coords.size();
							--m_numOfIsolatedEmptySpaces;
						}
					}
				}

			}

			/*
			for (int i = 0; i < nearbyCoords.size(); ++i)
			{
				std::cout << "\n";
				std::cout << k << ": (" << nearbyCoords[i].x << ", " << nearbyCoords[i].y << " )\n";

			}*/

		}





		std::cout << "DONE\n";

	}

	//slow function
	void addRandomWallTilesStage2()
	{

		//struct Coordinates
		//{
		//	int x;
		//	int y;
		//};

		std::vector<Coordinates> coords;

		calculateNumOfEmptyTiles();

		for (int j = 1; j < m_height - 1; ++j)
		{

			for (int i = 1; i < m_width - 1; ++i)
			{
				if (m_mazeTiles[tileCoordinate(i, j)] == 0)
				{
					coords.push_back({ i, j });

				}
			}
		}

		std::shuffle(std::begin(coords), std::end(coords), mersenne);


		if (m_loopFactor > 1)
			m_loopFactor = 1.0f;

		if (m_loopFactor < 0)
			m_loopFactor = 0.0f;

		//int xdTest{};
		//std::cout << "loopFactor: " << m_loopFactor << "\n";
		//std::cout << "coords.size(): " << coords.size() << "\n";
		//std::cout << "coords.size() * loopFactor: " << static_cast<int>(coords.size() * m_loopFactor * m_loopFactor) << "\n";
		//std::cin >> xdTest;

		int currentProgress{};
		int newProgress{};

		std::cout << "Adding random wall blocks...\n";

		for (int i = 0; i < static_cast<int>(coords.size() * m_loopFactor * m_loopFactor); ++i)
		{

			newProgress = static_cast<int>(static_cast<float>(100 * i / static_cast<int>(coords.size() * m_loopFactor * m_loopFactor - 1)));

			if (newProgress > currentProgress)
			{
				std::cout << "Adding random wall blocks... (" <<  newProgress << "%)\n";
				currentProgress = newProgress;
			}

			if (!m_mazeTiles[tileCoordinate(coords[i].x, coords[i].y)])
			{
				if (checkValidWallLocationC(coords[i].x, coords[i].y))
				{

					m_mazeTiles[tileCoordinate(coords[i].x, coords[i].y)] = 1;
					--m_emptySpace;

					//if (calculateConnectedEmptySpaces() > 1)
					if (!checkEmptySpaceIsConnectedCheap())
					{
						m_mazeTiles[tileCoordinate(coords[i].x, coords[i].y)] = 0;
						++m_emptySpace;
					}

				}
			}
			
		}

		//calculateConnectedEmptySpaces();
		std::cout << "DONE\n";

	}



	void calculateStartAndExitLocation()
	{
		std::cout << "Creating Start and Exit location...\n";

		int xStartCurrent{};
		int yStartCurrent{};
		int xEndCurrent{};
		int yEndCurrent{};

		int xStartNew{};
		int yStartNew{};
		int xEndNew{};
		int yEndNew{};

		int longestPathCurrent{};
		int longestPathNew{};

		int numOfDirections{};

		for (int j = 1; j < m_height - 1; ++j)
		{
			xStartNew = 1;
			yStartNew = j;


			if (!m_mazeTiles[tileCoordinate(xStartNew, yStartNew)])
			{
				numOfDirections = 0;
				if (!m_mazeTiles[tileCoordinate(xStartNew - 1, yStartNew)])
					++numOfDirections;
				if (!m_mazeTiles[tileCoordinate(xStartNew + 1, yStartNew)])
					++numOfDirections;
				if (!m_mazeTiles[tileCoordinate(xStartNew, yStartNew - 1)])
					++numOfDirections;
				if (!m_mazeTiles[tileCoordinate(xStartNew, yStartNew + 1)])
					++numOfDirections;

				if (numOfDirections == 1)
				{

					calculateLongestPath(xStartNew, yStartNew, xEndNew, yEndNew, longestPathNew);


					if (longestPathNew > longestPathCurrent)
					{
						longestPathCurrent = longestPathNew;
						xStartCurrent = xStartNew;
						yStartCurrent = yStartNew;
						xEndCurrent = xEndNew;
						yEndCurrent = yEndNew;

					}

					
				}

			}

			
		}

		for (int i = 2; i < m_width - 2; ++i)
		{
			xStartNew = i;
			yStartNew = 1;

			if (!m_mazeTiles[tileCoordinate(xStartNew, yStartNew)])
			{

				numOfDirections = 0;
				if (!m_mazeTiles[tileCoordinate(xStartNew - 1, yStartNew)])
					++numOfDirections;
				if (!m_mazeTiles[tileCoordinate(xStartNew + 1, yStartNew)])
					++numOfDirections;
				if (!m_mazeTiles[tileCoordinate(xStartNew, yStartNew - 1)])
					++numOfDirections;
				if (!m_mazeTiles[tileCoordinate(xStartNew, yStartNew + 1)])
					++numOfDirections;

				if (numOfDirections == 1)
				{

					calculateLongestPath(xStartNew, yStartNew, xEndNew, yEndNew, longestPathNew);

					if (longestPathNew > longestPathCurrent)
					{
						longestPathCurrent = longestPathNew;
						xStartCurrent = xStartNew;
						yStartCurrent = yStartNew;
						xEndCurrent = xEndNew;
						yEndCurrent = yEndNew;

					}
					
				}
			}

			
		}

		//std::cout << "DONE\n";
		//std::cout << "Start: (" << xStartCurrent << ", " << yStartCurrent << ")\n";
		//std::cout << "End: (" << xEndCurrent << ", " << yEndCurrent << ")\n";
		//std::cout << "Distance = " << longestPathCurrent << "\n";

		if (xStartCurrent == 1)
		{
			m_mazeTiles[tileCoordinate(0, yStartCurrent)] = 0;
		}
		else if (xStartCurrent == m_width - 2)
		{
			m_mazeTiles[tileCoordinate(m_width - 1, yStartCurrent)] = 0;
		}
		else if (yStartCurrent == 1)
		{
			m_mazeTiles[tileCoordinate(xStartCurrent, 0)] = 0;
		}
		else if (yStartCurrent == m_height - 2)
		{
			m_mazeTiles[tileCoordinate(xStartCurrent, m_height - 1)] = 0;
		}

		if (xEndCurrent == 1)
		{
			m_mazeTiles[tileCoordinate(0, yEndCurrent)] = 0;
		}
		else if (xEndCurrent == m_width - 2)
		{
			m_mazeTiles[tileCoordinate(m_width - 1, yEndCurrent)] = 0;
		}
		else if (yEndCurrent == 1)
		{
			m_mazeTiles[tileCoordinate(xEndCurrent, 0)] = 0;
		}
		else if (yEndCurrent == m_height - 2)
		{
			m_mazeTiles[tileCoordinate(xEndCurrent, m_height - 1)] = 0;
		}

		std::cout << "DONE\n";

	}


	void calculateNumOfEmptyTiles()
	{
		m_emptySpace = 0;

		for (int i = 1; i < m_width - 1; ++i)
		{
			for (int j = 1; j < m_height - 1; ++j)
			{
				if (!m_mazeTiles[tileCoordinate(i, j)])
					++m_emptySpace;
			}
		}
	}


	bool checkEmptySpaceIsConnectedCheap()
	{

		std::vector<Coordinates> coords1{};
		std::vector<Coordinates> coords2{};
		std::vector<int>distanceMap;

		int distance{};
		int numOfEmptySpace{};

		Coordinates start{ 1, 1 };
		Coordinates end{};

		distanceMap.resize(m_width*m_height);
		distanceMap[tileCoordinate(1, 1)] = distance;

		coords1.reserve(100);
		coords2.reserve(100);

		coords1.push_back(start);

		do
		{

			++distance;

			for (int i = 0; i < coords1.size(); ++i)
			{

				if (!m_mazeTiles[tileCoordinate(coords1[i].x, coords1[i].y - 1)] && distanceMap[tileCoordinate(coords1[i].x, coords1[i].y - 1)] == 0)
				{
					++numOfEmptySpace;
					coords2.push_back({ coords1[i].x, coords1[i].y - 1 });
					distanceMap[tileCoordinate(coords1[i].x, coords1[i].y - 1)] = distance;

				}

				if (!m_mazeTiles[tileCoordinate(coords1[i].x, coords1[i].y + 1)] && distanceMap[tileCoordinate(coords1[i].x, coords1[i].y + 1)] == 0)
				{
					++numOfEmptySpace;
					coords2.push_back({ coords1[i].x, coords1[i].y + 1 });
					distanceMap[tileCoordinate(coords1[i].x, coords1[i].y + 1)] = distance;

				}

				if (!m_mazeTiles[tileCoordinate(coords1[i].x - 1, coords1[i].y)] && distanceMap[tileCoordinate(coords1[i].x - 1, coords1[i].y)] == 0)
				{
					++numOfEmptySpace;
					coords2.push_back({ coords1[i].x - 1, coords1[i].y });
					distanceMap[tileCoordinate(coords1[i].x - 1, coords1[i].y)] = distance;

				}

				if (!m_mazeTiles[tileCoordinate(coords1[i].x + 1, coords1[i].y)] && distanceMap[tileCoordinate(coords1[i].x + 1, coords1[i].y)] == 0)
				{
					++numOfEmptySpace;
					coords2.push_back({ coords1[i].x + 1, coords1[i].y });
					distanceMap[tileCoordinate(coords1[i].x + 1, coords1[i].y)] = distance;

				}



			}

			if (coords2.size() == 0)
				end = { coords1[0].x, coords1[0].y };

			coords1.swap(coords2);
			coords2.clear();

		} while (coords1.size() > 0);


		return (numOfEmptySpace == m_emptySpace);

	}



	void calculateLongestPath(int x, int y, int &xOut, int &yOut, int &valOut)	//x, y is starting location
	{

		std::vector<Coordinates> coords1{};
		std::vector<Coordinates> coords2{};
		std::vector<int>distanceMap;

		int distance{};

		Coordinates start{x, y};
		Coordinates end{};

		distanceMap.resize(m_width*m_height);
		distanceMap[tileCoordinate(x, y)] = distance;

		coords1.reserve(100);
		coords2.reserve(100);
		
		coords1.push_back(start);

		do
		{

			++distance;

			for (int i = 0; i < coords1.size(); ++i)
			{

				if (!m_mazeTiles[tileCoordinate(coords1[i].x, coords1[i].y - 1)] && distanceMap[tileCoordinate(coords1[i].x, coords1[i].y - 1)] == 0)
				{

					coords2.push_back({ coords1[i].x, coords1[i].y - 1 });
					distanceMap[tileCoordinate(coords1[i].x, coords1[i].y - 1)] = distance;

				}

				if (!m_mazeTiles[tileCoordinate(coords1[i].x, coords1[i].y + 1)] && distanceMap[tileCoordinate(coords1[i].x, coords1[i].y + 1)] == 0)
				{

					coords2.push_back({ coords1[i].x, coords1[i].y + 1 });
					distanceMap[tileCoordinate(coords1[i].x, coords1[i].y + 1)] = distance;

				}

				if (!m_mazeTiles[tileCoordinate(coords1[i].x - 1, coords1[i].y)] && distanceMap[tileCoordinate(coords1[i].x - 1, coords1[i].y)] == 0)
				{

					coords2.push_back({ coords1[i].x - 1, coords1[i].y });
					distanceMap[tileCoordinate(coords1[i].x - 1, coords1[i].y)] = distance;

				}

				if (!m_mazeTiles[tileCoordinate(coords1[i].x + 1, coords1[i].y)] && distanceMap[tileCoordinate(coords1[i].x + 1, coords1[i].y)] == 0)
				{

					coords2.push_back({ coords1[i].x + 1, coords1[i].y });
					distanceMap[tileCoordinate(coords1[i].x + 1, coords1[i].y)] = distance;

				}



			}

			if (coords2.size() == 0)
				end = { coords1[0].x, coords1[0].y };

			coords1.swap(coords2);
			coords2.clear();

		} while (coords1.size() > 0);

		
		//std::cout << "The longest path from start (" << x << ", " << y << ") is to (" << end.x << ", " << end.y << ") (length = " << distance << ")\n";

		/*
		std::vector<unsigned char> image;
		image.resize(m_width * m_height * 4);

		for (int j = 0; j < m_height; ++j)
		{
			for (int i = 0; i < m_width; ++i)
			{

				image[4 * m_width * j + 4 * i + 0] = static_cast<bool>(distanceMap[tileCoordinate(i, j)])*64 + distanceMap[tileCoordinate(i, j)];
				image[4 * m_width * j + 4 * i + 1] = 0;
				image[4 * m_width * j + 4 * i + 2] = 0;
				image[4 * m_width * j + 4 * i + 3] = 255;

			}

		}


		encodeOneStep("maze_debug.png", image, m_width, m_height);
		*/
		
		Coordinates longestDistanceLocation{};
		int longestDistance{};
		int numOfDirections{};

		for (int j = 1; j < m_height - 1; ++j)
		{

			if (!m_mazeTiles[tileCoordinate(1, j)])
			{

				numOfDirections = 0;
				if (!m_mazeTiles[tileCoordinate(1 - 1, j)])
					++numOfDirections;
				if (!m_mazeTiles[tileCoordinate(1 + 1, j)])
					++numOfDirections;
				if (!m_mazeTiles[tileCoordinate(1, j - 1)])
					++numOfDirections;
				if (!m_mazeTiles[tileCoordinate(1, j + 1)])
					++numOfDirections;

				if (numOfDirections == 1)
				{
					if (distanceMap[tileCoordinate(1, j)] > longestDistance)
					{
						longestDistanceLocation = { 1, j };
						longestDistance = distanceMap[tileCoordinate(1, j)];
					}
				}
			}

			if (!m_mazeTiles[tileCoordinate(m_width - 2, j)])
			{

				numOfDirections = 0;
				if (!m_mazeTiles[tileCoordinate(m_width - 2 - 1, j)])
					++numOfDirections;
				if (!m_mazeTiles[tileCoordinate(m_width - 2 + 1, j)])
					++numOfDirections;
				if (!m_mazeTiles[tileCoordinate(m_width - 2, j - 1)])
					++numOfDirections;
				if (!m_mazeTiles[tileCoordinate(m_width - 2, j + 1)])
					++numOfDirections;

				if (numOfDirections == 1)
				{

					if (distanceMap[tileCoordinate(m_width - 2, j)] > longestDistance)
					{
						longestDistanceLocation = { m_width - 2, j };
						longestDistance = distanceMap[tileCoordinate(m_width - 2, j)];
					}

				}
			}
		}

		for (int i = 2; i < m_width - 2; ++i)
		{
			if (!m_mazeTiles[tileCoordinate(i, 1)])
			{

				numOfDirections = 0;
				if (!m_mazeTiles[tileCoordinate(i - 1, 1)])
					++numOfDirections;
				if (!m_mazeTiles[tileCoordinate(i + 1, 1)])
					++numOfDirections;
				if (!m_mazeTiles[tileCoordinate(i, 1 - 1)])
					++numOfDirections;
				if (!m_mazeTiles[tileCoordinate(i, 1 + 1)])
					++numOfDirections;

				if (numOfDirections == 1)
				{

					if (distanceMap[tileCoordinate(i, 1)] > longestDistance)
					{
						longestDistanceLocation = { i, 1 };
						longestDistance = distanceMap[tileCoordinate(i, 1)];
					}

				}

			}

			if (!m_mazeTiles[tileCoordinate(i, m_height - 2)])
			{

				numOfDirections = 0;
				if (!m_mazeTiles[tileCoordinate(i - 1, m_height - 2)])
					++numOfDirections;
				if (!m_mazeTiles[tileCoordinate(i + 1, m_height - 2)])
					++numOfDirections;
				if (!m_mazeTiles[tileCoordinate(i, m_height - 2 - 1)])
					++numOfDirections;
				if (!m_mazeTiles[tileCoordinate(i, m_height - 2 + 1)])
					++numOfDirections;

				if (numOfDirections == 1)
				{

					if (distanceMap[tileCoordinate(i, m_height - 2)] > longestDistance)
					{
						longestDistanceLocation = { i, m_height - 2 };
						longestDistance = distanceMap[tileCoordinate(i, m_height - 2)];
					}

				}

			}

		}

		//std::cout << "Longest distance edge tile is (" << longestDistanceLocation.x << ", " << longestDistanceLocation.y << ")\n";

		//std::cout << "(" << x << ", " << y << ") -> (" << longestDistanceLocation.x << ", " << longestDistanceLocation.y << "), D = " << longestDistance << "\n";

		xOut = longestDistanceLocation.x;
		yOut = longestDistanceLocation.y;
		valOut = longestDistance;

	}



	void DrawMazeToPng(const char* name = "maze.png")
	{

		std::vector<unsigned char> image;
		image.resize(m_width * m_height * 4);

		for (int j = 0; j < m_height; ++j)
		{
			for (int i = 0; i < m_width; ++i)
			{

				image[4 * m_width * j + 4 * i + 0] = 255 * !m_mazeTiles[tileCoordinate(i, j)];
				image[4 * m_width * j + 4 * i + 1] = 255 * !m_mazeTiles[tileCoordinate(i, j)];
				image[4 * m_width * j + 4 * i + 2] = 255 * !m_mazeTiles[tileCoordinate(i, j)];
				image[4 * m_width * j + 4 * i + 3] = 255;

			}

		}


		encodeOneStep(name, image, m_width, m_height);

	}


	void Draw()
	{
		for (int i = 0; i < m_mazeTiles.size(); ++i)
		{
			if (i % m_width == 0)
				std::cout << "\n";

			if (m_mazeTiles[i] == 0)
				std::cout << " ";					//change to " " when not debugging
			else
				std::cout << "X";

		}
	}


	void DrawDebugConnectedWhiteSpace()
	{
		for (int i = 0; i < m_mazeConnectedEmptySpace.size(); ++i)
		{
			if (i % m_width == 0)
				std::cout << "\n";

			if (m_mazeConnectedEmptySpace[i] == 0)
				std::cout << " ";					//change to " " when not debugging
			else
				std::cout << m_mazeConnectedEmptySpace[i];

		}


	}


	~Maze()
	{
	}


};



void encodeOneStep(const char* filename, std::vector<unsigned char>& image, unsigned width, unsigned height)
{
	//Encode the image
	unsigned error = lodepng::encode(filename, image, width, height);

	//if there's an error, display it
	if (error) std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;
}



int main()
{

	//intToString(0);

	/*
	unsigned int imageWidth = 512;
	unsigned int imageHeight = 512;
	std::vector<unsigned char> image;
	image.resize(imageWidth * imageHeight * 4);

	for (unsigned y = 0; y < imageHeight; y++)
		for (unsigned x = 0; x < imageWidth; x++)
		{
			image[4 * imageWidth * y + 4 * x + 0] = 255 * !(x & y);
			image[4 * imageWidth * y + 4 * x + 1] = x ^ y;
			image[4 * imageWidth * y + 4 * x + 2] = x | y;
			image[4 * imageWidth * y + 4 * x + 3] = 255;
		}

	encodeOneStep("testi.png", image, imageWidth, imageHeight);

	*/


	/*
	std::cout << !((8 % 2 == 0) && (10 % 2 == 0)) << "\n";



	int testArray[5*5]{};

	for (int i = 0; i < 5 * 5; ++i)
	{
		if (i % 5 == 0)
			std::cout << "\n";

		std::cout << testArray[i] << " ";
	}*/




	//max width: 119
	//max height: 29

	int width{};
	int height{};
	int quantity{};
	float loopFactor{};

	do
	{
		std::cout << "Enter width of the maze (odd number >= 9): ";
		std::cin >> width;
	} while ((width <= 8) || (width % 2 == 0));

	do
	{
		std::cout << "Enter height of the maze (odd number >= 9): ";
		std::cin >> height;
	} while ((height <= 8) || (height % 2 == 0));

	do
	{
		std::cout << "Enter loop factor (decimal number from 0 to 1. 1 = no loops, 0 = lots of loops): ";
		std::cin >> loopFactor;

	} while ((loopFactor < 0.000f) && (loopFactor > 1.000f));

	do
	{
		std::cout << "Enter how many mazes you want to generate (min: 1, max: 10): ";
		std::cin >> quantity;
	} while ((quantity <= 0) || (quantity > 10));


	Maze maze(width, height, loopFactor, quantity);

	std::cout << "Your maze(s) were saves as PNG files in the executable's folder\n";

	//maze.DrawAtPoint(9, 9);
	/*
	maze.Draw();

	maze.DrawRandomWallTilesStage1();
	maze.Draw();
	maze.createBorders();
	maze.freeEncircledSingleWhiteSpaces();
	maze.Draw();
	maze.connectLoneWallTiles();
	maze.Draw();
	maze.calculateConnectedEmptySpaces();
	maze.DrawDebugConnectedWhiteSpace();
	maze.connectAllEmptySpace();
	maze.DrawDebugConnectedWhiteSpace();
	maze.Draw();
	maze.calculateConnectedEmptySpaces();
	maze.DrawDebugConnectedWhiteSpace();
	maze.Draw();
	maze.addRandomWallTilesStage2();
	maze.Draw();
	maze.DrawDebugConnectedWhiteSpace();
	//maze.calculateLongestPath(1, 1);
	maze.calculateStartAndExitLocation();
	maze.Draw();
	maze.DrawMazeToPng();*/
	int test{};
	std::cin >> test;






}