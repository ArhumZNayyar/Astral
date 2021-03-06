#include "Enemy.h"
#include <iostream>
#include "SDL_timer.h"
#include "TextManager.h"
#include "Player.h"

TextManager txt;
//Base enemy class
Enemy::Enemy() {}

Enemy::~Enemy()
{
}

namespace enemyConstants {
	//bool removeEnemy = false;
}

Enemy::Enemy(Graphics &graphics, std::string filePath, int sourceX, int sourceY, int width, int height, Vector2 spawnPoint, int timeToUpdate) :
	AnimatedSprite(graphics, filePath, sourceX, sourceY, width, height, spawnPoint.x, spawnPoint.y, timeToUpdate),
	_direction(LEFT)
	//_maxHealth(3),
	//_currentHealth(3)
{}

void Enemy::update(int elapsedTime, Player &player) {
	AnimatedSprite::update(elapsedTime);
}

void Enemy::handleEnemyTileCollision(std::vector<Rectangle>& others)
{
}

void Enemy::bulletHit(float dmg) {
	this->_currentHealth -= dmg;
	std::cout << "hit! HP = " << this->_currentHealth << std::endl;
}

std::string Enemy::getName()
{
	return this->name;
}

void Enemy::playDeath() {
	if (deathCreated == false) {

	}
}

void Enemy::draw(Graphics &graphics) {
	AnimatedSprite::draw(graphics, this->_x, this->_y);
}

bool Enemy::isRemoveable() {
	return removeEnemy;
}

void Enemy::setRemoveable() {
	if (removeEnemy == true) {
		removeEnemy = false;
	}
}

void Enemy::dropLoot(Player & player)
{
	this->canDropLoot = true;
}

float Enemy::enemyExpAmount() {
	return 0;
}

//Bat class
Bat::Bat() {}

Bat::Bat(Graphics &graphics, Vector2 spawnPoint) :
	Enemy(graphics, "data\\enemy\\NpcCemet.png", 32, 32, 16, 16, spawnPoint, 140),
	_startingX(spawnPoint.x),
	_startingY(spawnPoint.y),
	_shouldMoveUp(false),
	_maxHealth(5),
	_currentHealth(5)
{
	this->setupAnimations();
	this->playAnimation("FlyLeft");
	this->_fireX = this->_startingX;
	this->_fireY = this->_startingY;
	this->_HPBar._x = this->_startingX;
	this->_HPBar._y = this->_startingY;
	this->_fireBall = Sprite(graphics, "data\\enemy\\NpcCemet.png", 220, 33, 13, 10, this->_fireX, (this->_fireY + 10));
	this->_HPBar = Sprite(graphics, "data\\enemy\\NpcCemet.png", 2, 157, 17, 7, this->_HPBar._x, this->_HPBar._y - 15);
	this->_HPValue = Sprite(graphics, "data\\enemy\\NpcCemet.png", 3, 167, 17, 5, this->_HPBar._x + 1, this->_HPBar._y + 2);
}

Bat::~Bat()
{
	this->destroySprite();
	this->_fireBall.destroySprite();
	this->_HPBar.destroySprite();
	this->_HPValue.destroySprite();
}

