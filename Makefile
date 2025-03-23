all:
	g++ -I src/include -L src/lib -o dino dino.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image

