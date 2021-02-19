#include "TextManager.h"
#include "Graphics.h"
#include "sprite.h"
#include <iostream>
#include "Global.h"
#include <string>


using namespace std;

namespace textConstants {
	bool activeText = false;
	int count = 0;
}

TextManager::TextManager() {

}

TextManager::TextManager(Graphics & graphics, Player & player)  {
	int posx = player.getX();
	int posy = player.getY();
	this->_chatBox = Sprite(graphics, "npcTextBox.png", 0, 0, 243, 23, posx, posy);
	//	this->_currentHealthBar = Sprite(graphics, "Textbox.png", 0, 25, 39, 5, 83, 72);
	cout << "chatBox added" << endl;
	graphics.loadImage("npcTextBox.png"); //loads sprite sheet in
	cout << "chatBox loaded" << endl;

	this->drawChatBox(graphics, player);

}


TextManager::TextManager(Graphics &graphics, const std::string &filePath, int sourceX, int sourceY, int width, int height,
	float posX, float posY) :
	//intializer
	_textX(posX),
	_textY(posY)

{	//set our sourceRectangle and spriteSheet
	this->_textSourceRect.x = sourceX;
	this->_textSourceRect.y = sourceY;
	this->_textSourceRect.w = width;
	this->_textSourceRect.h = height;

	cout << "_sourceRect w/h = " << _textSourceRect.w << " , " << _textSourceRect.h << endl;

	this->_textSheet = SDL_CreateTextureFromSurface(graphics.getRenderer(), graphics.loadImage(filePath)); //take a render and surface will give us surface if not load then give us
	if (this->_textSheet == NULL) { //if spritesheet is null that means there was an error while loading image
		printf("\nError: Unable to load image \n");
	}

	this->_TboundingBox = Rectangle(this->_textX, this->_textY, width * globals::SPRITE_SCALE, height * globals::SPRITE_SCALE);
	cout << "_sourceRect w/h = " << _textSourceRect.w << " , " << _textSourceRect.h << endl;
	cout << "_boundingBox w/h = " << width << " , " << height << endl;
}

void TextManager::setTextStatus(bool status) {
	textConstants::activeText = status;
}

bool TextManager::isActiveText() {
	if (textConstants::activeText == true) {
		return true;
	}

	if (textConstants::activeText == false) {
		return false;
	}

}

void TextManager::drawChatBox(Graphics & graphics, Player & player) {
	if (textConstants::activeText == true) {
		this->_chatBox.drawChat(graphics, player.getX(), player.getY());
	}
	
}

void TextManager::drawTest(Graphics &graphics) {
	SDL_Rect destinationRectangle = { 490, 502, 0 * globals::SPRITE_SCALE, 0 * globals::SPRITE_SCALE }; //where on screen we will be drawing
																																	//does drawing- Needs spriteSheet, pointer to sourceRectangle and pointer to destination which we just made;

	SDL_Surface *surface;
	SDL_Color color = { 255, 255, 255, 255 };

	surface = TTF_RenderText_Solid(font, "test text", color);
	SDL_Rect src = { 0, 0, 100, 100 };
	SDL_Rect dest = { 200, 200, 100, 100 };
	SDL_Texture *tex = SDL_CreateTextureFromSurface(graphics.getRenderer(), surface);
	//graphics.blitSurface(tex, NULL, &dest);

	graphics.blitSurface(tex, NULL, &destinationRectangle);
}