void Bat::update(int elapsedTime, Player &player) {
	this->_fireBall.update();
	this->_HPBar.update();
	this->_HPValue.update();
	this->_direction = player.getX() > this->_x ? RIGHT : LEFT;
	if (this->getCurrentHealth() > 0 && this->isVisible == true) {
		this->playAnimation(this->_direction == RIGHT ? "FlyRight" : "FlyLeft");
		//Move up or down
		this->_y += this->_shouldMoveUp ? -.06 : .06;
		this->_HPBar._y += this->_shouldMoveUp ? -.06 : .06;
		this->_HPValue._y += this->_shouldMoveUp ? -.06 : .06;
		float hpNum = (float)this->getCurrentHealth() / this->getMaxHealth();
		this->_HPValue.setSourceRectW(std::floor(hpNum * 17));

		if (this->_y > (this->_startingY + 30) || this->_y < this->_startingY - 30) {
			this->_shouldMoveUp = !this->_shouldMoveUp;
		}
		if (this->getBoundingBox().collidesWith(player.getBoundingBox())) {
			player.gainHealth(-4.5);
		}
		if (_fireBall.getBoundingBox().collidesWith(player.getBoundingBox())) {
			this->_fireBall.setX(this->_x);
			this->_fireBall.setY(this->_y);
			player.gainHealth(-8.5);
		}
		else if (_fireBall.getY() > this->_y + 160) {
			this->_fireBall.setX(this->_x);
			this->_fireBall.setY(this->_y);
		}
		else {
			this->_fireBall.addX(player.getX() > this->_x ? .06 : -.06);
			this->_fireBall.addY(player.getY() > this->_y ? .06 : -.06);
		}
	}
	else if (this->getCurrentHealth() <= 0) {
		_deathTimeElapsed += elapsedTime;
		if (isVisible == false)
			timerRespawn += elapsedTime;
		if (_deathTimeElapsed > _deathAnimationTime && isVisible == true && dyingAnimation == true) {
			this->dyingAnimation = false;
			this->_deathTimeElapsed = 0;
			this->isVisible = false;
		}
		else if (_deathTimeElapsed < _deathAnimationTime && isVisible == true) {
			this->dyingAnimation = true;
			this->playAnimation("BatDie");
		}
		if (timerRespawn > respawnTime && isVisible == false && dyingAnimation == false) {
			timerRespawn = 0;
			this->isVisible = true;
			this->_currentHealth = this->_maxHealth;
			this->removeEnemy = false;
			this->canDropLoot = false;
			_deathTimeElapsed = 0;
		}
	}

	Enemy::update(elapsedTime, player);
}


void Bat::draw(Graphics &graphics) {
	if (isVisible == true) {
		Enemy::draw(graphics);
		this->_fireBall.draw(graphics, this->_fireBall.getX(), this->_fireBall.getY());
		if (this->getCurrentHealth() < this->getMaxHealth()) {
			this->_HPBar.draw(graphics, this->_HPBar._x, this->_HPBar._y);
			this->_HPValue.draw(graphics, this->_HPValue._x, this->_HPValue._y);
		}

	}
}

void Bat::animationDone(std::string currentAnimation) {
	if (this->getCurrentHealth() <= 0) {
		if (this->canDropLoot == false) {
			this->removeEnemy = true;
			this->canDropLoot = true;
		}
	}
}

void Bat::dropLoot(Player &player) {
	
}

void Bat::setupAnimations() {
	this->addAnimation(3, 2, 32, "FlyLeft", 16, 16, Vector2(0, 0));
	this->addAnimation(3, 2, 48, "FlyRight", 16, 16, Vector2(0, 0));
	this->addAnimation(7, 6, 32, "BatDie", 16, 16, Vector2(0, 0));
	this->addAnimation(1, 220, 33, "FireBall", 13, 10, Vector2(0, 0));
}

void Bat::playDeath() {
	this->playAnimation("BatDie", true);
}

void Bat::playAttack() {
	
}

void Bat::touchPlayer(Player* player) {
		player->gainHealth(-1.0f);
}

void Bat::bulletHit(float dmg) {
	if (this->_currentHealth >= 1)
		this->_currentHealth -= dmg;
}

void Bat::setRemoveable() {
	if (removeEnemy == true) {
		removeEnemy = false;
	}
}

void Bat::handleEnemyTileCollision(std::vector<Rectangle>& others)
{
}

bool Bat::isRemoveable() {
	return removeEnemy;
}

float Bat::enemyExpAmount() {
	return Bat::batExp;
}

std::string Bat::getName()
{
	return this->name;
}


//Shade class
Shade::Shade() {}

Shade::~Shade()
{
	this->destroySprite();
	this->_shadeBall.destroySprite();
	this->_HPBar.destroySprite();
	this->_HPValue.destroySprite();
}

