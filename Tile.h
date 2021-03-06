#ifndef TILE_H
#define TILE_H

#include "Global.h"

struct SDL_Texture; //foward declare
class Graphics; //foward declare

class Tile {
public:
	Tile();
	Tile(SDL_Texture* tileset, Vector2 size, Vector2 tilesetPosition, Vector2 position);
	void update(int elapsedTime);
	void draw(Graphics &graphics);
protected:
	SDL_Texture* _tileset;
	Vector2 _size; //size of tile
	Vector2 _tilesetPosition; //position on the tileset that the tile is in
	Vector2 _position; //position in the map the tile is
};


#endif