std::string TextManager::renderTextbox(Graphics &graphics, const std::string &str, int posX, int posY, SDL_Color color) {
	unsigned int charsInLine = (100);

	int lineNum = 0;

	std::string line, word, excess;
	line.clear();
	word.clear();
	excess.clear();
	for (const char c : str) {
		if (!excess.empty()) {
			excess += c;
		}
		else if (c == ' ' || c == '\n') {
			if ((line.length() + word.length()) < charsInLine) {
				line += ' '; //line with word added is within length allowed
			}
			else {
				excess += renderTextboxLine(graphics, color, line, word, lineNum, posX, posY);
				line += word;
				word.clear();
			}
		}//if word completed
		else {
			word += c;
		}
		if (c == '\n') {
			excess += renderTextboxLine(graphics, color, line, word, lineNum, posX, posY);
		}
	}
	if (excess.empty())
	{
		if ((line.length() + word.length()) < charsInLine)
			line = line + ' ' + word;
		else
			excess += word;
		renderTextboxLine(graphics, color, line, word, lineNum, posX, posY);
	}
	return excess;
}

std::string TextManager::renderTextboxLine(Graphics &graphics, SDL_Color color, std::string &line, std::string &word, int &lineNum, int posX, int posY) {
	if (TTF_Init() == -1) {
		printf("TTF_Init: %s\n", TTF_GetError());
		exit(2);
	}
	SDL_Texture* text;
	int lineH = TTF_FontLineSkip(font);
	int boxH = 100;  // magic number: textbox 1/5 of screen tall.
	string excess;
	excess.clear();

	while (!line.empty() && line[0] == ' ')
		line.erase(line.begin());
	if (line.empty())
		return "";
	//text = renderText(ren, font, line, color);
	SDL_Surface *surface;
	surface = TTF_RenderText_Solid(font, line.c_str(), color);
	SDL_Texture *tex = SDL_CreateTextureFromSurface(graphics.getRenderer(), surface);
	SDL_Rect destinationRectangle = { posX - 260 , posY + 85, surface->w, surface->h }; //where on screen we will be drawing
	SDL_FreeSurface(surface);
	graphics.blitSurface(tex, NULL, &destinationRectangle);
	
	//renderTexture(text, ren, 10, (SCREEN_HEIGHT - boxH + lineNum * lineH + 10));
	// magic number 10 for top margin of text box.
	SDL_DestroyTexture(tex);
	line.clear();
	lineNum++;
	if (boxH < (lineNum * lineH + 20))  // margin of 10 at top and bottom.
		excess += word += ' ';
	return excess;
}  // string renderTextboxLine(

void TextManager::drawNpcText(Graphics &graphics, int x, int y, const std::string &str, int posX, int posY, SDL_Color color) { 
	//TTF_Init();
	if (TTF_Init() == -1) {
		printf("TTF_Init: %s\n", TTF_GetError());
		exit(2);
	} 
	
	TTF_Font *font = TTF_OpenFont("Arcadia.ttf", 18);
	SDL_Surface *surface;

	surface = TTF_RenderText_Solid(font, str.c_str(), color);
	SDL_Rect destinationRectangle = { posX - 260 , posY + 105, surface->w, surface->h }; //where on screen we will be drawing
	SDL_Texture *tex = SDL_CreateTextureFromSurface(graphics.getRenderer(), surface);
	//graphics.blitSurface(tex, NULL, &dest);

	graphics.blitSurface(tex, NULL, &destinationRectangle);
	SDL_FreeSurface(surface); //fixes crashing for access violation in loop
	TTF_CloseFont(font);
	SDL_DestroyTexture(tex);
}

void TextManager::drawDmg(Graphics & graphics, int x, int y, double dmg)
{
	if (TTF_Init() == -1) {
		printf("TTF_Init: %s\n", TTF_GetError());
		exit(2);
	}
	SDL_Surface *surface;
	SDL_Color color = { 255, 0, 0, 255 };
	int precisionVal = 2;
	//std::string dmgNum = std::to_string(dmg);
	std::string dmgNum = std::to_string(dmg).substr(0, std::to_string(dmg).find(".") + precisionVal + 1);
	TTF_Font *iFont = TTF_OpenFont("Arcadia.ttf", 18);
	surface = TTF_RenderText_Solid(iFont, dmgNum.c_str(), color);

	SDL_Rect destinationRectangle = { x, y - 30, surface->w, surface->h }; //where on screen we will be drawing
	SDL_Texture *tex = SDL_CreateTextureFromSurface(graphics.getRenderer(), surface);
	//cout << "drawDMG called!" << endl;
	//cout << "destRect: " << destinationRectangle.x << "," << destinationRectangle.y << "," << destinationRectangle.w << "," << destinationRectangle.h << endl;
	graphics.blitSurface(tex, NULL, &destinationRectangle);
	SDL_FreeSurface(surface); //fixes crashing for access violation in loop
	TTF_CloseFont(font);
	SDL_DestroyTexture(tex);
}