Shade::Shade(Graphics &graphics, Vector2 spawnPoint) :
	Enemy(graphics, "data\\enemy\\shade.png", 27, 7, 32, 32, spawnPoint, 140),
	_startingX(spawnPoint.x),
	_startingY(spawnPoint.y),
	_shouldMoveUp(false),
	_currentHealth(100),
	_maxHealth(100)
{
	this->setupAnimations();
	this->playAnimation("shadeRight");

	this->_shadeBall = Sprite(graphics, "data\\enemy\\shadeAttack.png", 0, 0, 22, 19, this->_startingX, (this->_startingY - 5));
	this->_HPBar = Sprite(graphics, "data\\enemy\\NpcCemet.png", 2, 157, 17, 7, this->_startingX + 56, this->_startingY - 15);
	this->_HPValue = Sprite(graphics, "data\\enemy\\NpcCemet.png", 3, 174, 17, 5, this->_startingX + 57, this->_startingY - 12);
	graphics.loadImage("data\\enemy\\shadeAttack.png");
}

void Shade::update(int elapsedTime, Player &player) {
	if (this->_dy <= this->GRAVITY_CAP) {
		//dy is change in y over this frame Delta Y if dy is less than or equal to gravity cap then we need to increase cuz we are not at the cap
		this->_dy += this->GRAVITY * elapsedTime;
	}
	this->_y += this->_dy * elapsedTime; //Gravity move them by Y
	//this->_HPBar._y += this->_dy * elapsedTime;
	//this->_HPValue._y += this->_dy * elapsedTime;
	this->_shadeBall.update();
	this->_HPBar.update();
	this->_HPValue.update();
	this->_direction = player.getX() > this->_x ? RIGHT : LEFT;
	if (this->getCurrentHealth() > 0 && this->isVisible == true) {
		if ((this->_direction == RIGHT && player.getX() > this->_x + 650) || (this->_direction == LEFT &&
			player.getX() <= this->_x - 650)) {
			this->playAnimation("shadeIdle");
			isIdle = true;
		}
		else {
			isIdle = false;
			this->playAnimation(this->_direction == RIGHT ? "shadeRight" : "shadeLeft");

			float hpNum = (float)this->getCurrentHealth() / this->getMaxHealth();
			this->_HPValue.setSourceRectW(std::floor(hpNum * 17));

			if (this->_direction == RIGHT) {
				this->_x += 0.07f;
				if (this->_boundingBox.collidesWith(player.getBoundingBox()))
					this->_x -= 0.07f;
				this->_HPBar._x += 0.07f;
				this->_HPValue._x += 0.07f;
				this->_HPBar._y = this->_y - 65;
				this->_HPValue._y = this->_y - 62;
			}
			else {
				this->_x -= 0.07f;
				if (this->_boundingBox.collidesWith(player.getBoundingBox()))
					this->_x += 0.07f;
				this->_HPBar._x -= 0.07f;
				this->_HPValue._x -= 0.07f;
				this->_HPBar._y = this->_y - 65;
				this->_HPValue._y = this->_y - 62;
			}
			if (this->getBoundingBox().collidesWith(player.getBoundingBox())) {
				player.gainHealth(-12.64f);
			}
			if (_shadeBall.getBoundingBox().collidesWith(player.getBoundingBox())) {
				this->_shadeBall.setX(this->_x);
				this->_shadeBall.setY(this->_y - 5);
				player.gainHealth(-26.69f);
			}
			else if (_shadeBall.getX() > this->_x + 350) {
				this->_shadeBall.setX(this->_x);
				this->_shadeBall.setY(this->_y + 5);
			}
			else {
				this->_shadeBall.addX(player.getX() > this->_x ? .8 : -.8);
			}
		}
	}

	else if (this->getCurrentHealth() <= 0) {
		_deathTimeElapsed += elapsedTime;
		if (isVisible == false)
			timerRespawn += elapsedTime;
		if (_deathTimeElapsed > _deathAnimationTime && isVisible == true && dyingAnimation == true) {
			this->dyingAnimation = false;
			this->_deathTimeElapsed = 0;
			this->isVisible = false;
		}
		else if (_deathTimeElapsed < _deathAnimationTime && isVisible == true) {
			this->dyingAnimation = true;
			this->playAnimation("shadeDie");
		}
		if (timerRespawn > respawnTime && isVisible == false && dyingAnimation == false) {
			timerRespawn = 0;
			_deathTimeElapsed = 0;
		}
	}

	AnimatedSprite::updateBoss(elapsedTime, this->_y);
}