void TextManager::drawItemQuantity(Graphics &graphics, int x, int y, const std::string &str, int posX, int posY, SDL_Color color) {
	//TTF_Init();
	if (TTF_Init() == -1) {
		printf("TTF_Init: %s\n", TTF_GetError());
		exit(2);
	}

	SDL_Surface *surface;
	TTF_Font *iFont = TTF_OpenFont("Arcadia.ttf", 12);
	surface = TTF_RenderText_Solid(iFont, str.c_str(), color);
	SDL_Rect destinationRectangle = { posX, posY, surface->w, surface->h }; //where on screen we will be drawing
	SDL_Texture *tex = SDL_CreateTextureFromSurface(graphics.getRenderer(), surface);
	//graphics.blitSurface(tex, NULL, &dest);

	graphics.blitSurface(tex, NULL, &destinationRectangle);
	SDL_FreeSurface(surface); //fixes crashing for access violation in loop
	TTF_CloseFont(font);
	SDL_DestroyTexture(tex);
}

void TextManager::setTextXY(Player &player) {
	this->_chatRect.x = player.getX();
	this->_chatRect.y = player.getY();
}


/*
void TextManager::update() {
	this->_boundingBox = Rectangle(this->_x, this->_y, this->_sourceRect.w * globals::SPRITE_SCALE, this->_sourceRect.h * globals::SPRITE_SCALE);
}*/


const Rectangle TextManager::getTextBoundingBox() const {
	return
		this->_TboundingBox;
}

//Side getCollisionSide
//Determine which side the collision happened on
const sides::Side TextManager::getTextBoxCollisionSide(Rectangle &other) const {
	int amtRight, amtLeft, amtTop, amtBottom; //lowest amount that its in (overlapping) is the side we are colliding on 
	amtRight = this->getTextBoundingBox().getRight() - other.getLeft();
	amtLeft = other.getRight() - this->getTextBoundingBox().getLeft();
	amtTop = other.getBottom() - this->getTextBoundingBox().getTop();
	amtBottom = this->getTextBoundingBox().getBottom() - other.getTop();
	//create array of absoulte value of these and loop through array to figure out which one is lowest by overrriding the lowest variable
	//If next one in array is lower than the one thats current lowest replace that one with the lowest.
	int vals[4] = { abs(amtRight), abs(amtLeft), abs(amtTop), abs(amtBottom) };
	int lowest = vals[0];
	for (int i = 0; i < 4; i++) {
		if (vals[i] < lowest) {
			lowest = vals[i];
		}
	}
	return
		lowest == abs(amtRight) ? sides::RIGHT : //if lowest is right return right..
		lowest == abs(amtLeft) ? sides::LEFT :
		lowest == abs(amtTop) ? sides::TOP :
		lowest == abs(amtBottom) ? sides::BOTTOM :
		sides::NONE;
}

void TextManager::setTextSourceRectX(int value) {
	this->_textSourceRect.x = value;
}

void TextManager::setTextSourceRectY(int value) {
	this->_textSourceRect.y = value;
}

void TextManager::setTextSourceRectW(int value) {
	this->_textSourceRect.w = value;
}

void TextManager::setTextSourceRectH(int value) {
	this->_textSourceRect.h = value;
}