void Shade::bulletHit(float dmg) {
	if (this->_currentHealth >= 1)
		this->_currentHealth -= dmg;
}

void Shade::draw(Graphics &graphics) {
	if (isVisible == true) {
	AnimatedSprite::drawBoss(graphics, this->_x, this->_y);
	if (isIdle == false)
		this->_shadeBall.draw(graphics, this->_shadeBall.getX(), this->_shadeBall.getY());
	if (this->getCurrentHealth() < this->getMaxHealth()) {
		this->_HPBar.draw(graphics, this->_HPBar._x, this->_HPBar._y);
		this->_HPValue.draw(graphics, this->_HPValue._x, this->_HPValue._y);
		}
	}
}

void Shade::animationDone(std::string currentAnimation) {
	if (this->getCurrentHealth() <= 0) {
		if (this->canDropLoot == false) {
			this->removeEnemy = true;
			this->canDropLoot = true;
		}
	}
}

void Shade::dropLoot(Player &player) {

}

void Shade::setupAnimations() {
	this->addSpecialAnimation(3, 0, 16, "shadeIdle", 29, 39, Vector2(0, -60));
	this->addAnimation(4, 1, 73, "shadeLeft", 30, 28, Vector2(0, -60));
	this->addAnimation(4, 1, 118, "shadeRight", 30, 28, Vector2(0, -60));
	this->addAnimation(6, 1, 168, "shadeDie", 30, 28, Vector2(0, -60));
}

void Shade::playDeath() {
	this->playAnimation("shadeDie", true);
}

void Shade::playAttack() {

}

void Shade::touchPlayer(Player* player) {
	player->gainHealth(-1.0f);
}

void Shade::handleEnemyTileCollision(std::vector<Rectangle>& others)
{
	for (int i = 0; i < others.size(); i++) {
		sides::Side collisionSide = Sprite::getCollisionSide(others.at(i));
		if (collisionSide != sides::NONE) {
			switch (collisionSide) {
			case sides::TOP:
				this->_dy = 0; //reset all gravity, if we arent grounded we fall to the ground
				this->_y = others.at(i).getBottom() + 1; //no longer go through things, stops us
				//this->_HPBar._y = others.at(i).getTop() + 1;
				//this->_HPValue._y = others.at(i).getTop() + 1;
				if (this->_grounded) { //only time we hit a top tile is if we are on a slope, (we are grounded on a slope)
					//this->_dx = 0; //stop movement on x-axis
					this->_x -= this->_direction == RIGHT ? 0.5f : -0.5f; //if we face right, subtract .5 from x pos otherwise subtract -.5 (adds .5)
				}
				break;
			case sides::BOTTOM: //hit the top (bottom) of tile push us back up ontop of tile
				this->_y = others.at(i).getTop() - this->_boundingBox.getHeight() - 1;
				//this->_HPBar._y = others.at(i).getTop() - this->_boundingBox.getHeight() - 1;
				//this->_HPValue._y = others.at(i).getTop() - this->_boundingBox.getHeight() - 1;
				this->_dy = 0;
				this->_grounded = true; //we are on ground since it pushed it back up
				break;
			case sides::LEFT:
				this->_x = others.at(i).getRight() + 1;
				break;
			case sides::RIGHT:
				this->_x = others.at(i).getLeft() - this->_boundingBox.getWidth() - 1;
				break;
			}
		}
	}
}

bool Shade::isRemoveable() {
	return removeEnemy;
}

void Shade::setRemoveable() {
	if (removeEnemy == true) {
		removeEnemy = false;
	}
}

float Shade::enemyExpAmount() {
	return Shade::shadeExp;
}

std::string Shade::getName()
{
	return this->name;
}
